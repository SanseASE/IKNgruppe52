#include <iostream>
#include <cstring>
#include <cstdio>
#include "../include/Transport.h"
#include "../include/TransConst.h"


#define DEFAULT_SEQNO 2

namespace Transport
{
    /// <summary>
    /// Initializes a new instance of the <see cref="Transport"/> class.
    /// </summary>
    Transport::Transport (short BUFSIZE)
    {
        link = new Link::Link(BUFSIZE+ACKSIZE);
        checksum = new Checksum();
        buffer = new char[BUFSIZE+ACKSIZE];
        seqNo = 0;
        old_seqNo = DEFAULT_SEQNO;
        errorCount = 0;
        recvSize = 0;
        dataReceived = false;
    }

    /// <summary>
    /// Delete dynamics attribute before delete this object.
    /// </summary>
    Transport::~Transport()
    {
        if(link) delete link;
        if(checksum) delete checksum;
        if(buffer) delete [] buffer;
    }

    /// <summary>
    /// Receives the ack.
    /// </summary>
    /// <returns>
    /// The ack.
    /// </returns>
    bool Transport::receiveAck()
    {
        recvSize = link->receive(buffer, ACKSIZE);
        dataReceived = true;

        if (recvSize == ACKSIZE)
        {

            if(!checksum->checkChecksum(buffer, ACKSIZE) ||
                buffer[SEQNO] != seqNo ||
                buffer[TYPE] != ACK)
            {
                return false;
            }

            seqNo = ((buffer[SEQNO] + 1) % 2);
        }

        return true;
    }

    /// <summary>
    /// Sends the ack.
    /// </summary>
    /// <param name='ackType'>
    /// Ack type.
    /// </param>
    void Transport::sendAck (bool ackType)
    {
        char ackBuf[ACKSIZE];
        ackBuf [SEQNO] = (ackType ? (buffer [SEQNO] + 1) % 2 : buffer [SEQNO]) ;
        ackBuf [TYPE] = ACK;
        checksum->calcChecksum (ackBuf, ACKSIZE);

       /* if(++errorCount == 1) // Simulate noise
         {
         ackBuf[1]++; // Important: Only spoil a checksum-field (ackBuf[0] or ackBuf[1])
         std::cout <<"Noise! byte #1 is spoiled in the third transmitted ACK-package" << std::endl;
         }*/

        link->send(ackBuf, ACKSIZE);
    }

    /// <summary>
    /// Send the specified buffer and size.
    /// </summary>
    /// <param name='buffer'>
    /// Buffer.
    /// </param>
    /// <param name='size'>
    /// Size.
    /// </param>
    void Transport::send(const char buf[], short size)
    {
        bool ack;
    do{

        buffer[SEQNO] = seqNo;
        buffer[TYPE] = DATA;

        memcpy(buffer+ACKSIZE, buf, size);
        checksum->calcChecksum(buffer, size+ACKSIZE);


        /// Transport test
        /*if(++errorCount == 1) // Simulate noise
         {
         //buffer[1]++;          // Important: Only spoil a checksum-field (buffer[0] or buffer[1])
         std::cout << "Noise! - byte #1 is spoiled in the third transmission" << std::endl;
         //break;
         }*/

        /// Sender pakke
        link->send(buffer, size+ACKSIZE);

        ack = receiveAck();
        std::cout << "Transport: " << (ack ? "Good" : "Bad") << " ack received" << std::endl;

        } while(ack == false);

        old_seqNo = DEFAULT_SEQNO;

    }

    /// <summary>
    /// Receive the specified buffer.
    /// </summary>
    /// <param name='buffer'>
    /// Buffer.
    /// </param>
    short Transport::receive(char buf[], short size)
     {
        bool check;

        while(1){
            recvSize = link->receive(buffer,size+ACKSIZE);

            check = checksum->checkChecksum(buffer, recvSize);
            if(buffer[TYPE] != DATA || check == false)
                sendAck(false);


            else if(buffer[SEQNO] == old_seqNo)
                sendAck(true);

            else
                sendAck(true);
        }

         old_seqNo = buffer[SEQNO];

         memcpy(buf, buffer+ACKSIZE, size);

         return recvSize-ACKSIZE;
     }
 }
