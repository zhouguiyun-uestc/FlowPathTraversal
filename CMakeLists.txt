cmake_minimum_required (VERSION 3.20)

project (FastWatershed  LANGUAGES CXX CUDA )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -openmp:llvm")
endif()

add_subdirectory(Grid)
add_subdirectory(FastWatershed)




