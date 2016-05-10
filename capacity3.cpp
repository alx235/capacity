/*#include <cmath>
#include <iostream>
#include <memory>
#include <chrono>*/
#include <mutex>

/*class A{
	int a;
	A& operator=(const A &x){return *this;}
	A(const A &x){}
	public:
		A()
		{
			a=1;
		}
		virtual~A(){std::cout<<"A finish\n";}//print 3 time because there is not overrid in derived
};

class B: public A{
	int b;
	public:
		B()
		{
			b=1;
		}
};*/

int main()
{
	//ponter fun
	/*int* p=((int*)2+3);
	std::cout<<sizeof(p)<<"\n";*/

	//inheritance fun
	/*A a;
	A a1;
	B b;
	B* ptr_b=&b;
	//A a1(a);//error
	//a=a1;//error
	A* ptr_a=ptr_b;//error if B:A or B: private/protected A or A()-private

	//cast fun
	const int c=1;
	int c1=1;
	//int& ref=c;//error	
	const int& ref=c1;
	const_cast<int&>(ref)=2;//SAFE because c1 non-const, otherwise-UB
	//dynamic_cast - expensive!
	//cast base to derived work if polymorphism exist (downcast), otherwise - compile error
	if (dynamic_cast<B*>(ptr_a))//if ref can cause exception badcast
		std::cout<<"ptr_a upcast true\n";
	//can cause compile error
	B* ptr_b2=static_cast<B*>(ptr_a);//downcast - UB!!!!
	A* ptr_a2=static_cast<A*>(ptr_b);//upcast work
	void* _tmp=static_cast<void*>(&c1);
	int* c2=static_cast<int*>(_tmp);
	std::cout<<*c2<<"\n";*/
	std::mutex lk;
	lk.lock();
	int i=99;
	lk.unlock();
	return 0;
}
