cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

# Dependencies
if (USE_SYSTEM_LIBRARIES)
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
	src/traceur/core/kernel/basic.cpp

	include/traceur/core/lightning/light.hpp
	include/traceur/core/material/material.hpp
	include/traceur/core/scene/scene.hpp
	include/traceur/core/scene/camera.hpp
	include/traceur/core/scene/graph/graph.hpp
	include/traceur/core/scene/graph/builder.hpp
	include/traceur/core/scene/graph/factory.hpp
	include/traceur/core/scene/graph/visitor.hpp
	include/traceur/core/scene/primitive/primitive.hpp
	include/traceur/core/scene/primitive/box.hpp
	include/traceur/core/scene/primitive/triangle.hpp
	src/traceur/core/scene/graph/vector.cpp

	include/traceur/exporter/exporter.hpp
	include/traceur/loader/loader.hpp
)
target_include_directories(traceur-core PUBLIC include/)
target_link_libraries(traceur-core GLM::GLM)
