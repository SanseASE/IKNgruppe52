#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include "../include/Transport.h"
#include "../include/lib.h"
#include "file_client.h"

#include "../include/Link.h"
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
    long filesize;
    char filesize_buf[BUFSIZE];

    Transport::Transport t(BUFSIZE);

    //Write filepath
    std::cout << "App: Sending filepath" << std::endl;
    char filepath[BUFSIZE] = {};
    strncpy(filepath, argv[1], BUFSIZE);
    strtok(filepath, "\n");
    t.send(filepath, strlen(filepath));

    //Read file size
    std::cout << "App: Reading file size" << std::endl;
    if (t.receive(filesize_buf, BUFSIZE) < 0){
        std::cout << "App: Error in receive" << std::endl;
        exit(1);
    }
    filesize = atol(filesize_buf);

    //Read file
    if(filesize != 0){
        std::cout << "App: Receiving file" << std::endl;
        receiveFile(filepath, &t);
        std::cout << "App: File received" << std::endl;
    }
    else{
        std::cout << "App: File not found" << std::endl;
    }
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
void file_client::receiveFile (std::string filepath, Transport::Transport *transport)
{
    int count = BUFSIZE;
    char buffer[BUFSIZE] = {};

    //Get filename
    const std::string filename = extractFileName(filepath);

    std::string realfilepath = "/root/Downloads/" + filename;

    //Create file to save incomming data
    FILE * fp = fopen(realfilepath.c_str(), "w");

    //Open file stream
    std::ofstream file_stream(realfilepath);

    //Receive file 1000 bytes at a time
    while (count == BUFSIZE){
        count = transport->receive(buffer, BUFSIZE);
        file_stream.write(buffer, count);
    }

    //Close file and stream
    file_stream.close();
    fclose(fp);
}		

/// <summary>
/// The entry point of the program, where the program control starts and ends.
/// </summary>
/// <param name='args'>
/// First argument: Filname
/// </param>
int main(int argc, char** argv)
{
    new file_client(argc, argv);
    return 0;
}
