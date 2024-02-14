#ifndef RR
#define RR
#include "Asgn2/include/lwp.h"
#include "Asgn2/include/fp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

// add our linked list struct here
//typedef struct Node*;
typedef struct Node {
    Tuple sensor_values;
    struct Node *next;
} Node;

typedef struct Queue {
    struct Node *head;
    struct Node *tail;
} Queue;

void rr_admit(thread new);
void rr_remove();
thread rr_next();
int rr_qlen(void);
void rr_enqueue(thread new);
struct Node* rr_dequeue();

struct Queue *queue;
extern struct scheduler roundrobin;
extern scheduler round_r;

# endif
