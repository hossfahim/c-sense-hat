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

// REPLACE BY YOUR RPI IP ADRESS IN YOUR LOCAL NETWORK
const char *HOST = "192.168.0.10";

int main()
{

    int client_socket;
    struct sockaddr_in server_addr;
    char T_DegC_Buffer[T_DegC_BuffER_SIZE];

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(HOST);
    server_addr.sin_port = htons(PORT);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Error connecting to server");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    //----------------------------------------------------------------------------------------------------------------------
    SenseHat *s = SenseHat_creer();
    int joystickInput;
    double desiredTemperature = 0.0;
    double power = 0.0;
    while (1)
    {
        // TO READ THE JOYSTICK INPUT VALUES
        // check SenseHat/senseHat.c
        /*
        case KEY_ENTER :  return 5;
        case KEY_UP    :  return 1;
        case KEY_RIGHT :  return 2;
        case KEY_DOWN  :  return 3;
        case KEY_LEFT  :  return 4;
        */
        joystickInput = SenseHat_recupererEtatJoystick(s);
        desiredTemperature = desiredTemperature + 1.05; // JUST TO TEST THE DATA TRANSFER
        power = power + 3.85; // JUST TO TEST THE DATA TRANSFER

        printf("%d\n", joystickInput);
        //----------------------------------------------------------------------
        // SEND DESIRED TEMPERATURE TO GUI
        char desiredTemperature_Buff[10]; // 10
        sprintf(desiredTemperature_Buff, "TD%f\n", desiredTemperature);

        if (send(client_socket, desiredTemperature_Buff, strlen(desiredTemperature_Buff), 0) == -1)
        {
            perror("Error sending data");
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        //----------------------------------------------------------------------
        // SEND POWER TO GUI
        
        char power_Buff[10]; // 10
        sprintf(power_Buff, "PW%f\n", power);

        if (send(client_socket, power_Buff, strlen(power_Buff), 0) == -1)
        {
            perror("Error sending data");
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        continue; // TO REMOVE IF YOU WANT TO SEE THE HUMIDITY, TEMPERATURE AND PRESSION UPDATED IN THE GUI

        //----------------------------------------------------------------------
        // HUMIDITY AND TEMPERATURE FROM HUMIDITY
        double T_DegC, H_rH;
        getHumidity(&T_DegC, &H_rH);
        printf("Temp (from humid) = %.1f°C\n", T_DegC);
        printf("Humidity = %.0f%% rH\n", H_rH);

        //----------------------------------------------------------------------
        // SEND TEMPERATURE TO GUI
        char T_DegC_Buff[10]; // 10
        sprintf(T_DegC_Buff, "TP%f\n", T_DegC);

        if (send(client_socket, T_DegC_Buff, strlen(T_DegC_Buff), 0) == -1)
        {
            perror("Error sending data");
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        //----------------------------------------------------------------------
        // SEND HUMIDITY TO GUI
        char H_rH_Buff[10]; // 10
        sprintf(H_rH_Buff, "HU%f\n", H_rH);

        if (send(client_socket, H_rH_Buff, strlen(H_rH_Buff), 0) == -1)
        {
            perror("Error sending data");
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        //----------------------------------------------------------------------
        // PRESSURE AND TEMPERATURE FROM PRESSURE
        double t_c, pressure;
        getPressure(&t_c, &pressure);
        printf("Temp (from press) = %.2f°C\n", t_c);
        printf("Pressure = %.0f hPa\n", pressure);

        //----------------------------------------------------------------------
        // SEND PRESSURE TO GUI
        char pressure_Buff[10]; // 10
        sprintf(pressure_Buff, "PR%f\n", pressure);

        if (send(client_socket, pressure_Buff, strlen(pressure_Buff), 0) == -1)
        {
            perror("Error sending data");
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        sleep(1);
    }

    // // Receive a response from the server
    // ssize_t bytes_received = recv(client_socket, T_DegC_Buffer, sizeof(T_DegC_Buffer), 0);
    // if (bytes_received == -1) {
    //     perror("Error receiving response");
    //     close(client_socket);
    //     exit(EXIT_FAILURE);
    // }

    // T_DegC_Buffer[bytes_received] = '\0';
    // printf("Received response from server: %s\n", T_DegC_Buffer);

    // Close the socket
    close(client_socket);

    return 0;
}
