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
#ifndef TRACEUR_CORE_KERNEL_KERNEL_H
#define TRACEUR_CORE_KERNEL_KERNEL_H

#include <string>
#include <memory>

#include <traceur/core/kernel/film.hpp>
#include <traceur/core/kernel/observer.hpp>
#include <traceur/core/scene/scene.hpp>
#include <traceur/core/scene/camera.hpp>

namespace traceur {
	/**
	 * This class represents an interface for a raytracing kernel supported by
	 * the Traceur project.
	 */
	class Kernel {
	public:
		/**
		 * Deconstruct the {@link Kernel} instance.
		 */
		virtual ~Kernel() {}

		/**
		 * Add an observer to this {@link Kernel} instance.
		 *
		 * @param[in] observer The observer to use.
		 */
		inline void add_observer(std::shared_ptr<traceur::KernelObserver> observer)
		{
			observers.push_back(observer);
		}

		/**
		 * Render the camera view of the given {@link Scene} into a
		 * {@link Film}.
		 *
		 * @param[in] scene The {@link Scene} to render.
		 * @param[in] camera The {@link Camera} to use.
		 * @return A {@link Film} of the scene to take ownership over.
		 */
		virtual std::unique_ptr<traceur::Film> render(const traceur::Scene &,
													  const traceur::Camera &) const = 0;

		/**
		 * Render a part of the given {@link Scene} into the {@link Film}
		 * passed to this function.
		 *
		 * @param[in] scene The {@link Scene} to render.
		 * @param[in] camera The {@link Camera} to use.
		 * @param[in] film The film to render the scene into.
		 * @param[in] offset The offset of the screen.
		 */
		virtual void render(const traceur::Scene &,
							const traceur::Camera &,
							traceur::Film &,
							const glm::ivec2 &) const = 0;

		/**
		 * Return the name of this kernel.
		 *
		 * @return A string representing the name of this kernel.
		 */
		virtual const std::string & name() const = 0;

	protected:
		/**
		 * The observers of the kernel.
		 */
		std::vector<std::shared_ptr<traceur::KernelObserver>> observers;
	};
}

#endif /* TRACEUR_CORE_KERNEL_KERNEL_H */
