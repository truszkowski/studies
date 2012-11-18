/* Piotr Truszkoski --- pt209223 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include "err.h"
#include "komunikaty.h"


/* Wypisywanie szczegółów conajwyżej poziomu 1.      */
void printf_1(const char * arg1, ...)
{
	va_list args;
	if (SZCZEGOLY >= 1) {	
		va_start(args, arg1);
		vprintf(arg1, args);
		va_end(args);	
	}
}

/* Wypisywanie szczegółów conajwyżej poziomu 2.      */
void printf_2(const char * arg1, ...)
{
	va_list args;
	if (SZCZEGOLY >= 2) {	
		va_start(args, arg1);
		vprintf(arg1, args);
		va_end(args);	
	}
}
