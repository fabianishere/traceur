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
#ifndef TRACEUR_FRONTEND_GLUT_PREVIEW_H
#define TRACEUR_FRONTEND_GLUT_PREVIEW_H

#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <glm/glm.hpp>
#include <traceur/core/kernel/observer.hpp>

namespace traceur {
	/**
	 * A partition in a render job.
	 */
	struct Partition {
		/**
		 * An unowned pointer to the film on which the pixels are drawn.
		 */
		const traceur::DirectFilm *film;

		/**
		 * The size of the partition on the screen.
		 */
		glm::ivec2 size;

		/**
		 * The offset of the partition on the screen.
		 */
		glm::ivec2 offset;

		/**
		 * A flag to indicate the partition has finished rendering.
		 */
		bool finished;

		/**
		 * The texture identifier of the partition.
		 */
		GLuint texture;
	};

	/**
	 * A {@link KernelObserver} for the GLUT frontend, which tracks the progress of the render job and
	 * the partitions that have been rendered and draws a real-time preview of the result in an OpenGL window.
	 */
	class GLUTPreviewObserver : public KernelObserver {
	public:
		/**
		 * A flag to enable the drawing of a real-time preview.
		 */
		bool enabled;

		/**
		 * This method renders the preview into the OpenGL window.
		 */
		void render();

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
		 * A map that maps the partition identifier to a partition structure.
		 */
		std::map<int, traceur::Partition> partitions;

		/**
		 * A boolean to indicate the preview was reset.
		 */
		std::atomic_bool reset;

		/**
		 * A conditional variable to notify a thread the reset has succeeded.
		 */
		std::condition_variable condition;

		/**
		 * A mutex to lock the map.
		 */
		std::mutex mutex;
	};
}

#endif /* TRACEUR_FRONTEND_GLUT_PREVIEW_H */
