
/* preserving order O(n2) */
void pushzeros(int *array, size_t size) {
	int i;
	int zeros = 0;
	for (i = 0; i < size; i++) {
		if (array[i] == 0) {
			zeros++;
			for (j = i; j < size-1; j++)
				array[j] = array[j+1];
		}
	}
	for (i = size-zeros; i < size; i++)
		array[i] = 0;
}

/* changing order O(n) */
void pushzeros2(int *array, const size_t size) {
	int i;
	int zeroi = size-1;
	for (i = 0; i < size; i++) {
		if (array[i] == 0) {
			array[i] ^= array[zeroi];
			array[zeroi] ^= array[i];
			array[i] ^= array[zeroi];
			zeroi--;
		}
	}
} 
