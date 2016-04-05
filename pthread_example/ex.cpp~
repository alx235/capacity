
//#include <algorithm>
#include <atomic>
#include <iostream>
#include <iterator>
#include <time.h>
#include <memory>

#define cpu_relax_flag
#ifdef cpu_relax_flag
	#define cpu_relax()	asm volatile("pause" ::: "memory")
#endif

//SPINLOCK MUTEX
//busy waiting + cpu_relax() for reduce CPU consumption
//use for short-time wait
//OR just use pthread library:
// pthread_spin_init(&spinlock, 0); pthread_spin_lock(&spinlock);
// pthread_spin_unlock(&spinlock); pthread_spin_destroy(&spinlock) - additionly release lock;
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
			lock.cleared(std::memory_order_released);
		}
};

//DEADLOCK EXAMPLE
/*
//pthread_mutex_trylock() usefull to prevent DEADLOCK, lock if can, return false if already locked
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
class Singleton_ex {
	std::atomic<Singleton_ex*> instance;
	std::mutex m_lock;
	int i;

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
	Singleton_ex* get_DCLP()
	{	// prevent memory reordering next line -> tmp non-atomic
		//non-atomic pointer is faster, see value before release
		//next conditions true until init finish
		Sigleton_ex* tmp = instance.load(std::memory_order_acquire);
		if (tmp = nullptr) //better init before with nullptr
		{
			std::lock_guard<std::mutex> lock(m_lock);
			//no sync-with,so current and next line are equal to if(instance = nullptr)
			tmp = instance.load(std::memory_order_relaxed);
			if (tmp = nullptr)//check again, we can just compare instance to nullprt (not lvalue)
			{
				tmp = new Singleton_ex();
				instance.store(tmp,std::memory_order_release);
			}
		return tmp;
	}		
};


int main()
{	
	
	return 0;
}
