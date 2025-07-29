#include "io_gdal.h"
//#include <gdal/gdal_priv.h>
#include <gdal_priv.h>

#include <iostream>
#include <exception>

using namespace std;

GDALDataType readRasterDataType(const std::filesystem::path& path)
{
	std::cout << "Reading input DEM file..." << std::endl;
	GDALDataset* poDataset;
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

	poDataset = (GDALDataset*)GDALOpen(path.string().c_str(), GA_ReadOnly);
	const char* errorMsg = CPLGetLastErrorMsg();
	if (poDataset == nullptr)
		throw runtime_error("Failed to open the input GeoTIFF file");

	GDALRasterBand* poBand;
	poBand = poDataset->GetRasterBand(1);
	GDALDataType dataType = poBand->GetRasterDataType();
	GDALClose((GDALDatasetH)poDataset);
	return dataType;
}

Grid<float> readRasterAsFloat(const std::filesystem::path& path)
{
	//Grid<float> gridF;
	//return gridF;

	auto dataType = readRasterDataType(path);
	if (dataType == GDALDataType::GDT_Int16)
	{
		Grid<short> grid = readRaster<short>(path);
		Grid<float> gridFloat = grid.to<float>();
		grid.changeNoDataValue(grid.noDataValue(), -9999);
		return gridFloat;
	}
	else if (dataType == GDALDataType::GDT_Int32)
	{
		Grid<int> grid = readRaster<int>(path);
		Grid<float> gridFloat = grid.to<float>();
		grid.changeNoDataValue(grid.noDataValue(), -9999);
		return gridFloat;
	}
	else if (dataType == GDALDataType::GDT_Float32)
	{
		Grid<float> grid = readRaster<float>(path);
		grid.changeNoDataValue(grid.noDataValue(), -9999);
		return grid;
	}
	else
		throw runtime_error("Unsupported data type. Curretly only short, int and float datatypes are supported.");
}





