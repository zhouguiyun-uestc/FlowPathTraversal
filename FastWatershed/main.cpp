#include <string>
#include <iostream>
#include <Grid/io_gdal.h>
#include <Grid/FlowAccu.h>
#include "WatershedFlowPathTraversal.h"
#include "WatershedPathTracing.h"
#include "WatershedRegionGrowing.h"
#include "WatershedRecursive.h"
#include <Grid/FillDEMandComputeFlowDir.h>
#include <Grid/tool.h>
#include <Grid/util.h>
using namespace std;


int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		cout << "FastWatershed is a program that implements different algorithms for calculating watersheds from flow direction matrices." << endl;
		cout << "FastWatershed also calculates flow direction matrices from unfilled raw DEMs." << endl;
		cout << "FastWatershed supports GeoTIFF file format through thd GDAL library." << endl;
		cout << "\n Usage: FastWatershed [method] inputFile outputFile." << endl;
		cout << "\n [method] can be 'flowdirection','compare','generateOutlets', 'FlowPathTraversal','FlowPathTraversalOpenMP','RegionGrowing','Recursive','PathTracing'" << endl;

		cout << "\nExample Usage1: FastWatershed flowdirection dem.tif flowdir.tif. \n\tCreate the flow direction GeoTIFF file from raw dem.tif file. The flow direction is coded in the same way as ArcGIS. \n\tPlease see http://help.arcgis.com/en/arcgisdesktop/10.0/help/index.html#//009z00000063000000.htm" << endl;
		cout << "\nExample Usage2: FastWatershed FlowPathTraversal flowdir.tif watershed.tif. \n\tCreate the watershed GeoTIFF file from flow direction file flowdir.tif" << endl;
		return 1;
	}

	try
	{
		string algName = argv[1];
		string inputPath = argv[2];
		string outputPath;
		string outletsPath;
		
		if (argc >= 5) {
			outletsPath = argv[3];
			outputPath = argv[4];
		}
		else if (argc >= 4) {
			string path = argv[3];
			if (path.ends_with(".txt"))
				outletsPath = path;
			else
				outputPath = path;
		}		

		if (algName == "flowdirection")
		{
			if (outputPath.empty()) {
				cout << "Output path is expected and not specified." << endl;
				return 1;
			}
			FillDEMandComputeFlowDir(inputPath, outputPath);
		}
		else if (algName == "compare") {
			string gridPath1 = argv[2];
			string gridPath2 = argv[3];
			auto grid1=readRaster<int>(gridPath1);
			auto grid2 = readRaster<int>(gridPath2);
			if (compareGrids(grid1, grid2)) {
				cout << "Two grids are the same!" << endl;
			}
			else cout << "Two grids are different!" << endl;
		}
		else if (algName == "generateOutlets") {
			TimeSpan span;
			Grid<FlowDir> dirGrid = readRaster<FlowDir>(inputPath);
			Grid<int> wsGrid(dirGrid);
			wsGrid.allocate();
			auto key2Cell = WatershedFastForOutletFiles(dirGrid, wsGrid);
			auto stats = computeStats(wsGrid, 255);
			writeOutletFile(stats, key2Cell, outletsPath);
		}
		else if (algName == "FlowPathTraversal" || algName=="FlowPathTraversalOpenMP" || algName == "RegionGrowing" || algName == "Recursive" || algName == "PathTracing")
		{

			std::map<Cell, int> outlets;
			if (!outletsPath.empty())
				outlets=loadOutletLocations(outletsPath);
			Grid<FlowDir> dirGrid = readRaster<FlowDir>(inputPath);

			TimeSpan span;
			Grid<int> wsGrid(dirGrid);
			wsGrid.allocate();
			
			cout << "Output grid is allocated!"<< endl;

			if (algName == "FlowPathTraversal")
				WatershedFlowPathTraversal(dirGrid, wsGrid, outlets);
			else if (algName == "FlowPathTraversalOpenMP")
				WatershedFlowPathTraversal_OpenMP(dirGrid, wsGrid,outlets);
			else if (algName == "RegionGrowing")
				WatershedRegionGrowing(dirGrid, wsGrid,outlets);
			else if (algName == "Recursive")
				WatershedRecursive(dirGrid, wsGrid,outlets);
			else if (algName == "PathTracing")
				WatershedByPathTracing(dirGrid, wsGrid);

			int time = span.elapsed_millseconds();
			cout << "Flow direction:" << inputPath << ",time in milliseconds using " << algName << ":" << time << endl;

			if (!outputPath.empty()) {
				writeRaster(wsGrid, outputPath);
			}
		}
		else {
			cout << "Unknown processing method!" << endl;
		}
	}
	catch (exception& e) {
		cout << e.what() << endl;
	}

	return 0;
}