#include <thread>
#include <mutex>
#include <exception>
#include <memory>
#include <iostream>
#include <deque>
#include <atomic>
#include <condition_variable>

struct empty_pop: std::exception
{
	const char* what() const throw();
};

#ifndef my_stack_h
	#define my_stack_h

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
			//don't know why code below(*) better...if std::shared_ptr<T>(rvalue) well designed(not null source if throw)
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
		explicit TSQueue(const TSeq& data):_data(data){};
		explicit TSQueue(const TSeq&& data=TSeq()):_data(data){};//default
		//i don't want set atomic counter of thread's enter and incr/decr in ctor/dtor of related obj
		//in begin of each f() and wait until zero counter and set return if done=1 before
		//SO, this must be called before/when ~ThreadPool 		
		void clear_before_dtor()
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
			value=std::move(_data.front());//efficienty if T has move ctor, otherwise copy-ctor ll'call			
			_data.pop_back();
		}
		std::shared_ptr<T> pop()
		{
			std::unique_lock<std::mutex> lk(_mutex);
			while(_data.empty()&&(!done.load(std::memory_order_acquire)))
				_cond_var.wait(lk);
			std::shared_ptr<T> res(std::move(_data.front()));
			_data.pop();
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

}
#endif

int main()
{	
	/*my_th_safe_structs::TSStack<int> a,b;
	int x=0;
	a.push(1);
	a.push(x);
	a.pop(x);
	a.pop(x);
	std::cout<<(a==b)<<"\n";*/
	std::cout<<"program finish"<<"\n";
	return 0; 
}
