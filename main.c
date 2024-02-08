#include "sock.h"
#include "pressure.h"
#include "humidity.h"

// REPLACE BY YOUR RPI IP ADRESS IN YOUR LOCAL NETWORK
const char *HOST = "192.168.0.59";

int main() {
	
	int client_socket = getOpenSocket(HOST);
	while(1){
		
		double Td = 25.0;
		
		double Ta = getTemperature();
		double power = ((Ta - Td )/6)*100;
		double p = getPressure();
		double hum = getHumidity();
		
		sendToGUI(client_socket,0, Td);
		sendToGUI(client_socket,1, Ta);
		sendToGUI(client_socket,2, Ta);
		sendToGUI(client_socket,3, p);
		sendToGUI(client_socket,4,hum);
		sleep(1);
		
		
		
		}
	return 0;
	}
