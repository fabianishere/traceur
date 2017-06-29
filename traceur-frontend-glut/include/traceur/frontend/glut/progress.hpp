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
#ifndef TRACEUR_FRONTEND_GLUT_PROGRESS_H
#define TRACEUR_FRONTEND_GLUT_PROGRESS_H

#include <atomic>
#include <chrono>
#include <cstdio>

#include <traceur/core/kernel/observer.hpp>

namespace traceur {
	/**
	 * A duration of time in wall and cpu time which a render job or partition took to render.
	 */
	struct TimeDuration {
		/**
		 * Construct a {@link TimeDuration} instance.
		 *
		 * @param[in] wall The duration in wall time (seconds).
		 * @param[in] cpu The duration in cpu time (seconds).
		 */
		TimeDuration(double, double);

		/**
		 * The duration in wall time (seconds).
		 */
		double wall;

		/**
		 * The duration in cpu time (seconds).
		 */
		double cpu;
	};

	/**
	 * A point in (wall and cpu) time, which can be used for calculations about the duration of a render job.
	 */
	struct TimePoint {
		/**
		 * Construct a {@link TimePoint} instance at the current point in wall and cpu time.
		 */
		TimePoint();

		/**
		 * Calculate the difference between two time points.
		 *
		 * @param[in] that The other time point to subtract.
		 */
		traceur::TimeDuration operator-(const traceur::TimePoint &) const;

		/**
		 * The time point in wall time.
		 */
		std::chrono::time_point<std::chrono::high_resolution_clock> wall;

		/**
		 * The time point in cpu time.
		 */
		time_t cpu;
	};

	/**
	 * This structure represents the progress of the render job in the partitions that need to be rendered,
	 * the partitions that have been rendered.
	 */
	struct Progress {
		/**
		 * The amount of partitions that have finished.
		 */
		std::atomic_int finished;

		/**
		 * The amount of partitions being used.
		 */
		int total;
	};

	/**
	 * A progress {@link KernelObserver} for the GLUT frontend, which tracks the progress of the render job and
	 * the partitions that have been rendered and reports this to the standard output stream (stdout).
	 */
	class ConsoleProgressObserver : public KernelObserver {
	public:
		/**
		 * Construct a {@link ProgressObserver} instance.
		 *
		 * @param[in] length The length of the progress bar.
		 */
		ConsoleProgressObserver(size_t length) : bar(length) {}

		/**
		 * This method is invoked when a render job is started on the kernel.
		 *
		 * @param[in] kernel The kernel that is rendering the scene.
		 * @param[in] scene The scene that is being rendered.
		 * @param[in] camera The {@link Camera} that is being used.
		 * @param[in] partitions The amount of partitions being used.
		 */
		virtual void renderStarted(const traceur::Kernel &,
								   const traceur::Scene &,
								   const traceur::Camera &,
								   int) override final;

		/**
		 * This method is invoked when a render job of a partition is started on
		 * the kernel.
		 *
		 * @param[in] kernel The kernel that is rendering the scene.
		 * @param[in] id The identifier of the partition.
		 * @param[in] film The {@link Film} the kernel renders the scene on.
		 * @param[in] offset The offset within the film.
		 */
		virtual void partitionStarted(const traceur::Kernel &,
									  int,
									  const traceur::Film &,
									  const glm::ivec2 &) override final;

		/**
		 * This method is invoked when a render job of a partition is finished
		 * on the kernel.
		 *
		 * @param[in] kernel The kernel that is rendering the scene.
		 * @param[in] id The identifier of the partition.
		 * @param[in] film The {@link Film} the kernel has rendered the scene on.
		 * @param[in] offset The offset within the film.
		 * @param[in] partition The identifier of the partition
		 */
		virtual void partitionFinished(const traceur::Kernel &,
									   int,
									   const traceur::Film &,
									   const glm::ivec2 &) override final;

		/**
		 * This method is invoked when a render job is finished on the kernel.
		 *
		 * @param[in] kernel The kernel that is rendering the scene.
		 * @param[in] film The {@link Film} the kernel has rendered the scene on.
		 */
		virtual void renderFinished(const traceur::Kernel &,
									const traceur::Film &) override final;

	private:
		/**
		 * The progress of the render job.
		 */
		traceur::Progress progress;

		/**
		 * The starting {@link TimePoint} of the render job.
		 */
		traceur::TimePoint start;

		/**
		 * The starting {@link TimePoint}s of the partitions.
		 */
		std::vector<traceur::TimePoint> partition_start;

		/**
		 * The starting {@link TimePoint}s of the partitions.
		 */
		std::vector<traceur::TimePoint> partition_end;

		/**
		 * The characters of the progress bar.
		 */
		std::vector<char> bar;
	};
}

#endif /* TRACEUR_FRONTEND_GLUT_PROGRESS_H */
