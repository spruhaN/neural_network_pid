
#include "globals.h"
#include "network.h"

#include <stdio.h>
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

State curr_state = PROPORTIONAL;

float integral = 0.0;
float prev_err = 0.0;

scheduler our_linked_list;

int main(void){
    int epoch = 5;
    int button_state = 0;
    hiddenNeuron h1,h2,h3;
    outNeuron o1,o2;

    // random time seed
    srand(time(NULL));

    while(1){ // do we want while in cases or in state
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

                    // if button pressed -> DATA (? we need to make sure its properly registered ?) :( no nested func in C
                    if (get_btn() & (button_state == 0)){ // if button is pressed for the first time
                        button_state = 1;
                    }else if (!get_btn() & (button_state == 1)){ // if button is released (has been pressed before and is currently not pressed)
                        button_state = 0;
                        curr_state = DATA;
                    }
                }

            case DATA:
                {
                // clear screen and print 00 Data
                clear_screen();
                lcd_cursor(0, 0);
                print_string("Data");

                // stops motors
                motor(0, 0);
                motor(1, 0);

                int n = 0;
                // repeat 
                    // print 50 inc 
                    lcd_cursor(5, 0);
                    print_num(n);

                    // get sensor data 
                    int left = analog(2);
                    int right = analog(3);

                    // print 01 left sensor 05 right sensor
                    lcd_cursor(0, 1);
                    print_num(left);

                    lcd_cursor(5, 1);
                    print_num(right);

                    // add Tuple to linked list 
                    Tuple* new_tuple = (Tuple*)malloc(sizeof(Tuple));
                    //new_tuple = &new_tuple;
                    new_tuple->left = left;
                    new_tuple->right = right;

                    our_linked_list->admit(*new_tuple);
}
                    // once finished -> EPOCH
                break;
    
            case EPOCH:
                {
                    // clear screen print 00 Training_0
                    clear_screen();
                    lcd_cursor(0,0);
                    print_string("Training_0");

                    // get accel
                    int y_loc = get_accel_y();

                    // scale epoch
                    if(y_val > 15 && y_val < 60){ // left
                        epoch--;
                    }else if(y_val < 235 && y_val > 190){ // right
                        epoch++;
                    }

                     // print epoch
                    lcd_cursor(0,1)
                    print_num(epoch);

                    // if button is pressed -> set scaled epoch -> TRAINING
                    if (get_btn() & (button_state == 0)){ // if button is pressed for the first time
                        button_state = 1;
                    }else if (!get_btn() & (button_state == 1)){ // if button is released (has been pressed before and is currently not pressed)
                        button_state = 0;
                        curr_state = TRAINING;
                    }

                    // delay so user gets time to read and change value
                    _delay_ms(1000);
                }
                break;

            case TRAINING:
                {
                    // clear screen print 00 Training 06 set epoch
                    clear_screen();
                    lcd_cursor(0,0);
                    print_string("Training");

                    // init 3 hidden neurons w/ rand w/b (? make global to main ?)
                    init_hidden_neuron(&h1);
                    init_hidden_neuron(&h2);
                    init_hidden_neuron(&h3);

                    // init 2 output neurons (? make global to main ?)
                    init_output_neuron(&o1);
                    init_output_neuron(&o2);

                    // turn into list of h neurons and o neurons


                    // for set epoch
                    for (int e = 0; e<epoch; e++){
                        // for each input pair (scale down 0-1)
                        for (int i = 0; i< 2; i++){ // change to queue length
                            // dequeue first element in queue store as Tuple
                            Tuple input_pair;

                            // trains neural network and updates all weights and biases need to pass neurons by ref
                            train_neural_network(input_pair);
                        }
                    }
                    // when finished -> NEURAL or break
                    curr_state = NEURAL;
                }

            case NEURAL:
                {
                    // clear screen print 00 Neural
                    clear_screen();
                    lcd_cursor(0,0);
                    print_string("Neural");

                    // get sensor input scale down to 0-1
                    float left_sensor = ((float)analog(2))/255.0;
                    float right_sensor = ((float)analog(3))/255.0;

                    // compute_neural_network ==> (? lol what ?)
                    Tuple motor_values = compute_neural_network(left_sensor, right_sensor);

                    // set motors to sensor output
                    motor(0,motor_values.left);
                    motor(1,motor_values.right);

                    // if button is pressed -> EPOCH
                    if (get_btn() & (button_state == 0)){ // if button is pressed for the first time
                            button_state = 1;
                        }else if (!get_btn() & (button_state == 1)){ // if button is released (has been pressed before and is currently not pressed)
                            button_state = 0;
                            curr_state = DATA;
                    }
                }

            default:
                // clear screen print 00 Error
                {
                    clear_screen();
                    lcd_cursor(0,0;);
                    print_string("ERR");
                }
        }
    }
    return 0;
}

void train_neural_network(Tuple input_pair, outNeuron* o1, outNeuron* o2, hiddenNeuron* h1, hiddenNeuron* h2, hiddenNeuron* h3){ // need to pass neurons by reference
    outNeuron update_o1,update_o2;
    hiddenNeuron update_h1, update_h2, update_h3;

    // run on compute and get output pair (?)
    output_pair = compute_proportional(input_pair.left, input_pair.right);

    // compute out neuron weight and bias x2 store in struct
    // 
    compute_output_neuron(output_pair,o1,&update_o1);
    compute_output_neuron(output_pair,o2,&update_o2);
    
    // compute hidden neuron weight and bias x3 store in struct ( " )
    compute_hidden_neuron(output_pair,h1,&update_h1);
    compute_hidden_neuron(output_pair,h2,&update_h2);
    compute_hidden_neuron(output_pair,h3,&update_h3);

    // update all 17 weights and biases ( then need ref )
    o1->w1 = update_o1.w1;
    o2->w2 = update_o2.w2;
    o1->w3 = update_o3.w3;
    o1->w1 = update_o1.w1;
    o1->w1 = update_o1.w1;
    o1->w1 = update_o1.w1;
}

Tuple compute_proportional(int left_val, int right_val) {
   Tuple mv;
   // how diff actual pos is from 0(ideal white - white)
   int error = IDEAL - (right_val - left_val);
    // print_value(er);

    //not using rn
   integral = error + integral;
   int derivative  = error - prev_err;
   prev_err = error; 

   int output =( Kp * error) + (Ki * integral) + (Kd * derivative);

   int left = 20 - output;
   int right  = -20 - output;

   left = constrain(left, 0, 100);
   right = constrain(right, -100, 0);

   mv.left = left;
   mv.right = right;

   return mv;
}

void init_hidden_neuron(hiddenNeuron *neuron) {
    neuron->w1 = (float)rand()/RAND_MAX;
    neuron->w2 = (float)rand()/RAND_MAX;
    neuron->bias = -1.0;
}

void init_output_neuron(outNeuron *neuron) {
    neuron->w1 = (float)rand()/RAND_MAX;
    neuron->w2 = (float)rand()/RAND_MAX;
    neuron->w3 = (float)rand()/RAND_MAX;
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
void rr_remove(Tuple victim) {
    Node *victim_node = rr_dequeue();
    free(victim_node);
}

// moves queue over
Tuple rr_next() {
    if(queue->head){
        return queue->head->sensor_values;
    }
    else{
        print_string("error");
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

struct scheduler roundrobin = {rr_admit, rr_remove, rr_next, rr_qlen};
scheduler round_r = &roundrobin;
