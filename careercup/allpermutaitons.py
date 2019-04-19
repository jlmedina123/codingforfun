
def allpermutations_yield(word):
    if len(word) <= 1:
        yield word
    else:
        head = word[0]
        tail = word[1:]
        for permutation in allpermutations_yield(tail):
            for i in range(len(word)):
                yield permutation[:i] + head + permutation[i:]
 
 
def allpermutations(word):
    if len(word) <= 1:
        return [word]
    head = word[0]
    tail = word[1:]
    perms = allpermutations(tail)
    result = []
    for perm in perms:
        for i in range(len(word)):
            result.append(perm[:i] + head + perm[i:]) # use if input (word) is a string
            # result.append(perm[:i] + [head] + perm[i:]) # use if input is list
    return result
 
a = "asdf"
for per in allpermutations_yield("asdf"):
    print "%s, " % per,
print
 
print allpermutations(a)
