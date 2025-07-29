#pragma once
#include <filesystem>
#include "flowDir.h"
#include "grid.h"
#include "cell.h"
#include <map>
#include <algorithm>

template<typename T>
bool compareGrids(Grid<T>& grid1, Grid<T>& grid2) {
	if (grid1.height() != grid2.height()) return false;
	if (grid1.width() != grid2.width()) return false;

	for (int r = 0; r < grid1.height(); r++)
		for (int c = 0; c < grid1.width(); c++)
		{
			Cell cell(r, c);
			if (grid1.isNoData(cell) && grid2.isNoData(cell)) continue;

			if (grid1(cell) != grid2(cell)) 
				return false;
		}
	return true;
}

inline void keepFirstNLargestByValue(std::map<int, int>& myMap, size_t N) {
	if (N >= myMap.size()) {
		return; // No action needed if N is too large
	}

	// Copy map entries to a vector for sorting
	std::vector<std::pair<int, int>> entries(myMap.begin(), myMap.end());

	// Sort the vector in descending order of values
	std::sort(entries.begin(), entries.end(),
		[](const auto& a, const auto& b) { return a.second > b.second; });

	// Keep only the first N entries
	entries.resize(N);

	// Rebuild the map from the remaining entries
	myMap.clear();
	myMap.insert(entries.begin(), entries.end());
}


template<typename T>
std::map<T,int> computeStats(Grid<T>& grid, int N) {
	std::map<T, int> stats;

	for (int r = 0; r < grid.height(); r++)
		for (int c = 0; c < grid.width(); c++)
		{
			Cell cell(r, c);
			if (grid.isNoData(cell)) continue;
			stats[grid(cell)]++;
		}
	
	keepFirstNLargestByValue(stats, N);

	return stats;
}

Cell getNeighboringCell(const Cell& c, int i);
FlowDir getReverseDir(int i);

FlowDir computeFlowDir(Grid<float>& dem, const Cell& c, float spill);
bool moveToDownstreamCell(Grid<FlowDir>& dirGrid, Cell& c,int* dirIndex=nullptr);

Grid<char> computeNIPSGrid(Grid<FlowDir>& dirGrid);