#include "power.h"

int PowActivated = 0;
pthread_barrier_t PowStartBarrier;


void* PowTask(void* ptr){
    printf("%s PowInit\n", __FUNCTION__);
	PowStruct* Pow = (PowStruct*) ptr;
	
	
	pthread_barrier_wait(&PowStartBarrier);
	
	while(PowActivated == 1){
		printf("%s PowActivated\n", __FUNCTION__);
		
	
		pthread_mutex_lock(&(Pow->Mutex));
		printf("%s PowWait\n", __FUNCTION__);
		if (PowActivated == 0)
					break;
					
		//calcul power
		float pow = ((Pow->Usr.Td - Pow->Temp.Ta)/6)*100;
		if(pow<0){
			pow = 0;
		}
		if(pow>100){
			pow = 100;
		}
		//Send to Afficheur
		sendToGUI(Pow->client_socket,1,pow);
		
    
    }
    printf("%s : Terminé\n", __FUNCTION__);
	pthread_exit(0); /* exit thread */
}
int PowInit(PowStruct* Pow){
    
    pthread_attr_t		attr;
	struct sched_param	param;
	int					minprio, maxprio;

	pthread_mutex_init(&(Pow->Mutex),NULL);
	


	int cr = pthread_barrier_init(&PowStartBarrier, NULL, 2);

	pthread_attr_init(&attr);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	minprio = sched_get_priority_min(POLICY);
	maxprio = sched_get_priority_max(POLICY);
	pthread_attr_setschedpolicy(&attr, POLICY);
	param.sched_priority = (maxprio - minprio)/3;
	pthread_attr_setstacksize(&attr, THREADSTACK);
	pthread_attr_setschedparam(&attr, &param);

	pthread_create( &Pow->Thread, &attr, &PowTask, Pow);
	pthread_attr_destroy(&attr);

	
	return 0;
    
    }
    
int PowStart(){
    
    PowActivated = 1;
	printf("%s PowActivated\n", __FUNCTION__);
	pthread_barrier_wait(&PowStartBarrier);

	pthread_barrier_destroy(&PowStartBarrier);
	printf("%s Power démarré\n", __FUNCTION__);

	return 0;
    
    
    }
	
int PowStop(PowStruct* Pow){
 
 PowActivated = 0;

	
	pthread_mutex_unlock(&(Pow->Mutex));

	pthread_join(Pow->Thread,NULL);
	
	pthread_mutex_destroy(&(Pow->Mutex));
	
	return 0;
}
