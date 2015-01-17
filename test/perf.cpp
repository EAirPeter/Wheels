#include <algorithm>
#include <array>
#include <cstdio>
#include <ctime>
#include <iterator>
#include <string>
#include <vector>

using namespace std;

const size_t MAX_SIZE = 1000000;
const size_t SORT_SIZE = 1000000;
const size_t MUL_SIZE = 10000;
const size_t RAC_TIMES = 400;
const size_t CON_TIMES = 100;

typedef int type;

vector<type> *pvec = new vector<type>(MAX_SIZE);
array<type, MAX_SIZE> *parr = new array<type, MAX_SIZE>();
basic_string<type> *pstr = new basic_string<type>(MAX_SIZE, type());
type *praw = new type[MAX_SIZE];

vector<type> &VEC = *pvec;
array<type, MAX_SIZE> &ARR = *parr;
basic_string<type> &STR = *pstr;
type *RAW = praw;

FILE *tmp;

inline double dur(const clock_t fr, const clock_t to) {
	return (double) (to - fr) / CLOCKS_PER_SEC;
}

inline double dur() {
	static clock_t prev = clock();
	double ret = (double) (clock() - prev) / CLOCKS_PER_SEC;
	prev = clock();
	return ret;
}

inline void rx(const type &u) {
	fprintf(tmp, "%d", (int) u);
}

void p(const char *const cat, const char *const inf, const double tim) {
	printf("[%s] %s: %f s\n", cat, inf, tim);
}

void gen() {
	for (size_t i = 0; i < MAX_SIZE; ++i)
		RAW[i] = static_cast<type>(rand());
	dur();
	copy(RAW, RAW + MAX_SIZE, VEC.begin());
	double tv = dur();
	copy(RAW, RAW + MAX_SIZE, ARR.begin());
	double ta = dur();
	copy(RAW, RAW + MAX_SIZE, STR.begin());
	double ts = dur();
	p("VEC", "COPY", tv);
	p("ARR", "COPY", ta);
	p("STR", "COPY", ts);
}

void tcon() {
	dur();
	for (size_t i = 0; i < CON_TIMES; ++i) {
		type *r = new type[MAX_SIZE];
		copy(RAW, RAW + MAX_SIZE, r);
		rx(r[0]);
		delete[] r;
	}
	double tr = dur();
	for (size_t i = 0; i < CON_TIMES; ++i) {
		vector<type> *v = new vector<type>(RAW, RAW + MAX_SIZE);
		rx(*v->begin());
		delete v;
	}
	double tv = dur();
	for (size_t i = 0; i < CON_TIMES; ++i) {
		array<type, MAX_SIZE> *a = new array<type, MAX_SIZE>();
		copy(RAW, RAW + MAX_SIZE, a->data());
		rx(*a->begin());
		delete a;
	}
	double ta = dur();
	for (size_t i = 0; i < CON_TIMES; ++i) {
		basic_string<type> *s = new basic_string<type>(RAW, RAW + MAX_SIZE);
		rx(*s->begin());
		delete s;
	}
	double ts = dur();
	p("RAW", "CON", tr);
	p("VEC", "CON", tv);
	p("ARR", "CON", ta);
	p("STR", "CON", ts);
}

void tsort() {
	dur();
	sort(RAW, RAW + SORT_SIZE);
	double tr = dur();
	sort(VEC.begin(), VEC.begin() + SORT_SIZE);
	double tv = dur();
	sort(ARR.begin(), ARR.begin() + SORT_SIZE);
	double ta = dur();
	sort(STR.begin(), STR.begin() + SORT_SIZE);
	double ts = dur();
	p("RAW", "SORT", tr);
	p("VEC", "SORT", tv);
	p("ARR", "SORT", ta);
	p("STR", "SORT", ts);
}

template<typename it>
void tmul(it in, it out) {
	for (size_t i = 0; i < MUL_SIZE; ++i)
		for (size_t j = 0; j < MUL_SIZE; ++j)
			out[i + j] += in[i] * in[j];
}

void tmul() {
	dur();
	type *r = new type[MUL_SIZE << 1] {};
	tmul(RAW, r);
	rx(r[0]);
	delete[] r;
	double tr = dur();
	vector<type> *v = new vector<type>(MUL_SIZE << 1);
	tmul(VEC.begin(), v->begin());
	rx(*v->begin());
	delete v;
	double tv = dur();
	array<type, MUL_SIZE << 1> *a = new array<type, MUL_SIZE << 1>();
	tmul(ARR.begin(), a->begin());
	rx(*a->begin());
	delete a;
	double ta = dur();
	basic_string<type> *s = new basic_string<type>(MUL_SIZE << 1, type());
	tmul(STR.begin(), s->begin());
	rx(*s->begin());
	delete s;
	double ts = dur();
	p("RAW", "MULT", tr);
	p("VEC", "MULT", tv);
	p("ARR", "MULT", ta);
	p("STR", "MULT", ts);
}

size_t _RAC[RAC_TIMES];

template<typename it>
void trac(it hd, void (*fun)(type &, type &, type &)) {
	for (size_t p1 : _RAC)
		for (size_t p2 : _RAC)
			for (size_t p3: _RAC)
				fun(*(hd + p1), *(hd + p2), *(hd + p3));
}

void mv(type &a, type &b, type &c) {
	type d = c;
	c = b;
	b = a;
	a = d;
}

void trac() {
	void (*fun)(type &, type &, type &) = mv;
	for (size_t i = 0; i < RAC_TIMES; ++i)
		_RAC[i] = static_cast<size_t>(rand()) % MAX_SIZE;
	dur();
	trac(RAW, fun);
	double tr = dur();
	trac(VEC.begin(), fun);
	double tv = dur();
	trac(ARR.begin(), fun);
	double ta = dur();
	trac(STR.begin(), fun);
	double ts = dur();
	p("RAW", "RACC", tr);
	p("VEC", "RACC", tv);
	p("ARR", "RACC", ta);
	p("STR", "RACC", ts);
}

int main() {
	tmp = fopen("perf.tmp", "w");
	srand((unsigned int) time(nullptr));
	gen();
	tcon();
	tsort();
	tmul();
	trac();
	fclose(tmp);
	return 0;
}

