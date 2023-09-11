cmake_minimum_required(VERSION 3.12)
project(visualizer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Find source files
file(GLOB SOURCES "src/*.cpp")
include_directories(include ../linear-algebra/lib src)

# Set cache variables for GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(/home/elijah/src/glfw build)
find_package(OpenGL REQUIRED)

# Add source files
add_executable(visualizer ${SOURCES})

target_link_libraries(visualizer glfw)
target_link_libraries(visualizer GLEW)
target_link_libraries(visualizer OpenGL::GL)

