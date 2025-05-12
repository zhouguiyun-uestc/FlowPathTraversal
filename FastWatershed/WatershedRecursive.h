#pragma once
#include <Grid/grid.h>
#include <map>
void WatershedRecursive(Grid<FlowDir>& dirGrid, Grid<int>& wsGrid, const std::map<Cell, int>& outlets = std::map<Cell, int>());

