def stairs_global(steps):
    global n
    if steps == 0:
        n+=1
        return
    if steps < 0:
        return
    stairs_global(steps-1)
    stairs_global(steps-2)
    stairs_global(steps-3)
 
def stairs(steps):
    if steps < 0:
        return 0
    elif steps == 0:
        return 1;
    else:
        return stairs(steps-1) + stairs(steps-2) + stairs(steps-3)
 
for i in range(10):
    n = 0
    stairs_global(i)
    print "%d steps: %d ways" % (i, n)
 
for i in range(10):
    print "%d steps: %d ways" % (i, stairs(i))
