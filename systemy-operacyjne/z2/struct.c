#include "struct.h"

void list_init( struct list * a ) { 
	a->next = a; 
	a->prev = a; 
}

void list_next( struct list * a, struct list * b ) {
	a->next->prev = b->prev; 
	b->prev->next = a->next;
	a->next = b; 
	b->prev = a;
}

int getline( char * buf , FILE * f ) { 
	int pos;
	if ( fgets( buf, BUFLEN, f ) == NULL ) return 1;
	pos = 0; while ( buf[++pos] != '\n' ); buf[pos] = 0;
	return 0;
}

char * lastname ( char * path ) {
	int i = 0, j = 0;
	while ( path[++j] ) if ( path[j] == '/' && path[j+1] ) i = j;
	return path + i + 1;
}

#include <string.h>
 
extern unsigned int sessCounter;
extern struct SESSION sessions[SESSIONLIMIT];

unsigned int findsession( const char * path ) {
	unsigned int i;
	
	for ( i = 0; i < sessCounter; ++i ) 
		if ( !strncmp( path, sessions[i].nm, 20 ) )
			return i;

	return -1;
}

#include <time.h>
#include <stdlib.h>

extern int daylight; /* zebysmy wiedzieli ile nam do Greenwich */

void settime( const char * src, char * dest, time_t * t ) {
/** format- YYYY-MM-DD_HH:MM:SS (GMT) **/	
	struct tm tmi;
	char buffer[BUFLEN];
	
	strncpy( buffer, src, BUFLEN );

	buffer[22] = 0; 
	buffer[13] = 0; tmi.tm_hour  = atoi( buffer + 11 ) - atoi( buffer + 19 );
	buffer[19] = 0; tmi.tm_sec   = atoi( buffer + 17 ); 
	buffer[16] = 0; tmi.tm_min   = atoi( buffer + 14 ); 
	buffer[10] = 0; tmi.tm_mday  = atoi( buffer +  8 ); 
	buffer[ 7] = 0; tmi.tm_mon   = atoi( buffer +  5 ) - 1; 
	buffer[ 4] = 0; tmi.tm_year  = atoi( buffer ) - 1900; 
	tmi.tm_isdst = 0; 

/* katalogi niechsa wg GMT, ale czas niech pokazuje lokalny */
	*t = mktime( &tmi ) + 60 * 60 * daylight;
	
	dest[ 0] = '/';
	dest[ 1] = ((char) ( (tmi.tm_year + 1900) / 1000 )) + '0';
	dest[ 2] = ((char) ( ( (tmi.tm_year + 1900) / 100 ) % 10 )) + '0' ; 
	dest[ 3] = ((char) ( ( (tmi.tm_year + 1900) / 10 ) % 10 )) + '0' ;
	dest[ 4] = ((char) ( (tmi.tm_year + 1900) % 10 )) + '0' ;
	dest[ 5] = '-';
	dest[ 6] = ((char) ( (tmi.tm_mon + 1) / 10 )) + '0' ;
	dest[ 7] = ((char) ( (tmi.tm_mon + 1) % 10 )) + '0' ;
	dest[ 8] = '-';
	dest[ 9] = ((char) ( (tmi.tm_mday) / 10 )) + '0' ;
	dest[10] = ((char) ( (tmi.tm_mday) % 10 )) + '0' ;
	dest[11] = '_';
	dest[12] = ((char) ( (tmi.tm_hour) / 10 )) + '0' ;
	dest[13] = ((char) ( (tmi.tm_hour) % 10 )) + '0' ;
	dest[14] = ':';
	dest[15] = ((char) ( (tmi.tm_min) / 10 )) + '0' ;
	dest[16] = ((char) ( (tmi.tm_min) % 10 )) + '0' ;
	dest[17] = ':';
	dest[18] = ((char) ( (tmi.tm_sec) / 10 )) + '0' ;
	dest[19] = ((char) ( (tmi.tm_sec) % 10 )) + '0' ;
	dest[20] = '\0';
	
}

