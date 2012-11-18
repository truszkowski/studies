#include <ctime>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>

#include "gtp.hh"
#include "class.hh"
#include "tree.hh"

// Plansza:
Board board;
int player = 2;
bool opmoved = true;

Tree tree;
#define memorylen ( 300 * 1024 * 1024 )
char *memory = NULL;// [memorylen];

int main(void)
{
	const size_t maxlinelen = 4096;
	char line[maxlinelen], cmd[maxlinelen];

	timeval tv;
	gettimeofday(&tv, 0);
	srand(tv.tv_usec);

	signal(SIGPIPE, SIG_IGN);
	
	memory = new char[memorylen];

	tree.silent();
	tree.init(memory, memorylen);
	int tmsec = 0;

	while (1) {
		// Odczytujemy cala linnie odrazu, zeby nie bylo potem, ze na jakims
		// scanfie czekamy i robimy przekroczenie czasu.
		//while (!gtp_is_something()) { }
		gtp_readline(line, maxlinelen);

		if (sscanf(line, "%s", cmd) != 1) continue;

		// Reszta linni to parametry polecenia, przesuwamy wskaznik na miejsce 
		// jeszcze nieprzeczytane.
		char *params = line + strlen(cmd) + 1;

		if (strcmp(cmd, "name") == 0) gtp_printf("= pt209223\n\n");
		else if (strcmp(cmd, "author") == 0) gtp_printf("= pt209223\n\n");
		else if (strcmp(cmd, "version") == 0) gtp_printf("= %s %s\n\n", __DATE__, __TIME__);
		else if (strcmp(cmd, "quit") == 0) { gtp_printf("= \n\n"); break; }
		else if (strcmp(cmd, "time_left") == 0) { 
			sscanf(params, "%d", &tmsec);
			gtp_printf("= \n\n");
		} else if (strcmp(cmd, "setboard") == 0) {
			// setboard <board_size=19> <Board::Pawns=5> <board=#.> = 
			// Tutaj opis planszy bedzie w kolejnych liniach...
			size_t bsize, psize;
			gtp_readline(line, maxlinelen);
			sscanf(line, "%u", &bsize);
			gtp_readline(line, maxlinelen);
			sscanf(line, "%u", &psize);

			char b[bsize * (bsize+1) + 1];

			bool format_ok = true;

			for (size_t i = 0; i < bsize; ++i) {
				gtp_readline(line, maxlinelen);
				int sn = snprintf(b+i*(bsize+1), bsize + 2, "%s", line);
				if (sn != (int)bsize + 1) { format_ok = false; break; }
			}

			if (!format_ok) {
				gtp_printf("? setboard : Niepoprawny format planszy\n\n");
				continue;
			}

			if (bsize == 0 || bsize > Board::MaxSize) {
				gtp_printf("? setboard : Nieprawidlowy rozmiar planszy(%u), powinien byc z przedzialu: 1-%u\n\n", bsize, Board::MaxSize);
				continue;
			}

			if (psize == 0 || psize > Board::MaxPawns) {
				gtp_printf("? setboard : Nieprawidlowa liczba pionkow(%u), powinno byc z przedzialu: 1 - %u\n\n", psize, Board::MaxPawns);
				continue;
			}

			gtp_printf("= \n\n");
			board.set(bsize, psize);
			board.set(b);
		} else if (strcmp(cmd, "place") == 0) {
			// place <player=al|num> <pawn_id=1|2|...a|b|...> <coord> = 
			char pl_str[maxlinelen], pawn_str[maxlinelen], coord_str[maxlinelen];
			int ss = sscanf(params, "%s %s %s", pl_str, pawn_str, coord_str);

			if (ss != 3) {
				gtp_printf("? place : Nie odczytalem wszystkich parametrow(player/pawn/coord) z linni : %s\n", line);
				continue;
			}

			int pl, pawn;

			if (strcmp(pl_str, "al") == 0) pl = 0;
			else if (strcmp(pl_str, "num") == 0) pl = 1;
			else { 
				gtp_printf("? place : Nieznany gracz : %s\n\n", pl_str);
				continue;
			}

			if (pl) { // == num
				pawn = atoi(pawn_str) - 1;
				if (pawn < 0 || pawn >= Board::Pawns) {
					gtp_printf("? place : Zle okreslony id(num) pionka : %s\n\n", pawn_str);
					continue;
				}
			} else { // == al
				pawn = pawn_str[0] - 'a';
				if (pawn < 0 || pawn >= Board::Pawns) {
					gtp_printf("? place : Zle okreslony id(al) pionka : %s\n\n", pawn_str);
					continue;
				}
			}

			Board::Coord coord = Board::conv(coord_str);
			if (!coord.good()) {
				gtp_printf("? place : Zle okreslone koordynaty : %s\n\n", coord_str);
				continue;
			}

			if (!board.checkPlace(pl, coord)) {
				gtp_printf("? place : Niepoprawne miejsce : %s %s %s\n\n", pl_str, pawn_str, coord_str);
				continue;
			}

			gtp_printf("= \n\n");

			board.place(pl, coord);
		} else if (strcmp(cmd, "genplace") == 0) {
			// genplace <player=al|num> <pawn_id=1|2|...a|b|...> = <coord>
			char pl_str[maxlinelen], pawn_str[maxlinelen];
			int ss = sscanf(params, "%s %s", pl_str, pawn_str);
			
			if (ss != 2) {
				gtp_printf("? genplace : Nie odczytalem wszystkich parametrow(player/pawn) z linni : %s\n", line);
				continue;
			}

			int pl, pawn;

			if (strcmp(pl_str, "al") == 0) pl = 0;
			else if (strcmp(pl_str, "num") == 0) pl = 1;
			else {
				gtp_printf("? genplace : Nieznany gracz : %s\n\n", pl_str);
				continue;
			}

			if (pl) { // == num
				pawn = atoi(pawn_str) - 1;
				if (pawn < 0 || pawn >= Board::Pawns) {
					gtp_printf("? genplace : Zle okreslony id(num) pionka : %s\n\n", pawn_str);
					continue;
				}
			} else { // == al
				pawn = pawn_str[0] - 'a';
				if (pawn < 0 || pawn >= Board::Pawns) {
					gtp_printf("? genplace : Zle okreslony id(al) pionka : %s\n\n", pawn_str);
					continue;
				}
			}

			Board::Coord coord;
			
			choosePlaceFor(board, pl, coord, tmsec);
			gtp_printf("= %s\n\n", Board::conv(coord));
			
			board.place(pl, coord);
			tmsec = 0; // Zerujemy bo wykonalismy juz ruch
		} else if (strcmp(cmd, "move") == 0) {
			// move <player=al|num> <pawn_id=1|2|...a|b|...> <coord> <coord>
			char pl_str[maxlinelen], pawn_str[maxlinelen], dirdst_str[maxlinelen], dirbck_str[maxlinelen];
			int ss = sscanf(params, "%s %s %s %s", pl_str, pawn_str, dirdst_str, dirbck_str);

			// Ignorujemy pass, do niczego w programie ta wiedza nie jest potrzebna
			if (strcmp(pl_str, "pass") == 0 || strcmp(pawn_str, "pass") == 0) {
				gtp_printf("= \n\n");
				continue;
			}

			if (ss != 4) {
				gtp_printf("? move : Nie odczytalem wszystkich parametow(player/pawn/dir/dir) z linni : %s\n", line);
				continue;
			}
		
			int pl, pawn;
			
			if (strcmp(pl_str, "al") == 0) pl = 0;
			else if (strcmp(pl_str, "num") == 0) pl = 1;
			else {
				gtp_printf("? move : Nieznany gracz : %s\n\n", pl_str);
				continue;
			}

			if (pl) { // == num
				pawn = atoi(pawn_str) - 1;
				if (pawn < 0 || pawn >= Board::Pawns) {
					gtp_printf("? move : Zle okreslony id(num) pionka : %s\n\n", pawn_str);
					continue;
				}
			} else { // == al
				pawn = pawn_str[0] - 'a';
				if (pawn < 0 || pawn >= Board::Pawns) {
					gtp_printf("? move : Zle okreslony id(al) pionka : %s\n\n", pawn_str);
					continue;
				}
			}

			Board::Coord src = board.pawns[pl][pawn].c;
			Board::Coord dst = Board::dir(src, dirdst_str);
			Board::Coord bck = Board::dir(dst, dirbck_str);

			if (!src.good() || !dst.good() || !bck.good()) {
				gtp_printf("? move : Zle okreslone koordynaty : %s i %s\n", dirdst_str, dirbck_str);
				continue;
			}

			if (!board.checkMove(pl, Board::Move(src, dst, bck))) {
				gtp_printf("? move : Niepoprawny ruch : %s %s %s %s\n", pl_str, pawn_str, dirdst_str, dirbck_str);
				continue;
			}
			gtp_printf("= \n\n");

			// Jak koncowka gry to olewamy co robi przeciwnik, dbamy tylko o swoj teren.
			if (board.isEndGame())
				tree.update(pl, Board::Move::Pass);
			else
				tree.update(pl, Board::Move(src, dst, bck));
			
			board.move(pl, Board::Move(src, dst, bck));
			opmoved = true;
		} else if (strcmp(cmd, "genmove") == 0) {
			// genmove <player=al|num> = ( <pawn_id=1|2|...a|b|...> <coord> <coord> )| pass
			char pl_str[maxlinelen], pawn_str[maxlinelen];
			int ss = sscanf(params, "%s", pl_str);

			if (ss != 1) {
				gtp_printf("? genmove : Nie odczytalem wszystkich parametow(player) z linni : %s\n", line);
				continue;
			}

			int pl, pawn;
			
			if (strcmp(pl_str, "al") == 0) pl = 0;
			else if (strcmp(pl_str, "num") == 0) pl = 1;
			else { 
				gtp_printf("? genmove : Nieznany gracz : %s\n", pl_str);
				continue;
			}

			if (player == 2) { // Trzeba utworzyc korzen gry
				player = pl; 
				tree.create(board, player);
			}

			// Jak przeciwnik sie nie rusza dopowiadamy sobie Pass...
			if (!opmoved) tree.update(!pl, Board::Move::Pass);

			opmoved = false;

			Board::Move move;
			tmsec = calcTimeoutFor(board, pl, tmsec);
			//tmsec = tmsec < 30000 ? 500 : 2000;
			//tmsec = tmsec < 30000 ? 500 : 5000;

			if (tree.choose(pl, move, tmsec) == false || move == Board::Move::Pass) {
				gtp_printf("= pass\n\n");
			} else {
				pawn = Board::Pawns;
				for (size_t i = 0; i < Board::Pawns; ++i)
					if (board.pawns[pl][i].c == move.src) { pawn = i; break; }

				assert(pawn < Board::Pawns);

				if (pl == 0) // al
					sprintf(pawn_str, "%c", 'a' + pawn);
				else // num
					sprintf(pawn_str, "%c", '1' + pawn);

				gtp_printf("= %s %s %s\n\n", pawn_str, 
						Board::dir(move.src, move.dst), Board::dir(move.dst, move.bck));

				board.move(pl, move);
			}
			
			tree.update(pl, move);
				
			tmsec = 0; // Zerujemy bo wykonalismy juz ruch
		} else 
			gtp_printf("? Nieprawidlowe polecenie : %s\n", line);
	}

	delete[] memory;
	return 0;
}


