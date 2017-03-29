//============================================================================
// Name        : file_client.cpp
// Author      : Gruppe 52
// Version     : 1.0
// Description : file_server in C++, Ansi-style
//============================================================================

#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>
using namespace std;

//This function is to be used once we have confirmed that an image is to be sent
//It should read and output an image file

//Fucntion for receiving image
int receive_image(int socket, char* file_direc)
{ 

//Declaration
int packet_size = 1000;
int recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;

char imagearray[packet_size];
FILE *image;

//Write file request to socket
stat = write(socket, file_direc, 512);

//Verification of file existence
char verification;
read(socket, &verification, sizeof(int));

if(verification == '0')
{
    cout << "error finding the image!" << endl;
    return 0;
}

cout << "succes finding the image!" << endl;

//Find the size of the image
do{
stat = read(socket, &size, sizeof(int));
}while(stat<0);

printf("Packet received.\n");
printf("Packet size: %i\n",stat);
printf("Image size: %i\n",size);
printf(" \n");

char buffer[] = "Got it";

//Send our verification signal
do{
stat = write(socket, &buffer, sizeof(int));
}while(stat<0);

printf("Reply sent\n");
printf(" \n");

//Open corresponding file/filepath to store receiving image
image = fopen(file_direc, "w");

if( image == NULL) {
printf("Error has occurred. Image file could not be opened\n");
return -1; }

//Loop while we have not received the entire file yet


struct timeval timeout = {10,0};

fd_set fds;
int buffer_fd;

while(recv_size < size) {


    FD_ZERO(&fds); //File descriptor set to zero
    FD_SET(socket,&fds); //Add File descriptor set

	// Indicates which of the specified file descriptors is ready for reading, writing, or has an error condition pending
    buffer_fd = select(FD_SETSIZE,&fds,NULL,NULL,&timeout);

    if (buffer_fd < 0)
       printf("error: bad file descriptor set.\n");

    if (buffer_fd == 0)
       printf("error: buffer read timeout expired.\n");

    if (buffer_fd > 0)
    {
		// Reads the size of the recieved packet
        do{
               read_size = read(socket,imagearray, packet_size);
            }while(read_size <0);

            printf("Packet number received: %i\n",packet_index);
        printf("Packet size: %i\n",read_size);


        //Write the currently read data into our image file
         write_size = fwrite(imagearray,1,read_size, image);
         printf("Written image size: %i\n",write_size);

             if(read_size !=write_size) {
                 printf("error in read write\n");    }


             //Increment the total number of bytes read
             recv_size += read_size;
             packet_index++;
             printf("Total received image size: %i\n",recv_size);
             printf(" \n");
             printf(" \n");
    }

}


  fclose(image);
  printf("Image successfully Received!\n");
  return 1;
  }

  int main(int argc, char *argv[])
  {

	  //Declarations
  int socket_desc;
  struct sockaddr_in server;


  //Create socket
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);

  //Error handling
  if (socket_desc == -1) {
  printf("Could not create socket");
  }

  //Fill with zero(zero - init)
  memset(&server, 0, sizeof(server)); 
  //Socket setup
  server.sin_addr.s_addr = inet_addr(argv[1]);
  server.sin_family = AF_INET;
  server.sin_port = htons( 9000 );

  //Connect to remote server
  if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0) {
  cout<<strerror(errno);
  close(socket_desc);
  puts("Connect Error");
  return 1;
  }

  puts("Connected\n");

  //Calls function to receive image 
  receive_image(socket_desc, argv[2]);

  // closes the socket connection
  close(socket_desc);

  return 0;
  }
