                                    Traceur

  What is it?
  -----------

  The Traceur project is an open-source raytracing engine written in
  C++, taking a modular approach to provide a powerful and flexible raytracing.


  Overview
  --------

  The Traceur project consists of several modules which together, form a 
  full-featured raytracing engine implementation, supporting multiple input 
  and output formats and raytracing kernels. The following is a list
  of all modules of the Traceur project with their descriptions:

  * core
    The core module provides the main interface of the raytracing engine, 
    abstracting away the raytracing kernels, acceleration structures, scene
    loaders and exporters.

  * loader-assimp
    The loader-assimp module provides a scene loaders using the Assimp library
    for parsing, providing support for up to 40 3D file formats.

  * export-ppm
    The export-ppm module provides a simple exporter for the ppm file format.

  * export-png
    The export-png module provides an exporter for the PNG file format using
    LodePNG.

  * frontend-cli
    The frontend-cli module provides a command line interface for the project.

  * frontend-glfw
    The frontend-glfw module provides a modern graphical frontend for the 
    project, which uses GLFW, GLEW and GLM for rendering purposes.

  * frontend-glut
    The frontend-glut module is the legacy GLUT frontend of the project based 
    on raytracing code provided  by the TI1806 course at 
    Delft University of Technolgy.


  Troubleshooting
  ---------------

  For troubleshooting, please visit the project wiki at
  https://github.com/fabianishere/traceur/wiki or take a look at the 
  documentation in the "docs" directory or online at 
  http://fabianishere.github.io/traceur/docs.


  Building
  --------

  CMake is used as building system, so please consult https://cmake.org for 
  installation instructions in case you haven't got.

  Download the source code as ZIP file from
  https://github.com/fabianishere/traceur/archive/master.zip or simply clone the
  repository at https://github.com/fabianishere/traceur using Git.

  After that, run the following commands in your command prompt in the source
  directory:

      $ mkdir build
      $ cd build
      $ cmake ..
      $ make

  to build the project.


  Documentation
  -------------

  The documentation is available in the "docs" directory or online at
  http://fabianishere.github.io/traceur/docs


  Licensing
  ---------

  The Traceur project is released under the MIT License.
  Please see the file called LICENSE.txt for more information.
