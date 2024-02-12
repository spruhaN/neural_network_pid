#include "network.h"

// NEED TO BE ALTERED!!!
float Kp = .08;
float Ki = 0 ;
float Kd = 0;

float integral = 0.0;
float prev_err = 0.0;

int main(void) {
   init();

   while(1){
      // read sensors 
      int left_val = analog(2);
      int right_val = analog(3);

      MotorValues motor_values = compute_proportional(left_val,right_val);

      motor(0, motor_values.left_motor);
      motor(1, motor_values.right_motor);
   }
}


MotorValues compute_proportional(int left_val, int right_val) {
   MotorValues mv;
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

   mv.left_motor = left;
   mv.right_motor = right;

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