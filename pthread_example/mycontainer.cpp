#include <thread>
#include <mutex>
#include <exception>
#include <memory>
#include <iostream>
#include <deque>

struct empty_pop: std::exception
{
	const char* what() const throw();
};

#ifndef my_stack_h
	#define my_stack_h

namespace my_th_safe_stack{

template<typename T,typename TSeq=std::deque<T>>
class thr_safe_stack
{
	private:
		TSeq _data;
		//make sure that _mutex dead after all threads finish (earlier decl than local thread pool object or move
		//..._mutex to global scope
		std::mutex _mutex;
	public:
		//don't need any dtor, it's only wrapper
		//omit another ctors for simplity
		explicit thr_safe_stack(const TSeq& data):_data(data){};
		explicit thr_safe_stack(const TSeq&& data=TSeq()):_data(data){};//default
		//set link to private date _data
		//omit another operators overloaded		
		template<typename T1,typename TSeq1>
		friend bool operator==(const thr_safe_stack<T1,TSeq1> &st1,
								const thr_safe_stack<T1,TSeq1> &st2);

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
			std::lock_guard<std::mutex> lk(_mutex);
			
			//following behavior can exist:
			//allocate memory for res
			//allocate memory for temporary make_shared
			//std::move cast to xvalue
			//move ctor for temporary make_shared (now lost data if well designed)
			//copy ctor for res (now lost data if well designed)
			//std::shared_ptr<T> res(std::make_shared<T>(std::move(data_queue.front())));
			
			//don't know why code below better...
			//if data_queue<unique_ptr<T>>
			if (_data.empty())
				throw empty_pop();
				
			std::shared_ptr<T> res(std::move(_data.front()));

			_data.pop();
			return res;//implicit move ctor
		}

		void pop(T &value)
		{
			value=std::move(_data.front());//efficienty if T has move ctor, otherwise use copy ctor			
			_data.pop_back();
		}

		bool empty()
		{
			std::lock_guard<std::mutex> lk(_mutex);
			return _data.empty();
		}
};
		template<typename T1,typename TSeq1>
		inline bool operator==(const thr_safe_stack<T1,TSeq1> &st1,
								const thr_safe_stack<T1,TSeq1> &st2)
		{
			return st1._data==st2._data;
		}

}
#endif

int main()
{	
	/*my_th_safe_stack::thr_safe_stack<int> a,b;
	int x=0;
	a.push(1);
	a.push(x);
	a.pop(x);
	a.pop(x);
	std::cout<<(a==b)<<"\n";*/
	std::cout<<"program finish"<<"\n";
	return 0;
}
