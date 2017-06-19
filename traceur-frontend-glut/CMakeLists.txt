cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

# Use the system bundled GLUT framework on Apple
if (APPLE)
	option(USE_SYSTEM_GLUT "Use an existing GLUT installation on the system instead of the library bundled in this distribution." ON)
endif()

# Statically link the standard library when using MinGW.
if (MINGW)
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

# Use an existing GLUT installation on the system
option(USE_SYSTEM_GLUT "Use an existing GLUT installation on the system instead of the library bundled in this distribution." OFF)

# OpenGL dependency definition
find_package(OpenGL REQUIRED)
if (NOT OPENGL_FOUND)
	message(FATAL_ERROR "The OpenGL library was not found on the system.")
else()
	# NOTE: The OpenGL target is not defined for some older CMake versions,
	# so define it ourselves
	# This part is part of the CMake distribution
	# See CMake/Modules/FindOpenGL.cmake
	# Copyright 2000-2017 Kitware, Inc. and Contributors
	# Modified work copyright 2017 Traceur project
	if (NOT TARGET OpenGL::GL)
		add_library(gl INTERFACE)
		if(IS_ABSOLUTE "${OPENGL_gl_LIBRARY}")
			if(OPENGL_gl_LIBRARY MATCHES "/([^/]+)\\.framework$")
				set(_gl_fw "${OPENGL_gl_LIBRARY}/${CMAKE_MATCH_1}")
				if(EXISTS "${_gl_fw}.tbd")
					string(APPEND _gl_fw ".tbd")
				endif()
				target_link_libraries(gl INTERFACE "${_gl_fw}")
			else()
				target_link_libraries(gl INTERFACE "${OPENGL_gl_LIBRARY}")
			endif()
		else()
			target_link_libraries(gl INTERFACE "${OPENGL_gl_LIBRARY}")
		endif()

		target_compile_definitions(gl INTERFACE "${OPENGL_INCLUDE_DIR}")
		add_library(OpenGL::GL ALIAS gl)
	endif()
	if (OPENGL_GLU_FOUND AND NOT TARGET OpenGL::GLU)
		add_library(glu INTERFACE)
		if(IS_ABSOLUTE "${OPENGL_glu_LIBRARY}")
			if(OPENGL_glu_LIBRARY MATCHES "/([^/]+)\\.framework$")
				set(_glu_fw "${OPENGL_glu_LIBRARY}/${CMAKE_MATCH_1}")
				if(EXISTS "${_glu_fw}.tbd")
					string(APPEND _glu_fw ".tbd")
				endif()
				target_link_libraries(glu INTERFACE OpenGL::GL "${_glu_fw}")
			else()
				target_link_libraries(glu INTERFACE OpenGL::GL "${OPENGL_glu_LIBRARY}")
			endif()
		else()
			target_link_libraries(glu INTERFACE OpenGL::GL "${OPENGL_glu_LIBRARY}")
		endif()
		add_library(OpenGL::GLU ALIAS glu)
	endif()
endif()

# FreeGLUT dependency definition
if (USE_SYSTEM_GLUT)
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

# The module definition
add_executable(traceur-frontend-glut
	include/traceur/frontend/glut/visitor.hpp
	include/imageWriter.h
	include/matrix.h
	include/raytracing.h
	include/traqueboule.h

	src/main.cpp
	src/raytracing.cpp
	src/frontend/glut/visitor.cpp
)
target_include_directories(traceur-frontend-glut PRIVATE include/)
target_link_libraries(traceur-frontend-glut traceur-loader-obj GLUT::GLUT OpenGL::GLU)
