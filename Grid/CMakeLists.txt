find_package(GDAL REQUIRED CONFIG)

add_library(
    GridLib
    io_gdal.cpp
	FillDEMandComputeFlowDir.cpp
	FlowAccu.cpp 
	tool.cpp 	
	util.cpp
	)

target_link_libraries(GridLib PUBLIC 
		GDAL::GDAL 
		)
target_include_directories(GridLib 
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>)

if (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
	target_compile_options(GridLib PUBLIC "/Od")
	target_compile_definitions(GridLib PUBLIC DEBUG)
endif()














