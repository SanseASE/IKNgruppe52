#include <cstdio>
#include "../include/Link.h"

namespace Link {

/**
  * Explicitet constructor for linklaget.
  * Åbner den serielle port og sætter baudraten, paritet og antal stopbits
  */
Link::Link(int bufsize)
{
	buffer = new char[(bufsize*2)];
	
    serialPort=v24OpenPort("/dev/ttyS1",V24_STANDARD);
    if ( serialPort==NULL )
    {
        fputs("error: sorry, open failed!\n",stderr);
        exit(1);
    }

    int rc=v24SetParameters(serialPort,V24_B115200,V24_8BIT,V24_NONE);
    if ( rc!=V24_E_OK )
    {
        fputs("error: setup of the port failed!\n",stderr);
        v24ClosePort(serialPort);
        exit(1);
    }

    // Uncomment following lines to use timeout
/*    rc=v24SetTimeouts(serialPort,5);
    if ( rc!=V24_E_OK )
    {
        fputs("error: setup of the port timeout failed!\n",stderr);
        v24ClosePort(serialPort);
        exit(1);
    }

    rc=v24FlushRxQueue(serialPort);
    if ( rc!= V24_E_OK )
    {
    	fputs("error: flushing receiverqueue\n", stderr);
    	v24ClosePort(serialPort);
    	exit(1);
    }

    rc=v24FlushTxQueue(serialPort);
    if ( rc!= V24_E_OK )
    {
    	fputs("error: flushing transmitterqueue\n", stderr);
    	v24ClosePort(serialPort);
    	exit(1);
    }
*/
}

/**
  * Destructor for linklaget.
  * Lukker den serielle port
  */
Link::~Link()
{
	if(serialPort != NULL)
		v24ClosePort(serialPort);
	if(buffer != NULL)
		delete [] buffer;
}

/**
 * Bytestuffer buffer, dog maximalt det antal som angives af size
 * Herefter sendes de til modtageren via den serielle port
 *
 * @param buffer De bytes som skal sendes
 * @param size Antal bytes der skal sendes
 */
void Link::send(const char buf[], short size)
{
    char sendbuf[(size*2)+2] = {0};
    int count = 1;
    sendbuf[0] = 'A';
    for (int i =  0; i < size; i++)
    {

        if (buf[i] == 'A')
        {
            sendbuf[count] = 'B';
            sendbuf[++count] = 'C';
            count++;
        }
        else if (buf[i] == 'B')
        {
            sendbuf[count] = 'B';
            sendbuf[++count] = 'D';
            count++;
        }
        else
        {
            sendbuf[count] = buf[i];
            count++;
        }

    }

    sendbuf[count]='A';
    count++;

    v24Write(serialPort, (unsigned char*)sendbuf, count);
}

/**
 * Modtager data fra den serielle port og debytestuffer data og gemmer disse i buffer.
 * Modtager højst det antal bytes som angives af size.
 * Husk kun at læse en byte ad gangen fra den serielle port.
 *
 * @param buffer De bytes som modtages efter debytestuffing
 * @param size Antal bytes der maximalt kan modtages
 * @return Antal bytes modtaget
 */
short Link::receive(char buf[], short size)
{

    unsigned char receive_buf[size-2];
    int count = 0;

    v24Read(serialPort, receive_buf, sizeof(receive_buf));


         for (int i =  1; i < size-1; i++)
         {

             if (receive_buf[i] == 'B' & receive_buf[i+1] == 'C')
             {
                 buf[count] = 'A';
                 count++;
                 i++;
             }
             else if (receive_buf[i] == 'B' & receive_buf[i+1] == 'D')
             {
                 buf[count] = 'B';
                 count++;
                 i++;
             }
             else
             {
                 buf[count] = receive_buf[i];
                 count++;
             }
         }
}

} /* namespace Link */
