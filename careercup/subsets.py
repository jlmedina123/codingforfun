def allsubsets_yield(seq):
    if len(seq) == 0:
         yield []
    else:
        head = seq[0]
        for tail in allsubsets_yield(seq[1:]):
            yield [head] + tail
            yield tail
 
def allsubsets(seq):
    if len(seq) <= 0:
        return [seq]
    results = []
    head = seq[0]
    tail = seq[1:]
    subsets = allsubsets(tail)
    for s in subsets:
        results.append([head] + s)
        results.append(s)
    return results
 
a = [1,2,3]
print allsubsets(a)
for i in allsubsets_yield(a): print i,
 
 
 
''' 
python powerset.py 
[[1, 2, 3], [2, 3], [1, 3], [3], [1, 2], [2], [1], []]
[1, 2, 3] [2, 3] [1, 3] [3] [1, 2] [2] [1] []
'''
