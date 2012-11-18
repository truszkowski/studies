#ifndef _INVADERS_TREE_HH_
#define _INVADERS_TREE_HH_

#include "utils.hh"
#include "class.hh"
#include "memory.hh"

/*
 * Drzewo gry, rozwijane wraz z jej przebiegiem.
 */

class Tree {
	public:

		Tree(void)
		{
			bzero(this, sizeof(Tree));
			listcurr = listall;
			verbose = true;
		}
		~Tree(void) { }

		// Przyjecie pamieci zewnetrznej, odpowiedni alokator zajmie sie tym
		void init(void *mem, size_t memsz)
		{
			if (mem) allocator.adopt(mem, memsz);
			if (verbose) fprintf(stderr, "=Tree= Init: Elementow = %u\n", allocator.allCnt());
		}

		// Wylaczenie trybu gadatliwego
		void silent(void)
		{
			verbose = false;
		}

		// Stworzenie drzewa, konkretniej korzenia.
		void create(const Board &board2, int pl) 
		{
			assert(!root);
			
			board = board2;
			player = pl;
			root = allocator.get();
			assert(root);
			Board::Move fake;
			
			// Ustawiamy korzen gry...
			root->set(pl, fake, Board::Score::MinFor(pl), NULL, root, this, listcurr);
			single = false;
		}

		// Aktualizacja drzewa o zapodany ruch
		void update(int pl, const Board::Move &move)
		{
			if (!root) return;

			assert(root && root->player == pl);

			board.move(pl, move);

			if (move == Board::Move::Pass && pl != player) single = true;
			
			Node *it = root->down;
			for (size_t i = 0, n = root->all_cnt; i < n; ++i) {
				Node *next = it->left;
				if (it->move != move) it->remove();
				it = next;
			}

			char s[4], d[4], b[4];

			if (root->all_cnt == 0) {
				if (verbose)
					fprintf(stderr, "=Tree= Update : No path, Player = %d Move = %s %s %s Single = %s\n", pl, 
							Board::conv(move.src, s), Board::conv(move.dst, d), Board::conv(move.bck, b), single?"Yes":"No");
				root = allocator.get();
				assert(root);
				++listcurr;
				depth = 0;
				root->set(!pl, move, Board::Score::MinFor(pl), NULL, root, this, listcurr);
			} else { 
				if (verbose)
					fprintf(stderr, "=Tree= Update : Exists path, Player = %d Move = %s %s %s Single = %s\n", pl, 
							Board::conv(move.src, s), Board::conv(move.dst, d), Board::conv(move.bck, b), single?"Yes":"No");
				// Wybrano jeden i tylko jeden ruch
				assert(root->all_cnt == 1);

				root = root->down;
				root->up->removeOnce();
				root->up = NULL;

				++listcurr;
				--depth;
			}
		}

		// Wybor ruchu
		bool choose(int pl, Board::Move &move, int tmsec)
		{
			(void)pl;
			clock.tic();
			char sstr[4], dstr[4], bstr[4];
			
			// Najpierw liczymy na glebokosc 1...
			Node *best = NULL; 
			
			if (dontsearch) { // Nie szukaj, cale drzewo zostalo przeczesane...
				if (verbose)
					fprintf(stderr, "=Tree= Don't search, Depth = %d Best = %p\n", depth, root->best);
				best = root->best;
			} else {
				Board b = board; // Musimy pracowac na kopii, rzucane wyjatki moga popsuc plansze
				int tm;
				depth = 1;

				best = root->ab(board, depth, -1);
				tm = clock.toc();

				if (verbose)
					fprintf(stderr, "=Tree= First choose : Time = %d.%.3d Best = %p Move = %s %s %s Single = %s\n",
							tm/1000, tm%1000, best, 
							best?Board::conv(best->move.src, sstr):"-", 
							best?Board::conv(best->move.dst, dstr):"-", 
							best?Board::conv(best->move.bck, bstr):"-", single?"Yes":"No");
		
				if (best == NULL) return false; // Nie mozemy wykonac ruchu...

				try {
					// Potem iteracyjnie poglebiamy...
					while (true) {
						cuts = 0; visited = 0;
						best = root->ab(b, depth + 1, tmsec);
						int tm = clock.toc();

						if (verbose)
							fprintf(stderr, "=Tree= Iteration : Depth = %d Time = %d.%.3d Cuts = %u Visited = %u Best = %p Move = %s %s %s Single = %s\n", 
									depth+1, tm/1000, tm%1000, cuts, visited, best, 
									best?Board::conv(best->move.src, sstr):"-", 
									best?Board::conv(best->move.dst, dstr):"-", 
									best?Board::conv(best->move.bck, bstr):"-", single?"Yes":"No");

						// Jak NULL - przeszlismy cale drzewko i nie mamy ruchu juz...
						if (best == NULL) { dontsearch = true; break; }
						// Ostatnia wartwa jest pusta. O ile, to tryb single...
						if (listcurr[depth + 1].all_cnt == 0) { if (single) { dontsearch = true; }; break; }

						++depth;

						if (tmsec < tm) throw Time();
					}
				}
				catch (const Break &e) { if (verbose) fprintf(stderr, "=Tree= Broken : %s\n", e.what()); }
				// Na depth mamy na jaka glebokosc mamy policzona alfabete...
			}

			move = best ? best->move : Board::Move::Pass;
		
			int tm = clock.toc();

			if (verbose) {
				print();
				fprintf(stderr, "=Tree= Choose : Time = %d.%.3d Timeout = %d.%.3d Depth = %d Cuts = %u Visited = %u Best = %p Move = %s %s %s\n", 
						tm/1000, tm%1000, tmsec/1000, tmsec%1000, depth, cuts, visited, best, 
						best?Board::conv(best->move.src, sstr):"-", 
						best?Board::conv(best->move.dst, dstr):"-", 
						best?Board::conv(best->move.bck, bstr):"-");
			}

			return true;
		}

		// Usuniecie calego drzewa
		void remove(void)
		{
			depth = 0;
			root->remove();
			root = NULL;
		}

	private:

		struct Node;

		struct NodeList {
			Node *first;
			size_t all_cnt, exp_cnt;
		} __attribute__((packed));

		struct Node {
			//Board board;    // plansza
			Board::Move move;
			Board::Score score;
			uint8_t player; // kto ma ruch...
			Node *up,       // z czego otrzymana plansza
					 *down,     // do jakich molziwosci prowadzi `this'
					 *right,    // inne mozliwosci otrzymania plansz z `up' (na lewo)
					 *left,     // inne mozliwosci otrzymania plansz z `up' (na prawo)
					 *llevel,   // plansze na lewo bedace na tym samym poziomie od korzenia
					 *rlevel,   // plansze na prawo bedace na tym samym poziomie od korzenia
					 *best;     // droga do najlepszego nodea
			uint8_t flags;  // flagi, mowiace np o tym ze wezel jest rozwiniety lub nie...
			Tree *tree;     // wskaznik na drzewko
			NodeList *list; // na jakiej liscie jestesmy
			uint16_t all_cnt, exp_cnt; // zliczamy

			static const uint8_t Expanded = 0x01;
			static const uint8_t Calculated = 0x02;

			Node(void) : up(NULL), down(NULL), right(NULL), left(NULL), llevel(NULL), rlevel(NULL), flags(0) { }
			~Node(void) { }
			
			void set(uint8_t pl, const Board::Move &m, const Board::Score &s,
					Node *u, Node *first, Tree *t, NodeList *nl)
			{
				move = m;
				score = s;
				player = pl;

				tree = t;
				up = u; 
				down = NULL; 
				best = NULL;
				list = nl;

				// Wrzucamy na liste sytuacji wygenerowanych przez `bo'
				if (first == this) left = right = this;
				else { // -> left <-(-> new <-)-> first <--> right <-
					right = first;
					left = first->left;
					first->left->right = this;
					first->left = this;
				}

				// Wrzucamy na liste sytuacji oddalana o te sama liczbe ruchow od korzenia...
				if (!list->first) list->first = rlevel = llevel = this; // Jeszcze nic nie dodano
				else { // -> llevel <--> first <-(-> new <-)-> rlevel <-
					rlevel = list->first;
					llevel = list->first->llevel;
					list->first->llevel->rlevel = this;
					list->first->llevel = this;
				}

				++list->all_cnt;
				++tree->all_cnt;
				all_cnt = exp_cnt = 0;
				if (up) ++up->all_cnt;

				flags = 0; // puste flagi nic nie robimy
			}

			void calc(Board &b) 
			{
				if (flags & Calculated) return;
				b.calc(score);
				flags += Calculated;
			}

			void expand(Board &bo, int tmsec = -1) 
			{ 
				if (flags & Expanded) return; // Ok, juz przerobione

				if (tree->allocator.freeCnt() < Board::MaxGenMoves) throw Memory();
				(void)tmsec; // Nie sprawdzamy to timeoutu bo to szybka funkcja

				Node *first = NULL;
				++list->exp_cnt;
				++tree->exp_cnt;
				if (up) ++up->exp_cnt;

				flags += Expanded;
				bool expanded = false;

				if (tree->single && player != tree->player) {
					// Jesli mamy etap gry kiedy gracze sa rozdzieleni,
					// powijamy ruchy przeciwnika. 

					Node *next = tree->allocator.get();
					assert(next);
					down = next;
					next->set(!player, Board::Move::Pass, Board::Score::MinFor(player),
							this, next, tree, list+1);
					return;
				}

				for (size_t p = 0; p < bo.pawnc[player]; ++p) {
					Board::Coord s = bo.pawns[player][p].c;
					for (size_t dn = 0; dn < 8; ++dn) {
						Board::Coord d = bo.nthNextCoord(s, dn);
						if (d.good() && bo[d].isEmpty()) {
							for (size_t bn = 0; bn < 8; ++bn) {
								Board::Coord b = bo.nthNextCoord(d, s, bn);
								if (b.good() && (bo[b].isEmpty() || b == s)) {
									// Bierzemy wezel ze specjalnego alokatora
									Node *next = tree->allocator.get();
									expanded = true;
									
									// Ustawiamy wskaznik down na jedna z mozliwosci w dol...
									if (!first) { first = next; down = next; }

									// Tworzymy krok w dol. SingleMode rozni sie od zwyklego trybu tylko tym
									// ze za kazdym krokiem rozpatrujemy tylko swoje ruchy...
									next->set(!player, Board::Move(s, d, b), Board::Score::MinFor(player), 
											this, first, tree, list+1); // +1 nast el w list
								}
							}
						}
					}
					
					// Jesli jest tryb single to nie ma znaczenia kolejnosc ruszania sie pionami
					// mozna zatem tutaj zaoszczedzic liczbe generowanych mozliwosci przez wybor 
					// do ruchu pionka o najnizszym id. 
					if (tree->single && expanded) return;
				}
			}
			
			void remove(void) 
			{ 
				// Usuwamy element z list...
				// Trzeba uwazac przy iteracjach list podczas ktorych usuwamy elementy!
	
				// tniemy tez elementy pod down :)
				Node *it = down;

				for (size_t i = 0, n = all_cnt; i < n; ++i) {
					Node *next = it->left;
					it->remove();
					it = next;
				} 

				// Teraz dopiero usuwamy aktualny wezel
				removeOnce();
			}

			void removeOnce(void) 
			{
				// O-o, z gory jest do nas dowiazanie, zmienmy to!
				if (up && up->down == this) {
					if (left == this) { up->down = NULL; }
					else { up->down = left; }
				}
				// wyjmuje element
				if (left != this) {
					left->right = right;
					right->left = left;
				} 
				
				// O-o, jestem firstem na liscie, zmienmy to!
				if (list->first == this) {
					if (llevel == this) { list->first = NULL; }
					else { list->first = llevel; }
				}
				// wyjmuje element
				if (llevel != this) {
					llevel->rlevel = rlevel;
					rlevel->llevel = llevel;
				}
				
				if (flags & Expanded) { --list->exp_cnt; --tree->exp_cnt; if (up) --up->exp_cnt; }
				--list->all_cnt; --tree->all_cnt; if (up) --up->all_cnt;
				flags = 0;

				// NULL-uje zeby potem, gdyby sie bug pojawil, nie mazalo po pamieci
				// ale rzucilo segfault albo cos...
				list = NULL;
				left = right = NULL;
				llevel = rlevel = NULL;
				up = down = NULL;

				// Zwracamy element alokatorowi do puli elementow...
				tree->allocator.put(this);
				// ...od teraz nie mozemy juz korzystac z node'a
			}

			Node *ab(Board &bo, size_t depth, int tmsec)
			{
				// Sprawdzamy moment stopu przed modyfikacja besta, aby best zawsze jakis byl
				if (tmsec > 0 && tmsec < tree->clock.toc()) throw Time();
				
				++tree->visited;

				if (depth == 0) { calc(bo); return NULL; }
				
				expand(bo, tmsec);
				Node *prevbest = best;
				best = NULL;

				assert(flags & Expanded);
				Node *it = down;
				score = Board::Score::MinFor(player);

				// Brak wezlow w dol - brak dozwolonruchu
				if (it == NULL) return NULL;
				
				// Patrzymy na poprzedni best, dzieki temu zawsze mamy jakiegos besta ustawionego
				// poza przypadkiem, ze nie mozemy wykonac zadnego ruchu.
				// Warto zauwazyc, ze best bedzie sie zmieniac z biegiem gry, wraz z kazdym zaglebieniem.
				if (prevbest != NULL) { 
					bo.move(player, prevbest->move);
					Node *chosen = prevbest->ab(bo, depth-1, tmsec);
					bo.undo(player, prevbest->move);

					if (!chosen && depth > 1) {
						score = Board::Score::MaxFor(player); best = prevbest; return best;
					}

					score = prevbest->score;
					best = prevbest;
				}

				// Patrzymy na inne galezie, moze bedzie cos lepszego
				for (size_t i = 0, n = all_cnt; i < n; ++i) {
					if (prevbest != it) { // Zeby nie odwiedzac x2 besta
						bo.move(player, it->move);
						Node *chosen = it->ab(bo, depth-1, tmsec);
						bo.undo(player, it->move);

						if (!chosen && depth > 1) {
							score = Board::Score::MaxFor(player); best = it; return best;
						}

						if (it->score.greater(player, score)) {
							score = it->score; best = it;
						}

						if (best && this != tree->root && up && // Ciecie
								!up->score.greater(player, score)) { 
							++tree->cuts; return best; 
						}
					}
					it = it->right;
				}

				// Jesli wszyscy odpowiedzieli MinVal i best nie byl zdefiniowany...
				if (best == NULL) { 
					if (tree->verbose)
						fprintf(stderr, "=Tree= Alfabeta : AllCnt = %u PrevBest = %p Score = %u, %u\n", 
								all_cnt, prevbest, score.value[0], score.value[1]);
					best = down;
				}

				return best;
			}

		} __attribute__((packed));

		// Max to 19 * 19 bo nie bedzie wiecej ruchow niz pol na planszy :)
		static const size_t MaxDepth = Board::MaxMoves; 
		// Jak wiele pol moze rozwijac heureza...
		static const uint8_t SingleMode = 0x01;
		static const uint8_t Calculated = 0x02;

		typedef Alloc<Node, 1000> TAlloc;
		TAlloc allocator;
		Node *root;
		NodeList *listcurr;
		NodeList listall[MaxDepth];
		size_t all_cnt, exp_cnt;
		size_t depth;
		size_t cuts, visited;
		Clock clock;
		Board board;
		int player;
		bool single;
		bool dontsearch;
		bool verbose;

		void print(void) const 
		{
			fprintf(stderr, "=Tree= Informacje      : search=%s single=%s\n", dontsearch?"No":"Yes", single?"Yes":"No");
			fprintf(stderr, "=Tree= Alokator        : wolnych=%u uzytych=%u wszystkich=%u\n", allocator.freeCnt(), allocator.inUseCnt(), allocator.allCnt());
			fprintf(stderr, "=Tree= Wezly           : uzytych=%u rozwinietych=%u glebokosc=%u\n", all_cnt, exp_cnt, depth);
			for (size_t i = 0; listcurr[i].all_cnt > 0; ++i)
				fprintf(stderr, "=Tree= Glebokosc %u     : uzytych=%u rozwinietych=%u\n", 
						i, listcurr[i].all_cnt, listcurr[i].exp_cnt);			
		}

		// Wyjatki reprezentujace przerwanie przeliczania 

		class Break : public std::exception {
			public:
				Break(void) throw() { }
				const char *what(void) const throw() { return "Tree::Break"; }
		};
		class Time : public Break {
			public:
				Time(void) throw() { }
				const char *what(void) const throw() { return "Tree::Time"; }
		};
		class Memory : public Break {
			public:
				Memory(void) throw() { }
				const char *what(void) const throw() { return "Tree::Memory"; }
		};

};


#endif

