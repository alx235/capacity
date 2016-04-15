
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <time.h>
#include <memory>
#include <chrono>

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
      stack_prms[0].left=0;
      stack_prms[0].right=right-1;
      while(sort_count--)
      {
	      bool left_set=false;
	      int left=stack_prms[sort_count].left;
	      int right=stack_prms[sort_count].right;

	      int i=left, j=right;
	      int tmp;
	      int mid=arr[(left+right)/2];
	 
	      /* partition */
	      while (i<=j) {
		    while (arr[i]<mid)
		          i++;
		    while (arr[j]>mid)
		          j--;
		    if (i<=j) 
			{
		    	tmp=arr[i];
		    	arr[i]=arr[j];
		    	arr[j]=tmp;
		    	i++;
		    	j--;
		    }
	      };
	      if (left<j)
	      {
			stack_prms[sort_count].right=j;
			stack_prms[sort_count++].left=left;
			left_set=true;
	      }
	      if (i<right)
	      {
			stack_prms[sort_count].left=i;
			stack_prms[sort_count++].right=right;
			if (left_set)
				std::swap(stack_prms[sort_count-1],stack_prms[sort_count-2]);
	      }
      }
}

inline void _merge(int* const a,int* const b,const int low,const int mid,const int high)
{
	int h,i,j,k;
	h=low;
	i=0;
	j=mid+1;
	// Merges the two array's into b[] until the first one is finish
	while((h<=mid)&&(j<=high))
	{
		if(a[h]<=a[j])
		{
			b[i]=a[h];
			h++;
		}
		else
		{
			b[i]=a[j];
			j++;
		}
		i++;
	}
	// Completes the array filling in it the missing values
	if(h>mid)
	{
		for(k=j;k<=high;k++)
		{
			b[i]=a[k];
			i++;
		}
	}
	else
	{
		for(k=h;k<=mid;k++)
		{
			b[i]=a[k];
			i++;
		}
	}
	// Prints into the original array
	for(k=0;k<=high-low;k++) 
	{
		a[k+low]=b[k];
	}
}

struct strct_prms2
{
	int low;
	int high;
	int mid;
	int check_;
	strct_prms2()
	{
		low=0;
		high=0;
		mid=0;
		check_=0;
	}	
};

void merge_sort(int* const a,const int high)
{
	const int LOW=0;
	const int TMPSIZE=high-LOW;
	const int STACKSIZE=log2(TMPSIZE)*3;
	int sort_count=0;
	strct_prms2 stack_prms[STACKSIZE];
	std::unique_ptr<int[]> b(new int[TMPSIZE]);
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
		//std::cout<<"low:"<<low<<" high:"<<high<<" count:"<<sort_count<<"\n";
		if((_ptr->check_))
		{
			_merge(a,b.get(),_low,_ptr->mid,_high);
			_ptr->check_=0;
			//std::cout<<"merge: "<<"low:"<<low<<" mid:"<<stack_prms[sort_count].mid<<" high:"<<high<<" count:"<<sort_count<<" tmp:"<<tmp<<"\n";
			--sort_count;
		}
		else
		{
			if(_low<_high)
				{	
					
					_ptr->check_=1;
					_mid=(_low+_high)/2;
					_ptr->mid=_mid;
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

inline int* _radix_sort(int* const buff,int* const first,int* const last,const int signbit)
{
	bool _is_true=0;
	int* const _result_begin=buff;
	int size=last-first;
	int* const _result_end=buff+size;

	int* _result1=_result_begin;
	int* _result2=_result_end-1;
	int* _first=first;
	int* _last=last;
	int _i=0;
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
			++_i;
		}
		++_first;
	}
	int* _right_start=last-_i;
	if (_i)
	{
		_first=first;//don't trust 3d library
		int* __result_end=_result_end;
		std::copy(_result_begin,_result1,_first);//left direct order
		//std::reverse_copy(_result2,__result_end,_right_start);
		while (_i)//right reverse order
		{
    		--__result_end;
    		*_right_start=*__result_end;
    		++_right_start;
			--_i;
  		}
	}
	return _right_start;
}

inline int* _radix_sort_unstable(int* const first,int* const last,const int signbit)
{
	bool _is_true=0;

	int* _first=first;
	int* _first2=first;//false start position
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
	if (_first2==first) return last;
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
	std::unique_ptr<int[]> tmp(new int[last-first]);
	int* _tmp=tmp.get();
	for (int lsb=0;lsb<byte_size+1;++lsb)
	{
		_radix_sort(_tmp,first,last,lsb);
	}
}
void msd_radix_sort(int* const first, int* const last,int bsize=31)
{
	int sort_count=1;
	int* _mid=nullptr; 
	/*std::unique_ptr<int[]> tmp(new int[lst-frst]);
	int* tmp_=tmp.get();*/
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
		data[i]=rand();
}

void check_correct(int* const data,const int size)
{
	std::cout<<"check\n";
	for (int i=0;i<size-1;++i)//check correct	
		if (data[i]>data[i+1])
			std::cout<<"violation";
}

/* A utility function to print array of size n */
void printArray(int* arr,int n)
{
    for (int i=0;i<n;++i)
        std::cout<<arr[i]<<" ";
    std::cout<<"\n";
}

void _helper(int* const data,const int size,const std::string &mes,
	void (*sort)(int* const, int* const,int),int def_size=31)
{
	#define _check_
	#ifndef _print_
		#define _check_
	#endif
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
	const int size=10000;
	std::cout<<"size="<<size<<"\n";
	std::unique_ptr<int[]> data(new int[size]);
	int* const _data=data.get();
	gen_rand(_data,size);
	/*std::unique_ptr<int[]> data2(new int[size]);
	std::unique_ptr<int[]> data3(new int[size]);
	std::unique_ptr<int[]> data4(new int[size]);
	int* const _data2=data2.get();
	int* const _data3=data3.get();
	int* const _data4=data4.get();
	std::copy(_data,_data+size,_data2);
	std::copy(_data,_data+size,_data3);
	std::copy(_data,_data+size,_data4);
	_helper(_data,size,"lsd__",lsd_radix_sort);
	_helper(_data2,size,"msd__",msd_radix_sort);*/
	//_helper2(_data,size,"quick",quickSort);
	_helper2(_data,size,"merge",merge_sort);
	//printArray(_data,size);
	//heapSort(_data,size);
	//printArray(_data,size);
	//merge_sort(_data,size);
	//check_correct(_data,size);
    return 0;
}
