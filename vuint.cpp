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
			puts("");
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
			return compareVU32(data, with.data) < 0;
		}

		vuint operator +(const vuint &with) const {
			if (data.size() < with.data.size())
				return vuint(plusVU32(with.data, data));
			else
				return vuint(plusVU32(data, with.data));
		}
	
		vuint operator -(const vuint &with) const {
			return vuint(minusVU32(data, with.data));
		}

		static u64 increaseVU32i(ivu32 begin1, ivu32 end1, civu32 begin2, civu32 end2, const u64 carry = 0) {
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

		static u64 decreaseVU32i(ivu32 begin1, ivu32 end1, civu32 begin2, civu32 end2, const u64 carry = 0) {
			u64 tmp = carry;
			ivu32 i;
			civu32 j;
			for (i = begin1, j = begin2; j != end2; ++i, ++j) {
				tmp += (u64) *i - *j;
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
		static inline vu32 plusVU32(const vu32 &vu1, const vu32 &vu2) {
			return plusVU32i(vu1.cbegin(), vu1.cend(), vu2.cbegin(), vu2.cend());
		}

		static inline vu32 minusVU32(const vu32 &vu1, const vu32 &vu2) {
			return minusVU32i(vu1.cbegin(), vu2.cend(), vu2.cbegin(), vu2.cend());
		}

		static vu32 plusVU32i(civu32 begin1, civu32 end1, civu32 begin2, civu32 end2) {
			vu32 res(begin1, end1);
			u64 tmp = increaseVU32i(res.begin(), res.end(), begin2, end2);
			if (tmp)
				res.push_back((u32) tmp);
			return res;
		}

		static vu32 minusVU32i(civu32 begin1, civu32 end1, civu32 begin2, civu32 end2) {
			vu32 res(begin1, end1);
			u64 tmp = decreaseVU32i(res.begin(), res.end(), begin2, end2);
			if (tmp)
				throw "Unsupported";
			return res;
		}


		static vu32 multiplyVU32i(civu32 begin1, civu32 end1, civu32 begin2, civu32 end2) {
			siz dis1 = std::distance(begin1, end1);
			siz dis2 = std::distance(begin2, end2);
			if (dis1 < 1 || dis2 < 1)
				return vu32(0);
			if (dis1 == 1)
				return multiplyVU32iu(begin2, end2, *begin1);
			if (dis2 == 1)
				return multiplyVU32iu(begin1, end1, *begin2);
			siz dis = (dis1 > dis2 ? dis1 : dis2) >> 1;
			vu32 res(dis1 + dis - 1);
			civu32 split1(begin1), split2(begin2);
			std::advance(split1, dis);
			std::advance(split2, dis);
			vu32 v1 = plusVU32i(begin1, split1, split1, end1);
			vu32 v2 = plusVU32i(begin2, split2, split2, end2);
			vu32 va = multiplyVU32i(begin1, split1, begin2, split2);
			vu32 vb = multiplyVU32i(v1.begin(), v1.end(), v2.begin(), v2.end());
			vu32 vc = multiplyVU32i(split1, end1, split2, end2);
			decreaseVU32i(vb.begin(), vb.end(), va.begin(), va.end());
			decreaseVU32i(vb.begin(), vb.end(), vc.begin(), vc.end());
			ivu32 o = res.begin();
			std::move(va.begin(), va.end(), o);
			std::advance(o, dis);
			u64 tmp = increaseVU32i(o, res.end(), vb.begin(), vb.end());
			std::advance(o, dis);
			increaseVU32i(o, res.end(), vc.begin(), vc.end(), tmp);
		}

		static vu32 multiplyVU32iu(civu32 begin, civu32 end, const u32 val) {
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

		static int compareVU32(const vu32 &op1, const vu32 &op2) {
			int res = op1.size() - op2.size();
			if (res)
				return res;
			for (vu32::const_reverse_iterator i = op1.crbegin(), j = op2.crbegin(); i != op1.crend(); ++i, ++j)
				if (*i != *j)
					return *i - *j;
			return 0;
		}

	private:
		vu32 data;
	};
}

int main() {
	typedef vio::vuint vt;
	vt a(3, 1);
	vt b(2, 2);
	a.print();
	b.print();
	vt c = a + b;
	vt d = a - b;
	c.print();
	d.print();
	return 0;
}
