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
#ifndef TRACEUR_CORE_KERNEL_BASIC_H
#define TRACEUR_CORE_KERNEL_BASIC_H

#include <traceur/core/kernel/kernel.hpp>
#include <traceur/core/kernel/pixel.hpp>

namespace traceur {
	/**
	 * A basic CPU raytracing {@link Kernel} that provides no optimizations and
	 * only implements ambient shading.
	 */
	class BasicKernel: public Kernel {
	public:
		/**
		 * Trace a single ray into the {@link Scene}.
		 *
		 * @param[in] scene The {@link Scene} to trace the ray into.
		 * @param[in] ray The {@link Ray} to trace into the scene.
		 * @return The color that has been found by the kernel.
		 */
		traceur::Pixel trace(const traceur::Scene &, const traceur::Ray &) const;
		
		/**
		 * Render the camera view of the given {@link Scene} into a
		 * {@link Film}.
		 *
		 * @param[in] scene The {@link Scene} to render.
		 * @param[in] camera The {@link Camera} to use.
		 * @return A {@link Film} of the scene to take ownership over.
		 */
		virtual std::unique_ptr<traceur::Film> render(const traceur::Scene &,
													  const traceur::Camera &) const final;

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
							const glm::ivec2 &) const final;
	};
}

#endif /* TRACEUR_CORE_KERNEL_BASIC_H */
