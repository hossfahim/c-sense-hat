/*
 *  C code to read humidity and temperature from the
 *  Raspberry Pi Sense HAT add-on board (HTS221 sensor)
 *
 *  sudo raspi-config --> interfacing options --> enable i2c
 *
 *  sudo apt install libi2c-dev
 *
 *  Build with:  gcc -Wall -O2 humidity.c -o humidity -li2c
 *               or just 'make'
 *
 *  Tested with:  Sense HAT v1.0 / Raspberry Pi 3 B+ / Raspbian GNU/Linux 10 (buster)
 *
 */

#ifndef HUMIDITY_H   /* Include guard */
#define HUMIDITY_H


#define DEV_PATH "/dev/i2c-1"
#define DEV_ID 0x5F
#define WHO_AM_I 0x0F

#define CTRL_REG1 0x20
#define CTRL_REG2 0x21

#define T0_OUT_L 0x3C
#define T0_OUT_H 0x3D
#define T1_OUT_L 0x3E
#define T1_OUT_H 0x3F
#define T0_degC_x8 0x32
#define T1_degC_x8 0x33
#define T1_T0_MSB 0x35

#define TEMP_OUT_L 0x2A
#define TEMP_OUT_H 0x2B

#define H0_T0_OUT_L 0x36
#define H0_T0_OUT_H 0x37
#define H1_T0_OUT_L 0x3A
#define H1_T0_OUT_H 0x3B
#define H0_rH_x2 0x30
#define H1_rH_x2 0x31

#define H_T_OUT_L 0x28
#define H_T_OUT_H 0x29



void delay(int t);
void getHumidity(double *, double *);

#endif