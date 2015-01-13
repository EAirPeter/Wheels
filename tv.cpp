#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iterator>
#include <vector>

template<typename vu32>
class tint {
	typedef std::uint32_t u32;
	typedef std::uint64_t u64;
	typedef std::int64_t i64;
	typedef typename vu32::iterator ivu32;
	typedef typename vu32::const_iterator civu32;
	typedef std::size_t siz;
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
		for (typename vu32::const_reverse_iterator i = v.crbegin() + 1; i != v.crend(); ++i)
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
private:
	vu32 data;
};

int main() {
	tint<std::vector<unsigned int>> v1(1000, 0x12312323);
	tint<std::vector<unsigned int>> v2(1000, 0x12312323);
	tint<std::array<unsigned int>> v2(1000, 0x12312323);
	tint<std::array<unsigned int>> v2(1000, 0x12312323);
	return 0;
}
