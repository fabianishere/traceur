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

traceur::MultithreadedKernel::MultithreadedKernel(const std::shared_ptr<traceur::Kernel> kernel,
												  int workers,
												  int partitions)
	: workers(workers),
	  partitions(partitions),
	  range(std::pair<int, int>(0, partitions)),
	  kernel(kernel),
	  pool(workers) {}

traceur::MultithreadedKernel::MultithreadedKernel(const std::shared_ptr<traceur::Kernel> kernel,
												  int workers,
												  int partitions,
												  std::pair<int, int> range)
	: workers(workers),
	  partitions(partitions),
	  range(range),
	  kernel(kernel),
	  pool(workers) {}

std::unique_ptr<traceur::Film> traceur::MultithreadedKernel::render(const traceur::Scene &scene,
																	const traceur::Camera &camera) const
{
	/* Notify observers about start */
	for (auto &observer : observers) {
		observer->renderStarted(*this, scene, camera, partitions);
	}

	/* Create a partitioned film for the render job */
	auto film = std::make_unique<traceur::PartitionedFilm<traceur::DirectFilm>>(
		camera.viewport[2],
		camera.viewport[3],
		partitions
	);

	auto jobs = std::queue<std::future<void>>();

	/* Enqueue render jobs */
	for (int i = range.first; i < range.second; i++) {
		jobs.emplace(pool.enqueue([&, i]() {
			auto &partition = film->operator()(i);
			auto offset = film->offset(i);

			/* Notify observers about start */
			for (auto &observer : observers) {
				observer->partitionStarted(*this, i, partition, offset);
			}
			/* Render the partition */
			kernel->render(scene, camera, partition, offset);

			/* Notify observers about finish */
			for (auto &observer : observers) {
				observer->partitionFinished(*this, i, partition, offset);
			}
		}));
	}

	/* Wait for all render jobs to finish */
	while (!jobs.empty()) {
		auto job = std::move(jobs.front());
		jobs.pop();
		job.wait();
	}

	/* Notify observers about finish */
	for (auto &observer : observers) {
		observer->renderFinished(*this, *film);
	}

	return std::move(film);
}

void traceur::MultithreadedKernel::render(const traceur::Scene &scene,
										  const traceur::Camera &camera,
										  traceur::Film &film,
										  const glm::ivec2 &offset) const
{
	// XXX This method does not make use of the thread pool
	kernel->render(scene, camera, film, offset);
}

traceur::MultithreadedKernelPool::MultithreadedKernelPool(int workers)
	: stop(false)
{
	/* Create the worker threads */
	for (int i = 0; i < workers; i++) {
		pool.push_back(std::thread(traceur::MultithreadedKernelWorker(*this)));
	}
}

traceur::MultithreadedKernelPool::~MultithreadedKernelPool()
{
	stop = true;
	condition.notify_all();

	/* Join all workers */
	for (auto &worker : pool) {
		worker.join();
	}
}

void traceur::MultithreadedKernelWorker::operator()()
{
	std::function<void()> job;
	while (true) {
		{
			/* Lock the queue */
			std::unique_lock<std::mutex> lock(pool.mutex);

			/* Wait for available jobs */
			pool.condition.wait(lock, [this] {
				return pool.stop || !pool.jobs.empty();
			});

			/* Stop the worker if needed */
			if (pool.stop && pool.jobs.empty())
				return;

			/* Poll a render job from the queue */
			job = std::move(pool.jobs.front());
			pool.jobs.pop();
		}

		/* Run the render job */
		job();
	}
}
