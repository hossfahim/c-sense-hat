#ifndef SOCK_H
#define SOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "humidity.h"
#include "pressure.h"
#include "SenseHat/senseHat.h"


// REPLACE BY THE PORT SET IN THE GUI
#define PORT 1234
#define T_DegC_BuffER_SIZE 1024

int getOpenSocket(char* HOST); 
void sendToGUI(int client_socket, int type, float val);

#endif
