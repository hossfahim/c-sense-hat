/*
 * user.c
 *
 *  Created on: 28/01/24
 *      Author: Baptiste
 */

#ifndef USER_H_
#define USER_H_

#include <stdio.h>   /* Standard input/output definitions */
#include <stdlib.h>
#include <stdint.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <signal.h>
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include "SenseHat/senseHat.h"
#include "pressure.h"


#define POLICY SCHED_RR
//#define POLICY SCHED_FIFO
#define THREADSTACK 65536

#define INCR_TEMP 1

#define KEY_ENTER 5
#define KEY_UP    1
#define KEY_RIGHT 2
#define KEY_DOWN  3
#define KEY_LEFT  4

typedef struct user_struct {
	float		Ta;
	float		Td;
	pthread_t	Thread;
	int 		Keys;
	sem_t   Sem;
} UserStruct;


int UserInit (UserStruct *User);
int UserStart (void);
int UserStop (UserStruct *User);

#endif /* USER_H_ */
