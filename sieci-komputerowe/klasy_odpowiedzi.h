#ifndef _KLASY_ODPOWIEDZI_H_
#define _KLASY_ODPOWIEDZI_H_

#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <pthread.h>

/* STL */
#include <set>

/* C */
#include "definicje.h"
#include "operatory.h"
#include "klasy.h"

using namespace std;


/* Klasa odpowiedzi */

class odpowiedz
{
	public:
		slowo nazwa_pliku;
		int id_pliku;
		in_addr nadawca;
		
		odpowiedz() {}
		~odpowiedz() {}
		
		void ustaw(const slowo s, const in_addr nad, const int id)
		{
			strcpy(nazwa_pliku, s);
			id_pliku = id;
			nadawca = nad;
		}
		bool operator==(const odpowiedz & odp)
		{
			return ((nazwa_pliku == odp.nazwa_pliku) && (nadawca == odp.nadawca) && (id_pliku == odp.id_pliku));
		}
		bool operator!=(const odpowiedz & odp)
		{
			return ((nazwa_pliku != odp.nazwa_pliku) && (nadawca != odp.nadawca) && (id_pliku != odp.id_pliku));
		}
		odpowiedz & operator=(const odpowiedz & odp)
		{
			ustaw(odp.nazwa_pliku, odp.nadawca, odp.id_pliku);
			return *this;
		}
		void wypisz()
		{
			printf("> %s %d %s\n",nazwa_pliku, id_pliku, inet_ntoa(nadawca));
		}
};

inline bool operator<(const odpowiedz & odp1, const odpowiedz & odp2)
{
	if (odp1.id_pliku < odp2.id_pliku)
		return true;
	else 
		return (odp1.nadawca.s_addr < odp2.nadawca.s_addr);
}

class odpowiedzi
{
	private:
		set<odpowiedz> zbior_odp;
		pthread_mutex_t semafor;

	public:
		odpowiedzi() 
		{
			pthread_mutex_init(&semafor, NULL);
		}
		~odpowiedzi() {}
		
		void dodaj(const slowo np, const in_addr n, const int d) 
		{
			pthread_mutex_lock(&semafor);
			odpowiedz odp;
			odp.ustaw(np, n, d);
			zbior_odp.insert(odp);
			pthread_mutex_unlock(&semafor);
		}
		bool znajdz(slowo np, in_addr n, int d)
		{
			odpowiedz odp;
			odp.ustaw(np,n,d);
			if (zbior_odp.find(odp) == zbior_odp.end())
				return false;
			else 
				return true;
		}
		void wypisz()
		{
			set<odpowiedz>::iterator it;
			printf("> Szpieg:\n");
			for (it = zbior_odp.begin(); it != zbior_odp.end(); it++)
				printf("> %s %d %s\n",it->nazwa_pliku, it->id_pliku, inet_ntoa(it->nadawca)); //it->wypisz();
		}		
};

#endif
