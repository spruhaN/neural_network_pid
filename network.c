
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
                // stops motors
                // repeat 
                    // print 50 inc 
                    // get sensor data 
                    // print 01 left sensor 05 right sensor
                    // stores input pairs needs to be dynamic(linked or realloc?)

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