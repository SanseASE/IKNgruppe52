#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include "../include/Transport.h"
#include "../include/lib.h"
#include "file_client.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

/// <summary>
/// The BUFSIZE
/// </summary>
#define BUFSIZE 1000

/// <summary>
/// Initializes a new instance of the <see cref="file_client"/> class.
///
/// file_client metoden opretter en peer-to-peer forbindelse
/// Sender en forspørgsel for en bestemt fil om denne findes på serveren
/// Modtager filen hvis denne findes eller en besked om at den ikke findes (jvf. protokol beskrivelse)
/// Lukker alle streams og den modtagede fil
/// Udskriver en fejl-meddelelse hvis ikke antal argumenter er rigtige
/// </summary>
/// <param name='args'>
/// Filnavn med evtuelle sti.
/// </param>
file_client::file_client(int argc, char **argv)
{
    Transport::Transport * transport = new Transport::Transport(BUFSIZE);

    //Sende filnavn
    transport->send(argv[1],sizeof(argv[1]));


    //Modtage fil
    string fileName(argv[1]);
    receiveFile(fileName,transport);
}

/// <summary>
/// Receives the file.
/// </summary>
/// <param name='fileName'>
/// File name.
/// </param>
/// <param name='transport'>
/// Transportlaget
/// </param>
void file_client::receiveFile (std::string fileName, Transport::Transport *transport)
{
    int fd;
    char receive_buf[BUFSIZE];
    long fileSize;

    transport->receive(receive_buf,BUFSIZE);
    fileSize = atol(receive_buf);

    //Åbner fil

     fd = open(fileName.c_str(), O_WRONLY | O_CREAT,S_IXGRP);


     //Modtager fil
     for (int i = 0; i<fileSize/BUFSIZE+1; i++)
     {
         bzero(receive_buf,BUFSIZE); //Tømmer bufferen

         if (i<fileSize/BUFSIZE) //Så længe der er mere end 1000 tilbage
         {
             read(fd,receive_buf,BUFSIZE);
             transport->receive(receive_buf, BUFSIZE);
         }

         else
         {
             read(fd,receive_buf,(fileSize%BUFSIZE));
             transport->receive(receive_buf, (fileSize%BUFSIZE));

         }
     }
}

/// <summary>
/// The entry point of the program, where the program control starts and ends.
/// </summary>
/// <param name='args'>
/// First argument: Filname
/// </param>
int main(int argc, char** argv)
{

    //file_client * client = new file_client(argc, argv);




    Transport::Transport trans(BUFSIZE);

   char recv_buf[100];
   trans.receive(recv_buf, 100);
   cout << "Besked: " << recv_buf << endl;
/*
   while(1)
   {
   link.receive(recv_buf, 100);

   cout << recv_buf << endl;
   }
    */

    return 0;
}
