add_library(geodecy INTERFACE)
target_include_directories(geodecy
    PUBLIC INTERFACE public_include
	)
target_link_libraries(geodecy
	INTERFACE glm
	)

add_subdirectory(test)
