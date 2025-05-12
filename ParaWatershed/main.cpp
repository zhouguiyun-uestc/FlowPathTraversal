#include "tool.h"
int main(int argc, char* argv[])
{
	generateTiles("E:\\DEM\\flowdirs\\beijing_flow.tif", 100, 100, "E:\\DEM\\tiledflowdirs");
	return 0;
}