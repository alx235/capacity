//from https://rosettacode.org/wiki/Sorting_algorithms/Radix_sort#C.2B.2B
#include <algorithm>
#include <iostream>
#include <iterator>

struct strct_prms
{
      int left;
      int right;
};

void quickSort(int *arr, int left, int right)
{
      int sort_count = 1;
      strct_prms *stack_prms = new strct_prms[right-left];

      stack_prms[0].left = left;
      stack_prms[0].right = right;

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
			std::cout<<"left\n";
			stack_prms[sort_count].right = j;
			stack_prms[sort_count++].left = left;
			ignore = true;
	      }
	      if (i < right)
	      {
			std::cout<<"right\n";
			if (ignore)//must keep order
			{
				stack_prms[sort_count+1] = stack_prms[sort_count];//shift left to next
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
		std::cout<<"end\n";
      }
      delete[] stack_prms;
}
 
// Radix sort comparator for 32-bit two's complement integers
class radix_test
{
    const int bit; // bit position [0..31] to examine
public:
    radix_test(int offset) : bit(offset) {} // constructor
 
    bool operator()(const int &value) const // function call operator
    {
        if (bit == 31) // sign bit
            return value < 0; // negative int to left partition
        else
            return !(value & (1 << bit)); // 0 bit to left partition
    }
};
 
// Most significant digit radix sort (recursive)
void msd_radix_sort(int *first, int *last, int msb = 31)
{
    if (first != last && msb >= 0)
    {
        int *mid = std::partition(first, last, radix_test(msb));
        msb--; // decrement most-significant-bit
        msd_radix_sort(first, mid, msb); // sort left partition
        msd_radix_sort(mid, last, msb); // sort right partition
    }
}
 
// test radix_sort
int main()
{
    int data[] = { 170, 45, 75, -90, -802, 24, 2, 66, 67 };

	int data2[] = { 170, 45, 75, -90, -802, 24, 2, 66, 67 };
	std::copy(data2, data2 + 9, std::ostream_iterator<int>(std::cout, " "));
	quickSort(data2,0,9);
 
    /*msd_radix_sort(data, data + 9);
 
    std::copy(data, data + 9, std::ostream_iterator<int>(std::cout, " "));*/
    std::cout<<"\n";
    std::copy(data2, data2 + 9, std::ostream_iterator<int>(std::cout, " "));

	/*int a = 1;int a2 = 2;
	int const &a_r = a;
	const int a3;*/
 
    return 0;
}