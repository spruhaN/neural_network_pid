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


Tuple compute_proportional(int left_val, int right_val);

int constrain(int value, int minVal, int maxVal);

void motor(u08 num, int8_t speed);

void print_value(int val, int val2); // for debugging