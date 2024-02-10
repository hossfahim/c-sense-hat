#include "sock.h"
#include "pressure.h"
#include "humidity.h"
#include "user.h"
#include <sys/time.h>
#include "SenseHat/senseHat.h"

// REPLACE BY YOUR RPI IP ADRESS IN YOUR LOCAL NETWORK
const char *HOST = "192.168.0.59";

 // Variables globales pour envoi periodique
 TempStruct Temp;
 PressStruct Press;
 HumStruct Hum;
 

void getSignal(int u)
{
    

     pthread_mutex_unlock(&(Temp.Mutex));
     pthread_mutex_unlock(&(Press.Mutex));
     pthread_mutex_unlock(&(Hum.Mutex));;
 }
int main() {
    
int client_socket = getOpenSocket(HOST);


// Initialiser les capteurs    
    Temp.Ta = 10;
    Press.p = 10;
    Hum.hum = 10;
    
    Temp.client_socket = client_socket;
    Press.client_socket = client_socket;
    Hum.client_socket = client_socket;

//Creer et demarrer les threads periodique
	
	TempInit(&Temp);
	PressInit(&Press);
	HumInit(&Hum);
	
	TempStart();
	PressStart();
	HumStart();

//Programmer le timer pour taches periodiques
  struct itimerval timer;
    
    signal(SIGALRM,getSignal);
    
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;
    timer.it_value = timer.it_interval;
    
    setitimer(ITIMER_REAL, & timer, NULL);

 //La tache joystick
    UserStruct User;
    //Initialiser temperature desiree
    User.Td = 15;
    User.client_socket = client_socket;
    
    SenseHat *s = SenseHat_creer();
    
    UserInit(&User);
    
    UserStart();
    
    User.Keys = SenseHat_recupererEtatJoystick(s); 
    
    while(User.Keys != KEY_ENTER){
        
        printf("enter : %d\n", User.Keys);
        
        if(User.Keys != 0){
            //Update Temp Ambiante
          User.Ta = Temp.Ta;
            //Unlock mutex
            pthread_mutex_unlock(&(User.Mutex));
        }
        
        User.Keys = SenseHat_recupererEtatJoystick(s); //Commenter si FICTIF
    }
  
     
    UserStop(&User);
       
       return 0;
       }
