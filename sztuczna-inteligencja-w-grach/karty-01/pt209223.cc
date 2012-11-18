#include <ctime>
#include <sys/time.h>
#include <signal.h>

#include "gtp.hh"
#include "game.hh"

Game game;

int main(void)
{
	char cmd[32], player[32], move[32];
	int stack, card, mybet, opbet, mv, left;

	timeval tv;
	gettimeofday(&tv, 0);
	srand(tv.tv_usec);

	signal(SIGPIPE, SIG_IGN);
	
	while (1) {
		if (gtp_scanf("%s", cmd)) break; 

		if (strcmp(cmd, "name") == 0) { if (gtp_printf("= pt209223\n\n")) break; } 
		else if (strcmp(cmd, "author") == 0) { if (gtp_printf("= pt209223\n\n")) break; } 
		else if (strcmp(cmd, "quit") == 0) { gtp_printf("= \n\n"); break; } 
		else if (strcmp(cmd, "time_left") == 0) { gtp_scanf("%d", &left); gtp_printf("= \n\n"); } 
		else if (strcmp(cmd, "youare") == 0) { 
			// youare <player=SB|BB> <stack=50> <card=0|1>
			if (gtp_scanf("%s%d%d", player, &stack, &card)) break;
			gtp_printf("= \n\n");

			if (strcmp(player, "SB") == 0) game.set(1, stack, card);
			else if (strcmp(player, "BB") == 0) game.set(2, stack, card);
			else {
				fprintf(stderr, "Nierozpoznany gracz : %s\n", player);
				break;
			}

		} else if (strcmp(cmd, "genmove") == 0) {
			// genmove <mybet=1|2|...> <opbet=1|2|...>
			if (gtp_scanf("%d%d", &mybet, &opbet)) break;

			switch (mv = game.genmove(mybet, opbet)) {
				case Pass:
					gtp_printf("= pass\n\n"); break;
				case Call:
					gtp_printf("= call\n\n"); break;
				default:
					gtp_printf("= raiseto %d\n\n", mv);
			}
		} else if (strcmp(cmd, "oppmove") == 0) {
			// oppmove <move=call|pass|raise (x=1|2|...)>
			if (gtp_scanf("%s", move)) break;
			gtp_printf("= \n\n");

			if (strcmp(move, "pass") == 0) game.oppmove(Pass);
			else if (strcmp(move, "call") == 0) game.oppmove(Call);
			else if (strcmp(move, "raiseto") == 0) {
				int bet;
				gtp_scanf("%d", &bet);
				game.oppmove(bet);
			} else {
				gtp_printf("? Nieznany ruch : %s\n\n", move);
				break;
			}
		} else if (strcmp(cmd, "showdown") == 0) {
			// showdown <player=SB|BB> <card=0|1>
			if (gtp_scanf("%s %d", player, &card)) break;
			gtp_printf("= \n\n");

			if (strcmp(player, "SB") == 0) game.showdown(0, card);
			else if (strcmp(player, "BB") == 0) game.showdown(1, card);
			else {
				fprintf(stderr, "Nierozpoznany gracz : %s\n", player);
				break;
			}
		} else {
			gtp_printf("? Nie obslugiwane polecenie : %s\n\n", cmd);
			break;
		}
	}

	return 0;
}


