#ifndef _KONF_H_
#define _KONF_H_


/* Obsługa błędów: */
#define PIPE(arg) if (pipe (arg) == -1) syserr("Błąd w pipe(%d)!",getpid());
#define CLOSE(arg) if (close (arg) == -1) syserr("Błąd w  close(%d)!",getpid());
#define WAIT(arg) if (wait (arg) == -1) syserr("Błąd w  wait(%d)!",getpid());
#define WRITE(arg1,arg2,arg3) if (write (arg1,arg2,arg3) <= 0) syserr("Błąd w write(%d)!",getpid());
#define READ(arg1,arg2,arg3) if (read (arg1,arg2,arg3) <= 0) syserr("Błąd w read(%d)!",getpid());
#define ERR(arg) syserr("Error in %s(%d)",arg,getpid());


/* Stałe: */
#define MAX 512 /* maksymalna długośc słowa */


/* Struktura pakietu jaki jest przesyłany między procesami w pierścieniu: */
struct pakiet {
  char tresc[MAX]; /* przechowyane słowo */
  int licznik; /* ile razy ma pakiet być przetworzony */
};


/* Struktura pakietu konfiguracyjnego dla pierścienia: */
struct info {
	int ostatni; /* przechowywany nr deskryptora zapisu dla ostatniego procesu */
	int licznik; /* ile ma jeszcze procesów powstać */
};


/* Rozmiary struktur. */
#define PAKIET_ROZ sizeof(struct pakiet)
#define INFO_ROZ sizeof(struct info)


#endif

