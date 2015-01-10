#include <cstdio>

using namespace std;

class test {
public:
	test() {
		printf("default: %d\n", ++tot);
	}

	test(const int y) {
		x = y;
		printf("val: %d\n", ++tot);
	}

	test(const test &n) {
		x = n.x;
		printf("copy: %d\n", ++tot);
	}

	void print() {
		printf("%d\n", x);
	}

	int x;
	static int tot;
};

test operator + (const test &a, const test &b) {
	test c;
	c.x = a.x + b.x;
	return c;
}

int test::tot = 0;

int main() {
	test a(21);
	test b(33);
	test u = a + b;
	u.print();
	return 0;
}

