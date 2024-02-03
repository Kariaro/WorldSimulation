add_library(rendering)
target_sources(rendering
	PUBLIC planet/src/planet.cpp
	)
target_include_directories(rendering
    PUBLIC planet/public_include
	)
target_link_libraries(rendering
	PUBLIC glfw3
	PUBLIC glad
	PUBLIC stb_image
	PUBLIC opengl32
	PUBLIC utility_math
	)
