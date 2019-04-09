/*
Given an int array which might contain duplicates, find the largest subset of it which form a sequence. 
Eg. {1,6,10,4,7,9,5} 
then ans is 4,5,6,7 
*/


int largestsequence(int seq[], size_t size) {
	sort(seq, size);
	int i, j, n, max, maxi;
	maxi = 0;	
	max = 0;
	for (i = 0; i < size; i++) {
		n = 0;
		for (j = i+1; j < size; j++) {
			if (seq[j+1] == seq[j])
				n++;
			else
				break;
		}
		if (n > max) {
			max = n;
			maxi = i;
		}
	}
}

int largestsequencebitmap(int seq[], size_t size) {
	int i, max, maxi;
	uint64_t bitmap = 0;
	for (i = 0; i < size; i++)
		bitmap |= 1<<seq[i];
	i = 0;
	while (i < sizeof(bitmap)*8) {
		if (1<<bitmap[i])
			n++;
		else {
			if (n > max) {
				maxi = i-1;
				max = n-1;
			}
			n = 0;
		}
		i++;
	}
	maxi = maxi - max;
	for (i = 0; i < max; i++)
		printf("%d ", seq[i+maxi]);
}

