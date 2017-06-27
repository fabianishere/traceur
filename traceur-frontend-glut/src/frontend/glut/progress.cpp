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
#include <vector>

#include <traceur/core/kernel/kernel.hpp>
#include <traceur/frontend/glut/progress.hpp>

traceur::TimeDuration::TimeDuration(double wall, double cpu) : wall(wall), cpu(cpu) {}
traceur::TimePoint::TimePoint() : wall(std::chrono::high_resolution_clock::now()), cpu(std::clock()) {}
traceur::TimeDuration traceur::TimePoint::operator-(const traceur::TimePoint &other) const
{
	return traceur::TimeDuration(
		std::chrono::duration_cast<std::chrono::duration<double>>(wall - other.wall).count(),
		double(cpu - other.cpu) / CLOCKS_PER_SEC
	);
}

void traceur::ConsoleProgressObserver::renderStarted(const traceur::Kernel &kernel,
													 const traceur::Scene &,
													 const traceur::Camera &,
													 int partitions)
{
	printf("[%s] Rendering scene\n", kernel.name().c_str());

	// Initialise timer
	start = traceur::TimePoint();

	// Initialise progress counters
	progress.total = partitions;
	progress.finished.store(0);
	partition_start.reserve(static_cast<size_t>(partitions));
	partition_end.reserve(static_cast<size_t>(partitions));

	// Initialise progress meters
	std::fill(bar.begin(), bar.end(), '-');
	printf("[%s] Progress: [%s] 0.0%% (0/0)", kernel.name().c_str(), bar.data());
	fflush(stdout);
}


void traceur::ConsoleProgressObserver::partitionStarted(const traceur::Kernel &,
														int id,
														const traceur::Film &film,
														const glm::ivec2 &offset)
{
	partition_start[id] = traceur::TimePoint();
}

void traceur::ConsoleProgressObserver::partitionFinished(const traceur::Kernel &kernel,
														 int id,
														 const traceur::Film &film,
														 const glm::ivec2 &offset)
{
	partition_end[id] = traceur::TimePoint();

	// Calculate percentage and increase progress meter
	int finished = ++progress.finished;
	float percentage = (float) finished / progress.total;

	// Update bar
	int index = static_cast<int>(percentage * (bar.size() - 1));
	bar[index] = '=';

	// Print progress meter
	printf("\r[%s] Progress: [%s] %0.f%% (%d/%d)", kernel.name().c_str(), bar.data(), percentage * 100, finished, progress.total);
	fflush(stdout);
}

void traceur::ConsoleProgressObserver::renderFinished(const traceur::Kernel &kernel,
													  const traceur::Film &)
{
	// Calculate the mean duration of a partition
	double wall_sum = 0;
	double cpu_sum = 0;
	for (int i = 0; i < progress.total; i++) {
		auto elapsed = partition_end[i] - partition_start[i];
		wall_sum += elapsed.wall;
		cpu_sum += elapsed.cpu;
	}
	double wall_mean = wall_sum / (double) progress.total;
	double cpu_mean = cpu_sum / (double) progress.total;

	// Finish progress meter
	printf("\n");

	// Calculate the elapsed time
	auto elapsed = traceur::TimePoint() - start;
	printf("[%s] Rendering done\n", kernel.name().c_str());
	printf(
		"[%s] cpu [%.3fs total, %.3fs mean], real [%.3fs total, %.3fs mean]\n",
		kernel.name().c_str(), elapsed.cpu, cpu_mean, elapsed.wall, wall_mean
	);
}
