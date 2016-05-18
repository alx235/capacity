//<THREAD MARKS
//1) Launch thread give overhead: OS allocate kernel res, stack space and add thread to scheduler
//.. =>many threads run once slow down system performance.
//2) Context swith OS(kernel) Can reduce performance (oversubscription),flush cache and make full memory barrier
//..(OS+CPU specific)
//3) Use high-level threads abstraction slower than use low-level.
//6) Cache lines have 32-64 (ofthen) bit size, if data lower this size it can cause false sharing
//..threads modificate multiple item in one cache-line this can produce large cache data or invalidate queue (CPU specific)
//..(cache alighning avoid this)
//7) Sync-With CAN slow performance by reodering access from different core
////7.1) High Contention core wait each other very ofthen (ONE MUTEX FOR MANY THREADS)
////7.2) Low Contention rarely waiting situation (chance deadlock grow)
//8) Effect by mutex contention DIFFERENT than atomic instructions sync (low time futex wait don't call kernel - POSIX)
//...but kernel sleep,shedule slow sync and reduce high contention
//9) Cache mis - chunk of data haven't store in cache, must be loaded to cache which can replace another thread's data cache
//10) data proximity: data accessed by single thread is spread out in memory, can replace another thread's data cache
//<THREAD MARKS
//It's only theory, result depends on OS and CPU specific offcourse


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
#include <chrono>
#include <vector>

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
//Pthread_mutex_TRrylock() usefull to prevent DEADLOCK, lock if can, return false if already locked
//Use lock/unlock in Same Order to avoid DEADLOCK
//If 2 THreads have dependency on lock use SAME mutex to avoid case when one release and another
//...start to wait infinitely
//AVOID NESTED LOCK
//TRANSFER mutex ownership between scope by std::unique_lock (has only move ctors)

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
			if (tmp == nullptr) //better init before with nullptr
			{
				std::lock_guard<std::mutex> lock(m_lock);
				//no sync-with,so current and next line are equal to if(instance = nullptr)
				tmp = instance.load(std::memory_order_relaxed);
				if (tmp == nullptr)//check again, we can just compare instance to nullprt (not lvalue)
				{
					tmp = new Singleton_ex();
					instance.store(tmp,std::memory_order_release);//(2) sync-with by acquire-release
				}
			return tmp;
		}		
};*/

//Pthread_Cond_T is "signal and wait"
//wake up waiting thread by signal,waiting thread must wait until some condition become true (producer/consumer)
//waiting(pthread_cond_wait) case must be rare (how much rare?...)
//usefull in producer/consumer problem and semaphore, only one non-abstract example i have founded is bounded queue.
//semaphore - limit count of threads have access(by wait(limit--)) to share resource, if limit reached (0) next will 	"wait" until one of accessed finish(post(limit++)), ++ current number and send "wake up" signal to one "waiting"...
//not found any "not study" example of semaphore...
//"waiting" implemented (in POSIX) by futex with WAIT flag and null timeout (futex is syscall) - thread "sleep"
//until futex with WAKE flag call.
//advantage is use unlock from not owned thread!
//efficiency of pthread_cond_wait lesser mutex (POSIX)
//Performance highly depend on platform/CPU instruction

class Binaphore {//or pthread_mutex_timedlock but without "illegal" unlock
	pthread_mutex_t  mutex;
	pthread_cond_t cond;
	bool locked,is_clear;
	//int timeout;
	public:
		Binaphore(/*int timeout_*/):locked(0),is_clear(0)
		{
			//is_clear = ATOMIC_FLAG_INIT;
			mutex = PTHREAD_MUTEX_INITIALIZER;
			cond = PTHREAD_COND_INITIALIZER;
			//locked = ATOMIC_FLAG_INIT;//default state is cleared(false)
			//set default timeout, otherwise wait return immediately
			/*if (!timeout_)
				timeout+=2;*///
		}
		
		//omit memory order because this 2 call rare and possibly from one thread
		void set_not_clear()
		{
			is_clear=0;
		}
		void clear_res()
		{
			if (!is_clear)
			{
				is_clear=1;
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
			int ret=0;//default succefull f() value
			//first time flag is zero, read (0) mod (1) write(1), next thread will blocked
			//if timeout: no mutex lock, er<0
			//if wait "lie": locked back it to wait again
			while(locked&&(!ret))
			{
				//atomic: release mutex when calling wait and lock cond
				//after succesfull (rc=0) return wait, acquire mutex again
				//ret = pthread_cond_timedwait(&cond, &mutex, &timeToWait);
				ret=pthread_cond_wait(&cond, &mutex);
			}
			locked=1;
			if ((ret<0)/*&& (ret!=ETIMEDOUT)*/)
				std::cout<<"lock failled err:"<<ret<<" \n";
			#ifdef debug_Timoutlck
			std::cout<<"ret:"<<ret<<"\n";
			#endif
			pthread_mutex_unlock(&mutex);
		}

		void unlock()
		{	
			//use same mutex to avoid parallel lock and unlock, because there is a dependency on lock 
			//in wait/signal it can be chance that signal be earlier than wait, and wait thread can be wait
			//infinitely
			pthread_mutex_lock(&mutex);
			locked=0;//set locked to false
			pthread_cond_signal(&cond);
			pthread_mutex_unlock(&mutex);
		}
		
		~Binaphore()
		{
			clear_res();
		}
};

//PTHREAD_MUTEX_ERRORCHECK: recursive lock from one thread cause error
//PTHREAD_MUTEX_RECURSIVE: ...not cause, counter++, release if counter=0, has counter limit
//...pthread_mutex_trylock() counter++, return true, false if reach limit or ...
//PTHREAD_MUTEX_DEFAULT: ...cause UB
//PTHREAD_MUTEX_NORMAL: ...cause DL

//pthread_mutex_unlock() fail if thread doesn't own mutex, UB - for DEFAULT and NORMAL
//pthread_mutex_lock() fail if thread has lower priority


//---C++11 STD---\\

//STD THREAD OBJECT IT IS NOT THREAD IT SELF!!!

//Copy-constructor&Operator= forbid for thread object 

//std::async + GCC = no comment (at least today...)

//std::thread is just class-wrapper, but COmfortable and SAFE!!!
//...but pthread more flexiable and fast
//pthread create with default joinable attr
//...(can be joined only ONCE, by only thread WITHOUT data races, if not - UB)
//failed to join joinable thread produce Zombie!
//join return control to thread call join, detached - immediately, returning value (void*) return to second join arg
//(1)
//joinable thread MUST be JOin to avoid resource leak!
//threads need it's own try/catch
//to cancel tread just use return(always, never return local var address)
//if main thread exit() - UB

//(1)
//C++11 thread MUSN'T be JOinable(not been detached or joined AND represent active state) before call ~THread()
//...or also can be Moved(non-copyable,non-assignable), Default-Constructed
//thread terminate normal or by thow exception (cancel is difficult)
//to use NAtive pthread.h function call native_handle() for fast performance, but with carefull!
//c++11: THread that has finished is still active thread of execution and joinable
//...(4)Detached THread CAN BE active after ~thread object
//......Because THread OBject is only Wrapper!
//(2)
//return value to called thread by std::promise and std::future with try/catch block in joined thread (or just send pointer as agr to avoid infinite future_wait or using try/catch)
//...prom_.set_value use single mutex
//...i don't know real situation where this can be usefull 


//std::recursive_mutex use TRy-lock to avoid throw (max counter overflow)
//std::mutex recursive lock from owned thread - UB
//...std::unlock from not-owned thread - UB, NO-throw,
//......prior to lock SYNC_WITH
//...std::try_lock, NO-throw
//......PRior unlock,lock SYNC_WITH if true!
//......SPuriously fail - "lie" that has owner
//...std::lock, THrow!
//above true for both mutex and recursive_mutex, except recursive lock

//std::lock_guard - scope lock
//std::unique_lock - Dtor call unlock the associated mutex, if OWned (Preffered than Unlock!)
//std::unique_lock::lock/try_lock throw if no associated mutex or if already locked by this unique_lock
//std::unique_lock::unlock throw if no associated mutex or mutex is not locked
//std::unique_lock::release break associated relation, no throw
//...deffered case for std::lock after, adopt - before, trylock - for trylock
//std::unique_lock::owns_lock check if this own locked mutex

//std::lock(arg1,...argn) to AVoid DEADLOCK! if use mutex with different order (pass by arg as example)
//...unspecified series of calls to lock, try_lock, unlock all if throw (does it seems be UB?)
//...may cause LL (unable to make further progress) problem or performance degradation
//...so i preffer just don't use it...

//(1*)
//it seems std::condition_variable equal pthread impl with except that it can throw 
//std::condition_variable for std::unique_lock (for efficienty in some platform!?)
//std::condition_variable_any with any lock...
//...Dtor require notify_all, if mutex GLobal(not always good choice!) and if we join in ~ThreadPool which local than all
//...Threads have finished before, if mutex local than it's lifecycle musn't end until all thread finish.
//...(Mutex can be declared earlier than ThreadPool)  

//STd::ONnce_flag/std::call_ONce - to call function ONce(DCLP like)!

//(1++)
//~std::thread=>std::terminate() if it not joined or detached

//(5)
//std::thread works with any callable type, so we can use function object (not temporary, it Ignore new thread run!)
//...avoid by parentheses or Braces or lamda expr (C non-compatible)

//(6)
//Don't use Cast local params when pass to thread function arg, local can be Destroyed before cast end
//(7) PASS PARAM reference to Thread Arg with STD::REF or use STD::BIND, otherwise Thread will make FULL-COPY
//(8) USE STD::MOVE for object that can only be move
//(9) THREAD std:move usefull for Thread Pool, when pushing in container just move temporary thread, NOT create new
//...and NOT destroy
//(10) std::thread::hardware_concurrency()
//(11) shared_ptr usefull for container and deffer delete

void thread_f1(/*std::promise<int> prom_*/)
{
	
	//(4)
	//std::this_thread::sleep_for(std::chrono::seconds(10));
	
	//(2)
	//prom_.set_value(1);//notify future

	//(5)
	std::this_thread::sleep_for(std::chrono::seconds(5));
	std::cout<<"run with id:"<<std::this_thread::get_id()<< "\n";
}

//(5)
/*class background_task
{
	public:
		void operator()()
		{
			thread_f1();
		}
};*/

//(5++)	
class thread_guard//NOT usefull:if stack overflow you'll get memory leak 
{
	std::thread& t;
	public:
		explicit thread_guard(std::thread &t_):t(t_)
		{}
		~thread_guard()
		{
			std::cout<<"~guard"<<"\n";
			if (t.joinable())//double join/detached BAD idea
			{
				t.join();//or detached
				std::cout<<"guard finish"<<"\n";
			}
		}
		thread_guard(const thread_guard &x)=delete;
		thread_guard& operator=(const thread_guard &x)=delete;
};

//(8)
void f2(std::unique_ptr<int> _data)
{
}

//(11)if another thread trow, dtor won't be called
void f3()
{
	/*try
	{*/
		//throw 20;
	/*}
	catch(...)
	{*/
		//std::cout<<"throw\n";
	//}
}

int main()
{	
	//(2)
	//std::promise<int> prom_;
	//std::future<int> prom_future = prom_.get_future();
	//std::thread t1(thread_f1);
	//prom.future_wait();//wait until set_value can cause infinite wait, if thread-t1 sleep forever
	//std::cout<<"recieved value:"<<prom_future.get()<<"\n";

	//(1)
	//t1.join();//wait until execution end then release resources

	
	{	//(1++)
		//std::thread t1(thread_f1);
		//t1.join();//if not join or detached =>std::terminate()
		
		//(4)
		//std::thread t1(thread_f1);
		//t1.detach();//if main thread exit =>UB
	}
	//(4)
	//std::this_thread::sleep_for(std::chrono::seconds(15));
	
	//(5)	
	/*std::cout<<"main id:"<<std::this_thread::get_id()<<"\n";
	//std::thread t1(background_task());//just return thread object, don't call new thread or associated f()
	std::thread t1{background_task()};//with {} work correct
	t1.join();*/
	//(5++)
	/*try
	{	
		std::thread t1(thread_f1);
		thread_guard g(t1);
		//IF it throw than we lost chance to join/detach thread
		//solution:try/catch or class wrapper thread guard
		throw 20;
		//t1.join();//if not join or detached =>std::terminate(), guard wrapper allow omit this
	}
	catch(...){};
	std::this_thread::sleep_for(std::chrono::seconds(15));*/
	
	//(8)
	/*
	std::unique_ptr<int> data(new int(1));
	f2(std::unique_ptr<int>(new int(1)));//move auto for temporary
	//f2(data);//error:but not auto for named
	f2(std::move(data));//correct*/

	//(9-10)
	/*std::vector<std::thread> th_pool;
	const int TH_MAX=std::thread::hardware_concurrency()-1;
	th_pool.reserve(TH_MAX);
	for(int i=0;i<TH_MAX;++i)
		th_pool.push_back(std::thread(thread_f1));
	for(int i=0;i<TH_MAX;++i)
		th_pool[i].join();
	std::cout<<"availiable threads count:"<<std::thread::hardware_concurrency()<<"\n";*/

	//(11)
	/*
	std::thread t1(f3);
	thread_guard g(t1);
	std::this_thread::sleep_for(std::chrono::seconds(5));
	*/
	std::cout<<"program finish"<<"\n";
	return 0;
}
