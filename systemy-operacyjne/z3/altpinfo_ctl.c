#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "altpinfo/altpinfo.h"

int main( int argc, char * argv[] ) {
	char buffer[FMT_CHAR_LIMIT];
	int fd, code;	

	if ( ( fd = open( ALTPINFO_DEVICE, O_RDWR ) ) < 0 ) {
		fprintf ( stderr, "Blad w open!\n" );
		return 1;
	}
	
	if ( argc > 1 ) {
		if ( ( code = ioctl( fd, API_SET_FMT, argv[1] ) ) != 0 ) 
			fprintf ( stderr, "Blad w ioctl( %d, API_SET_FMT, %s )\n", fd, argv[1] );
	} else {
		if ( ( code = ioctl( fd, API_GET_FMT, buffer ) ) != 0 )
			fprintf ( stderr, "Blad w ioctl( %d, GPI_SET_FMT, <?> )\n", fd );
		else
			fprintf( stdout, buffer );
	}

	close( fd );
	return code;
}
