#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <dirent.h>

#include "definicje.h"
#include "operatory.h"

using namespace std;


in_addr & zero(in_addr & a) 
{
	a.s_addr = 0;
	return a;
}

bool operator==(const in_addr a, const in_addr b) 
{
	return (a.s_addr == b.s_addr);
}

bool operator!=(const in_addr a, const in_addr b) 
{
	return (a.s_addr != b.s_addr);
}

ssize_t read_while(int fd, void * buf, size_t count)
{
	ssize_t s, w = 0;
	int i = 0;
	while (count > 0)
		switch (s = read(fd, (void *)(((char *) buf) + w), count))
		{
			case -1:
				return -1;
				
			case 0:
				return 0;
				
			default:
				i++;
				w += s;
				count -= s;
		}
	if (count != 0)
		return -1;
	if (i > 1)
		debuger("> Podzielono. Liczba porcji: %d\n", i);
	return w;
}

void debuger(const char * arg1, ...)
{
	va_list args;
	if (DEBUG) {	
		va_start(args, arg1);
		vprintf(arg1, args);
		va_end(args);	
	}
}

