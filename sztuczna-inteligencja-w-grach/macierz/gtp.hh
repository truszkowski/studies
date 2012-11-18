#ifndef __SIG_GTP_H__
#define __SIG_GTP_H__

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <stdarg.h>

typedef enum {
	gtp_ok = 0, gtp_eof = -1
} gtp_t;

// Zapis do kontrolera
static inline gtp_t gtp_printf(const char *fmt, ...) 
{
	va_list args;

	va_start(args, fmt);
	int r = vfprintf(stdout, fmt, args);
	va_end(args);
	
	if (r < 0) {
		if (errno == 0) return gtp_eof;
		fprintf(stderr, "Blad zapisu na `stdout' (%s)\n", strerror(errno));
		abort();
	}
	
	fflush(stdout);
	return gtp_ok;
}

// Odczyt od kontrolera 
static inline gtp_t gtp_scanf(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	int r = vfscanf(stdin, fmt, args);
	va_end(args);
	
	if (r < 0) {
		if (errno == 0) return gtp_eof;
		fprintf(stderr, "Blad odczytu z `stdin' (%s)\n", strerror(errno)); 
		abort();
	}

	return gtp_ok;
}

#endif

