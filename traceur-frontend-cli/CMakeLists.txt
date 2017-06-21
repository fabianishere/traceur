cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

# getopt shim
add_library(getopt INTERFACE)
target_include_directories(getopt INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/deps/getopt)

# The module definition
add_executable(traceur-frontend-cli
	src/frontend/cli/main.cpp
)
target_include_directories(traceur-frontend-cli PRIVATE include/)
target_link_libraries(traceur-frontend-cli traceur-loader-obj traceur-exporter-ppm getopt)

# Copy sample assets in build directory
file(GLOB ASSETS "${CMAKE_SOURCE_DIR}/assets/*")
add_custom_command(TARGET traceur-frontend-cli
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/assets
	COMMAND ${CMAKE_COMMAND} -E copy ${ASSETS} ${CMAKE_CURRENT_BINARY_DIR}/assets
)
