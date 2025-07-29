#pragma once
#include <filesystem>
#include "grid.h"
//#include "io_gdal.h"
//#include <gdal/gdal_priv.h>
#include <gdal_priv.h>

#include <iostream>
#include <exception>


using namespace std;

Grid<float> readRasterAsFloat(const std::filesystem::path& path);

template<typename T>
void writeRaster(Grid<T>& grid, const std::filesystem::path& path)
{
	//return;

	char** papszOptions = nullptr;
	int width = grid.width();
	int height = grid.height();
	GDALDataType type;
	if (std::is_same<T, float>::value) {
		type = GDALDataType::GDT_Float32;
	}
	else if (std::is_same<T, short>::value) {
		type = GDALDataType::GDT_Int16;
	}
	else if (std::is_same<T, int>::value) {
		type = GDALDataType::GDT_Int32;
	}
	else if (std::is_same<T, unsigned int>::value) {
		type = GDALDataType::GDT_UInt32;
	}
	else if (std::is_same<T, unsigned char>::value || std::is_same<T, FlowDir>::value) {
		type = GDALDataType::GDT_Byte;
	}
	else
	{
		throw runtime_error("Unsupported data type for writing. Curretly only byte,short, int, unsinged int and float datatypes are supported.");
	}

	GDALDataset* poDataset;
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

	GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName("GTiff");

	poDataset = poDriver->Create(path.string().c_str(), width, height, 1, type, papszOptions);
	if (poDataset == nullptr)
		throw runtime_error("Failed to create GeoTIFF file");

	poDataset->SetGeoTransform(grid.GeoTransform().data());

	GDALRasterBand* poBand;
	poBand = poDataset->GetRasterBand(1);
	poBand->SetNoDataValue((double)grid.noDataValue());

	CPLErr error = poBand->RasterIO(GF_Write, 0, 0, width, height,
		grid.getDataPtr(), width, height, type, 0, 0);

	GDALClose((GDALDatasetH)poDataset);
}
template<typename T>
bool typeMatch(GDALDataType type) {

	//check dataType and T match each other
	if (std::is_same<T, float>::value)
	{
		return type == GDALDataType::GDT_Float32;
	}
	else if (std::is_same<T, short>::value) {
		return type == GDALDataType::GDT_Int16;
	}
	else if (std::is_same<T, int>::value) {
		return type == GDALDataType::GDT_Int32;
	}
	else if (std::is_same<T, unsigned int>::value) {
		return type == GDALDataType::GDT_UInt32;
	}
	else if (std::is_same<T, unsigned char>::value || std::is_same<T, FlowDir>::value) {
		return type == GDALDataType::GDT_Byte;
	}
	else
	{
		throw runtime_error("Unsupported data type for writing. Curretly only byte,short, int, unsigned int and float datatypes are supported.");
	}
}

template<typename T>
Grid<T> readRaster(const std::filesystem::path& path)
{
	//Grid<T> grid;
	//return grid;

	GDALDataset* poDataset;
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

	poDataset = (GDALDataset*)GDALOpen(path.string().c_str(), GA_ReadOnly);
	const char* errorMsg = CPLGetLastErrorMsg();
	if (poDataset == nullptr)
		throw runtime_error("Failed to open the input GeoTIFF file:"+path.string());

	GDALRasterBand* poBand;
	poBand = poDataset->GetRasterBand(1);
	GDALDataType dataType = poBand->GetRasterDataType();	
	if (!typeMatch<T>(dataType)) {
		throw runtime_error("Data type does not match expected type");
	}

	int width = poBand->GetXSize();
	int height = poBand->GetYSize();

	Grid<T> grid(height, width);
	grid.setNoDataValue(poBand->GetNoDataValue());

	poDataset->GetGeoTransform(grid.GeoTransform().data());
	try
	{
		grid.allocate();
		poBand->RasterIO(GF_Read, 0, 0, width, height,
			(void*)grid.getDataPtr(), width, height, dataType, 0, 0);
		GDALClose((GDALDatasetH)poDataset);
		return grid;
	}
	catch (exception ex) {
		GDALClose((GDALDatasetH)poDataset);
		throw runtime_error(ex.what());
	}
}




