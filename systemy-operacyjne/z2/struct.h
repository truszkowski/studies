#ifndef __PT209223_STRUCT_H__
#define __PT209223_STRUCT_H__

/* implementacji list, zgrubsza taka jak w kernelu linuxa */
struct list { struct list * prev, * next; };

extern void list_init( struct list * a );
extern void list_next( struct list * a, struct list * b );

#include <sys/types.h>
#include <sys/stat.h>

#define BUFLEN             1024 /* rozmiar bufora */
#define PATHLEN          BUFLEN /* maksymalna dlugosc sciezki */
#define SESSIONLIMIT        128 /* maksymalna liczba sesji */

/* 
 * Rozdzielam metryczke plika na dwie gdyz, 
 * wiele informacji sie by powrtarzalo gdybym 
 * przechoywal poprostu struct stat
 */
 
/* metryczka pliku */
struct file_stat {
	mode_t mode;             /* albo katalog albo plik */
	off_t size;              /* rozmiar */
	nlink_t nlink;           /* tw. dowiazania */
	time_t atime;            /* data dostepu */
};

/* metryczna wspolna */
struct global_stat {
	int uid;                 /* wlasciciel */
	int gid;                 /* wlasciciel */
	time_t time;             /* czas modyfikacji/zmiany */
};

/* tak wyglada plik */
struct node {
	char nm[PATHLEN];        /* sciezka do pliku (logiczna) */
	struct file_stat st;     /* metryczka pliku */
	struct list * in;        /* zawartosc katalogu */
	struct list next;        /* sasiedzi w katalogu */
	char tmpnm[PATHLEN];     /* lokalizacja w /tmp/ */
	int count;               /* licznik odwolan */
	pthread_mutex_t mutex;   /* ochrona */
};

/* cala struktura plikow  */
struct SESSION { 
	char nm[PATHLEN];        /* sciezka do katalogu (logiczna) */
	char date[PATHLEN];      /* format daty rdiff-owski */
	struct node * nodes;     /* tablica plikow */
	struct global_stat st;   /* metryczka globalna */
	unsigned int nodeslen;   /* ile plikow */
};

/* wydobycie wezla z listy (cos na wzor makra current w linuxie) */
#define getnode( x ) \
  ( (struct node *) (((char *) x) - sizeof(struct list *) - sizeof(struct file_stat) - PATHLEN ) )

/* podmiana metryczek na struct stat */
#define makestat( st, file_s, global_s ) {                              \
  bzero( (st), sizeof(struct stat) );                                   \
  (st)->st_uid = (global_s)->uid; (st)->st_gid = (global_s)->gid;       \
  (st)->st_ctime = (st)->st_mtime = (global_s)->time;                   \
  (st)->st_atime = (file_s)->atime; (st)->st_nlink = (file_s)->nlink;   \
  (st)->st_mode = (file_s)->mode; (st)->st_size = (file_s)->size;       \
}

/* ukrywa date ze sciezki */
#define hidedate(x) \
  ( ( x ) + 20 ) /* /YYYY-MM-DD_HH:MM:SS <- 20 */

#include <stdio.h>

extern int getline( char * buf , FILE * f );    /* pobranie linni */
extern char * lastname ( char * path );         /* nazwa pliku/katalogu */
extern unsigned int findsession( const char * path ); /* znajdz sesje */
extern void settime( const char * src, char * dest, time_t * t ); /* konwerter czasu na GMT */

/* zamiana zmiennych */
#define swap( a, b ) { (a)^=(b); (b)^=(a); (a)^=(b); }
#define Swap( Type, A, B ) {                          \
  struct Type C;                                      \
  memcpy( &(C), &(A), sizeof(struct Type) );          \
  memcpy( &(A), &(B), sizeof(struct Type) );          \
  memcpy( &(B), &(C), sizeof(struct Type) );          \
}

#endif
