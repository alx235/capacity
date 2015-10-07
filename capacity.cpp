//g++ capacity.cpp -o capacity -Wall -pthread -std=c++0x
#include <iostream>
/*#include <thread>*/
#include <ctime>
using namespace std;

void ab(int &a) {
}

void hoarasort(int* a, int first, int last) {   
  int first_ = first, last_ = last;int tmp, mid = a[(first + last) / 2];
  std::cout<<"first:"<<a[first]<<"\n";
  std::cout<<"last:"<<a[last]<<"\n";
  do {
    while (a[first_] < mid)
      first_++;
    while (a[last_] > mid)
      last_--;
    if (first_ <= last_) {
      if (first_ < last_) {
        tmp=a[first_];
        a[first_]=a[last_];
        a[last_]=tmp;
      }
      first_++;
      last_--;
    }
 }
 while (first_ <= last_);

 if (first_ < last) hoarasort(a, first_, last);
 if (first < last_) hoarasort(a, first,last_);
}

/*
struct A {
  int a;
};

//new except handler
void bad_mem_() {
	std:: cerr << "bad alloc" << "\n";
	throw std::bad_alloc();
}

//OVERLOAD OPERATOR
class Example {
  int x;
  A *a;
  static const int ex1=1;//static const POD(!) member can be init in declaration
  //int &a;
  //const int b;
  public:
  Example (A *a_) {
    a = a_;
  };
    
  //DEREF OVERLOAD, can return any type, ofthen use in smart pointer class
  A* operator-> () {
    return a;
  };

  /*Example(int b_,int a_):a(a_),b(b_) {//const and ref MUST be init in init-list of constructor
  }*//*
  private://private access for defenited copy and assignment operations block this operation...
  explicit Example(const Example &ex) {
    //init raw memory, NO cast type
  }
  Example& operator=(const Example &ex) {
    if(this!=&ex) {//check equal himself(adress in memory)
      //delete old, create new
    }
    return *this;
  }
  Example* operator&(); //prefix (NO ARG ACCEPT) unar oper (ONE OPERAND)
  Example operator&(Example X); //binar AND required 1 arg, at least
  const Example& operator[](int i);//any POD type arg, const args mod block mod element!!!

  ///FUNCTIONAL CALL
  void operator() (Example &ex) {
    ex.x = 1;
  };

};

/*
both C/C++ complile code h file code example

#ifdef __cplusplus
extern "C" {
#endif

somedef
// all of your legacy and compatible C code here

#ifdef __cplusplus
}
#endif

to use function with another lang. compiler you need put just declaration using 'C' compatible .h file:

extern(in C++) "C" //mean link somedef with C lang specification
somedef

that link with definiton contained in binary file(lib, etc.)
     and not take influence on call's semantics (check type, etc.)
     Why? see cpp_marks!

add lib to linker:
g++ -std=c++11 my_code.cpp -lmy_library -L/custom_directory_path

create a library of this C code. The following steps create a shared library :

$ gcc -c -Wall -Werror -fPIC Cfile.c
$ gcc -shared -o libCfile.so Cfile.o
*/

/*
ambiguous

void f1(int i,int j) {
}

void f1(double i,double j) {
}
*/

/*
//Mystack...
template<class T> class StackSmartPtr {
  T* ar_ptr;

public:

  StackSmartPtr(T* v_ptr_) {
    ar_ptr = v_ptr_;
  }

  ~StackSmartPtr() {
    delete[] ar_ptr;
  }
};

template<class T> class My_Stack {
  T* v_ptr;
  size_t max_size;
  size_t top;

  inline void insert(const T& item) {
    v_ptr[top++]=item;
  }

public:

  My_Stack(size_t size) {
    v_ptr = new T[size];
    max_size = size;
    top = 0;
  }

  ~My_Stack() {
    delete[] v_ptr;
  }

  void push_back(T item) {
    if (top<max_size) {
      insert(item);
    }
    else {
      //delete anycase
      StackSmartPtr<T> smart_ptr_(v_ptr);
      size_t new_size = max_size*2;
      T* tmp_ptr = new T[new_size];
      std::copy(v_ptr,v_ptr+max_size,tmp_ptr);
      max_size = new_size;
      v_ptr = tmp_ptr;
      insert(item);
    }
  }

  T pop_back() {
    return v_ptr[--top];
  }
};
*/

/*
//static local init time!
struct  B {
  int p;  
  B() {
    p=0;
  }
  
  ~B() {
    cout<<"~B"<<endl;
  }
};

struct  A:B {
  
  A() {
    ;
    throw int ();
  }
  
  ~A() {
    cout<<"~A"<<endl;
  }
};

int i=1;

//thread's life-cycle
void some(int s) {
std::cout<<"join tread" << i << " \n";
  this_thread::sleep_for(chrono::seconds(s));
std::cout<<"join tread finish\n";
}*/

/*
//predeclaration for two-side lynk
struct List;

struct Link {
  Link* pre;
  List* some;
};

struct List {
  Link* head;
  };*/

int main (int argc, char * argv[]) {
  /*
  int num;
  cin>>num;
  size_t size = sizeof(int) * 8 + 1;
  char bin[size];
  char * bin_ptr = bin;
  unsigned int tmp=/*0xFFFFFFFF;*//*0x80000000;
  while(--size) {
    *bin_ptr++=(num&tmp)>0 ? '1' : '0';
    cout<<tmp<<endl;
    tmp>>=1;
    
  }
  *bin_ptr='\0';
  cout<<bin<<endl;*/
  //A *obj = NULL;
  //std::shared_ptr<A> obj;
  /*std::cout<<"main thread begin\n";
  std::thread thr_j1(&some,5);
  std::thread thr_j2(&some,10);
 std::cout<<"main thread create threads finish\n";
 thr_j1.join();
 std::cout<<"1 finish\n";i=5;
 thr_j2.join();

 std::cout<<"main thread end\n";
 return 0;*/
  /*
  My_Stack<string> mystack_str(2);
  mystack_str.push_back("1");
  mystack_str.push_back("2");
  mystack_str.push_back("3");
  std::cout<<"last:"<<mystack_str.pop_back()<<"\n";*/
  /*
  //void* safe with same type, maybe with equal length type..?
  int a=1;
  int* p = &a;
  void *pv;
  //implicit conversion
  pv = p;
  //explicit conversion
  int *p2 = static_cast<int*>(pv);
  std::cout<<"v:"<<*p2<<"\n";
  */

  /*
  char* v = "static";
  for (int i=0;v[i]!=0;i++)
    v[i]; 
  for (char* p=v;*p!=0;p++)
    *p; 
    */

  /*
  //ambiguous overloaded
  f1(2);*/

  /*
  //operator -> overload
  A a;

  Example ex(&a);

  ex->a;
  */

  /*
  //some ex

  std::stringstream input = std::stringstream(argv[0]);
  std::string path(input.str());
  std::cout << "path:" << path << "\n";
  char ch;
  /*
  //IGNORE END INPUT:enter code
  while (std::cin>>/*std::noskipws>>*//*ch) {
				      //CHECK UNICODE
				      if ((ch < 0) || (ch >= 255)) {
				      std::cerr << "error\n";
				      continue;
				      }
				      if (isdigit(ch))
				      std::cout << "num\n";
				      else 
				      if (isalpha(ch))
				      std::cout << "alpha\n";
				      else
				      if (isspace(ch)) {
				      std::cout << "space\n";
					
				      }
				      }*/

  /*
  //SUBexpression no order, and new except handler
  int a, b;
  //left operand ( is the object of a mathematical operation) compute first!
  a = (b = 2, b + 1);
  std::cout << "a:"<<a << "\n";
  std::cin>>ch;
  //computing order of subexpression is undefined!
  //v[i] = i++; - NEVER!!!

  */

  /*
    std::set_new_handler(bad_mem_);
    //std::cout << "size:" << sizeof(long long) << "\n";
    while(1)
    new long [1000 * 256 * 2];
    std::cin >> ch;
    //long ar[1000*256*2];
    */

  /*
  //-2!!!
  int a = 0xfffffffe;
  std::cout << "a:" <<a << "\n";
  std::cin >> ch;
  return 0;
  */
  /*
  srand(time(0)); // AUTO RANDOMIZE HOAR ALGORITM CALL
  int count = 10;
  int * const mas = new int[count];
  int *ar = mas;
  std::cout<<"unsort array:\n";
  for (int i=0;i<count;i++) {
    *ar++ = rand() % 10;
  }
  for (int i=0;i<count;i++) {
    std::cout<<i<<":"<<mas[i]<<"\n";
  }
  hoarasort(mas, 0, count-1);
  std::cout<<"sort array:\n";
  for (int i=0;i<count;i++) {
    std::cout<<mas[i]<<"\n";
  }
  delete[] mas;
  double a = 1;
  // REF must be lvalue and strong type!
  ab(a);
  return 0;
  */
}
