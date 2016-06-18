#include <stdio.h>
#include <stdlib.h>
 
struct node {
    struct node *left;
    struct node *right;
    int key;
};
typedef struct node node_t;
 
node_t * alloc_node(int key) {
    node_t *node = malloc(sizeof(node_t));
    node->left = NULL;
    node->right = NULL;
    node->key = key;
    return node;
}
 
node_t * init_tree(int key) {
    node_t *root = alloc_node(key);
    return root;
}
 
void insert(node_t *root, int key) {
    node_t *newnode = alloc_node(key);
    node_t *ptr = root;
    while (ptr != NULL) {
        if (ptr->key == key) {
            printf("key %d is already in tree\n", key);
            free(ptr);
            return;
        }
        if (ptr->key > key) {
            if (ptr->left == NULL) { 
                ptr->left = newnode;
                return;
            }
            ptr = ptr->left;
        }
        else if (ptr->key < key) {
             if (ptr->right == NULL) {
                 ptr->right = newnode;
                 return;
             }
             ptr = ptr->right;
        }
    }
    printf("Failed to insert key %d\n", key);
}
 
int find_key(node_t *root, int key) {
    node_t *ptr = root;
    while (ptr != NULL) {
        if (ptr->key == key) 
            return ptr->key;
        else if (ptr->key > key) 
            ptr = ptr->left;
        else
            ptr = ptr->right;
    }
    printf("cannot find key %d\n", key);
    return 0;
}
  
void traverse_inorder(node_t *node) {
    if (node == NULL) 
        return;
    traverse_inorder(node->left);   // left
    printf("%d, ", node->key);      // current
    traverse_inorder(node->right);  // right
}
 
void traverse_preorder(node_t *node) {
    if (node == NULL)
        return;
    printf("%d, ", node->key);       // current
    traverse_preorder(node->left);   // left
    traverse_preorder(node->right); // right
}
 
void traverse_postorder(node_t *node) {
    if (node == NULL)
        return;
    traverse_postorder(node->left);    // left
    traverse_postorder(node->right);   // right
    printf("%d, ", node->key); // current
}
 
int maxdepth(node_t *node) {
    if (node == NULL) 
        return 0;
    int nleft = maxdepth(node->left);
    int nright = maxdepth(node->right);
    if (nleft>nright)
        return 1 + nleft;
    return 1 + nright;
}
 
int mindepth(node_t *node) {
    if (node == NULL) 
        return 0;
    int nleft = mindepth(node->left);
    int nright = mindepth(node->right);
    if (nleft>nright)
        return 1 + nright;
    return 1 + nleft;
}
 
int isbalanced(node_t *root) {
    int min = mindepth(root);
    int max = maxdepth(root);
    if (max - min > 1)
        return 0;
    else
        return 1;
}
 
 
int main(int argc, int **argv) {
    int i;
    int keys[] = {5, 4, 8, 6, 1, 43, 6};
    node_t *tree = init_tree(3);
    /* inserting keys */
    for (i = 0; i < 7; i++) {
        printf("Inserting key %d\n", keys[i]);
        insert(tree, keys[i]);
    }
    /* finding keys */
    printf("Finding key %d: %d\n", 43, find_key(tree, 43));
    printf("Finding key %d: %d\n", 15, find_key(tree, 15));
 
    /* traversing tree */
    traverse_inorder(tree); printf("\n"); 
    traverse_preorder(tree); printf("\n");
    traverse_postorder(tree); printf("\n");
 
    /* depth */
    printf("Depth of shortest branch: %d\n", mindepth(tree));
    printf("Depth of longest branch: %d\n", maxdepth(tree));
    if (isbalanced(tree) == 0) 
        printf("Tree unbalanced\n");
    else
        printf("Tree balanced\n");
  
    /* check if tree A is subtree of tree B */
    // just put A's keys in array inorder, and same for B
    // if A is subtree, inorder keys must be subarray of B
 
  
 
   return 0;
}
