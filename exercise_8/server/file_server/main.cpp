//============================================================================
// Name        : file_server.cpp
// Author      : Ragnar, Thomas og Benjamin
// Version     : 1.2
// Description : file_server UDP in C++
//============================================================================

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define BUFLEN 512  //Max length of buffer
#define PORT 9000   //The port on which to listen for incoming data

using namespace std;
void findData(char *buffer);

void errormsg(char *msg)
{
    perror(msg);    //write out system error message
    exit(1);        //continue the program without quitting it
}

int main(int argc, char *argv[])
{
    struct sockaddr_in si_me, serv_addr;
    int socketfd, slen = sizeof(serv_addr) , recv_len;
    char buffer[BUFLEN];

    //create a UDP socket
    if ((socketfd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        errormsg((char *)"socket");
    }

    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if( bind(socketfd , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        errormsg((char *)"bind");
    }

    //keep listening for data
    while(1)
    {
        cout << "Waiting for data from client...";
        fflush(stdout);

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(socketfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &serv_addr, (socklen_t*)&slen)) == -1)
        {
            errormsg((char *)"recvfrom()");
        }

        //print details of the client/peer and the data received
        cout << endl << "Received data from: " << inet_ntoa(serv_addr.sin_addr) << endl;

        //find out what the user asked for
        findData(buffer);

        //print out the data from the user
        cout << buffer << endl;

        //now reply the client with the same data
        if (sendto(socketfd, buffer, sizeof(buffer), 0, (struct sockaddr*) &serv_addr, slen) == -1)
        {
            errormsg((char *)"sendto()");
        }
    }

    close(socketfd);
    return 0;
}

void findData(char *buffer)
{
    int file;

    //if u/U
    if (buffer[0] == 'u' || buffer[0] == 'U')
    {
        memset(buffer, 0, BUFLEN); // zero out the buffer
        if ((file = open("/proc/uptime", O_RDONLY)) == -1)
        {
         errormsg((char *)"Can't open file: /proc/uptime");
        }
        read(file, buffer, BUFLEN);
    }

    //if l/L
    else if (buffer[0] == 'l' || buffer[0] == 'L')
    {
        memset(buffer, 0, BUFLEN); // zero out the buffer
        if ((file = open("/proc/loadavg", O_RDONLY)) == -1)
        {
         errormsg((char *) "Can't open file: /proc/loadavg");
        }
        read(file, buffer, BUFLEN);
    }

    else
    {
        memset(buffer, 0, BUFLEN); // zero out the buffer
        strncpy(buffer, "I told you to ONLY write u/U or l/L... tsk tsk..\n", BUFLEN);
    }

    close(file);

    return;
}
