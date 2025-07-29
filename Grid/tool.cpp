#include "tool.h"

static std::array<FlowDir, 8> dirs =
{
	FlowDir::Right,
	FlowDir::BottomRight,
	FlowDir::Bottom,
	FlowDir::BottomLeft,
	FlowDir::Left,
	FlowDir::TopLeft,
	FlowDir::Top,
	FlowDir::TopRight
};

/*
*	reverse flow direction
*/
static std::array<FlowDir, 8> reserseDirs = {
FlowDir::Left,
FlowDir::TopLeft,
FlowDir::Top,
FlowDir::TopRight,
FlowDir::Right,
FlowDir::BottomRight,
FlowDir::Bottom,
FlowDir::BottomLeft };


//the reverse direction of neighbor i
FlowDir getReverseDir(int i)
{
#ifdef DEBUG
	return reserseDirs.at(i);
#else
	return reserseDirs[i];
#endif
}


static std::array<int, 8> rowOffsets = { 0, 1, 1, 1, 0,-1,-1,-1 };
static std::array<int, 8> colOffsets = { 1, 1, 0,-1,-1,-1, 0, 1 };

Cell getNeighboringCell(const Cell& c, int n)
{
#ifdef DEBUG
	return Cell(c.row + rowOffsets.at(n), c.col + colOffsets.at(n));
#else
	return Cell(c.row + rowOffsets[n], c.col + colOffsets[n]);
#endif

}

float getLength(unsigned int dir)
{
	if ((dir & 0x1) == 1)
	{
		return 1.41421f;
	}
	else return 1.0f;
}

FlowDir computeFlowDir(Grid<float>& dem, const Cell& c, float spill)
{
	float iSpill, max, gradient;
	unsigned char steepestSpill;
	max = 0.0f;
	steepestSpill = 255;
	unsigned char lastIndexINGridNoData = 0;

	//using D8 method to calculate the flow direction
	for (int i = 0; i < 8; i++)
	{
		Cell n = getNeighboringCell(c, i);

		if (dem.isInGrid(n) && !dem.isNoData(n) && (iSpill = dem(n)) < spill)
		{
			gradient = (spill - iSpill) / getLength(i);
			if (max < gradient)
			{
				max = gradient;
				steepestSpill = i;
			}
		}
		if (!dem.isInGrid(n) || dem.isNoData(n))
		{
			lastIndexINGridNoData = i;
		}

	}
	return steepestSpill != 255 ? dirs[steepestSpill] : dirs[lastIndexINGridNoData];
}

bool moveToDownstreamCell(Grid<FlowDir>& dirGrid, Cell& c, int* dirIndex)
{
	FlowDir dir = dirGrid(c);
	int iDir = -1;
	for (int i = 0; i < 8; i++)
	{
		if (dirs[i] == dir)
		{
			iDir = i;
			break;
		}
	}
	if (iDir == -1) return false;
	Cell n = getNeighboringCell(c, iDir);
	if (dirGrid.isInGrid(n) && !dirGrid.isNoData(n))
	{
		c = n;
		if (dirIndex) 
			*dirIndex = iDir;
		return true;
	}
	else
	{
		return false;
	}
}




Grid<char> computeNIPSGrid(Grid<FlowDir>& dirGrid)
{
	Grid<char> nipsGrid(dirGrid);
	nipsGrid.allocate();

	int width = dirGrid.width();
	int height = dirGrid.height();
	for (unsigned int row = 0; row < height; ++row)
	{
		for (unsigned int col = 0; col < width; ++col)
		{
			Cell c(row, col);
			if (dirGrid.isNoData(c)) continue;
			if (moveToDownstreamCell(dirGrid, c))
				nipsGrid.add(c, 1);
		}
	}
	return nipsGrid;
}
