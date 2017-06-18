cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)

if (USE_SYSTEM_LIBRARIES OR APPLE)
	find_package(GLUT REQUIRED)
	if (NOT GLUT_FOUND)
		message(FATAL_ERROR 
			"The GLUT library was not found on the system. "
			"Did you install it correctly?")
	endif()
else()
	# NOTE: FreeGLUT does not build on macOS
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/freeglut/freeglut")
		message(FATAL_ERROR 
			"The FreeGLUT library was not found in the repository. "
			"Did you clone the submodules?.")
	endif()

	option(FREEGLUT_BUILD_SHARED_LIBS "" OFF)
	option(FREEGLUT_BUILD_STATIC_LIBS "" ON)
	option(FREEGLUT_BUILD_DEMOS "" OFF)

	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/freeglut/freeglut/freeglut)
	target_include_directories(freeglut_static PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/freeglut/freeglut/freeglut/include>)
	add_library(GLUT::GLUT ALIAS freeglut_static)
endif()

add_executable(traceur-frontend-glut src/main.cpp src/mesh.cpp src/raytracing.cpp)
target_include_directories(traceur-frontend-glut PRIVATE include/)
target_link_libraries(traceur-frontend-glut traceur-loader-obj GLUT::GLUT OpenGL::GL)
