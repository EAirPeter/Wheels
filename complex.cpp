#include <algorithm>
#include <cstdio>
#include <complex>
#include <ctime>
#include <iterator>
#include <vector>

using namespace std;

const size_t N = 20;
const size_t MAX_SIZE = 1 << N;
const size_t FFT_SIZE = 1 << 18;
const size_t MUL_SIZE = 1000;
const size_t RAC_TIMES = 40;
const size_t CON_TIMES = 10;

template<typename tp>
class vcompx1 {
public:
	vcompx1(const tp m = 0, const tp n = 0) : a(m), b(n) {
	}
	vcompx1(const vcompx1<tp> &v) : a(v.a), b(v.b) {
	}
	inline vcompx1<tp> operator *(const vcompx1<tp> &with) const {
		return vcompx1<tp>(a * with.a - b * with.b, a * with.b + b * with.a);
	}
	inline vcompx1<tp> operator -(const vcompx1<tp> &with) const {
		return vcompx1<tp>(a - with.a, b - with.b);
	}
	inline vcompx1<tp> &operator =(const vcompx1<tp> &with) {
		a = with.a;
		b = with.b;
		return *this;
	}
	inline vcompx1<tp> &operator *=(const vcompx1<tp> &with) {
		tp v1 = a * with.a - b * with.b;
		b = a * with.b + b * with.a;
		a = v1;
		return *this;
	}
	inline vcompx1<tp> &operator /=(const tp with) {
		a /= with;
		b /= with;
		return *this;
	}
	inline vcompx1<tp> &operator +=(const vcompx1<tp> &with) {
		a += with.a;
		b += with.b;
		return *this;
	}

	tp a, b;
};

template<typename tp>
class vcompx2 {
public:
	vcompx2(const tp m = 0, const tp n = 0) : a(m), b(n) {
	}
	vcompx2(const vcompx2<tp> &v) : a(v.a), b(v.b) {
	}
	inline vcompx2<tp> operator *(const vcompx2<tp> &with) const {
		tp v1 = a * with.a;
		tp v2 = b * with.b;
		return vcompx2<tp>(v1 - v2, (a + b) * (with.a + with.b) - v1 - v2);
	}
	inline vcompx2<tp> operator -(const vcompx2<tp> &with) const {
		return vcompx2<tp>(a - with.a, b - with.b);
	}
	inline vcompx2<tp> &operator =(const vcompx2<tp> &with) {
		a = with.a;
		b = with.b;
		return *this;
	}
	inline vcompx2<tp> &operator *=(const vcompx2<tp> &with) {
		tp v1 = a * with.a;
		tp v2 = b * with.b;
		b = (a + b) * (with.a + with.b) - v1 - v2;
		a = v1 - v2;
		return *this;
	}
	inline vcompx2<tp> &operator /=(const tp with) {
		a /= with;
		b /= with;
		return *this;
	}
	inline vcompx2<tp> &operator +=(const vcompx2<tp> &with) {
		a += with.a;
		b += with.b;
		return *this;
	}
	tp a, b;
};

typedef long double type;
typedef complex<type> cstd;
typedef vcompx1<type> cvc1;
typedef vcompx2<type> cvc2;

vector<cstd> STD(MAX_SIZE);
vector<cvc1> VC1(MAX_SIZE);
vector<cvc2> VC2(MAX_SIZE);

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
	for (size_t i = 0; i < MAX_SIZE; ++i) {
		type a = rand();
		type b = rand();
		STD[i] = cstd(a, b);
		VC1[i] = cvc1(a, b);
		VC2[i] = cvc2(a, b);
	}
}

template<typename it, typename tp>
void fft(it A, const tp &, const size_t N, const int V) {
	static const type PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679L;
	const type tmp = V * PI * 2;
	for (size_t i = 0; i < N; ++i) {
		size_t r = 0;
		for (size_t j = i, k = 1; k < N; j >>= 1, k <<= 1)
			r = (r << 1) | (j & 1);
		if (i < r)
			swap(A[i], A[r]);
	}
	for (size_t h = 2, g = 1; h <= N; h <<= 1, g <<= 1) {
		tp wn(cos(tmp / h), sin(tmp / h));
		for (size_t i = 0; i < N; i += h) {
			tp w(1, 0);
			for (size_t j = 0; j < g; ++j) {
				tp t(w * A[i | j | g]);
				A[i | j | g] = A[i | j] - t;
				A[i | j] += t;
				w *= wn;
			}
		}
	}
	if (V < 0)
		for (size_t i = 0; i < N; ++i)
			A[i] /= N;
}

void tfft() {
	dur();
	fft(STD.begin(), STD.back(), FFT_SIZE, 1);
	fft(STD.begin(), STD.back(), FFT_SIZE, -1);
	double tst = dur();
	fft(VC1.begin(), VC1.back(), FFT_SIZE, 1);
	fft(VC1.begin(), VC1.back(), FFT_SIZE, -1);
	double tv1 = dur();
	fft(VC2.begin(), VC2.back(), FFT_SIZE, 1);
	fft(VC2.begin(), VC2.back(), FFT_SIZE, -1);
	double tv2 = dur();
	p("STD", "FFT", tst);
	p("VC1", "FFT", tv1);
	p("VC2", "FFT", tv2);
}

void tcon() {
	dur();
	type a = rand();
	type b = rand();
	for (size_t i = 0; i < CON_TIMES; ++i)
		for (size_t j = 0; j < CON_TIMES; ++j) {
			cstd *st = new cstd(a, b);
			delete st;
		}
	double tst = dur();
	for (size_t i = 0; i < CON_TIMES; ++i)
		for (size_t j = 0; j < CON_TIMES; ++j) {
			cvc1 *v1 = new cvc1(a, b);
			delete v1;
		}
	double tv1 = dur();
	for (size_t i = 0; i < CON_TIMES; ++i)
		for (size_t j = 0; j < CON_TIMES; ++j) {
			cvc2 *v2 = new cvc2(a, b);
			delete v2;
		}
	double tv2 = dur();
	p("STD", "CON", tst);
	p("VC1", "CON", tv1);
	p("VC2", "CON", tv2);
}

template<typename it>
void tmul(it in, it out) {
	for (size_t i = 0; i < MUL_SIZE; ++i)
		for (size_t j = 0; j < MUL_SIZE; ++j)
			out[i + j] += in[i] * in[j];
}

void tmul() {
	dur();
	vector<cstd> *vst = new vector<cstd>(MUL_SIZE << 1);
	tmul(STD.begin(), vst->begin());
	delete vst;
	double tst = dur();
	vector<cvc1> *vv1 = new vector<cvc1>(MUL_SIZE << 1);
	tmul(VC1.begin(), vv1->begin());
	delete vv1;
	double tv1 = dur();
	vector<cvc2> *vv2 = new vector<cvc2>(MUL_SIZE << 1);
	tmul(VC2.begin(), vv2->begin());
	delete vv2;
	double tv2 = dur();
	p("STD", "MULT", tst);
	p("VC1", "MULT", tv1);
	p("VC2", "MULT", tv2);
}

size_t _RAC[RAC_TIMES];

template<typename it>
void trac(it hd) {
	for (size_t p1 : _RAC)
		for (size_t p2 : _RAC)
			for (size_t p3: _RAC) {
				auto d = hd[p1];
				hd[p1] = hd[p2];
				hd[p2] = hd[p3];
				hd[p3] = d;
			}
}

template<typename tp>
void mv(tp &a, tp &b, tp &c) {
	tp d = c;
	c = b;
	b = a;
	a = d;
}

void trac() {
	for (size_t i = 0; i < RAC_TIMES; ++i)
		_RAC[i] = static_cast<size_t>(rand()) % MAX_SIZE;
	dur();
	trac(STD.begin());
	double tst = dur();
	trac(VC1.begin());
	double tv1 = dur();
	trac(VC2.begin());
	double tv2 = dur();
	p("STD", "RACC", tst);
	p("VC1", "RACC", tv1);
	p("VC2", "RACC", tv2);
}

int main() {
	tmp = fopen("perf.tmp", "w");
	srand((unsigned int) time(nullptr));
	gen();
	tcon();
	tfft();
	tmul();
	trac();
	fclose(tmp);
	return 0;
}

