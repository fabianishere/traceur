cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

# Dependencies
find_library(GLM REQUIRED)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${GLM_INCLUDE_DIR})

add_library(traceur-core STATIC
	src/traceur/core/kernel/basic.cpp
	src/traceur/core/scene/graph/vector.cpp
)
target_include_directories(traceur-core PUBLIC include/)
target_link_libraries(traceur-core glm)
