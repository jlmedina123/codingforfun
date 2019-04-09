/*
https://jlmedina123.wordpress.com/2013/04/09/sorting-in-c/

Sorting in C

Below there are common sorting routines implemented in C.

Bubble sort and Insert sort are simpler to implement, with average runtime O(n^2). Bubble sort is the simplest, and just passes through all the data n times, swapping elements that are not in order. Insert sort improves on bubble sort. It passes though the data n times, but it takes advantage of the fact that after pass i-th, all elements from index 0 to i are already sorted. So it just finds where to insert element i+1 within the first i elements, and shifts the greater elements to the right. In the best case (array previously sorted), runtime is O(n).

Quicksort and Heapsort are faster, and have average runtime O(n*log n). Quicksort uses divide and conquer, by diving the list in two, and recursively sorting the two subsets (which means dividing each in two again). Heapsort converts the list into a heap, and keeps taking the heap’s root, which is the largest element.

Distribution sorting is the fastest type of algorithm, and can achieve linear runtime O(n). Bucket sort divides the data into subsets, which can be sorted on different processors. Flash sort assumes data follows a known probability distribution. It then separates data into m subsets (m ~ 0.5 n) or classes with a min and max number, which partially orders the data. Then it applies insertion sort to each subset.

*/


/* bubble sort routine 
 * Worst case performance: O(n^2)
 * Best case performance: O(n)
 * Average performance: O(n^2)
 * Space: O(n)
 */
void bubblesort(int sequence[], int size)
{
    int i;
    int pass;
    /* passing all elements */
    for (pass = 0; pass < size - 1; pass++)
    {
        for (i = 0; i < size - 1; i++)
        {
            /* swapping elements if needed */
            if (sequence[i] > sequence[i+1])
                swap(&sequence[i], &sequence[i+1]);
        }
    }
}
 
/* function to swap two value on two variables */
void swap(int *p1, int *p2)
{
    int temp;
    temp = *p1;
    *p1 = * p2;
    *p2 = temp;
    /* without temp variable:
    *p1 ^= *p2;
    *p2 ^= *p1;
    *p1 ^= *p2;
    */
}
 
/* insertion sort routine 
 * Worst case performance: O(n^2)
 * Best case performance: O(n)
 * Average performance: O(n^2)
 * Space: O(n)
 */
void insertionsort(int sequence[], int size)
{
  int i, k, element;
    for (i = 1; i < size; i++) {
        element = seq[i]; // save element
        k = i-1;          // scan all elements to left
        // if seq previously sorted, exp in while will always be false at
        // first try, which means runtime will be linear. 
        while (k >= 0 && seq[k] > element) { // find where to place element
            seq[k+1] = seq[k]; // keep shifting to right
            k--;
        }
        seq[k+1] = element; // place element at new ordered place
    }
}
 
 
/* quick sort routine driver */
void quicksort(int sequence[], int size)
{
    quicksortroutine(sequence, 0, size - 1);
}
 
/* quick sort routine 
 * Worst case performance: O(n^2)
 * Best case performance: O(n log n)
 * Average performance: O(n log n)
 * Space: O(log n) or O(n)
 */
void quicksortroutine(int sequence[], int left, int right)
{
    int pivot, pivot_value, l_hold, r_hold;
    l_hold = left;
    r_hold = right;
    pivot = left; // could by any
    pivot_value = sequence[left]; 
    // all numbers smaller than pivot_value will be to left of pivot_value
    // all numbers greater than pivot value will be to right of pivot value
    while (left < right)
    {
        while((sequence[right] >= pivot_value) && (left < right))
            right--;
        if (left != right)
        {
            sequence[left] = sequence[right];
            left++;
         }
         while ((sequence[left] <= pivot_value) && (left < right))
             left++;
         if (left != right)
         {
             sequence[right] = sequence[left];
             right--;
         }
    }
    /* 'left' is now the border between smaller numbers than pivot_value
        (to the left) and greater numbers than pivot_value (to the right) */
    sequence[left] = pivot_value; /* pivot_value is at its final sorted location
                                     in the array */
    pivot = left;
    left = l_hold;
    right = r_hold;
    /* recursive call to sort the subarrays to the left and right of pivot */
    if (left < pivot)
        quicksortroutine(sequence, left, pivot-1);
    if (right > pivot)
        quicksortroutine(sequence, pivot+1, right);
}
 
/* shell sort routine 
 * worst and best performance depends on gap sequence 
 */
void shellsort(int a[], int n)
{
    int j,i,k,m,mid;
    for(m = n/2;m>0;m/=2)
    {
        for(j = m;j< n;j++)
        {
            for(i=j-m;i>=0;i-=m)
            {
                if(a[i+m]>=a[i])
                    break;
                else
                {
                    mid = a[i];
                    a[i] = a[i+m];
                    a[i+m] = mid;
                }
            }
        }
    }
}
 
/* selection sort 
 * Worst case performance: O(n^2)
 * Best case performance: O(n^2)
 * Average performance: O(n^2)
 * Space: O(n)
 */
void selectionsort(int sequence[], int size)
{
    int i, j;
    int min, temp;
    for (i = 0; i < size-1; i++)
    {
        min = i;
        for (j = i+1; j < size; j++)
        {
            if (sequence[j] < sequence[min])
                min = j;
        }
        swap(&sequence[i], &sequence[min]);
    }
}
 
/* heap sort routine 
 * Worst case performance: O(n log n)
 * Best case performance: O(n log n)
 * Average performance: O(n log n)
 * Space: O(n)
 */
void heapsort(int numbers[], int array_size) {
  int i, temp;
  
  for (i = (array_size / 2); i >= 0; i--) {
    siftDown(numbers, i, array_size - 1);
  }
  
  for (i = array_size-1; i >= 1; i--) {
    /* swap */
    temp = numbers[0];
    numbers[0] = numbers[i];
    numbers[i] = temp;
  
    siftDown(numbers, 0, i-1);
  }
}
 
/* shift down function used for heap sort routine */
void siftDown(int numbers[], int root, int bottom) {
  int maxChild = root * 2 + 1;
  
  /* find the biggest child */
  if(maxChild < bottom) {
    int otherChild = maxChild + 1;
    /* reversed for stability */
    maxChild = (numbers[otherChild] > numbers[maxChild])?otherChild:maxChild;
  } else {
    /* dont overflow */
    if(maxChild > bottom) return;
  }
  
  /* if we have the correct ordering, we are done */
  if(numbers[root] >= numbers[maxChild]) return;
  
  /* swap */
  int temp = numbers[root];
  numbers[root] = numbers[maxChild];
  numbers[maxChild] = temp;
  
  /* tail queue recursion. Will be compiled as a loop with correct compiler switches */
  siftDown(numbers, maxChild, bottom);
}
 
/* merge sort routine driver */
void mergesort(int numbers[], int array_size)
{
  int temp[SIZE];
  m_sort(numbers, temp, 0, array_size - 1);
}
  
/* merge sort routine 
 * Worst case performance: O(n log n)
 * Best case performance: O(n log n)
 * Average performance: O(n log n)
 * Space: O(n)
 */
void m_sort(int numbers[], int temp[], int left, int right)
{
  int mid;
  if (right > left)
  {
    mid = (right + left) / 2;
    m_sort(numbers, temp, left, mid);
    m_sort(numbers, temp, mid+1, right);
  
    merge(numbers, temp, left, mid+1, right);
  }
}
 
/* merge routine used by merge sort routine */
void merge(int numbers[], int temp[], int left, int mid, int right)
{
  int i, left_end, num_elements, tmp_pos;
  
  left_end = mid - 1;
  tmp_pos = left;
  num_elements = right - left + 1;
  /* merging the two sides of the array */
  while ((left <= left_end) && (mid <= right))
  {
    if (numbers[left] <= numbers[mid])
    {
      temp[tmp_pos] = numbers[left];
      tmp_pos = tmp_pos + 1;
      left = left +1;
    }
    else
    {
      temp[tmp_pos] = numbers[mid];
      tmp_pos = tmp_pos + 1;
      mid = mid + 1;
    }
  }
  
  while (left <= left_end)
  {
    temp[tmp_pos] = numbers[left];
    left = left + 1;
    tmp_pos = tmp_pos + 1;
  }
  while (mid <= right)
  {
    temp[tmp_pos] = numbers[mid];
    mid = mid + 1;
    tmp_pos = tmp_pos + 1;
  }
  
  for (i=0; i <= num_elements; i++)
  {
    numbers[right] = temp[right];
    right = right - 1;
  }
}

