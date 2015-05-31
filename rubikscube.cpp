#include <cctype>
#include <iostream>
#include <string>
#include <vector>

enum Face {
	F, L, R, B, U, D
};

Face getFaceFromChar(const char c) {
	switch (c) {
	case 'F':
		return Face::F;
	case 'L':
		return Face::L;
	case 'R':
		return Face::R;
	case 'B':
		return Face::B;
	case 'U':
		return Face::U;
	case 'D':
		return Face::D;
	}
	throw "FAILED: getFaceFromChar";
}

char getCharFromFace(const Face f) {
	switch (f) {
	case F:
		return 'F';
	case L:
		return 'L';
	case R:
		return 'R';
	case B:
		return 'B';
	case U:
		return 'U';
	case D:
		return 'D';
	}
	throw "FAILED: getCharFromFace";
}

class View {
	friend class RubiksCube;
private:
	//NOCHECK "(Face)(Rotate)"
	View(const char *s) {
		face = getFaceFromChar(s[0]);
		rota = s[1] & 0x0f;
	}
public:
	Face face;
	int rota;
};

class Step {
public:
	Step(const Face pFace, const bool pLayer, const unsigned int pTime) : face(pFace), layer(pLayer), time(pTime) {
	}

	bool combine(const Step &step) {
		if (step.face == face && step.layer == layer) {
			time = (time + step.time) & 0x03;
			return true;
		}
		return false;
	}

public:
	Face face;
	bool layer;
	unsigned int time;
};

class RubiksCube {
public:
	RubiksCube(const char cube[6][12]) : view("F0"), sol() {
		for (auto i = 0; i < 6; ++i) {
			colo[i] = cube[i][4];
			for (auto j = 0; j < 9; ++j)
				data[i][j] = cube[i][j];
		}
	}
	
	std::string solve() {
		sol.clear();
		view = "F0";

		//1.1
		for (int i = 0; i < 4; ++i) {
			View v = view;
			sol11();
			view = v;
			change("R0");
		}

		//1.2
		for (int i = 0; i < 4; ++i) {
			View v = view;
			sol12();
			view = v;
			change("R0");
		}

		//1.3
		while (col(F) != get(F, 4))
			rotCW(D, true);
		change("F2");
		
		//2.1
		for (int i = 0; i < 4; ++i) {
			View v = view;
			sol21();
			view = v;
			change("R0");
		}

		//3.1
		sol31();

		return getString();
	}

private:
	void sol11() {
		char f = col(F);
		char u = col(U);
		if (m0(get(U, 7), get(F, 1), u, f))
			return;
		if (m0(get(U, 7), get(F, 1), f, u)) {
			rotDB(F);
			rotCW(D);
			rotCW(R);
			rotCC(F);
			rotCC(R);
			return;
		}
		if (m1(get(U, 3), get(L, 1), u, f))
			rotDB(L);
		if (m1(get(U, 5), get(R, 1), u, f))
			rotDB(R);
		if (m1(get(U, 1), get(B, 1), u, f))
			rotDB(B);
		if (m0(get(D, 1), get(F, 7), f, u)) {
			rotCW(D);
			rotCW(R);
			rotCC(F);
			rotCC(R);
			return;
		}
		if (m0(get(D, 3), get(L, 7), f, u)) {
			rotCC(L);
			rotCW(F);
			rotCW(L);
			return;
		}
		if (m0(get(D, 5), get(R, 7), f, u)) {
			rotCW(R);
			rotCC(F);
			rotCC(R);
			return;
		}
		if (m0(get(D, 7), get(B, 7), f, u)) {
			rotCW(D);
			rotCC(L);
			rotCW(F);
			rotCW(L);
			return;
		}
		if (m0(get(D, 1), get(F, 7), u, f)) {
			rotDB(F);
			return;
		}
		if (m0(get(D, 3), get(L, 7), u, f)) {
			rotCW(D);
			rotDB(F);
			return;
		}
		if (m0(get(D, 5), get(R, 7), u, f)) {
			rotCC(D);
			rotDB(F);
			return;
		}
		if (m0(get(D, 7), get(B, 7), u, f)) {
			rotDB(D);
			rotDB(F);
			return;
		}
		for (int i = 0; i < 4; ++i) {
			if (m0(get(F, 3), get(L, 5), f, u)) {
				rotCW(F);
				return;
			}
			if (m0(get(F, 5), get(R, 3), f, u)) {
				rotCC(F);
				return;
			}
			rotCW(D, true);
		}
		throw "FAILED: sol11";
	}

	void sol12() {
		char f = col(F);
		char u = col(U);
		char r = col(R);
		if (m0(get(F, 2), get(U, 8), get(R, 0), f, u, r))
			return;
		View v = view;
		for (int i = 0; i < 4; ++i) {
			if (m0(get(R, 0), get(U, 8), get(F, 2), u, f, r)) {
				rotCC(R);
				rotCC(D);
				rotCW(R);
				break;
			}
			if (m0(get(F, 2), get(R, 0), get(U, 8), u, f, r)) {
				rotCW(F);
				rotCW(D);
				rotCC(F);
				break;
			}
			if (m0(get(U, 8), get(F, 2), get(R, 0), u, f, r)) {
				rotCW(F);
				rotCW(D);
				rotCC(F);
				break;
			}
			change("R0");
		}
		view = v;
		for (int i = 0; i < 4; ++i) {
			if (m0(get(R, 6), get(F, 8), get(D, 2), u, f, r)) {
				rotCC(R);
				rotCC(D);
				rotCW(R);
				return;
			}
			if (m0(get(F, 8), get(D, 2), get(R, 6), u, f, r)) {
				rotCW(F);
				rotCW(D);
				rotCC(F);
				return;
			}
			if (m0(get(D, 2), get(R, 6), get(F, 8), u, f, r)) {
				rotCC(R);
				rotDB(D);
				rotCW(R);
				rotCW(D);
				rotCC(R);
				rotCC(D);
				rotCW(R);
				return;
			}
			rotCW(D, true);
		}
		throw "FAILED: sol12";
	}

	void sol2a() {
		rotCW(R);
		rotCC(U);
		rotCC(R);
		rotCC(U);
		rotCC(F);
		rotCW(U);
		rotCW(F);
	}

	void sol2b() {
		rotCC(F);
		rotCW(U);
		rotCW(F);
		rotCW(U);
		rotCW(R);
		rotCC(U);
		rotCC(R);
	}

	void sol21() {
		char f = col(F);
		char r = col(R);
		if (m0(get(F, 5), get(R, 3), f, r))
			return;
		View v = view;
		for (int i = 0; i < 4; ++i) {
			if (m1(get(F, 5), get(R, 3), r, f)) {
				sol2a();
				rotCC(U);
				sol2b();
				break;
			}
			change("R0");
		}
		view = v;
		for (int i = 0; i < 4; ++i) {
			if (m0(get(L, 1), get(U, 3), f, r))
				return sol2a();
			if (m0(get(U, 1), get(B, 1), f, r))
				return sol2b();
			rotCW(U);
		}
		throw "FAILED: sol21";
	}

	void sol3a() {
		rotCC(F);
		rotCC(U);
		rotCC(L);
		rotCW(U);
		rotCW(L);
		rotCW(F);
	}

	void sol31() {
		char u = col(U);
		if (m3(get(U, 3), get(U, 4), get(U, 5), u) && m2(get(U, 1), get(U, 7), u))
			change("R0");
		if (m3(get(U, 1), get(U, 4), get(U, 7), u) && m2(get(U, 3), get(U, 5), u))
			sol3a();
		for (int i = 0; i < 4; ++i) {

		}
	}

	bool m0(const char a0, const char a1, const char b0, const char b1) {
		return a0 == b0 && a1 == b1;
	}

	bool m0(const char a0, const char a1, const char a2, const char b0, const char b1, const char b2) {
		return a0 == b0 && a1 == b1 && a2 == b2;
	}

	bool m1(const char a0, const char a1, const char b0, const char b1) {
		return (a0 == b0 && a1 == b1) || (a0 == b1 && a1 == b0);
	}

	bool m2(const char c0, const char c1, const char d) {
		return c0 != d && c1 != d;
	}

	bool m3(const char c0, const char c1, const char c2, const char d) {
		return c0 == d && c1 == d && c2 == d;
	}

private:
	void rotDB(const Face face, const bool layer = false) {
		rotCW(face, layer);
		rotCW(face, layer);
	}

	void rotCW(const Face face, const bool layer = false) {
		rotAbsCW(curViews()[face].face, layer);
	}

	void rotCC(const Face face, const bool layer = false) {
		rotAbsCC(curViews()[face].face, layer);
	}

	void rotAbsCW(const Face face, const bool layer) {
		add(Step(face, layer, 1));
		rotPatCW(face);
		View vL = VIEW[face][0][L];
		View vU = VIEW[face][0][U];
		View vR = VIEW[face][0][R];
		View vD = VIEW[face][0][D];
		rotPatEdge(
			data[vL.face][ROTA[vL.rota][8]], data[vL.face][ROTA[vL.rota][5]], data[vL.face][ROTA[vL.rota][2]],
			data[vU.face][ROTA[vU.rota][6]], data[vU.face][ROTA[vU.rota][7]], data[vU.face][ROTA[vU.rota][8]],
			data[vR.face][ROTA[vR.rota][0]], data[vR.face][ROTA[vR.rota][3]], data[vR.face][ROTA[vR.rota][6]],
			data[vD.face][ROTA[vD.rota][2]], data[vD.face][ROTA[vD.rota][1]], data[vD.face][ROTA[vD.rota][0]]
		);
		if (layer)
			rotPatEdge(
				data[vL.face][ROTA[vL.rota][7]], data[vL.face][ROTA[vL.rota][4]], data[vL.face][ROTA[vL.rota][1]],
				data[vU.face][ROTA[vU.rota][3]], data[vU.face][ROTA[vU.rota][4]], data[vU.face][ROTA[vU.rota][5]],
				data[vR.face][ROTA[vR.rota][1]], data[vR.face][ROTA[vR.rota][4]], data[vR.face][ROTA[vR.rota][7]],
				data[vD.face][ROTA[vD.rota][5]], data[vD.face][ROTA[vD.rota][4]], data[vD.face][ROTA[vD.rota][3]]
			);
	}

	void rotAbsCC(const Face face, const bool layer) {
		add(Step(face, layer, 3));
		rotPatCC(face);
		View vL = VIEW[face][0][L];
		View vD = VIEW[face][0][D];
		View vR = VIEW[face][0][R];
		View vU = VIEW[face][0][U];
		rotPatEdge(
			data[vL.face][ROTA[vL.rota][8]], data[vL.face][ROTA[vL.rota][5]], data[vL.face][ROTA[vL.rota][2]],
			data[vD.face][ROTA[vD.rota][2]], data[vD.face][ROTA[vD.rota][1]], data[vD.face][ROTA[vD.rota][0]],
			data[vR.face][ROTA[vR.rota][0]], data[vR.face][ROTA[vR.rota][3]], data[vR.face][ROTA[vR.rota][6]],
			data[vU.face][ROTA[vU.rota][6]], data[vU.face][ROTA[vU.rota][7]], data[vU.face][ROTA[vU.rota][8]]
		);
		if (layer)
			rotPatEdge(
				data[vL.face][ROTA[vL.rota][7]], data[vL.face][ROTA[vL.rota][4]], data[vL.face][ROTA[vL.rota][1]],
				data[vD.face][ROTA[vD.rota][5]], data[vD.face][ROTA[vD.rota][4]], data[vD.face][ROTA[vD.rota][3]],
				data[vR.face][ROTA[vR.rota][1]], data[vR.face][ROTA[vR.rota][4]], data[vR.face][ROTA[vR.rota][7]],
				data[vU.face][ROTA[vU.rota][3]], data[vU.face][ROTA[vU.rota][4]], data[vU.face][ROTA[vU.rota][5]]
			);
	}

	// A0 A1 A2    D0 D1 D2
	// B0 B1 B2    A0 A1 A2
	// C0 C1 C2    B0 B1 B2
	// D0 D1 D2    C0 C1 C2
	void rotPatEdge(char &a0, char &a1, char &a2, char &b0, char &b1, char &b2, char &c0, char &c1, char &c2, char &d0, char &d1, char &d2) {
		char t0 = a0, t1 = a1, t2 = a2;
		a0 = d0, a1 = d1, a2 = d2;
		d0 = c0, d1 = c1, d2 = c2;
		c0 = b0, c1 = b1, c2 = b2;
		b0 = t0, b1 = t1, b2 = t2;
	}

	// 012    630
	// 345 -> 741
	// 678    852
	void rotPatCW(char &c0, char &c1, char &c2, char &c3, char &c4, char &c5, char &c6, char &c7, char &c8) {
		char t = c0;
		c0 = c6;
		c6 = c8;
		c8 = c2;
		c2 = t;
		t = c1;
		c1 = c3;
		c3 = c7;
		c7 = c5;
		c5 = t;
	}
	void rotPatCW(const Face face) {
		rotPatCW(data[face][0], data[face][1], data[face][2], data[face][3], data[face][4], data[face][5], data[face][6], data[face][7], data[face][8]);
	}

	// 012    258
	// 345 -> 147
	// 678    036
	void rotPatCC(char &c0, char &c1, char &c2, char &c3, char &c4, char &c5, char &c6, char &c7, char &c8) {
		char t = c0;
		c0 = c2;
		c2 = c8;
		c8 = c6;
		c6 = t;
		t = c1;
		c1 = c5;
		c5 = c7;
		c7 = c3;
		c3 = t;
	}
	void rotPatCC(const Face face) {
		rotPatCC(data[face][0], data[face][1], data[face][2], data[face][3], data[face][4], data[face][5], data[face][6], data[face][7], data[face][8]);
	}

private:
	char &get(const Face face, const int i) {
		View v = curViews()[face];
		return data[v.face][ROTA[v.rota][i]];
	}
	
	char col(const Face face) const {
		return colo[curViews()[face].face];
	}

	void change(const View &v) {
		view = curViews()[v.face];
		view.rota = (view.rota + v.rota) & 0x03;
	}

	const View *curViews() const {
		return VIEW[view.face][view.rota];
	}

	void add(const Step &step) {
		if (sol.empty() || !sol.back().combine(step))
			sol.push_back(step);
	}

	std::string getString() const {
		std::string res;
		for (Step s : sol)
			if (s.time) {
				char c = getCharFromFace(s.face);
				if (s.layer)
					c = std::tolower(c);
				res += c;
				if (s.time == 2)
					res += '2';
				if (s.time == 3)
					res += '\'';
			}
		return res;
	}

private:
	View view;
	char colo[6];
	char data[6][9];
	std::vector<Step> sol;

	static const View VIEW[6][4][6];
	static const int ROTA[4][9];
};

const View RubiksCube::VIEW[6][4][6] { {
		{"F0", "L0", "R0", "B0", "U0", "D0"},
		{"F1", "D1", "U1", "B3", "L1", "R3"},
		{"F2", "R2", "L2", "B2", "D2", "U2"},
		{"F3", "U3", "D3", "B1", "R3", "L3"},
	}, {
		{"L0", "B0", "F0", "R0", "U3", "D1"},
		{"L1", "D2", "U0", "R3", "B1", "F1"},
		{"L2", "F2", "B2", "R2", "D3", "U1"},
		{"L3", "U2", "D0", "R1", "F3", "B3"},
	}, {
		{"R0", "F0", "B0", "L0", "U1", "D3"},
		{"R1", "D0", "U2", "L3", "F1", "B1"},
		{"R2", "B2", "F2", "L2", "D1", "U3"},
		{"R3", "U0", "D2", "L1", "B3", "F3"},
	}, {
		{"B0", "R0", "L0", "F0", "U2", "D2"},
		{"B1", "D3", "U3", "F3", "R1", "L1"},
		{"B2", "L2", "R2", "F2", "D0", "U0"},
		{"B3", "U1", "D1", "F1", "L3", "R3"},
	}, {
		{"U0", "L1", "R3", "D2", "B2", "F0"},
		{"U1", "F1", "B3", "D1", "L2", "R0"},
		{"U2", "R1", "L3", "D0", "F2", "B0"},
		{"U3", "B1", "F3", "D3", "R2", "L0"},
	}, {
		{"D0", "L3", "R1", "U2", "F0", "B2"},
		{"D1", "B3", "F1", "U1", "L0", "R2"},
		{"D2", "R3", "L1", "U0", "B0", "F2"},
		{"D3", "F3", "B1", "U3", "R0", "L2"},
	},
};

const int RubiksCube::ROTA[4][9] {
	{0, 1, 2, 3, 4, 5, 6, 7, 8},
	{6, 3, 0, 7, 4, 1, 8, 5, 2},
	{8, 7, 6, 5, 4, 3, 2, 1, 0},
	{2, 5, 8, 1, 4, 7, 0, 3, 6},
};

int main() {
	using namespace std;
	char s[6][12];
	for (int i = 0; i < 6; ++i)
		cin >> s[i];
	RubiksCube cube(s);
	cout << cube.solve() << endl;
}
