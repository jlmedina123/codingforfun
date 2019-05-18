
/*
https://www.careercup.com/question?id=5716403849003008
Given an array of integers greater than zero, find if it is possible to split it in two (without reordering the elements), such that the sum of the two resulting arrays is the same. Print the resulting arrays.
*/
int splitarray(int arr[], size_t size) {
	int i;
	int sumforward[] = malloc(size);
	int sumbackwards[] = malloc(size);
	int c = 0;
	sumforward[0] = arr[0];
	sumbackwards[0] = arr[size-1];
	for (i = 1; i < size-1; i++) {
		sumforward[i] = sumforward[i-1] + arr[i];
		sumbackward[i] = sumbackward[i-1] + arr[size-i];
	}
	for (i = 0; i < size; i++) {
		if (sumforward[i] == sumbackward[i]) {
			return i;
		}
	}
	return -1;
}

int leftsum(int arr[], size_t n, int i, int sum) {
	if (i =< 0)
		return 0;
	sum+=arr[i];
	return sum + leftsum(arr, n, i-1, sum);
}

int rightsum(int arr, size_t n, int i, int sum) {
	if (i >= n)
                return 0;
	sum+=arr[i];
	return sum + rightsum(arr, n, i+1, sum);
 
int splitarrayrec(int arr[], size_t n) {
	int i;
	int left, right;
	for (i = 0; i < n; i++) {
		left = leftsum(arr, n, i);
		right = rightsum(arr, n, i);
		if (left == right)
			return i;
	}
	return -1;
}		


