
find_package(CUDAToolkit REQUIRED)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(OpenMP REQUIRED)
find_package(GDAL REQUIRED CONFIG)

add_library(
    WatershedAlgorithms
	WatershedFlowPathTraversal.cpp
	WatershedFlowPathTraversal_OpenMP.cpp
	WatershedPathTracing.cpp
	WatershedRecursive.cpp 
	WatershedRegionGrowing.cpp 
	WatershedFast_gpu.cpp
	WatershedFast_gpu.cu
	)

target_link_libraries(WatershedAlgorithms PUBLIC 
		CUDA::cudart
		OpenMP::OpenMP_CXX
		GridLib
		)

add_executable(
	FastWatershed
	main.cpp
	)

add_executable(
	FastWatershedBatch
	main_batch.cpp
	)
add_executable(
	FastWatershedTest
	main_test.cpp
	)

target_link_libraries( FastWatershed PUBLIC WatershedAlgorithms)
target_link_libraries( FastWatershedBatch PUBLIC WatershedAlgorithms)
target_link_libraries( FastWatershedTest PUBLIC WatershedAlgorithms)


install(TARGETS FastWatershed        
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )
install(TARGETS FastWatershedBatch        
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )