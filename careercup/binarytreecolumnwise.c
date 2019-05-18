/*

Given the root of a binary tree, print the nodes column wise and row wise.

..............6
............/....\
...........9......4
........../..\......\
.........5....1.....3
..........\........./
...........0.......7
The answer would be 5 9 0 6 1 4 7 3.

https://www.careercup.com/question?id=5709428993556480

*/

void printtree(struct node * tree) {
	if (!tree)
		return;
	struct node *curr = tree->left;
	struct node *prev = tree;
	if (!curr) {
		printf("%d ", prev->value);
		printtree(prev->right);
	}
	while (curr->left) {
		prev = curr;
		curr = curr->left;
	}
	printf("%d ", curr->value);
	printf("%d ", prev->value);
	printtree(prev->right);
	printtree(curr->right)
	
}

void printtree(struct node *tree) {
	if (!tree)
		return;
	printtree(tree->left);
	printf("%d ", tree->value);
	printtree(tree->right);
}

void printtreecolumnrow(struct node *tree, int line, int col) {
	struct node *curr = tree;
	while (curr->left) {
		curr->line = line;
		curr->col = col;
		curr = curr->left;
		line-=1;
		col-=1;
	}	
