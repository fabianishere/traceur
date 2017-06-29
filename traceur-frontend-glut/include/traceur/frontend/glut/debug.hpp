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
#ifndef TRACEUR_FRONTEND_GLUT_DEBUG_H
#define TRACEUR_FRONTEND_GLUT_DEBUG_H

#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <traceur/core/kernel/ray.hpp>
#include <traceur/core/kernel/hit.hpp>
#include <traceur/core/scene/scene.hpp>
#include <traceur/core/scene/primitive/primitive.hpp>

namespace traceur {
	/**
	 * A debug ray that is drawn into an OpenGL context.
	 */
	struct DebugRay {
		/**
		 * The origin of the ray.
		 */
		glm::vec3 origin;

		/**
		 * The destination of the ray.
		 */
		glm::vec3 destination;

		/**
		 * The possible primitive that is hit by the ray.
		 */
		const Primitive *primitive;

		/**
		 * The depth of the ray.
		 */
		int depth;

		/**
		 * Construct a {@link DebugRay} instance.
		 *
		 * @param[in] origin The origin of the ray.
		 * @param[in] destination The destination of the ray.
		 * @param[in] primitive The possible primitive that is hit by the ray.
		 * @param[in] depth The depth of the ray.
		 */
		DebugRay(const glm::vec3 &origin, const glm::vec3 &destination, const Primitive *primitive, int depth) :
			origin(origin), destination(destination), primitive(primitive), depth(depth) {}
	};

	/**
	 * A class that traces a ray to the given screen coordinates into the scene and draws a debug ray and its
	 * reflections/refractions in an OpenGL window.
	 */
	class DebugTracer {
	public:
		/**
		 * The scene of this debug tracer.
		 */
		std::shared_ptr<traceur::Scene> scene;

		/**
		 * The maximum depth of the debug tracer.
		 */
		int maximum_depth;

		/**
		 * Construct a {@link DebugTracer} instance.
		 *
		 * @param[in] scene The scene of this debug tracer.
		 * @param[in] depth The maximum depth of the rays.
		 */
		DebugTracer(std::shared_ptr<traceur::Scene> &scene, int depth) : scene(scene), maximum_depth(depth) {}

		/**
		 * Computes the reflection ray given the hit context.
		 *
		 * @param[in] ray The ray to use.
		 * @param[in] hit The hit structure to use.
		 * @param[in] depth The depth of the recursion.
		 */
		void reflection(const traceur::Ray &, const traceur::Hit &, int);

		/**
		 * Computes the refraction ray given the hit context.
		 *
		 * @param[in] ray The ray to use.
		 * @param[in] hit The hit structure to use.
		 * @param[in] depth The depth of the recursion.
		 */
		void refraction(const traceur::Ray &, const traceur::Hit &, int);

		/**
		 * Computes the test rays to draw given a ray.
		 *
		 * @param[in] ray The ray to use.
		 * @param[in] depth The depth of the recursion.
		 * @return <code>true</code> if there was an intersection, <code>false</code>
		 * otherwise.
		 */
		bool trace(const traceur::Ray &, int);

		/**
		 * Computes the test rays for the given position of the screen.
		 *
		 * @param[in] x The x-coordinate on the screen.
		 * @param[in] y The y-coordinate on the screen.
		 */
		void trace(int, int);

		/**
		 * Render the test rays in the current OpenGL context.
		 */
		void render() const;
	private:
		/**
		 * Create a {@link Ray} instance for the given window coordinates
		 * (x, y) using the OpenGL context for the camera settings.
		 *
		 * @param[in] x The screen x-coordinate to create a ray to.
		 * @param[in] y The screen y-coordinate to create a ray to.
		 */
		traceur::Ray rayFrom(int, int) const;

		/**
		 * The debug rays to draw.
		 */
		std::vector<traceur::DebugRay> rays;
	};
}

#endif /* TRACEUR_FRONTEND_GLUT_DEBUG_H */
