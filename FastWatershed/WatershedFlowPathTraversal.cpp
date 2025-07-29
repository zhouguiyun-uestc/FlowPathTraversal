#include "WatershedFlowPathTraversal.h"
#include <Grid/tool.h>
#include <vector>
#include <iostream>
using namespace std;

void WatershedFlowPathTraversal(Grid<FlowDir>& dirGrid, Grid<int>& wsGrid, const std::map<Cell, int>& outlets)
{	
	cout << "computing watershed using our proposed flow path traversal algorithm: " << endl;
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
		cout << "Use all computed outlets: " << (wsIndex-1) << endl;
	}
	
	int no_data_value = wsGrid.noDataValue();
	//cout << "no_data_value of flowdir Grid: " << (unsigned char)dirGrid.noDataValue() << endl;
	//cout << "no_data_value of wsGrid: " <<no_data_value << endl;

	std::vector<Cell> tracedCells;
	tracedCells.reserve(2 * (height + width));
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			Cell c(row, col);
			if (dirGrid.isNoData(c)) continue;

			tracedCells.clear();
			int id;
			do
			{
				id = wsGrid(c);
				if (id == no_data_value) {
					tracedCells.push_back(c);
				}
				else
					break;
			} while (moveToDownstreamCell(dirGrid, c));

			if (id == no_data_value) id = -1;


			if (tracedCells.size() > 0)
			{
				for (auto& cell : tracedCells)
					wsGrid(cell) = id;
			}

		}
	}

	//mark all -1 as no_data
	if (outlets.size() > 0) {
		int id;
		for (int row = 0; row < height; row++)
			for (int col = 0; col < width; col++) {
				id = wsGrid(row, col);
				if (id == -1) wsGrid(row, col) = no_data_value;
			}
	}
}

//compute watersheds and return [label, cell] map
std::map<int, Cell> WatershedFastForOutletFiles(Grid<FlowDir>& dirGrid, Grid<int>& wsGrid)
{
	//give all border cells that drain outside the dem area an ID
	int height = dirGrid.height();
	int width = dirGrid.width();
	std::map<int, Cell> key2Cell;
	int wsIndex = 1;
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			Cell c(row, col);
			if (!dirGrid.isNoData(c))
			{
				if (!moveToDownstreamCell(dirGrid, c)) {
					key2Cell[wsIndex] = c;
					wsGrid(c) = wsIndex++;
				}
			}
		}
	}

	std::vector<Cell> tracedCells;
	tracedCells.reserve(2 * (height + width));
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			Cell c(row, col);
			if (!dirGrid.isNoData(c))
			{
				tracedCells.clear();
				int id = -1;
				do
				{
					if (!wsGrid.isNoData(c))
					{
						id = wsGrid(c);
						break;
					}
					tracedCells.push_back(c);
				} while (moveToDownstreamCell(dirGrid, c));

				if (id != -1) {
					for (auto& c : tracedCells)
						wsGrid(c) = id;
				}
			}
		}
	}
	return key2Cell;
}