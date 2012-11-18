#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <time.h>
#include <unistd.h>

#define AUTHOR "Piotr Truszkowski, pt209223"
#define LOGFILE "watchfs.log"
#define ALLRIGHT 0
#define BUFLEN 1024

extern char ** environ;

void findlogfile ( char * lpath ) {
	int it, len;
	for ( it = 0; environ[it] != NULL; ++it )
		if ( (len = strspn( environ[it], "HOME" )) == 4 ) {
			sprintf ( lpath, "%s/%s", environ[it] + len + 1, LOGFILE );
			return;
		}
	fprintf ( stderr, "ERR_ENVFAIL($HOME)\n" );
	exit(1);
}

void logvfs ( const char * path , const char * lpath ) {
	FILE * flog = 0;
	struct statvfs svfs;
	time_t tmi = time(0);
	struct tm * ntm = localtime(&tmi);
	char buffer[BUFLEN];
	int fail = 0;

	if ( !( flog = fopen( lpath, "a" ) ) ) {
		fprintf ( stderr, "ERR_OPENFAIL(%s)\n", lpath );
		exit(1);
	}

	sprintf( buffer, "%s %s", path, asctime(ntm) );
	buffer[strlen(buffer)-1] = '\0';
	
	switch ( fail = statvfs( path, &svfs ) ) {
	case ALLRIGHT:
		sprintf ( buffer, "%s %.f%c\n", buffer,
			100.0 * ((double) (svfs.f_blocks - svfs.f_bfree)) / ((double) svfs.f_blocks), '%'
		);
		break; /* wystarczylby w zasadzie if */
	case EACCES: case EBADF: case EFAULT: case EINTR:
	case EIO: case ELOOP: case ENAMETOOLONG: case ENOENT:
	case ENOMEM: case ENOSYS: case ENOTDIR: case EOVERFLOW:
	default: /* Aaa, niech zapisze tez do logow, ze bylo cos nie tak */
		sprintf ( buffer, "%s Err:[ I Cannot get statistics(%d)! ]\n", buffer, fail );
	}
		
	if ( !fwrite ( buffer, strlen(buffer), 1, flog ) ) {
		fprintf ( stderr, "ERR_WRITEFAIL(%s)\n", lpath );
		exit(1);
	}
	
	if ( fclose ( flog ) ) {
		fprintf ( stderr, "ERR_CLOSEFAIL(%s)\n", lpath );
		exit(1);
	}
	
	if ( fail != ALLRIGHT ) {
		fprintf ( stderr, "ERR_STATVFSFAIL(%d)\n", fail );
		exit(1);
	}
}

int main ( int argc , char * argv[] ) {
	const char * path;
	char lpath[BUFLEN];
	int pause;
	
	if ( argc < 3 ) {
		fprintf ( stderr, "ERR_USAGEFAIL\n" );
		exit(1);
	}
	
	path = argv[1];
	pause = atoi(argv[2]);
	findlogfile(lpath);
	
	for (;;) {
		logvfs ( path , lpath );
		sleep  ( pause );
	}
	
}
