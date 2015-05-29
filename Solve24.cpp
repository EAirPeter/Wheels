#include <cmath>
#include <iostream>
#include <stack>
#include <string>

class Solve24{
public:
	std::string solve24(const int p1, const int p2, const int p3, const int p4) {
		if (!checkArg(p1))
			return "Invalid Argument(p1)";
		if (!checkArg(p2))
			return "Invalid Argument(p2)";
		if (!checkArg(p3))
			return "Invalid Argument(p3)";
		if (!checkArg(p4))
			return "Invalid Argument(p4)";
		D[1] = p1;
		D[2] = p2;
		D[4] = p3;
		D[8] = p4;
		if (dfs(0, 0, 15))
			return getMid();
		else
			return "No Solution";
	}
private:
	bool checkArg(const int x) {
		return x > 0 && x < 14;
	}
	
	bool dfs(const int d, const int e, const int s) {
		if (d == 7)
			return std::fabs(E[0] - 24) < 1e-8;
		if (e >= 2) {
			double e1 = E[e - 2];
			double e2 = E[e - 1];
			S[d] = ADD;
			E[e - 2] = e1 + e2;
			if (dfs(d + 1, e - 1, s))
				return true;
			S[d] = SUB;
			E[e - 2] = e1 - e2;
			if (dfs(d + 1, e - 1, s))
				return true;
			S[d] = MUL;
			E[e - 2] = e1 * e2;
			if (dfs(d + 1, e - 1, s))
				return true;
			S[d] = DIV;
			if (std::fabs(e1) > 1e-8) {
				E[e - 2] = e1 / e2;
				if (dfs(d + 1, e - 1, s))
					return true;
			}
			E[e - 2] = e1;
			E[e - 1] = e2;
		}
		for (int t = s; t; ) {
			int u = t & -t;
			t ^= u;
			E[e] = D[u];
			S[d] = u;
			if (dfs(d + 1, e + 1, s ^ u))
				return true;
		}
		return false;
	}
	
	std::string getMid() {
		const int *p = PRI + 4;
		std::stack<std::pair<int, std::string>> s;
		for (int e : S) {
			if (e > 0)
				s.push(std::make_pair(e, std::to_string(D[e])));
			else {
				auto e2 = s.top();
				s.pop();
				auto e1 = s.top();
				s.pop();
				std::string u;
				if (p[e1.first] < p[e])
					u += '(';
				u += e1.second;
				if (p[e1.first] < p[e])
					u += ')';
				u += OPR[e + 4];
				if (p[e2.first] < p[e] || (e == DIV && e2.first < 0) || (e == SUB && e2.first == ADD))
					u += '(';
				u += e2.second;
				if (p[e2.first] < p[e] || (e == DIV && e2.first < 0) || (e == SUB && e2.first == ADD))
					u += ')';
				s.push(std::make_pair(e, u));
			}
		}
		return s.top().second;
	}
private:
	int D[9], S[7];
	double E[4];
	static const int PRI[4];
	static const char OPR[4];
	static const int ADD = -4, SUB = -3, MUL = -2, DIV = -1;
};

const char Solve24::OPR[4] = {'+', '-', '*', '/'};
const int Solve24::PRI[4] = {1, 1, 2, 2};

int main() {
	int a, b, c, d;
	using namespace std;
	cin >> a >> b >> c >> d;
	cout << Solve24().solve24(a, b, c, d) << endl;
	return 0;
}
