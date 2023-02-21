#pragma once

class LifeCell
{
private:
	int cell_xSpot;
	int cell_ySpot;

	int cell_gen;
	
	bool cell_isAlive;
	bool cell_willBeAlive;
	bool cell_isBorder;
	bool cell_toRender;

	char cell_myColor;
	char cell_myNextColor;

	LifeCell* cell_nw;
	LifeCell* cell_n;
	LifeCell* cell_ne;
	LifeCell* cell_w;
	LifeCell* cell_e;
	LifeCell* cell_sw;
	LifeCell* cell_s;
	LifeCell* cell_se;

	bool cell_isBlock;

	static const int STABLE_GEN = 5;


public:
	LifeCell() : cell_xSpot(0), cell_ySpot(0), cell_gen(0), cell_myColor('b'), cell_myNextColor('b'), cell_isBorder(true), cell_isAlive(false), cell_willBeAlive(false), cell_isBlock(false), cell_toRender(true),
		cell_nw(nullptr), cell_n(nullptr), cell_ne(nullptr),
		cell_w(nullptr), cell_e(nullptr),
		cell_sw(nullptr), cell_s(nullptr), cell_se(nullptr)
	{};

	bool init(int x, int y, char color, bool border, bool alive, LifeCell* nw, LifeCell* n, LifeCell* ne, LifeCell* w, LifeCell* e, LifeCell* sw, LifeCell* s, LifeCell* se);
	void nextGen();
	bool couldBeBlock();
	void pollNeighbors(int& redCount, int& greenCount, int& yellowCount);

	inline bool isBorder() { return cell_isBorder; };
	inline bool isAlive() { return cell_isAlive; };
	inline int getGen() { return cell_gen; };
	inline char getColor() { return cell_myColor; };
	inline char getNextColor() { return cell_myNextColor; };
	inline void renew() { cell_isAlive = cell_willBeAlive; cell_myColor = cell_myNextColor; };
	inline void makeBlock() { cell_isBlock = true; cell_myColor = 'y'; cell_toRender = true;};
	inline bool getRender() {
		if (cell_toRender) { cell_toRender = false; return true; }
		else return false;
	}

	
};