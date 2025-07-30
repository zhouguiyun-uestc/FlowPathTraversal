Manuscript Title:
An Efficient Flow Path Traversal Algorithm for Watershed Delineation from Raster Digital Elevation Models for Multicore Architectures

Authors:
Guiyun Zhou, Zihao Zhang, Suhua Fu, Jiayuan Lin

Corresponding author:
Guiyun Zhou (zhouguiyun@uestc.edu.cn)

This repository contains the source code for the FlowPathTraversal algorithm proposed in the manuscript above. The algorithm is designed for efficient flow path traversal and watershed delineation from raster Digital Elevation Models (DEMs), with a focus on performance in multicore computing environments. All experiments and evaluations presented in the manuscript were conducted using this code.

The program can be compiled and executed on both Windows and Linux platforms.

FlowPathTraversal processes GeoTIFF files via the GDAL library. Please make sure the GDAL library is properly linked during compilation. 

Usage:
The program provides a set of different methods. 
FastWatershed [Method] other_parameters

The supported methods include:
"flowdirection": compute flow direction grid from input DEM file
           Example usage: FastFlow flowdirection dem.tif flowdir.tif
"compare": detertimine whether two int32 grids are identical or not
           Example usage: FastFlow compare watershed1.tif watershed2.tif
"generateOutlets": calculate outlet files containing up to outlets of 255 biggest watersheds 
           Example usage: FastFlow generateOutlets flowdir.tif outlets.txt
           

Description:
The algorithm supports parallel execution on multicore processors, making it suitable for high-resolution and large-scale DEM datasets.
Full technical details and performance evaluation are provided in the manuscript.

Test Data:
Test DEM data used in the manuscript can be downloaded from:
http://www.mngeo.state.mn.us/

Please convert the data into GeoTIFF format using ArcGIS or another GIS tool before running the program.
