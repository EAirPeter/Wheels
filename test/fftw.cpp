#include <cstdio>
#include <ctime>

#include <fftw3.h>

using namespace std;

inline double dur(const clock_t fr, const clock_t to) {
	return (double) (to - fr) / CLOCKS_PER_SEC;
}

inline double dur() {
	static clock_t prev = clock();
	double ret = (double) (clock() - prev) / CLOCKS_PER_SEC;
	prev = clock();
	return ret;
}


fftwl_complex *c1, *c2, *c3;
fftwl_plan pf, pb;

int main() {
	int N, T;
	scanf("%d%d", &N, &T);
	dur();
	c1 = (fftwl_complex *) fftwl_malloc(sizeof(fftwl_complex) * N);
	c2 = (fftwl_complex *) fftwl_malloc(sizeof(fftwl_complex) * N);
	c3 = (fftwl_complex *) fftwl_malloc(sizeof(fftwl_complex) * N);
	pf = fftwl_plan_dft_1d(N, c1, c2, FFTW_FORWARD, FFTW_ESTIMATE);
	pb = fftwl_plan_dft_1d(N, c2, c3, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftwl_execute(pf);
	fftwl_execute(pb);
	fftwl_destroy_plan(pf);
	fftwl_destroy_plan(pb);
	fftwl_free(c1);
	fftwl_free(c2);
	fftwl_free(c3);
	double tf = dur();
	printf("%dcmpx\n%dtimes\n%fsec\n", N, T, tf);
	for (int i = 0; i < N; ++i)
		if (cmp(c1[i][0], c3[i][0]) || cmp(c1[i]))
	return 0;
}
