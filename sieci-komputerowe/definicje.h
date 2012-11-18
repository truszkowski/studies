#ifndef _DEFINICJE_H_
#define _DEFINICJE_H_

/* Port do komunikacji */
#define PORT htons(16666)

/* Maksymalna głebokość przeszukiwania */
#define MAX_GLB 5
/* Ograniczenie na wrowadzane słowa */
#define MAX_SLW 50
/* Rozmiar pobieranych fragmentów plików */
#define MAX_DANE 2000
/* Określenie trybu debugerowania */
#define DEBUG true

/* Maksymalna liczba otwartych kanałów informacyjnych */
#define MAX_INF 10
/* Maksymalna liczba otwartych kanałów niezdefiniowanych */
#define MAX_NZN 10
/* Maksymalna liczba udostępnianych plików */
#define MAX_UDS 100
/* Maksymalna liczba pobieranych plików */
#define MAX_POB 100
/* Maksymalna liczba przekazywanych fragmentów */
#define MAX_FRG 100
/* Maksymalna liczba odpytań */
#define MAX_ODP 20
/* Maksymalna liczba zapytań */
#define MAX_PYT 20
/* Maksymalna liczba posiadanych sąsiadów */
#define MAX_SAS MAX_INF
/* Maksymalna liczba źrodeł */
#define MAX_ZDL 20
/* Maksymalna liczba od ilu można sciągać jeden plik naraz */
#define MAX_NRZ MAX_ZDL
/* Minimalna porcja fragmentów */
#define MIN_PRC 10
/* Maksymalna porcja fragmentów */
#define MAX_PRC 10
/* Maxymalna liczba ponownych połączeń */
#define MAX_PRZERWAN 10

/* Maksymalna liczba kanałów */
#define MAX_KANAL (MAX_INF + (MAX_POB * MAX_ZDL))
/* Kanał informacyjny */
#define KANAL_INF 1
/* Kanał przeznaczony wyłącznie do pobierania plików */
#define KANAL_POB 2
/* Kanał przeznaczony wyłącznie do wysyłania plików */
#define KANAL_FRG 3
/* Kanał przeznaczony wyłącznie do pośredniczenia */
#define KANAL_PSR 4
/* Kanał niezdefiniowany */
#define KANAL_NZN 5
/* Kanał pusty */
#define KANAL_NUL 0

/* Na raz działajacych watków łaczenia się */
#define MAX_WTK_POL_INF 5
/* Na raz działajacych watków łaczenia się */
#define MAX_WTK_POL_POB 50
/* Tyle plików naraz można ściągać */
#define MAX_WTK_POB 10
/* Tyle można wysyłać naraz */
#define MAX_WTK_FRG 50

/* Katalog z plikami udostępnianymi */
#define KATALOG_UDS "./"  /* "/home/tmp/pt/" */
/* Katalog z pobranymi plikami */
#define KATALOG_POB "./tmp/" /* "/home/tmp/pt/tmp/" */


/* Słowo długości maksymalnie MAX_SLW   */
typedef char slowo[MAX_SLW];
/* PRzechowywane dane z fragmentu pliku */
typedef char dane[MAX_DANE];


/* SOCKET_PAKIET */

/* Informacja o rodzaju połączenia */
#define SOCKET_INF 1
/* Zapytanie o plik */
#define SOCKET_PYT 2
/* Odpowiedź o pliku */
#define SOCKET_ODP 3
/* Zapytanie o sąsiadów */
#define SOCKET_SAS_PYT 4
/* Odpowiedź o sąsiadach */
#define SOCKET_SAS_ODP 5
/* Niemożność ustanowienia bezpośredniego połączenia - przesyłana jest lista sąsiadów */
#define SOCKET_SAS_PSR 6
/* Żadanie pobrania pliku */
#define SOCKET_POB 7
/* Fragment pliku */
#define SOCKET_FRG 8
/* Żadanie połączenia z drugiej strony */
#define SOCKET_KMN 9
/* Informacja o niemożności połączenia */
#define SOCKET_BRK 10


struct socket_pakiet
{
	int typ;
};

#define R_SOCKET_PAKIET sizeof(struct socket_pakiet)

/* SOCKET_INF */

struct inf_pakiet 
{
	int inf;          
	slowo nazwa_pliku;     // Dodatkowe rozróżnianie połączeń pobierania.
};

#define R_INF_PAKIET sizeof(struct inf_pakiet)

/* SOCKET_PYT */

struct pyt_pakiet
{
	int id;
	int glebokosc;
	slowo nazwa_pliku;
	in_addr nadawca;
	in_addr posrednicy[MAX_GLB];
};

#define R_PYT_PAKIET sizeof(struct pyt_pakiet)

/* SOCKET_ODP */

struct odp_pakiet
{
	int id;
	int glebokosc;
	slowo nazwa_pliku;
	off_t rozmiar;
	in_addr nadawca;
	in_addr posrednicy[MAX_GLB];
	long liczba_frg;
	long frg_start[MAX_NRZ];
	long frg_koniec[MAX_NRZ];
};

#define R_ODP_PAKIET sizeof(struct odp_pakiet)

/* SOCKET_SAS_PYT */

struct sas_pyt_pakiet
{
	char pyt;
};

#define R_SAS_PYT_PAKIET sizeof(struct sas_pyt_pakiet)

/* SOCKET_SAS_ODP */

struct sas_odp_pakiet
{
	int liczba;
	in_addr lista[MAX_INF];
};

#define R_SAS_ODP_PAKIET sizeof(struct sas_odp_pakiet)

/* SOCKET_SAS_PSR */

struct sas_psr_pakiet
{
	int liczba;
	in_addr lista[MAX_SAS];
};

#define R_SAS_PSR_PAKIET sizeof(struct sas_psr_pakiet)

/* SOCKET_POB */

struct pob_pakiet
{
	slowo nazwa_pliku;
	long frg_start;
	long frg_koniec;
	int id;
	in_addr nadawca;
};

#define R_POB_PAKIET sizeof(struct pob_pakiet)

/* SOCKET_FRG */

struct frg_pakiet
{
	long nr_frg;
	long rozmiar;
	dane fragment;
};

#define R_FRG_PAKIET sizeof(struct frg_pakiet)

/* SOCKET_KMN */

struct kmn_pakiet
{
	in_addr nadawca;
	in_addr adresat;
	slowo nazwa_pliku;
	int id_pliku;
	int glebokosc;
	in_addr posrednicy[MAX_GLB];
};

#define R_KMN_PAKIET sizeof(struct kmn_pakiet)

/* SOCKET_BRK */

struct brk_pakiet
{
	in_addr nadawca;
	in_addr adresat;
	slowo nazwa_pliku;
	int id_pliku;
	int glebokosc;
	in_addr posrednicy[MAX_GLB];
};

#define R_BRK_PAKIET sizeof(struct brk_pakiet)

#endif
