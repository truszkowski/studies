#ifndef _KLASY_POLACZENIA_H_
#define _KLASY_POLACZENIA_H_

/* STL */
#include <stack> 
#include <set>

/* C */
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <pthread.h>

#include "definicje.h"
#include "operatory.h"
#include "klasy.h"

using namespace std;


class kanal
{
	public:
		int dsk;
		in_addr adr;
		int typ;
		
		kanal()
		{
			dsk = KANAL_NUL;
			typ = KANAL_NUL;
		}
		~kanal() {}
		void ustaw(int d, in_addr a, int t)
		{
			dsk = d; adr = a; typ = t;
		}
		void ustaw(int t)
		{
			typ = t;
		}
		void reset()
		{
			printf("> Rozłaczony z %s(%d)\n", inet_ntoa(adr), dsk);
			if (close(dsk) == -1)
				printf("> Błąd w close!\n");
			dsk = KANAL_NUL;
			typ = KANAL_NUL;
		}
		bool wolny()
		{
			return (typ == KANAL_NUL);
		}
		bool inf()
		{
			return (typ == KANAL_INF);
		}
		bool pob()
		{
			return (typ == KANAL_POB);
		}
		void wypisz()
		{
			printf("> %s %d %d\n", inet_ntoa(adr), dsk, typ);
		}
};

typedef set<int>::iterator it_inf; // iterator
typedef set<int>::iterator it_nzn; // iterator
typedef set<int>::iterator it_pob; // iterator

class polaczenia
{
	public:
		in_addr ip;
		uint16_t port;
		int user_odczyt;
		int socket_odczyt;
		kanal kanaly[MAX_KANAL];
		set<int> zbior_inf;
		set<int> zbior_nzn;
		set<int> zbior_pob;
		int dsk2rek[10000]; 
		stack<int> wolne;
		pthread_mutex_t semafor;
		
		polaczenia()
		{
			int i;
			user_odczyt = 0;
			socket_odczyt = 0;
			for (i = 0; i < MAX_KANAL; i++)
				wolne.push(i);
			pthread_mutex_init(&semafor, NULL);  //
			
		}
		~polaczenia() {}
		
		int rekord2dsk(int rek)
		{
			return kanaly[rek].dsk;
		}
		void start(char * adres)
		{
			ustaw_user();
			ustaw_socket(adres);
		}
		void koniec()
		{
			int i;
			printf("> Zamykanie otwartych połączeń...\n");
			for (i = 0; i < MAX_KANAL; i++)
				if (kanaly[i].typ != KANAL_NUL)
					reset_kanal(i);
			printf("> Zamykanie gniazda...\n");
			reset_socket();
			printf("> Zamykanie interfejsu...\n");
			reset_user();
		}
		
		int ustaw_kanal(int d, in_addr a, int t)
		{
			if (t == KANAL_INF)
				return ustaw_kanal_inf(d,a);
			else if (t == KANAL_NZN)
				return ustaw_kanal_nzn(d,a);
			else 
				return ustaw_kanal_pob(d,a,t);
		}
		int ustaw_kanal_nzn(int d, in_addr a)
		{
			
			pthread_mutex_lock(&semafor);
			int i, r = wolne_nzn(); // Jak cos jest wolne to jest od razu zdejmowane!
			if (r == -1) {
				printf("> Brak wolnych połączeń!\n");
				pthread_mutex_unlock(&semafor);
				return -1;
			}
			kanaly[r].ustaw(d,a,KANAL_NZN);
			dsk2rek[d] = r;
			zbior_nzn.insert(r);
			printf("> Połączony z %s(%d)\n", inet_ntoa(a), d);
			pthread_mutex_unlock(&semafor);
			return r;
		}
		int ustaw_kanal_pob(int d, in_addr a, int t)
		{
			pthread_mutex_lock(&semafor);
			int i, r = wolne_pob(); // Jak cos jest wolne to jest od razu zdejmowane!
			if (r == -1) {
				printf("> Brak wolnych połączeń!\n");
				pthread_mutex_unlock(&semafor);
				return -1;
			}
			kanaly[r].ustaw(d,a,t);
			dsk2rek[d] = r;
			zbior_pob.insert(r);
			printf("> Połączony z %s(%d)\n", inet_ntoa(a), d);
			pthread_mutex_unlock(&semafor);
			return r;
		}
		int ustaw_kanal_inf(int d, in_addr a)
		{
			pthread_mutex_lock(&semafor);
			int i, r = wolne_inf(); // Jak cos jest wolne to jest od razu zdejmowane!
			if (r == -1) {
				printf("> Brak wolnych połączeń!\n");
				pthread_mutex_unlock(&semafor);
				return -1;
			}
			kanaly[r].ustaw(d,a,KANAL_INF);
			dsk2rek[d] = r;
			zbior_inf.insert(r);
			printf("> Połączony z %s(%d)\n", inet_ntoa(a), d);
			pthread_mutex_unlock(&semafor);
			return r;
		}
		int dsk2rekord(int d)
		{
			return dsk2rek[d];
		}
		void reset_kanal(int r)
		{
			pthread_mutex_lock(&semafor);
			if (kanaly[r].typ == KANAL_INF)
				zbior_inf.erase(r);
			else if ((kanaly[r].typ == KANAL_POB) || (kanaly[r].typ == KANAL_FRG) || (kanaly[r].typ == KANAL_PSR))
				zbior_pob.erase(r);
			else 
				zbior_nzn.erase(r);
			wolne.push(r);
			kanaly[r].reset();				
			pthread_mutex_unlock(&semafor);
		}
		int ustaw(int x, int t) // x - rekord
		{			
			pthread_mutex_lock(&semafor);
			if (!kanaly[x].wolny()) {
				if ((t == KANAL_INF) && (zbior_inf.size() < MAX_INF)) {
					kanaly[x].typ = t;
					zbior_inf.insert(x);
				} else if (((t == KANAL_POB) || (t == KANAL_FRG) || (t == KANAL_PSR)) && (zbior_pob.size() < MAX_POB)) {
					kanaly[x].typ = t;
					zbior_pob.insert(x);
				} else {
					reset_kanal(x);
					pthread_mutex_unlock(&semafor);
					return -1;
				}
				zbior_nzn.erase(x);
			} else
				x = -1;
			pthread_mutex_unlock(&semafor);
			return x; // -1?
		}
		void wypisz()
		{
			it_inf i;
			printf("> Adres: %s\n", inet_ntoa(ip));
			printf("> Połaczenia:\n");
			printf("> odczyt z gniazda: %d\n", socket_odczyt);
			if (!zbior_nzn.empty()) printf("> Połączenia nieokreślone:\n");
			for (i = iterator_nzn(); i != it_nzn_koniec(); i++) {
				printf("> %d\t", *i);
				kanaly[*i].wypisz();
			}
			if (!zbior_inf.empty()) printf("> Połączenia informacyjne:\n");
			for (i = iterator_inf(); i != it_inf_koniec(); i++) {
				printf("> %d\t", *i);
				kanaly[*i].wypisz();
			}
			if (!zbior_pob.empty()) printf("> Połączenia pobierania:\n");
			for (i = iterator_pob(); i != it_pob_koniec(); i++) {
				printf("> %d\t", *i);
				kanaly[*i].wypisz();
			}
		}
		it_inf iterator_inf()
		{
			return zbior_inf.begin();
		}
		it_nzn iterator_nzn()
		{
			return zbior_nzn.begin();
		}
		it_pob iterator_pob()
		{
			return zbior_pob.begin();
		}
		int polacz(in_addr a, int typ)
		{
			return -1;
		}
		it_inf it_inf_koniec()
		{
			return zbior_inf.end();
		}
		it_nzn it_nzn_koniec()
		{
			return zbior_nzn.end();
		}
		it_pob it_pob_koniec()
		{
			return zbior_pob.end();
		}
	private:
		void ustaw_user()
		{
			user_odczyt = 0;
		}
		void ustaw_socket(char * adr)
		{
			hostent * htmp;
			slowo host, adres;
			struct sockaddr_in serwer;
			socklen_t dlugosc;
			int gniazdo;
			
			strncpy(adres, adr, MAX_SLW);

			if (strcmp(adres, "domyślny") == 0) {
				gethostname(host, sizeof(host));
				if ((htmp = gethostbyname(host)) == 0)
					perror("> Błąd w gethostbyname!\n");
			} else 
				strcpy(host, adres);
			
			if ((gniazdo = socket(PF_INET, SOCK_STREAM, 0)) < 0) 
				perror("> Błąd w socket!\n"); 
			
				serwer.sin_family = AF_INET;
			
			if (strcmp(adres, "domyślny") == 0) {
				memcpy((char *) &serwer.sin_addr, (char *) htmp->h_addr, (size_t) htmp->h_length);
			} else
				inet_aton(adres, &serwer.sin_addr);
				
//				serwer.sin_addr.s_addr = htonl(INADDR_ANY);
				serwer.sin_port = PORT;
			
			if (bind(gniazdo, (struct sockaddr *) &serwer, (socklen_t) sizeof(serwer)) < 0) 
				perror("> Błąd w bind!\n");
			
			dlugosc = sizeof(serwer);
			if (getsockname(gniazdo, (struct sockaddr *) &serwer, (socklen_t *) &dlugosc) < 0) 
				perror("> Błąd w getsockname!\n"); 
				
			if (listen(gniazdo, 5) == -1) 
				perror("> Błąd w listen!\n"); 
			
			socket_odczyt = gniazdo;
			ip = serwer.sin_addr;	
			
			printf("> Adres: %s (%s)\n", host, inet_ntoa(ip));
			printf("> Otwarty port: %d\n", htons(PORT));
		}
		int wolne_inf() 
		{
			int i;
			if (zbior_inf.size() == MAX_INF)
				return -1;
			i = wolne.top();
			wolne.pop();
			return i;
		}
		int wolne_pob()
		{
			int i;
			if (zbior_pob.size() == (MAX_KANAL - MAX_INF - MAX_NZN))
				return -1;
			i = wolne.top();
			wolne.pop();
			return i;
		}
		int wolne_nzn()
		{
			int i;
			if (zbior_nzn.size() == MAX_NZN)
				return -1;
			i = wolne.top();
			wolne.pop();
			return i;
		}
		void reset_socket()
		{
			if (close(socket_odczyt) == -1) 
				printf("> Błąd w close!\n");
			socket_odczyt = 0;
		}
		void reset_user()
		{
			/* Nic... */
		}
};

#endif
