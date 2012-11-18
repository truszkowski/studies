#ifndef __SIG_GAME_HH__
#define __SIG_GAME_HH__

#include <cassert>

#define MaxGames 1000
#define MaxBluff 49
#define MaxDepth 60
#define Pass (-1)
#define Call (-2)

class Game {
	public:
		Game(void) { bzero(this, sizeof(Game)); }
		~Game(void) { }

		int player, money, card[2], bet[2], rate[2];
		bool ending, mypass, oppass, mymove, opmove;

		int history[MaxDepth], depth, random_mode;
		int bluffs[MaxDepth], zeros[MaxDepth], ones[MaxDepth];

		int cnt_pass[MaxDepth][2];
		int cnt_call[MaxDepth][2];
		int cnt_raiseto[MaxDepth][2];

		void set(int bind, int stack, int cd)
		{
			assert(bind == 1 || bind == 2);
			assert(cd == 0 || cd == 1);

			player = bind - 1;
			money = stack;
			card[0] = cd;
			card[1] = 2;
			bet[0] = bind;
			bet[1] = bind == 2 ? 1:2;
			ending = false;
			mypass = false;
			oppass = false;
			mymove = false;
			opmove = false;

			history[0] = bet[0];
			depth = 1;
		}

		bool opbluff(void) const 
		{
//			if (ones[history[depth-1]] > 2*zeros[history[depth-1]]) return false;
//			if (ones[bet[1]+1] > 2*zeros[history[depth-1]]) return false;

			if (2*bluffs[history[depth-1]] > zeros[history[depth-1]]) return true;
			return false; //10*bluffs[bet[1]+1] > 6*zeros[history[depth-1]];
		}

		int make_bluff(void) 
		{
			// Przy stanie 1:2 blefujemy zawsze...
			if (bet[0] == 1) return bet[1]+1;
			
			// Za wysoka stawka...
			if (bet[1]+1 > MaxBluff) return bet[0]+1 < bet[1] ? Pass : Call;
	
			// Jesli zaczynam ostro przegrywac, gram losowo...
			if ((rate[0] + 100 < rate[1] || (random_mode % 10) > 0) && rate[0] - 50 < rate[1]) {
				++random_mode;
				if (bet[1]+1 < 15 && (rand() % 4) == 0) return bet[1]+1;
				return bet[0]+1 < bet[1] ? Pass : Call;
			} else random_mode = 0;
		
			// Czy jest prawdopodobne, ze przeciwnik blefowal przy poprzednim ruchu? 
			if (opbluff()) return bet[1]+1;

			return bet[0]+1 < bet[1] ? Pass : Call;
		}

		int genmove(int bet0, int bet1)
		{
			assert(bet0 <= bet1 && bet0 <= money && bet1 <= money);
			assert(bet0 == bet[0] && bet1 == bet[1]);

			if (card[0] == 0) { // Blefujeeemy :)
				int mv = make_bluff();

				if (mv > 0) {
					bet[0] = mv;
					history[depth++] = bet[0];
					return bet[0];
				} else if (mv == Pass) {
					ending = true;
					mypass = true;
					return Pass;
				} else { // Call
					ending = true;
					bet[0] = bet[1];
					return Call;
				}
			} else if (bet[1] == money || (rand() % 5) == 0) {
				ending = true;
				bet[0] = money;
				return Call;
			} else {
				bet[0] = bet[1]+1;
				
				// Troche losowosci...
				if (rand() % 10 > 0) { }
				else if (rand() % 10 > 0) bet[0] += 1;
				else bet[0] += 2;
				
				if (bet[0] > money) bet[0] = money;
				history[depth++] = bet[0];
				return bet[0];
			}
		}

		void oppmove(int mv)
		{
			switch (mv) {
				case Pass:
					ending = true;
					oppass = true;
					break;
				case Call:
					ending = true;
					bet[1] = bet[0];
					break;
				default:
					assert(mv > 0 && mv <= money);
					bet[1] = mv;
			}
		}

		void showdown(int pl, int cd)
		{
			assert(ending);

			if (player == pl) assert(cd == card[0]);
			else {
				assert(cd == 0 || cd == 1);
				card[1] = cd;
			
				if (mypass) {
					rate[0] -= bet[0];
					rate[1] += bet[0];
				} else if (oppass) {
					rate[0] += bet[1];
					rate[1] -= bet[1];
				} else if (card[0] > card[1]) {
					rate[0] += bet[1];
					rate[1] -= bet[1];
				} else if (card[0] > card[1]) {
					rate[0] -= bet[0];
					rate[1] += bet[0];
				}

				if (cd == 0) {
					for (int i = 0; i < depth-1; ++i) {
						++bluffs[history[i]];
						++zeros[history[i]];
					}

					++zeros[history[depth-1]];
					if (!oppass) ++bluffs[history[depth-1]];

				} else {
					for (int i = 0; i < depth; ++i) {
						++ones[history[i]];
					}
				}

				//fprintf(stderr, "wynik = %d:%d, tab = [", rate[0], rate[1]);
				//for (int i = 0; i < MaxDepth; ++i) 
				//	if (zeros[i]) fprintf(stderr, " %d:%d/%d/%d", i, bluffs[i], zeros[i], ones[i]);
				//fprintf(stderr, " ]\n");

			}
		}

};

#endif
