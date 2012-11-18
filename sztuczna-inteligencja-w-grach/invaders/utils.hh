#ifndef __INVADERS_UTILS_HH_
#define __INVADERS_UTILS_HH_

#include <stdint.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/time.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <exception>
#include <cassert>

// Timer, moze sie przyda...
class Clock {
	public:
		Clock(void) { tic(); }
		~Clock(void) { }

		void tic(void) 
		{
			gettimeofday(&t, 0);
		}

		int toc(void) const
		{
			struct timeval t2;
			gettimeofday(&t2, 0);
			
			uint64_t passed = 
				(((uint64_t)t2.tv_sec)*1000000ULL + ((uint64_t)t2.tv_usec))
				- 
				(((uint64_t)t.tv_sec)*1000000ULL + ((uint64_t)t.tv_usec));

			return (int)(passed/1000ULL);
		}

	private:
		struct timeval t;
};

// Gdy jakis mega-blad
class invaders_error : std::exception {
	public:
		invaders_error(const char *fmt, ...) throw() 
		{
			va_list args;
			va_start(args, fmt);
			vsnprintf(error, maxerrorlen, fmt, args);
			va_end(args);
		}
		~invaders_error(void) throw() { }

		const char *what(void) const throw() 
		{
			return error;
		}

	private:
		static const size_t maxerrorlen = 256;
		char error[maxerrorlen];
};

#endif

