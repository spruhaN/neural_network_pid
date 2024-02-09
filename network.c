
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

    while(1){ // do we want while in cases or in state
        switch (curr_state){
            case PROPORTIONAL:
                // 00 print_string("Proportional");
                // repeat (?)
                    {
                        // get sensor data 
                        int left_val = analog(2);
                        int right_val = analog(3);

                        // calls compute_proportional and sets motor
                        Tuple motor_values = compute_proportional(left_val,right_val);

                        motor(0, motor_values.left);
                        motor(1, motor_values.right);

                        // if button pressed -> DATA (? we need to make sure its properly registered ?)
                    }
                break;

            case DATA:
                // clear screen and print 00 Data
                lcd_cursor(0, 0);
                print_string("DATA");

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

                    // once finished -> EPOCH
                break;
    
            case EPOCH:
                // clear screen print 00 Training_0
                // repeat
                    // get accel
                    // print scaled epoch
                    // if button is pressed -> set scaled epoch -> TRAINING
                break;

            case TRAINING:
                // clear screen print 00 Training 06 set epoch
                // init 3 hidden neurons w/ rand w/b (? make global to main ?)
                // init 2 output neurons (? make global to main ?)
                // for set epoch
                    // for each input pair (scale down 0-1)
                    // train_neural_network ==>
                        // run on compute and get output pair (?)
                        // compute out neuron weight and bias x2 store in struct (??)
                        // compute hidden neuron weight and bias x3 store in struct (??)
                        // update all 17 weights and biases (?)
                // when finished -> NEURAL
                curr_state = NEURAL; // or break

            case NEURAL:
                // clear screen print 00 Neural
                // repeat
                    // get sensor input scale down to 0-1
                    // compute_neural_network ==> (? lol what ?)
                    // set motors to sensor output
                    // if button is pressed -> EPOCH
                break;

            default:
                // clear screen print 00 Error
                {}
        }
    }
    return 0;
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
