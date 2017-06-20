cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

add_library(traceur-exporter-ppm STATIC
	include/traceur/exporter/ppm.hpp
	src/traceur/exporter/ppm.cpp
)
target_include_directories(traceur-exporter-ppm PUBLIC include/)
target_link_libraries(traceur-exporter-ppm traceur-core)
