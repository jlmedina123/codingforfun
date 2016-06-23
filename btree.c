/****************************************************/                                       
/* Using a Btree structure, read numbers from an    */
/* input file and form a tree. All keys in the nodes*/
/* of the left subtree will be lower than parent key*/
/* and all keys in the right subtree are higher     */
/* than the right subtree. Traverse the tree in     */
/* order after each split. Print the input values   */
/* and the output node key values in order to       */
/* output files                                     */
/* The display and                                  */
/* traverse functions are designed recursively      */
/* to simplify the design                           */
/****************************************************/
 
 
/* Preprocessor directives */
#include<stdio.h>
#include <stdlib.h>
#define M 5
#define FILENAME "input.txt"
#define FILEOUT "output.txt"
 
/* global variables */
struct node{
    int n;             /* number of keys in node */
    int keys[M-1];     /* array of keys */
    struct node *p[M]; /* pointers to children */
};

/* using enumeration for key status */
enum KeyStatus { Duplicate, SearchFailure, Success, InsertIt, LessKeys}; 
typedef struct node NODE;
NODE* root = NULL; /* defining tree's root as global */
FILE *fdout;
 
/* function prototypes */
void insert(int );
void display(NODE *, int, int);
enum KeyStatus insertHelper(NODE *, int , int *, NODE** );
int searchPos(int x, int *key_arr, int n);
void traverse(NODE *);
 
 
/* main function */
int main()
{
    int key;
    /* opening files */
    printf("Opening files %s and %s.\n", FILENAME, FILEOUT);
    FILE *fd = fopen(FILENAME, "r");
    fdout = fopen(FILEOUT, "w");
    if (fd == NULL || fdout == NULL)
    {
         perror("Could not open file");
         return -1;
    }
    /* reading input sequence */
    fscanf (fd, "%d, ", &key);
    fprintf(fdout, "Adding the following elements to tree as read from file %s:\n\n", FILENAME);
    while (fscanf (fd, "%d, ", &key) != EOF)
    {
        fprintf(fdout, "Adding key %d\n", key);
        /* insert key */
        insert(key); 
        fprintf(fdout, "Tree with new key:\n");
        /* display tree */
        display(root, 0, 0);
        fprintf(fdout, "\n");
        fprintf(fdout, "Traversing tree inorder:\n");
        /* traverse tree inorder */
        traverse(root);
        fprintf(fdout, "\n\n");
    }
    close(fd);
    close(fdout);
 
    printf("Sequence has been read from %s, and output written to %s\n", FILENAME, FILEOUT);
 
    return 0;
}/* end of main()*/
 
void insert(int key)
{
    struct node *newnode;
    int upKey;
    enum KeyStatus value;
    value = insertHelper(root, key, &upKey, &newnode); /* returns the key status, upkey, and newnode */
    if (value == Duplicate)
        fprintf(fdout, "Key is already in tree\n");
    if (value == InsertIt)
    {
        struct node *uproot = root;
        root=malloc(sizeof(struct node));
        root->n = 1;
        root->keys[0] = upKey;
        root->p[0] = uproot;
        root->p[1] = newnode;
    }/* end of if */
}/* end of insert()*/
 
enum KeyStatus insertHelper(struct node *ptr, int key, int *upKey, struct node **newnode)
{
    struct node *newPtr, *lastPtr;
    int pos, i, n, splitPos;
    int newKey, lastKey;
    enum KeyStatus value;
    /* inserting in root */
    if (ptr == NULL)
    {
        *newnode = NULL;
        *upKey = key;
        return InsertIt;
    }
    n = ptr->n; /* number of keys in node */
    pos = searchPos(key, ptr->keys, n); /* checking where to insert key in node */
    /* key is duplicate */
    if (pos < n && key == ptr->keys[pos])
        return Duplicate;
    value = insertHelper(ptr->p[pos], key, &newKey, &newPtr);
    if (value != InsertIt)
        return value;
    /*If keys in node is less than M (number of keys in node)*/
    if (n < M - 1)
    {
        pos = searchPos(newKey, ptr->keys, n);
        /*Shifting the key and pointer right for inserting the new key*/
        for (i=n; i>pos; i--)
        {
            ptr->keys[i] = ptr->keys[i-1];
            ptr->p[i+1] = ptr->p[i];
        }
        /*Key is inserted at exact location*/
        ptr->keys[pos] = newKey;
        ptr->p[pos+1] = newPtr;
        ++ptr->n; /*incrementing the number of keys in node*/
        return Success;
    }
    /* keys in node greater than M, so splitting is necessary */
    /*If keys in nodes are maximum and position of node to be inserted is last*/
    if (pos == M - 1)
    {
        lastKey = newKey;
        lastPtr = newPtr;
    }
    else /*If keys in node are maximum and position of node to be inserted is not last*/
    {
        lastKey = ptr->keys[M-2];
        lastPtr = ptr->p[M-1];
        for (i=M-2; i>pos; i--)
        {
            ptr->keys[i] = ptr->keys[i-1];
            ptr->p[i+1] = ptr->p[i];
        }
        ptr->keys[pos] = newKey;
        ptr->p[pos+1] = newPtr;
    }
    splitPos = (M - 1)/2;
    (*upKey) = ptr->keys[splitPos];
 
    (*newnode)=malloc(sizeof(struct node));/* right node after split*/
    ptr->n = splitPos; /* number of keys for left splitted node*/
    (*newnode)->n = M-1-splitPos;/* number of keys for right splitted node*/
    for (i=0; i < (*newnode)->n; i++)
    {
        (*newnode)->p[i] = ptr->p[i + splitPos + 1];
        if(i < (*newnode)->n - 1)
            (*newnode)->keys[i] = ptr->keys[i + splitPos + 1];
        else
            (*newnode)->keys[i] = lastKey;
    }
    (*newnode)->p[(*newnode)->n] = lastPtr;
    fprintf(fdout, "Node has been splitted at key %d\n", ptr->keys[splitPos]);
    return InsertIt;
}/* end of ins() */
 
void display(struct node *nodeptr, int blanks, int childno)
{
    if (nodeptr)
    {
        int i;
        for(i=1; i<=blanks; i++)
            fprintf(fdout, " ");
        if (childno != 0) 
            fprintf(fdout, "child node %d: ", childno);
        else
            fprintf(fdout, "root node: ");
        for (i=0; i < nodeptr->n; i++)
            fprintf(fdout, "%d ", nodeptr->keys[i]);
        fprintf(fdout, "\n");
        for (i=0; i <= nodeptr->n; i++)
            display(nodeptr->p[i], blanks+5, i+1); /* display child */
    }/* end of if */
}/* end of display() */
 
void traverse(struct node *nodeptr)
{
    int i;
    if (nodeptr)
    {
        for (i = 0; i < nodeptr->n; i++) // traverse keys and children in node 
        {
            traverse(nodeptr->p[i]);      // print left subtree from key position 
            fprintf(fdout, "%d, ", nodeptr->keys[i]); // print key 
        }
        //i++; //for loop post-increments i, so i is incremented after leaving loop
        traverse(nodeptr->p[i]); // there is one more child pointer than key
    }
} /* end of traverse */
 
int searchPos(int key, int *key_arr, int n)
{
    int pos=0;
    while (pos < n && key > key_arr[pos])
        pos++;
    return pos;
}/*End of searchPos()*/
