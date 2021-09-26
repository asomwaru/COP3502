#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leak_detector_c.h"
#define MAXLEN 30


typedef struct itemNode {
    char name[MAXLEN];   
    int count;   
    struct itemNode *left, *right; 
} itemNode;

typedef struct treeNameNode {   
    char treeName[MAXLEN];   
    struct treeNameNode *left, *right;   
    itemNode *theTree; 
} treeNameNode;


// required functions
treeNameNode* createTreeNameNode(treeNameNode* root, char name[MAXLEN]);
treeNameNode* buildNameTree(treeNameNode* root, itemNode* item);
void traverse_in_traverse(treeNameNode *root, FILE* out);
treeNameNode * searchNameNode(treeNameNode* root, char treeName[50]);

// helper functions
itemNode* build_item_tree(itemNode* root, itemNode* insertNode);
itemNode* create_item_node(char animal[MAXLEN], int count);

void inorder_nameTree(treeNameNode* root, FILE* out);
void inorder_itemTree(itemNode* root, FILE* out);

void free_name_tree(treeNameNode* root);
void deforest(treeNameNode* root);
void free_tree(itemNode* root);

// query functions and helpers
itemNode* search_item(itemNode* root, char tree_name[MAXLEN]);
int search_item_before(itemNode* root, char item_name[MAXLEN], int* count);

int height_balance(itemNode* root);
int max(int a, int b);

int get_count(itemNode* root);

itemNode* min_itemNode(itemNode* node);
itemNode* delete_itemNode(itemNode* root, char key[MAXLEN]);

treeNameNode* min_nameTree(treeNameNode* node);
treeNameNode* delete_nameTree(treeNameNode* root, char key[MAXLEN]);


int main() {
    atexit(report_mem_leak);
    FILE* in = fopen("in.txt", "r");
    FILE* out = fopen("out.txt", "w");
    
    int treeNameNum, insertItems, queries;
    treeNameNode* root = NULL;

    fscanf(in, "%d %d %d", &treeNameNum, &insertItems, &queries);

    // read in the name trees
    for (int i=0; i<treeNameNum; i++) {
        char name[MAXLEN];

        fscanf(in, "%s", name);

        root = createTreeNameNode(root, name);
    }

    // print out the trees in-order
    inorder_nameTree(root, out);
    fprintf(out, "\n");

    // read in the item node names with their count
    for (int i=0; i<insertItems; i++) {
        char treeName[MAXLEN], animal[MAXLEN];
        int population;

        fscanf(in, "%s %s %d", treeName, animal, &population);

        treeNameNode* name = searchNameNode(root, treeName);
        itemNode* temp = create_item_node(animal, population);

        name = buildNameTree(name, temp);
    }

    // print out the name trees and its item tree in-order
    traverse_in_traverse(root, out); 

    // read in the queries
    for (int i=0; i<queries; i++) {
        char command[MAXLEN], treeName[MAXLEN], delimiter[MAXLEN];

        // since the commands have varying arguments, we read in the command first
        fscanf(in, "%s", command);

        if (strcmp("search", command) == 0) {
            // read in the tree name and item node
            fscanf(in, "%s %s", treeName, delimiter);
            treeNameNode* name_tree = searchNameNode(root, treeName);

            // make sure that the name tree isn't null
            if (name_tree == NULL) {
                fprintf(out, "%s does not exist\n", treeName);
                continue;
            }

            itemNode* item = search_item(name_tree->theTree, delimiter);

            if (item != NULL) {
                fprintf(out, "%d %s found in %s\n", item->count, item->name, name_tree->treeName);
            } else {
                fprintf(out, "%s not found in %s\n", delimiter, treeName);
            }


        } else if (strcmp("item_before", command) == 0) {
            fscanf(in, "%s %s", treeName, delimiter);
            treeNameNode* tree = searchNameNode(root, treeName);

            // initialize the count before passing it to the item_before function
            int count = 0;

            search_item_before(tree->theTree, delimiter, &count);

            fprintf(out, "item before %s: %d\n", delimiter, count);
            
        } else if (strcmp("height_balance", command) == 0) {
            fscanf(in, "%s", treeName);
            treeNameNode* tree = searchNameNode(root, treeName);

            // get the balance of the right and left 
            int left, right, balanced;
            left = height_balance(tree->theTree->left);
            right = height_balance(tree->theTree->right);

            balanced = right - left;

            if (balanced > 1) {
                fprintf(out, "%s: left height %d, right height %d, difference %d, not balanced\n", tree->treeName, left, right, balanced);
            } else {
                fprintf(out, "%s: left height %d, right height %d, difference %d, balanced\n", tree->treeName, left, right, balanced);
            }

        } else if (strcmp("count", command) == 0) {
            fscanf(in, "%s", treeName);
            treeNameNode* tree = searchNameNode(root, treeName);

            // get the total count of the tree, we pass in the item tree because the function is recursive
            int count = get_count(tree->theTree);

            fprintf(out, "%s count %d\n", treeName, count);

        } else if (strcmp("delete", command) == 0) {
            fscanf(in, "%s %s", treeName, delimiter);
            treeNameNode* name = searchNameNode(root, treeName);

            // delete the item node by passing in the root of the item tree
            name->theTree = delete_itemNode(name->theTree, delimiter);
            
            fprintf(out, "%s deleted from %s\n", delimiter, treeName);

        } else if (strcmp("delete_name", command) == 0) {
            fscanf(in, "%s", treeName);
            treeNameNode* name = searchNameNode(root, treeName);

            root = delete_nameTree(root, treeName);

            fprintf(out, "%s deleted\n", treeName);

        } else if (strcmp("reduce", command) == 0) {
            int val;
            fscanf(in, "%s %s %d", treeName, delimiter, &val);
            treeNameNode* name = searchNameNode(root, treeName);

            // get the item node to check if we need to delete or change the value of the item node
            itemNode* item = search_item(name->theTree, delimiter);

            if ((item->count - val) <= 0) {
                name->theTree = delete_itemNode(name->theTree, delimiter);
            } else {
                item->count = item->count - val;
            }

            fprintf(out, "%s reduced\n", delimiter);
        } 
    }

    // we delete all of the name trees and its item trees
    deforest(root);

    fclose(in);
    fclose(out);
    return 0;
}


treeNameNode* createTreeNameNode(treeNameNode* root, char name[MAXLEN]) {
    if (root == NULL) {
        root = (treeNameNode*)malloc(sizeof(treeNameNode));
        strcpy(root->treeName, name);

        // set the sub tree and its item tree to null
        root->left = NULL;
        root->right = NULL;
        root->theTree = NULL;

        return root;
    }

    // follow the path of the name tree to insert the new name node
    if (strcmp(name, root->treeName) > 0) {
        root->right = createTreeNameNode(root->right, name);
    } else {
        root->left = createTreeNameNode(root->left, name);
    }

    return root;
}

void inorder_nameTree(treeNameNode* root, FILE* out) {
    // print out the tree using the method in-order for it to be alphabetical
    if (root != NULL) {
        inorder_nameTree(root->left, out);
        fprintf(out, "%s ", root->treeName);
        inorder_nameTree(root->right, out);
    }
}


treeNameNode* searchNameNode(treeNameNode* root, char treeName[50]) {
    if (root == NULL) {
        // in case we get to a dead end
        return NULL;
    }

    // follow the path of the name tree recursively
    if (strcmp(treeName, root->treeName) == 0) {
        return root;
    } else if (strcmp(treeName, root->treeName) > 0) {
        return searchNameNode(root->right, treeName);
    } else {
        return searchNameNode(root->left, treeName);
    }
}

treeNameNode* buildNameTree(treeNameNode* root, itemNode* item) {
    if (root->theTree == NULL) {
        // ensure we don't have an empty item tree
        root->theTree = item;

        return root;
    }

    // use helper function to insert the new item node
    root->theTree = build_item_tree(root->theTree, item);

    return root;
}

itemNode* build_item_tree(itemNode* root, itemNode* insertNode) {
    if (root == NULL) {
        // if we get to the end of the path then we add the insertNode
        root = insertNode;

        return root;
    }

    // follow the path of the item tree
    if (strcmp(insertNode->name, root->name) > 0) {
        root->right = build_item_tree(root->right, insertNode);
    } else {
        root->left = build_item_tree(root->left, insertNode);
    }

    return root;
}

itemNode* create_item_node(char animal[MAXLEN], int count) {
    // create an itemNode
    itemNode* item = (itemNode*)malloc(sizeof(itemNode));

    strcpy(item->name, animal);
    item->left = NULL;
    item->right = NULL;

    item->count = count;

    return item;
}


void traverse_in_traverse(treeNameNode *root, FILE* out) {
    // print out the name tree and its item tree in-order
    if (root != NULL) {
        traverse_in_traverse(root->left, out);
        fprintf(out, "===%s===\n", root->treeName);

        inorder_itemTree(root->theTree, out);
        fprintf(out, "\n");

        traverse_in_traverse(root->right, out);
    }
}

void inorder_itemTree(itemNode* root, FILE* out) {
    // print out the item tree in-order
    if (root != NULL) {
        inorder_itemTree(root->left, out);
        fprintf(out, "%s ", root->name);
        inorder_itemTree(root->right, out);
    }
}


itemNode* search_item(itemNode* root, char tree_name[MAXLEN]) {
    if (root == NULL) {
        return NULL;
    }

    // follow the path of the item tree to find the item node
    if (strcmp(tree_name, root->name) == 0) {
        return root;

    } else if (strcmp(tree_name, root->name) > 0) {
        return search_item(root->right, tree_name);

    } else {
        return search_item(root->left, tree_name);
    }
}

int search_item_before(itemNode* root, char item_name[MAXLEN], int* count) {
    if (root != NULL) {
        int left = search_item_before(root->left, item_name, count);

        // make sure that the node is not the node we're looking for or that it is bigger than the value we want
        if (strcmp(root->name, item_name) < 0) {
            *count += 1;
        }

        search_item_before(root->right, item_name, count);
    }
}

int height_balance(itemNode* root) {
    if (root == NULL) {
        return -1;
    }

    int left, right;

    // get balance of left and right sub-trees
    left = height_balance(root->left);
    right = height_balance(root->right);

    // get biggest sub-tree height + 1
    return max(left, right) + 1;
}

int max(int a, int b) { 
    // get the maximum out of the 2 values
    return (a >= b) ? a : b; 
} 


int get_count(itemNode* root) {
    // continually add the left and right child nodes and return the total count
    if (root == NULL) {
        return 0;
    }

    return root->count + get_count(root->left) + get_count(root->right);
}


itemNode* min_itemNode(itemNode* node) {
    itemNode* current = node;

    // travel along the left side to get the smallest value possible for the item tree
    while (current && current->left != NULL)
        current = current->left;
 
    return current;
}

itemNode* delete_itemNode(itemNode* root, char key[MAXLEN]) {
    if (root == NULL) {
        return root;
    }
 
    if (strcmp(key, root->name) < 0) {
        root->left = delete_itemNode(root->left, key);

    } else if (strcmp(key, root->name) > 0) {
        root->right = delete_itemNode(root->right, key);

    } else {
        // check if the tree has only one child node
        if (root->left == NULL) {
            itemNode* temp = root->right;
            free(root);
            return temp;

        } else if (root->right == NULL) {
            itemNode* temp = root->left;
            free(root);
            return temp;
        }

        // continually find the smallest leaf node to replace the item node with
        itemNode* temp = min_itemNode(root->right);

        // copy over the values from smallest value to root
        strcpy(root->name, temp->name);
        root->count = temp->count;
        
        root->right = delete_itemNode(root->right, temp->name);
    }

    return root;
}


treeNameNode* delete_nameTree(treeNameNode* root, char key[MAXLEN]) {
    // same code but adjusted for name tree instead of item tree
    if (root == NULL) {
        return root;
    }
 
    if (strcmp(key, root->treeName) < 0) {
        root->left = delete_nameTree(root->left, key);

    } else if (strcmp(key, root->treeName) > 0) {
        root->right = delete_nameTree(root->right, key);

    } else {
        // check if the tree has only one child node
        if (root->left == NULL) {
            treeNameNode* temp = root->right;
            free_name_tree(root);
            return temp;

        } else if (root->right == NULL) {
            treeNameNode* temp = root->left;
            free_name_tree(root);
            return temp;
        }

        // continually find the smallest leaf node to replace the name node with
        treeNameNode* temp = min_nameTree(root->right);
 
        // copy over the values from smallest value to root
        strcpy(root->treeName, temp->treeName);
        
        root->right = delete_nameTree(root->right, temp->treeName);
    }

    return root;

}

treeNameNode* min_nameTree(treeNameNode* node) {
    treeNameNode* current = node;
 
    // travel along the left side to get the smallest value possible for the item tree
    while (current && current->left != NULL) {
        current = current->left;
    }
 
    return current;
}



void deforest(treeNameNode* root) {
    // use post-order to delete the leaf and sub trees before removing the parent tree
    if (root != NULL) {
        deforest(root->left);
        deforest(root->right);
        free_name_tree(root);
    }
}

void free_name_tree(treeNameNode* root) {
    // free up item tree before freeing up name tree
    free_tree(root->theTree);
    free(root);
}

void free_tree(itemNode* root) {
    // free up item tree using post-order method
    if (root != NULL) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}
