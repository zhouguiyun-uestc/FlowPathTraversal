#include "WatershedFlowPathTraversal.h"
#include "WatershedFast_gpu.h"
#include <Grid/tool.h>
#include <cuda_runtime.h>
#include <cuda.h>
#include <cuda_runtime_api.h>
#include <Grid/util.h>
#include <iostream>

using namespace std;

void WatershedFastGPU(Grid<FlowDir>& dirGrid, Grid<int>& wsGrid, const std::map<Cell, int>& outlets)
{
	int height = dirGrid.height();
	int width = dirGrid.width();

	if (outlets.size() > 0) {
		//use existing outlets
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

	unsigned char* devDirGrid=nullptr;
	int * devWsGrid=nullptr;
	unsigned int byteCount = dirGrid.height() * dirGrid.width();
	
	cudaMalloc((void**)&devDirGrid, byteCount);
	cudaMalloc((void**)&devWsGrid, byteCount*sizeof(int));
	cudaMemcpy((void*)devWsGrid, (void*)wsGrid.getDataPtr(), byteCount*sizeof(int), cudaMemcpyHostToDevice);
	
	TimeSpan span;
	cudaMemcpy((void*)devDirGrid, (void*)dirGrid.getDataPtr(), byteCount,cudaMemcpyHostToDevice);
	cudaDeviceSynchronize();
	//Ö´ÐÐ´úÂë
	WatershedFlowPathTraversalGPU(devDirGrid, devWsGrid, dirGrid.height(), dirGrid.width());

	cudaMemcpy((void*)wsGrid.getDataPtr(), (void*)devWsGrid, byteCount * sizeof(int), cudaMemcpyDeviceToHost);
	cudaDeviceSynchronize();
	cudaFree(devDirGrid);
    cudaFree(devWsGrid);
	cout << "actual time in milliseconds using " << span.elapsed_millseconds() << endl;

}