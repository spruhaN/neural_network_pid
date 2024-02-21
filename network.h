
#include <time.h>
#include <stdbool.h>
#include <string.h>


# define IDEAL 0

typedef enum {
    PROPORTIONAL,
    DATA,
    EPOCH,
    TRAINING,
    NEURAL
} State;

typedef struct mv{
    float left;
    float right;
}Tuple;

/**
 * the hidden neuron which should be initialized with random values
*/
typedef struct HiddenNeuron{
    float w[2], bias;
}HiddenNeuron;

typedef struct OutputNeuron{
    float w[3], bias;
}OutputNeuron;

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
    void   (*remove)(); /* remove a Tuple from the pool */
    Tuple (*next)(void);            /* select a Tuple to schedule   */
    int    (*qlen)(void);            /* number of ready Tuples       */
    struct Node* (*dequeue)(void);
} *scheduler;

void rr_admit(Tuple new);
void rr_remove();
Tuple rr_next();
int rr_qlen(void);
void rr_enqueue(Tuple new);
struct Node* rr_dequeue();
void admit_data(int a, int b);

struct Queue *queue;
extern struct scheduler roundrobin;
extern scheduler round_r;

Tuple compute_proportional(float left_val, float right_val);

int constrain(int value, int minVal, int maxVal);

void print_value(int val, int val2); // for debugging

void init_hidden_neuron(HiddenNeuron *neuron);

void init_output_neuron(OutputNeuron *neuron);

void train_neural_network(Tuple input_pair, HiddenNeuron* hidden_neurons[], OutputNeuron* output_neurons[]);

Tuple compute_neural_network(Tuple input_pair, HiddenNeuron* hidden_neurons[], OutputNeuron* output_neurons[]);

void update_all(HiddenNeuron* hidden_neurons[], OutputNeuron* output_neurons[], HiddenNeuron hidden_update_neurons[], OutputNeuron output_update_neurons[]);

float sigmoid(float x);

void motor(int num, int8_t speed);
