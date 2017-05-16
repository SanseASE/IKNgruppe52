
#ifndef __LINKLAGET_H__
#define __LINKLAGET_H__

#include <ezV24/ezV24.h>
#include <iostream>

#define DELIMITER 'A'

namespace Link {

class Link
{
public:
	Link(int bufsize );
	~Link();
    void send(const char [], short size);
	short receive(char buf[], short size);
private:
    Link( const Link &r){}
    Link& operator=(const Link &){}
	char *buffer;
	v24_port_t *serialPort;
    int bufsize_;
};


} /* namespace Linklaget */
#endif /* LINKLAGET_H_ */

