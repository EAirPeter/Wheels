#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iterator>
#include <vector>

#include "vcomplex.h"

namespace vio {

	class vuint {
		typedef std::uint32_t u32;
		typedef std::uint64_t u64;
		typedef std::int64_t i64;
		typedef std::size_t siz;
		typedef std::vector<u32> vu32;
		typedef vu32::iterator ivu32;
		typedef vu32::const_iterator civu32;
		typedef vcomplex<long double> cld;
		static const siz MAGIC_NUM = 480;
	public:
		// for DEBUG
		void print(const char *const title) {
			print(title, data);
		}
		static void print(const char *const t, const vu32 &v) {
			printf("%s: 0x", t);
			if (v.empty()) {
				puts("0");
				return;
			}
			printf("%x", v.back());
			for (vu32::const_reverse_iterator i = v.crbegin() + 1; i != v.crend(); ++i)
				printf("%08x", *i);
			puts("");
		}
#		define CHECK12 assert(std::distance(begin1, end1) >= std::distance(begin2, end2))

	public:
		vuint() : data() {
		}

		vuint(const u32 _val) : data(1, _val) {
		}

		vuint(const vu32 &_vec) : data(_vec) {
		}

		vuint(const siz _rep, const u32 val) : data(_rep, val) {
		}

		bool operator <(const vuint &with) const {
			return cmp(data, with.data) < 0;
		}

		bool operator ==(const vuint &with) const {
			return !cmp(data, with.data);
		}

		vuint operator +(const vuint &with) const {
			return vuint(add(data, with.data));
		}

		vuint operator -(const vuint &with) const {
			return vuint(sub(data, with.data));
		}

		vuint operator *(const vuint &with) const {
			return vuint(mul(data, with.data));
		}

		vuint operator /(const vuint &with) const {
			return vuint(div(data, with.data));
		}

	private:
		static inline vu32 add(const vu32 &v1, const vu32 &v2) {
			if (v1.size() > v2.size()) {
				vu32 vo(v1.size() + 1);
				siz zo = add(vo.begin(), v1.cbegin(), v1.size(), v2.cbegin(), v2.size());
				vo.resize(wrap(vo.cbegin(), zo));
				return std::move(vo);
			}
			else {
				vu32 vo(v2.size() + 1);
				siz zo = add(vo.begin(), v2.cbegin(), v2.size(), v1.cbegin(), v1.size());
				vo.resize(wrap(vo.cbegin(), zo));
				return std::move(vo);
			}
		}

		static inline vu32 sub(const vu32 &v1, const vu32 &v2) {
			vu32 vo(v1.size());
			siz zo = sub(vo.begin(), v1.cbegin(), v1.size(), v2.cbegin(), v2.size());
			vo.resize(wrap(vo.cbegin(), zo));
			return std::move(vo);
		}

		static inline vu32 mul(const vu32 &v1, const vu32 &v2) {
			siz z1 = v1.size();
			siz z2 = v2.size();
			siz zo = (((wrap(v1.cbegin(), z1) > wrap(v2.cbegin(), z2) ? z1 : z2) + 1) >> 1) * 13;
			vu32 vo(zo);
			zo = mul(vo.begin(), v1.cbegin(), z1, v2.cbegin(), z2);
			vo.resize(wrap(vo.cbegin(), zo));
			return std::move(vo);
		}
		
		static inline vu32 div(const vu32 &, const vu32 &) {
			return vu32();
		}
	
		static siz mul_fft(ivu32 o, civu32 h1, siz z1, civu32 h2, siz z2) {
			wrap(h1, z1);
			wrap(h2, z2);
			ivu32 w = o;
			siz z = 1;
			while (z < z1 || z < z2)
				z <<= 1;
			z <<= 1;
			vector<cld> v1(z), v2(z);
			for (siz i = 0; i < z1; ++i)
				v1[i] = *(h1++);
			for (siz i = 0; i < z2; ++i)
				v2[i] = *(h2++);
			cld::fft(v1.begin(), z, 1);
			cld::fft(v2.begin(), z, 1);
			for (siz i = 0; i < z; ++i)
				v1[i] *= v2[i];
			cld::fft(v1.begin(), z, -1);
			u64 tmp = 0;
			for (siz i = 0; i < z; ++i) {
				tmp += (u64) (v1[i].a + 0.5);
				*(o++) = (u32) tmp;
				tmp >>= 32;
			}
			return o - w;
		}
		
		static siz mul(ivu32 o, civu32 h1, siz z1, civu32 h2, siz z2) {
			wrap(h1, z1);
			wrap(h2, z2);
			if (!z1 || !z2)
				return 0;
			if (z1 == 1)
				return mul(o, h2, z2, *h1);
			if (z2 == 1)
				return mul(o, h1, z1, *h2);
			if (z1 * z2 < MAGIC_NUM)
				return mul0(o, h1, z1, h2, z2);
			siz s = ((z1 > z2 ? z1 : z2) + 1) >> 1;
			siz i1 = s > z1 ? z1 : s;
			siz i2 = s > z2 ? z2 : s;
			ivu32 h3 = o + s * 4;
			ivu32 h4 = o + s * 5 + 1;
			ivu32 h5 = o + s * 6 + 2;
			siz s1 = mul(o, h1, i1, h2, i2);
			siz s2 = mul(o + (s << 1), h1 + i1, z1 - i1, h2 + i2, z2 - i2);
			siz s3 = add(h3, h1, i1, h1 + i1, z1 - i1);
			siz s4 = add(h4, h2, i2, h2 + i2, z2 - i2);
			siz s5 = mul(h5, h3, s3, h4, s4);
			s5 = sub(h5, h5, s5, o, s1);
			s5 = sub(h5, h5, s5, o + (s << 1), s2);
			std::fill(o + s1, o + (s << 1), 0);
			std::fill(o + s2 + (s << 1), o + (s << 2), 0);
			siz s6 = add(o + s, h5, s5);
			siz s7 = s6 > s + s2 ? s + s6 : (s << 1) + s2;
			return s7;
		}
		
		static siz mul0(ivu32 o, civu32 h1, siz z1, civu32 h2, siz z2) {
			wrap(h1, z1);
			wrap(h2, z2);
			siz z = z1 + z2 + 2;
			std::fill(o, o + z, 0);
			u64 tmp = 0;
			for (siz i = 0; i < z1; ++i) {
				for (siz j = 0; j < z2; ++j) {
					tmp = (u64) h1[i] * h2[j];
					for (siz k = i + j; tmp; ++k) {
						tmp += (u64) o[k];
						o[k] = (u32) tmp;
						tmp >>= 32;
					}
				}
			}
			wrap(o, z);
			return z;
		}
		
		static siz mul(ivu32 o, civu32 h, siz z, u32 v) {
			if (!v)
				return 0;
			u64 tmp = 0;
			ivu32 w = o;
			for (siz i = 0; i < z; ++i) {
				tmp += (u64) *(h++) * v;
				*(o++) = (u32) tmp;
				tmp >>= 32;
			}
			while (tmp) {
				*(o++) = (u32) tmp;
				tmp >>= 32;
			}
			return o - w;
		}
		
		//ASSUME: memory cleared
		static siz add(ivu32 o, civu32 h, siz z) {
			wrap(h, z);
			u64 tmp = 0;
			ivu32 w = o;
			for (siz i = 0; i < z; ++i) {
				tmp += (u64) *o + *(h++);
				*(o++) = (u32) tmp;
				tmp >>= 32;
			}
			while (tmp) {
				tmp += (u64) *o;
				*(o++) = (u32) tmp;
				tmp >>= 32;
			}
			return o - w;
		}
		
		//ASSUME: z1 >= z2
		static siz add(ivu32 o, civu32 h1, siz z1, civu32 h2, siz z2) {
			assert(z1 >= z2);
			u64 tmp = 0;
			ivu32 w = o;
			for (siz i = 0; i < z2; ++i) {
				tmp += (u64) *(h1++) + *(h2++);
				*(o++) = (u32) tmp;
				tmp >>= 32;
			}
			for (siz i = z2; i < z1; ++i) {
				tmp += (u64) *(h1++);
				*(o++) = (u32) tmp;
				tmp >>= 32;
			}
			while (tmp) {
				*(o++) = (u32) tmp;
				tmp >>= 32;
			}
			return o - w;
		}
		
		//ASSUME: h1[0, z1) >= h2[0, z2)
		static siz sub(ivu32 o, civu32 h1, siz z1, civu32 h2, siz z2) {
			assert(z1 >= z2);
			i64 tmp = 0;
			ivu32 w = o;
			for (siz i = 0; i < z2; ++i) {
				tmp += (i64) *(h1++) - *(h2++);
				*(o++) = (u32) tmp;
				tmp >>= 32;
			}
			for (siz i = z2; i < z1; ++i) {
				tmp += (i64) *(h1++);
				*(o++) = (u32) tmp;
				tmp >>= 32;
			}
			//ASSUME tmp==0
			assert(!tmp);
			while (o != w && !*(o - 1))
				--o;
			return o - w;
		}

	public:
		static int cmp(const vu32 &v1, const vu32 &v2) {
			int res = (int) v1.size() - (int) v2.size();
			if (res)
				return res;
			for (vu32::const_reverse_iterator i = v1.crbegin(), j = v2.crbegin(); i != v1.crend(); ++i, ++j)
				if (*i != *j)
					return *i - *j;
			return 0;
		}

		static int cmp(civu32 h1, siz z1, civu32 h2, siz z2) {
			wrap(h1, z1);
			wrap(h2, z2);
			int res = (int) z1 - (int) z2;
			if (res)
				return res;
			for (siz i = z1 - 1; i < z1; --i)
				if (h1[i] != h2[i])
					return h1[i] - h2[i];
		}
		static inline siz wrap(civu32 h, siz &z) {
			while (z > 0 && !h[z - 1])
				--z;
			return z;
		}
	public://private:
		vu32 data;

	};
}

#define TEST_TINT
#ifdef TEST_TINT

#include "tint.h"
typedef tint tt;
#else
typedef vio::vuint tt;
#endif
typedef vio::vuint vt;
typedef vio::vcomplex<long double> vc;
#include <cstdlib>
#include <ctime>

#define DECI(z, a, b) vt v##z(a, b); tt t##z(a, b);

inline double dur(const clock_t fr, const clock_t to) {
	return (double) (to - fr) / CLOCKS_PER_SEC;
}

inline double dur() {
	static clock_t prev = clock();
	double ret = dur(prev, clock());
	prev = clock();
	return ret;
}

int mainFFT() {
	size_t N;
	size_t T;
	std::scanf("%u%u", &N, &T);
	N = 1 << N;
	std::vector<vc> v1(N);
	for (size_t i = 0; i < N; ++i)
		v1[i] = vc(rand() & 0xffff, rand() & 0xffff);
	std::vector<vc> v2(v1);
	dur();
	for (size_t i = 0; i < T; ++i) {
		vc::fft(v1.begin(), v1.size(), 1);
		vc::fft(v1.begin(), v1.size(), -1);
	}
	double tfft = dur();
	bool s = true;
	for (size_t i = 0; i < N; ++i)
		if (v1[i] != v2[i]) {
			s = false;
			break;
		}
	printf("FFT: size=%ucmplx; rep=%utimes; time=%fs\n", N, T, tfft);
	puts(s ? "FFT SUCCEEDED" : "FFT FAILED");
	return 0;
}

int mainCALC() {
	int m, n;
	std::scanf("%d%d", &m, &n);
	DECI(a, m, 0xFFFFFFFF);
	DECI(b, n, 0xFFFFFFFF);
	//vt a(3, 0x10000100);
	//vt b(2, 0xFDEEEFFE);
	//a.print("a");
	//b.print("b");
	//vt c = a + b;
	//vt d = a - b;
	std::clock_t t1 = clock();
	vt ve = va * vb;
	std::clock_t t2 = clock();
	//vt te = va.mul(vb);
	tt te = ta * tb;
	std::clock_t t3 = clock();
	//c.print("a + b");
	//d.print("a - b");
	//e.print("a * b");
	if (m + n < 400) {
		ve.print("v: a * b");
		te.print("t: a * b");
	}
	std::printf("v: %f s\n", (double) (t2 - t1) / CLOCKS_PER_SEC);
	std::printf("t: %f s\n", (double) (t3 - t2) / CLOCKS_PER_SEC);
	std::puts(vt::cmp(ve.data, te.data) ? "v neq t" : "v eql t");
	//while (true);
	return 0;
}

int main() {
	return mainFFT();
}

