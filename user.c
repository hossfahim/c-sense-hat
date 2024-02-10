/*
 * User.c
 *
 *  Created on: 28/01/24
 *      Author: Baptiste
 */


#include "user.h"

int UserActivated = 0;

pthread_barrier_t 	UserStartBarrier;


void *UserTask ( void *ptr ) {

	printf("%s UserInit\n", __FUNCTION__);
	UserStruct *User = (UserStruct *) ptr;
	
	pthread_barrier_wait(&UserStartBarrier);
	
	while(UserActivated == 1){
		
		
		//Attente valeur joystick (mutex)
		
		pthread_mutex_lock(&(User->Mutex));
		printf("%s UserWait\n", __FUNCTION__);
		if (UserActivated == 0)
					break;
					
		//Traitement event (User->Keys)
		//recuperer T ambiante
		User->Ta = getTemperature();
		//Changement T désiré
		
		if(User->Keys == KEY_DOWN){
			User->Td -=INCR_TEMP;
		}else if (User->Keys == KEY_UP){
			User->Td +=INCR_TEMP;
		}
		if(User->Td<5){
			User->Td = 5;
		}
		if(User->Td>30){
			User->Td = 30;
		}
		//Calcule Puissance
		float P = ((User->Td - User->Ta )/6)*100;
		//Puissance Doit etre Entre 0 et 100
		if(P<0){
			P = 0;
		}
		if(P>100){
			P = 100;
		}

		//Send to Afficheur
		sendToGUI(User->client_socket,0,User->Td);
		sendToGUI(User->client_socket,1,P);
		
	}
	
	printf("%s : Terminé\n", __FUNCTION__);
	pthread_exit(0); /* exit thread */
}


int UserInit (UserStruct *User) {

	pthread_attr_t		attr;
	struct sched_param	param;
	int					minprio, maxprio;

	pthread_mutex_init(&(User->Mutex),NULL);
	

	int cr = pthread_barrier_init(&UserStartBarrier, NULL, 2);
	

	pthread_attr_init(&attr);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	minprio = sched_get_priority_min(POLICY);
	maxprio = sched_get_priority_max(POLICY);
	pthread_attr_setschedpolicy(&attr, POLICY);
	param.sched_priority = minprio + (maxprio - minprio)/2;
	pthread_attr_setstacksize(&attr, THREADSTACK);
	pthread_attr_setschedparam(&attr, &param);

	
	pthread_create( &User->Thread, &attr, &UserTask, User);
	pthread_attr_destroy(&attr);
	
	//User->Td = 10;

	return 0;
}



int UserStart (void) {
                                            
	UserActivated = 1;
	
	pthread_barrier_wait(&UserStartBarrier);

	pthread_barrier_destroy(&UserStartBarrier);
	printf("%s User démarré\n", __FUNCTION__);

	return 0;
}



int UserStop (UserStruct *User) {


	UserActivated = 0;

	pthread_mutex_unlock(&(User->Mutex));

	pthread_join(User->Thread,NULL);
	
	pthread_mutex_destroy(&(User->Mutex));
	
	return 0;
}
