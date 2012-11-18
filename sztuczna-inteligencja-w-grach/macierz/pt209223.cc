#include <ctime>
#include <sys/time.h>
#include <signal.h>

#include "gtp.hh"
#include "game.hh"

Game game;
size_t me = 0;

int main(void)
{
	char cmd[128], player[128], pl0[2], pl1[2];

	timeval tv;
	gettimeofday(&tv, 0);
	srand(tv.tv_usec);

	signal(SIGPIPE, SIG_IGN);
	uint32_t m0, m1;
	
	while (1) {
		if (gtp_scanf("%s", cmd)) break; 

		if (strcmp(cmd, "name") == 0) {
			if (gtp_printf("= pt209223\n\n")) break;
		} else if (strcmp(cmd, "author") == 0) {
			if (gtp_printf("= pt209223\n\n")) break;
		} else if (strcmp(cmd, "quit") == 0) {
			gtp_printf("= \n\n");
			break;
		} else if (strcmp(cmd, "genmove") == 0) {
			gtp_scanf("%s", player);
			if (strcmp(player, "min") == 0) { // max
				me = 0; // wybieram wiersz !
				gtp_printf("= %c\n\n", game.move(0) ? 'b':'s');
			} else if (strcmp(player, "max") == 0) { // min
				me = 1; // wybieram kolumne !
				gtp_printf("= %c\n\n", game.move(1) ? 'r':'l');
			} else {
				gtp_printf("? Nieznany gracz\n\n");
				break;
			}
		} else if (strcmp(cmd, "last") == 0) {
			gtp_scanf("%s %s", pl0, pl1);
			gtp_printf("= \n\n");

			if (pl0[0] == 's' || pl0[0] == 'b') { // max (s/b)
				m0 = (pl0[0] == 's') ? 0:1; // s/b
				m1 = (pl1[0] == 'l') ? 0:1; // l/r
			} else { // min (l/r)
				m0 = (pl1[0] == 's') ? 0:1; // s/b
				m1 = (pl0[0] == 'l') ? 0:1; // l/r
			}
			
			game.add(m0, m1);
		} else {
			gtp_printf("? Nie obslugiwane polecenie : %s\n\n", cmd);
			break;
		}
	}

	return 0;
}


