



/*
 *  C code to read humidity and temperature from the
 *  Raspberry Pi Sense HAT add-on board (HTS221 sensor)
 *
 *  sudo raspi-config --> interfacing options --> enable i2c
 *
 *  sudo apt install libi2c-dev
 *
 *  Build with:  gcc -Wall -O2 humidity.c -o humidity -li2c
 *               or just 'make'
 *
 *  Tested with:  Sense HAT v1.0 / Raspberry Pi 3 B+ / Raspbian GNU/Linux 10 (buster)
 *
 */
#include "humidity.h"
#include "user.h"

#define DEV_PATH "/dev/i2c-1"
#define DEV_ID 0x5F
#define WHO_AM_I 0x0F

#define CTRL_REG1 0x20
#define CTRL_REG2 0x21

#define T0_OUT_L 0x3C
#define T0_OUT_H 0x3D
#define T1_OUT_L 0x3E
#define T1_OUT_H 0x3F
#define T0_degC_x8 0x32
#define T1_degC_x8 0x33
#define T1_T0_MSB 0x35

#define TEMP_OUT_L 0x2A
#define TEMP_OUT_H 0x2B

#define H0_T0_OUT_L 0x36
#define H0_T0_OUT_H 0x37
#define H1_T0_OUT_L 0x3A
#define H1_T0_OUT_H 0x3B
#define H0_rH_x2 0x30
#define H1_rH_x2 0x31

#define H_T_OUT_L 0x28
#define H_T_OUT_H 0x29

int HumActivated = 0;
pthread_barrier_t HumStartBarrier;

void delay1(int);

double getHumidity() {
    int fd = 0;
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
    if (i2c_smbus_read_byte_data(fd, WHO_AM_I) != 0xBC) {
        printf("%s\n", "who_am_i error");
        close(fd);
        exit(1);
    }

    /* Power down the device (clean start) */
    i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x00);

    /* Turn on the humidity sensor analog front end in single shot mode  */
    i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x84);

    /* Run one-shot measurement (temperature and humidity). The set bit will be reset by the
     * sensor itself after execution (self-clearing bit) */
    i2c_smbus_write_byte_data(fd, CTRL_REG2, 0x01);

    /* Wait until the measurement is completed */
    do {
        delay1(25); /* 25 milliseconds */
        status = i2c_smbus_read_byte_data(fd, CTRL_REG2);
    } while (status != 0);



    /* Read calibration relative humidity LSB (ADC) data
     * (humidity calibration x-data for two points)
     */
    uint8_t h0_out_l = i2c_smbus_read_byte_data(fd, H0_T0_OUT_L);
    uint8_t h0_out_h = i2c_smbus_read_byte_data(fd, H0_T0_OUT_H);
    uint8_t h1_out_l = i2c_smbus_read_byte_data(fd, H1_T0_OUT_L);
    uint8_t h1_out_h = i2c_smbus_read_byte_data(fd, H1_T0_OUT_H);

    /* Read relative humidity (% rH) data
     * (humidity calibration y-data for two points)
     */
    uint8_t h0_rh_x2 = i2c_smbus_read_byte_data(fd, H0_rH_x2);
    uint8_t h1_rh_x2 = i2c_smbus_read_byte_data(fd, H1_rH_x2);



    /* make 16 bit values (bit shift)
     * (humidity calibration x-values)
     */
    int16_t H0_T0_OUT = h0_out_h << 8 | h0_out_l;
    int16_t H1_T0_OUT = h1_out_h << 8 | h1_out_l;





    /* Humidity calibration values
     * (humidity calibration y-values)
     */
    double H0_rH = h0_rh_x2 / 2.0;
    double H1_rH = h1_rh_x2 / 2.0;

    /* Solve the linear equasions 'y = mx + c' to give the
     * calibration straight line graphs for temperature and humidity
     */


    double h_gradient_m = (H1_rH - H0_rH) / (H1_T0_OUT - H0_T0_OUT);
    double h_intercept_c = H1_rH - (h_gradient_m * H1_T0_OUT);




    /* Read the ambient humidity measurement (2 bytes to read) */
    uint8_t h_t_out_l = i2c_smbus_read_byte_data(fd, H_T_OUT_L);
    uint8_t h_t_out_h = i2c_smbus_read_byte_data(fd, H_T_OUT_H);

    /* make 16 bit value */
    int16_t H_T_OUT = h_t_out_h << 8 | h_t_out_l;



    /* Calculate ambient humidity */
    double H_rH = (h_gradient_m * H_T_OUT) + h_intercept_c;


    /* Power down the device */
    i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x00);
    close(fd);

    return H_rH;
}

void delay1(int t) {
    usleep(t * 1000);
}

void* HumTask(void* ptr){
    printf("%s UserInit\n", __FUNCTION__);
	HumStruct* Hum = (HumStruct*) ptr;
	
	pthread_barrier_wait(&HumStartBarrier);
	
	while(HumActivated == 1){
		printf("%s UserActivated\n", __FUNCTION__);
		
	
		pthread_mutex_lock(&(Hum->Mutex));
		printf("%s HumWait\n", __FUNCTION__);
		if (HumActivated == 0)
					break;
					
		//recuperer Humidity
		Hum->hum = getHumidity();

		//Send to Afficheur
		sendToGUI(Hum->client_socket,4,Hum->hum);
		
    
    }
    printf("%s : Terminé\n", __FUNCTION__);
	pthread_exit(0); /* exit thread */
}
int HumInit(HumStruct* Hum){
    
    pthread_attr_t		attr;
	struct sched_param	param;
	int					minprio, maxprio;

	pthread_mutex_init(&(Hum->Mutex),NULL);
	


	int cr = pthread_barrier_init(&HumStartBarrier, NULL, 2);

	pthread_attr_init(&attr);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	minprio = sched_get_priority_min(POLICY);
	maxprio = sched_get_priority_max(POLICY);
	pthread_attr_setschedpolicy(&attr, POLICY);
	param.sched_priority = (maxprio - minprio)/2;
	pthread_attr_setstacksize(&attr, THREADSTACK);
	pthread_attr_setschedparam(&attr, &param);

	pthread_create( &Hum->Thread, &attr, &HumTask, Hum);
	pthread_attr_destroy(&attr);

	Hum->hum = getHumidity();

	return 0;
    
    }
    
int HumStart(){
    
    HumActivated = 1;
	printf("%s HumActivated\n", __FUNCTION__);
	pthread_barrier_wait(&HumStartBarrier);

	pthread_barrier_destroy(&HumStartBarrier);
	printf("%s Humidity démarré\n", __FUNCTION__);

	return 0;
    
    
    }
	
int HumStop(HumStruct* Hum){
 
 HumActivated = 0;

	
	pthread_mutex_unlock(&(Hum->Mutex));

	pthread_join(Hum->Thread,NULL);
	
	pthread_mutex_destroy(&(Hum->Mutex));
	
	return 0;
}
