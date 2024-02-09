/*
 *  C code to read pressure and temperature from the
 *  Raspberry Pi Sense HAT add-on board (LPS25H sensor)
 *
 *  sudo raspi-config --> interfacing options --> enable i2c
 *
 *  sudo apt install libi2c-dev
 *
 *  Build with:   gcc -Wall -O2 pressure.c -o pressure -li2c
 *                or just 'make'
 *
 *  Tested with:  Sense HAT v1.0 / Raspberry Pi 3 B+ / Raspbian GNU/Linux 10 (buster)
 *
 */
#include "pressure.h"


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


int PressActivated = 0;
int TempActivated = 0;

pthread_barrier_t 	PressStartBarrier;
pthread_barrier_t 	TempStartBarrier;

void delay2(int);

double getPressure () {
    int fd = 0;

    uint8_t press_out_xl = 0;
    uint8_t press_out_l = 0;
    uint8_t press_out_h = 0;

    int32_t press_out = 0;
    double pressure = 0.0;

    uint8_t status = 0;

    /* open i2c comms */
    if ((fd = open(DEV_PATH, O_RDWR)) < 0) {
        perror("Unable to open i2c device");
        exit(1);
    }

    /* configure i2c slave */
    if (ioctl(fd, I2C_SLAVE, DEV_ID) < 0) {
        perror("Unable to configure i2c slave device");
        close(fd);
        exit(1);
    }

    /* check we are who we should be */
    if (i2c_smbus_read_byte_data(fd, WHO_AM_I) != 0xBD) {
        printf("%s\n", "who_am_i error");
        close(fd);
        exit(1);
    }

    /* Power down the device (clean start) */
    i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x00);

    /* Turn on the pressure sensor analog front end in single shot mode  */
    i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x84);

    /* Run one-shot measurement (temperature and pressure), the set bit will be reset by the
     * sensor itself after execution (self-clearing bit)
     */
    i2c_smbus_write_byte_data(fd, CTRL_REG2, 0x01);

    /* Wait until the measurement is complete */
    do {
        delay2(25); /* 25 milliseconds */
        status = i2c_smbus_read_byte_data(fd, CTRL_REG2);
    } while (status != 0);



    /* Read the pressure measurement (3 bytes to read) */
    press_out_xl = i2c_smbus_read_byte_data(fd, PRESS_OUT_XL);
    press_out_l = i2c_smbus_read_byte_data(fd, PRESS_OUT_L);
    press_out_h = i2c_smbus_read_byte_data(fd, PRESS_OUT_H);

    /* make 16 and 24 bit values (using bit shift) */
    
    press_out = press_out_h << 16 | press_out_l << 8 | press_out_xl;

    /* calculate output values */

    pressure = press_out / 4096.0;



    /* Power down the device */
    i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x00);

    close(fd);

    return pressure;
}



double getTemperature(){
    
int fd = 0;
    uint8_t temp_out_l = 0, temp_out_h = 0;
    int16_t temp_out = 0;
    double temp = 0.0;

    uint8_t status = 0;

    /* open i2c comms */
    if ((fd = open(DEV_PATH, O_RDWR)) < 0) {
        perror("Unable to open i2c device");
        exit(1);
    }

    /* configure i2c slave */
    if (ioctl(fd, I2C_SLAVE, DEV_ID) < 0) {
        perror("Unable to configure i2c slave device");
        close(fd);
        exit(1);
    }

    /* check we are who we should be */
    if (i2c_smbus_read_byte_data(fd, WHO_AM_I) != 0xBD) {
        printf("%s\n", "who_am_i error");
        close(fd);
        exit(1);
    }

    /* Power down the device (clean start) */
    i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x00);

    /* Turn on the pressure sensor analog front end in single shot mode  */
    i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x84);

    /* Run one-shot measurement (temperature and pressure), the set bit will be reset by the
     * sensor itself after execution (self-clearing bit)
     */
    i2c_smbus_write_byte_data(fd, CTRL_REG2, 0x01);

    /* Wait until the measurement is complete */
    do {
        delay2(25); /* 25 milliseconds */
        status = i2c_smbus_read_byte_data(fd, CTRL_REG2);
    } while (status != 0);



    /* make 16 and 24 bit values (using bit shift) */
    temp_out = temp_out_h << 8 | temp_out_l;
   

    /* calculate output values */
    temp = 42.5 + (temp_out / 480.0);

    /* Power down the device */
    i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x00);

    close(fd);

    return temp;
    
    
    }

void delay2(int t) {
    

    usleep(t * 1000);
}

void* PressTask(void* ptr){
    printf("%s UserInit\n", __FUNCTION__);
	PressStruct* Press = (PressStruct*) ptr;
	
	pthread_barrier_wait(&PressStartBarrier);
	
	while(PressActivated == 1){
		printf("%s UserActivated\n", __FUNCTION__);
		
		//Attente valeur joystick (semaphore)
		//sem_wait(&(User->Sem));
		pthread_mutex_lock(&(Press->Mutex));
		printf("%s PressWait\n", __FUNCTION__);
		if (PressActivated == 0)
					break;
					
		//recuperer Pressure
		Press->p = getPressure();

		//Send to Afficheur
		sendToGUI(Press->client_socket,3,Press->p);
		
		//sem post
    
    }
    printf("%s : Terminé\n", __FUNCTION__);
	pthread_exit(0); /* exit thread */
}
int PressInit(PressStruct* Press){
    
    pthread_attr_t		attr;
	struct sched_param	param;
	int					minprio, maxprio;
	printf("sem ?\n");
	//sem_init(&(User->Sem), 0, 0);
	pthread_mutex_init(&(Press->Mutex),NULL);
	
	printf("barrier ?\n");

	int cr = pthread_barrier_init(&PressStartBarrier, NULL, 2);
	
	printf("thread ?\n");
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

	printf("thread create ?\n");
	pthread_create( &Press->Thread, &attr, &PressTask, Press);
	pthread_attr_destroy(&attr);
	
	printf("temp ?\n");
	Press->p = getPressure();

	return 0;
    
    }
    
int PressStart(){
    
    PressActivated = 1;
	printf("%s PressActivated\n", __FUNCTION__);
	pthread_barrier_wait(&PressStartBarrier);

	pthread_barrier_destroy(&PressStartBarrier);
	printf("%s Pressure démarré\n", __FUNCTION__);

	return 0;
    
    
    }
    
int PressStop(PressStruct* Press){
 
 PressActivated = 0;

	//sem_post (&(User->Sem));
	pthread_mutex_unlock(&(Press->Mutex));

	pthread_join(Press->Thread,NULL);
	
	//sem_destroy(&(User->Sem));
	pthread_mutex_destroy(&(Press->Mutex));
	
	return 0;
}
    
void* TempTask(void* ptr){
    printf("%s UserInit\n", __FUNCTION__);
	TempStruct* Temp = (TempStruct*) ptr;
	
	pthread_barrier_wait(&TempStartBarrier);
	
	while(TempActivated == 1){
		printf("%s UserActivated\n", __FUNCTION__);
		
	
		pthread_mutex_lock(&(Temp->Mutex));
		printf("%s TempWait\n", __FUNCTION__);
		if (TempActivated == 0)
					break;
					
		//recuperer Tempure
		Temp->Ta = getTemperature();

		//Send to Afficheur
		sendToGUI(Temp->client_socket,3,Temp->Ta);
		
		//sem post
    
    }
    printf("%s : Terminé\n", __FUNCTION__);
	pthread_exit(0); /* exit thread */
}
int TempInit(TempStruct* Temp){
    
    pthread_attr_t		attr;
	struct sched_param	param;
	int					minprio, maxprio;
	printf("sem ?\n");
	//sem_init(&(User->Sem), 0, 0);
	pthread_mutex_init(&(Temp->Mutex),NULL);
	
	printf("barrier ?\n");

	int cr = pthread_barrier_init(&TempStartBarrier, NULL, 2);
	
	printf("thread ?\n");
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

	printf("thread create ?\n");
	pthread_create( &Temp->Thread, &attr, &TempTask, Temp);
	pthread_attr_destroy(&attr);
	
	printf("temp ?\n");
	Temp->Ta = getTemperature();

	return 0;
    
    }
    
int TempStart(){
    
    TempActivated = 1;
	printf("%s TempActivated\n", __FUNCTION__);
	pthread_barrier_wait(&TempStartBarrier);

	pthread_barrier_destroy(&TempStartBarrier);
	printf("%s Temperatuure démarré\n", __FUNCTION__);

	return 0;
    
    
    }
    
int TempStop(TempStruct* Temp){
 
 TempActivated = 0;

	//sem_post (&(User->Sem));
	pthread_mutex_unlock(&(Temp->Mutex));

	pthread_join(Temp->Thread,NULL);
	
	//sem_destroy(&(User->Sem));
	pthread_mutex_destroy(&(Temp->Mutex));
	
	return 0;
}
