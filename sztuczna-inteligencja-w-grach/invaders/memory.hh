#ifndef _INVADERS_MEMORY_HH_
#define _INVADERS_MEMORY_HH_

#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <stdarg.h>
#include <memory>
#include <exception>

// Zarzadzamy pamiecia,
// * alloc() - alokacja zadanej liczby blokow =false - zabraklo pamieci,
// * get()   - pobieramy wskaznik, jak =NULL - zabraklo pamieci,
// * put()   - zwracamy wskaznik, wazne aby ten wskaznik byl wczesniej 
//             pobrany przez get().
// * adopt() - przywlaczenie obcej pamieci, bedzie ona dostepna w obiekcie 
//             przez caly czas i uzywana zwyczajnie. NIE jest zwalniana.
//
// Mozna ustawic limit alokacji, nie bedzie alokowanych wiecej elementow.
// Pamiec jest zwalniana tylko raz - w destruktorze.
//

template <class T, size_t BlockSize = 10000>
class Alloc {
	public:
		class Exception : public std::exception {
			public:
				Exception(const char *fmt, ...) throw()
				{
					va_list args;
					va_start(args, fmt);
					vsnprintf(exc, maxexclen, fmt, args);
					va_end(args);
				}
				~Exception(void) throw() { }

				const char *what(void) const throw()
				{
					return exc;
				}

			private:
				static const size_t maxexclen = 256;
				char exc[maxexclen];
		};

		Alloc(size_t limit = 0) 
			: m_all_cnt(0), m_free_cnt(0), m_limit(limit), 
			m_alloced(0), m_inuse(0), m_free(0) { }

		~Alloc(void) 
		{
			while (m_alloced) {
				Alloced *a = m_alloced;
				m_alloced = m_alloced->next;
				delete[] a;
			}
		}

		// allokacja
		bool alloc(size_t blocks = 1) 
		{
#ifndef NDEBUG
			if (m_free_cnt > m_all_cnt || 
					((m_free_cnt > 0) ? (m_free == NULL):(m_free != NULL)) ||
					((m_all_cnt > m_free_cnt) ? (m_inuse == NULL):(m_inuse != NULL)))
				throw Exception("Blad wewnetrzny przy `alloc' : all=%u free=%u free=%p inuse=%p", 
						m_all_cnt, m_free_cnt, m_free, m_inuse);
#endif

			if (m_limit > 0 && m_limit < m_all_cnt + blocks*BlockSize) return false;

			Alloced *a = new(std::nothrow) Alloced[blocks];
			if (a == NULL) return false;

			for (size_t i = 0; i < blocks; ++i) {
				a[i].e[0].t = a[i].t;
				a[i].e[0].next = m_free;
				for (size_t j = 1; j < BlockSize; ++j) {
					a[i].e[j].t = a[i].t+j;
					a[i].e[j].next = a[i].e+i-1;
				}
				a[i].next = m_alloced;
				m_alloced = a+i;
				m_free = a[i].e+BlockSize-1;
			}

			m_all_cnt += blocks*BlockSize;
			m_free_cnt += blocks*BlockSize;

			return true;
		}

		// Pobranie pamieci
		T *get(void)
		{
#ifndef NDEBUG
			if (m_free_cnt > m_all_cnt || 
					((m_free_cnt > 0) ? (m_free == NULL):(m_free != NULL)) ||
					((m_all_cnt > m_free_cnt) ? (m_inuse == NULL):(m_inuse != NULL)))
				throw Exception("Blad wewnetrzny przy `get' : all=%u free=%u free=%p inuse=%p", 
						m_all_cnt, m_free_cnt, m_free, m_inuse);
#endif
			if (m_free_cnt == 0 && !alloc()) return NULL;
			
#ifndef NDEBUG
			if (!m_free || m_free_cnt == 0) 
				throw Exception("Blad wewnetrzny przy `get' : free=%p free=%u", m_free, m_free_cnt);
#endif

			--m_free_cnt;

			Ent *e = m_free;
			m_free = m_free->next;
			e->next = m_inuse;
			m_inuse = e;

			return e->t; 
		}

		// Zwrocenie pamieci do puli
		void put(T *ptr)
		{
#ifndef NDEBUG
			if (!m_inuse || m_free_cnt >= m_all_cnt ||
					((m_free_cnt > 0) ? (m_free == NULL):(m_free != NULL)) ||
					((m_all_cnt > m_free_cnt) ? (m_inuse == NULL):(m_inuse != NULL)))
				throw Exception("Blad wewnetrzny przy `put' : all=%u free=%u free=%p inuse=%p", 
						m_all_cnt, m_free_cnt, m_free, m_inuse);
#endif

			++m_free_cnt;

			Ent *e = m_inuse;
			m_inuse = m_inuse->next;

			e->t = ptr;
			e->next = m_free;
			m_free = e;
		}

		// Przyjmij te pamiec...
		void adopt(void *ptr, size_t len)
		{
			Adopted *a = (Adopted*)ptr;
			size_t sz = len / sizeof(Adopted);
			assert(sz > 0);

			m_all_cnt += sz;
			m_free_cnt += sz;
			
			a[0].e.next = m_free;
			a[0].e.t = &a[0].t;

			for (size_t i = 1; i < sz; ++i) {
				a[i].e.next = &a[i-1].e;
				a[i].e.t = &a[i].t;
			}
			m_free = &a[sz-1].e;

		}

		size_t allCnt(void) const { return m_all_cnt; }
		size_t freeCnt(void) const { return m_free_cnt; }
		size_t inUseCnt(void) const { return m_all_cnt - m_free_cnt; }

	private:
		size_t m_all_cnt, m_free_cnt, m_limit;
		
		struct Ent { T *t; Ent *next; } __attribute__((packed));
		struct Alloced { T t[BlockSize]; Ent e[BlockSize]; Alloced *next; } __attribute__((packed));
		struct Adopted { T t; Ent e; } __attribute__((packed));
		
		Alloced *m_alloced;
		Ent *m_inuse, *m_free;
};

#endif

