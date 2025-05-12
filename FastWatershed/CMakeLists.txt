find_package(OpenMP REQUIRED)
find_package(GDAL REQUIRED CONFIG)

add_library(
    WatershedAlgorithms
	WatershedFlowPathTraversal.cpp
	WatershedFlowPathTraversal_OpenMP.cpp
	WatershedPathTracing.cpp
	WatershedRecursive.cpp 
	WatershedRegionGrowing.cpp 
	)

target_link_libraries(WatershedAlgorithms PUBLIC 
		OpenMP::OpenMP_CXX
		GridLib
		)

add_executable(
	FastWatershed
	main.cpp
	)

target_link_libraries( FastWatershed PUBLIC WatershedAlgorithms)

install(TARGETS FastWatershed        
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )
