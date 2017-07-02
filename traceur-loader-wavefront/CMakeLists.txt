cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

# filesystem library shim
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/filesystem/filesystem")
	message(FATAL_ERROR
			"The filesystem library shim was not found in the repository. "
			"Did you clone the submodules?.")
endif()

add_library(CXX::FileSystem INTERFACE IMPORTED)
set_target_properties(CXX::FileSystem PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/deps/filesystem")

add_library(traceur-loader-wavefront STATIC
	include/traceur/loader/wavefront.hpp
	src/traceur/loader/wavefront.cpp
)
target_include_directories(traceur-loader-wavefront PUBLIC include/)
target_link_libraries(traceur-loader-wavefront traceur-core CXX::FileSystem)
