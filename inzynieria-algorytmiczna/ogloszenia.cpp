#include <cstdio>
#include <utility>
#include <algorithm>
#include <cassert>
#define swap(a,b) do { (a)^=(b); (b)^=(a); (a)^=(b); } while (0)
#define abs(a,b) ( (a) < (b) ? (b) - (a) : (a) - (b) )
using namespace std;
typedef unsigned short int u16;
typedef unsigned long int u32;
typedef unsigned long long int u64;
typedef pair<pair<u32, u32>, pair<u32, u32> > rect;
const u32 MAX = 50000; // 65536;

u64 mul64(u64 a, u64 b) { return a * b; }

// Zbior prostokatow.
rect v[MAX]; // 4 * 4 * MAX 
// X-strutura dla miotly, posortowane indeksy v[] po X-sach
u32 front[MAX], back[MAX]; // 2 * 4 * MAX
u32 n;
// Pole & obwod.
u64 area = 0, edje = 0;

bool front_cmp(const u32 &i, const u32 &j) { return (v[i].first.first < v[j].first.first); } 
bool back_cmp(const u32 &i, const u32 &j) { return (v[i].second.first < v[j].second.first); }

// Reprezentacja drzewa przedzialowego.
// podajemy przedzial na jakim ma dzialac i ile max wezlow w nim bedzie.
template <u32 MinValue, u32 MaxValue, u32 MaxNodes>
class ITree {
	private:
		// Reprezentacja wezla w drzewie przedzialowym.
		struct Node { 
			Node *left, *right; 
			u32 count, cover;
		};
#define NONE ((Node*)0x0)
#define FULL ((Node*)0x1)
//		static const u32 NONE = 0x0;
//		static const u32 FULL = 0x1;

		// korzen i wolne wierzcholki.
		Node *root, *buffer;
		u32 buflen;

	public:
		ITree(void) : root(NONE), buffer(new Node[MaxNodes]), buflen(0) { }
		~ITree(void) { delete[] buffer; }

		// Dodaj, usun, pokrycie, czysc.
		void add(u32 a, u32 b) { add(root, a, b-1, MinValue, MaxValue); }
		void del(u32 a, u32 b) { del(root, a, b-1, MinValue, MaxValue); }
		u32 cover(void) const { return root ? root->cover : 0; }
		void clear(void) { root = NONE; buflen = 0; }
		u32 size(void) const { return buflen; }
	
	private:
		Node *get_free(void) {
			// Bierzemy z bufora nowy wezel.
			Node *node = &buffer[buflen++];
			node->left = node->right = NONE;
			node->count = node->cover = 0;
			return node;
		}
		void set_cover(Node *node, u32 a, u32 b) {
			// ustawiamy pokrycie.
			if (node->count) node->cover = b-a+1;
			else { 
				u32 left = 0, right = 0;

				if (node->left == FULL) left = ((a+b)/2-a+1);
				else if (node->left != NONE) left = node->left->cover;
				
				if (node->right == FULL) right = (b-(a+b)/2+0);
				else if (node->right != NONE) right = node->right->cover;

				node->cover = left + right;
			}
		}
		void add(Node *&node, u32 a, u32 b, u32 minv, u32 maxv) {
			if (node == NONE) { // Nowy wezel w drzewie.
				// Byc moze wystarczy jedynie zamarkowac, ze tutaj jest jakis wezel.
				if (minv == maxv || (a == minv && b == maxv)) { node = FULL; return; }
				// wpp przydzielamy.
				node = get_free();
			} else if (node == FULL) {
				// Tym razem "zmarkowany" wezel musi zaistniec fizycznie.
				node = get_free(); 
				node->count = 1; // bo drugie odwiedziny (1 byl "mark").
				node->cover = maxv-minv+1; // poprzedni tak by ustawil cover.
			}
			if (minv == maxv || (a == minv && b == maxv)) ++node->count; 
			else {
				u32 middle = (minv+maxv)/2;

				// W dol, rekurencyjnie...
				if (a > middle) {
					add(node->right, a, b, middle+1, maxv);
				} else if (b <= middle) {
					add(node->left, a, b, minv, middle);
				} else {
					add(node->left, a, middle, minv, middle);
					add(node->right, middle+1, b, middle+1, maxv);
				}
			}
			set_cover(node, minv, maxv);
		}
		void del(Node *&node, u32 a, u32 b, u32 minv, u32 maxv) {
			if (node == FULL) { node = NONE; return; }

			if (minv == maxv || (a == minv && b == maxv)) --node->count;
			else {
				u32 middle = (minv+maxv)/2;

				// W dol, rekurencyjnie...
				if (a > middle) { 
					del(node->right, a, b, middle+1, maxv);
				} else if (b <= middle) {
					del(node->left, a, b, minv, middle);
				} else {
					del(node->left, a, middle, minv, middle);
					del(node->right, middle+1, b, middle+1, maxv);
				}
			}
			set_cover(node, minv, maxv);
		}
};


// Nasze drzewko: (gdy: MAX=2^16)
// Bez markowania "FULL": 
//   maxsize = ( 2*2^17-1 ) + ( 2*12*2*2^16 ) + ( 2*2^16 ) = 3.538.943
// Z markowaniem "FULL": 
//   maxsize = ( 2*2^17-1 ) + ( 13*2*2^16 ) = 1.966.079
ITree<0, (1<<30)-1, 38*MAX> itree;

int main(void) {
	scanf("%lu", &n);
	for (u32 i = 0; i < n; ++i)
		scanf("%lu%lu%lu%lu", 
				&v[i].first.first, &v[i].first.second,
				&v[i].second.first, &v[i].second.second);

	// Przygotowujemy X-strukture dla zamiatania.
	for (u32 i = 0; i < n; ++i) front[i] = back[i] = i;
	sort(front, front+n, front_cmp);
	sort(back, back+n, back_cmp);

	// Odznaczamy start
	itree.add(v[front[0]].first.second, v[front[0]].second.second);
	u32 xlast = v[front[0]].first.first;
	u32 ylast = itree.cover();
	area = 0;
	edje = itree.cover();
//	fprintf(stderr, "--x-- a:((%lu %lu) (%lu %lu)) area=%llu, cover=%lu, xlast=%lu, edje=%llu, ylast=%lu\n", v[front[0]].first.first, v[front[0]].first.second, v[front[0]].second.first, v[front[0]].second.second, area, itree.cover(), xlast, edje, ylast);

	// Przechodzimy po X-strukturze.
	for (u32 i = 1, j = 0; j < n; ) {
		if (i < n && v[front[i]].first.first <= v[back[j]].second.first) { 
			// Zwiekszamy pole...
			area += mul64((v[front[i]].first.first - xlast), itree.cover());
			// Dodajemy prostokat na Y-strukture...
			itree.add(v[front[i]].first.second, v[front[i]].second.second);
			// Odznaczamy miejsce dokad policzylismy...
			xlast = v[front[i]].first.first;
			// Zliczamy pionowe krawedzie...
			edje += abs(itree.cover(), ylast);
			// Zapamietujemy aktualna dlugosc pionowych krawedzi...
			ylast = itree.cover(); 
//			fprintf(stderr, "--x-- a:((%lu %lu) (%lu %lu)) area=%llu, cover=%lu, xlast=%lu, edje=%llu, ylast=%lu\n", v[front[i]].first.first, v[front[i]].first.second, v[front[i]].second.first, v[front[i]].second.second, area, itree.cover(), xlast, edje, ylast);
			++i; 
		} else {
			// Zwiekszamy pole...
			area += mul64((v[back[j]].second.first - xlast), itree.cover());
			// Zdejmujemy z Y-struktury.
			itree.del(v[back[j]].first.second, v[back[j]].second.second);
			// Odznaczamy miejsce dokad policzylismy...
			xlast = v[back[j]].second.first;
			// Zliczamy pionowe krawedzie...
			edje += abs(itree.cover(), ylast);
			// Zapamietujemy aktualna dlugosc pionowych krawedzi...
			ylast = itree.cover();
//			fprintf(stderr, "--x-- d:((%lu %lu) (%lu %lu)) area=%llu, cover=%lu, xlast=%lu, edje=%llu, ylast=%lu\n", v[back[j]].first.first, v[back[j]].first.second, v[back[j]].second.first, v[back[j]].second.second, area, itree.cover(), xlast, edje, ylast);
			++j;
		}
	}

	fprintf(stderr, "size= %lu\n", itree.size());
	// Czyscimy...
	itree.clear();

	// Policzylismy juz cale pole a obwod tylko dla krawedzi pionowych,
	// musimy jeszcze tylko podliczyc dla pioziomych, wiec zmieniamy
	// kierunek zamiatania z lewo-prawo na dol-gora.
	
	// Zamienimy wiec wspolrzedne X z Y.
	for (u32 i = 0; i < n; ++i) {
		swap(v[i].first.first, v[i].first.second);
		swap(v[i].second.first, v[i].second.second);
	}
	// I dalej identycznie jak wczesniej...

	// Przygotowujemy X-strukture dla zamiatania.
	for (u32 i = 0; i < n; ++i) front[i] = back[i] = i;
	sort(front,front+n,front_cmp);
	sort(back,back+n,back_cmp);

	// Odznaczamy start
	itree.add(v[front[0]].first.second, v[front[0]].second.second);
//	u32 xlast = v[front[0]].first.first;
	ylast = itree.cover();
//	area = 0;
	edje += itree.cover();
//	fprintf(stderr, "--y-- a:((%lu %lu) (%lu %lu)) cover=%lu, edje=%llu, ylast=%lu\n", v[front[0]].first.second, v[front[0]].first.first, v[front[0]].second.second, v[front[0]].second.first, itree.cover(), edje, ylast);

	// Przechodzimy po X-strukturze.
	for (u32 i = 1, j = 0; j < n; ) {
		if (i < n && v[front[i]].first.first <= v[back[j]].second.first) { 
//			// Zwiekszamy pole...
//			area += (v[front[i]].first.first - xlast)*itree.cover();
			// Dodajemy prostokat na Y-strukture...
			itree.add(v[front[i]].first.second, v[front[i]].second.second);
//			// Odznaczamy miejsce dokad policzylismy...
//			xlast = v[front[i]].first.first;
			// Zliczamy pionowe krawedzie...
			edje += abs(itree.cover(), ylast);
			// Zapamietujemy aktualna dlugosc pionowych krawedzi...
			ylast = itree.cover(); 
//			fprintf(stderr, "--y-- a:((%lu %lu) (%lu %lu)) cover=%lu, edje=%llu, ylast=%lu\n", v[front[i]].first.second, v[front[i]].first.first, v[front[i]].second.second, v[front[i]].second.first, itree.cover(), edje, ylast);

			++i; 
		} else { 
//			// Zwiekszamy pole...
//			area += (v[back[j]].second.first - xlast)*itree.cover();
			// Zdejmujemy z Y-struktury.
			itree.del(v[back[j]].first.second, v[back[j]].second.second);
//			// Odznaczamy miejsce dokad policzylismy...
//			xlast = v[back[j]].second.first;
			// Zliczamy pionowe krawedzie...
			edje += abs(itree.cover(), ylast);
			// Zapamietujemy aktualna dlugosc pionowych krawedzi...
			ylast = itree.cover();
//			fprintf(stderr, "--y-- d:((%lu %lu) (%lu %lu)) cover=%lu, edje=%llu, ylast=%lu\n", v[back[j]].first.second, v[back[j]].first.first, v[back[j]].second.second, v[back[j]].second.first, itree.cover(), edje, ylast);

			++j;
		}
	}

	fprintf(stderr, "size= %lu\n", itree.size());
	// Done.

	printf("%llu %llu\n", area, edje);
	return 0;
}

