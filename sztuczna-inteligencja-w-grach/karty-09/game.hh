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
		bool ending, mypass, oppass;

		void set(int bind, int stack, int cd)
		{
			assert(bind == 1 || bind == 2);
			assert(cd >= 0 && cd <= 9);

			player = bind - 1;
			money = stack;
			card[0] = cd;
			card[1] = 2;
			bet[0] = bind;
			bet[1] = bind == 2 ? 1:2;
			ending = false;
			mypass = false;
			oppass = false;
		}

		int genmove(int bet0, int bet1)
		{
			assert(bet0 <= bet1 && bet0 <= money && bet1 <= money);
			assert(bet0 == bet[0] && bet1 == bet[1]);

			if (card[0] == 0) { ending = true; mypass = true; return Pass; }
			if (card[0] == 9) {
				if (bet[1] == money) {
					bet[0] = bet[1];
					ending = true;
					return Call;
				} 
				bet[0] = bet[1]+1;
				return bet[0];
			}
			
			if ((rand() % 10) < card[0]) {
				if (bet[1] == money) {
					bet[0] = bet[1];
					ending = true;
					return Call;
				}
				bet[0] = bet[1]+1;
				return bet[0];
			}

			ending = true;

			if (rand() % 2 == 0 || bet[0]+1 < bet[1]) {
				mypass = true;
				return Pass;
			}

			bet[0] = bet[1];
			return Call;
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
				assert(cd >= 0 && cd <= 9);
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
			}
		}

};

#endif
