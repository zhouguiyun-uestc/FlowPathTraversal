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
#include <fstream>
#include <unordered_map>
using namespace std;

void testWatersed(const std::filesystem::path& flowDirPath)
{
	try
	{

#ifdef _WIN32
		Grid<FlowDir> dirGrid = readRaster<FlowDir>(flowDirPath);
#else
		Grid<FlowDir> dirGrid = readRaster<FlowDir>("/home/kylin/ansai_flowdir.tif");
#endif
		TimeSpan span;
		//Grid<int> wsGrid(dirGrid);
		//wsGrid.allocate();
//		WatershedFast(dirGrid, wsGrid);
//		cout << "time elapsed for fast algorithm:" << span.elapsed_millseconds() << endl;
//#ifdef _WIN32
//		writeRaster(wsGrid, flowDirPath.parent_path() / (flowDirPath.stem().string() + "_ws_fast.tif"));
//#else
//		writeRaster(wsGrid, "/home/kylin/ansai_ws.tif");
//#endif

		//WatershedFastOpenMP(dirGrid, wsGrid);
		//cout << "time elapsed for fast algorithm:" << span.elapsed_millseconds() << endl;
		//writeRaster(wsGrid, flowDirPath.parent_path() / (flowDirPath.stem().string() + "_ws_openmp.tif"));

		//Grid<int> wsGridPathTracing(dirGrid);
		//wsGridPathTracing.allocate();
		//WatershedByPathTracing(dirGrid, wsGridPathTracing);
		//writeRaster(wsGridPathTracing, "e:/temp/ansai_ws_pathtracing.tif");

	/*	span.reset();
		Grid<int> wsGridIterative(dirGrid);
		wsGridIterative.allocate();
		WatershedIterative(dirGrid, wsGridIterative);
		cout << "time elapsed for iterative algorithm:" << span.elapsed_millseconds() << endl;
		writeRaster(wsGridIterative, flowDirPath.parent_path() / (flowDirPath.stem().string() + "_ws_iterative.tif"));*/

		//span.reset();
		//Grid<int> wsGridRecursive(dirGrid);
		//wsGridRecursive.allocate();
		//WatershedRecursive(dirGrid, wsGridRecursive);
		//cout << "time elapsed for recursive algorithm:" << span.elapsed_millseconds() << endl;
		//writeRaster(wsGridRecursive, "e:/temp/ansai_ws_recursive.tif");

//		Grid<int> wsGridGPU(dirGrid);
//		wsGridGPU.allocate();
//		WatershedFastGPU(dirGrid, wsGridGPU);
//#ifdef _WIN32
//		writeRaster(wsGridGPU, "e:/temp/ansai_ws_gpu.tif");
//#else
//		writeRaster(wsGridGPU, "/home/kylin/ansai_ws_gpu.tif");
//#endif
	}
	catch (exception& ex) {
		cout << ex.what() << endl;
	}
}

void testFastWatershedOpenMP(const std::filesystem::path& flowDirPath, const std::filesystem::path& wsPath)
{
	Grid<FlowDir> dirGrid = readRaster<FlowDir>(flowDirPath);
	TimeSpan span;
	Grid<int> wsGrid(dirGrid);
	wsGrid.allocate();
	WatershedFlowPathTraversal_OpenMP(dirGrid, wsGrid);
	cout << "time elapsed for openmp algorithm:" << span.elapsed_millseconds() << endl;
	writeRaster(wsGrid, wsPath);
}

void testFastWatershed(const std::filesystem::path& flowDirPath, const std::filesystem::path& wsPath)
{
	Grid<FlowDir> dirGrid = readRaster<FlowDir>(flowDirPath);
	TimeSpan span;
	Grid<int> wsGrid(dirGrid);
	wsGrid.allocate();
	WatershedFlowPathTraversal(dirGrid, wsGrid);
	cout << "time elapsed for fast algorithm:" << span.elapsed_millseconds() << endl;
	writeRaster(wsGrid, wsPath);
}

void testFastWatershedGPU(const std::filesystem::path& flowDirPath)
{
	Grid<FlowDir> dirGrid = readRaster<FlowDir>(flowDirPath);
	TimeSpan span;
	Grid<int> wsGrid(dirGrid);
	wsGrid.allocate();
	WatershedFastGPU(dirGrid, wsGrid);
	cout << "time elapsed for fast algorithm:" << span.elapsed_millseconds() << endl;
	writeRaster(wsGrid, flowDirPath.parent_path() / (flowDirPath.stem().string() + "_ws_gpu.tif"));
}

void testFastWatershedGPUWithOutletFiles(const std::filesystem::path& flowDirPath, const std::filesystem::path& wsPath, const std::filesystem::path& outletFilePath)
{
	auto outlets = loadOutletLocations(outletFilePath);
	Grid<FlowDir> dirGrid = readRaster<FlowDir>(flowDirPath);
	TimeSpan span;
	Grid<int> wsGrid(dirGrid);
	wsGrid.allocate();
	WatershedFastGPU(dirGrid, wsGrid, outlets);
	writeRaster(wsGrid, wsPath);
}

void testFastWatershedWithOutletFiles(const std::filesystem::path& flowDirPath, const std::filesystem::path& wsPath,const std::filesystem::path& outletFilePath)
{
	auto outlets = loadOutletLocations(outletFilePath);
	Grid<FlowDir> dirGrid = readRaster<FlowDir>(flowDirPath);
	TimeSpan span;
	Grid<int> wsGrid(dirGrid);
	wsGrid.allocate();
	WatershedFlowPathTraversal(dirGrid, wsGrid,outlets);
	writeRaster(wsGrid, wsPath);
}

void testFastWatershedAndOutletFiles(const std::filesystem::path& flowDirPath, const std::filesystem::path& outletFilePath)
{
	Grid<FlowDir> dirGrid = readRaster<FlowDir>(flowDirPath);
	TimeSpan span;
	Grid<int> wsGrid(dirGrid);
	wsGrid.allocate();
	auto key2Cell=WatershedFastForOutletFiles(dirGrid, wsGrid);
	auto stats=computeStats(wsGrid, 255);
	writeOutletFile(stats, key2Cell, outletFilePath);
}

void testFlowDirCompute(const std::filesystem::path& demPath,const  std::filesystem::path& outputFolder)
{	
	auto outputPath = outputFolder / (demPath.stem().string() + "_flowDir.tif");
	FillDEMandComputeFlowDir(demPath, outputPath);
	Grid<FlowDir> dirGrid = readRaster<FlowDir>(outputPath);
	Grid<unsigned int> accuGrid(dirGrid);
	accuGrid.allocate();
	computeFlowAccu(dirGrid, accuGrid);
	writeRaster(accuGrid, outputFolder / (demPath.stem().string() + "_accu.tif"));
}


void testRegionGrowing(const std::filesystem::path& flowDirPath, const std::filesystem::path& outletsPath,
	const std::filesystem::path& wsPath)
{
	auto outlets = loadOutletLocations(outletsPath);
	Grid<FlowDir> dirGrid = readRaster<FlowDir>(flowDirPath);
	TimeSpan span;
	Grid<int> wsGrid(dirGrid);
	wsGrid.allocate();
	WatershedRegionGrowing(dirGrid, wsGrid,outlets);
	cout << "time elapsed for region growing:" << span.elapsed_millseconds() << endl;
	writeRaster(wsGrid, wsPath);
}

int main(int argc, char* argv[])
{
	//testFastWatershedGPU("e:/DEM/flowdirs/ansai_flowDir.tif");
	//testFastWatershedAndOutletFiles("e:/DEM/flowdirs/ansai_flow.tif", "e:/DEM/flowdirs/ansai_outlets_new.txt");
	//testFastWatershedWithOutletFiles("e:/DEM/flowdirs/ansai_flow.tif", "e:/DEM/Watershed/ansai_ws.tif","e:/DEM/flowdirs/ansai_outlets_new.txt");
	
//	testFastWatershedAndOutletFiles("e:/DEM/flowdirs/aitkin_flow.tif", "e:/DEM/flowdirs/aitkin_outlets_new.txt");
//	testFastWatershedWithOutletFiles("e:/DEM/flowdirs/aitkin_flow.tif", "e:/DEM/Watershed/aitkin_ws.tif","e:/DEM/flowdirs/aitkin_outlets_new.txt");

	//testFastWatershedGPUWithOutletFiles("e:/DEM/flowdirs/ansai_flow.tif", "e:/DEM/Watershed/ansai_ws.tif", "e:/DEM/flowdirs/ansai_outlets_new.txt");
	//testFastWatershed("e:/DEM/flowdirs/aitkin_flow.tif","e:/DEM/Watershed/aitkin_fast.tif");
	//testFastWatershedOpenMP("e:/DEM/flowdirs/aitkin_flow.tif", "e:/DEM/Watershed/aitkin_openmp.tif");

	//testFastWatershedOpenMP("e:/DEM/flowdirs/ansai_flow.tif", "e:/DEM/Watershed/ansai_openmp.tif");
	//testFastWatershed("e:/DEM/flowdirs/ansai_flow.tif", "e:/DEM/Watershed/ansai_fast.tif");
	std::filesystem::path folder="/home/kylin/.vs/FastWatershed/1b3dff6e-6a6e-44cb-b359-0c6e14d239be/out/build/Linux-GCC-Release";
	testRegionGrowing(folder/"1m_flow"/"benton_flow.tif",folder/"1m_outlets"/ "benton_ws.txt", 
		folder / "testFlowdir" / "benton_regionflow_alloutlets.tif");

	//std::filesystem::path folder = "e:\\DEM\\flowdirs";
	//testRegionGrowing(
	//	folder /"benton_flow.tif", 
	//	folder /"benton_ws.txt",
	//	folder / "benton_regionflow_alloutlets_ws.tif");

	return 0;
}