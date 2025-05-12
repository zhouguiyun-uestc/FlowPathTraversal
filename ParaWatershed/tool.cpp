#include <gdal_priv.h>
#include <string>
#include <stdexcept>
#include <cmath>
#include <fstream>
#include <iostream>
#include "grid_info.h"
#include <Grid/flowDir.h>
#include <Grid/grid.h>
#include <Grid/io_gdal.h>

//read the input geotiff file and create tiles from them directly
void generateTiles(const std::filesystem::path& filePath, int tileHeight, int tileWidth, const std::filesystem::path& outputFolder) {
    std::filesystem::path inputFilePath = filePath;
    std::filesystem::path output = outputFolder;
    GDALAllRegister();
    GDALDataset* fin = (GDALDataset*)GDALOpen(inputFilePath.string().c_str(), GA_ReadOnly);
    if (fin == nullptr)
        throw std::runtime_error("Could not open file '" + inputFilePath.string() + "' to get dimensions.");
    
    GDALRasterBand* band = fin->GetRasterBand(1);
    GDALDataType type = band->GetRasterDataType();
    int grandHeight = band->GetYSize();
    int grandWidth = band->GetXSize();
    std::array< double,6 > geotransform;
    fin->GetGeoTransform(&geotransform[0]);
    int height, width;

    int gridHeight = std::ceil((double)grandHeight / tileHeight);
    int gridWidth = std::ceil((double)grandWidth / tileWidth);

    for (int tileRow = 0; tileRow < gridHeight; tileRow++) {
        for (int tileCol = 0; tileCol < gridWidth; tileCol++) {
            auto outputFileName = std::to_string(tileRow) + "_" + std::to_string(tileCol) + ".tif";
            auto path = output/ outputFileName;
            //rows and cols to be read 
            height = (grandHeight - tileHeight * tileRow >= tileHeight) ? tileHeight : (grandHeight - tileHeight * tileRow);
            width = (grandWidth - tileWidth * tileCol >= tileWidth) ? tileWidth : (grandWidth - tileWidth * tileCol);
            
            Grid<FlowDir> tile(height,width);
            tile.allocate();

            auto returnValue = band->RasterIO(GF_Read, tileWidth * tileCol, tileHeight * tileRow,
                tile.width(), tile.height(), (void*)tile.getDataPtr(), tile.width(), tile.height(), type, 0, 0);
            if (returnValue != CE_None) {
                throw std::runtime_error("An error occured while trying to read '" + path.string() + " 'into RAM with GDAL.");
            }
            std::array< double,6 > tileGeotransform(geotransform);
            tileGeotransform[0] = geotransform[0] + tileWidth * tileCol * geotransform[1] + tileHeight * tileRow * geotransform[2];
            tileGeotransform[3] = geotransform[3] + tileHeight * tileRow * geotransform[5] + tileWidth * tileCol * geotransform[4];
            tile.GeoTransform() = tileGeotransform;
            writeRaster(tile, path);
        }
    }
    GDALClose((GDALDatasetH)fin);

    auto txtPath = output /"tileInfo.txt";
    std::ofstream fout;
    fout.open(txtPath);
    if (fout.fail()) {
        std::cerr << "Open " << txtPath << " error!" << std::endl;
        return;
    }
    for (int tileRow = 0; tileRow < gridHeight; tileRow++) {
        for (int tileCol = 0; tileCol < gridWidth; tileCol++) {
            std::string outputFileName = std::to_string(tileRow) + "_" + std::to_string(tileCol) + ".tif";
            auto path = output/outputFileName;
            fout << path << ",";
        }
        fout << std::endl;
    }
}

void mergeTiles(GridInfo& gridInfo, const char* outputFilePath) {
    int grandHeight = gridInfo.grandHeight;
    int grandWidth = gridInfo.grandWidth;
    int gridHeight = gridInfo.gridHeight;
    int gridWidth = gridInfo.gridWidth;
    int tileHeight = gridInfo.tileHeight;
    int tileWidth = gridInfo.tileWidth;
    std::string inputFolder = gridInfo.outputFolder;
    Grid<FlowDir> tiles(grandHeight, grandWidth);
    tiles.allocate();

    for (int tileRow = 0; tileRow < gridHeight; tileRow++) {
        for (int tileCol = 0; tileCol < gridWidth; tileCol++) {
            std::string fileName = inputFolder + "/" + std::to_string(tileRow) + "_" + std::to_string(tileCol) + ".tif";
            
            Grid<FlowDir> tile=readRaster<FlowDir>(fileName);
            if (tileRow == 0 && tileCol == 0) {
                tiles.GeoTransform() = tile.GeoTransform();
            }

            int height = tile.height();
            int width = tile.width();
            int startRow = tileHeight * tileRow;
            int startCol = tileWidth * tileCol;
            for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                    tiles(startRow + row, startCol + col) = tile(row, col);
                }
            }
        }
    }
    writeRaster(tiles, outputFilePath);
}