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
        bool ack;
        do{
            // Add transport header
            buffer[SEQNO] = seqNo;
            buffer[TYPE] = DATA;
            memcpy(buffer+ACKSIZE, buf, size);
            checksum->calcChecksum(buffer,size+ACKSIZE);

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

        }while(ack != true);

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

                sendAck(false);
            }
            else{
                // Check if the message is a retransmission
                if (buffer[SEQNO] == old_seqNo){

                    sendAck(true);
                }
                // The message is a new non-corrupt message
                else{

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


