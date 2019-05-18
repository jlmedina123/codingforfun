'''
node { 
node * left, * right; 
} 
Given a list of node to determine whether the node in the list can form a valid binary tree. several points of judgment 
1. need to ensure that all left, right pointer point to the node inside list 
2. no cycle 
3. All node must be connected 
Boolean isValidTree(List<Node> list){}

'''

class Node:
	def __init__(self, val):
		self.val = val
		self.left = None
		self.right = None


def checkcycle(node, n):
	if node is None or (node.right is None and node.left is None):
		return False
	if n == 0:
		return True
	if checkcycle(node.left, n-1) == True or checkcycle(node.right, n-1) == True
		return True

def validbinarytree(seq):	
	# 1. need to ensure that all left, right pointer point to the node inside list 
	for node in seq:
		if node.left not in seq or node.right not in seq:
			return False

	# 2. no cycle
	for node in seq:
		if checkcycle(node.left, len(seq))
			return False
		if checkcycle(node.right, len(seq))
			return False


root = Node(5)
root.left = Node(2)
root.right = Node(5)


