cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

add_library(traceur-loader-obj STATIC
	include/traceur/loader/obj.hpp
	src/traceur/loader/obj.cpp
)
target_include_directories(traceur-loader-obj PUBLIC include/)
target_link_libraries(traceur-loader-obj traceur-core)
