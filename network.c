
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

#define Kp .08
#define Ki 0
#define Kd 0

#define LEARNING_RATE .08
#define NUM_OUT_NEURONS 2
#define NUM_HIDDEN_NEURONS 3
#define EPOCH_TIME 500

State curr_state = PROPORTIONAL;

scheduler our_linked_list;

float integral = 0.0;
float prev_err = 0.0;


int main(void){
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

    init();

    while(1){
        switch (curr_state){
            case PROPORTIONAL:
                {
                    // 00 print_string("Proportional")
                    lcd_cursor(0,0);
                    print_string("Proportional");
                    
                    // get sensor data 
                    int left_val = analog(2);
                    int right_val = analog(3);

                    // calls compute_proportional and sets motor
                    Tuple motor_values = compute_proportional(left_val,right_val);

                    motor(0, motor_values.left);
                    motor(1, motor_values.right);

                    // if button pressed -> DATA
                    if (get_btn() && (button_state == 0)){ // if button is pressed for the first time
                        button_state = 1;
                    }else if (!get_btn() && (button_state == 1)){ // if button is released (has been pressed before and is currently not pressed)
                        button_state = 0;
                        curr_state = DATA;
                        clear_screen();
                        // clear screen and print 00 Data
                        lcd_cursor(0, 0);
                        print_string("Data");

                        // stops motors
                        motor(0, 0);
                        motor(1, 0);
                    }
                }
                break;

            case DATA:
                {
                    clear_screen();

                    // get sensor data 
                    int left = analog(2);
                    int right = analog(3);

                    // print 50 inc 
                    lcd_cursor(0, 0);
                    print_string("Data");

                    lcd_cursor(6,0);
                    print_num(n);

                    // print 01 left sensor 05 right sensor
                    lcd_cursor(0, 1);
                    print_num(left);

                    lcd_cursor(5, 1);
                    print_num(right);

                    // add Tuple to linked list 
                    admit_data(left,right);
                    
                    //print_value(left, right);
                    //_delay_ms(1000);

                    n++;

                    // if button pressed -> EPOCH
                    for (int i = 0; i < 100 ; i ++){
                        if (get_btn() && (button_state == 0)){ // if button is pressed for the first time
                            button_state = 1;
                        }else if (!get_btn() && (button_state == 1)){ // if button is released (has been pressed before and is currently not pressed)
                            button_state = 0;
                            curr_state = EPOCH;
                            // clear screen print 00 Training_0
                            clear_screen();
                            lcd_cursor(0,0);
                            print_string("0_Training");
                        }
                        _delay_ms(1);
                    }
                }
                break;
            case EPOCH:
                {
                    // get accel
                    int y_val = get_accel_y();

                    // scale epoch
                    if(y_val > 15 && y_val < 60){ // left
                        if (epoch > 11){
                            epoch -= 10;
                        }
                    }else if(y_val < 235 && y_val > 190){ // right
                        epoch += 10;
                    }

                     // print epoch
                    lcd_cursor(0,1);
                    print_num(epoch);

                    // if button is pressed -> set scaled epoch -> TRAINING
                    for (int i = 0; i<EPOCH_TIME; i++){
                        if (get_btn() && (button_state == 0)){ // if button is pressed for the first time
                            button_state = 1;
                        }else if (!get_btn() && (button_state == 1)){ // if button is released (has been pressed before and is currently not pressed)
                            button_state = 0;
                            curr_state = TRAINING;
                            // clear screen print 00 Training 06 set epoch
                            clear_screen();
                            lcd_cursor(0,0);
                            print_string("Train");
                            //lcd_cursor(0,1);
                            //print_num(epoch);
                        }
                        _delay_ms(1);
                    }
                }
                break;
            case TRAINING:
                {
                    // init 3 hidden neurons w/ rand w/b (? make global to main ?)
                    init_hidden_neuron(&h1);
                    init_hidden_neuron(&h2);
                    init_hidden_neuron(&h3);

                    // init 2 output neurons (? make global to main ?)
                    init_output_neuron(&o1);
                    init_output_neuron(&o2);

                    // for set epoch
                    for (int e = 0; e<epoch; e++){
                        // for each input pair (scale down 0-1)
                        int len = round_r->qlen();
                        for (int i = 0; i<len; i++){ // change to queue length
                            // dequeue first element in queue store as Tuple
                            Tuple input_pair = round_r->dequeue()->sensor_values;

                            // print dequeue on bottom and epoch
                            clear_screen();
                            lcd_cursor(5,0);
                            print_num(e);
                            lcd_cursor(0,1);
                            print_num(floor(input_pair.left));
                            lcd_cursor(5,1);
                            print_num(floor(input_pair.right));
                            _delay_ms(100);

                            // trains neural network and updates all weights and biases need to pass neurons by ref
                            train_neural_network(input_pair, hidden_neurons, output_neurons);
                        }
                    }

                    // when finished -> NEURAL or break
                    curr_state = NEURAL;

                    // clear screen print 00 Neural
                    clear_screen();
                    lcd_cursor(0,0);
                    print_string("Neural");
                }
                break;
            case NEURAL:
                {
                    // // get sensor input scale down to 0-1
                    float left_sensor = ((float)analog(2))/255.0;
                    float right_sensor = ((float)analog(3))/255.0;

                    // // compute_neural_network ==> forward pass
                    Tuple input_pair;
                    input_pair.left = left_sensor;
                    input_pair.right = right_sensor;
                    Tuple motor_values = compute_neural_network(input_pair, hidden_neurons, output_neurons);

                    // set motors to sensor output
                    motor_values.left = floor(motor_values.left * 100);
                    motor_values.right =floor( -1 * motor_values.left * 100);
                    motor(0,motor_values.left);
                    motor(1,motor_values.right);

                    clear_screen();
                    lcd_cursor(0,0);
                    print_string("Neural");
                    lcd_cursor(0,1);
                    print_num((int)motor_values.left);
                    lcd_cursor(5,1);
                    print_num((int)motor_values.right);

                    // if button is pressed -> EPOCH
                    if (get_btn() && (button_state == 0)){ // if button is pressed for the first time
                            button_state = 1;
                        }else if (!get_btn() && (button_state == 1)){ // if button is released (has been pressed before and is currently not pressed)
                            button_state = 0;
                            curr_state = EPOCH;
                            clear_screen();
                            lcd_cursor(0,0);
                            print_string("0_Training");

                    }
                }
                break;
            default:
                // clear screen print 00 Error
                {
                    clear_screen();
                    lcd_cursor(0,0);
                    print_string("ERR");
                }
        }
    }
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
    float hidden_out[NUM_HIDDEN_NEURONS] = {0};
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
        input += output_neurons[i]->bias;
        actual_pair[i] = sigmoid(input);
    }

    Tuple motor;
    motor.left = actual_pair[0];
    motor.right = actual_pair[1];
    return motor;
}

void train_neural_network(Tuple input_pair, HiddenNeuron* hidden_neurons[], OutputNeuron* output_neurons[]){ // need to pass neurons by reference
    // DOES THIS SCALE PROPERLY
    OutputNeuron update_o1, update_o2;
    HiddenNeuron update_h1, update_h2, update_h3;

    init_hidden_neuron(&update_h1);
    init_hidden_neuron(&update_h2);
    init_hidden_neuron(&update_h3);

    init_output_neuron(&update_o1);
    init_output_neuron(&update_o2);

    OutputNeuron output_update_neurons[] = {update_o1, update_o2};
    HiddenNeuron hidden_update_neurons[] = {update_h1, update_h2, update_h3};

    float out_net[NUM_OUT_NEURONS] = {0};

    // run on compute and get output pair (?)
    Tuple output_pair = compute_proportional(input_pair.left, input_pair.right);

    // either make struct better[make list of two] or remove tuple later...
    float sensor_pair[] = {input_pair.left/255.0, input_pair.right/255.0}; 
    // should i divide by 100 and flip right
    float target_pair[] = {output_pair.left, output_pair.right};

    // FORWARD PASS (try to make this a function) but we need to access hidden out and actual pair
    // find outputs for each hidden neuron
    float hidden_out[NUM_HIDDEN_NEURONS] = {0};
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
        input += output_neurons[i]->bias;
        actual_pair[i] = sigmoid(input);
    }

    // calculate total error why??
    // float total_error = .5 * (pow((output_pair.left - actual_pair[0]),2) + pow((output_pair.right - actual_pair[1]),2));

    // BACK PROPOGATION
    // update output weights 
    for (int i = 0; i < 2; i++){
        for (int j = 0; j < 3; j++){
            // for each output neuron(2) for each weight(3) of the output neuron put in a copy struct
            // storing net values for hidden adjusting
            out_net[i] = (actual_pair[i] - target_pair[i]) * (actual_pair[i] - (1.0-actual_pair[i]));
            output_update_neurons[i].w[j] = calculate_output_weight(out_net[i], hidden_out[j], output_neurons[i]->w[j]);
        }
        output_update_neurons[i].bias = calculate_output_weight(out_net[i], -1.0, output_neurons[i]->bias);
    }

    // update hidden weights
    float sum_out = 0;
    for (int i = 0; i < NUM_HIDDEN_NEURONS; i++){
        for (int j = 0; j < 2; j++){ // number of sensors
            // for each hidden neuron 3 for each weight 2
            // IS THIS UPDATE VALUE ???
            float sum_out = out_net[0]*output_update_neurons[0].w[i] + out_net[1]*output_update_neurons[1].w[i];
            hidden_update_neurons[i].w[j] = calculate_hidden_weight(sum_out, hidden_out[i], sensor_pair[j], hidden_neurons[i]->w[j]);
        } 
        // NO INPUT VAL FOR BIAS so 1
        hidden_update_neurons[i].bias = calculate_hidden_weight(sum_out, hidden_out[i], 1, hidden_neurons[i]->bias);
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
    neuron->bias = -1.0;
}

void init_output_neuron(OutputNeuron *neuron) {
    for (int i = 0; i < 3; i++) {
        neuron->w[i] = (float)rand() / RAND_MAX;
    }
    neuron->bias = -1.0;
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

void motor(u08 num, int8_t speed){
   int speed_calc = floor(.3*speed + 127);
   set_servo(num,speed_calc);
}

void print_value(int val, int val2){
   char buffer [33];
   char buffer2 [33];
   itoa(val, buffer, 10);
   itoa(val2, buffer2, 10);
   strcat(buffer, ". ");
   strcat(buffer2, ". ");
   lcd_cursor(0,0);
   print_string(buffer);
   lcd_cursor(0,1);
   print_string(buffer2);
   //_delay_ms(1000);
   clear_screen();
}



// LINKED LIST

struct Queue *queue;

// add sensor tuple to queue
void admit_data(int a, int b){
    Tuple new;
    new.left = a;
    new.right = b;
    round_r->admit(new);
}

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
