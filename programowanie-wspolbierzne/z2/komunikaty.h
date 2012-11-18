/* Piotr Truszkoski --- pt209223 */

#ifndef _KOMUNIKATY_H_
#define _KOMUNIKATY_H_

/* Nadzorca wykonawców tworzy kolejke do komunikacji *
 * wykonawców z klientami.                           */
#define KLUCZ          989L 

/* Nadzorcy dodatkowo tworzą kolejki do składania    *
 * raportów.                                         */
#define KLUCZ_KLI      468L
#define KLUCZ_WYK      101L

/* Typy przyznawania priorytetów                     */
#define LOSOWO            1
#define MODULO            2

/* Flagi dla typów komunikatów.                      */
#define PYTANIE           1 /* pytanie o sume a + b  */
#define ODPOWIEDZ         2 /* odpowiedz c = a + b   */
#define KONIEC            3 /* ogloszenie o koncu    */

/* Stała poziomu szczegółowości komunikatów 0,1,2    *
 * 0 - tylko informacje zbiorcze,                    *
 * 1 - dodatkowo informacje od nadzoców,             *
 * 2 - dodatkowo informacje od klientów i wykonawców */
#define SZCZEGOLY 2

/* Przesyłana struktura w kolejkach komunikatów.     */
typedef struct
{
	int a;           /* liczba a                       */
	int b;           /* liczba b                       */
	int c;           /* liczba c(suma a i b            */
	int typ;         /* typ komunikatu                 */
	long klient;     /* numer klienta(1-M)             */
	long priorytet;  /* numer priorytetu(1-K)          */
} paczka; 

typedef struct
{
	long mtype;
	paczka dane;
} komunikat;
	
#define ROZMIAR sizeof(paczka)

/* Wypisywanie szczegółów conajwyżej poziomu 1.      */
extern void printf_1(const char * arg1, ...);
/* Wypisywanie szczegółów conajwyżej poziomu 2.      */
extern void printf_2(const char * arg1, ...);

#endif

