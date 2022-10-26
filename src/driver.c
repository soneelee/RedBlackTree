#include "rbtree.h"
#include <stdio.h>
int main(int argc, char *argv[]) {

    rbtree *tree = new_rbtree();

    // key_t arr1[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};

    rbtree_insert(tree, 10);
    rbtree_insert(tree, 5);
    rbtree_insert(tree, 8);
    rbtree_insert(tree, 34);
    rbtree_insert(tree, 67);
    // rbtree_insert(tree, 23);
    // rbtree_insert(tree, 156);
    // rbtree_insert(tree, 24);
    // rbtree_insert(tree, 2);
    // rbtree_insert(tree, 12);
    // rbtree_insert(tree, 24);
    // rbtree_insert(tree, 36);
    // rbtree_insert(tree, 990);
    // rbtree_insert(tree, 25);

    node_t* p = rbtree_find(tree, 8);
    
    rbtree_erase(tree, p);
    rbtree_display(tree, tree->root);



    // printf("%d", tree1 -> root -> key);

    // rbtree_insert(tree, 6);
    // rbtree_insert(tree, 18);
    // rbtree_insert(tree, 3);
    // rbtree_insert(tree, 9);
    // rbtree_insert(tree, 15);
    // rbtree_insert(tree, 21);

    // printf("%d\n", rbtree_find(tree,18)->key);
    // printf("%d\n", rbtree_find(tree,3)->key);
    // printf("%d\n", rbtree_find(tree,9)->key);
    // printf("%d\n", rbtree_find(tree,15)->key);
    // printf("%d\n", rbtree_find(tree,24)->key);
    return 0;

}