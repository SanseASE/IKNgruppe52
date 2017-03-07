//============================================================================
// Name        : file_server.cpp
// Author      : Lars Mortensen
// Version     : 1.0
// Description : file_server in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iknlib.h>

using namespace std;


/**
 * main starter serveren og venter på en forbindelse fra en klient
 * Læser filnavn som kommer fra klienten.
 * Undersøger om filens findes på serveren.
 * Sender filstørrelsen tilbage til klienten (0 = Filens findes ikke)
 * Hvis filen findes sendes den nu til klienten
 *
 * HUSK at lukke forbindelsen til klienten og filen nÃ¥r denne er sendt til klienten
 *
 * @throws IOException
 *
 */

    /* error handling*/
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

    /* Sender fil */

int sendFile(int socket){


    FILE *picture;
    int size, read_size, stat, packet_index;
    char send_buffer[10240], read_buffer[256];
    packet_index = 1;

    picture = fopen("kage.jpg", "r");
    printf("Getting Picture Size\n");

    if(picture == NULL) {
         printf("Error Opening Image File"); }

    fseek(picture, 0, SEEK_END);
    size = ftell(picture);
    fseek(picture, 0, SEEK_SET);
    printf("Total Picture size: %i\n",size);

    //Send Picture Size
    printf("Sending Picture Size\n");
    write(socket, (void *)&size, sizeof(int));

    //Send Picture as Byte Array
    printf("Sending Picture as Byte Array\n");

    do { //Read while we get errors that are due to signals.
       stat=read(socket, &read_buffer , 255);
       printf("Bytes read: %i\n",stat);
    } while (stat < 0);

    printf("Received data in socket\n");
    printf("Socket data: %c\n", read_buffer);

    while(!feof(picture)) {
    //while(packet_index = 1){
       //Read from the file into our send buffer
       read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);

       //Send data through our socket
       do{
         stat = write(socket, send_buffer, read_size);
       }while (stat < 0);

       printf("Packet Number: %i\n",packet_index);
       printf("Packet Size Sent: %i\n",read_size);
       printf(" \n");
       printf(" \n");


       packet_index++;

       //Zero out our send buffer
       bzero(send_buffer, sizeof(send_buffer));
      }
     }


int main(int argc, char *argv[])
{

    int sockfd;
    int newsockfd;
    int portno;
    socklen_t clilen;


        /******* Server opsætning *********/

    struct sockaddr_in serv_addr, cli_addr;

    /* error handling for port number*/
    if (argc < 2) {
             fprintf(stderr,"ERROR, ingen port stillet til rådighed\n");
             exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0); //address domain of the socket, type of socket, protocol
         if (sockfd < 0)
            error("ERROR ved åbning af socket");

     bzero((char *) &serv_addr, sizeof(serv_addr)); //sets all values in a buffer to zero

     portno = atoi(argv[1]); // converts portnumber argument string of degits into an integer


     serv_addr.sin_family = AF_INET;
     serv_addr.sin_port = htons(portno);
     serv_addr.sin_addr.s_addr = INADDR_ANY; // <--- muligvis server adresss


     if (bind(sockfd, (struct sockaddr *) &serv_addr, // binds a socket to an address
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");

     listen(sockfd,5);
     clilen = sizeof(cli_addr);


     /******* Venter på accept *********/

     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); //blocks process until a client connects to the server
     if (newsockfd < 0)
       error("ERROR on accept");


     /******* Fil udveksling  *********/

     fflush(stdout);

     if (new_socket<0)
     {
       perror("Accept Failed");
       return 1;
     }

      sendFile(newsockfd);

      close(sockfd);
      fflush(stdout);

     return 0;
}

