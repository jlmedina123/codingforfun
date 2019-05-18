class Node:
	def __init__(self, val):
		self.val = val
		self.left = None
		self.right = None

def distrowcolumn(tree, col, row):
	if tree is None:
		return
	if col not in weights:
		weights[col] = {}
	distrowcolumn(tree.left, col-1, row-1)
	weights[col][row] = tree.val
	distrowcolumn(tree.right, col-1, row+1)
	

root =  Node(1)
root.left = Node(2)
root.right = Node(3)
root.left.left = Node(4)
root.left.right = Node(5)
root.right.left = Node(6)
root.right.right = Node(7)
root.right.left.right = Node(8)

weights = {}
distrowcolumn(root, 0, 0)
for col in sorted(weights.keys()):
	for row in sorted(weights[col].keys()):
		print weights[col][row]


