#include "WatershedRecursive.h"
#include "WatershedRegionGrowing.h"
#include <queue>
#include <Grid/tool.h>
#include <iostream>

using namespace std;

void computeUpstreamWatershed(Grid<FlowDir>& dirGrid, Grid<int>& wsGrid, const Cell& c, int wsIndex) {
	for (int i = 0; i < 8; i++) {
		Cell n = getNeighboringCell(c, i);
		if (!dirGrid.isInGrid(n) || dirGrid.isNoData(n)) continue;
		if (dirGrid(n) == getReverseDir(i)) {
			//neighbor i drains to c
			wsGrid(n) = wsIndex;
			computeUpstreamWatershed(dirGrid, wsGrid, n, wsIndex);
		}
	}
}

void WatershedRecursive(Grid<FlowDir>& dirGrid, Grid<int>& wsGrid, const std::map<Cell, int>& outlets)
{
	cout << "computing watershed using recursive algorithm: " << endl;

	if (outlets.size() > 0) {
		//use existing outlets
		cout << "Use existing outlets: " << outlets.size() << endl;
		for (auto& [cell, label] : outlets) {
			wsGrid(cell) = label;
			computeUpstreamWatershed(dirGrid, wsGrid, cell, label);
		}
	}
	else {

		//obtain all outlet  cells that drain outside the dem area an ID 	
		int height = dirGrid.height();
		int width = dirGrid.width();

		int wsIndex = 1;
		for (int row = 0; row < height; row++)
		{
			for (int col = 0; col < width; col++)
			{
				Cell c(row, col);
				if (!dirGrid.isNoData(c))
				{
					if (!moveToDownstreamCell(dirGrid, c)) {
						wsGrid(c) = wsIndex;
						computeUpstreamWatershed(dirGrid, wsGrid, c, wsIndex);
						wsIndex++;
					}
				}
			}
		}
	}
}

