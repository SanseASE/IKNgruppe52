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
        //char ackBuf[ACKSIZE];
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
        buffer[SEQNO] = seqNo;
        buffer[TYPE] = DATA;

        memcpy(buffer+ACKSIZE, buf, size);
        checksum->calcChecksum(buffer, size+ACKSIZE);

        bool ack;
        errorCount = 0;
        /// Transport test
        /*if(++errorCount == 1) // Simulate noise
         {
         //buffer[1]++;          // Important: Only spoil a checksum-field (buffer[0] or buffer[1])
         std::cout << "Noise! - byte #1 is spoiled in the third transmission" << std::endl;
         //break;
         }*/
        do{
        std::cout << "Transport: Error count = " << errorCount << std::endl;
        /// Sender pakke
        link->send(buffer, size+ACKSIZE);

        ack = receiveAck();
        std::cout << "Transport: " << (ack ? "Good" : "Bad") << " ack received" << std::endl;

        } while(++errorCount < 5 && ack != true);

        if (errorCount == 5 && !ack){
                    std::cout << "Transport: Error count = 5 -> send request aborted" << std::endl;
                }

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

        while(1){
            recvSize = link->receive(buffer,size+ACKSIZE);

            if(buffer[TYPE] != DATA || checksum->checkChecksum(buffer, recvSize) == false){
                std::cout << "Transport: Error in received message" << std::endl
                          << "Transport: Sending ACK" << (int)buffer[SEQNO]+1%2 << std::endl;
                sendAck(false);
            }
            else{
                // Check if the message is a retransmission
                if (buffer[SEQNO] == old_seqNo){
                    std::cout << "Transport: Retrasmission received - ignoring the message" << std::endl
                              << "Transport: Sending ACK" << (int)buffer[SEQNO] << std::endl;
                    sendAck(true);
                }
                // The message is a new non-corrupt message
                else{
                    std::cout << "Transport: Good message received" << std::endl
                              << "Transport: Sending ACK" << (int)buffer[SEQNO] << std::endl;
                    sendAck(true);
                    break;
                }
            }
        }

         old_seqNo = buffer[SEQNO];
         memcpy(buf, buffer+ACKSIZE, recvSize-ACKSIZE);

         return recvSize-ACKSIZE;
     }
 }
