#ifndef PRESSURE_H   /* Include guard */
#define PRESSURE_H


#define DEV_ID 0x5c
#define DEV_PATH "/dev/i2c-1"
#define WHO_AM_I 0x0F
#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define PRESS_OUT_XL 0x28
#define PRESS_OUT_L 0x29
#define PRESS_OUT_H 0x2A
#define TEMP_OUT_L 0x2B
#define TEMP_OUT_H 0x2C

void delay2(int); // to avoid conflicts with the one defined in humidity.h
void getPressure(double *, double *);

#endif