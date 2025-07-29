#include "util.h"
#include "cell.h"
#include <fstream>
#include <iostream>

using namespace std;
void writeOutletFile(std::map<int, int>& keyCellCount, std::map<int, Cell> key2Cell, const std::filesystem::path& outletFilePath)
{
	//Éú³Éoutlet file
	std::fstream file(outletFilePath, std::ios::out);
	int row, col, label;
	int newLabel = 1;
	for (auto [key, count] : keyCellCount) {
		Cell c = key2Cell.at(key);
		file << c.row + 1 << "\t" << c.col + 1 << "\t" << newLabel << endl;
		newLabel++;
	}
	file.close();
}

std::map<Cell, int> loadOutletLocations(const std::filesystem::path& outletFilePath)
{
	std::map<Cell, int> cell2Label;

	std::fstream file(outletFilePath, std::ios::in);
	int row, col, label;
	//int times = 0;
	while (file >> row >> col >> label)
	{
		//times++;
		//cout << times << endl;
		cell2Label[Cell(row - 1, col - 1)] = label;
	}

	file.close();
	return cell2Label;
}