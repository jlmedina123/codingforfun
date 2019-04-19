/*
Given a circular single linked list.Write a program that deletes every kth node until only one node is left. 
After kth node is deleted, start the procedure from (k+1)th node. 
e.g.list is 1->2->3->4->5->1 
k=3 
1. You are at 1, delete 3. 
List is: 1->2->4->5->1 
2. You are at 4, delete 1 
List is: 2->4->5->2 
3. You are at 2,delete 5 
List is: 2->4->2 
4. You are at 2, delete 2 
List is: 4 
Return 4. 

How efficient you can do it?

*/
struct node {
	struct node *next;
	int value;
}

void insert(struct node **tree, int value) {
	struct node *new = malloc(sizeof(struct node));
	new->value = value;
	new->next = NULL;
	assert(!tree);
	if (!*tree) {
		*tree = new;
		new->next = new;
		return;
	}
	struct node *prev = *tree;
	struct node *curr = (*tree)->next;
	while (curr != *tree && curr->value < value) {
		prev = curr;
		curr = curr->next;
	}
	prev->next = new;
	new->next = current;
}

struct node * kdelete(struct node **tree) {
	struct node * curr = *tree;
	int i = 0;
	while (curr->next != curr) {
		while (i < 3) {
			curr = curr->next;
			i--;
		}
		curr->value = curr->next->value;
		curr->next = curr->next->next;
		free(curr->next);
		i = 0;
	}	
}	
