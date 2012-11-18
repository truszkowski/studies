#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include "altpinfo/altpinfo.h"
using namespace std;

int main( int argc, char * argv[] ) { 

	if ( argc == 1 ) {
		cout << "Usage: " << argv[0] << " <pid | all>" << " <fmt>" << endl;
		return 1;
	}

	int fd = open( ALTPINFO_DEVICE, O_RDWR );
	if ( fd < 0 ) {
		cout << "Blad w open!" << endl;
		return 2;
	}
	
	char buffer[100*BUFLEN];
	
	if ( argc > 2 )
		if ( ioctl( fd, API_SET_FMT, argv[2] ) )
			cerr << "Bad fmt, using default..." << endl;
	
	if ( write( fd, argv[1], strlen( argv[1] ) ) < 0 ) {
		cerr << "Cannot write this!" << endl;
		return 3;
	}
	
	int pack = 1024, len = 0, pos = 0;
	do {

		int rlen = read( fd, buffer + pos, pack );
		
		if ( rlen < 0 ) {
			cerr << "Error while reading!" << endl;
			break;
		}
		
		pos += rlen;
		
		if ( ioctl( fd, API_GET_CURLEN, &len ) ) {
			cerr << "Cannot get API_GET_CURLEN!" << endl;
			break;
		}
		
	} while ( len > 0 );
	
	buffer[pos] = '\0';

	cout << buffer << endl;

	close( fd );
	return 0;
}
