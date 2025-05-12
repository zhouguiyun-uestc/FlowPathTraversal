#include "WatershedRegionGrowing.h"
#include <queue>
#include <Grid/tool.h>
#include <iostream>
using namespace std;

void WatershedRegionGrowing(Grid<FlowDir>& dirGrid, Grid<int>& wsGrid, const std::map<Cell, int>& outlets)
{
	cout << "computing watershed using region-growing algorithm: " << endl;

	//give all border cells that drain outside the dem area an ID 
	//and push them into queue
	int height = dirGrid.height();
	int width = dirGrid.width();
	
	std::queue<Cell> queue;
	if (outlets.size() > 0) {
		//use existing outlets
		for (auto& [cell, label] : outlets) {
			wsGrid(cell) = label;
			queue.push(cell);
		}
		cout << "Use existing outlets: " << queue.size() << endl;
		//int s = 0;
		//s++;
	}
	else {
		int wsIndex = 1;
		for (int row = 0; row < height; row++)
		{
			for (int col = 0; col < width; col++)
			{
				Cell c(row, col);
				if (!dirGrid.isNoData(c))
				{
					if (!moveToDownstreamCell(dirGrid, c)) {
						wsGrid(c) = wsIndex++;
						queue.push(c);
					}
				}
			}
		}
	}

	while (queue.size() != 0) {
		Cell c = queue.front();
		queue.pop();
		int id = wsGrid(c);
		//find neighboring cells that flow to c
		for (int i = 0; i < 8; i++) {
			Cell n = getNeighboringCell(c, i);
			if (!dirGrid.isInGrid(n) || dirGrid.isNoData(n)) continue;
			if (dirGrid(n) == getReverseDir(i)) {
				wsGrid(n) = id;
				queue.push(n);
			}
		}
	}
}

