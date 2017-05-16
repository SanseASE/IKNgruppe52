#include <iostream>
#include <cstring>
#include <cstdio>
#include "../include/Transport.h"
#include "../include/TransConst.h"


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
        recvSize = link->receive(ackBuf, ACKSIZE);
        dataReceived = true;

        if (recvSize == ACKSIZE)
        {
            if(!checksum->checkChecksum(ackBuf, ACKSIZE) ||
                ackBuf[SEQNO] != seqNo ||
                ackBuf[TYPE] != ACK)
            {
                return false;
            }

            seqNo = ((ackBuf[SEQNO] + 1) % 2);
            return true;
        }

        return false;
    }

	/// <summary>
	/// Sends the ack.
	/// </summary>
	/// <param name='ackType'>
	/// Ack type.
	/// </param>
	void Transport::sendAck (bool ackType)
	{
        ackBuf[SEQNO] = (ackType ? buffer[SEQNO] : (buffer [SEQNO] + 1) % 2) ;
        ackBuf[TYPE] = ACK;
		checksum->calcChecksum (ackBuf, ACKSIZE);
        /// Transport Test
        /*if (rand() % 2){
            ackBuf[1]++;
            std::cout << "Transport: Noise! Ack byte #1 corrupted" << std::endl;
        }*/
        /// End Test
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
        // Add transport header
        //seqNo = ((old_seqNo + 1) % 2);
        buffer[SEQNO] = seqNo;
        buffer[TYPE] = DATA;
        memcpy(buffer+ACKSIZE, buf, size);
        checksum->calcChecksum(buffer,size+ACKSIZE);

        //old_seqNo = seqNo;
        errorCount = 0;
        bool ack;
        char temp = buffer[4];

        do{
            std::cout << "Transport: Error count = " << errorCount << std::endl;

            ///Transport Test - corrupt message
            /*if (rand() % 2){
                buffer[1]++;
                std::cout << "Transport: Noise! Message byte #1 corrupted!" << std::endl;
            }
            else{
                buffer[4] = temp;
            }*/
            /// End test

            // Send packet
            link->send(buffer, size+ACKSIZE);

            // Wait for ack
            ack = receiveAck();
            std::cout << "Transport: " << (ack ? "Good" : "Bad") << " ack received" << std::endl;

        }while(++errorCount < 5 && ack != true);

        if (errorCount == 5 && !ack){
            std::cout << "Transport: Error count = 5 -> send request aborted" << std::endl;
        }
	}

	/// <summary>
	/// Receive the specified buffer.
	/// </summary>
	/// <param name='buffer'>
	/// Buffer.
	/// </param>
    ///
	short Transport::receive(char buf[], short size)
	{
        while(1){
            recvSize = link->receive(buffer, size+ACKSIZE);

            // Check if there's an error in the received message
            if (buffer[TYPE] != DATA || checksum->checkChecksum(buffer, recvSize) == false){
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
        memcpy(buf,buffer+ACKSIZE,recvSize-ACKSIZE);

        return recvSize-ACKSIZE;
	}
}


