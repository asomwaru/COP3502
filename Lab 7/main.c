#include <stdio.h>
#include <stdlib.h>


typedef struct node {
    struct node* next;
    int val;
} node;


node* insert(node* current, int val);
node* del(node* current, int val);
void print(node* current);
void freeList(node* current);
void copy(node* source, node** dest);


int main() {
    int number = 0, choice = 0;

    node* pList = NULL;
    node* nList = NULL;

    while (choice != 4) {
        printf("Do you want to (1)insert, (2)delete, (3)Copy (4)quit.\n");         
        scanf("%d", &choice);        
        printf("Your choice is %d\n", choice);

        if (choice == 1) {
            printf("Enter the value to insert\n");             
            scanf("%d", &number);            
            pList = insert(pList, number); // Look at the list.     

            printf("Items in linked list: ");            
            print(pList);            
            printf("\n");

        } else if (choice == 2) {
            printf("Enter the value to delete.\n");   // Get the number.            
            scanf("%d", &number);            
            pList = del(pList, number);

            printf("Items in linked list: ");            
            print(pList);            
            printf("\n");

        } else if (choice == 3) {
            if (nList)
                freeList(nList);

            copy(pList, &nList);

            printf("Items in NEW linked list: ");            
            print(nList);            
            printf("\n");

        } else {
            break;
        }
    }

    freeList(nList);    
    freeList(pList);    
    printf("\nBye..\n");

    return 0;
}

node* insert(node* current, int val) {
    if (current == NULL || current->val >= val) {
        node* temp = (node*)malloc(sizeof(node));
        temp->val = val;
        temp->next = current;

        return temp;
    }

    current->next = insert(current->next, val);
    return current;
}

node* del(node* current, int val) {
    if (current == NULL) {
        return NULL;
    } else if (current->val == val) {
        node* temp = current->next;
        free(current);

        return temp;
    }

    current->next = del(current->next, val);
    return current;
}

void print(node* current) {
    if (current == NULL) {
        printf("\n");
    } else {
        printf("%d ", current->val);
        print(current->next);
    }
}

void freeList(node* current) {
    while (current != NULL) {
        current = del(current, current->val);
    }
}

void copy(node* source, node** dest) {
    if (source != NULL) {
        (*dest) = malloc(sizeof(node));
        (*dest)->val = source->val;
        (*dest)->next = NULL;

        copy(source->next, &((*dest)->next));
    }
}
