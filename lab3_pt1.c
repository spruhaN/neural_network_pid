// Name: Sarah Hallam, Aiden Jacob, and Spruha Nayak
// Assignment: Lab 1 part 4
// Description: Based on Bumblebee's position, its scrolls 416 across the screen accordingly.

#include "globals.h"
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>


int main(void){
   init();
   int i =0;
   int button_state = 0;

    while(1){
        // getting ascceleiromeyter values
        clear_screen();
        print_num(i);

        _delay_ms(100);
        if (get_btn() && (button_state == 0)){ // if button is pressed for the first time
            button_state = 1;
         }else if (!get_btn() && (button_state == 1)){ // if button is released (has been pressed before and is currently not pressed)
            button_state = 0;
            i++;
         }
    }

    return 0;
}