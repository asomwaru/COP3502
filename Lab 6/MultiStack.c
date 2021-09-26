#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define SIZE 100
#define EMPTY -1


typedef struct stack {
    char items[SIZE];
    int top;
} stack;


void initialize(struct stack* stackPtr);
int full(struct stack* stackPtr);
int push(struct stack* stackPtr, char value);
int empty(struct stack* stackPtr);
char pop(struct stack* stackPtr);
char top(struct stack* stackPtr);
void display(struct stack* stackPtr);


int checkBalance(char exp[]);
char* infix_to_postfix(char infix[]);
int isParenthese(char ch);
int isOperator(char ch);
int priority(char ch);


int main() {
    stack stack1;
    char exp[SIZE];

    printf("Enter an expression\n");
    scanf("%[^\n]s", exp);

    printf("Your expression: %s\n", exp);
    if (!checkBalance(exp)) {
        printf("Parenthese imbalanced\n");
        return 0;
    }

    char* post = infix_to_postfix(exp);
    printf("Your expression in postfix is:\n%s\n", post);
    free(post);
    
    return 0;
}

int checkBalance(char exp[]) {
    stack s;
    initialize(&s);

    for (int i=0; exp[i] != '\0'; i++) {
        if (exp[i] == '(' || exp[i] == '[' || exp[i] == '{') {
            push(&s, exp[i]);
        } else if (exp[i] == ')' || exp[i] == ']' || exp[i] == '}') {
            char popped = pop(&s);

            if (exp[i] == ')' && popped != '(') {
                return 0;
            } else if (exp[i] == ']' && popped != '[') {
                return 0;
            } else if (exp[i] == '}' && popped != '{') {
                return 0;
            }
        } 
    }

    return empty(&s);
}


char* infix_to_postfix(char infix[]) {
    char* postfix = (char*)malloc(sizeof(char) * strlen(infix) + 1);
    int pf_index = 0;

    stack op;
    initialize(&op);

    for (int i=0; infix[i] != '\0'; i++) {
        if (isdigit(infix[i])) {
            while (isdigit(infix[i])) {
                postfix[pf_index] = infix[i];
                pf_index++;
                i++;
            }

            i--;
            postfix[pf_index] = ' ';
            pf_index++;

        } else if (isOperator(infix[i])) {
            if (empty(&op)) {
                push(&op, infix[i]);

            } else {
                char top_op = top(&op);
                if (priority(infix[i]) > priority(top_op)) {
                    push(&op, infix[i]);

                } else {
                    while (!empty(&op) && !isParenthese(top_op) && priority(infix[i]) <= priority(top_op)) {
                        postfix[pf_index] = pop(&op);
                        pf_index++;

                        postfix[pf_index] = ' ';
                        pf_index++;

                        top_op = top(&op);
                    }

                    push(&op, infix[i]);
                }
            }

        } else if (infix[i] == ')') { 
            while (top(&op) != '(') {
                postfix[pf_index] = pop(&op);
                pf_index++;

                postfix[pf_index] = ' ';
                pf_index++;
            }

            pop(&op);
        }
    }

    if (!empty(&op)) {
        while (!empty(&op)) {
            postfix[pf_index] = pop(&op);
            pf_index++;

            postfix[pf_index] = ' ';
            pf_index++;
        }
    }

    return postfix;
}

int isParenthese(char ch) {
    return (ch == '(' || ch == ')');
}

int isOperator(char ch) {
    return (ch == '+' || ch == '-' || ch == '/' || ch == '*' || ch == '^' || ch == '%' || ch == '(');
}

int priority(char ch) {
    if (ch == '^') return 3;
    if (ch == '/' || ch == '*' || ch == '%') return 2;
    if (ch == '+' || ch == '-') return 1;

    if (ch == '{' || ch == '[' || ch == '(') return 3;
}


void initialize(struct stack* stackPtr) {
    // stackPtr->items = (char*)malloc(sizeof(char));
    stackPtr->top = -1;
}

// If the push occurs, 1 is returned. If the
// stack is full and the push can't be done, 0 is
// returned.
int push(struct stack* stackPtr, char value) {

    // Check if the stack is full.
    if (full(stackPtr))
        return 0;

    // Add value to the top of the stack and adjust the value of the top.
    stackPtr->items[stackPtr->top+1] = value;
    (stackPtr->top)++;
    return 1;
}

// Returns true iff the stack pointed to by stackPtr is full.
int full(struct stack* stackPtr) {
    return (stackPtr->top == SIZE - 1);
}

// Returns true iff the stack pointed to by stackPtr is empty.
int empty(struct stack* stackPtr) {
    return (stackPtr->top == -1);
}

// Pre-condition: The stack pointed to by stackPtr is NOT empty.
// Post-condition: The value on the top of the stack is popped and returned.
// Note: If the stack pointed to by stackPtr is empty, -1 is returned.
char pop(struct stack* stackPtr) {

    int retval;

    // Check the case that the stack is empty.
    if (empty(stackPtr))
        return EMPTY;

    // Retrieve the item from the top of the stack, adjust the top and return
    // the item.
    retval = stackPtr->items[stackPtr->top];
    (stackPtr->top)--;
    return retval;
}

// Pre-condition: The stack pointed to by stackPtr is NOT empty.
// Post-condition: The value on the top of the stack is returned.
// Note: If the stack pointed to by stackPtr is empty, -1 is returned.
char top(struct stack* stackPtr) {

    // Take care of the empty case.
    if (empty(stackPtr))
        return EMPTY;

    // Return the desired item.
    return stackPtr->items[stackPtr->top];
}

void display(struct stack* stackPtr) {
    printf("\nPrinting the Current stack...");
    for(int i=0; i<=stackPtr->top; i++)
        printf("%c ", stackPtr->items[i]);
}
