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
#ifndef TRACEUR_CORE_KERNEL_OBSERVER_H
#define TRACEUR_CORE_KERNEL_OBSERVER_H

#include <glm/glm.hpp>

namespace traceur {
	/* Forward Declarations */
	class Kernel;
	class Scene;
	class Camera;
	class Film;

	/**
	 * A {@link KernelObserver} observes a {@link Kernel} instance and is
	 * notified of events happening in the kernel.
	 *
	 * A kernel does not provide any synchronisation guarantees for an observer
	 * and therefore this class is not thread-safe.
	 */
	class KernelObserver {
	public:
		/**
		 * Deconstruct a {@link KernelObserver} instance.
		 */
		virtual ~KernelObserver() {}

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
								   int) {}

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
									  const glm::ivec2 &) {}

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
									   const glm::ivec2 &) {}

		/**
		 * This method is invoked when a render job is finished on the kernel.
		 *
		 * @param[in] kernel The kernel that is rendering the scene.
		 * @param[in] film The {@link Film} the kernel has rendered the scene on.
		 */
		virtual void renderFinished(const traceur::Kernel &,
									const traceur::Film &) {}
	};
}

#endif /* TRACEUR_CORE_KERNEL_OBSERVER_H */
