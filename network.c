
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
                // clear screen and print 00 Data
                // stops motors
                // repeat 
                    // print 50 inc 
                    // get sensor data 
                    // print 01 left sensor 05 right sensor
                    // stores input pairs needs to be dynamic queue

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

                            // trains neural network and updates all weights and biases
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

void train_neural_network(Tuple input_pair){ // need to pass neurons by reference

    // run on compute and get output pair (?)
    output_pair = compute_proportional(input_pair.left, input_pair.right);

    // compute out neuron weight and bias x2 store in struct ( pass by val )
    // can we also initilaize a new output neuron that stores all these values and then replace old with new at end?
    updatedNeuron update_o1 = compute_output_neuron(output_pair, o1);
    updatedNeuron update_o2 = compute_output_neuron(output_pair, o2);
    
    // compute hidden neuron weight and bias x3 store in struct ( " )
    updatedNeuron update_h1 = compute_hidden_neuron(output_pair, h1);
    updatedNeuron update_h2 = compute_hidden_neuron(output_pair, h2);
    updatedNeuron update_h3 = compute_hidden_neuron(output_pair, h3);

    // update all 17 weights and biases ( then need ref )
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