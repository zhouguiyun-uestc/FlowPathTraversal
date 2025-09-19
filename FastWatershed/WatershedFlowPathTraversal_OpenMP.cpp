#include "WatershedFlowPathTraversal.h"
#include <Grid/tool.h>
#include <vector>
#include <iostream>

using namespace std;
void WatershedFlowPathTraversal_OpenMP(Grid<FlowDir>& dirGrid, Grid<int>& wsGrid, const std::map<Cell, int>& outlets)
{
	cout << "computing watershed using OpenMP and our proposed flow path traversal algorithm: " << endl;

	//give all border cells that drain outside the dem area an ID
	int height = dirGrid.height();
	int width = dirGrid.width();
	if (outlets.size() > 0) {
		//use existing outlets
		for (auto& [cell, label] : outlets) {
			wsGrid(cell) = label;
		}
	}
	else
	{
		int wsIndex = 1;


		cout << "Get all watershed labels: " << endl;

		#pragma omp parallel for
		for (int row = 0; row < height; row++)
		{
			for (int col = 0; col < width; col++)
			{
				Cell c(row, col);
				if (!dirGrid.isNoData(c))
				{
					if (!moveToDownstreamCell(dirGrid, c)) {
						#pragma omp atomic capture
						wsGrid(c) = wsIndex++;
					}
				}
			}
		}
		cout << "Use all computed outlets: " << (wsIndex - 1) << endl;
	}



	std::vector<Cell> tracedCells;
	tracedCells.reserve(2 * (height + width));
	int no_data_value = wsGrid.noDataValue();
	
	#pragma omp parallel for private(tracedCells) 
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			Cell c(row, col);
			if (dirGrid.isNoData(c)) continue;
			tracedCells.clear();
			int id = -1;
			do
			{
				#pragma omp atomic read
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
				for (auto& c : tracedCells)
					#pragma omp atomic write
					wsGrid(c) = id;
			}
		}
	}

	//mark all -1 as no_data
	if (outlets.size() > 0) {
		int id;
		#pragma omp parallel for private(id) 
		for (int row = 0; row < height; row++)
			for (int col = 0; col < width; col++) {
				id = wsGrid(row, col);
				if (id == -1) wsGrid(row, col) = no_data_value;
			}
	}
}


