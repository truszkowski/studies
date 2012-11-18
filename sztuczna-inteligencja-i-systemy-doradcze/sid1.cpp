/* Piotr Truszkowski */
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <queue>
#include <math.h>
#include <sys/time.h>

#define REP(i,n) for (i = 0; i < n; ++i)
#define REP2(i,n1,n2) for (i = n1; i < n2; ++i)
#define MIN(a,b) ((a) < (b)) ? (a) : (b)
#define REALLY_NOT_MIN 10000
#define square(x) ((x) * (x))
#define XDISTANCE(a,b) ((a).first - (b).first)
#define YDISTANCE(a,b) ((a).second - (b).second)
#define DISTANCE(a,b) sqrt((double) square((a).first - (b).first) + square((a).second - (b).second))

using namespace std;

inline unsigned long getTime(void)  {
	struct timeval tv; struct timezone tz;
	gettimeofday(&tv, &tz);
	return (tv.tv_sec*1000 + tv.tv_usec/1000);
}

typedef float float_T;
typedef int int_T;

class Circle { // kolko, potrafi samo sprawdzic czy jakis punkt jest wewnatrz niego.
	public:
		pair<float_T, float_T> p;
		float_T r;
		Circle(void) {}
		virtual ~Circle(void) {}
		virtual bool include(pair<float_T, float_T> xy) { return r >= DISTANCE(xy, p); }
};

class Cell { // komorka, zna swoja pozycje oraz pozycje poprzednio odwiedzonej komórki.
	private:
		float_T howFar;
		pair<int_T, int_T> previous;
		pair<int_T, int_T> position;		
	public:
		Cell(pair<int_T, int_T> pos, float_T hf) { position=pos; howFar=hf; }
		Cell(pair<int_T, int_T> pos, pair<int_T, int_T> prev, float_T hf) { position=pos; howFar=hf; previous = prev; }
		virtual ~Cell(void) {}
		friend bool operator<(const class Cell, const class Cell);
		virtual float_T getDistance(void) { return howFar; }
		virtual pair<int_T, int_T> getPosition(void) { return position; }
		virtual pair<int_T, int_T> getPrevious(void) { return previous; }
		virtual vector<pair<int_T, int_T> > neightbours(void) { 
			vector<pair<int_T,int_T> > vn; 
			vn.push_back(make_pair(position.first + 1,position.second)); vn.push_back(make_pair(position.first,position.second + 1)); 
			vn.push_back(make_pair(position.first - 1,position.second)); vn.push_back(make_pair(position.first,position.second - 1));
			return vn;
		}
};

inline bool operator<(const class Cell a, const class Cell b) {
	return a.howFar > b.howFar;
}

class Grid { // siatka, generuje uklad komorek i znajduje droge(ciag komorek) miedzy dwiema wskazanymi punktami.
	private:
		pair<float_T, float_T> startPoint, endPoint, leftDown;
		pair<int_T, int_T> startCell, endCell;
		float_T step;
		priority_queue<class Cell> cellQueue;
		map<pair<int_T, int_T>, pair<int_T, int_T> > dirtyCells;
		map<pair<int_T, int_T>, pair<int_T, int_T> >::iterator iter; // wrr......!!
		vector<class Circle> circles;
		vector<pair<int_T, int_T> > trace;
		unsigned long timeOut;
	public:
		Grid(void) { step = -1; timeOut = 60000; } // najwyzej minute moze trwac przeszukiwanie
		virtual ~Grid(void) { reload(); }
		virtual void setStep(float_T s) { step = s; }
		virtual void setTimeOut(unsigned long to) { timeOut = to; }
		virtual void load(const char * fn) { loadFile(fn); set(); }
		virtual void reload(void) { /*cellQueue.clear(); dirtyCells.clear(); trace.clear();*/ }
		virtual void solve(void) { search(); result(); }
	protected:
		virtual void loadFile(const char *); // wczytuje plik z danymi
		virtual void set(void); // ustawia parametry siatki
		virtual pair<int_T, int_T> xy2ij(pair<float_T, float_T> pf) { // konwerter z ukladu float_T na int_T
			return make_pair((int_T) ceil((pf.first - leftDown.first)/step), (int_T) ceil((pf.second - leftDown.second)/step));
		}
		virtual void addCircle(class Circle);
		virtual void search(void);
		virtual void result(void);
};

void Grid::loadFile(const char * fn) {
	fstream f(fn, fstream::in);
	unsigned int count, i;
	f >> startPoint.first >> startPoint.second >> endPoint.first >> endPoint.second >> count;
	circles.resize(count);
	REP(i, count) f >> circles[i].p.first >> circles[i].p.second >> circles[i].r; 
	f.close();
}

void Grid::set(void) {
	int_T i;
	float_T xMin = MIN(startPoint.first, endPoint.first);
	float_T yMin = MIN(startPoint.second, endPoint.second);
	float_T rMin = REALLY_NOT_MIN, xMinTemp, yMinTemp;
	REP(i, (int_T) circles.size()) { // okreslamy parametry siatki...	
		xMinTemp = circles[i].p.first - circles[i].r; yMinTemp = circles[i].p.second - circles[i].r;
		xMin = MIN(xMin, xMinTemp); yMin = MIN(yMin, yMinTemp);
		if (step == -1) rMin = MIN(rMin, circles[i].r);
	}
	if (step == -1) step = rMin; // ustalamy rozmiar komórki na wielkość najmniejszego promienia, 
	leftDown.first = xMin; leftDown.second = yMin; // okreslamy lewy dolny rog siatki, 
	REP(i, (int_T) circles.size()) addCircle(circles[i]); // do siatki wrzucamy kółka, 
	startCell = xy2ij(startPoint); endCell = xy2ij(endPoint); // gdzie na siatce jest start i koniec, 
	circles.clear(); // na koniec czyscimy vector kółek, niebedzie juz potrzebny. 
	cout << "Grid begin at: (" << leftDown.first << "," << leftDown.second << ") step: " << step << endl;
	cout << "Start cell: (" << startCell.first << "," << startCell.second << ") End cell: (" << endCell.first << "," << endCell.second << ")" << endl;
	cout << "Timeout: " << timeOut << "ms" << endl;
}

void Grid::addCircle(class Circle c) {
	int_T i, j; float_T xf = XDISTANCE(c.p, leftDown), yf = YDISTANCE(c.p, leftDown);
	REP2(i, (int_T) ceil((xf - c.r)/step), (int_T) ceil((xf + c.r)/step)) REP2(j, (int_T) ceil((yf - c.r)/step), (int_T) ceil((yf + c.r)/step)) {
		pair<float_T, float_T> ld = make_pair(leftDown.first + i*step, leftDown.second + j*step);
		pair<float_T, float_T> rd = make_pair(leftDown.first + (i+1)*step, leftDown.second + j*step);
		pair<float_T, float_T> lu = make_pair(leftDown.first + i*step, leftDown.second + (j+1)*step);
		pair<float_T, float_T> ru = make_pair(leftDown.first + (i+1)*step, leftDown.second + (j+1)*step);
		unsigned int count = 0;
		if (c.include(ld)) count++;
		if (c.include(lu)) count++;
		if (c.include(rd)) count++;
		if (c.include(ru)) count++;
		if (count == 4) dirtyCells[make_pair(i,j)] = make_pair(i,j); // sytuacja beznadziejna
		if (count == 3) dirtyCells[make_pair(i,j)] = make_pair(i,j); // dwie sciany sa wolne
		if (count == 2) dirtyCells[make_pair(i,j)] = make_pair(i,j); // trzy sciany sa wolne (tak naprawde niekoniecznie...)
	}
}

void Grid::search(void) { // przeszukiwanie A*...
	cout << "\nSearching..." << endl;
	class Cell sc(startCell, DISTANCE(startCell, endCell));
	cellQueue.push(sc);
	unsigned long timeStart = getTime(), count = 0;
	while(!cellQueue.empty()) {// niema gdzie pojsc...
		if ((getTime() - timeStart) > timeOut) { cout << "\n...TimeOut (visited: " << count << ")!" << endl; exit(0); }
		class Cell act = cellQueue.top(); cellQueue.pop();
		pair<int_T, int_T> actpos = act.getPosition();
		cout << "\rtime: " << (getTime() - timeStart) << "ms    visited: " << ++count << "    distance: " << act.getDistance()*step << "                 ";
		if (actpos == endCell) {// oto koniec!
			cout << "\n\nTrace has been found (time: " << (getTime() - timeStart) << "ms, visited: " << count << ")!" << endl;
			dirtyCells[actpos] = act.getPrevious();
			while (actpos!=startCell) {
				trace.push_back(actpos); actpos=dirtyCells[actpos];
			}
			return;
		}
		vector<pair<int_T, int_T> > vn = act.neightbours();
		int_T i; REP(i,(int_T) vn.size()) // trzeba dodac sasiadow
			if (dirtyCells.find(vn[i]) == dirtyCells.end()) { // o ile nie sa 'brudni'...
				class Cell nxt(vn[i], actpos, DISTANCE(vn[i], endCell)); cellQueue.push(nxt);
			}
		dirtyCells[actpos] = act.getPrevious(); // na koniec 'brudzimy' odwiedzonego.
	}
	cout << "\n\nTrace doesn't exist (" << (getTime() - timeStart) << "ms, " << count << "visited)!" << endl;
}

void Grid::result(void) {
	int_T i;
	cout << "Start> x: " << startCell.first << " y: " << startCell.second << endl;
	for (i = trace.size(); i > 0; --i) cout << " |     x: " << trace[i].first << " y: " << trace[i].second << endl;
	cout << "End>   x: " << endCell.first << " y: " << endCell.second << endl;
	cout << "Length: " << (trace.size() * step) << endl;
}

int main(int argc, char * argv[]) {
	Grid grid;
	if (argc > 1) {
		if (argc > 2) grid.setStep(atof(argv[2]));
		if (argc > 3) grid.setTimeOut(atoi(argv[3]));
		grid.load(argv[1]);
		grid.solve();
	} else
		cout << "Usage: " << argv[0] << " <file> { <step> <timeout(ms)> }" << endl;
	return 0;
}
