#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iterator>
#include <vector>

namespace vio {

	class vuint {
		typedef std::uint32_t u32;
		typedef std::uint64_t u64;
		typedef std::int64_t i64;
		typedef std::size_t siz;
		typedef std::vector<u32> vu32;
		typedef vu32::iterator ivu32;
		typedef vu32::const_iterator civu32;
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
			return cmpVU32(data, with.data) < 0;
		}

		bool operator ==(const vuint &with) const {
			return !cmpVU32(data, with.data);
		}

		vuint operator +(const vuint &with) const {
			if (data.size() < with.data.size())
				return vuint(addVU32(with.data, data));
			else
				return vuint(addVU32(data, with.data));
		}

		vuint operator -(const vuint &with) const {
			return vuint(subVU32(data, with.data));
		}

		vuint operator *(const vuint &with) const {
			return vuint(mulVU32(data, with.data));
		}


		vuint mul(const vuint &with) const {
			return vuint(mulVU32ir(data.cbegin(), data.cend(), with.data.cbegin(), with.data.cend()));
		}

	private:
		static u64 incVU32i(ivu32 begin1, ivu32 end1, civu32 begin2, civu32 end2, const u64 carry = 0) {
			CHECK12;
			u64 tmp = carry;
			ivu32 i;
			civu32 j;
			for (i = begin1, j = begin2; j != end2; ++i, ++j) {
				tmp += (u64) *i + *j;
				*i = (u32) tmp;
				tmp >>= 32;
			}
			for (; tmp && i != end1; ++i) {
				tmp += *i;
				*i = (u32) tmp;
				tmp >>= 32;
			}
			return tmp;

		}

		static i64 decVU32i(ivu32 begin1, ivu32 end1, civu32 begin2, civu32 end2, const i64 carry = 0) {
			CHECK12;
			i64 tmp = carry;
			ivu32 i;
			civu32 j;
			for (i = begin1, j = begin2; j != end2; ++i, ++j) {
				tmp += (i64) *i - *j;
				*i = (u32) tmp;
				tmp >>= 32;
			}
			for (; tmp && i != end1; ++i) {
				tmp += *i;
				*i = (u32) tmp;
				tmp >>= 32;
			}
			return tmp;

		}
		static inline vu32 addVU32(const vu32 &vu1, const vu32 &vu2) {
			return addVU32i(vu1.cbegin(), vu1.cend(), vu2.cbegin(), vu2.cend());
		}

		static inline vu32 subVU32(const vu32 &vu1, const vu32 &vu2) {
			return wrap(subVU32i(vu1.cbegin(), vu1.cend(), vu2.cbegin(), vu2.cend()));
		}

		static inline vu32 mulVU32(const vu32 &vu1, const vu32 &vu2) {
			return mulVU32i(vu1.cbegin(), vu1.cend(), vu2.cbegin(), vu2.cend());
		}

		static vu32 addVU32i(civu32 begin1, civu32 end1, civu32 begin2, civu32 end2, const u64 carry = 0) {
			CHECK12;
			vu32 res(begin1, end1);
			u64 tmp = incVU32i(res.begin(), res.end(), begin2, end2, carry);
			if (tmp)
				res.push_back((u32) tmp);
			return res;
		}

		static vu32 subVU32i(civu32 begin1, civu32 end1, civu32 begin2, civu32 end2, const i64 carry = 0) {
			CHECK12;
			vu32 res(begin1, end1);
			u64 tmp = decVU32i(res.begin(), res.end(), begin2, end2, carry);
			if (tmp)
				throw "Unsupported";
			return res;
		}

		static vu32 mulVU32ir(civu32 begin1, civu32 end1, civu32 begin2, civu32 end2) {
			siz dis1 = end1 - begin1;
			siz dis2 = end2 - begin2;
			const u32 *a = &*begin1;
			const u32 *b = &*begin2;
			vu32 r(dis1 + dis2 + 2);
			u64 tmp = 0;
			for (siz i = 0; i < dis1; ++i)
				for (siz j = 0; j < dis2; ++j) {
					tmp = (u64) a[i] * b[j];
					for (siz k = i + j; tmp; ++k) {
						tmp += r[k];
						r[k] = (u32) tmp;
						tmp >>= 32;
					}
				}
			wrapVU32(r);
			return r;
		}

		static vu32 mulVU32i(civu32 begin1, civu32 end1, civu32 begin2, civu32 end2) {
			siz dis1 = end1 - begin1;
			siz dis2 = end2 - begin2;
			if (dis1 < 1 || dis2 < 1)
				return vu32(0);
			if (dis1 == 1)
				return mulVU32iu(begin2, end2, *begin1);
			if (dis2 == 1)
				return mulVU32iu(begin1, end1, *begin2);
			if (dis1 < 80 || dis2 < 80)
				return mulVU32ir(begin1, end1, begin2, end2);
			siz dis = ((dis1 > dis2 ? dis1 : dis2) + 1) >> 1;
			civu32 split1(begin1), split2(begin2);
			split1 += dis > dis1 ? dis1 : dis;
			split2 += dis > dis2 ? dis2 : dis;
			const vu32 &v1 = addVU32i(begin1, split1, split1, end1);
			const vu32 &v2 = addVU32i(begin2, split2, split2, end2);
			const vu32 &va = mulVU32i(begin1, split1, begin2, split2);
			vu32 vb = mulVU32i(v1.begin(), v1.end(), v2.begin(), v2.end());
			const vu32 &vc = mulVU32i(split1, end1, split2, end2);
			decVU32i(vb.begin(), vb.end(), va.begin(), va.end());
			decVU32i(vb.begin(), vb.end(), vc.begin(), vc.end());
			siz resz = va.size();
			if (resz < vb.size() + dis)
				resz = vb.size() + dis;
			if (resz < vc.size() + (dis << 1))
				resz = vc.size() + (dis << 1);
			vu32 res(resz);
			ivu32 o = res.begin();
			incVU32i(o, res.end(), va.begin(), va.end());
			o += dis;
			u64 tmp = incVU32i(o, res.end(), vb.begin(), vb.end());
			o += dis;
			tmp = incVU32i(o, res.end(), vc.begin(), vc.end(), tmp);
			while (tmp) {
				res.push_back((u32) tmp);
				tmp >>= 32;
			}
			return wrap(res);
		}

		static vu32 mulVU32iu(civu32 begin, civu32 end, const u32 val) {
			vu32 res(end - begin);
			u64 tmp = 0;
			civu32 i;
			ivu32 k;
			for (i = begin, k = res.begin(); i != end; ++i, ++k) {
				u64 mpt = (u64) *i * val;
				u64 ptm = mpt >> 32;
				tmp += (u32) mpt;
				*k = (u32) tmp;
				tmp = (tmp >> 32) + ptm;
			}
			while (tmp) {
				res.push_back((u32) tmp);
				tmp >>= 32;
			}
			return res;
		}

		static vu32 divVU32iu(civu32 begin, civu32 end, const u32 val) {
			siz dis = end - begin;
			if (!dis || (dis == 1 && *begin < val))
				return vu32();
			vu32 res(dis);
			u64 tmp = 0;
			civu32 i = end - 1;
			ivu32 k = res.end() - 1;
			for (; i >= begin; --i, --k) {
				tmp = (tmp << 32) | *i;
				*k = (u32) (tmp / val);
				tmp %= val;
			}
			return wrapVU32(res);
		}

		static int cmpVU32(const vu32 &op1, const vu32 &op2) {
			int res = (int) op1.size() - (int) op2.size();
			if (res)
				return res;
			for (vu32::const_reverse_iterator i = op1.crbegin(), j = op2.crbegin(); i != op1.crend(); ++i, ++j)
				if (*i != *j)
					return *i - *j;
			return 0;
		}

		static int cmpVU32i(civu32 begin1, civu32 end1, civu32 begin2, civu32 end2) {
			while (!*(end1 - 1))
				--end1;
			while (!*(end2 - 1))
				--end2;
			siz dis1 = end1 - begin1;
			siz dis2 = end2 - begin2;
			int res = (int) dis1 - (int) dis2;
			if (res)
				return res;
			for (civu32 i = end1 - 1, j = end2 - 1; i >= begin1; --i, --j)
				if (*i != *j)
					return *i < *j;
		}

		static vu32 wrap(vu32 v) {
			return wrapVU32(v);
		}

		static vu32 &wrapVU32(vu32 & v) {
			while (!v.back())
				v.pop_back();
			return v;
		}
	private:
		vu32 data;
		template<typename it1, typename it2>
		class MulHelper {
		public:
			MulHelper(it1 head1, siz size1, it2 head2, siz size2) : H1(head1), Z1(size1), H2(head2), Z2(size2) {
				for (; Z1 > 0 && !*(H1 + Z1 - 1); --Z1);
				for (; Z2 > 0 && !*(H2 + Z2 - 1); --Z2);
	
			}

			vu32 mul() {
				siz zax = (((Z1 > Z2 ? Z1 : Z2) + 1) >> 1) * 6;
				vu32 V(zax);
				V.resize(Z1 + Z2);
			}

		private:
			it1 H1;
			siz Z1;
			it2 H2;
			siz Z2;
		};

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

#include <ctime>

#define DECI(z, a, b) vt v##z(a, b); tt t##z(a, b);
/*
int main() {
	DECI(a, 2, 0x00000001);
	vt vc = va.div(0x3322);
	va.print("a");
	vc.print("a / x");
}
*/

int xmain() {
	DECI(a, 40001, 0xA0EA01F0);
	DECI(b, 40000, 0x2DEEEF1E);
	//vt a(3, 0x10000100);
	//vt b(2, 0xFDEEEFFE);
	//a.print("a");
	//b.print("b");
	//vt c = a + b;
	//vt d = a - b;
	std::time_t t1 = clock();
	vt ve = va * vb;
	std::time_t t2 = clock();
	vt te = va.mul(vb);
	//tt te = ta * tb;
	std::time_t t3 = clock();
	//c.print("a + b");
	//d.print("a - b");
	//e.print("a * b");
	std::printf("v: %f s\n", (double) (t2 - t1) / CLOCKS_PER_SEC);
	//ve.print("v: a * b");
	std::printf("t: %f s\n", (double) (t3 - t2) / CLOCKS_PER_SEC);
	//te.print("t: a * b");
	std::puts(ve == te ? "v equ t" : "v neq t");
	//while (true);
	return 0;
}

