#include "globals.h"
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

# define IDEAL 0

typedef enum {
    PROPORTIONAL,
    DATA,
    EPOCH,
    TRAINING,
    NEURAL
} State;

typedef struct mv {
    int left;
    int right;
}Tuple;


typedef struct HiddenNeuron{
    float w1;  // left sensor
    float w2; // right sensor
    float bias;
}hiddenNeuron;

typedef struct OutputNeuron{
    float w1; // h1
    float w2; // h2
    float w3; // h3
    float bias;
}outNeuron;

typedef struct Node {
    Tuple sensor_values;
    struct Node *next;
} Node;

typedef struct Queue {
    struct Node *head;
    struct Node *tail;
} Queue;

/* Tuple that describes a scheduler */
typedef struct scheduler {
  void   (*admit)(Tuple new);     /* add a Tuple to the pool      */
  void   (*remove)(Tuple victim); /* remove a Tuple from the pool */
  Tuple (*next)(void);            /* select a Tuple to schedule   */
  int    (*qlen)(void);            /* number of ready Tuples       */
} *scheduler;

void rr_admit(Tuple new);
void rr_remove(Tuple victim);
Tuple rr_next();
int rr_qlen(void);
void rr_enqueue(Tuple new);
struct Node* rr_dequeue();

struct Queue *queue;
extern struct scheduler roundrobin;
extern scheduler round_r;

Tuple compute_proportional(int left_val, int right_val);

int constrain(int value, int minVal, int maxVal);

void motor(u08 num, int8_t speed);

void print_value(int val, int val2); // for debugging