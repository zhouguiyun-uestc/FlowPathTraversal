cmake_minimum_required (VERSION 3.20)

if( NOT WIN32)
	set(CMAKE_CUDA_ARCHITECTURES 86)
	set(CMAKE_CUDA_COMPILER "/usr/local/cuda-12.8/bin/nvcc")
	set (CMAKE_C_COMPILER "/usr/bin/gcc")
	set (CMAKE_CXX_COMPILER "/usr/bin/g++")
	set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

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




