#include "ArtiLife.h"

bool LifeCell::init(int x, int y, char color, bool border, bool alive, LifeCell* nw, LifeCell* n, LifeCell* ne, LifeCell* w, LifeCell* e, LifeCell* sw, LifeCell* s, LifeCell* se)
{
	cell_xSpot = x;
	cell_ySpot = y;
	cell_myColor = color;
	
	cell_isBorder = border;
	cell_isAlive = alive;
	
	cell_nw = nw;
	cell_n = n;
	cell_ne = ne;
	cell_w = w;
	cell_e = e;
	cell_sw = sw;
	cell_s = s;
	cell_se = se;

	cell_isBlock = false;

	cell_gen = 0;
	cell_toRender = true;

	return true;
}

void LifeCell::pollNeighbors(int& redCount, int& greenCount, int& yellowCount)
{
	redCount = 0;
	greenCount = 0;
	yellowCount = 0;

	// poll neighbors

	if (cell_nw->getColor() == 'r')
		redCount++;
	if (cell_nw->getColor() == 'g')
		greenCount++;
	if (cell_nw->getColor() == 'y')
		yellowCount++;

	if (cell_n->getColor() == 'r')
		redCount++;
	if (cell_n->getColor() == 'g')
		greenCount++;
	if (cell_n->getColor() == 'y')
		yellowCount++;

	if (cell_ne->getColor() == 'r')
		redCount++;
	if (cell_ne->getColor() == 'g')
		greenCount++;
	if (cell_ne->getColor() == 'y')
		yellowCount++;

	if (cell_w->getColor() == 'r')
		redCount++;
	if (cell_w->getColor() == 'g')
		greenCount++;
	if (cell_w->getColor() == 'y')
		yellowCount++;

	if (cell_e->getColor() == 'r')
		redCount++;
	if (cell_e->getColor() == 'g')
		greenCount++;
	if (cell_e->getColor() == 'y')
		yellowCount++;

	if (cell_sw->getColor() == 'r')
		redCount++;
	if (cell_sw->getColor() == 'g')
		greenCount++;
	if (cell_sw->getColor() == 'y')
		yellowCount++;

	if (cell_s->getColor() == 'r')
		redCount++;
	if (cell_s->getColor() == 'g')
		greenCount++;
	if (cell_s->getColor() == 'y')
		yellowCount++;

	if (cell_se->getColor() == 'r')
		redCount++;
	if (cell_se->getColor() == 'g')
		greenCount++;
	if (cell_se->getColor() == 'y')
		yellowCount++;

}

void LifeCell::nextGen()
{
	if (cell_isBorder)
	{
		cell_gen++;
		return;
	}

	cell_willBeAlive = false;
	cell_myNextColor = 'w';
	cell_isBlock = false;

	// poll the surrounding cells
	
	int redCount = 0;
	int greenCount = 0;
	int yellowCount = 0;
	int allCount = 0;
	int friendCount = 0;

	pollNeighbors(redCount, greenCount, yellowCount);
	allCount = redCount + greenCount + yellowCount;
	if (cell_myColor == 'r')
		friendCount = redCount + yellowCount;
	if (cell_myColor == 'g')
		friendCount = greenCount + yellowCount;
	if (cell_myColor == 'y')
		friendCount = allCount;

	// will a cell be born?

	if (!cell_isAlive)
	{
		if (allCount == 3) // must have exactly three living neighbors
		{
			// determine if red or green color

			if (redCount > 1 || (redCount==1 && yellowCount==2))
			{
				cell_myNextColor = 'r';
				cell_willBeAlive = true;
				cell_gen = 0;
				cell_toRender = true;
				return;
			}
			else if (greenCount > 1 || (greenCount==1 && yellowCount==2))
			{
				cell_myNextColor = 'g';
				cell_willBeAlive = true;
				cell_gen = 0;
				cell_toRender = true;
				return;
			}
			else									// must be yellow
			{
				cell_myNextColor = 'y';
				cell_willBeAlive = true;
				cell_gen = 0;
				cell_toRender = true;
				return;
			}
		}
	}

	// will a cell survive?

	if (cell_isAlive)
	{
		if (allCount == 2 || allCount == 3 || ((friendCount==2 || friendCount==3) && allCount<5))
		{
			cell_willBeAlive = true;
			cell_myNextColor = cell_myColor;

			// will a cell be converted?

			if (cell_myColor == 'r' && greenCount >= 2)
			{
				cell_myNextColor = 'g';
				cell_gen = 0;
				cell_toRender = true;
				return;
			}
			if (cell_myColor == 'g' && redCount >= 2)
			{
				cell_myNextColor = 'r';
				cell_gen = 0;
				cell_toRender = true;
				return;
			}
			if (cell_myColor == 'y')
			{
				if (redCount > greenCount)
				{
					cell_myNextColor = 'r';
					cell_gen = 0;
					cell_toRender = true;
					return;
				}
				if (greenCount > redCount)
				{
					cell_myNextColor = 'g';
					cell_gen = 0;
					cell_toRender = true;
					return;
				}
			}
		}
		else // cell has died
		{
			cell_toRender = true;
			cell_gen = 0;
			cell_myNextColor = 'w';
			return;
		}
	}
	cell_gen++;
	
	return;
}

bool LifeCell::couldBeBlock() 
{
	// stability check

	if (cell_isBorder || cell_isBlock)
		return false;

	if (!cell_isAlive || cell_gen < STABLE_GEN)
		return false;
	
	if (cell_nw->getGen() < STABLE_GEN
		|| cell_n->getGen() < STABLE_GEN
		|| cell_ne->getGen() < STABLE_GEN
		|| cell_w->getGen() < STABLE_GEN
		|| cell_e->getGen() < STABLE_GEN
		|| cell_sw->getGen() < STABLE_GEN
		|| cell_s->getGen() < STABLE_GEN
		|| cell_se->getGen() < STABLE_GEN)
		return false;

	// poll the surrounding cells

	int redCount = 0;
	int greenCount = 0;
	int yellowCount = 0;

	pollNeighbors(redCount, greenCount, yellowCount);


	// check for block possibility of four corners

	if ((redCount == 3 && greenCount == 0 && yellowCount == 0) 
		|| (greenCount == 3 && redCount == 0 && yellowCount == 0) 
		|| (greenCount == 0 && redCount == 0 && yellowCount == 3))
	{
		if (cell_e->isAlive() && cell_se->isAlive() && cell_s->isAlive()) return true;
		if (cell_e->isAlive() && cell_ne->isAlive() && cell_n->isAlive()) return true;
		if (cell_w->isAlive() && cell_sw->isAlive() && cell_s->isAlive()) return true;
		if (cell_w->isAlive() && cell_nw->isAlive() && cell_n->isAlive()) return true;
	}
	return false;
}
