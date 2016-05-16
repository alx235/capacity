#include <thread>
#include <mutex>
#include <exception>
#include <memory>
#include <iostream>
#include <deque>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <functional>

struct empty_pop: std::exception
{
	const char* what() const throw();
};

#ifndef my_stack_h
	#define my_stack_h

void f1()
{
	std::cout<<"f1\n";
}

void f2(int i,std::string j)
{
	std::cout<<"f2\n";
}

namespace my_th_safe_structs{

template<typename T,typename TSeq=std::deque<T>>
class TSStack
{
	private:
		TSeq _data;
		//make sure that _mutex dead after all threads finish (e.g.: earlier decl than local thread pool object)
		std::mutex _mutex;
	public:
		//don't need any dtor, it's only wrapper
		//omit another ctors for simplity
		explicit TSStack(const TSeq& data):_data(data){};
		explicit TSStack(const TSeq&& data=TSeq()):_data(data){};//default
		//set link to private date _data
		//omit another operators overloaded		
		template<typename T1,typename TSeq1>
		friend bool operator==(const TSStack<T1,TSeq1> &st1,
								const TSStack<T1,TSeq1> &st2);

		void push(T &&new_val)
		{
			std::lock_guard<std::mutex> lk(_mutex);
			_data.push_back(new_val);
			std::cout<<"&&"<<"\n";
		}

		void push(const T &new_val)
		{
			std::lock_guard<std::mutex> lk(_mutex);
			_data.push_back(new_val);
			std::cout<<"&"<<"\n";
		}
	
		std::shared_ptr<T> pop()
		{

			//following behavior can exist:
			//allocate memory for res
			//allocate memory for temporary make_shared
			//std::move cast to xvalue
			//move ctor for temporary make_shared (now lost data if well designed)
			//copy ctor for res (now lost data if well designed)
			//(*)std::shared_ptr<T> res(std::make_shared<T>(std::move(data_queue.front())));

			std::lock_guard<std::mutex> lk(_mutex);
			if (_data.empty())
				throw empty_pop();
			//don't know why code below(*) better...if std::shared_ptr<T>(rvalue) well designed(not set null to source if throw)
			//if data_queue<unique_ptr<T>>
			//as i understand move ctor only copy pointer address, so didn't find any dangerous unless (null source if throw)
			std::shared_ptr<T> res(std::move(_data.front()));

			_data.pop();
			return res;//implicit move ctor(prvalue)
		}

		void pop(T &value)
		{
			std::lock_guard<std::mutex> lk(_mutex);
			if (_data.empty())
				throw empty_pop();
			value=std::move(_data.front());//efficienty if T has move ctor, otherwise use copy ctor			
			_data.pop_back();
		}

		bool empty()
		{
			std::lock_guard<std::mutex> lk(_mutex);
			return _data.empty();
		}
};

template<typename T,typename TSeq=std::deque<T>>
class TSQueue
{
	private:
		TSeq _data;
		//make sure that ~_mutex after all threads finish (earlier decl than local thread pool)
		std::mutex _mutex;
		std::condition_variable _cond_var;
		std::atomic<int> done;
	public:
		//don't need any dtor, it's only wrapper
		//omit another ctors for simplity
		explicit TSQueue(const TSeq& data):_data(data)
		{
			done.store(0,std::memory_order_release);
		}
		explicit TSQueue(const TSeq&& data=TSeq()):_data(data)//default
		{
			done.store(0,std::memory_order_release);
		}
		//i don't want set atomic counter of thread's enter and incr/decr in ctor/dtor of related obj
		//in begin of each f() and wait until zero counter and set return if done=1 before
		//SO, this must be called before/when ~ThreadPool 		
		void clear_before_dtor() noexcept
		{
			done.store(1,std::memory_order_release);//probably low contention because of max 1 concurent thread 
			_cond_var.notify_all();//(**)+guarantee no thread's wait before dtor
		}
		bool empty()
		{
			std::unique_lock<std::mutex> lk(_mutex);
			return _data.empty();
		}
		void push(T &&new_val)
		{
			std::unique_lock<std::mutex> lk(_mutex);
			_data.push_back(new_val);//become visible by mutex lock/unlock sync-with
			_cond_var.notify_one();
			std::cout<<"&&"<<"\n";
		}
		void push(const T &new_val)
		{
			std::unique_lock<std::mutex> lk(_mutex);
			_data.push_back(new_val);//become visible by mutex lock/unlock sync-with
			_cond_var.notify_one();
			std::cout<<"&"<<"\n";
		}
		void pop(T &value)
		{
			std::unique_lock<std::mutex> lk(_mutex);
			//probably low contention because of max 1 concurent thread
			//(**)+guarantee no thread's wait before dtor
			while(_data.empty()&&(!done.load(std::memory_order_acquire)))
				_cond_var.wait(lk);
			if (done.load(std::memory_order_acquire))
				return;
			value=std::move(_data.front());//efficienty if T has move ctor, otherwise copy-ctor ll'call			
			_data.pop_front();
		}
		std::shared_ptr<T> pop()
		{
			std::unique_lock<std::mutex> lk(_mutex);
			while(_data.empty()&&(!done.load(std::memory_order_acquire)))
				_cond_var.wait(lk);
			if (done.load(std::memory_order_acquire))
				return;
			std::shared_ptr<T> res(std::move(_data.front()));
			_data.pop_front();
			return res;//implicit move ctor
		}
	private:
};
		template<typename T1,typename TSeq1>
		inline bool operator==(const TSStack<T1,TSeq1> &st1,
								const TSStack<T1,TSeq1> &st2)
		{
			return st1._data==st2._data;
		}

//simple threadpoll
class ThreadPool
{
	private:
		std::atomic<bool> done;
		my_th_safe_structs::TSQueue<std::function<void()> > work_queue;
		std::vector<std::thread> _pool;
		
		const int _TH_Max;
		
		void dowork()
		{
			try
			{
				std::function<void()> task;
				while(!done.load(std::memory_order_acquire))
				{
					std::cout<<"start wait task\n";
					work_queue.pop(task);//avoid hight contention for done by wait
					if(task)
					{
						std::cout<<"start do task\n";
						task();
					}
				}
			}
			catch(const std::exception &exc)
			{
				std::cerr<<exc.what();
				std::cout<<"\nthrow with id:"<<std::this_thread::get_id()<<"\n";
			}
			catch(...)
			{
				std::cout<<"throw with id:"<<std::this_thread::get_id()<<"\n";
			}
			std::cout<<"finish work\n";
		}
	public:
		
		ThreadPool():_TH_Max(std::thread::hardware_concurrency())
		{
			std::cout<<"thread max:"<<_TH_Max<<"\n";
			done.store(false,std::memory_order_release);
			_pool.reserve(_TH_Max);
			for (int i=0;i<_TH_Max;++i)
			{
				std::cout<<"thread\n";
				std::thread thrd(&my_th_safe_structs::ThreadPool::dowork,this);
				try
				{
					_pool.push_back(std::move(thrd));
					std::cout<<"add thread complete\n";
				}
				catch(...)
				{
					std::cout<<"THREADPOOL throw with id:"<<std::this_thread::get_id()<<"\n";
					if (thrd.joinable())
						thrd.join();
					throw 1111;
				}
			}	
		}
		
		~ThreadPool()
		{
			std::cout<<"~THREADPOOL\n";
			if(!done.load(std::memory_order_acquire))
				stop();
		}
		
		ThreadPool(const ThreadPool &x)=delete;
		ThreadPool& operator=(const ThreadPool &x)=delete;

		void add_task(std::function<void()> task)
		{
			work_queue.push(task);
		}

		void stop()
		{
			done.store(true,std::memory_order_release);
			work_queue.clear_before_dtor();
			for (int i=0;i<_pool.size();++i)
				if(_pool[i].joinable())
					_pool[i].join();
		}
};

}

#endif

/*
//std::function fun
void print_num(int i)
{
    std::cout << i << '\n';
}
void print_num2(int i,int j)
{
    std::cout << i+j << '\n';
}
void print_num3(float i,int j,int k)
{
    std::cout << i+j+k << '\n';
}

template<typename FTYPE>
void call_ftype_wrap(FTYPE f)
{
	f();
}*/

int main()
{	
	/*
	my_th_safe_structs::TSStack<int> a,b;
	int x=0;
	a.push(1);
	a.push(x);
	a.pop(x);
	a.pop(x);
	std::cout<<(a==b)<<"\n";
	*/
	/*
	//std::function fun
	std::function<void()> f_display_31337 = std::bind(print_num, 31337);
	std::function<void()> f_display_31337_2 = std::bind(print_num2, 31337,1);
	std::function<void()> f_display_31337_3 = std::bind(print_num3, static_cast<float>(31337),1,2);
	f_display_31337();
	f_display_31337_2();
	f_display_31337_3();
	
	call_ftype_wrap(std::bind(print_num, 31337));
	call_ftype_wrap(std::bind(print_num2, 31337,1));
	call_ftype_wrap(std::bind(print_num3, static_cast<float>(31337),1,2));
	std::function<void()> ar_fwrap[3];

	ar_fwrap[0]=std::bind(print_num, 31337);
	ar_fwrap[1]=std::bind(print_num2, 31337,1);
	ar_fwrap[2]=std::bind(print_num3, static_cast<float>(31337),1,2);
	ar_fwrap[0]();
	ar_fwrap[1]();
	ar_fwrap[2]();

	my_th_safe_structs::TSQueue<std::function<void()> > work_queue;
	work_queue.push(std::bind(print_num, 31337));
	work_queue.push(std::bind(print_num2, 31337,1));
	work_queue.push(std::bind(print_num3, static_cast<float>(31337),1,2));
	
	std::function<void()> task;
	work_queue.pop(task);
	task();
	work_queue.pop(task);
	task();
	work_queue.pop(task);
	task();
	*/
	my_th_safe_structs::ThreadPool thrpool;
	thrpool.add_task(f1);
	thrpool.add_task(std::bind(f2,1,"abc"));
	std::this_thread::sleep_for(std::chrono::seconds(5));
	thrpool.stop();
	std::cout<<"program finish"<<"\n";
	return 0;
}
