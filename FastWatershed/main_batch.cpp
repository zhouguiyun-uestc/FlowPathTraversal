#include <string>
#include <iostream>
#include <Grid/io_gdal.h>
#include <Grid/FlowAccu.h>
#include "WatershedFlowPathTraversal.h"
#include "WatershedPathTracing.h"
#include "WatershedRegionGrowing.h"
#include "WatershedRecursive.h"
#include <Grid/FillDEMandComputeFlowDir.h>
#include <Grid/tool.h>
#include <Grid/util.h>
#include <map>
using namespace std;

#include <fstream>
#include <vector>

std::string trim(const std::string& str) {
	auto start = str.begin();
	while (start != str.end() && std::isspace(*start)) {
		start++;
	}

	auto end = str.end();
	while (end != start && std::isspace(*(end - 1))) {
		end--;
	}

	return std::string(start, end);
}

std::vector<std::string> readAlllines(const std::filesystem::path& path) {
	// 打开文件
	std::ifstream file(path);

	// 检查文件是否成功打开
	if (!file.is_open()) 
		throw runtime_error("cannot open file:" + path.string());
	

	// 用于存储每一行的向量
	std::vector<std::string> lines;
	std::string line;

	// 逐行读取文件
	while (std::getline(file, line)) {
		line = trim(line);
		if (line.length()>0)
			lines.push_back(line);
	}

	// 关闭文件
	file.close();

	// 输出读取到的所有行
	for (const auto& l : lines) {
		std::cout << l << std::endl;
	}

	return lines;
}
void write(map<string, int> times, const std::filesystem::path& path)
{
	// 打开文件用于写入
	std::ofstream outFile(path);

	// 检查文件是否成功打开
	if (!outFile.is_open()) {
		throw runtime_error("cannot create file:" + path.string());
	}

	// 遍历 map 并将键值对写入文件
	for (const auto& pair : times) {
		outFile << pair.first << "\t" << pair.second << std::endl;
	}

	// 关闭文件
	outFile.close();
}
std::vector<std::string> split(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::istringstream iss(str);
	std::string token;
	while (std::getline(iss, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}



int main(int argc, char* argv[])
{
	if (argc < 4)
	{
		cout << "FastWatershed supports GeoTIFF file format through thd GDAL library." << endl;
		cout << "\n Usage: FastWatershed [method] inputTextFile outputStatPath." << endl;
		cout << "\n [method] can be 'FastWatershed', 'FastWatershedGPU','Iterative','Recursive'" << endl;
		system("pause");
		return 1;
	}
	map<string, int> times;
	string algName = argv[1];

	string inputPath = argv[2];
	string outputPath = argv[3];
	try
	{

		if (!inputPath.ends_with(".txt")) {
			cout << "Input file must be a txt file" << endl;
			return 1;
		}
		auto paths=readAlllines(inputPath);


		for (auto filePath : paths) {
			if (algName == "FastTracing" || algName=="FastTracingOpenMP" || algName == "FastTracingGPU" || algName == "Iterative" || algName == "Recursive")
			{
				cout << "processing file:" << filePath <<endl;

				Grid<FlowDir> dirGrid = readRaster<FlowDir>(filePath);
				TimeSpan span;
				Grid<int> wsGrid(dirGrid);
				wsGrid.allocate();
				if (algName == "FastTracing")
					WatershedFlowPathTraversal(dirGrid, wsGrid);
				else if (algName == "FastTracingOpenMP")
					WatershedFlowPathTraversal_OpenMP(dirGrid, wsGrid);
				else if (algName == "FastWatershedGPU")
					WatershedFastGPU(dirGrid, wsGrid);
				else if (algName == "Recursive")
					WatershedRecursive(dirGrid, wsGrid);
				else if (algName == "PathTracing")
					WatershedByPathTracing(dirGrid, wsGrid);

				int time = span.elapsed_millseconds();
				auto filename_parts = split(std::filesystem::path(filePath).stem().string(), '_');
				times[filename_parts[0]] = time;
			}
			else {
				cout << "Unknown processing method!" << endl;
			}
		}
	}
	catch (exception& e) {
		cout << e.what() << endl;
		return 1;
	}
	write(times, outputPath);
	//把时间输出到文件
	return 0;
}