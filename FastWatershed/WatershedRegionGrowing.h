#pragma once
#include <Grid/grid.h>
#include <map>
void WatershedRegionGrowing(Grid<FlowDir>& dirGrid, Grid<int>& wsGrid, const std::map<Cell, int>& outlets = std::map<Cell, int>());

