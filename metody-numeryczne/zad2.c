#include <stdio.h>
#include <stdlib.h>
#include "method.h"

int main( int argc , char * argv[] ) {
	if ( 	argc < 3 )
		printf  ("%s <alfa> <rozmiar>\n", argv[0]);
	else
		testall ( atof(argv[1]), atoi(argv[2]) );
	return 0;
}
