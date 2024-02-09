#include "rr.h"
#include <stdio.h>

struct Queue *queue;

// adds node to tail
void rr_enqueue(thread new) {
    struct Node *new_tail = (struct Node *)malloc(sizeof(struct Node));

    // initialize the new Node
    new_tail->current_thread = new;
    new_tail->next = NULL;

    // check if the queue is empty
    if (queue == NULL || queue->head == NULL) {
        if(queue == NULL){
            queue = (struct Queue *)malloc(sizeof(struct Queue));
            (*queue).head = NULL;
            (*queue).tail = NULL;
        }

        queue->head = new_tail;
        queue->tail = new_tail;
        // update the tail pointer to the new Node
        queue->tail->next = NULL;
    } else {
        // update the next pointer of the current tail
        queue->tail->next = new_tail;
        // update the tail pointer to the new Node
        queue->tail = new_tail;
    }
}

// removes thread from queue
struct Node* rr_dequeue() {
    struct Node *old_thread_node = queue->head;
    if (queue->head == NULL){
        fprintf(stderr, "Dequeuing from empty queue\n");
        return NULL;
    }
    queue->head = queue->head->next;
    return old_thread_node;
}

// enqueues new thread to queue
void rr_admit(thread new) {
    rr_enqueue(new);
}

// removes thread from front of queue
void rr_remove(thread victim) {
    Node *victim_node = rr_dequeue();
    free(victim_node);
}

// moves queue over
thread rr_next() {
    if(queue->head){
        return queue->head->current_thread;
    }
    else{
        return NULL;
    }
}

// gets length of queue
int rr_qlen(void) {
    int counter = 0;
    struct Node* temp = queue->head;
    while(temp){
        counter++;
        temp = temp->next;
    }
    return counter;
}

struct scheduler roundrobin = {NULL, NULL, rr_admit, rr_remove, rr_next, rr_qlen};
scheduler round_r = &roundrobin;

void print_queue(){
    Node* testNode = NULL;
    if(queue){
        testNode = queue->head;
    }

    while(testNode){
        printf("%lu\n", testNode->current_thread->tid);
        testNode = testNode->next;
    }
    printf("\nBREAK\n");
}



// FREEING AND INIT DONE IN LWP.C

// creates queue structure and puts current process [main] into queue DO THIS IN LWP.C NOT HERE
// DONT NEED INIT OR SHUTDOWN
// DONT NEED TO ALLOCATE ANY MEMORY

// This is to be called before any threads are admitted to the scheduler. It’s to allow the scheduler to set up.
// This one is allowed to be NULL, so don’t call it if it is.
// void rr_init() {
//     queue = (struct Queue *)malloc(sizeof(struct Queue));
//     struct Node *main = (struct Node *)malloc(sizeof(struct Node));
//     struct threadinfo_st *thread_new = (struct threadinfo_st *)malloc(sizeof(struct threadinfo_st *));
//     struct threadinfo_st thread_info = (struct threadinfo_st)malloc(sizeof(struct threadinfo_st));
//     // how should we populate main
//     thread_info.tid = 0;
//     thread_info.stack = 0;
//     thread_info.stacksize = 0;
//     thread_info.state = 0;
//     thread_info.status = NULL;
//     thread_info.lib_one = NULL;
//     thread_info.lib_two = NULL;
//     thread_info.sched_one = NULL;
//     thread_info.sched_two = NULL;
//     thread_info.exited = NULL;
//     enqueue(thread thread_new);
// }

// frees entire queue
// Lwp library is done with a scheduler to allow it to clean up
// This, too, is allowed to be NULL, so don’t call it if it is.
// void rr_shutdown() {
//     while(queue.head){
//         rr_remove(queue.head.current_thread);
//     }
//     free(queue);
// }

