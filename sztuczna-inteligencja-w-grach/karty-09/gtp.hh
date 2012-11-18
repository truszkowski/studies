#ifndef __SIG_GTP_H__
#define __SIG_GTP_H__

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <stdarg.h>

#define GTP_TIMEOUT_PER_MOVE 1

typedef enum {
	gtp_ok = 0, gtp_eof = -1, gtp_timeout = -2, gtp_error = -3
} gtp_t;

// Opis statusu komunikacji
static inline const char *string_of_gtp(gtp_t t)
{
	static char unknown[32];
	static const char *enums[] = { "Ok", "Koniec pliku", "Minal czas", "Blad" };

	if (t >= -3 && t <= 0) return enums[-t];
	
	snprintf(unknown, 32, "Nieznany blad : %d", t);
	return unknown;
}

// Zapis do kontrolera
static inline gtp_t gtp_fprintf(FILE *stream, const char *fmt, ...) 
{
	va_list args;

	va_start(args, fmt);
	int r = vfprintf(stream, fmt, args);
	va_end(args);
	
	if (r < 0) {
		fprintf(stderr, "GTP : Blad zapisu na `stdout' (%s)\n", strerror(errno));
		abort();
	}
	
	fflush(stream);
	return gtp_ok;
}

// Odczyt od kontrolera 
static inline gtp_t gtp_fscanf(FILE *stream, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	int r = vfscanf(stream, fmt, args);
	va_end(args);
	
	if (r < 0) {
		if (errno == 0) return gtp_eof;
		fprintf(stderr, "GTP : Blad odczytu z `stdin' (%s)\n", strerror(errno)); 
		abort();
	}

	return gtp_ok;
}

#define gtp_scanf(fmt, args...) gtp_fscanf(stdin, fmt, ##args)
#define gtp_printf(fmt, args...) gtp_fprintf(stdout, fmt, ##args)

// Zapis do strumienia i odczyt
static inline gtp_t gtp_communicate(FILE *out, FILE *in, const char *fmt, ...)
{
	va_list args;
	
	va_start(args, fmt);
	int r = vfprintf(out, fmt, args);
	va_end(args);

	if (r < 0) {
		fprintf(stderr, "GTP : Blad zapisu na `stdout' (%s)\n", strerror(errno));
		return gtp_error;
	}
	
	fflush(out); // Zapis 

	int fd = fileno(in);
	if (fd < 0) {
		fprintf(stderr, "GTP : Zly deskryptor `stdin' (%s)\n", strerror(errno));
		abort();
	}

	struct timeval tv = { 1, 0 };
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(fd, &fds);

	int sel = select(fd + 1, &fds, NULL, NULL, &tv);
	if (sel < 0) {
		fprintf(stderr, "GTP : Blad w select() %s\n", strerror(errno));
		abort();
	}

	if (sel == 0) return gtp_timeout;

	char eq[1024]; 

	gtp_t t = gtp_fscanf(in, "%s", eq);
	if (t != gtp_ok) return t;

	if (strcmp(eq, "=")) return gtp_error;

	return gtp_ok;
}

#endif

