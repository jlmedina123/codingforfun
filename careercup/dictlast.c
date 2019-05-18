/*
Write a new data structure, "Dictionary with Last" 

Methods: 
set(key, value) - adds an element to the dictionary 
get(key) - returns the element 
delete(key) - removes the element 
last() - returns the last key that was added or read. 

In case a key was removed, last will return the previous key in order.
*/

struct data {
	int key;
	int value;
	int date;
}

struct data * dict[20];
int stack[20];
int stackindex = 0;

void push(int key) {
	stack[stackindex++] = key;
}
void set(int key, int value) {
	struct data * new = malloc(sizeof(struct data));
	new->value = value;
	dict[key] = new;
	push(key);
}

int get(int key) {
	return dict[key];
}

int last(void) {
	return dict[stack[stackindex-1]];
}
	
