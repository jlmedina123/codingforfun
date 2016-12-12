"""
A graph can be defined in computer science as a data structure with a set of 
vertices (or nodes) and a set of edges (connection between nodes).  
The easiest way to represent a graph is with a matrix.

There are several types of problems that can be resolve with a graph. 
The following summarizes the problems, and the algorithms to solve them:

- Exploring graph: given a node s, find how to reach all other nodes
        Depth-first search
        Breadth-first search
      
- Single-source shortest path: given a source s, find the shortest path 
  to all other vertices.
        Dijkstra’s algorithm
   
- All pairs shortest path: get the shortest path from any node to any 
  node. Gives matrices dist[u][v] and pred[u][v]
        Floyd-Warshall algorithm
       
- Minimum Spanning Tree: spanning tree is the set of edges that ensures 
  all vertices are connected. Minimum ST is the set of edges with minimim 
  total weight that ensures all vertices are connected.
        Prim’s algorithm
"""


def recursive_dfs(graph, start, path=[]):
    path=path+[start]
    for node in graph[start]:
        if node not in path:
            path=recursive_dfs(graph, node, path)
    return path
 
def iterative_dfs(graph, start):
    path = []
    # dfs uses a stack, so that it visits the last node
    # pushed to stack (explores as deep as possible first)
    stack=[start]
    while stack:
        v=stack.pop() # pops out the last in (go deeper)
        if v not in path:
            path+=[v] # add v to path
            stack+=graph[v] # push v's neighbors to stack
    return path
 
def iterative_bfs(graph, start):
    path = []
    queue=[start]
    # bfs uses a queue, so that it visits the nodes pushed
    # to the queue first. So it first visits all the neighbors
    # and then the neighbors' neighbors (explores the soroundings
    # as much as possible without going deep)
    while queue:
        v=queue.pop(0) # pops out the first in (go wider)
        if v not in path:
            path+=[v]
            queue+=graph[v] # push v's neighbors to queue
    return path
 


'''
+---- A
|   /   \
|  B--D--C
|   \ | /
+---- E
'''
graph = {'A':['B','C'],'B':['D','E'],'C':['D','E'],'D':['E'],'E':['A']}
print 'recursive dfs ', recursive_dfs(graph, 'A')
print 'iterative dfs ', iterative_dfs(graph, 'A')
print 'iterative bfs ', iterative_bfs(graph, 'A')

"""
recursive dfs  ['A', 'B', 'D', 'E', 'C']
iterative dfs  ['A', 'C', 'E', 'D', 'B']
iterative bfs  ['A', 'B', 'C', 'D', 'E']
"""


def popmin(pqueue):
    # A (ascending or min) priority queue keeps element with
    # lowest priority on top. So pop function pops out the element with
    # lowest value. It can be implemented as sorted or unsorted array
    # (dictionary in this case) or as a tree (lowest priority element is
    # root of tree)
    lowest = 1000
    keylowest = None
    for key in pqueue:
        if pqueue[key] < lowest:
            lowest = pqueue[key]
            keylowest = key
    del pqueue[keylowest]
    return keylowest
 
def dijkstra(graph, start):
    # Using priority queue to keep track of minium distance from start
    # to a vertex.
    pqueue = {} # vertex: distance to start
    dist = {}   # vertex: distance to start
    pred = {}   # vertex: previous (predecesor) vertex in shortest path
 
    # initializing dictionaries
    for v in graph:
        dist[v] = 1000
        pred[v] = -1
    dist[start] = 0
    for v in graph:
        pqueue[v] = dist[v] # equivalent to push into queue
 
    while pqueue:
        u = popmin(pqueue) # for priority queues, pop will get the element with smallest value
        for v in graph[u].keys(): # for each neighbor of u
            w = graph[u][v] # distance u to v
            newdist = dist[u] + w
            if (newdist < dist[v]): # is new distance shorter than one in dist?
                # found new shorter distance. save it
                pqueue[v] = newdist
                dist[v] = newdist
                pred[v] = u
 
    return dist, pred
 
graph = {0 : {1:6, 2:8},
1 : {4:11},
2 : {3: 9},
3 : {},
4 : {5:3},
5 : {2: 7, 3:4}}
 
dist, pred = dijkstra(graph, 0)
print "Predecesors in shortest path:"
for v in pred: print "%s: %s" % (v, pred[v])
print "Shortest distance from each vertex:"
for v in dist: print "%s: %s" % (v, dist[v])

"""
Predecesors in shortest path:
0: -1
1: 0
2: 0
3: 2
4: 1
5: 4
Shortest distance from each vertex:
0: 0
1: 6
2: 8
3: 17
4: 17
5: 20
"""

def floydwarshall(graph):
 
    # Initialize dist and pred:
    # copy graph into dist, but add infinite where there is
    # no edge, and 0 in the diagonal
    dist = {}
    pred = {}
    for u in graph:
        dist[u] = {}
        pred[u] = {}
        for v in graph:
            dist[u][v] = 1000
            pred[u][v] = -1
        dist[u][u] = 0
        for neighbor in graph[u]:
            dist[u][neighbor] = graph[u][neighbor]
            pred[u][neighbor] = u
 
    for t in graph:
        # given dist u to v, check if path u - t - v is shorter
        for u in graph:
            for v in graph:
                newdist = dist[u][t] + dist[t][v]
                if newdist &lt; dist[u][v]:
                    dist[u][v] = newdist
                    pred[u][v] = pred[t][v] # route new path through t
 
    return dist, pred
 
 
 
graph = {0 : {1:6, 2:8},
         1 : {4:11},
         2 : {3: 9},
         3 : {},
         4 : {5:3},
         5 : {2: 7, 3:4}}
 
dist, pred = floydwarshall(graph)
print "Predecesors in shortest path:"
for v in pred: print "%s: %s" % (v, pred[v])
print "Shortest distance from each vertex:"
for v in dist: print "%s: %s" % (v, dist[v])

"""
python floydwarshall.py 
Predecesors in shortest path:
0: {0: -1, 1: 0, 2: 0, 3: 2, 4: 1, 5: 4}
1: {0: -1, 1: -1, 2: 5, 3: 5, 4: 1, 5: 4}
2: {0: -1, 1: -1, 2: -1, 3: 2, 4: -1, 5: -1}
3: {0: -1, 1: -1, 2: -1, 3: -1, 4: -1, 5: -1}
4: {0: -1, 1: -1, 2: 5, 3: 5, 4: -1, 5: 4}
5: {0: -1, 1: -1, 2: 5, 3: 5, 4: -1, 5: -1}
Shortest distance from each vertex:
0: {0: 0, 1: 6, 2: 8, 3: 17, 4: 17, 5: 20}
1: {0: 1000, 1: 0, 2: 21, 3: 18, 4: 11, 5: 14}
2: {0: 1000, 1: 1000, 2: 0, 3: 9, 4: 1000, 5: 1000}
3: {0: 1000, 1: 1000, 2: 1000, 3: 0, 4: 1000, 5: 1000}
4: {0: 1000, 1: 1000, 2: 10, 3: 7, 4: 0, 5: 3}
5: {0: 1000, 1: 1000, 2: 7, 3: 4, 4: 1000, 5: 0}
"""


def popmin(pqueue):
    # A (ascending or min) priority queue keeps element with
    # lowest priority on top. So pop function pops out the element with
    # lowest value. It can be implemented as sorted or unsorted array
    # (dictionary in this case) or as a tree (lowest priority element is
    # root of tree)
    lowest = 1000
    keylowest = None
    for key in pqueue:
        if pqueue[key] < lowest:
            lowest = pqueue[key]
            keylowest = key
    del pqueue[keylowest]
    return keylowest
 
def prim(graph, root):
    pred = {} # pair {vertex: predecesor in MST}
    key = {}  # keep track of minimum weight for each vertex
    pqueue = {} # priority queue implemented as dictionary
 
    for v in graph:
        pred[v] = -1
        key[v] = 1000
    key[root] = 0
    for v in graph:
        pqueue[v] = key[v]
     
    while pqueue:
        u = popmin(pqueue)
        for v in graph[u]: # all neighbors of v
            if v in pqueue and graph[u][v] < key[v]:
                pred[v] = u
                key[v] = graph[u][v]
                pqueue[v] = graph[u][v]
    return pred
 
graph = {0 : {1:6, 2:8},
1 : {4:11},
2 : {3: 9},
3 : {},
4 : {5:3},
5 : {2: 7, 3:4}}
 
pred = prim(graph, 0)
for v in pred: print "%s: %s" % (v, pred[v])
 
 
"""
python graph.py 
0: -1
1: 0
2: 0
3: 2
4: 1
5: 4
"""         
