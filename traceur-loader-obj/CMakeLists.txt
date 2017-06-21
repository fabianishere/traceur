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

add_library(traceur-loader-obj STATIC
	include/traceur/loader/obj.hpp
	src/traceur/loader/obj.cpp
)
target_include_directories(traceur-loader-obj PUBLIC include/)
target_link_libraries(traceur-loader-obj traceur-core CXX::FileSystem)
