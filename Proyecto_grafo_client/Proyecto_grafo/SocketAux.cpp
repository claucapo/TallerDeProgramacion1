#ifndef S_READ_H
#define S_READ_H

#include <WinSock2.h>
using namespace std;

int sRead(SOCKET source, char* buffer, int length) {
	int offset = 0;
	int lastRead;
	do {
		lastRead = recv(source, buffer + offset, length - offset, 0);
		if (lastRead > 0)
			offset += lastRead;
		else if (lastRead == 0)
			return lastRead;
	} while (offset < length);
}


#endif