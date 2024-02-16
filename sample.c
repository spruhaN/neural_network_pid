#include "globals.h"
#include "network.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define Kp .001
#define Ki 0
#define Kd 0

#define LEARNING_RATE .15
#define NUM_OUT_NEURONS 2
#define NUM_HIDDEN_NEURONS 3
#define EPOCH_TIME 500

State curr_state = TRAINING;

scheduler our_linked_list;

float integral = 0.0;
float prev_err = 0.0;
float hidden_out[NUM_HIDDEN_NEURONS] = {0};

void admit_data(int a, int b){
    Tuple new;
    new.left = a;
    new.right = b;
    round_r->admit(new);
}

int main(void){
    // ADDED
    init();
    motor(0,0);
    motor(1,0);

    int epoch = 10;
    int button_state = 0;
    int n = 0;
    HiddenNeuron h1,h2,h3;
    OutputNeuron o1,o2;

    // make a list of the hidden and output references to pass into functions
    HiddenNeuron* hidden_neurons[] = {&h1,&h2,&h3};
    OutputNeuron* output_neurons[] = {&o1,&o2};

    // random time seed
    srand(time(NULL));

    admit_data(5,128);
    admit_data(6,4);
    admit_data(4,4);
//    admit_data(5,4);
//    admit_data(67,28);
//    admit_data(5,120);
//    admit_data(5,122);
//    admit_data(5,136);
//    admit_data(159,5);
//    admit_data(128,4);
//    admit_data(120,5);
//    admit_data(5,5);
//    admit_data(144,5);
//    admit_data(5,122);
//    admit_data(7,6);

    // init 3 hidden neurons w/ rand w/b (? make global to main ?)
//    init_hidden_neuron(&h1);
//    init_hidden_neuron(&h2);
//    init_hidden_neuron(&h3);
//
//    // init 2 output neurons (? make global to main ?)
//    init_output_neuron(&o1);
//    init_output_neuron(&o2);

    // set values instead of random :(

    h1.w[0] = 0.2423;
    h1.w[1] = 0.6023;
    h1.bias = 0.6604;

    h2.w[0] = 0.8761;
    h2.w[1] = 0.3045;
    h2.bias = 0.1245;

    h3.w[0] = 0.0508;
    h3.w[1] = 0.3376;
    h3.bias = 0.2635;

    o1.w[0] = 0.9528;
    o1.w[1] = 0.5186;
    o1.w[2] = 0.2965;
    o1.bias = 0.5775;

    o2.w[0] = 0.2975;
    o2.w[1] = 0.1967;
    o2.w[2] = 0.9792;
    o2.bias = 0.5234;




    // for set epoch

    for (int e = 0; e<50; e++){
        // for each input pair (scale down 0-1)
        int len = round_r->qlen();
        struct Node *saved_head = queue->head;
        for (int i = 0; i<len; i++){ // change to queue length
            // dequeue first element in queue store as Tuple
            Tuple input_pair = round_r->dequeue()->sensor_values;

            // trains neural network and updates all weights and biases need to pass neurons by ref
            train_neural_network(input_pair, hidden_neurons, output_neurons);
        }
        queue->head = saved_head;
    }



    // sample value
    Tuple tests;
    tests.left = 128.0;
    tests.right = 5.0;

    Tuple motor_target = compute_proportional(tests.left, tests.right);

    // our neural network
    tests.left = tests.left/255.0;
    tests.right = tests.right/255.0;

    Tuple motor_actual = compute_neural_network(tests,hidden_neurons,output_neurons);
    motor_actual.left = motor_actual.left * 100;
    motor_actual.right = -1 * motor_actual.right * 100;

    uint16_t pid_left = motor_target.left;
    uint16_t pid_right = motor_target.left;
    uint16_t neural_left = motor_target.left;
    uint16_t neural_right = motor_target.left;

    lcd_cursor(0,0);
    print_num(pid_left);
    lcd_cursor(5,0);
    print_num(pid_right);
    lcd_cursor(0,1);
    print_num(neural_left);
    lcd_cursor(5,1);
    print_num(neural_right);

    return 0;
}

float calculate_output_weight(float net, float hidden_out, float old_weight){
    float sum = net * (hidden_out);
    return old_weight - (LEARNING_RATE * sum);
}


float calculate_hidden_weight(float sum_out, float hidden_out, float input_sensor, float old_weight){
    float sum = sum_out * (hidden_out * (1-hidden_out)) * input_sensor;
    return old_weight - (LEARNING_RATE * sum);
}

Tuple compute_neural_network(Tuple input_pair, HiddenNeuron* hidden_neurons[], OutputNeuron* output_neurons[]){
    float sensor_pair[] = {input_pair.left, input_pair.right};
    // find outputs for each hidden neuron
    for(int i = 0; i < NUM_HIDDEN_NEURONS; i++){
        // for each hidden neuron we sum the net a1(left sensor) * w1 + a2(right sensor) * w2 - bias and pass through sigmoid
        float input = (sensor_pair[0] * hidden_neurons[i]->w[0]) + (sensor_pair[1] * hidden_neurons[i]->w[1]) - hidden_neurons[i]->bias;
        hidden_out[i] = sigmoid(input);
    }

    // pass through output neurons and get actual
    float actual_pair[NUM_OUT_NEURONS] = {0};
    for(int i = 0; i  < NUM_OUT_NEURONS; i++){
        // for each output neuron we look at the weight with prev linked hidden and multiply with the prev calc output
        float input = 0;
        for (int j = 0; j < NUM_HIDDEN_NEURONS; j++){
            input += hidden_out[j] * output_neurons[i]->w[j];
        }
        input -= output_neurons[i]->bias;
        actual_pair[i] = sigmoid(input);
    }

    Tuple motor;
    motor.left = actual_pair[0];
    motor.right = actual_pair[1];
    return motor;
}

void train_neural_network(Tuple input_pair, HiddenNeuron* hidden_neurons[], OutputNeuron* output_neurons[]){ // need to pass neurons by reference
    OutputNeuron update_o1, update_o2;
    HiddenNeuron update_h1, update_h2, update_h3;

    OutputNeuron output_update_neurons[] = {update_o1, update_o2};
    HiddenNeuron hidden_update_neurons[] = {update_h1, update_h2, update_h3};

    float out_net[NUM_OUT_NEURONS] = {0};

    // run on compute and get output pair (?)
    Tuple output_pair = compute_proportional(input_pair.left, input_pair.right);

    // scale motor output values from 100 -> (0-1)
    output_pair.left = output_pair.left/100.0;
    output_pair.right = -1 * output_pair.right/100.0;

    input_pair.left = input_pair.left/255.0;
    input_pair.right = input_pair.right/255.0;

    // scale sensor for training from 255 int 0 to 1 (- for right)
    float sensor_pair[] = {input_pair.left, input_pair.right};
    float target_pair[] = {output_pair.left, output_pair.right};

    // FORWARD PASS (try to make this a function) but we need to access hidden out and actual pair
    Tuple actual_tuple = compute_neural_network(input_pair,hidden_neurons,output_neurons);
    float actual_pair[] = {actual_tuple.left, actual_tuple.right};

    // calculate total error why??
    // float total_error = .5 * (pow((output_pair.left - actual_pair[0]),2) + pow((output_pair.right - actual_pair[1]),2));

    // BACK PROPOGATION
    // update output weights
    for (int i = 0; i < 2; i++){
        for (int j = 0; j < 3; j++){
            // for each output neuron(2) for each weight(3) of the output neuron put in a copy struct
            // storing net values for hidden adjusting
            float diff = (actual_pair[i] - target_pair[i]);
            float self = (actual_pair[i] * (1.0-actual_pair[i]));
            out_net[i] = diff * self;
            float etot = out_net[i] * hidden_out[j];
            float new_weight = output_neurons[i]->w[j] - (.15 * etot);
            output_update_neurons[i].w[j] = new_weight;
        }
        output_update_neurons[i].bias = calculate_output_weight(out_net[i], -1.0, output_neurons[i]->bias);
    }

    // update hidden weights
    for (int i = 0; i < NUM_HIDDEN_NEURONS; i ++){
        float sum_out = out_net[0]*output_neurons[0]->w[i] + out_net[1]*output_neurons[1]->w[i];
        float ho = hidden_out[i];
        for (int j = 0; j < 2; j++){ // number of sensors
            // for each hidden neuron 3 for each weight
            float sp = sensor_pair[j];
            float ow = hidden_neurons[i]->w[j];
            hidden_update_neurons[i].w[j] = calculate_hidden_weight(sum_out, ho, sp, ow);
        }
        // NO INPUT VAL FOR BIAS ???
        hidden_update_neurons[i].bias = calculate_hidden_weight(sum_out, hidden_out[i], -1.0, hidden_neurons[i]->bias);
    }
    // update all 17 weights and biases
    update_all(hidden_neurons,output_neurons,hidden_update_neurons,output_update_neurons);
}

void update_all(HiddenNeuron* hidden_neurons[], OutputNeuron* output_neurons[], HiddenNeuron hidden_update_neurons[], OutputNeuron output_update_neurons[]){
    // update hidden
    for (int i = 0; i<NUM_HIDDEN_NEURONS;i++){
        for (int j = 0; j < 2; j++){ // num sensors
            hidden_neurons[i]->w[j] = hidden_update_neurons[i].w[j];
        }
        hidden_neurons[i]->bias = hidden_update_neurons[i].bias;
    }

    // update output
    for (int i = 0; i<NUM_OUT_NEURONS;i++){
        for (int j = 0; j < NUM_HIDDEN_NEURONS; j++){
            output_neurons[i]->w[j] = output_update_neurons[i].w[j];
        }
        output_neurons[i]->bias = output_update_neurons[i].bias;
    }
}

float sigmoid(float x) {
    return (float)(1.0/(1.0 + (float)exp(-x)));
}

Tuple compute_proportional(float sensor_left, float sensor_right) {
    float ideal_pos = 0;
    float actual_pos = sensor_right - sensor_left;
    // how diff it is from 0(ideal white - white)
    float error = ideal_pos - actual_pos;
    // print_value(er);
    //not using rn
    integral = error + integral;
    float derivative  = error - prev_err;
    prev_err = error;

    float output = (Kp * error) + (Ki * integral) + (Kd * derivative);

    int left_motor = 20 - output;
    int right_motor = -20 - output;

    left_motor = constrain(left_motor, 0, 100);
    right_motor = constrain(right_motor, -100, 0);

    Tuple motor;
    motor.left = left_motor;
    motor.right = right_motor;

    return motor;
}

void init_hidden_neuron(HiddenNeuron *neuron) {
    for (int i = 0; i < 2; i++) {
        neuron->w[i] = (float)rand() / RAND_MAX;
    }
    neuron->bias = (float)rand() / RAND_MAX;
}

void init_output_neuron(OutputNeuron *neuron) {
    for (int i = 0; i < 3; i++) {
        neuron->w[i] = (float)rand() / RAND_MAX;
    }
    neuron->bias = (float)rand() / RAND_MAX;
}

int constrain(int value, int minVal, int maxVal) {
    if (value < minVal) {
        return minVal;
    } else if (value > maxVal) {
        return maxVal;
    } else {
        return value;
    }
}

// void motor(u08 num, int8_t speed){
//    int speed_calc = floor(.3*speed + 127);
//    set_servo(num,speed_calc);
// }

// void print_value(int val, int val2){
//    char buffer [33];
//    char buffer2 [33];
//    itoa(val, buffer, 10);
//    itoa(val2, buffer2, 10);
//    strcat(buffer, ". ");
//    strcat(buffer2, ". ");
//    lcd_cursor(0,0);
//    print_string(buffer);
//    lcd_cursor(0,1);
//    print_string(buffer2);
//    //_delay_ms(1000);
//    clear_screen();
// }



// LINKED LIST

struct Queue *queue;

// adds node to tail
void rr_enqueue(Tuple new) {
    struct Node *new_tail = (struct Node *)malloc(sizeof(struct Node));

    // initialize the new Node
    new_tail->sensor_values = new;
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

// removes Tuple from queue
struct Node* rr_dequeue() {
    struct Node *old_Tuple_node = queue->head;
    if (queue->head == NULL){
        fprintf(stderr, "Dequeuing from empty queue\n");
        return NULL;
    }
    queue->head = queue->head->next;
    return old_Tuple_node;
}

// enqueues new Tuple to queue
void rr_admit(Tuple new) {
    rr_enqueue(new);
}

// removes Tuple from front of queue
void rr_remove() {
    Node *victim_node = rr_dequeue();
    free(victim_node);
}

// moves queue over
Tuple rr_next() {
    return queue->head->sensor_values;
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

struct scheduler roundrobin = {rr_admit, rr_remove, rr_next, rr_qlen, rr_dequeue};
scheduler round_r = &roundrobin;


void motor(u08 num, int8_t speed){
   int speed_calc = floor(.3*speed + 127);
   set_servo(num,speed_calc);
}