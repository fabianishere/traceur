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

#include <traceur/core/kernel/multithreaded.hpp>

traceur::MultithreadedKernel::MultithreadedKernel(const std::shared_ptr<traceur::Kernel> kernel, int N) :
	kernel(kernel), N(N) {}

std::unique_ptr<traceur::Film> traceur::MultithreadedKernel::render(
		const traceur::Scene &scene,
		const traceur::Camera &camera) const
{
	auto film = std::make_unique<traceur::PartitionedFilm<traceur::DirectFilm>>(
			camera.viewport[2],
			camera.viewport[3],
			N
	);
	auto tasks = std::vector<std::future<void>>(film->n);

	/* Initialise threads */
	for (int i = 0; i < film->n; i++) {
		auto &partition = (*film)(i);
		auto offset = glm::ivec2(
				(i * partition.width) % film->width,
				(i / film->columns) * partition.height
		);

		tasks[i] = std::async(std::launch::async, [this, &scene, offset, &partition, &camera] {
			this->kernel->render(scene, camera, partition, offset);
		});
	}

	/* Wait for all tasks */
	for (auto &task : tasks) {
		task.wait();
	}

	return std::move(film);
}

void traceur::MultithreadedKernel::render(
		const traceur::Scene &scene,
		const traceur::Camera &camera,
		traceur::Film &film,
		const glm::ivec2 &offset) const
{
	kernel->render(scene, camera, film, offset);
}