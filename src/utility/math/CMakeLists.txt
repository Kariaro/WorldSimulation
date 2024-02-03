add_library(utility_math INTERFACE)
#target_sources(utility_math
#	PUBLIC
#	)
target_include_directories(utility_math
    PUBLIC INTERFACE public_include
	)
target_link_libraries(utility_math
	INTERFACE glm
	)

add_subdirectory(test)