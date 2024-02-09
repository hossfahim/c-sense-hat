

#ifndef HUMIDITY_H
#define HUMIDITY_H

#include <pthread.h>
#include <fcntl.h>
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>


void delay1 (int);
double getHumidity();

typedef struct hum_struct {
	float		hum;
	pthread_t	Thread;
	pthread_mutex_t Mutex;
	int client_socket;

} HumStruct;

void* HumTask(void* ptr);
int HumInit (HumStruct *Temp);
int HumStart (void);
int HumStop (HumStruct *Temp);

#endif
