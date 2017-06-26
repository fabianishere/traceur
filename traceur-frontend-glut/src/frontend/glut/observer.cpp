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

#include <map>
#include <stdio.h>

#include <traceur/core/kernel/kernel.hpp>
#include <traceur/frontend/glut/observer.hpp>

extern std::vector<std::tuple<glm::ivec2, glm::ivec2, bool>> parts;

void traceur::ProgressObserver::renderStarted(const traceur::Kernel &kernel,
											  const traceur::Scene &,
											  const traceur::Camera &,
											  int partitions)
{
	parts.clear();
	parts.resize(static_cast<unsigned long>(partitions));

	/* Start timing and progress */
	this->partitions = partitions;
	printf("[%s] Rendering scene\n", kernel.name().c_str());

	// Time the ray tracing
	wall = std::chrono::high_resolution_clock::now();
	cpu = std::clock();
	finished.store(0);

	printf("[%s] progress: 0%%", kernel.name().c_str());
	fflush(stdout);
}


void traceur::ProgressObserver::partitionStarted(const traceur::Kernel &,
												 int id,
												 const traceur::Film &film,
												 const glm::ivec2 &offset)
{
	parts[id] = std::tuple<glm::ivec2, glm::ivec2, bool>(glm::ivec2(film.width, film.height), offset, false);
}

void traceur::ProgressObserver::partitionFinished(const traceur::Kernel &kernel,
												  int id,
												  const traceur::Film &film,
												  const glm::ivec2 &offset)
{
	/* Increase progress meter */
	int finished = ++this->finished;
	float percentage = ((float)finished / partitions) * 100;
	printf("\r[%s] Progress: %0.f%% (%d/%d)", kernel.name().c_str(), percentage, finished, partitions);
	fflush(stdout);

	parts[id] = std::tuple<glm::ivec2, glm::ivec2, bool>(glm::ivec2(film.width, film.height), offset, true);
}

void traceur::ProgressObserver::renderFinished(const traceur::Kernel &kernel,
											   const traceur::Film &)
{
	/* Finish progress meter */
	printf("\n");
	// Calculate the elapsed time
	auto wallB = std::chrono::high_resolution_clock::now();
	auto cpuB = std::clock();
	double elapsed_wall = std::chrono::duration_cast<std::chrono::duration<double>>(wallB - wall).count();
	double elapsed_cpu = double(cpuB - cpu) / CLOCKS_PER_SEC;
	printf("[%s] Rendering done (cpu %.3fs, real %.3fs)\n", kernel.name().c_str(), elapsed_cpu, elapsed_wall);
}
