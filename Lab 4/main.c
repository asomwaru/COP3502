#include<stdio.h>
#include<stdlib.h>

typedef struct node{
	int data;
	struct node *next;
} node;

node* insert_front(node *root, int item) {
	node *temp;

	temp= (node *) malloc(sizeof(node));
	temp->data=item;
	temp->next=NULL;
	if(root==NULL) 
        root=temp;
    else {
        temp->next = root;
        root = temp;
    }
  return root;

}

node* insert_end(node* root, int item) {
	node *t;
	node *temp;
	
	temp= (node *) malloc(sizeof(node));
	temp->data=item;
	temp->next=NULL;
	if(root==NULL)
		root=temp;
	else {
		t=root; //t is being used to start traversing
        while(t->next!=NULL) //keep going till the end
            t=t->next;

        t->next=temp; //t is at the last node of the linked list, so add temp after t.
	}
  return root;
}

void display(node* t) {
	while(t!=NULL) {
		printf("%d ",t->data);
		t=t->next;
	}
}

node* reverse_list(node* head) {
    if (head == NULL || head->next == NULL)
        return head;

    node* temp = NULL;
    node* reversed = head;
    node* current = head->next;

    reversed->next = NULL;

    while (current != NULL) {
        temp = current;
        current = current->next;

        temp->next = reversed;
        
        reversed = temp;
    }

    return reversed;
}

void insertToPlace(node* head, int val, int place) {
    node* current = head;

    int i=1;
    while (current->next != NULL && i!=place-1) {
        current = current->next;
        i++;
    }

    node* temp = (node*)malloc(sizeof(node));
    temp->data = val;

    temp->next = current->next;
    current->next = temp;
}

int main() {
    node *root=NULL; //very important line. Otherwise all function will fail
	node *t;
	int ch,ele,v, del;
	while(1) {
		printf("\nMenu: 1. insert at front, 2. reverse list 3. Insert to place 0. exit: ");
	    scanf("%d",&ch);
		
        if(ch == 0) {
			printf("\nGOOD BYE>>>>\n");
			break;
		}

		if(ch == 1) {
			printf("\nEnter data(an integer): ");
			scanf("%d",&ele);
			root = insert_front(root, ele);

            display(root);
		}

		if(ch == 2) {
			root = reverse_list(root);
            display(root);
		}

        if(ch == 3) {
            printf("\nEnter the value you want to insert: ");
            scanf("%d", &v);

            printf("\nEnter the place you want to insert it: ");
            scanf("%d", &ele);

            if (ele <= 1) {
                printf("\nInvalid input\n");
            } else {
                insertToPlace(root, v, ele);
                display(root);
            }
        }
	}

    return 0;
}
