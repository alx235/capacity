
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <time.h>
#include <memory>
#include <chrono>

//for std::partition
// Radix sort comparator for 32-bit two's complement integers
/*class radix_test
{
    const int bit; // bit position [0..31] to examine
public:
    radix_test(int offset) : bit(offset) {} // constructor
 
    bool operator()(const int &value) const // function call operator
    {
        if (bit==31) // sign bit
            return value<0; // negative int to left partition
        else
            return !(value & (1<<bit)); // 0 bit to left partition
    }
};*/

/*
void __msd_radix_sort_stable(int *first, int *last,int* const tmp,int msb=31)
{
    if ((first!=last) && (msb>=0))
    {
		//std::cout<<"msb:"<<msb<<"\n";
        int *_mid = __radix_sort(tmp,first,last,msb);
        msb--;
        __msd_radix_sort_stable(first,_mid,tmp,msb);
        __msd_radix_sort_stable(_mid,last,tmp,msb);
    }
}

void msd_radix_sort_stable(int *first, int *last,int msb=31)
{
	std::unique_ptr<int[]> _tmp(new int[last-first]);
	__msd_radix_sort_st(first,last,_tmp.get(),msb);
}*/

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

void quickSort(int *arr, int left, int right)//iterative QSort
{
      int sort_count = 1;
      std::unique_ptr<strct_prms[]> stack_prms (new strct_prms[right-left]());
      stack_prms[0].left = left;
      stack_prms[0].right = right-1;

      while(sort_count--)
      {
	      bool ignore=false;
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
	 
	      /* "recursion" */
	      if (left<j)
	      {
			//std::cout<<"left\n";
			stack_prms[sort_count].right=j;
			stack_prms[sort_count++].left=left;
			ignore=true;
	      }
	      if (i<right)
	      {
			//std::cout<<"right\n";
			if (ignore)//must keep order
			{
				stack_prms[sort_count+1]=stack_prms[sort_count];//shift to next
				//set right before left to keep true order	
				stack_prms[sort_count].left=i;
				stack_prms[sort_count++].right=right;
				sort_count++;//increment again because we push twice
			}
			else
			{
				stack_prms[sort_count].left=i;
				stack_prms[sort_count++].right=right;
			}
	      }
		//std::cout<<"end\n";
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

void merge_sort(int* const a,const int low_,const int high_)
{
	int sort_count=0;
	std::unique_ptr<strct_prms2[]> stack_prms(new strct_prms2[high_-low_]());
	std::unique_ptr<int[]> b(new int[high_-low_]());
	stack_prms[0].low=low_;
	stack_prms[0].high=high_-1;
	int low=-1;
	int high=-1;
	int mid=-1;
	while(sort_count>=0)
	{
		low=stack_prms[sort_count].low;
		high=stack_prms[sort_count].high;
		//std::cout<<"low:"<<low<<" high:"<<high<<" count:"<<sort_count<<"\n";
		if(int tmp = stack_prms[sort_count].check_)
		{
			_merge(a,b.get(),low,stack_prms[sort_count].mid,high);
			stack_prms[sort_count].check_=0;
			//std::cout<<"merge: "<<"low:"<<low<<" mid:"<<stack_prms[sort_count].mid<<" high:"<<high<<" count:"<<sort_count<<" tmp:"<<tmp<<"\n";
			--sort_count;
		}
		else
		{
			if(low<high)
				{	
					stack_prms[sort_count].check_=1;
					mid=(low+high)/2;
					stack_prms[sort_count++].mid=mid;

					stack_prms[sort_count].low=mid+1;
					stack_prms[sort_count++].high=high;

					stack_prms[sort_count].low=low;
					stack_prms[sort_count].high=mid;
				}
			else
				--sort_count;
		}
	}
}

void _heapify(int* const arr,const int &n, int j, bool exch_frst_lst)
{
	int sort_count=0;
	int max=0;
	int l=0;
	int r=0;
	int m=n;
	int tmp=0;
	//(1):j=n/2-1;
	//(2):j=n-1
	for (int i=j;i >= 0;--i) 
	{		
		sort_count=1;
		if (exch_frst_lst)//(2)
		{
			std::swap(arr[0],arr[i]);//exchange root and tail
			m=i;//shift border guard one to root(not check tail again)
			i=0;//always start check from root to new border guard position
		}
		tmp=i;
		while(sort_count--)//up max to root, lowest become down to tail
		{
			max=i;//Initialize max as root

			l=2*i+1;//left
			r=2*i+2;//right
			if ((l<m) && (arr[l]>arr[max]))
				max=l;
			if ((r<m) && (arr[r]>arr[max]))
				max=r;
			// if max is not root
			if (max!=i)
			{
				std::swap(arr[i],arr[max]);//exchange in sub-tree,continue check max child
				i=max;
				++sort_count;
			}
		}
		i=tmp;
	}
}
 
//not require extra memory, but unstable
void heapSort(int* const arr,int n)
{
    //Build heap,up largest to root, lowest become down to tail
	_heapify(arr,n,n/2-1,false);
    //exchange root with tail, shift border guard one to root(not check tail again),lowest become down to tail
	//always start check from root to new border guard position
	_heapify(arr,0,n-1,true);
}

inline int* __radix_sort(int* const buff,int* const first,int* const last,const int signbit)
{
	bool _is_true=0;
	int* const _result_begin=buff;
	int* const _result_end=buff+(last-first);

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
	if (_i)
	{
		//std::cout<<_i<<"\n";
		std::copy(_result_begin,_result_end,first);
		//std::cout<<first[0]<<" "<<first[1]<<" "<<first[2]<<"\n";
	}
	return last-_i;
}

inline int* __radix_sort_unstable(int* const first,int* const last,const int signbit)
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
	int* first_;
	int* last_;
	int msb_;
	strct_prms3()
	{
		first_=nullptr;
		last_=nullptr;
		msb_=0;
	}	
};

void lsd_radix_sort(int* const first, int* const last,int byte_size=31)
{
	std::unique_ptr<int[]> _tmp(new int[last-first]);
	int* _tmp2 = _tmp.get();
	
	for (int lsb=0;lsb<byte_size+1;++lsb)
	{
		__radix_sort(_tmp2,first,last,lsb);
	}
}

// Most significant digit radix sort (iterative)

void msd_radix_sort(int *frst, int *lst,int bsize = 31)
{
	int sort_count=1;
	int* mid_=nullptr; 
	std::unique_ptr<int[]> tmp(new int[lst-frst]);
	int* tmp_=tmp.get();
	strct_prms3 stack_prms[bsize+1];
	stack_prms[0].first_=frst;
	stack_prms[0].last_=lst;
	stack_prms[0].msb_=bsize;

	int* first_=nullptr;
	int* last_=nullptr;
	int msb_=-1;
	
    while (sort_count--)
    {	
		strct_prms3* stack_tmp=stack_prms+sort_count;
		first_=stack_tmp->first_;
		last_=stack_tmp->last_;
		msb_=stack_tmp->msb_;   
		if ((first_!=last_) && (msb_>=0))
		{
			//mid_=__radix_sort_unstable(first_,last_,msb_);
			mid_=__radix_sort(tmp_,first_,last_,msb_);
			--msb_;
			
			stack_tmp->first_=mid_;
			stack_tmp->last_=last_;
			stack_tmp->msb_=msb_;
			++stack_tmp;
			++sort_count;

			stack_tmp->first_=first_;
			stack_tmp->last_=mid_;
			stack_tmp->msb_=msb_;
			++sort_count;	
		}
    }
}

void gen_rand(int* const data,const int size)
{
	srand((unsigned)time(NULL));
	for (int i=0;i<size;++i)
		data[i]=pow(-1,i)*(rand()/(i+10000000)+rand()/1000000000 + i + i^2);
}

void check_correct(int* const data,const int size)
{
	for (int i=0;i<size;++i)//check correct
	for (int j=i;j<size;++j)	
		if (data[i]>data[j])
			std::cout<<"violation";
}

/* A utility function to print array of size n */
void printArray(int* arr,int n)
{
    for (int i=0; i<n; ++i)
        std::cout << arr[i] << " ";
    std::cout << "\n";
}

void _test(){};

void _helper(int* const data,const int size,const std::string &mes,
	void (*sort)(int* const, int* const,int),int def_size=31)
{
	#define _print_
	//#define _check_
	using namespace std::chrono;
	steady_clock::time_point begin,end;
	gen_rand(data,size);
	#ifdef _print_
	printArray(data,size);
	#endif
	begin=steady_clock::now();
	(*sort)(data,data+size,def_size);
	end=steady_clock::now();
	
	std::cout<<mes<<": "<<duration_cast<nanoseconds>(end-begin).count()<<" ns"<<std::endl;
	#ifdef _print_
	printArray(data,size);
	#endif
	#ifdef _check_
	check_correct(data,size);
	#endif
}
 
// test
int main()
{	
	const int size=10;
	//const int size=100000000;
	//const int size=10000;
	std::cout<<"size="<<size<<"\n";
	std::unique_ptr<int[]> data(new int[size]);
	int* _data=data.get();
	//_helper(_data,size,"msd",&msd_radix_sort);
	_helper(_data,size,"lsd",&lsd_radix_sort);
 	//gen_rand(_data,size);printArray(_data,size);lsd_radix_sort(_data,_data+size);printArray(_data,size);
    return 0;
}
