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
        seqNo = '0';
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
      do{
        buffer[SEQNO] = seqNo;
        buffer[TYPE] = DATA;

        memcpy(buffer+ACKSIZE, buf, size);


        checksum->calcChecksum(buffer, size+ACKSIZE);

        for(int i = 0; i<size+ACKSIZE; i++){
        std::cout << "Transport buffer: " << (char)buffer[i] << std::endl;}

        /*if(++errorCount == 1) // Simulate noise
         {
         buffer[1]++;          // Important: Only spoil a checksum-field (buffer[0] or buffer[1])
         std::cout << "Noise! - byte #1 is spoiled in the third transmission" << std::endl;
         //break;
         }*/

        link->send(buffer, size+ACKSIZE);
        } while(receiveAck() == false);

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
        do
        {
        link->receive(buffer,size);

        std::cout << "Modtaget i transport layer: " << buffer << std::endl << std::endl;

        checksum->checkChecksum(buffer+CHKSUMSIZE, size-CHKSUMSIZE);

        } while(checksum == false || buffer[SEQNO] == old_seqNo);

        sendAck(true);

        old_seqNo = buffer[SEQNO];

        memcpy(buf, buffer+ACKSIZE, size);

        return (buf, size);


    }
}

