#include <algorithm>
#include <cmath>

using namespace std;

namespace vio {
	template<typename tp>
	class vcomplex {
		typedef std::size_t siz;
	public:
		vcomplex(const tp m = 0, const tp n = 0) : a(m), b(n) {
		}
		vcomplex(const vcomplex<tp> &v) : a(v.a), b(v.b) {
		}
		inline bool operator !=(const vcomplex<tp> &with) const {
			return !(*this == with);
		}

		inline bool operator ==(const vcomplex<tp> &with) const {
			return fabs(a - with.a) < 1e-8 && fabs(b - with.b) < 1e-8;
		}

		inline vcomplex<tp> operator *(const vcomplex<tp> &with) const {
			return vcomplex<tp>(a * with.a - b * with.b, a * with.b + b * with.a);
		}
		inline vcomplex<tp> operator -(const vcomplex<tp> &with) const {
			return vcomplex<tp>(a - with.a, b - with.b);
		}
		inline vcomplex<tp> &operator =(const vcomplex<tp> &with) {
			a = with.a;
			b = with.b;
			return *this;
		}
		inline vcomplex<tp> &operator *=(const vcomplex<tp> &with) {
			tp v1 = a * with.a - b * with.b;
			b = a * with.b + b * with.a;
			a = v1;
			return *this;
		}
		inline vcomplex<tp> &operator /=(const tp with) {
			a /= with;
			b /= with;
			return *this;
		}
		inline vcomplex<tp> &operator +=(const vcomplex<tp> &with) {
			a += with.a;
			b += with.b;
			return *this;
		}

		template<typename it>
		static void fft(it A, const siz N, const int V) {
			static const tp PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679L;
			const tp tmp = PI *V * 2;
			for (siz i = 0; i < N; ++i) {
				siz r = 0;
				for (siz j = i, k = 1; k < N; j >>= 1, k <<= 1)
					r = (r << 1) | (j & 1);
				if (i < r)
					swap(A[i], A[r]);
			}
			for (siz h = 2, g = 1; h <= N; h <<= 1, g <<= 1) {
				vcomplex<tp> wn(cos(tmp / h), sin(tmp / h));
				for (siz i = 0; i < N; i += h) {
					vcomplex<tp> w(1, 0);
					for (siz j = 0; j < g; ++j) {
						vcomplex<tp> t(w * A[i | j | g]);
						A[i | j | g] = A[i | j] - t;
						A[i | j] += t;
						w *= wn;
					}
				}
			}
			if (V < 0)
				for (siz i = 0; i < N; ++i)
					A[i] /= N;
		}
	private:
		tp a, b;
	public:
	};
}

