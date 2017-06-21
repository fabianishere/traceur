/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Traceur authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <ctime>
#include <memory>
#include <filesystem/path.h>
#include <iostream>

/* Use shim for getopt on Windows */
#ifdef _WIN32
#include <getopt.h>
#else
#include <unistd.h>
#endif


#include <glm/glm.hpp>

#include <traceur/core/kernel/basic.hpp>
#include <traceur/core/scene/graph/factory.hpp>
#include <traceur/core/scene/graph/vector.hpp>
#include <traceur/loader/obj.hpp>
#include <traceur/exporter/ppm.hpp>

/**
 * The main entry point of the program.
 *
 * @param[in] argc The number of command line arguments passed to this program.
 * @param[in] argv The command line arguments passed to this program.
 */
int main(int argc, char** argv)
{
	int c = -1;
	int width = 800;
	int height = 800;

	// Set camera directions
	glm::vec3 eye = glm::vec3(2, 2, 4);
	glm::vec3 center = glm::vec3(0, 0, -1);
	glm::vec3 up = glm::vec3(0, 1, 0);

	float x = 0.f, y = 0.f, z = 0.f;
	while ((c = getopt(argc, argv, "w:h:e:c:u:")) != -1) {
		switch(c) {
			case 'w':
				width = atoi(optarg);
				break;
			case 'h':
				height = atoi(optarg);
			case 'e':
				sscanf(optarg, "(%f, %f, %f)", &x, &y, &z);
				eye = glm::vec3(x, y, z);
				break;
			case 'c':
				sscanf(optarg, "(%f, %f, %f)", &x, &y, &z);
				center = glm::vec3(x, y, z);
				break;
			case 'u':
				sscanf(optarg, "(%f, %f, %f)", &x, &y, &z);
				up = glm::vec3(x, y, z);
				break;
			default:
				continue;
		}
	}

	auto factory = traceur::make_factory<traceur::VectorSceneGraphBuilder>();
	auto loader = std::make_unique<traceur::ObjLoader>(std::move(factory));
	auto kernel = std::make_unique<traceur::BasicKernel>();
	auto exporter = std::make_unique<traceur::PPMExporter>();

	// Set up viewport
	glm::ivec4 viewport = glm::ivec4(0, 0, width, height);

	// Set up the camera
	traceur::Camera camera = traceur::Camera(viewport)
			.lookAt(eye, center - eye, up)
			.perspective(glm::radians(50.f), 1, 0.01, 10);

	for (int i = optind, j = 1; i < argc; i++, j++) {
		auto path = filesystem::path(argv[i]);

		printf("[%d] Loading scene at path \"%s\"\n", j, argv[i]);
		auto scene = loader->load(path.str());

		printf("[%d] Rendering scene (kernel = %s)\n", j, "basic");

		// Time the ray tracing
		clock_t begin = std::clock();

		// Render the scene and capture the result
		auto result = kernel->render(*scene, camera);

		// Calculate the elapsed time
		clock_t end = std::clock();
		double secs = double(end - begin) / CLOCKS_PER_SEC;
		printf("[%d] Rendering took %f seconds\n", j, secs);

		// Export the result to a file
		auto target = path.filename() + ".ppm";
		exporter->write(*result, target);
		printf("[%d] Saved result to %s\n", j, target.c_str());
	}
	return 0;
}