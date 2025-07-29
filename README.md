Manuscript Title:
An Efficient Flow Path Traversal Algorithm for Watershed Delineation from Raster Digital Elevation Models for Multicore Architectures

Authors:
Guiyun Zhou, Zihao Zhang, Suhua Fu, Jiayuan Lin

Corresponding Author:
Guiyun Zhou (zhouguiyun@uestc.edu.cn)

This repository contains the source code for the FlowPathTraversal algorithm proposed in the manuscript above. The algorithm is designed for efficient flow path traversal and watershed delineation from raster Digital Elevation Models (DEMs), with a focus on performance in multicore computing environments. All experiments and evaluations presented in the manuscript were conducted using this code.

The program can be compiled and executed on both Windows and Linux platforms.

FlowPathTraversal supports floating-point GeoTIFF DEM files via the GDAL library. Please make sure the GDAL library is properly linked during compilation. GDAL version 1.91 was used in our experiments.

Usage:
The algorithm can be run using the following command-line syntax:
FastWatershed Method Input_DEM_flow (optional:Input_DEM_flow_outlets) output_DEM_watershed

Example:
FastWatershed FlowPathTraversal dem_flow.tif (dem_outlets.txt) dem_ws.tif

Description:
The algorithm supports parallel execution on multicore processors, making it suitable for high-resolution and large-scale DEM datasets.
Full technical details and performance evaluation are provided in the manuscript.

Test Data:
Test DEM data used in the manuscript can be downloaded from:
http://www.mngeo.state.mn.us/

Please convert the data into GeoTIFF format using ArcGIS or another GIS tool before running the program.
