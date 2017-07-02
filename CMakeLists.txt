cmake_minimum_required(VERSION 3.5)
project(traceur)

add_subdirectory(traceur-core)
add_subdirectory(traceur-frontend-glut)
add_subdirectory(traceur-frontend-cli)
add_subdirectory(traceur-loader-wavefront)
add_subdirectory(traceur-exporter-ppm)
