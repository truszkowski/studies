#ifndef _KLASY_FRAGMENTY_H_
#define _KLASY_FRAGMENTY_H_

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
#include "klasy.h"

using namespace std;


class fragmenty
{
	public:
		slowo nazwa_pliku[MAX_FRG];
		int id_pliku[MAX_POB];
};

#endif 
