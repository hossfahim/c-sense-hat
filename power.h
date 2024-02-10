

#ifndef POWER_H
#define POWER_H

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "user.h"
#include "pressure.h"

typedef struct pow_struct {
	
	UserStruct 	Usr;
	TempStruct 	Temp;
	pthread_t	Thread;
	pthread_mutex_t Mutex;
	int client_socket;

} PowStruct;

void* PowTask(void* ptr);
int PowInit (PowStruct *Pow);
int PowStart (void);
int PowStop (PowStruct *Pow);

#endif
