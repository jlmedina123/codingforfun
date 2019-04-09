/* 
Give you an array which has n integers,it has both positive and negative integers.Now you need sort this array in a special way.After that,the negative integers should in the front,and the positive integers should in the back.Also the relative position should not be changed. 
eg. -1 1 3 -2 2 ans: -1 -2 1 3 2. 
o(n)time complexity and o(1) space complexity is perfect.
*/

int sortsign(int seq[], int size) {
	int i;
	int *pos = malloc(sizeof(int)*size);
	int *neg = malloc(sizeof(int)*size);
	int p = 0;
	int n = 0;
	for (i = 0; i < size; i++) {
		if (seq[i] > 0)
			pos[p++] = seq[i];
		else
			neg[n++] = seq[i];
	}
	for (i = 0; i < n; i++)
		seq[i] = neg[i];
	n=0;
	for ( ; i < size; i++)
		seq[i] = pos[n++];
}

int sortsignbubble(int seq[], int size) {
	for (i = 0; i < size-1; i++) {
		for (i = 0; i < size-1; i++) {
		if (seq[i] > 0 && seq[i+1] < 0)
			swap(&seq[i], &seq[i+1]);
	
