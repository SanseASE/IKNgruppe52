#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include "../include/Transport.h"
#include "../include/Link.h"
#include "../include/lib.h"
#include "file_server.h"

/// <summary>
/// The BUFSIZE
/// </summary>
#define BUFSIZE 1000

/// <summary>
/// Initializes a new instance of the <see cref="file_server"/> class.
/// </summary>
file_server::file_server ()
{
	// TO DO Your own code
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
	// To do Your own code
}

/// <summary>
/// The entry point of the program, where the program control starts and ends.
/// </summary>
/// <param name='args'>
/// The command-line arguments.
/// </param>
int main(int argc, char **argv)
{

    Transport::Transport trans(BUFSIZE);

    char send[100]= "HENNING!! DIN OSTEMAD BLIVER KOLD";
    trans.send(send, strlen(send));

    /*
    char send[100];
    std::cout << "skriv meddelelsen her: " << std::endl;
    while(1)
    {
    fgets(send, sizeof(send), stdin);

     link.send(send, sizeof(send));
    //new file_server();
    }*/
	
	return 0;
}
