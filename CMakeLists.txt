cmake_minimum_required(VERSION 3.13)
project(Templates)

option(BUILD_DEBUG "Build the debug binary." off)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall)
if(BUILD_DEBUG)
	add_compile_options(-g)
endif()

file(GLOB_RECURSE sources "src/*.cpp")

add_executable(Templates ${sources})
set_target_properties(Templates PROPERTIES OUTPUT_NAME "template")

target_link_libraries(Templates stdc++fs)

target_include_directories(Templates PUBLIC "include" "lib")