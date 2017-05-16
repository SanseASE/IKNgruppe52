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
            // Filling local buffer
            memcpy(buffer+ACKSIZE, buf, size);
            // Calculating checksum
            checksum->calcChecksum(buffer,size+ACKSIZE);

            // Send packet
            link->send(buffer, size+ACKSIZE);

            // Wait for ack
            ack = receiveAck();
            std::cout << "Transport: " << (ack ? "Good" : "Bad") << " ack received" << std::endl;

            // looping until we get a good ack from client
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
            // receive the buffer from link layer
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

        // saving seqNo in varible for later use
        old_seqNo = buffer[SEQNO];
        // Filling buffer
        memcpy(buf,buffer+ACKSIZE,recvSize-ACKSIZE);

        return recvSize-ACKSIZE;
	}
}


