
find_package(OpenMP REQUIRED)

add_library(
    ParaWatershedLib
	tool.cpp
	grid_info.h
)

target_link_libraries(ParaWatershedLib PUBLIC 
		OpenMP::OpenMP_CXX
		GridLib
		)
add_executable(
	ParaWatershedTest
	main.cpp
	)
target_link_libraries( ParaWatershedTest PUBLIC ParaWatershedLib)

install(TARGETS ParaWatershedTest        
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )

