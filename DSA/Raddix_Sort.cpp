
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <time.h>
#include <memory>
#include <chrono>

/* A utility function to print array of size n */
void printArray(int* arr,int n)
{
    for (int i=0;i<n;++i)
        std::cout<<arr[i]<<" ";
    std::cout<<"\n";
}

struct strct_prms
{
	int left;
	int right;
	strct_prms()
	{
		left=0;
		right=0;
	}	
};
void quickSort(int *arr,int right)//iterative QSort
{
	int sort_count=1;
	std::unique_ptr<strct_prms[]> stack_prms (new strct_prms[right]);
	strct_prms* _stack_prms=stack_prms.get();
	strct_prms* _ptr=nullptr;
	stack_prms[0].left=0;
	stack_prms[0].right=right-1;

	int* _ar1=nullptr;
	int* _ar2=nullptr;
	bool _left_set=false;
	int _left=-1;
	int _right=-1;
	int _tmp=-1;
	int _tmp_left=-1,_tmp_right=-1;
	int _mid=-1;
	int i=0,j=0;
	while(sort_count--)
	{
		_left_set=false;
		_ptr=_stack_prms+sort_count;
		_left=_ptr->left;
		_right=_ptr->right;
		i=_left;
		_ar1=arr+_left;
		j=_right;
		_ar2=arr+_right;
		_mid=arr[(_left+_right)>>1];
		while (i<=j)
		{
			while (*_ar1<_mid)
			{
	  			++_ar1;++i;
			}
			while (*_ar2>_mid)
			{
	  			--_ar2;--j;
			}
			if (i<=j) 
			{
			_tmp=*_ar1;
			*_ar1=*_ar2;
			*_ar2=_tmp;
			++_ar1;++i;
			--_ar2;--j;
			}
		}
		if(_left<j)
		{
			_ptr->right=j;
			_ptr->left=_left;
			++sort_count;++_ptr;
			_left_set=true;
		}
		if(i<_right)
		{
			_ptr->left=i;
			_ptr->right=_right;
			++sort_count;++_ptr;
			if(_left_set)
			{
				//exchange _ptr-1 and _ptr-2 to keep recursive order 
				--_ptr;
				_tmp_left=_ptr->left;
				_tmp_right=_ptr->right;
				*_ptr=*(_ptr-1);
				--_ptr;
				_ptr->left=_tmp_left;
				_ptr->right=_tmp_right;
			}
		}
	}
}

inline void _merge(int* const a,int* const b,const int low,const int high)
{
	if (low==high)
		return;
	int _low1=low;
	const int mid=(low+high)>>1;
	int _low2=mid+1;
	int* _b=b;//tmp buffer

	int* _a1=a+_low1;//first array
	int* _a2=a+_low2;//second array
	while((_low1<=mid)&&(_low2<=high))
	{
		if(*_a1<*_a2)
		{
			*_b=*_a1;
			++_low1;++_b;++_a1;
		}
		else
		{
			*_b=*_a2;
			++_low2;++_b;++_a2;
		}
	}
	if (_low1<=mid)
		while (_low1<=mid)
		{
			*_b=*_a1;
			++_low1;++_b;++_a1;			
		}
	else
		while (_low2<=high)
		{
			*_b=*_a2;
			++_low2;++_b;++_a2;		
		}
	int* _b_start=b;
	std::copy(_b_start,_b,a+low);
}

struct strct_prms2
{
	int low;
	int high;
	int check_;
	strct_prms2()
	{
		low=-1;
		high=-1;
		check_=0;
	}	
};

void merge_sort(int* const a,const int high)
{
	const int LOW=0;
	const int TMPSIZE=high-LOW;
	const int STACKSIZE=log2(TMPSIZE)*2+3;
	int sort_count=0;
	strct_prms2 stack_prms[STACKSIZE];
	std::unique_ptr<int[]> buff(new int[TMPSIZE]);
	int* _buff = buff.get();
	stack_prms[0].low=LOW;
	stack_prms[0].high=high-1;
	strct_prms2* _ptr=nullptr;
	int _low=-1;
	int _high=-1;
	int _mid=-1;
	while(sort_count>=0)
	{
		strct_prms2* _ptr=stack_prms+sort_count;
		_low=_ptr->low;
		_high=_ptr->high;
		if((_ptr->check_))
		{
			_merge(a,_buff,_low,_high);
			_ptr->check_=0;
			--sort_count;
		}
		else
		{
			if(_low<_high)
				{	
					
					_ptr->check_=1;
					_mid=(_low+_high)>>1;
					++sort_count;++_ptr;
					_ptr->low=_mid+1;
					_ptr->high=_high;
					++sort_count;++_ptr;
					_ptr->low=_low;
					_ptr->high=_mid;
				}
			else
				--sort_count;
		}
	}
}

void _heapify(int* const arr,const int &n, int j, bool exch_frst_lst)
{
	int sort_count=0;
	int _max=0;
	int _l=0;
	int _r=0;
	int m=n;
	int _tmp=0;

	//(1):j=n/2-1;
	//(2):j=n-1
	for (int i=j;i >= 0;--i) 
	{		
		sort_count=1;
		_tmp=i;
		if (exch_frst_lst)//(2)
		{
			std::swap(arr[0],arr[i]);//exchange root and tail
			m=i;//shift border guard one to root(not check tail again)
			i=0;//always start check from root to new border guard position
		}
		while(sort_count--)//up max to root, lowest become down to tail
		{
			_max=i;//Initialize max as root

			_l=2*i+1;//left
			_r=2*i+2;//right
			if ((_l<m) && (arr[_l]>arr[_max]))
				_max=_l;
			if ((_r<m) && (arr[_r]>arr[_max]))
				_max=_r;
			// if max is not root
			if (_max!=i)
			{
				std::swap(arr[i],arr[_max]);//exchange in sub-tree,continue check max child
				i=_max;
				++sort_count;
			}
		}
		i=_tmp;
	}
}
 
//not require extra memory, but unstable
void heapSort(int* const arr,const int n)
{
    //Build heap,up largest to root, lowest become down to tail
	_heapify(arr,n,n/2-1,false);
    //exchange root with tail, shift border guard one to root(not check tail again),lowest become down to tail
	//always start check from root to new border guard position
	_heapify(arr,0,n-1,true);
}

inline int* _radix_sort(int* const buff,int* const first,const int size,const int signbit)
{
	bool _is_true=0;
	int* const _result_begin=buff;
	int* const _result_end=buff+size;
	int* const last=first+size;

	int* _result1=_result_begin;
	int* _result2=_result_end-1;
	int* _first=first;
	int* _last=last;
	int i=0;
	for (;_first!=_last;)
	{
        if (signbit==31)//sign bit
            _is_true=*_first<0;//negative int to left partition
        else
            _is_true=!(*_first & (1<<signbit));//0 bit to left partition
		if (_is_true)
		{//left
			*_result1=*_first;
			++_result1;
		}
		else
		{//right,second group
			*_result2=*_first;
			--_result2;
			++i;
		}
		++_first;
	}
	int* _right_start=last-i;
	if (i)
	{
		_first=first;//don't trust 3d library
		int* __result_end=_result_end;
		int* __right_start=_right_start;
		std::copy(_result_begin,_result1,_first);//left direct order
		//std::reverse_copy(_result2,__result_end,_right_start);
		while (i)//right reverse order
		{
    		--__result_end;
    		*__right_start=*__result_end;
    		++__right_start;
			--i;
  		}
	}
	return _right_start;
}

inline int* _radix_sort_unstable(int* const first,int* const last,const int signbit)
{
	bool _is_true=0;

	int* _first=first;
	int* _first2=last;//false start position
	int* _last=last;
	bool _false_found=0;
	for (;_first!=_last;)
	{
        if (signbit==31)//sign bit
            _is_true=*_first<0;//negative int to left partition
        else
            _is_true=!(*_first & (1<<signbit));//0 bit to left partition
		if (_is_true)
		{//left
			if (_false_found)
			{
				std::swap(*_first,*_first2);//send false forward, always exchange with first false
				++_first2;
			}	
		}
		else
		{//right,second group
			if (!_false_found)
			{	
				_false_found=1;
				_first2=_first;//position where false
			}
		}
		++_first;
	}
	return _first2;
}

struct strct_prms3
{
	int* first;
	int* last;
	int msb;
	strct_prms3()
	{
		first=nullptr;
		last=nullptr;
		msb=0;
	}	
};

void lsd_radix_sort(int* const first, int* const last,int byte_size=31)
{
	std::unique_ptr<int[]> buff(new int[last-first]);
	int* _buff=buff.get();
	int size=last-first;
	for (int lsb=0;lsb<byte_size+1;++lsb)
	{
		_radix_sort(_buff,first,size,lsb);
	}
}
void msd_radix_sort(int* const first, int* const last,int bsize=31)
{
	int sort_count=1;
	int* _mid=nullptr; 
	//std::unique_ptr<int[]> tmp(new int[lst-frst]);
	//int* tmp_=tmp.get();
	strct_prms3 stack_prms[bsize+1];
	stack_prms[0].first=first;
	stack_prms[0].last=last;
	stack_prms[0].msb=bsize;

	int* _first=nullptr;
	int* _last=nullptr;
	int _msb=-1;
	
    while (sort_count--)
    {	
		strct_prms3* _ptr=stack_prms+sort_count;
		_first=_ptr->first;
		_last=_ptr->last;
		_msb=_ptr->msb;   
		if ((_first!=_last) && (_msb>=0))
		{
			_mid=_radix_sort_unstable(_first,_last,_msb);
			//mid_=__radix_sort(tmp_,first_,last_,msb_);
			--_msb;
			
			_ptr->first=_mid;
			_ptr->last=_last;
			_ptr->msb=_msb;
			++_ptr;
			++sort_count;

			_ptr->first=_first;
			_ptr->last=_mid;
			_ptr->msb=_msb;
			++sort_count;	
		}
    }
}

void gen_rand(int* const data,const int size)
{
	srand((unsigned)time(NULL));
	for (int i=0;i<size;++i)
		//data[i]=/*pow(-1,i)**/(rand()/(i+10000000)+rand()/1000000000 + i + i^2);
		//data[i]=-1000+(i+1)/100000;
		data[i]=rand()/1000000;
}

void check_correct(int* const data,const int size)
{
	std::cout<<"check\n";
	for (int i=0;i<size-1;++i)//check correct	
		if (data[i]>data[i+1])
		{
			std::cout<<"violation\n";
			return;
		}
}

void _helper(int* const data,const int size,const std::string &mes,
	void (*sort)(int* const, int* const,int),int def_size=31)
{
	//#define _print_
	#define _check_
	using namespace std::chrono;
	steady_clock::time_point begin,end;
	gen_rand(data,size);
	#ifdef _print_
	{	
		std::cout<<"old value\n";
		printArray(data,size);
	}
	#endif
	begin=steady_clock::now();
	(*sort)(data,data+size,def_size);
	end=steady_clock::now();
	
	std::cout<<mes<<": "<<duration_cast<nanoseconds>(end-begin).count()<<" ns"<<std::endl;
	#ifdef _print_
	{
		std::cout<<"new value\n";
		printArray(data,size);
	}
	#endif
	#ifdef _check_
	check_correct(data,size);
	#endif
}

void _helper2(int* const data,const int size,const std::string &mes,
	void (*sort)(int* const, int))
{
	using namespace std::chrono;
	steady_clock::time_point begin,end;
	gen_rand(data,size);
	#ifdef _print_
	{	
		std::cout<<"old value\n";
		printArray(data,size);
	}
	#endif
	begin=steady_clock::now();
	(*sort)(data,size);
	end=steady_clock::now();
	
	std::cout<<mes<<": "<<duration_cast<nanoseconds>(end-begin).count()<<" ns"<<std::endl;
	#ifdef _print_
	{
		std::cout<<"new value\n";
		printArray(data,size);
	}
	#endif
	#ifdef _check_
	check_correct(data,size);
	#endif
}
// test
int main()
{	
	const int size=10000000;
	std::cout<<"size="<<size<<"\n";
	std::unique_ptr<int[]> data(new int[size]);
	int* const _data=data.get();
	gen_rand(_data,size);
	std::unique_ptr<int[]> data2(new int[size]);
	std::unique_ptr<int[]> data3(new int[size]);
	std::unique_ptr<int[]> data4(new int[size]);
	std::unique_ptr<int[]> data5(new int[size]);
	int* const _data2=data2.get();
	int* const _data3=data3.get();
	int* const _data4=data4.get();
	int* const _data5=data4.get();
	std::copy(_data,_data+size,_data2);
	std::copy(_data,_data+size,_data3);
	std::copy(_data,_data+size,_data4);
	std::copy(_data,_data+size,_data5);
	_helper(_data,size,"lsd__",lsd_radix_sort);
	_helper(_data2,size,"msd__",msd_radix_sort);
	_helper2(_data3,size,"quick",quickSort);
	_helper2(_data4,size,"merge",merge_sort);
	_helper2(_data5,size,"heap_",heapSort);

	return 0;
}
