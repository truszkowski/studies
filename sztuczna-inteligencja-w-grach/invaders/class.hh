#ifndef _INVADERS_CLASS_HH_
#define _INVADERS_CLASS_HH_

// Tutaj definiujemy punkty przyznawane 
// funkcji oceniajacej, zeby dalo sie wzglednie
// latwo zmieniac.

#ifndef SC_EQ_0
#define SC_EQ_0 2
#endif

#ifndef SC_EQ_X
#define SC_EQ_X 10
#endif

#ifndef SC_EQ_1_HIGH
#define SC_EQ_1_HIGH  9
#endif

#ifndef SC_EQ_1_LOW
#define SC_EQ_1_LOW   1
#endif

#ifndef SC_EQ_2_HIGH
#define SC_EQ_2_HIGH 10
#endif

#ifndef SC_EQ_2_LOW
#define SC_EQ_2_LOW   0
#endif

#include "utils.hh"

class Board {
	public:
		static const uint8_t MaxSize = 19;       // Maksymalny rozmiar planszy
		static const uint8_t MaxPawns = 5;       // Maksymalna liczba pionkow
		static uint8_t Size/* = 19*/;            // Aktualny rozmiar planszy
		static uint8_t Pawns/* = 5*/;            // Aktualna liczba pionkow

		static const char BlockedField = '#';    // Znak zablokowanego pola 
		static const char EmptyField = '.';      // Znak pustego pola

		static const uint8_t NoScore = 255;      // Nie przydzielone punkty (tutaj odleglosc od pionka)
		static const uint8_t NoPawn = MaxPawns;  // Nie przydzielony pionek 

		static const size_t MaxGenMoves = MaxPawns*8*8 + 1;     // Maksymalna liczba ruchow pionkami
		static const size_t MaxGenPlaces = MaxSize*MaxSize + 1; // Maksymalna liczba postawien pionka
		static const size_t MaxMoves = 2*MaxSize*MaxSize + 1;   // Maksymalna liczba ruchow w partii
	
		static const int16_t MaxScore = MaxSize*MaxSize;  // Maksymalna wartosc oceny
		static const int16_t MinScore = 0;                // Minimalna wartosc oceny

		struct Field { // Pole
			char c;
			
			Field(void) : c(EmptyField) { }
			~Field(void) { }
			
			bool isBlocked(void) const { return c == BlockedField; }
			bool isEmpty(void) const { return c == EmptyField; }
			bool isPlayer(int pl) const { return c >= pl?'1':'a' && c <= (pl?'1':'a')+Pawns; }
		} __attribute__((packed));

		struct Coord { // Koordynaty
			uint8_t i, j;

			Coord(void) : i(0), j(0) { }
			Coord(const Coord &c) : i(c.i), j(c.j) { }
			Coord(uint8_t i, uint8_t j) : i(i), j(j) { }
			~Coord(void) { }
			
			bool good(void) const { return i < Size && j < Size; }
			bool bad(void) const { return i >= Size || j >= Size; }

			bool operator==(const Coord &c) const { return i == c.i && j == c.j; }
			bool operator!=(const Coord &c) const { return i != c.i || j != c.j; }
		} __attribute__((packed));

		struct Pawn { // Pionek
			Coord c;

			Pawn(void) : c() { }
			~Pawn(void) { }
		} __attribute__((packed));

		struct Score { // Ocena
			static const int16_t Max =  16383; // - Trzeba uwazac aby wartosci
			static const int16_t Min = -16383; // - sie nie przekrecily...
			int16_t value[2]; // Na ocene skladaja sie wplywy graczy

			static Score MaxFor(int player) { Score s; s.value[player] = Max; s.value[!player] = Min; return s; }
			static Score MinFor(int player) { Score s; s.value[player] = Min; s.value[!player] = Max; return s; }

			Score(void) { value[0] = value[1] = 0; }
			~Score(void) { }

			bool greater(int pl, const Score &s) const
			{
				return value[pl] + s.value[!pl] > value[!pl] + s.value[pl];	
			}
			bool equal(int pl, const Score &s) const
			{
				return value[pl] + s.value[!pl] == value[!pl] + s.value[pl];	
			}

		} __attribute__((packed));

		struct Move { // Ruch
			Coord src, dst, bck;

			Move(void) { }
			Move(const Coord &s, const Coord &d, const Coord &b) : src(s), dst(d), bck(b) { }
			~Move(void) { }

			void set(const Coord &s, const Coord &d, const Coord &b) 
			{
				src = s; dst = d; bck = b;
			}
			bool operator==(const Move &m) const 
			{
				return bck == m.bck && dst == m.dst && src == m.src;
			}
			bool operator!=(const Move &m) const 
			{
				return bck != m.bck || dst != m.dst || src != m.src;
			}

			static const Move Pass;//(Coord(MaxSize, MaxSize), Coord(MaxSize, MaxSize), Coord(MaxSize, MaxSize));

			bool isPass(void) const { return (*this) == Pass; }

		} __attribute__((packed));

		class View { // Oglad planszy
			public:
				// Rozszerzone informacje o polu
				struct Field { 
					char c;
					uint8_t count[2]; // - jak szybko tu dojde
					uint8_t owner[2]; // - ktory pionek ma najblizej

					Field(void) { set(EmptyField); }
					~Field(void) { }

					void set(char cc) { c = cc; count[0] = count[1] = NoScore; owner[0] = owner[1] = NoPawn; }
					bool isBlocked(void) const { return c == BlockedField; }
					bool isEmpty(void) const { return c == EmptyField; }
				};

				// Rozszerzone informacje o pionku
				struct Pawn {
					Coord c;
					uint8_t flags;
					uint16_t count;
	
					Pawn(void) : flags(0), count(0) { }
					~Pawn(void) { }

					void set(const Coord &cc) { c = cc; flags = 0; count = 0; }
				};

				Field fields[MaxSize][MaxSize];
				Pawn pawns[2][MaxPawns];
				Score score;
				uint8_t pawnc[2];

				View(void) { }
				~View(void) { }
	
				Field &operator[](const Coord &c) 
				{
					assert(c.i < Size && c.j < Size);
					return fields[c.i][c.j];
				}

				const Field &operator[](const Coord &c) const
				{
					assert(c.i < Size && c.j < Size);
					return fields[c.i][c.j];
				}

				Field &getField(uint8_t i, uint8_t j) 
				{
					assert(i < Size && j < Size);
					return fields[i][j];
				}

				const Field &getField(uint8_t i, uint8_t j) const
				{
					assert(i < Size && j < Size);
					return fields[i][j];
				}

				static int16_t F(uint8_t me, uint8_t he)
				{
					if (me == 0) return 10;
					uint16_t p = (((int16_t)he)-((int16_t)me)) * ((int16_t)he)/((int16_t)me);
					return p > 10 ? 10 : p;
				}

				void calc(void) 
				{
					// Liczymy rankingi, ogolem oraz osobno dla kazdego pionka
					// Trzeba uwazac na score, Max = 16838, Min = -16383 !!! 
					score.value[0] = score.value[1] = 0;
					for (size_t i = 0; i < Size; ++i) {
						for (size_t j = 0; j < Size; ++j) {
							if (!fields[i][j].isBlocked()) {
								switch (((int)fields[i][j].count[0]) - ((int)fields[i][j].count[1])) {
									case -2:
										score.value[0] += SC_EQ_2_HIGH;
										score.value[1] += SC_EQ_2_LOW;
										break;
									case 2:
										score.value[0] += SC_EQ_2_LOW;
										score.value[1] += SC_EQ_2_HIGH;
										break;
									case -1:
										score.value[0] += SC_EQ_1_HIGH;
										score.value[1] += SC_EQ_1_LOW; 
										break;
									case 1:
										score.value[0] += SC_EQ_1_LOW;   
										score.value[1] += SC_EQ_1_HIGH; 
										break;
									case 0:
										score.value[0] += SC_EQ_0;       
										score.value[1] += SC_EQ_0; 
										break;
									default:
										if (fields[i][j].count[0] < fields[i][j].count[1]) 
											score.value[0] += SC_EQ_X; 
										else 
											score.value[1] += SC_EQ_X;
								}
							}
						}
					}
				}
				void calc2(void)
				{
					// Liczymy, ale troche inaczej, teraz wazne tez aby jak najszybciej
					// dojsc w dowolne miejsce planszy...
					for (size_t i = 0; i < Size; ++i) {
						for (size_t j = 0; j < Size; ++j) {
							if (!fields[i][j].isBlocked()) {
								score.value[0] += fields[i][j].count[0] < (Board::Size/3) ? ((Board::Size/3)-fields[i][j].count[0]) : 1;
								score.value[1] += fields[i][j].count[1] < (Board::Size/3) ? ((Board::Size/3)-fields[i][j].count[1]) : 1;
							}
						}
					}
				}
		};

		Field fields[MaxSize][MaxSize];
		Pawn pawns[2][MaxPawns];
		uint8_t pawnc[2];

		// Definiujemy tablice przesuniec:
		//
		// | (-1,-1) (-1, 0) (-1. 1) |     |  4   5   6  |     |  0   3   6  |
		// | ( 0,-1) ( i, j) ( 0, 1) |  =  |  3   x   7  |  =  |  1   x   7  |
		// | ( 1,-1) ( 1, 0) ( 1, 1) |     |  2   1   0  |     |  2   5   8  |
		//    ^ wg wspolrzednych           ^ odpowiadajacy     ^ odpowiadajacy index 
		//        na planszy               index w tablicy
		//

		static const int8_t inexts[16]; // = { 1, 1,  1,  0, -1, -1, -1, 0,  1, 1,  1,  0, -1, -1, -1, 0 };
		static const int8_t jnexts[16]; // = { 1, 0, -1, -1, -1,  0,  1, 1,  1, 0, -1, -1, -1,  0,  1, 1 };
		static const int8_t ijmove[9];  // = { 4, 3, 2, 5, 0, 1, 6, 7, 0 };

		Coord nthNextCoord(uint8_t i, uint8_t j, size_t nth) const
		{
			// Generator n-tego sasiedniego pola dla pola (i,j)
			return Coord(i+inexts[nth], j+jnexts[nth]);
		}

		Coord nthNextCoord(const Coord &c, size_t nth) const 
		{
			return nthNextCoord(c.i, c.j, nth);
		}

		Coord nthNextCoord(uint8_t i, uint8_t j, uint8_t bi, uint8_t bj, size_t nth) const
		{
			int8_t mv = 1 + bi - i + 3*(1 + bj - j); // f(x,y) = (1+x) + 3*(1+y)
			// (bi-i, bj-j) = ( -1,0,+1 ; -1,0,+1 ) fn(x,y) = 1 + x + 3*(1 + y)
			return Coord(i+inexts[nth+ijmove[mv]],j+jnexts[nth+ijmove[mv]]);
		}

		Coord nthNextCoord(const Coord &c, const Coord &first, size_t nth) const 
		{
			return nthNextCoord(c.i, c.j, first.i, first.j, nth);
		}

		Field &operator[](const Coord &c) 
		{
			assert(c.i < Size && c.j < Size);
			return fields[c.i][c.j];
		}

		const Field &operator[](const Coord &c) const
		{
			assert(c.i < Size && c.j < Size);
			return fields[c.i][c.j];
		}

		Field &getField(uint8_t i, uint8_t j) 
		{
			assert(i < Size && j < Size);
			return fields[i][j];
		}

		const Field &getField(uint8_t i, uint8_t j) const
		{
			assert(i < Size && j < Size);
			return fields[i][j];
		}
		
		void blockField(const Coord &c)
		{
			blockField(c.i, c.j);
		}

		void blockField(size_t i, size_t j)
		{
			assert(i < Size && j < Size);
			fields[i][j].c = BlockedField;
		}

		// Umieszczenie pionka
		void place(int pl, const Coord &dst)
		{
			assert(checkPlace(pl, dst));
			pawns[pl][pawnc[pl]].c = dst;
			fields[dst.i][dst.j].c = (pl?'1':'a') + pawnc[pl];
			++pawnc[pl];
		}

		// Cofniecie umieszczenia pionka
		void undo(int pl, const Coord &dst)
		{
			assert(fields[dst.i][dst.j].isPlayer(pl));
			assert(pawns[pl][pawnc[pl]-1].c == dst);
			fields[dst.i][dst.j].c = EmptyField;
			--pawnc[pl];
		}

		// Wykonanie ruchu
		void move(int pl, const Move &m)
		{
			if (m.isPass()) return;

			assert(checkMove(pl, m));
			// Przesuwamy pionka...
			fields[m.dst.i][m.dst.j].c = fields[m.src.i][m.src.j].c;
			fields[m.src.i][m.src.j].c = EmptyField;
			
			// Sprawdzamy czy napewno zablokujemy puste pole
			assert(fields[m.bck.i][m.bck.j].c == EmptyField);
			
			// Blokujemy
			blockField(m.bck.i, m.bck.j);

			// Na koniec zmieniamy pozycje pionka w tablicy pionkow
			for (size_t i = 0; i < Pawns; ++i)
				if (m.src == pawns[pl][i].c) { pawns[pl][i].c = m.dst; return; }

			throw invaders_error("Ej, nie mam takiego pionka w tablicy : i,j = %u,%u", m.src.i, m.src.j);
		}

		// Cofniecie wykonania ruchu
		void undo(int pl, const Move &m)
		{
			if (m.isPass()) return;

			// Sprawdzamy...
			assert(m.src.good() && m.dst.good() && m.bck.good());
			assert(fields[m.bck.i][m.bck.j].isBlocked() && fields[m.dst.i][m.dst.j].isPlayer(pl));
			assert(m.src == m.bck || !fields[m.src.i][m.src.j].isBlocked());

			// Przywracamy pozycje
			fields[m.bck.i][m.bck.j].c = EmptyField;
			fields[m.src.i][m.src.j].c = fields[m.dst.i][m.dst.j].c;
			fields[m.dst.i][m.dst.j].c = EmptyField;
			
			// Na koniec zmieniamy pozycje pionka w tablicy pionkow
			for (size_t i = 0; i < Pawns; ++i)
				if (m.dst == pawns[pl][i].c) { pawns[pl][i].c = m.src; return; }

			throw invaders_error("Ej, nie mam takiego pionka w tablicy : i,k = %u,%u", m.src.i, m.src.j);
		}

		// Rzucenie okiem na plansze i wyrobienie sobie o niej ogladu...
		void look(View &view, bool findBridges = false) const
		{
			bool bfsmarked[Size][Size]; // ktore pola odwiedzil bfs
			Coord queue[Size*Size];     // kolejka bfsa
			uint8_t count[Size*Size];   // poziom bfsa
			uint8_t owner[Size*Size];   // wlasciciel

			// Graczowi !as nie zmienila sie pozycja wiec lookamy bfsem tylko dla as

			// Czyscimy struktury...
			for (size_t i = 0; i < Size; ++i)
				for (size_t j = 0; j < Size; ++j)
					view.fields[i][j].set(fields[i][j].c);

			for (int pl = 0; pl < 2; ++pl) {
				for (size_t i = 0; i < Size; ++i)
					for (size_t j = 0; j < Size; ++j)
						owner[i*Size+j] = NoPawn;

				// Bedziemy dokonywac podzialu terytorium bsfem
				size_t queue_bgn = 0, queue_end = 0;

				// Startujemy bfsem od pozycji pionkow
				for (size_t i = 0; i < pawnc[pl]; ++i) {
					queue[queue_end] = pawns[pl][i].c;
					count[queue_end] = 0;
					owner[queue_end] = i;
					++queue_end;
				}

				// Zmarkujmy, aby nie odwiedzac pol zablokowanych
				for (size_t i = 0; i < Size; ++i)
					for (size_t j = 0; j < Size; ++j) 
						bfsmarked[i][j] = fields[i][j].isBlocked();

				// Dopoki cos jest do odwiedzenia...
				while (queue_bgn < queue_end) {
					for (size_t i = 0; i < 8; ++i) {
						Coord c = nthNextCoord(queue[queue_bgn], i);
						// Jesli tylko wspolrzedne sa ok i pole jest
						// niezmarkowane to je odwiedzamy...
						if (c.good() && !bfsmarked[c.i][c.j]) {
							bfsmarked[c.i][c.j] = true;
							view.fields[c.i][c.j].count[pl] = count[queue_bgn];
							view.fields[c.i][c.j].owner[pl] = owner[queue_bgn];
							queue[queue_end] = c;
							count[queue_end] = count[queue_bgn]+1;
							owner[queue_end] = owner[queue_bgn];
							++queue_end; // Dokladamy na kolejke
						}
					}
					++queue_bgn; // Zdejmujemy z kolejki
				}
			}
			
			view.score.value[0] = 0;
			view.score.value[1] = 0;

			for (int pl = 0; pl < 2; ++pl)
				for (size_t i = 0; i < pawnc[pl]; ++i) 
					view.pawns[pl][i].set(pawns[pl][i].c);

			if (findBridges) { }
		}

		Board(void) 
		{
			bzero(this, sizeof(Board));
		}

		Board(const Board &b)
		{
			memcpy(this, &b, sizeof(Board));
		}

		~Board(void) { }

		// Ustawienie rozmiaru planszy i maksymalnej liczby pionkow
		static void set(uint8_t size, uint8_t pawns) 
		{
			assert(size > 0 && size <= MaxSize && pawns > 0 && pawns <= MaxPawns);
			Size = size;
			Pawns = pawns;
		}

		// Ustawienie opisu planszy
		void set(const char *buf)
		{
			bzero(this, sizeof(Board));

			// Ustawiamy odpowiednie pola...
			for (size_t i = 0; i < Size; ++i) {
				for (size_t j = 0; j < Size; ++j) {
					fields[i][j].c = buf[i*(Size+1)+j];
					assert(fields[i][j].c == EmptyField || fields[i][j].c == BlockedField);
				}
				assert(buf[i*(Size+1) + Size] == '\n');
			}
		}

		// Wydruk planszy
		void print(void) const;

		/*** Rozne takie konwersje... ***/

		static const char *conv(const Coord &c) 
		{
			static char cstr[4];
			return conv(c, cstr);
		}
		
		static const char *conv(const Coord &c, char *cstr)
		{
			if (c.i < Size || c.j < Size) {
				int sn = snprintf(cstr, 4, "%c%u", 'a'+c.j, Size - c.i);
				if (sn < 0 || sn >= 4) throw invaders_error("konwersja");
			} else snprintf(cstr, 4, "pas");
			return cstr;
		}

		static Coord conv(const char *cstr)
		{
			return Coord(Size-atoi(cstr+1), cstr[0]-'a');
		}

		static Coord dir(const Coord &c, const char *d)
		{
			static const char *dstr[8] = { "n", "ne", "e", "se", "s", "sw", "w", "nw" };
			static int8_t is[8] = { -1, -1, 0, 1, 1,  1,  0, -1 };
			static int8_t js[8] = {  0,  1, 1, 1, 0, -1, -1, -1 };
			
			for (size_t n = 0; n < 8; ++n) 
				if (strcmp(d, dstr[n]) == 0) return Coord(c.i+is[n], c.j+js[n]);

			return Coord(Size, Size);
		}

		static const char *dir(const Coord &src, const Coord &dst)
		{
			static const char *dstr[8] = { "n", "ne", "e", "se", "s", "sw", "w", "nw" };
			static int8_t is[8] = { -1, -1, 0, 1, 1,  1,  0, -1 };
			static int8_t js[8] = {  0,  1, 1, 1, 0, -1, -1, -1 };

			for (size_t n = 0; n < 8; ++n) 
				if (src.i+is[n] == dst.i && src.j+js[n] == dst.j) return dstr[n];

			return NULL;
		}

		/*** Koniec funkcji do konwersji... ***/

		// Sprawdz czy taki ruch jest poprawny
		bool checkPlace(int pl, const Coord &dst) const
		{
			if (!dst.good()) return false;
			if (fields[dst.i][dst.j].c != EmptyField) return false;
			if (pawnc[pl] >= Pawns) return false;

			for (size_t p = 0; p < 2; ++p)
				for (size_t i = 0; i < pawnc[p]; ++i) 
					if (dst == pawns[p][i].c) return false;

			return true;	
		}

		// Sprawdz czy taki ruch jest poprawny
		bool checkMove(int pl, const Move &m) const
		{
			if (!m.src.good() || !m.dst.good() || !m.bck.good()) return false;
			if (fields[m.src.i][m.src.j].c < (pl?'1':'a') || fields[m.src.i][m.src.j].c > (pl?'1':'a')+Pawns) return false;
			if (fields[m.dst.i][m.dst.j].c != EmptyField) return false;

			{
				bool correct = false;
				for (size_t n = 0; n < 8; ++n)
					if (nthNextCoord(m.src, n) == m.dst) { correct = true; break; }
				if (!correct) return false;
				correct = false;
				for (size_t n = 0; n < 8; ++n)
					if (nthNextCoord(m.src, n) == m.dst) { correct = true; break; }
				if (!correct) return false;
			}
			
			for (size_t i = 0; i < Pawns; ++i)
				if (m.src == pawns[pl][i].c) return true;

			return false;
		}

		// Czy to jest gra koncowa(piony graczy rozdzielone)?
		bool isEndGame(void) const 
		{
			View view;
			look(view);

			for (size_t i = 0; i < pawnc[0]; ++i)
				if (view[pawns[0][i].c].owner[1] != NoPawn) return false;

			return true;
		}

		// Funkcja oceny planszy
		void calc(Score &score) const 
		{
			View view;
			look(view);
			view.calc();
			score = view.score;
		}

		void calc2(Score &score) const
		{
			View view;
			look(view);
			view.calc2();
			score = view.score;
		}

} __attribute__((packed));


// Funkcja wybierajaca rozstawienie pionow
bool choosePlaceFor(const Board &b, int pl, Board::Coord &dst, int tmsec = 0);


// Funkcja obliczajaca ile potrzeba czasu na ruch, jesli na cala partie jest tmsec
int calcTimeoutFor(const Board &b, int pl, int tmsec);

#endif

