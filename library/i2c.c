#include "globals.h"
#include <util/delay.h>
#include <avr/io.h>

void i2c_start() {
   //set both to high
   I2C_DDR &= ~( _BV(SDA_PIN) | _BV(SCL_PIN) ); //make as input to set high
   _delay_us(I2C_DELAY);

   SDA_LO;
   _delay_us(I2C_DELAY);

   SCL_LO;
   _delay_us(I2C_DELAY);
}


void i2c_stop() {
   SCL_HI;
   _delay_us(I2C_DELAY);

   SDA_HI;
   _delay_us(I2C_DELAY);
}

void unlock_bus() {
   SDA_HI;
   _delay_us(I2C_DELAY);

   while (!(I2C_PIN & _BV(SDA_PIN))) {
      SCL_HI;
      _delay_us(I2C_DELAY);
      SCL_LO;
      _delay_us(I2C_DELAY);
   }

   SCL_HI;
}

void clock_scl() {
      SCL_HI;
      _delay_us(I2C_DELAY);
      SCL_LO;
      _delay_us(I2C_DELAY);
}

//send the register address to read
void send_address(u08 addr, u08 reg, u08 read) {
   u08 i;
   //u08 addr = 0x1C; //MMA8453 accelerometer
   //addr = 0x1E; //compass

   unlock_bus();

   i2c_start(); //send start bit

   //send address bits 6:0
   for (i=0;i<7;i++) {
      if (addr & _BV(6)) { //send 1
         SDA_HI;
      } else { //send 0
         SDA_LO;
      }

      _delay_us(I2C_DELAY);
      clock_scl();

      addr = addr << 1;
   }

   //send W bit of 0
   SDA_LO;
   _delay_us(I2C_DELAY);
   clock_scl();

   SDA_HI; //release the data line
   _delay_us(1);
   SCL_HI;

   //read the ACK
   if (I2C_PIN & _BV(SDA_PIN)) {
      print_string ("addrNACK");
      while(1){}
   } else {
      //received ACK
      //print_string ("ACK",3);
   }
   _delay_us(I2C_DELAY);
   SCL_LO;
   _delay_us(I2C_DELAY);

   /////////send the register address
   //register address
   for (i=0;i<8;i++) {
      if (reg & _BV(7)) { //send 1
         SDA_HI;
      } else { //send 0
         SDA_LO;
      }

      _delay_us(I2C_DELAY);
      clock_scl();

      reg = reg << 1;
   }

   SDA_HI; //release the data line
   _delay_us(1);
   SCL_HI;

   //read the ACK
   if (I2C_PIN & _BV(SDA_PIN)) {
      print_string ("NACK");
      while(1){}
   } else {
      //received ACK
      //print_string ("ACK",3);
   }
   _delay_us(I2C_DELAY);
   SCL_LO;
   _delay_us(I2C_DELAY);

   if (read)
      i2c_stop();
}

/* */
void write_register(u08* data, u08 num) {
   u08 i,j;

   for (j=0;j<num;j++) {
      //send data bits 7:0
      for (i=0;i<8;i++) {
         if (*data & _BV(7)) { //send 1
            SDA_HI;
         } else { //send 0
            SDA_LO;
         }

         _delay_us(I2C_DELAY);
         clock_scl();

         *data = *data << 1;
      }

      data++;

      SDA_HI; //release the data line
      _delay_us(1);
      SCL_HI;

      //read the ACK
      if (I2C_PIN & _BV(SDA_PIN)) {
         print_string ("wr_NACK");
         while(1){}
      } else {
      }

      _delay_us(I2C_DELAY);
      SCL_LO;
      _delay_us(I2C_DELAY);
   }

   i2c_stop();

}

void read_register(u08 addr, u08* data, u08 num) {
   u08 i,j;
   //u08 addr = MMA8453_ADDR;
   //addr = COMPASS_ADDR; //compass

   i2c_start(); //send start bit

   //send device address bits 6:0
   for (i=0;i<7;i++) {
      if (addr & _BV(6)) { //send 1
         SDA_HI;
      } else { //send 0
         SDA_LO;
      }

      _delay_us(I2C_DELAY);
      clock_scl();

      addr = addr << 1;
   }

   //send R bit of 1
   SDA_HI;
   _delay_us(I2C_DELAY);
   clock_scl();

   SDA_HI; //release the data line
   _delay_us(1);
   SCL_HI;

   //read the ACK
   if (I2C_PIN & _BV(SDA_PIN)) {
      print_string ("readNACK");
      while(1) {}
   } else {
      //received ACK
      //print_string ("ACK",3);
   }
   _delay_us(I2C_DELAY);
   SCL_LO;
   _delay_us(I2C_DELAY);

   /////////read the data from the device
   SDA_HI; //release the data line
   _delay_us(1);
   for (j=0;j<num;j++) {
      for (i=0;i<8;i++) {
         *data = *data << 1;
         SCL_HI;
         //_delay_us(I2C_DELAY/2);
         _delay_us(1);

         if (I2C_PIN & _BV(SDA_PIN)) {
            *data |= 1;
         }

         //_delay_us(I2C_DELAY/2);
         _delay_us(1);
         SCL_LO;

         _delay_us(I2C_DELAY);
      }

      //master sends ACK
      SDA_LO;
      _delay_us(1);
      clock_scl();
      
      data++; //move pointer to the next byte
   }

   _delay_us(1);

   i2c_stop();
}

void i2c_regwrite(u08 dev_addr, u08 address, u08 data) {
   send_address(dev_addr, address,0);
   write_register(&data,1);
}

u08 i2c_regread(u08 dev_addr, u08 address) {
   u08 temp;
   send_address(dev_addr, address,1);
   read_register(dev_addr, &temp,1);
   return temp;
}

u08 get_accel_x() {
   return i2c_regread(MMA8453_ADDR,0x1);
}

u08 get_accel_y() {
   return i2c_regread(MMA8453_ADDR,0x3);
}

u08 get_accel_z() {
   return i2c_regread(MMA8453_ADDR,0x5);
}
