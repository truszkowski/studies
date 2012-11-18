#ifndef __SIG_GTP_H__
#define __SIG_GTP_H__

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <stdarg.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>

// Zapis do kontrolera
static inline void gtp_printf(const char *fmt, ...) 
{
	va_list args;

	va_start(args, fmt);
	int r = vprintf(fmt, args);
	va_end(args);
	
	if (r < 0) {
		fprintf(stderr, "gtp_printf : Blad zapisu na `stdout' (%s)\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	fflush(stdout);
}

// Odczyt calej linni(maksymalnie `len-1' bajtow) od kontrolera
static inline void gtp_readline(char *line, size_t len)
{
	size_t mv = 0;
	while (true) {
		if (fgets(line+mv, len-mv, stdin) == NULL) {
			if (errno == 0)
				fprintf(stderr, "gtp_readline : Koniec strumienia `stdin'\n");
			else 
				fprintf(stderr, "gtp_readline : Blad odczytu z `stdin' (%s)\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		mv += strlen(line);
		if (line[mv-1] == '\n') break;
	}
}

#endif

