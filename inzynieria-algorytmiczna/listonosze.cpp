#include <cstdio>
const unsigned MAX = 3000;

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
	void remove(ent *e) {
		if (e != first) e->data = first->data;
		first = first->next;
	}
};

struct stack {
	unsigned ents[MAX];
	unsigned begin, end;

	stack(void) : begin(0), end(0) { }
	~stack(void) { }
	
	void push(unsigned x) { ents[end++] = x; }
	unsigned pop(void) { return ents[--end]; }
	bool empty(void) const { return begin == end; }
};


// 1 <= n <= 3000
// 1 <= n <= 200 
unsigned n, entc;
stack queue;
list tree[MAX];
list::ent ents[2*MAX];

unsigned mark[MAX], marked;
unsigned val_2_2[MAX], // 2 wchodzi 2 wychodzi
				 val_2_1[MAX], // 2 wchodzi 1 wychodzi
				 val_2_0[MAX], // 2 wchodzi 0 wychodzi
				 val_1_1[MAX], // 1 wchodzi 1 wychodzi
				 val_1_0[MAX]; // 1 wchodzi 0 wychdozi

void stepbystep(unsigned id) {
	list::ent *lent;

	for (lent = tree[id].first; lent; lent = lent->next) {
		val_1_0[id] = 0;
		val_1_1[id] = 0;
		val_2_0[id] = 0;
		val_2_1[id] = 0;
		val_2_2[id] = 0;
		// costam :P //
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

	queue.push(0);

	for (unsigned i = 0; i < n; ++i) {
		unsigned it = queue.ents[i];
		mark[it] = ++marked;

		for (list::ent *lent = tree[it].first; lent; lent = lent->next)
			if (mark[lent->data]) tree[it].remove(lent);
			else queue.push(lent->data);
	}

	while (!queue.empty()) 
		stepbystep(queue.pop());

	printf("%u\n", val_2_1[0]>val_2_0[0]?val_2_1[0]:val_2_0[0]);
	return 0;
}

