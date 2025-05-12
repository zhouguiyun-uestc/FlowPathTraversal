#pragma once
#include "grid.h"
//flow accumulation by Zhou et al. 2019
void computeFlowAccu(Grid<FlowDir>& dirGrid, Grid<unsigned int>& accuGrid);
