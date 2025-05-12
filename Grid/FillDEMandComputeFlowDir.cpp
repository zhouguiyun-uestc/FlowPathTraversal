#include "FillDEMandComputeFlowDir.h"
#include "io_gdal.h"
#include <queue>
#include "cell.h"
#include "tool.h"
#include "util.h"

class SpillCell :public Cell
{
public:
	SpillCell() {
		spill = -9999;
	}

	SpillCell(const Cell& c) :Cell(c)
	{
		spill = -9999;
	}
	SpillCell(const Cell c, float spill) :spill(spill), Cell(c)
	{

	}
	float spill;

};
class SpillCellCompare {
public:
	bool operator()(const SpillCell& c1, const SpillCell& c2)
	{
		return c1.spill > c2.spill;
	}
};

using PriorityQueue=std::priority_queue<SpillCell, std::deque<SpillCell>, SpillCellCompare>;

//Fill depresspions and calculate flow directions from input DEM using the algorithm in Wang and Liu (2006)
void FillDEMandComputeFlowDir(const std::filesystem::path& inputPath, const std::filesystem::path& outputPath)
{
	Grid<float> dem = readRasterAsFloat(inputPath);
	
	TimeSpan span;

	int height = dem.height();
	int width = dem.width();
	Grid<bool> maskGrid(dem);
	maskGrid.allocate();
	
	Grid<FlowDir> flowDirGrid(dem);
	flowDirGrid.allocate();
	
	PriorityQueue queue;
	
	int validElementsCount = 0;
	cout << "Using the method in Wang&Liu(2006) to fill depressions...\n";
	
	//Push all border cells into the queue
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			Cell c(row, col);
			if (!dem.isNoData(c))
			{
				validElementsCount++;
				for (int i = 0; i < 8; i++)
				{
					Cell n=getNeighboringCell(c,i);

					if (!dem.isInGrid(n) || dem.isNoData(n))
					{
						//Cell currentCell(row, col, dem(c));
						queue.push(SpillCell(c,dem(c)));
						maskGrid(c) = true;
						break;
					}
				}
			}
		}
	}

	int percentFive = validElementsCount / 20;

	cout << "Start filling depressions and calculationg flow direciton matrix ..." << endl;
	int count = 0;
	while (!queue.empty())
	{
		count++;
		if (count % percentFive == 0)
		{
			int percentNum = count / percentFive;
			cout << "Progress£º" << percentNum * 5 << "%\r";
		}
		SpillCell c = queue.top();
		queue.pop();
		float spill = c.spill;
		for (int i = 0; i < 8; i++)
		{
			Cell n = getNeighboringCell(c, i);			
			if (dem.isInGrid(n) && !dem.isNoData(n))
			{
				if (!maskGrid(n))
				{
					SpillCell spillCell(n);
					maskGrid(n) = true;

					float nSpill = dem(n);
					// a depression or a flat region is encountered
					if (nSpill <= spill)
					{
						dem(n) = spill;
						spillCell.spill = spill;
						//set the flow direction as the reverse of the search direction
						flowDirGrid(n.row, n.col)=getReverseDir(i);
					}
					else
					{
						spillCell.spill = nSpill;
					}
					queue.push(spillCell);
				}
			}
		}

		//if the flow direction of the cell is unspecified yet, use D8 rule to assign the direction
		if (flowDirGrid.isNoData(c))
		{
			FlowDir dir = computeFlowDir(dem, c, spill);
			flowDirGrid(c)=dir;
		}

	}
	cout << "Finish filling depressions and calculating flow direciton matrix." << endl;

	int time = span.elapsed_millseconds();
	writeRaster(flowDirGrid, outputPath);
	cout << "Flow direction grid created!" << endl;
	cout << "time in milliseconds for flow direction calculation:" << time << endl;
}