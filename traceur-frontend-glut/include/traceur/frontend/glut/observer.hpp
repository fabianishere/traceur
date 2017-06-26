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
#ifndef TRACEUR_FRONTEND_GLUT_OBSERVER_H
#define TRACEUR_FRONTEND_GLUT_OBSERVER_H

#include <atomic>
#include <chrono>
#include <traceur/core/kernel/observer.hpp>

namespace traceur {
	/**
	 * A progress observer for the GLUT frontend.
	 */
	class ProgressObserver : public KernelObserver {
	public:
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
								   int) final;

		/**
		 * This method is invoked when a render job of a partition is started on
		 * the kernel.
		 *
		 * @param[in] kernel The kernel that is rendering the scene.
		 * @param[in] film The {@link Film} the kernel renders the scene on.
		 * @param[in] offset The offset within the film.
		 */
		virtual void partitionStarted(const traceur::Kernel &,
									  const traceur::Film &,
									  const glm::ivec2 &) final;

		/**
		 * This method is invoked when a render job of a partition is finished
		 * on the kernel.
		 *
		 * @param[in] kernel The kernel that is rendering the scene.
		 * @param[in] film The {@link Film} the kernel has rendered the scene on.
		 * @param[in] offset The offset within the film.
		 */
		virtual void partitionFinished(const traceur::Kernel &,
									   const traceur::Film &,
									   const glm::ivec2 &) final;

		/**
		 * This method is invoked when a render job is finished on the kernel.
		 *
		 * @param[in] kernel The kernel that is rendering the scene.
		 * @param[in] film The {@link Film} the kernel has rendered the scene on.
		 */
		virtual void renderFinished(const traceur::Kernel &,
									const traceur::Film &) final;

	private:
		/**
		 * The amount of partitions that have finished.
		 */
		std::atomic_int finished;

		/**
		 * The amount of partitions being used.
		 */
		int partitions;

		/**
		 * The starting point in wall time.
		 */
		std::chrono::time_point<std::chrono::high_resolution_clock> wall;

		/**
		 * The starting point in cpu time.
		 */
		time_t cpu;
	};
}

#endif /* TRACEUR_FRONTEND_GLUT_OBSERVER_H */
