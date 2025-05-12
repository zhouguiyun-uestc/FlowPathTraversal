#pragma once
#include <filesystem>
#include "grid.h"

void FillDEMandComputeFlowDir(const std::filesystem::path& inputPath, const std::filesystem::path& outputPath);