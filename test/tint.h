#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iterator>
#include <vector>

class tint {
	typedef std::uint32_t u32;
	typedef std::uint64_t u64;
	typedef std::int64_t i64;
	typedef std::vector<u32> vu32;
	typedef vu32::iterator ivu32;
	typedef vu32::const_iterator civu32;
	typedef vu32::size_type siz;
public:
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
	tint(const vu32 &_vec) : data(_vec) {
	}
	tint(const siz _rep, const u32 val) : data(_rep, val) {
	}
	tint operator *(const tint &with) const {
		const vu32 &a = data;
		const vu32 &b = with.data;
		vu32 r(a.size() + b.size());
		u64 tmp;
		for (size_t i = 0; i < a.size(); ++i)
			for (size_t j = 0; j < b.size(); ++j) {
				tmp = (u64) a[i] * b[j];
				for (size_t k = i + j; tmp; ++k) {
					tmp += r[k];
					r[k] = (u32) tmp;
					tmp >>= 32;
				}
			}
		wrapVU32(r);
		return tint(r);
	}
	static vu32 &wrapVU32(vu32 & v) {
		while (!v.back())
			v.pop_back();
		return v;
	}
	vu32 data;
};
