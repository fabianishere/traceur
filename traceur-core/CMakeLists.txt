cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

# Option to use threading
option(USE_THREADING "Add support for multi-threading ray-tracing kernels" ON)

# Threading
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
if (NOT THREADS_FOUND)
	# Check if thread library has been found.
	message(STATUS "Threading library not found. Disabling multi-threading ray-tracing kernels.")
	option(USE_THREADING "Add support for multi-threading ray-tracing kernels" OFF)
elseif(MINGW)
	# MinGW does not support std::async
	option(USE_THREADING "Add support for multi-threading ray-tracing kernels" OFF)
endif()

# Use an existing GLM installation on the system
option(USE_SYSTEM_GLM "Use an existing GLM installation on the system instead of the library bundled in this distribution." OFF)

# GLM
if (USE_SYSTEM_GLM)
	find_library(GLM REQUIRED)
	if (NOT GLM_FOUND)
		message(FATAL_ERROR 
			"The GLM library was not found on the system. "
			"Did you install it correctly?")
	endif()

	add_library(GLM::GLM INTERFACE IMPORTED)
	set_target_properties(GLM::GLM PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${GLM_INCLUDE_DIR}")
else()

	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/glm/glm")
		message(FATAL_ERROR 
			"The GLM library was not found in the repository. "
			"Did you clone the submodules?.")
	endif()

	add_library(GLM::GLM INTERFACE IMPORTED)
	set_target_properties(GLM::GLM PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/deps/glm")
endif()

add_library(traceur-core STATIC
	include/traceur/core/kernel/kernel.hpp
	include/traceur/core/kernel/pixel.hpp
	include/traceur/core/kernel/film.hpp
	include/traceur/core/kernel/hit.hpp
	include/traceur/core/kernel/ray.hpp
	include/traceur/core/kernel/basic.hpp
	include/traceur/core/kernel/multithreaded.hpp
	src/traceur/core/kernel/basic.cpp
	src/traceur/core/kernel/multithreaded.cpp

	include/traceur/core/lightning/light.hpp
	include/traceur/core/material/material.hpp
	include/traceur/core/scene/scene.hpp
	include/traceur/core/scene/camera.hpp
	include/traceur/core/scene/graph/graph.hpp
	include/traceur/core/scene/graph/builder.hpp
	include/traceur/core/scene/graph/factory.hpp
	include/traceur/core/scene/graph/visitor.hpp
	include/traceur/core/scene/graph/vector.hpp
	include/traceur/core/scene/graph/kdtree.hpp
	include/traceur/core/scene/primitive/primitive.hpp
	include/traceur/core/scene/primitive/sphere.hpp
	include/traceur/core/scene/primitive/triangle.hpp
	src/traceur/core/scene/graph/vector.cpp
	src/traceur/core/scene/graph/kdtree.cpp

	include/traceur/exporter/exporter.hpp
	include/traceur/loader/loader.hpp
)
target_include_directories(traceur-core PUBLIC include/)
target_link_libraries(traceur-core GLM::GLM)

if (USE_THREADING)
	message(STATUS "Using multi-threading ray-tracing kernels")
	target_compile_definitions(traceur-core PUBLIC -DUSE_THREADING=1)
	target_link_libraries(traceur-core Threads::Threads)
endif()

if(MSVC)
	# Force to always compile with W4
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# Update if necessary
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()
