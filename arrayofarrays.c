int main(void) {
    int ar[3][4] = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}};
 
    /* 'ar' is an array of arrays. The + and - operators are
     * overloaded, meaning that ptr + value gets replaced by
     * compiler by ptr + sizeof(*ptr)*value
     * This means that in this case a + value gets replaced
     * by ar + sizeof(*ar)*value = ar + sizeof(ar*)*value =
     * ar + sizeof(int ar[4])*value
     *
     * Incrementing ar moves ar to the second element in array
     */
 
    unsigned long addr = (unsigned long)ar;
 
    printf("%d %d %d %d \n", **ar,
                             *(int *)(addr + sizeof(int)),
                             *(int *)(addr + 2*sizeof(int)),
                             *(int *)(addr + 3*sizeof(int)));
 
    addr = (unsigned long)(ar+1); // accessing second element in list
    printf("%d %d %d %d\n", **(ar+1),
                            *(int *)(addr + sizeof(int)),
                            *(int *)(addr + 2*sizeof(int)),
                            *(int *)(addr + 3*sizeof(int)));
    return 0;
}
 
