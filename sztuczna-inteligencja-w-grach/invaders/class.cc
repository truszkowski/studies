#include "class.hh"

// Definiujemy tablice przesuniec:
//
// | (-1,-1) (-1, 0) (-1. 1) |     |  4   5   6  |     |  0   3   6  |
// | ( 0,-1) ( i, j) ( 0, 1) |  =  |  3   x   7  |  =  |  1   x   7  |
// | ( 1,-1) ( 1, 0) ( 1, 1) |     |  2   1   0  |     |  2   5   8  |
//    ^ wg wspolrzednych           ^ odpowiadajacy     ^ odpowiadajacy index 
//        na planszy               index w tablicy
//

const int8_t Board::inexts[16] = { 1, 1,  1,  0, -1, -1, -1, 0,  1, 1,  1,  0, -1, -1, -1, 0 };
const int8_t Board::jnexts[16] = { 1, 0, -1, -1, -1,  0,  1, 1,  1, 0, -1, -1, -1,  0,  1, 1 };
const int8_t Board::ijmove[9]  = { 4, 3, 2, 5, 0, 1, 6, 7, 0 };

uint8_t Board::Size = 19;           // Aktualny rozmiar planszy
uint8_t Board::Pawns = 5;           // Aktualna liczba pionkow

const Board::Move Board::Move::Pass(
		Board::Coord(Board::MaxSize, Board::MaxSize), 
		Board::Coord(Board::MaxSize, Board::MaxSize), 
		Board::Coord(MaxSize, MaxSize));

static const Board::Coord firstshot[4][6] = {
	{ Board::Coord(11,  2), Board::Coord(11,  3), Board::Coord(12,  2), Board::Coord(12,  3), Board::Coord(13,  2), Board::Coord(13,  3) },
	{ Board::Coord( 2,  5), Board::Coord( 2,  6), Board::Coord( 2,  7), Board::Coord( 3,  5), Board::Coord( 3,  6), Board::Coord( 3,  7) },
	{ Board::Coord(15, 11), Board::Coord(15, 12), Board::Coord(16, 11), Board::Coord(16, 12), Board::Coord(), Board::Coord() },
	{ Board::Coord( 5, 15), Board::Coord( 5, 16), Board::Coord( 6, 15), Board::Coord( 6, 16), Board::Coord(), Board::Coord() }
};

static const size_t firstshot_count[4] = { 6, 6, 4, 4 };

static const Board::Coord secondshot[] = {
	Board::Coord(10,  3), Board::Coord(10,  4), Board::Coord(11,  4), Board::Coord(12,  4), Board::Coord(13,  4), Board::Coord(14,  4), 
	Board::Coord(14,  3), Board::Coord( 3,  4), Board::Coord( 4,  4), Board::Coord( 4,  5), Board::Coord( 4,  6), Board::Coord( 4,  7), 
	Board::Coord( 4,  8), Board::Coord( 3,  8), Board::Coord(15, 10), Board::Coord(14, 10), Board::Coord(14, 11), Board::Coord(14, 12), 
	Board::Coord(14, 13), Board::Coord(14, 14), Board::Coord(14, 15), Board::Coord(15, 13), Board::Coord(16, 13), Board::Coord(15, 14), 
	Board::Coord( 4, 14), Board::Coord( 5, 14), Board::Coord( 5, 14), Board::Coord( 6, 14), Board::Coord( 7, 14), Board::Coord( 8, 14), 
	Board::Coord( 8, 15), Board::Coord( 7, 15), Board::Coord( 7, 16), Board::Coord(10, 14), Board::Coord(10, 15), Board::Coord(11, 14), 
	Board::Coord(11, 15), Board::Coord(11, 16), Board::Coord(12, 14), Board::Coord(12, 15), Board::Coord(12, 16), Board::Coord(13, 14), 
	Board::Coord(13, 14), Board::Coord(13, 15), Board::Coord(13, 16), Board::Coord( 5,  5), Board::Coord( 5,  6), Board::Coord( 6,  7), 
	Board::Coord( 5, 13), Board::Coord( 6, 13), Board::Coord( 7, 13), Board::Coord(11,  5), Board::Coord(12,  5), Board::Coord(13,  5), 
	Board::Coord(13, 11), Board::Coord(13, 12), Board::Coord(13, 13), Board::Coord(12, 13), Board::Coord(11, 13), Board::Coord(10,  5),
	Board::Coord( 5,  8), Board::Coord(11,  2), Board::Coord(11,  3), Board::Coord(12,  2), Board::Coord(12,  3), Board::Coord(13,  2), 
	Board::Coord(13,  3), Board::Coord( 2,  5), Board::Coord( 2,  6), Board::Coord( 2,  7), Board::Coord( 3,  5), Board::Coord( 3,  6), 
	Board::Coord( 3,  7), Board::Coord(15, 11), Board::Coord(15, 12), Board::Coord(16, 11), Board::Coord(16, 12), Board::Coord( 5, 15), 
	Board::Coord( 5, 16), Board::Coord( 6, 15), Board::Coord( 6, 16)
};

static const size_t secondshot_count = 81;

enum BType { TNone, TDef, TNDef };

static BType type = TNone;
static const size_t def_size = 19;
static const size_t def_pawns = 5;
static const char *def_board[] = {
	"...................", 
	"...................",
	"...................",
	"...#.....#######...",
	"...#...............",
	"...#...............",
	"...#...............",
	"...#...............",
	"...#...............",
	"...#.....#######...",
	"...................",
	"...................",
	"...................",
	"...................",
	"...................",
	"...#######.....#...",
	"...................",
	"...................",
	"..................."
};

void Board::print(void) const
{
	fprintf(stderr, " - a b c d e f g h i j k l m n o p q r s -\n");

	for (size_t i = 0; i < Size; ++i) {
		fprintf(stderr, "%2u ", Size-i);

		for (size_t j = 0; j < Size; ++j)
			fprintf(stderr, "%c ", fields[i][j].c);

		fprintf(stderr, "%2u \n", Size-i);	
	}

	fprintf(stderr, " - a b c d e f g h i j k l m n o p q r s -\n");

	for (size_t i = 0; i < Pawns; ++i) {
		if (i < pawnc[0]) 
			fprintf(stderr, "- %c => %3s -    ", 'a'+i, conv(pawns[0][i].c));
		else
			fprintf(stderr, "- - => --- -    ");

		if (i < pawnc[1])
			fprintf(stderr, "- %c => %3s -", '1'+i, conv(pawns[1][i].c));
		else 
			fprintf(stderr, "- - => --- -");

		fprintf(stderr, "\n");
	}
}

static bool choosePlaceForDef(const Board &b, int pl, Board::Coord &dst)
{
	for (size_t i = 0; i < 4; ++i) {
		bool busy = false;
		for (size_t j = 0; j < firstshot_count[i]; ++j) {
			if (!b[firstshot[i][j]].isEmpty()) { busy = true; break; }
			for (size_t k = 0; k < 8; ++k) {
				Board::Coord c = b.nthNextCoord(firstshot[i][j], k);
				if (c.good() && (!b[c].isEmpty() && !b[c].isBlocked())) { busy = true; break; }
			}
		}

		if (!busy) {
			int rd = rand() % firstshot_count[i];
			dst = firstshot[i][rd];
			return true;
		}
	}

	Board::Coord toRandom[Board::MaxGenPlaces];
	int16_t toRandomLen = 0;

	Board::Score best = Board::Score::MinFor(pl);

	if (b.pawnc[pl] < 4) {
		for (size_t i = 0; i < secondshot_count; ++i) {
			if (b[secondshot[i]].isEmpty()) {
				bool busy = false;
				for (size_t k = 0; k < 8; ++k) {
					Board::Coord c = b.nthNextCoord(secondshot[i], k);
					if (c.good() && (!b[c].isEmpty() && !b[c].isBlocked())) { busy = true; break; }
				}
				if (!busy) {
					Board::Score score;
					Board b2(b);
					b2.place(pl, secondshot[i]);
					b2.calc(score);

					if (score.greater(pl, best)) {
						toRandom[0] = secondshot[i]; toRandomLen = 1; best = score;
					} else if (score.equal(pl, best)) {
						toRandom[toRandomLen] = secondshot[i]; ++toRandomLen;
					}
				}
			}
		}
	} else {
		for (size_t i = 0; i < Board::Size; ++i) {
			for (size_t j = 0; j < Board::Size; ++j) {
				if (b.getField(i, j).isEmpty()) {
					bool busy = false;
					for (size_t k = 0; k < 8; ++k) {
						Board::Coord c = b.nthNextCoord(i, j, k);
						if (c.good() && (!b[c].isEmpty() && !b[c].isBlocked())) { busy = true; break; }
					}
					if (!busy) {
						Board::Score score;
						Board b2(b);
						b2.place(pl, Board::Coord(i, j));
						b2.calc(score);

						if (score.greater(pl, best)) {
							toRandom[0] = Board::Coord(i, j); toRandomLen = 1; best = score;
						} else if (score.equal(pl, best)) { 
							toRandom[toRandomLen] = Board::Coord(i, j); ++toRandomLen;
						}
					}
				}
			}
		}
	}

	assert(toRandomLen > 0);

	int rd = rand() % toRandomLen;

	dst = toRandom[rd];

	return true;
}

bool choosePlaceFor(const Board &b, int pl, Board::Coord &dst, int)
{
	// Sprawdzenie jak wybierac...
	/**/switch (type) {
		case TNone:
			if (Board::Size != def_size || Board::Pawns != def_pawns) { type = TNDef; break; }
			for (size_t i = 0; i < Board::Size; ++i) {
				for (size_t j = 0; j < Board::Size; ++j) 
					if (b.fields[i][j].isBlocked() != 
							(def_board[i][j] == Board::BlockedField)) { type = TNDef; fprintf(stderr, "<> %u %u '%c' '%c'\n", i, j, b.fields[i][j].c, def_board[i][j]); break; }
				if (type == TNDef) break;
			}
			if (type == TNDef) break;
			type = TDef;
		case TDef:
			// Wg ksiegi otwarc !
			return choosePlaceForDef(b, pl, dst);
		case TNDef:
			(void)0;
	}/**/

	// Wg normalnego algorytmu,,,

	Board board(b);

	Board::Score best = Board::Score::MinFor(pl), score;
	Board::Coord places[Board::Size*Board::Size];
	size_t count = 0;

	for (size_t i = 0; i < Board::Size; ++i) {
		for (size_t j = 0; j < Board::Size; ++j) {
			if (b.fields[i][j].isEmpty()) {
				board.place(pl, Board::Coord(i, j));
				board.calc2(score);
				//score = Board::Score::MaxFor(pl);
				board.undo(pl, Board::Coord(i, j));

				if (score.greater(pl, best)) {
					places[0] = Board::Coord(i, j);
					count = 1;
					best = score;
				} else if (score.equal(pl, best)) {
					places[count] = Board::Coord(i, j);
					++count;
				}
			}
		}
	}
	
	if (count == 0) return false;

	Board::Coord final[count];
	best = Board::Score::MinFor(pl);
	size_t count2 = 0;

	for (size_t i = 0; i < count; ++i) {
		board.place(pl, places[i]);
		board.calc(score);
		board.undo(pl, places[i]);

		if (score.greater(pl, best)) {
			final[0] = places[i];
			count2 = 1;
			best = score;
		} else if (score.equal(pl, best)) {
			final[count2] = places[i];
			++count2;
		}
	}

	int choose = count2 > 0 ? ( rand()%count2 ) : 0;
	dst = final[choose];

	return true;
}


int calcTimeoutFor(const Board &b, int pl, int tmsec)
{
	Board::View view;
	b.look(view);

	size_t both = 0, only[2] = { 0, 0 };
	
	for (size_t i = 0; i < Board::Size; ++i) 
		for (size_t j = 0; j < Board::Size; ++j)
			if (view.fields[i][j].isEmpty()) {
				if (view.fields[i][j].owner[0] != Board::NoPawn) {
					if (view.fields[i][j].owner[1] != Board::NoPawn) ++both;
					else ++only[0];
				} else if (view.fields[i][j].owner[1] != Board::NoPawn) ++only[1];
			}

	int moves = both/2 + only[pl] + 1;
	return moves < 5 ? ( tmsec / 6 ) : ( ( 13 * tmsec ) / ( 10 * moves ) );
}

