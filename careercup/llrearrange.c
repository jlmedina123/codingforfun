/*
https://www.careercup.com/question?id=5701630599626752
LinkedList : 
Input : A>B>C>D>E 
Output: A>E>B>D>C
*/

struct node {
	struct node * next;
	char value;
}

void addbeginning(struct node **list, char value) {
	struct node * new = malloc(sizeof(struct node));
	new->value = value;
	new->next = NULL;
	if (!*list) {
		*list->new;
		return;
	}
	new->next = (*list)->next;
	*list = new;
}


void rearrage_withstack(struct node * list) {
        struct node *left = list;
        struct node *right = list;
        while (right) {
                right = right->next;
                push(right, stack);
        }
        while (!empty(stack)) {
                right = pop(stack);
                right->next = left->next;
                left->next = right;
                left = right;
                right = pop(stack); // alternate. wont use this one
        }
}

void rearrange(struct node **list, int order[], int len) {
	struct node *array[] = malloc(sizeof(struct node *) * len);
	struct node *curr = *list;
	int i = 0;
	while (curr) {
		array[i] = curr;
		i++;
		curr = curr->next;
	}
	*list = array[order[0]];
	curr = *list;
	for (i = 1; i < len-1; i++) {
		curr->next = array[order[i+1]]->next;
		curr = curr->next;
	}
}

void rearrangerecursive(struct node **list) {
	struct node *curr = *list;
	
int main(int argc, const char *argv[]) {
	struct node ** list = NULL;
		
