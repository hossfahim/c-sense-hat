#include "sock.h"
#include "pressure.h"
#include "humidity.h"
#include "user.h"
#include <sys/time.h>
#include "SenseHat/senseHat.h"

// REPLACE BY YOUR RPI IP ADRESS IN YOUR LOCAL NETWORK
const char *HOST = "192.168.0.59";

void getSignal()
{
	
	PressStart();
	TempStart();
	
	
}




int main() {
	


int client_socket = getOpenSocket(HOST);


	TempStruct Temp;
	PressStruct Press;
	Temp.client_socket = client_socket;
	Press.client_socket = client_socket;
	
	TempInit(&Temp);
	PressInit(&Press);

     //Timer pour envoi periodique
	
	struct itimerval timer;
	
	signal(SIGALRM,getSignal);
	
	timer.it_interval.tv_sec = 1;
	timer.it_value = timer.it_interval;
	
	setitimer(ITIMER_REAL, & timer, NULL);

while(1)
pause();

	UserStruct User;
	//Initialiser temperature desiree
	User.Td = 15;
	User.client_socket = client_socket;
	
	SenseHat *s = SenseHat_creer();
	
	UserInit(&User);
	
	UserStart();
	printf("Waitinf for Joystick\n");

	User.Keys = SenseHat_recupererEtatJoystick(s); //Commenter si FICTIF
	while(User.Keys != KEY_ENTER){
		
		printf("enter : %d\n", User.Keys);
		
		if(User.Keys != 0){
			//Update Temp Ambiante
		  User.Ta = getTemperature();
			//Unlock mutex
			pthread_mutex_unlock(&(User.Mutex));
		}
		
		User.Keys = SenseHat_recupererEtatJoystick(s); //Commenter si FICTIF
	}
	
	UserStop(&User);
       
 
	
       
       return 0;
       
	}
