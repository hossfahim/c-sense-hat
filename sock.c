
#include "sock.h"


int getOpenSocket(char* HOST){
    
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
    
    return client_socket;
    
    }

void sendToGUI(int client_socket, int type, float val){
    double value = val;
    char Buff[10]; // 10
switch (type){
    
    case 0:
    //SEND DESIRED TEMPERATURE TO GUI

        sprintf(Buff, "TD%f\n", value);

        if (send(client_socket,Buff, strlen(Buff), 0) == -1)
        {
            perror("Error sending data");
            close(client_socket);
            exit(EXIT_FAILURE);
        }
    
    break;
    
    case 1:
    //SEND POWER TO GUI

        sprintf(Buff, "PW%f\n", value);

        if (send(client_socket, Buff, strlen(Buff), 0) == -1)
        {
            perror("Error sending data");
            close(client_socket);
            exit(EXIT_FAILURE);
        }
   
    break;
    
    case 2:
    // SEND AMBIANT TEMPERATURE TO GUI
       
        sprintf(Buff, "TP%f\n", value);

        if (send(client_socket, Buff, strlen(Buff), 0) == -1)
        {
            perror("Error sending data");
            close(client_socket);
            exit(EXIT_FAILURE);
        }
    
    break;
    
    case 3:
    // SEND PRESSURE TO GUI
    
        sprintf(Buff, "PR%f\n", value);

        if (send(client_socket, Buff, strlen(Buff), 0) == -1)
        {
            perror("Error sending data");
            close(client_socket);
            exit(EXIT_FAILURE);
        }
    break;
    
    case 4:
    //SEND HUMIDITY to GUI
    
        sprintf(Buff, "HU%f\n", value);

        if (send(client_socket, Buff, strlen(Buff), 0) == -1)
        {
            perror("Error sending data");
            close(client_socket);
            exit(EXIT_FAILURE);
        }
    
    }    

}
