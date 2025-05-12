cmake_minimum_required (VERSION 3.20)

if( NOT WIN32)
	set(CMAKE_CUDA_ARCHITECTURES 86)
	set(CMAKE_CUDA_COMPILER "/usr/local/cuda-12.8/bin/nvcc")
	set (CMAKE_C_COMPILER "/usr/bin/gcc")
	set (CMAKE_CXX_COMPILER "/usr/bin/g++")
	set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

set(CMAKE_CUDA_ARCHITECTURES 86)
project (HydroDEM  LANGUAGES CXX CUDA )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -openmp:llvm")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:16777216")
	set(CMAKE_INSTALL_PREFIX "e:/DEM")
endif()


#显示详细头文件
if (false)
	#输出详细信息： 
	#Windows上： "buildCommandArgs": "-v",
	#Linux："buildCommandArgs": "-- VERBOSE=1",

	#get_cmake_property(VARS VARIABLES)
	#foreach(VAR ${VARS})
	#    if(VAR MATCHES "^GDAL_")
	#        message(STATUS "${VAR} = ${${VAR}}")
	#    endif()
	#endforeach()

	message(STATUS "CUDA Compiler: ${CMAKE_CUDA_COMPILER}")
	execute_process(COMMAND ${CMAKE_CUDA_COMPILER} --version
					OUTPUT_VARIABLE CUDA_VERSION_OUTPUT)
	message(STATUS "CUDA Version: ${CUDA_VERSION_OUTPUT}")

	message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
endif()


add_subdirectory(Grid)
add_subdirectory(FastWatershed)
add_subdirectory(ParaWatershed)



