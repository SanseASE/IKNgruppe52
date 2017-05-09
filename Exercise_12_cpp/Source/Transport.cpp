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
/*
    Her skal der laves fejl kode til implemtering af støj til test
 */
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
      do
        {
        buffer[SEQNO] = seqNo;
        buffer[TYPE] = DATA;
        memcpy(buffer+ACKSIZE, buf, size);

        checksum->calcChecksum(buffer+CHKSUMSIZE, (size+ACKSIZE)-CHKSUMSIZE);

        /*
     Her skal der laves fejl kode til implemtering af støj til test
        */
        link->send(buffer, size);
        } while(receiveAck() == false);

        std::cout << buffer<< std::endl;

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
        //memcpy(buffer, buf, size);
        std::cout << buffer << std::endl;
        std::cout << "buffer" << std::endl;


        checksum->checkChecksum(buffer+CHKSUMSIZE, size-CHKSUMSIZE);

        } while(checksum == false || seqNo == old_seqNo);

        sendAck(true);

        old_seqNo = seqNo;

        return (buffer, size);


	}
}

