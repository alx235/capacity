
#include <algorithm>
#include <iostream>
#include <iterator>
#include <time.h>
#include <memory>

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

void quickSort(int *arr, int left, int right)//iterative QSort
{
      int sort_count = 1;
      std::unique_ptr<strct_prms[]> stack_prms (new strct_prms[right-left]());
      stack_prms[0].left = left;
      stack_prms[0].right = right-1;

      while(sort_count--)
      {
	      bool ignore = false;
	      int left = stack_prms[sort_count].left;
	      int right = stack_prms[sort_count].right;

	      int i = left, j = right;
	      int tmp;
	      int mid = arr[(left + right) / 2];
	 
	      /* partition */
	      while (i <= j) {
		    while (arr[i] < mid)
		          i++;
		    while (arr[j] > mid)
		          j--;
		    if (i <= j) {
		          tmp = arr[i];
		          arr[i] = arr[j];
		          arr[j] = tmp;
		          i++;
		          j--;
		    }
	      };
	 
	      /* recursion */
	      if (left < j)
	      {
			//std::cout<<"left\n";
			stack_prms[sort_count].right = j;
			stack_prms[sort_count++].left = left;
			ignore = true;
	      }
	      if (i < right)
	      {
			//std::cout<<"right\n";
			if (ignore)//must keep order
			{
				stack_prms[sort_count+1] = stack_prms[sort_count];//shift to next
				//set right before left to keep true order	
				stack_prms[sort_count].left = i;
				stack_prms[sort_count++].right = right;
				sort_count++;//increment again because we push twice
			}
			else
			{
				stack_prms[sort_count].left = i;
				stack_prms[sort_count++].right = right;
			}
	      }
		//std::cout<<"end\n";
      }
}

void inline merge(int* const a,int* const b,const int low,const int mid,const int high)
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
			merge(a,b.get(),low,stack_prms[sort_count].mid,high);
			stack_prms[sort_count].check_=0;
			//std::cout<<"merge: "<<"low:"<<low<<" mid:"<<stack_prms[sort_count].mid<<" high:"<<high<<" count:"<<sort_count<<" tmp:"<<tmp<<"\n";
			--sort_count;
		}
		else
		{
			if(low<high)
				{	
					stack_prms[sort_count].check_=1;
					//std::cout<<"set: "<<"low:"<<low<<" high:"<<high<<" count:"<<sort_count<<"\n";
					mid=(low+high)/2;
					stack_prms[sort_count++].mid=mid;

					stack_prms[sort_count].low=mid+1;
					stack_prms[sort_count++].high=high;

					stack_prms[sort_count].low=low;
					stack_prms[sort_count].high=mid;
					//merge_sort(a,low,mid);
					//merge_sort(a,mid+1,high);
					//merge(a,low,mid,high);
				}
			else
				--sort_count;
		}
	}
}
 
// Radix sort comparator for 32-bit two's complement integers
class radix_test
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
};
 
// Most significant digit radix sort (recursive)
void msd_radix_sort(int *first, int *last, int msb = 31)
{
    if ((first!=last) && (msb>=0))
    {
        int *mid = std::partition(first, last, radix_test(msb));
        msb--; // decrement most-significant-bit
        msd_radix_sort(first, mid, msb); // sort left partition
        msd_radix_sort(mid, last, msb); // sort right partition
    }
}

/* A utility function to print array of size n */
void printArray(int* arr,int n)
{
    for (int i=0; i<n; ++i)
        std::cout << arr[i] << " ";
    std::cout << "\n";
}

// To heapify a subtree rooted with node i which is
// an index in arr[]. n is size of heap
void heapify(int* const arr,const int &n, int j, bool exch_frst_lst)
{
	int sort_count=0;
	int largest=0;
	int l=0;
	int r=0;
	int m=n;
	int tmp=0;
	//(1):Build heap (rearrange array) - j=n/2-1;
	//(2):one by one extract an element from heap - j=n-1
	for (int i=j;i >= 0;--i) 
	{		
		sort_count=1;
		if (exch_frst_lst)//(2):Initialize largest as first,step-by-step reduce sort length	
		{
			std::swap(arr[0],arr[i]);//(2):Move current root to end, end to start
			m=i;
			tmp=i;
			i=0;
		}
		else
			tmp=i;
		while(sort_count--)
		{
			largest=i;//Initialize largest as root

			l=2*i+1;//left = 2*i + 1
			r=2*i+2;//right = 2*i + 2
			// If left child is larger than root
			if ((l<m) && (arr[l]>arr[largest]))
				largest=l;
			// If right child is larger than largest so far
			if ((r<m) && (arr[r]>arr[largest]))
				largest=r;
			// If largest is not root
			if (largest!=i)
			{
				std::swap(arr[i],arr[largest]);
				// Recursively heapify the affected sub-tree (3 elements)
				//heapify(arr, n, largest);
				i=largest;
				++sort_count;
			}
		}
		i=tmp;
	}
}
 
// main function to do heap sort
void heapSort(int* const arr,int n)
{
    // Build heap (rearrange array)
	heapify(arr,n,n/2-1,false);
    //one by one extract an element from heap
	heapify(arr,0,n-1,true);
}

void gen_rand(int* const data,const int size)
{
	srand((unsigned)time(NULL));
	for (int i=0;i<size;++i)
		data[i]=rand()/(i+10000000)+rand()/1000000000 + i + i^2;
}

void check_correct(int* const data,const int size)
{
	for (int i=0;i<size;++i)//check correct
	for (int j=i;j<size;++j)	
		if (data[i]>data[j])
			std::cout<<"--";
}
 
// test radix_sort
int main()
{	
	const int size=10000;
	std::unique_ptr<int[]> data(new int[size]);
	gen_rand(data.get(),size);quickSort(data.get(),0,size);check_correct(data.get(),size);//printArray(data.get(),size);
	gen_rand(data.get(),size);heapSort(data.get(),size);check_correct(data.get(),size);//printArray(data.get(),size);
	gen_rand(data.get(),size);msd_radix_sort(data.get(),data.get()+size);check_correct(data.get(),size);//printArray(data.get(),size);
	gen_rand(data.get(),size);merge_sort(data.get(),0,size);check_correct(data.get(),size);//printArray(data.get(),size);

	/*int a = 1;int a2 = 2;
	int const &a_r = a;
	const int a3;*/
 
    return 0;
}
