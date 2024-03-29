cmake_minimum_required(VERSION 3.27)
cmake_policy(SET CMP0067 NEW) 

project(
	"Simulation"
	VERSION 1.0
	LANGUAGES C CXX
)

# Configure cross platform compilation without windows specific directories
set(TEST_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/build/exe)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libstdc++ -std=c++20")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")

add_definitions(-DGLM_FORCE_SILENT_WARNINGS)

# Find packages
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in '${GLFW3_INCLUDE_DIR}'")

# TODO: Figure out static lib in the future
# set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
	set(HTTPLIB_REQUIRE_OPENSSL ON)
else()
	message(WARNING "Could not find OpenSSL, will not be able to download map data from HTTPS")
endif()
set(HTTPLIB_COMPILE ON)
set(HTTPLIB_NO_EXCEPTIONS ON)
add_subdirectory(extern/cpp-httplib)


# Link directory
link_directories(lib)

# Create imgui library
set(IMGUI_PATH "extern/imgui")
file(GLOB IMGUI_SOURCES "${IMGUI_PATH}/*.cpp")
set(IMGUI_BACKENDS_SOURCES
	"extern/imgui/backends/imgui_impl_glfw.cpp"
	"extern/imgui/backends/imgui_impl_opengl2.cpp"
	"extern/imgui/backends/imgui_impl_opengl3.cpp")
add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_BACKENDS_SOURCES})
target_link_libraries(imgui PRIVATE glfw3)
target_include_directories(imgui
	PUBLIC ${IMGUI_PATH}
	PUBLIC ${IMGUI_PATH}/backends)

# Add third party sources
add_subdirectory(extern/glm)
add_subdirectory(extern/pugixml)

add_subdirectory(includes/glad)
add_subdirectory(includes/stb_image)

include_directories(includes)

enable_testing()
add_subdirectory(extern/googletest)

# Compile options
add_link_options(
	-static
	-std=c++20
)
add_compile_options(
	-O3
# Error logging
	-Wall
	-Wextra
	-Wpedantic
	-Werror
# Removed (TODO Try make the build as clean as possible and do not randomly remove warnings)
	-Wno-unused-parameter
	-Wno-unused-private-field
	-Wno-unused-command-line-argument
	-Wno-language-extension-token
	-Wno-missing-field-initializers
	-Wno-missing-braces
)

# Add source 
add_subdirectory(src/application)
add_subdirectory(src/ecs)
add_subdirectory(src/rendering)
add_subdirectory(src/geodecy)
add_subdirectory(src/wmsclient)
