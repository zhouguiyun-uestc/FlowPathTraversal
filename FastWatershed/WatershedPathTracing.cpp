#include "WatershedPathTracing.h"
#include <Grid/tool.h>
#include <vector>
#include <iostream>

using namespace std;
void WatershedByPathTracing(Grid<FlowDir>& dirGrid, Grid<int>& wsGrid, const std::map<Cell, int>& outlets)
{
	//give all border cells that drain outside the dem area an ID
	int height = dirGrid.height();
	int width = dirGrid.width();

	if (outlets.size() > 0) {
		//use existing outlets
		cout << "Use existing outlets: " << outlets.size() << endl;
		for (auto& [cell, label] : outlets) {
			wsGrid(cell) = label;
		}
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
					}
				}
			}
		}
	}

	for (int row = 0; row < height; row++)
	{
		//cout << "Row: " << row  << endl;
		for (int col = 0; col < width; col++)
		{
			Cell c(row, col);
			if (!dirGrid.isNoData(c))
			{
				int id = -1;
				int count = 0;
				do
				{
					count++;
					if (!wsGrid.isNoData(c))
					{
						id = wsGrid(c);
						break;
					}
				} while (moveToDownstreamCell(dirGrid, c));

				if (id > -1)
					wsGrid(Cell(row,col)) = id;
			}
		}
	}
}