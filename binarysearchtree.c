#include <stdio.h>
#include <stdlib.h>

struct node {
    struct node *left;
    struct node *right;
    int value;
};

int insert(struct node **tree, int value) {
    struct node *new = calloc(sizeof(struct node), 1);
    new->value = value;
    if (!*tree) {
        *tree = new;
        return;
    }
    struct node *p = *tree;
    while (p) {
        if (p->value > value) {
            if (p->left)
                p = p->left;
            else {
                p->left = new;
                return;
            }
        } else if (p->value  < value) {
            if (p->right)
                p = p->right;
            else {
                p->right = new;
                return;
            }
        } else { 
            printf("Duplicate\n"); 
            return; 
        }
    }
}

struct node * findnode(struct node *tree, int value) {
    if (!tree)
        return NULL;
    struct node *p = tree;
    while (p) {
        if (p->value == value)
            return p;
        else if (p->value > value)
            p = p->left;
        else
            p = p->right;
    }
    printf("Couldnt find node\n");
    return NULL;
}

/* tree traversal:
    - Depth-first search: search tree is deepened as much as possible
    before visiting the next child
        * inorder: gets values in order
        * preorder
        * postorder
    - Breadth-first earch: traversed in level-order. Visit every node
    on a level before going to next level

*/

void dfs_inorder(node_t *node) {
    if (node == NULL) 
        return;
    traverse_inorder(node->left);   // left
    printf("%d, ", node->key);      // current
    traverse_inorder(node->right);  // right
}
 
void dfs_preorder(node_t *node) {
    if (node == NULL)
        return;
    printf("%d, ", node->key);       // current
    traverse_preorder(node->left);   // left
    traverse_preorder(node->right);  // right
}
 
void dfs_postorder(node_t *node) {
    if (node == NULL)
        return;
    traverse_postorder(node->left);    // left
    traverse_postorder(node->right);   // right
    printf("%d, ", node->key);         // current    

void bfs(node_t *node) {
}


int main() {
    struct node * tree = NULL;
    int a[] = {5, 2, 7, 3, 1, 6, 9};
    int i;
    for (i = 0; i < 7; i++) {
        insert(&tree, a[i]);
        print(tree);
    }
    return 0;
}


