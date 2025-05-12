#include "FlowAccu.h"
#include "tool.h"
#include <iostream>

using namespace std;
void computeFlowAccu(Grid<FlowDir>& dirGrid, Grid<unsigned int>& accuGrid)
{
	int width = dirGrid.width();
	int height = dirGrid.height();

	cout<<"Using Zhou's algorithm to compute the flow accumulation matrix ... "<<endl;

	auto nipsGrid=computeNIPSGrid(dirGrid);	

	//Initialize flow accumulation matrix with one
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			Cell c(row,col);
			if (!dirGrid.isNoData(c))
				accuGrid(c)=1;
		}
	}

	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			Cell c(row, col);
			if (!dirGrid.isNoData(c))
			{
				if (nipsGrid(c) != 0)
					continue;

				int nAccuCellNum = 0;
				do
				{
					int currentNodeAccu = accuGrid(c);
					currentNodeAccu += nAccuCellNum;
					accuGrid(c)=currentNodeAccu;
					nAccuCellNum = currentNodeAccu;
					if (nipsGrid(c) >= 2)
					{
						nipsGrid.add(c, - 1);
						break;
					}
				} while (moveToDownstreamCell(dirGrid,c));
			}
		}
	}
}