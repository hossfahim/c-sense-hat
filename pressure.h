#ifndef PRESSURE_H
#define PRESSURE_H

#include <fcntl.h>
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <pthread.h>
#include "user.h"


void delay2(int);
double getPressure();
double getTemperature();

typedef struct press_struct {
	float		p;
	int client_socket;
	pthread_t	Thread;
	pthread_mutex_t Mutex;

} PressStruct;

void* PressTask ( void* ptr );
int PressInit (PressStruct *Press);
int PressStart (void);
int PressStop (PressStruct *Press);

typedef struct temp_struct {
	float		Ta;
	int client_socket;
	pthread_t	Thread;
	pthread_mutex_t Mutex;

} TempStruct;

void* TempTask(void* ptr);
int TempInit (TempStruct *Temp);
int TempStart (void);
int TempStop (TempStruct *Temp);

#endif
