# Create output binary
add_executable(simulation
    src/main.cpp
	)
target_sources(simulation
	PUBLIC
	)
target_include_directories(simulation
    PUBLIC public_include
	)
target_link_libraries(simulation
	PUBLIC glfw3
	PUBLIC ecs
	PUBLIC glm
	PUBLIC rendering
	PUBLIC wms_client
	)
