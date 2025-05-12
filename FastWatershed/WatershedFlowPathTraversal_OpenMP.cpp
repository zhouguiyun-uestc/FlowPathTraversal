#include "WatershedFlowPathTraversal.h"
#include <Grid/tool.h>
#include <vector>
#include <iostream>

using namespace std;
//Grid<char> UpstreamFlowDirGrid(Grid<FlowDir>& dirGrid)
//{	
//	Grid<char> upstreamFlowDirGrid(dirGrid);
//	upstreamFlowDirGrid.allocate(-1);
//
//	int height = dirGrid.height();
//	int width = dirGrid.width();
//	
//	#pragma omp parallel for 
//	for (int row = 0; row < height; ++row)
//	{
//		for (int col = 0; col < width; ++col)
//		{
//			Cell c(row, col);
//			if (dirGrid.isNoData(c)) continue;
//			int dirIndex = 0;
//			if (!moveToDownstreamCell(dirGrid, c,&dirIndex)) continue;
//
//			//have alreay been set
//			if (!upstreamFlowDirGrid.isNoData(c)) continue;
//
//			//set the incoming upstream index
//			upstreamFlowDirGrid(c) = dirIndex;
//
//		}
//	}
//	return upstreamFlowDirGrid;
//}
//
//void WatershedFastOpenMP1(Grid<FlowDir>& dirGrid, Grid<int>& wsGrid)
//{
//	//compute UpstreamFlowDirGrid
//	Grid<char> upstreamFlowDirGrid=UpstreamFlowDirGrid(dirGrid);
//	int height = dirGrid.height();
//	int width = dirGrid.width();
//
//	int wsIndex = 1;
//
//
//	cout << "Get all watershed labels: " << endl;
//
////#pragma omp parallel for
//	for (int row = 0; row < height; row++)
//	{
//		for (int col = 0; col < width; col++)
//		{
//			Cell c(row, col);
//			if (!dirGrid.isNoData(c))
//			{
//				if (!moveToDownstreamCell(dirGrid, c)) {
//					wsGrid(c) = wsIndex;
////#pragma omp atomic
//					wsIndex++;
//				}
//			}
//		}
//	}
//
//	//¼ÆËã
//	std::atomic<bool> isChanged=true;
//
//	int count = 0;
//	while (isChanged) {
//
//		isChanged = false;
//		std::vector<Cell> tracedCells;
//		tracedCells.reserve(2 * (height + width));
//
//		#pragma omp parallel for private(tracedCells) 
//		for (int row = 0; row < height; row++)
//		{
//			for (int col = 0; col < width; col++)
//			{
//				Cell c(row, col);
//
//				if (dirGrid.isNoData(c)) continue;
//				if (!wsGrid.isNoData(c)) continue;
//				if (!upstreamFlowDirGrid.isNoData(c)) continue;				
//
//				tracedCells.clear();
//				int id = -1;
//				int dirIndex = 0;
//				char upstreamFlowDirIndex;
//				do
//				{
//					tracedCells.push_back(c);
//					
//					if (!moveToDownstreamCell(dirGrid, c,&dirIndex)) 
//						break;
//
//					//next cell has a label
//					if (!wsGrid.isNoData(c))
//					{
//						id = wsGrid(c);
//						break;
//					}
//
//					//allow tracing along one branch only
//					upstreamFlowDirIndex = upstreamFlowDirGrid(c);
//					if (dirIndex != upstreamFlowDirIndex) 
//						break;
//
//				} while (true);
//
//				if (id != -1 && tracedCells.size()>0) {
//					isChanged = true;
//					for (auto& c : tracedCells)
//						wsGrid(c) = id;
//				}
//
//			}
//		}
//		count++;
//		cout << "count: " << count << endl;
//	}
//}

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
						wsGrid(c) = wsIndex;
						#pragma omp atomic
						wsIndex++;
					}
				}
			}
		}
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

