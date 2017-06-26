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

    const float globalOffset = 0.00001f;

	/**
	 * This struct represents the ray-tracing context of the kernel.
	 */
	struct TracingContext {
		/**
		 * The {@link Scene} that is being rendered.
		 */
		const traceur::Scene &scene;

		/**
		 * The {@link Camera} that is being used to render the scene.
		 */
		const traceur::Camera &camera;

		/**
		 * The {@link Ray} that is being traced into the scene.
		 */
		const traceur::Ray &ray;

		/**
		 * The {@link Hit} with a node.
		 */
		const traceur::Hit &hit;

		/**
		 * Construct a {@link TracingContext}
		 *
		 * @param[in] scene The scene of this context.
		 * @param[in] camera The camera of this context.
		 * @param[in] ray The ray that is being traced into the scene.
		 * @param[in] hit The hit that occured.
		 */
		TracingContext(const traceur::Scene &scene,
					   const traceur::Camera &camera,
					   const traceur::Ray &ray,
					   const traceur::Hit &hit) : scene(scene), camera(camera), ray(ray), hit(hit) {}
	};

	/**
	 * A basic CPU raytracing {@link Kernel}.
	 */
	class BasicKernel: public Kernel {
	public:
		/**
		 * Trace a single ray into the {@link Scene}.
		 *
		 * @param[in] scene The scene to trace the ray into.
		 * @param[in] camera The camera that captures the scene.
		 * @param[in] ray The ray that is traced.
		 * @param[in] depth The depth of the recursion.
		 * @return The color that has been found by the kernel.
		 */
		traceur::Pixel trace(const traceur::Scene &,
							 const traceur::Camera &,
							 const traceur::Ray &,
							 int) const;

		/**
		 * Shade a pixel with a given {@link Hit}.
		 *
		 * @param[in] context The context within we are shading.
		 * @param[in] scene The {@link Scene} to gather lights.
		 * @return The color that has been found.
		 */
		traceur::Pixel shade(const traceur::TracingContext &, int) const;

		/**
		 * Calculate the diffuse effect for the given hit, given the direction
		 * of a light.
		 *
		 * @param[in] context The context within we are shading.
		 * @param[in] lightDir The direction of the light.
		 * @return The result of the diffuse effect.
		 */
		traceur::Pixel diffuse(const traceur::TracingContext &,
							   const glm::vec3 &) const;

		/**
		 * Calculate the Blinn-Phong specularity for the given light.
		 *
		 * @param[in] context The context within we are shading.
		 * @param[in] lightDir The direction of the light.
		 * @return The result of the specular effect.
		 */
		traceur::Pixel specular(const traceur::TracingContext &,
								const glm::vec3 &) const;

        /**
         * Calculate the reflection effect for the given tracing context.
         *
         * @param[in] context The context within we are shading.
         * @param[in] depth The depth of the recursion.
         * @return The result of the reflection effect.
         */
        traceur::Pixel reflection(const traceur::TracingContext &,
                                  int depth) const;

        /**
		 * Calculate the reflection effect for the given tracing context.
		 *
		 * @param[in] context The context within we are shading.
		 * @param[in] depth The depth of the recursion.
         * @param[in] normal The normal to reflect of off.
		 * @return The result of the reflection effect.
		 */
        traceur::Pixel reflection(const traceur::TracingContext &,
                                  int depth, const glm::vec3 &) const;

        /**
         * Calculate the refraction effect for the given tracing context.
         * @param[in] context The context within we are refracting.
         * @param[in] depth The depth of the recursion.
         * @return The result of the refraction effect.
         */
		traceur::Pixel refraction(const traceur::TracingContext &,
                                  int depth) const;

        /**
         * Calculate the transparency effect for the given tracing context.
         * @param[in] context The context within we are transparent.
         * @param[in] depth The depth of the recursion.
         * @return The result of the refraction effect.
         */
        traceur::Pixel transparent(const traceur::TracingContext &,
                                  int depth) const;

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

		/**
		 * Return the name of this kernel.
		 *
		 * @return A string representing the name of this kernel.
		 */
		virtual const std::string & name() const final
		{
			static const std::string name = "basic";
			return name;
		}
	};
}

#endif /* TRACEUR_CORE_KERNEL_BASIC_H */
