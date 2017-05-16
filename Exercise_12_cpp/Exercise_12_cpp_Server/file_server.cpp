#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include "../include/Transport.h"
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
    Transport::Transport t(BUFSIZE);
    long filesize;
    char filepath[BUFSIZE] = {};

    while(1){
        //Read filepath
        t.resetSeqno();
        std::cout << "\nApp: Waiting for filepath" << std::endl;
        t.receive(filepath, BUFSIZE);

        //Find file on server
        filesize = check_File_Exists(filepath);
        if(filesize == 0){
            std::cout << "App: File not found" << std::endl;
            t.send("0", 1);
        }
        else{
            //Send the size of the file
            std::cout << "App: Sending filesize" << std::endl;
            char filesize_a[256] = {};
            sprintf(filesize_a, "%li", filesize);
            t.send(filesize_a, strlen(filesize_a));

            std::cout << "App: Sending file to client" << std::endl;

            //Send file to client
            sendFile(filepath, filesize, &t);
            std::cout << "App: File sent" << std::endl;
        }
    }
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
    std::ifstream myfile(fileName);
    char data[BUFSIZE];
    long sentBytes = 0;

    //Check if file open
    if(myfile.is_open()){
        //Write file 1000 bytes at a time
        while(sentBytes < fileSize){
            myfile.read(data, BUFSIZE);
            transport->send(data, (fileSize-sentBytes > BUFSIZE ? BUFSIZE : fileSize-sentBytes));
            sentBytes += BUFSIZE;
        }

        //Close file
        myfile.close();
    }
}

/// <summary>
/// The entry point of the program, where the program control starts and ends.
/// </summary>
/// <param name='args'>
/// The command-line arguments.
/// </param>
int main(int argc, char **argv)
{
    /// App Test
    new file_server();

    /// Link Test
    /**
    Link::Link l(BUFSIZE);
    char buf[BUFSIZE];

    std::cout << "Link Test" << std::endl;
    int rc = l.receive(buf, BUFSIZE);
    buf[rc] = 0;
    std::cout << "App: Message received: " << buf << std::endl;
    */

    /// Transport Test
    /**
    Transport::Transport t(BUFSIZE);
    char buf[BUFSIZE];
    std::cout << "Transport Test" << std::endl;

    while(1){
        std::cout << "App: Waiting for message" << std::endl;
        int rc = t.receive(buf, BUFSIZE);
        buf[rc] = 0;
        std::cout << "App: Message received: " << buf << '\n' << std::endl;
    }
    */

    return 0;
}
