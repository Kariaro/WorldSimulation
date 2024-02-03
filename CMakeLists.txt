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
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)
set(BUILD_SHARED_LIBS Off)

add_definitions(-DGLM_FORCE_SILENT_WARNINGS)

set(CMAKE_EXE_LINKER_FLAGS "-static -static-libstdc++ -std=c++20")
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# Find packages
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

SET(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
SET(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "")
SET(ASSIMP_NO_EXPORT ON CACHE BOOL "")
 
# Add third party sources
add_subdirectory(extern/glm)

add_subdirectory(includes/glad)
add_subdirectory(includes/stb_image)

# set(LIBS glfw3 opengl32 assimp freetype irrKlang)

link_directories(lib)
include_directories(includes)

# Compile options
add_compile_options(
	-lopengl32
	-static
#	-static-libgcc
#	-static-libstdc++
	-O3
# Error logging
	-Wall
	-Wextra
	-Wpedantic
	-Werror
# Removed
	-Wno-unused-parameter
	-Wno-unused-private-field
# Temporary
	-Wno-missing-field-initializers
	-Wno-missing-braces
)

enable_testing()
add_subdirectory(extern/googletest)

# Add source 
add_subdirectory(src/application)
add_subdirectory(src/ecs)
add_subdirectory(src/rendering)
add_subdirectory(src/utility/math)
