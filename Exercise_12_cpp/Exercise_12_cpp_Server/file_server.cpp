#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include "../include/Transport.h"
#include "../include/lib.h"
#include "file_server.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

/// <summary>
/// The BUFSIZE
/// </summary>
#define BUFSIZE 1000

/// <summary>
/// Initializes a new instance of the <see cref="file_server"/> class.
/// </summary>
file_server::file_server ()
{
    Transport::Transport * transport = new Transport::Transport(BUFSIZE);
    long fileSize;

    //Modtage et filnavn
    transport->receive(buffer,BUFSIZE);
    std::cout << "Filnavn: " << buffer << std::endl;
    string fileName(buffer);


    //Sende filstørrelse
    sprintf(buffer,"%d",fileSize);
    transport->send(buffer,sizeof(buffer));


    //Sender fil
    sendFile(fileName,fileSize,transport);

}

/// <summary>
/// Sends the file.
/// </summary>
/// <param name='fileName'>
/// File name.
/// </param>
/// <param name='fileSize'>
/// File size.
/// </param>
/// <param name='transport'>
/// Transport lag.
/// </param>
void file_server::sendFile(std::string fileName, long fileSize, Transport::Transport *transport)
{
       int fd;

       // Åbner fil
       fd = open(fileName.c_str(), O_RDONLY);

       /*Sender fil*/
       for (int i = 0; i<fileSize/1000+1; i++)
       {

           if (i<fileSize/1000) //Så længe der er mere end 1000 tilbage
           {
               read(fd, buffer, BUFSIZE);
               transport->send(buffer,BUFSIZE);
           }
           else
           {
               read(fd, buffer, fileSize%BUFSIZE);
               transport->send(buffer, fileSize%BUFSIZE);
           }

       }

       close(fd);
}

/// <summary>
/// The entry point of the program, where the program control starts and ends.
/// </summary>
/// <param name='args'>
/// The command-line arguments.
/// </param>
int main(int argc, char **argv)
{
     //file_server* server= new file_server();
   Transport::Transport trans(BUFSIZE);

    char send[100]= "HENNING!! DIN OSTEMAD BLIVER KOLD";
    std::cout << "Streng som skal sendes: " << send << std::endl;
    trans.send(send, strlen(send));
/*
    char send[100];
    std::cout << "skriv meddelelsen her: " << std::endl;
    while(1)
    {
    fgets(send, sizeof(send), stdin);

     link.send(send, sizeof(send));
    }
    */
	return 0;
}
