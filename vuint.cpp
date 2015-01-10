#include <algorithm>
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
		typedef std::vector<u32> vu32;
		typedef vu32::iterator ivu32;
		typedef vu32::const_iterator civu32;
		typedef vu32::size_type siz;
	public:
		void print() {
			print(data);
		}
		static void print(const vu32 &v) {
			for (vu32::const_reverse_iterator i = v.crbegin(); i != v.crend(); ++i)
				printf("%08x", *i);
			puts("\npn");
		}

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

	private:
		static u64 incVU32i(ivu32 begin1, ivu32 end1, civu32 begin2, civu32 end2, const u64 carry = 0) {
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
			return std::move(addVU32i(vu1.cbegin(), vu1.cend(), vu2.cbegin(), vu2.cend()));
		}

		static inline vu32 subVU32(const vu32 &vu1, const vu32 &vu2) {
			return wrap(subVU32i(vu1.cbegin(), vu1.cend(), vu2.cbegin(), vu2.cend()));
		}

		static inline vu32 mulVU32(const vu32 &vu1, const vu32 &vu2) {
			return mulVU32i(vu1.cbegin(), vu1.cend(), vu2.cbegin(), vu2.cend());
		}

		static vu32 addVU32i(civu32 begin1, civu32 end1, civu32 begin2, civu32 end2, const u64 carry = 0) {
			vu32 res(begin1, end1);
			u64 tmp = incVU32i(res.begin(), res.end(), begin2, end2, carry);
			if (tmp)
				res.push_back((u32) tmp);
			return std::move(res);
		}

		static vu32 subVU32i(civu32 begin1, civu32 end1, civu32 begin2, civu32 end2, const i64 carry = 0) {
			vu32 res(begin1, end1);
			u64 tmp = decVU32i(res.begin(), res.end(), begin2, end2, carry);
			if (tmp)
				throw "Unsupported";
			return res;
		}


		static vu32 mulVU32i(civu32 begin1, civu32 end1, civu32 begin2, civu32 end2) {
			siz dis1 = std::distance(begin1, end1);
			siz dis2 = std::distance(begin2, end2);
			if (dis1 < 1 || dis2 < 1)
				return vu32(0);
			if (dis1 == 1)
				return mulVU32iu(begin2, end2, *begin1);
			if (dis2 == 1)
				return mulVU32iu(begin1, end1, *begin2);
			siz dis = (dis1 > dis2 ? dis1 : dis2) >> 1;
			vu32 res(dis1 + dis2 - 1);
			civu32 split1(begin1), split2(begin2);
			std::advance(split1, dis);
			std::advance(split2, dis);
			const vu32 &&v1 = addVU32i(begin1, split1, split1, end1);
			printf("v1: ");print(v1);
			const vu32 &&v2 = addVU32i(begin2, split2, split2, end2);
			printf("v2: ");print(v2);
			puts("calcva");
			const vu32 &&va = mulVU32i(begin1, split1, begin2, split2);
			printf("va: ");print(va);
			puts("calcvb");
			vu32 vb = mulVU32i(v1.begin(), v1.end(), v2.begin(), v2.end());
			printf("vb: ");print(vb);
			puts("calcvc");
			const vu32 &&vc = mulVU32i(split1, end1, split2, end2);
			printf("vc: ");print(vc);
			puts("decvc1");
			decVU32i(vb.begin(), vb.end(), va.begin(), va.end());
			puts("deccvc2");
			decVU32i(vb.begin(), vb.end(), vc.begin(), vc.end());
			printf("vb: ");print(vb);
			ivu32 o = res.begin();
			incVU32i(o, res.end(), va.begin(), va.end());
			printf("res1: ");print(res);
			std::advance(o, dis);
			u64 tmp = incVU32i(o, res.end(), vb.begin(), vb.end());
			printf("res2: ");print(res);
			std::advance(o, dis);
			tmp = incVU32i(o, res.end(), vc.begin(), vc.end(), tmp);
			printf("res3: ");print(res);
			while (tmp) {
				res.push_back((u32) tmp);
				tmp >>= 32;
			}
			printf("res: ");print(res);
			return res;
		}

		static vu32 mulVU32iu(civu32 begin, civu32 end, const u32 val) {
			vu32 res(std::distance(begin, end));
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

		static int cmpVU32(const vu32 &op1, const vu32 &op2) {
			int res = op1.size() - op2.size();
			if (res)
				return res;
			for (vu32::const_reverse_iterator i = op1.crbegin(), j = op2.crbegin(); i != op1.crend(); ++i, ++j)
				if (*i != *j)
					return *i - *j;
			return 0;
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
	};
}

typedef vio::vuint vt;
vt a(2, 0xfafdfdfd);
vt b(2, 0x324895ee);

int main() {
	printf("a: ");a.print();
	printf("b: ");b.print();
	vt c = a + b;
	vt d = a - b;
	vt e = a * b;
	printf("a + b: ");c.print();
	printf("a - b: ");d.print();
	printf("a * b: ");e.print();
	return 0;
}

