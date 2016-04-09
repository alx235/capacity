//g++ ex.cpp -pthread -std=c++11
//#include <algorithm>
#include <atomic>
#include <iostream>
#include <iterator>
#include <time.h>
#include <memory>
#include <thread>
#include <pthread.h>
#include <time.h>
#include <future>

#define cpu_relax_flag
#ifdef cpu_relax_flag
	#define cpu_relax()	asm volatile("pause" ::: "memory")
#endif

//SPINLOCK MUTEX
//busy waiting + cpu_relax() for reduce CPU consumption
//use for short-time wait
//OR just use pthread library spinlock impl
class SpinLock
{
	std::atomic_flag locked = ATOMIC_FLAG_INIT;//default state is cleared(false)
	public:
		//one thread set lock, obtain value (maybe false), release itself, lock next threads
		//std::atomic_flag - gurantee lock-free (in most case), clear/test_and_set - atomic operation,
		//so another threads cannot interrupe that operation while executed by one
		//offcourse, execution's order is unknown
		void lock()
		{
		    while (locked.test_and_set(std::memory_order_acquire))
			{
			#ifdef cpu_relax_flag
		 	cpu_relax();
			#endif
			}
		}
		void unlock()
		{
			locked.clear(std::memory_order_release);
		}
};

//DEADLOCK EXAMPLE
/*
//pthread_mutex_trylock() usefull to prevent DEADLOCK, lock if can, return false if already locked
//use lock/unlock in same order to avoid DEADLOCK
//if 2 threads have dependency on lock use same mutex to avoid case when one release and another
//start to wait infinitely
void *function1()
{
   ...
   pthread_mutex_lock(&lock1);           // Execution step 1
   pthread_mutex_lock(&lock2);           // Execution step 3 DEADLOCK!!!
   ...
   ...
   pthread_mutex_lock(&lock2);
   pthread_mutex_lock(&lock1);
   ...
} 

void *function2()
{
   ...
   pthread_mutex_lock(&lock2);           // Execution step 2
   pthread_mutex_lock(&lock1);
   ...
   ...
   pthread_mutex_lock(&lock1);
   pthread_mutex_lock(&lock2);
   ...
} */

//Thread-safe Singleton example(guarantee C++11)
/*class Singleton_ex {
	std::atomic<Singleton_ex*> instance;
	std::mutex m_lock;

	public:
		Singleton_ex{}
		static &Singleton_ex get()
		{
			static Singleton_ex instance;//local static init once, another thread wait until init finish
			return instance;
			//or you can use DCLP (Double-Checked Locking pattern)

		}
	
		//from http://preshing.com/20130930/double-checked-locking-is-fixed-in-cpp11/
		//there are different with:
		//Singleton* tmp = m_instance.load(std::memory_order_relaxed);
		//std::atomic_thread_fence(std::memory_order_acquire);
		//std::atomic_thread_fence(std::memory_order_release);
		//m_instance.store(tmp, std::memory_order_relaxed);
		//...but it compile equal code (for now!)
		//!SYNC_WITH work in acquire calling after release
		Singleton_ex* get_DCLP()
		{	// prevent memory reordering next line -> tmp non-atomic
			//non-atomic pointer is faster, see value before release
			//next conditions true until init finish
			Sigleton_ex* tmp = instance.load(std::memory_order_acquire);//(1) sync-with by acquire-release
			if (tmp = nullptr) //better init before with nullptr
			{
				std::lock_guard<std::mutex> lock(m_lock);
				//no sync-with,so current and next line are equal to if(instance = nullptr)
				tmp = instance.load(std::memory_order_relaxed);
				if (tmp = nullptr)//check again, we can just compare instance to nullprt (not lvalue)
				{
					tmp = new Singleton_ex();
					instance.store(tmp,std::memory_order_release);//(2) sync-with by acquire-release
				}
			return tmp;
		}		
};*/

//pthread_cond_t is "signal and wait"
//wake up waiting thread by signal,waiting thread must wait until some condition become true (producer/consumer)
//waiting(pthread_cond_wait) case must be rare
//usefull in producer/consumer problem and semaphore, only one non-abstract example i have founded is bounded queue.
//semaphore - limit count of threads have access(by wait(limit--)) to share resource, if limit reached (0) next will 	"wait" until one of accessed finish(post(limit++)), ++ current number and send "wake up" signal to one "waiting"...
//not found any "not study" example of semaphore...
//"waiting" implemented (in POSIX) by futex with WAIT flag and null timeout (futex is syscall) - thread "sleep"
//until futex with WAKE flag call.
//advantage is use unlock from not owned thread!
//efficiency of pthread_cond_wait lesser mutex (POSIX)
//Performance highly depend on platform/CPU instruction
//...OR just use C++11 timed_mutex)

class Binaphore {//or pthread_mutex_timedlock but without "illegal" unlock
	pthread_mutex_t  mutex;
	pthread_cond_t cond;
	std::atomic_flag locked,is_clear;
	//int timeout;
	public:
		Binaphore(/*int timeout_*/):locked(ATOMIC_FLAG_INIT),is_clear(ATOMIC_FLAG_INIT)
		{
			//is_clear = ATOMIC_FLAG_INIT;
			mutex = PTHREAD_MUTEX_INITIALIZER;
			cond = PTHREAD_COND_INITIALIZER;
			//locked = ATOMIC_FLAG_INIT;//default state is cleared(false)
			set default timeout, otherwise wait return immediately
			/*if (!timeout_)
				timeout+=2;*///
		}
		
		//omit memory order because this 2 call rare and possibly from one thread
		void set_not_clear()
		{
			is_clear.clear();
		}
		void clear_res()
		{
			if (!is_clear.test_and_set())
			{
				pthread_cond_destroy(&cond);
				pthread_mutex_destroy(&mutex);	
			}
		}

		void lock()
		{
			pthread_mutex_lock(&mutex);//(1*)guarantee satisfy wait req of mutex lock and be same, otherwise - UB
			/*struct timespec timeToWait;
			clock_gettime(CLOCK_REALTIME, &timeToWait);
			timeToWait.tv_sec += timeout;*/
			int ret = 0;//default succefull f() value
			//first time flag is zero, read (0) mod (1) write(1), next thread will blocked
			//if timeout: no mutex lock, er<0
			//if wait "lie": locked back it to wait again
			while (locked.test_and_set(std::memory_order_acquire) && (!ret))
				//atomic: release mutex when calling wait and lock cond
				//after succesfull (rc=0) return wait, released thread lock mutex, inside wait
				//ret = pthread_cond_timedwait(&cond, &mutex, &timeToWait);
				ret = pthread_cond_wait(&cond, &mutex);
			if ((ret<0) /*&& (ret!=ETIMEDOUT)*/)
				std::cout<<"lock failled err:"<<ret<<" \n";
			#ifdef debug_Timoutlck
			std::cout<<"ret:"<<ret<<"\n";
			#endif
			pthread_mutex_unlock(&mutex);
		}

		void unlock()
		{	
			//use same mutex to avoid parallel lock and unlock, because there is a dependency on lock 
			//in wait/signal it can probability that signal be earlier than wait, and wait thread can be wait
			//infinitely
			pthread_mutex_lock(&mutex);
			locked.clear(std::memory_order_release);//set locked to false
			pthread_cond_signal(&cond);
			pthread_mutex_unlock(&mutex);
		}
		
		~Binaphore()
		{
			clear_res();
		}
};

//PTHREAD_MUTEX_ERRORCHECK: recursive lock form one thread cause error
//PTHREAD_MUTEX_RECURSIVE: ...not cause, counter++, release if counter=0, has counter limit
//...pthread_mutex_trylock() counter++, return true, false if reach limit or ...
//PTHREAD_MUTEX_DEFAULT: ...cause UB
//PTHREAD_MUTEX_NORMAL: ...cause deadlock

//pthread_mutex_unlock() fail if thread doesn't own mutex, UB - for DEFAULT and NORMAL
//pthread_mutex_lock() fail if thread has lower priority


//std::thread is just class-wrapper, but comfortable)
//...but pthread more flexiable and fast
//pthread create with default joinable attr
//...(can be joined only ONCE, by only thread WITHOUT data races, if not - UB)
//failed to join joinable thread produce Zombie!
//join return control to thread call join, detached - immediately, returning value (void*) return to second join arg
//joinable thread MUST be join to avoid resource leak!
//threads need it's own try/catch
//to cancel tread just use return(always, never return local var address)
//if main thread exit() - UB

//(1)
//c++11 thread MUSN'T be joinable(not been detached or joined) before call ~thread()
//...or also can be moved(non-copyable,non-assignable), default-constructed
//thread terminate normal or by thow exception (cancel is difficult)
//to use native pthread.h function call native_handle() for fast performance, but with carefull!
//return value by std::promise and std::future (or just send pointer as agr to avoid infinite future_wait)
//c++11: thread that has finished is still active thread of execution and joinable

//std::recursive_mutex use try-lock to avoid system error (max counter overflow)
//std::mutex recursive lock from owned thread - UB
//...std::unlock from not-owned thread - UB, not throw,
//......prior to lock SYNC_WITH
//...std::try_lock, no-throw
//......prior unlock,lock SYNC_WITH if true!
//......spuriously fail!
//...std::lock, throw!
//above true for both mutex and recursive_mutex

//std::lock_guard - scope lock

//std::unique_lock????!!!!

//(1*)
//it seems std::condition_variable equal pthread impl with except that it can throw 
//std::condition_variable for std::unique_lock (for efficienty)
//std::condition_variable_any with any lock...

//std::once_flag/std::call_once - to call function once!

void thread_f1(/*std::promise<int> prom_*/)
{
	std::cout<<"run with id:"<<std::this_thread::get_id()<< "\n";
	//prom_.set_value(1);//notify future
}

int main()
{	
	//(1)
	//std::promise<int> prom_;
	//std::future<int> prom_future = prom_.get_future();
	std::thread t1(thread_f1);
	//prom.future_wait();//wait until set_value can cause infinite wait, if there is exception in t1 thread
	//std::cout<<"recieved value:"<<prom_future.get()<<"\n";
	t1.join();//wait until execution end then release resources
	std::cout<<"program finish"<<"\n";
	return 0;
}
