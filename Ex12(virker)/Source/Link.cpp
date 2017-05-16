#include <cstdio>
#include "../include/Link.h"


namespace Link {

/**
  * Explicitet constructor for linklaget.
  * Åbner den serielle port og sætter baudraten, paritet og antal stopbits
  */
Link::Link(int bufsize) : bufsize_(bufsize)
{
    buffer = new char[(bufsize*2)+2];
	
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

    // Insert Delimiter
    int count = 1;
    buffer[0] = 'A';

    // Run through the parameter buf;
    // if an A is found insert BC in the local buffer,
    // if a B is found insert BD in the local buffer, else pass the char unchanged
    for (int i = 0; i < size; ++i){
        if(buf[i] == 'A'){
            buffer[count++] = 'B';
            buffer[count++] = 'C';
        }
        else if(buf[i] == 'B'){
            buffer[count++] = 'B';
            buffer[count++] = 'D';
        }
        else
            buffer[count++] = buf[i];
    }
    // Append Delimiter
    buffer[count++] = 'A';

    // Write the local buffer to the serialPort
    v24Write(serialPort, (unsigned char*)buffer, count);


    /// Link Test - remove print
    ///std::cout << "Link: buffer written to serial port: " << buffer << std::endl;
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
	//variable to determine real end
    short count = 0, i = 0;

	
	//Wait for an 'A'
    while(v24Getc(serialPort) != 'A'){}


    while(1){
        // Read next char
        buffer[count] = v24Getc(serialPort);

        // Check for Delimiter
        if(buffer[count] == 'A'){

            return i;
        }

        // Check for the char combination BC (A) and BD (B);
        // If found make change in the parameter buf[]
        // If not found pass the char to the parameter buf[]
        if(count > 0 && buffer[count-1] == 'B'){
            if(buffer[count] == 'C')
                buf[i-1] = 'A';
            else if(buffer[count] == 'D'){}
            else{
                std::cout << "Link: transmission error!" << std::endl;
                return -1;
            }
        }
        else{
            buf[i++] = buffer[count];
        }

    ++count;
    }
}

} /* namespace Link */
//Tobias er ikke sej
