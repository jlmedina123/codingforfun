/* 
Give you an array which has n integers,it has both positive and negative integers.Now you need sort this array in a special way.After that,the negative integers should in the front,and the positive integers should in the back.Also the relative position should not be changed. 
eg. -1 1 3 -2 2 ans: -1 -2 1 3 2. 
o(n)time complexity and o(1) space complexity is perfect.

https://www.careercup.com/question?id=5201559730257920

*/

void sortsign1(int *seq, size_t size) {
        int *pos = malloc(sizeof(int) * size);
        int *neg = malloc(sizeof(int) * size);
        int *p = pos;
        int *n = new;
        int i;
        for (i = 0; i < size; i++) {
                if (seq[i] < 0)
                        *n++ = seq[i];
                else
                        *p++ = seq[i];
        }
        int *pp, *nn;   
        i = 0;
        for (pp = pos; pp < p; pp++)
                seq[i++] = *pp; 
        for (nn = neq; nn < n; nn++)
                seq[i++] = *nn;
        free(pos);
        free(neg);
}

void sortsign2(int *seq, size_t size) {
	int negs = 0;
	int pos = 0;
	int *p;
	for (i = 0; i < size; i++) {
		if (seq[i] < 0) negs++;
		else	pos++;
	}
	assert(pos + negs == size);
	for (i = 0; i < size; i++) {
		if ((seq[i] > 0) && i < negs) {
			intruder = seq[i];
			memcpy(seq[i], seq
