#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leak_detector_c.h"


typedef struct Soldier {
    int sequenceNumber;

    struct Soldier* next;
    struct Soldier* previous;
} Soldier;

typedef struct Queue {
    int id;
    // n, k, th
    int n_Soldiers, jump, threshold;
    char* name;

    struct Soldier* front;
    struct Soldier* end;
} Queue;


// Basic queue functions
void enqueue(Queue* q, Soldier* s);
int dequeue(Queue* q);
Soldier* peek(Queue* q);
int isempty(Queue* q);

// required functions
void display(FILE* out, Queue* q); // show the queue
void rearrange_circle(Queue* q); // reverse list
Soldier* createSoldier(int sequence); // creates soldier
void create_reverse_circle(Queue *q); // create descending list

// helper functions
void selection_sort(Queue** arr, int length);
Queue* create_ground(int id, char* name, int n, int k, int th);
void free_queue(Queue* q);

// phase1 functions
void phase1_exec(FILE* out, Queue* ground);
int get_next_prisoner(Queue* ground, int pos, int jump);
void remove_prisoner(Queue* ground, int prisoner);
void move_head(Queue* q, int value);

// phase2 functions
void phase2_exec(FILE* out, Queue** grounds, int length);
int empty_count(Queue** grounds, int length);
int get_highest_prisoner(Queue** grounds, int length);
int duplicates(Queue** grounds, int length, int skip);
int smallest_ground(Queue** grounds, int length, int value);
void reorder_queue(Queue* ground);


int main() {
    atexit(report_mem_leak);
    FILE* in = fopen("in.txt", "r");
    FILE* out =  fopen("out.txt", "w");

    int len_grounds;
    fscanf(in, "%d", &len_grounds);

    Queue** grounds = (Queue**)malloc(sizeof(Queue*) * len_grounds);

    for (int i=0; i<len_grounds; i++) {
        int num, jump, thresh, id;
        char temp[50];

        fscanf(in, "%d %s %d %d %d", &id, temp, &num, &jump, &thresh);
        grounds[i] = create_ground(id, temp, num, jump, thresh);

        create_reverse_circle(grounds[i]);
    }

    // sort the ground indexes
    selection_sort(grounds, len_grounds);

    fprintf(out, "Initial nonempty lists status\n");
    for (int i=0; i<len_grounds; i++) {
        display(out, grounds[i]);
    }

    fprintf(out, "\nAfter ordering nonempty lists status\n");

    for (int i=0; i<len_grounds; i++) {
        rearrange_circle(grounds[i]);
        display(out, grounds[i]);
    }

    fprintf(out, "\nPhase1 execution\n\n");

    for (int i=0; i<len_grounds; i++) {
        phase1_exec(out, grounds[i]);
        fprintf(out, "\n");
        reorder_queue(grounds[i]);
    }

    fprintf(out, "Phase2 execution\n");

    phase2_exec(out, grounds, len_grounds);

    for (int i=0; i<len_grounds; i++) {
        free_queue(grounds[i]);
    }

    free(grounds);

    fclose(in);
    fclose(out);
    return 0;
}


// helper/misc functions
void selection_sort(Queue** arr, int length) {
    // i chose this algorithm because it was easiest to implement imo
    for (int i=0; i<length; i++) {
        int min = i;
        for (int j=i+1; j<length; j++) {
            if (arr[j]->id < arr[min]->id) {
                min = j;
            }
        }

        if (min != i) {
            Queue* temp;

            temp = arr[min];
            arr[min] = arr[i];
            arr[i] = temp;
        }
    }
}

void free_queue(Queue* q) {
    // free up memory
    if (!isempty(q)) {
        Soldier* current = q->front;

        while (current != q->front) {
            current = current->next;
            free(current->previous);
        }

        free(current);
    }

    free(q->name);
    free(q);
}


// phase2 functions
void phase2_exec(FILE* out, Queue** grounds, int length) {
    // we execute soldiers until there are 1 ground left
    while (empty_count(grounds, length) != length-1) {
        int gd_index = get_highest_prisoner(grounds, length);

        if (duplicates(grounds, length, gd_index)) {
            gd_index = smallest_ground(grounds, length, peek(grounds[gd_index])->sequenceNumber);
        }

        Soldier* prisoner = peek(grounds[gd_index]);

        fprintf(out, "Executed Soldier %d from line %d\n", prisoner->sequenceNumber, grounds[gd_index]->id);
        
        dequeue(grounds[gd_index]);
    }

    // get the final ground index
    int final_index = 0;
    for (int i=0; i<length; i++) {
        if (!isempty(grounds[i])) {
            final_index = i;
            break;
        }
    }

    // this is if the final ground has one soldier left
    Soldier* current = grounds[final_index]->front;
    if (grounds[final_index]->front == grounds[final_index]->end) {
        fprintf(out, "\nSoldier %d from line %d will survive", current->sequenceNumber, grounds[final_index]->id);
        dequeue(grounds[final_index]);

        return;
    }

    // go through the final ground executing each soldier
    while (current != grounds[final_index]->end) {
        current = current->next;
        fprintf(out, "Executed Soldier %d from line %d\n", current->previous->sequenceNumber, grounds[final_index]->id);
        free(current->previous);
    }

    fprintf(out, "\nSoldier %d from line %d will survive", current->sequenceNumber, grounds[final_index]->id);
    free(current);
}

int smallest_ground(Queue** grounds, int length, int value) {
    // get the smallest ground id if we have duplicates in the front
    int index = 11;

    for (int i=0; i<length; i++) {
        if (isempty(grounds[i]))
            continue;

        if (peek(grounds[i])->sequenceNumber == value && index == 11)
            index = i;

        if (peek(grounds[i])->sequenceNumber == value && grounds[i]->id <= grounds[index]->id)
            index = i;
    }

    return index;
}

int duplicates(Queue** grounds, int length, int skip) {
    // skip is the index of the ground we're comparing
    // make sure there's no extra values given 

    Soldier* current = peek(grounds[skip]);

    for (int i=0; i<length; i++) {
        if (i == skip || isempty(grounds[i]))
            continue;

        if (peek(grounds[i])->sequenceNumber == current->sequenceNumber)
            return 1;
    }

    return 0;
}

int get_highest_prisoner(Queue** grounds, int length) {
    // get the highest prisoner across all of the grounds
    int ground_index = -1;
    int biggest_val = -1;

    for (int i=0; i<length; i++) {
        if (isempty(grounds[i]))
            continue;

        Soldier* front = peek(grounds[i]);

        if (front->sequenceNumber > biggest_val) {
            ground_index = i;
            biggest_val = front->sequenceNumber;
        }
    }

    return ground_index;
}

int empty_count(Queue** grounds, int length) {
    // count how many empty grounds there are
    int count = 0;

    for (int i=0; i<length; i++)
        if (isempty(grounds[i]))
            count++;
    
    return count;
}


// phase1 functions
void phase1_exec(FILE* out, Queue* ground) {
    int size = ground->n_Soldiers;

    Soldier* current = ground->end;

    fprintf(out, "Line# %d %s\n", ground->id, ground->name);

    // we ensure that the jump value isn't higher than the number of soldiers
    move_head(ground, ground->jump % (1 + ground->n_Soldiers));
    
    while (size != ground->threshold) {
        current = peek(ground);
        int next_prisoner = get_next_prisoner(ground, current->sequenceNumber, ground->jump);

        fprintf(out, "Soldier# %d executed\n", current->sequenceNumber);
        remove_prisoner(ground, current->sequenceNumber);

        move_head(ground, next_prisoner);
        size--;
    }
}

void reorder_queue(Queue* ground) {
    // find the lowest id in the queue and set to head
    // fixes the rotated queue post phase1
    Soldier* current = ground->front->next;
    int min = ground->n_Soldiers;

    if (current == ground->end) { // queue with length 1 or 2
        min = current->sequenceNumber;
        move_head(ground, min);
    }

    while (current != ground->front) {
        if (current->sequenceNumber < min)
            min = current->sequenceNumber;
        
        current = current->next;
    }

    if (current->sequenceNumber < min)
        min = current->sequenceNumber;

    move_head(ground, min);
}

int get_next_prisoner(Queue* ground, int pos, int jump) {
    // get the next prisoner given the position and jump value
    Soldier* current = ground->front;

    while(current->next != NULL) {
        if (current->sequenceNumber == pos) {
            break;
        }
        current = current->next;
    }

    int i = 0;
    while (i < jump) {
        if (current->next->sequenceNumber == pos) {
            current = current->next->next;
        } else {
            current = current->next;
        }
        i++;
    }

    return current->sequenceNumber;
}

void move_head(Queue* q, int value) {
    // move the head to where ever the given value is
    Soldier* current = q->front;

    while (current->sequenceNumber != value) {
        current = current->next;
    }

    q->front = current;
    q->end = current->previous;
}

void remove_prisoner(Queue* ground, int prisoner) {
    // remove prisoner give the value
    Soldier* current = ground->front;
    
    while (current->sequenceNumber != prisoner) {
        current = current->next;
    }

    current->previous->next = current->next;
    current->next->previous = current->previous;
    free(current);
}

Queue* create_ground(int id, char* name, int n, int k, int th) {
    // create an execution ground
    Queue* temp_que = (Queue*)malloc(sizeof(Queue));

    temp_que->id = id;
    
    temp_que->name =(char*)malloc(sizeof(char) * 51);
    strcpy(temp_que->name, name);

    temp_que->n_Soldiers = n;
    temp_que->jump = k;
    temp_que->threshold = th;
    
    temp_que->front = NULL;
    temp_que->end = NULL;

    return temp_que;
}


// Required functions
Soldier* createSoldier(int sequence) { // allocate the 
    Soldier* s = (Soldier*)malloc(sizeof(Soldier));
    s->sequenceNumber = sequence;
    s->next = NULL;
    s->previous = NULL;

    return s;
}

void create_reverse_circle(Queue *q) { // descending list of numbers
    int i = q->n_Soldiers;
    while (i != 0) {
        Soldier* current = createSoldier(i);
        enqueue(q, current);
        i--;
    }

    q->front->previous = q->end;
}

void rearrange_circle(Queue* q) { // reverse the list
    Soldier* head = q->front;
    Soldier* tail = q->end;

    Soldier* current = q->front;
    Soldier* temp;

    temp = current->next;
    current->next = current->previous;
    current->previous = temp;

    current = current->next;

    do {
        temp = current->next;
        current->next = current->previous;
        current->previous = temp;

        current = current->next;

    } while (current != q->front);

    q->front = tail;
    q->end = head;
}

void display(FILE* out, Queue* q) {
    // print out the queue details
    fprintf(out, "%d %s ", q->id, q->name);

    Soldier* current = q->front;
    while (current != q->end) {
        fprintf(out, "%d ", current->sequenceNumber);
        current = current->next;
    }

    fprintf(out, "%d", current->sequenceNumber);

    fprintf(out, "\n");
}


// Regular queue functions
void enqueue(Queue* q, Soldier* s) {
    // add to end
    if (q == NULL || (q->end == NULL && q->front == NULL)) {
        q->front = s;
        q->end = s;

        s->previous = s;

    } else {

        s->previous = q->end;
        q->end->next = s;
        q->end = s;
        q->end->next = q->front;
    }
}

int dequeue(Queue* q) {
    // return int from the front
    if (q == NULL || (q->end == NULL && q->front == NULL)) {
        return -1;
    }

    int val;
    Soldier* front = q->front;

    if (q->front == q->end) { // single item list
        val = q->front->sequenceNumber;
        q->front = NULL;
        q->end = NULL;

    } else if (q->front->next == q->end) { // 2 item list
        val = q->front->sequenceNumber;
        q->front = q->end;

    } else { // 3+ list
        val = q->front->sequenceNumber;
        q->front = q->front->next;
        q->front->previous = q->end;
        q->end = q->front->previous;
    }

    free(front);
    return val;
}

Soldier* peek(Queue* q) {
    // get front of queue without dequeueing
    if (q == NULL || (q->end == NULL && q->front == NULL)) {
        return NULL;
    }

    return q->front;
}

int isempty(Queue* q) {
    // check is the queue is empty
    if (q == NULL || (q->end == NULL && q->front == NULL)) {
        return 1;
    } else {
        return 0;
    }
}
