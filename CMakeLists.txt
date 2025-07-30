cmake_minimum_required (VERSION 3.20)

set(CMAKE_CUDA_ARCHITECTURES 86)

project (FastWatershed  LANGUAGES CXX )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -openmp:llvm")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:16777216")
endif()

add_subdirectory(Grid)
add_subdirectory(FastWatershed)




