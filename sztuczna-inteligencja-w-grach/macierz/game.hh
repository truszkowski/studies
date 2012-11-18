#ifndef __SIG_GAME_HH__
#define __SIG_GAME_HH__

#include <stdint.h>

#define MaxDepth 12
#define MaxBufLen ( ( (1<<(2*MaxDepth)) - 1 ) / 3 )
#define Mask 1

// Swiat gry
static const int World[2][2][2] = 
{ //     l (min)   r (min) = gracz 1
	{ { 1,-1 }, { -1,1 } }, // s (max) = gracz 0
	{ { -3,3 }, { 3,-3 } }  // b (max) = gracz 0
};

// Rozgrywka
class Game {
	public:
		uint32_t history;       //-- historia ostatnich ruchow. --//
		uint32_t moves;         //-- liczba wykonanych ruchow.  --//
		int rates[2];           //-- wynik dotychczasowy        --//
		
		struct Map {
			uint8_t prevs[2], mask;
		} __attribute__((packed));

		static uint32_t bmask[13];
		Map *map[MaxDepth];
		char buf[MaxBufLen * sizeof(Map)];

		// Zgodnosc wykonywanych ruchow wg algorytmu.
		// wszystkie trafy / dobre trafy (przez gracza 0|1).
		uint32_t all[MaxDepth][2], ok[MaxDepth][2], 
						 // Ostatnie ruchy - na ich podstawie mozemy oceniac 
						 // czy warto sie trzymac strategii.
						 ok2[MaxDepth][2], ok2cnt[MaxDepth][2];
		bool ch2[MaxDepth][2]; // Ktorej strategi sie trzymac.

		Game(void) 
		{ 
			bzero(this, sizeof(Game));
			Map *mem = (Map*)buf;
			for (size_t i = 0, l = 1; i < MaxDepth; ++i, l *= 4) {
				map[i] = mem;
				mem += l;
			}
		}
		~Game(void) { }

		bool checkMatches(int as, int mv) const
		{
			for (size_t i = MaxDepth-1; i > 0; --i) {
				// gdy ch2[i][as] jest true oznacza to ze trzymamy sie strategii zmiany, czyli robimy ruch jakiego
				// przeciwnik sie nie spodziewa.
				if (all[!as][i] >= 3 && (((float)all[!as][i])*0.8 <= ((float)ok[!as][i]) || ch2[i][as])) {
					// Czyli przeciwnik zgrubsza zachowuje sie wg naszego scenariusza.
					int hits0, hits1;
					if (as == 0) {
						hits0 = zeros_cnt(map[i][history&bmask[i]].prevs[1], map[i][history&bmask[i]].mask&Mask);
						hits1 = ones_cnt(map[i][history&bmask[i]].prevs[1], map[i][history&bmask[i]].mask&Mask);
						mv = (hits0 >= hits1) ? 1:0; // Odpowiadam odwrotnie od oczekiwan :)
					} else {
						hits0 = zeros_cnt(map[i][history&bmask[i]].prevs[0], map[i][history&bmask[i]].mask&Mask);
						hits1 = ones_cnt(map[i][history&bmask[i]].prevs[0], map[i][history&bmask[i]].mask&Mask);
						mv = (hits0 >= hits1) ? 0:1; // Opowiadam odwrotnie od oczekiwan :)
					}
					return true;
				} 
			}
			return false;
		}

		int move(int as) const 
		{
			int mv = (rand() % 3) ? 0:1;
			if (checkMatches(as, mv)) return mv;

			for (size_t i = MaxDepth-1; i > 0; --i) {
				if (moves >= i && map[i][history&bmask[i]].mask > 0) {
					int hits0, hits1;
					if (as == 0) { // wybieramy wiersz
						hits0 = zeros_cnt(map[i][history&bmask[i]].prevs[1], map[i][history&bmask[i]].mask&Mask);
						hits1 = ones_cnt(map[i][history&bmask[i]].prevs[1], map[i][history&bmask[i]].mask&Mask);
						return hits0 >= hits1 ? 0:1;
					} else { // wybieramy kolumne
						hits0 = zeros_cnt(map[i][history&bmask[i]].prevs[0], map[i][history&bmask[i]].mask&Mask);
						hits1 = ones_cnt(map[i][history&bmask[i]].prevs[0], map[i][history&bmask[i]].mask&Mask);
						return hits0 >= hits1 ? 1:0;
					}
				}
			}

			return mv;
		}
	
		void add(int m0, int m1) 
		{ // dodanie posuniecia do historii
			for (size_t i = 1; i < MaxDepth; ++i) {
				if (moves >= i) {
					map[i][history&bmask[i]].prevs[0] = (map[i][history&bmask[i]].prevs[0] << 1)|m0;
					map[i][history&bmask[i]].prevs[1] = (map[i][history&bmask[i]].prevs[1] << 1)|m1;
					map[i][history&bmask[i]].mask = (map[i][history&bmask[i]].mask << 1)|1;
					int hits0, hits1;

					if (all[i][0] == 0) ++ok[i][0]; // Pierwszy zawsze dobry
					else { 
						hits0 = zeros_cnt(map[i][history&bmask[i]].prevs[1], map[i][history&bmask[i]].mask&Mask);
						hits1 = ones_cnt(map[i][history&bmask[i]].prevs[1], map[i][history&bmask[i]].mask&Mask);
						if (hits0 >= hits1) { if (m0 == 0) ++ok[i][0]; }
						else if (m0 == 1) ++ok[i][0];
					}
					
					if (all[i][1] == 0) ++ok[i][1]; // Pierwszy zawsze dobry
					else { 
						hits0 = zeros_cnt(map[i][history&bmask[i]].prevs[0], map[i][history&bmask[i]].mask&Mask);
						hits1 = ones_cnt(map[i][history&bmask[i]].prevs[0], map[i][history&bmask[i]].mask&Mask);				
						if (hits0 >= hits1) { if (m1 == 0) ++ok[i][1]; }
						else if (m1 == 1) ++ok[i][1];
					}

					int p0 = last32_rate(i, 0); // Patrzymy na statystyki z ostatnich (max 32) gier
					int p1 = last32_rate(i, 1); // --//--

					// jesli przez te ostatnie gry bylo zle to zmieniamy strategie.
					if (ok2cnt[i][0] > 20 && p0 < 5) { ch2[i][0] = !ch2[i][0]; ok2cnt[i][0] = 0; }
					if (ok2cnt[i][1] > 20 && p1 > 5) { ch2[i][1] = !ch2[i][1]; ok2cnt[i][1] = 0; }

					if (ok2cnt[i][0] < 32) ++ok2cnt[i][0];
					if (ok2cnt[i][1] < 32) ++ok2cnt[i][1];
					ok2[i][0] = (ok2[i][0]<<1)|m0; 
					ok2[i][1] = (ok2[i][1]<<1)|m1; 

					++all[i][0];
					++all[i][1];
				}
			}

			++moves;
			history = (m0|(m1<<1)|(history<<2));
			rates[0] += World[m0][m1][0];
			rates[1] += World[m0][m1][1];
		}

		static int ones_cnt(uint8_t last, uint8_t mask)
		{
			int cnt = 0;
			last &= mask;
			static const uint8_t m[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
			for (size_t i = 0; i < 8; ++i) if (last & m[i]) ++cnt;
			return cnt;
		}
		
		static int zeros_cnt(uint8_t last, uint8_t mask) { return ones_cnt(~last, mask); }

		int last32_rate(int dep, int as) const
		{
			int p = 0;
			
			for (size_t i = 0, j = 1; i < ok2cnt[dep][as]; ++i, j <<= 1)
				p += World[(ok2[dep][0]&j)?1:0][(ok2[dep][1]&j)?1:0][0];
			
			return p;
		}

	private:
		Game(const Game &);
};

uint32_t Game::bmask[13] = { 
	       1,      3,      15,      63,
	     255,   1023,    4095,   16383,
	   65535, 262143, 1048575, 4194303,
	16777215
};

#endif

