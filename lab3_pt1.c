// Name: Sarah Hallam, Aiden Jacob, and Spruha Nayak
// Assignment: Lab 2 part 
// Description: 

#include "globals.h"
#include "network.h"
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void motor(u08 num, int8_t speed);

int constrain(int value, int minVal, int maxVal);

Tuple updatePID(float ideal_pos, float actual_pos);

void adjustMotors(float output);

void print_value(int val, int val2);

void handleCorner(int motor_set,int left, int right);

// NEED TO BE ALTERED!!!
float Kp = .08;
float Ki = 0 ;
float Kd = 0;

float integral = 0.0;
float prev_err = 0.0;

int main(void) {
   init();

   while(1){
      //motor (1,0);
      //motor(0,0);

      // read sensors 
      int left_val = analog(2);
      int right_val = analog(3);
      //print_value(left_val,right_val);
      // pos
      
      Tuple motor_val = compute_proportional(left_val, right_val);

      motor(0,motor_val.left);
      motor(1,motor_val.right);
   }
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


// adjusting motors to calc pid val
void adjustMotors(float output) {
   //int baseSpeed = 50;
   //print_value(output,0);
   int left_motor = 20 - output;
   int right_motor = -20 - output;

   //print_num(output);
   //_delay_ms(100);
   // can it handle -'s ??and these are in opposite directions
   //print_value(left_motor, right_motor);
   left_motor = constrain(left_motor, 0, 100);
   right_motor = constrain(right_motor, -100, 0);

   motor(0, left_motor);
   motor(1, right_motor);
   //clear_screen();
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