cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(traceur-frontend-glut src/main.cpp src/mesh.cpp src/raytracing.cpp)
target_include_directories(traceur-frontend-glut PRIVATE include/)
target_link_libraries(traceur-frontend-glut traceur-loader-obj GLUT::GLUT OpenGL::GL)
