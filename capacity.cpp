//g++ capacity.cpp -o capacity
#include <iostream>
using namespace std;

int main (int argc, char * argv[]) {
  int num;
  cin>>num;
  size_t size = sizeof(int) * 8 + 1;
  char bin[size];
  char * bin_ptr = bin;
  unsigned int tmp=/*0xFFFFFFFF;*/0x80000000;
  while(--size) {
    *bin_ptr++=(num&tmp)>0 ? '1' : '0';
    cout<<tmp<<endl;
    tmp>>=1;
    
  }
  *bin_ptr='\0';
  cout<<bin<<endl;
}
