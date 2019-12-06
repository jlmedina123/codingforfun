/*
Given a binary tree, print the path that has the maximum path sum. 

For this problem, a path is defined as any sequence of nodes from some starting node to any node in the tree along the parent-child connections. The path must contain at least one node and does not need to go through the root. the node val may be negative one
*/

int maxpath(struct node *tree, struct node ) {
	if (!tree) return 0
	l = tree->value + maxpath(tree->left, max+tree->value);
	r = tree->value + maxpath(tree->right, max+tree->value);
	
	if (l > r)
		if (l < max) {
			beggining
		
	else
		return r


