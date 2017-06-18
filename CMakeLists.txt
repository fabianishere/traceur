cmake_minimum_required(VERSION 3.5)
project(traceur)

# Use system libraries
option(USE_SYSTEM_LIBRARIES
	"Use system libraries instead of the libraries bundled in this distribution." OFF)

add_subdirectory(traceur-core)
add_subdirectory(traceur-frontend-glut)
add_subdirectory(traceur-loader-obj)
