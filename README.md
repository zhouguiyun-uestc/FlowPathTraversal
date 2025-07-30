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

**"flowdirection"**: compute flow direction grid from input DEM file
Example usage: FastFlow flowdirection dem.tif flowdir.tif           
<br>
**"compare"**: detertimine whether two int32 grids are identical or not
          <p>  Example usage: FastFlow compare watershed1.tif watershed2.tif           
<br>
**"generateOutlets"**: calculate outlet files containing up to outlets of 255 biggest watersheds. The output outlet file contains 1-based row, col and label of each outlet.
       <p> Example usage: FastFlow generateOutlets flowdir.tif outlets.txt
 <br>          
**"FlowPathTraversal"**: use our proposed flow path traversal algorithm to calculate watershed. 
          <p>  Example usage 1: FastFlow FlowPathTraversal flowdir.tif watershed.tif   <br>//automatically calculate all draining outlets and delineate watersheds
         <p>   Example usage 2: FastFlow FlowPathTraversal flowdir.tif outlets.txt watershed.tif  <br>//use the specified outlet file to delineate watersheds       
 <br>
**"FlowPathTraversalOpenMP"**: use our proposed OpenMP-based parallel flow path traversal algorithm to delineate watersheds. 
       <p>     Example usage 1: FastFlow FlowPathTraversalOpenMP flowdir.tif watershed.tif   <br>//automatically calculate all draining outlets and delineate watersheds
        <p>    Example usage 2: FastFlow FlowPathTraversalOpenMP flowdir.tif outlets.txt watershed.tif  <br>//use the specified outlet file to delineate watersheds 
 <br>          
**"RegionGrowing"**: use region growing method to delineate watersheds
      <p>      Example usage 1: FastFlow RegionGrowing flowdir.tif watershed.tif   <br>//automatically calculate all draining outlets and delineate watersheds
       <p>     Example usage 2: FastFlow RegionGrowing flowdir.tif outlets.txt watershed.tif  <br>//use the specified outlet file to delineate watersheds 
<br>           
**"Recursive"**: use recursive method to delineate watersheds
       <p>     Example usage 1: FastFlow Recursive flowdir.tif watershed.tif   <br>//automatically calculate all draining outlets and delineate watersheds
      <p>      Example usage 2: FastFlow Recursive flowdir.tif outlets.txt watershed.tif  <br>//use the specified outlet file to delineate watersheds           
       
**Important Notice:**
<p>By default, the processed grids should contain cells no more than the maximum value hold in an unsigned int32 (which is 4294967295). 
If larger grids need to processed, please define the macro _MASSIVE_DATASET_ in grid.h to enable correct indexing of the cells. 
</p>

Detailed descriptions of the watershed delineation algorithms are provided in the manuscript. Please refer to the code for more details on its uages

Test Data:
Test DEM data used in the manuscript can be downloaded from:
http://www.mngeo.state.mn.us/

Please convert the data into GeoTIFF format using ArcGIS or another GIS tool before running the program.
