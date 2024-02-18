add_library(rendering)
target_sources(rendering
	PUBLIC planet/src/worldrender.cpp
	PUBLIC planet/src/quadsphere.cpp
	)
target_include_directories(rendering
    PUBLIC planet/public_include
	)
target_link_libraries(rendering
	PUBLIC glfw3
	PUBLIC glad
	PUBLIC stb_image
	PUBLIC opengl32
	PUBLIC geodecy
	PUBLIC imgui
	PUBLIC wms_client
	)
