cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

# Statically link the standard library when using MinGW.
if (MINGW)
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

# getopt shim for windows
add_library(getopt INTERFACE)
if (MSVC)
	target_include_directories(getopt INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/deps/getopt)
endif()

# The module definition
add_executable(traceur-frontend-cli
	src/frontend/cli/main.cpp
)
target_include_directories(traceur-frontend-cli PRIVATE include/)
target_link_libraries(traceur-frontend-cli traceur-loader-obj traceur-exporter-ppm getopt)

# Copy sample assets in build directory
add_custom_command(TARGET traceur-frontend-cli
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
