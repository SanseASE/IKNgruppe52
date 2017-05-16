#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__

#include "../include/Link.h"
#include "../include/Checksum.h"
#include "../include/TransConst.h"

#define DEFAULT_SEQNO 2

/// <summary>
/// Transport.
/// </summary>
namespace Transport
{
	class Transport
	{
	public:
		Transport(short BUFSIZE);
		~Transport();
		void send(const char buf[], short size);
		short receive(char buf[], short size);
        void resetSeqno(){
            seqNo = 0;
            old_seqNo = DEFAULT_SEQNO;
        }

	private:	
		Link::Link *link;
		Checksum *checksum;
		char *buffer;
		unsigned char seqNo;
		unsigned char old_seqNo;
		short errorCount;
        short recvSize;
        bool dataReceived;

        char ackBuf[ACKSIZE];
		bool receiveAck();
		void sendAck (bool ackType);
		Transport( const Transport &right){};
		Transport& operator=(const Transport &) {};

	};
}


#endif /* __TRANSPORT_H__ */

