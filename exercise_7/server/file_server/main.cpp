//============================================================================
// Name        : file_server.cpp
// Author      : Gruppe 52
// Version     : 1.0
// Description : file_server in C++
//============================================================================

   #include<string.h>
   #include<arpa/inet.h>
   #include<unistd.h>
   #include<iostream>


   using namespace std;


   //Function for sending image from server to client
   int send_image(int socket){

	//Declarations
   FILE *picture;
   int size, read_size, stat, packet_index;
   char send_buffer[1000], read_buffer[256];
   packet_index = 1;

   //Read message from socket (from client)
   read(socket, read_buffer , 255);

   //Print requested file and directory to terminal
   cout << "File directory: " << read_buffer << endl;

   //Open file for future work
   picture = fopen(read_buffer, "r");

   //Error handling
   if(picture == NULL) {
       cout << "Error Opening Image File: " << read_buffer << endl;
       write(socket, "0", sizeof(int));
       return 1;
   }

   //Prints service message: Opening file
   cout << "Opening File: " << read_buffer << endl;
   
   //Verificationmessage to client (file exists)
   write(socket, "1", sizeof(int));

   //Picture size
   printf("Getting Picture Size\n");
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

   cout << "Received data in socket" << endl;
   cout << "Socket data: " << read_buffer << endl;

   while(!feof(picture)) {
      //Read from the file into our send buffer
      read_size = fread(send_buffer, 1, sizeof(send_buffer), picture);

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
   return 0;
    }

    int main()
    {
		//Declarations
      int socket_desc , new_socket , c;
      struct sockaddr_in server , client;

      //Create socket
      socket_desc = socket(AF_INET , SOCK_STREAM , 0);
      if (socket_desc == -1)
      {
         printf("Could not create socket");
      }

      //Prepare the sockaddr_in structure
      server.sin_family = AF_INET;
      server.sin_addr.s_addr = INADDR_ANY;
      server.sin_port = htons( 9000 );

      //Bind
     if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
     {
       puts("bind failed");
       return 1;
     }

     puts("bind done");

     while(1)
     {
     //Listen
     listen(socket_desc , 3);
	 
      puts("Waiting for incoming connections...");
      c = sizeof(struct sockaddr_in);
	//Accept an incoming connection
     if((new_socket = accept(socket_desc, (struct sockaddr *)&client,(socklen_t*)&c))){
puts("Connection accepted");
         }

    fflush(stdout);

    if (new_socket<0)
    {
      perror("Accept Failed");
      return 1;
    }

    send_image(new_socket);
}
	//Close socket
    close(socket_desc);
    fflush(stdout);
    return 0;
    }
