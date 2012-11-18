#include <cstdio>
const unsigned MAX = 500000;

struct list {
	struct ent {
		unsigned  data;
		ent *next;

		ent(void) : next(NULL) { }
		~ent(void) { }
	};

	ent *first;

	list(void) : first(NULL) { }
	~list(void) { }

	void put(ent *e) {
		e->next = first;
		first = e;
	}
};

struct fifo {
	unsigned ents[MAX];
	unsigned begin, end;

	fifo(void) : begin(0), end(0) { }
	~fifo(void) { }
	
	void push(unsigned x) { ents[end++] = x; }
	unsigned pop(void) { return ents[--end]; }
	bool empty(void) const { return begin == end; }
};

unsigned n, m;

fifo queue; // 2MB
list tree[MAX]; // 2MB
list::ent ents[2*MAX]; // 8MB
unsigned entc;

unsigned mark[MAX], marked; // 2MB

unsigned vi[MAX], vo[MAX]; // 4MB
unsigned ci[MAX], co[MAX]; // 4MB

unsigned ms[MAX], mr[MAX], mc, mx; // 4MB

inline unsigned mulmod(unsigned a, unsigned b) {
	unsigned long long aa = a, bb = b, ab = (aa*bb)%m;
	return (unsigned)ab;
//	return (unsigned)(((unsigned long long)a)*((unsigned long long)b))%m;
}

void no_recursive(void) {
	while (!queue.empty()) {
		unsigned id = queue.pop();

		bool iseq = false;
		co[id] = 1;
		ci[id] = 0;
		list::ent *lent;

		for (lent = tree[id].first; lent; lent = lent->next)
			if (mark[id] < mark[lent->data]) {
				vo[id] += vi[lent->data];
				if (vi[lent->data] == vo[lent->data]) iseq = true;
	
				co[id] = mulmod(co[id], (vo[lent->data]==vi[lent->data] ? (co[lent->data]+ci[lent->data])%m : ci[lent->data]));
			}

		mx = 1;
		mc = 0;

		if (iseq) {
			for (lent = tree[id].first; lent; lent = lent->next)
				if (mark[id] < mark[lent->data]) {
					if (vi[lent->data] != vo[lent->data]) mx = mulmod(mx, ci[lent->data]);
					else ms[mc++] = lent->data;
				}
		} else {
			for (lent = tree[id].first; lent; lent = lent->next) 
				if (mark[id] < mark[lent->data]) 
					ms[mc++] = lent->data; // vi > vo
		}

		unsigned i;

		if (mc) {
			mr[0] = (vi[ms[0]] == vo[ms[0]]) ? (co[ms[0]]+ci[ms[0]])%m : ci[ms[0]];

			for (i = 1; i+1 < mc; ++i) 
				mr[i] = mulmod(mr[i-1], (vi[ms[i]] == vo[ms[i]]) ? (co[ms[i]]+ci[ms[i]])%m : ci[ms[i]]);

			unsigned back = 1;
			for (i = mc-1; i > 0; --i) {
				mr[i] = mulmod(mulmod(mr[i-1], back), co[ms[i]]);
				back = mulmod(back, (vi[ms[i]] == vo[ms[i]]) ? (co[ms[i]]+ci[ms[i]])%m : ci[ms[i]]);
			}

			mr[0] = mulmod(back, co[ms[i]]);
		}

		for (i = 0; i < mc; ++i)
			ci[id] = (ci[id] + mr[i])%m;

		ci[id] = mulmod(ci[id], mx);

		vi[id] = vo[id] + (iseq ? 1 : 0);

//		printf("id: %u, vi: %u, vo: %u, ci: %u, co: %u\n", id+1, vi[id], vo[id], ci[id], co[id]);
	}
}

int main(void) {
	scanf("%u", &n);

	for (unsigned i = 0; i < n - 1; ++i) {
		unsigned a, b;
		scanf("%u%u", &a, &b);
		
		ents[entc].data = b-1;
		tree[a-1].put(ents+entc);
		++entc;
		ents[entc].data = a-1;
		tree[b-1].put(ents+entc);
		++entc;
	}

	scanf("%u", &m);

	queue.push(0);

	for (unsigned i = 0; i < n; ++i) {
		unsigned it = queue.ents[i];
		mark[it] = ++marked;

		for (list::ent *lent = tree[it].first; lent; lent = lent->next)
			if (!mark[lent->data]) 
				queue.push(lent->data);
	}

	no_recursive();

	printf("%u\n%u\n", vi[0], (n == 1 ? 1 : (vi[0] == vo[0] ? ci[0]+co[0] : ci[0])) % m);

	return 0;
}


