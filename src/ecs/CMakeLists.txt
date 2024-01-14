add_library(ecs INTERFACE)
#target_sources(ecs
#	PUBLIC
#	)
target_include_directories(ecs
    PUBLIC INTERFACE public_include
	)
target_link_libraries(ecs
	)
