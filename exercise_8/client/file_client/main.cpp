//============================================================================
// Name        : file_client.cpp
// Author      : Ragnar, Thomas og Benjamin
// Version     : 1.2
// Description : file_client UDP in C++
//============================================================================

#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>

using namespace std;

#define BUFLEN 512  //Max length of buffer
#define PORT 9000   //The port on which to send data

void errormsg(char *msg)
{
    perror(msg);    //write out system error message
    exit(1);        //continue the program without quitting it
}

int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr;
    int socketfd, slen=sizeof(serv_addr);
    char buffer[BUFLEN];
    char message[BUFLEN];

    //create a UDP socket
    if ((socketfd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        errormsg((char *)"socket");
    }

    // zero out the structure
    memset((char *) &serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    //Insert IP adress in socket "argv[1]"
    if (inet_aton(argv[1] , &serv_addr.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    //keep listening for data
    while(1)
    {
        cout << "Enter u/U (uptime) or l/L (Loadtime): ";
        fgets(message, BUFLEN, stdin);

        //send the command to the server
        if (sendto(socketfd, message, strlen(message) , 0 , (struct sockaddr *) &serv_addr, slen)==-1)
        {
            errormsg((char *)"sendto()");
        }

        //clear the buffer by filling null, it might have previously received data
        memset(buffer,'\0', BUFLEN);

        //try to receive some data, this is a blocking call
        if (recvfrom(socketfd, buffer, BUFLEN, 0, (struct sockaddr *) &serv_addr, (socklen_t*)&slen) == -1)
        {
            errormsg((char *)"recvfrom()");
        }

        cout << buffer << endl;
    }

    close(socketfd);
    return 0;
}
