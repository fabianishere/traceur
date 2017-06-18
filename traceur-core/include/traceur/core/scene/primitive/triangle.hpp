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
#ifndef TRACEUR_CORE_SCENE_PRIMITIVE_TRIANGLE_H
#define TRACEUR_CORE_SCENE_PRIMITIVE_TRIANGLE_H

#include <traceur/core/scene/primitive/primitive.hpp>

namespace traceur {
	/**
	 * A geometric primitive that represents a triangle shape.
	 */
	class Triangle: public Primitive {
	public:
		/**
		 * The vector to the second vertex of the triangle from the origin of
		 * this triangle.
		 */
		const glm::vec3 u;

		/**
		 * The vector tot the third vertex of the triangle from the origin of
		 * this triangle.
		 */
		const glm::vec3 v;

		/**
		 * Construct a {@link Triangle} instance.
		 *
		 * @param[in] origin The origin location of the sphere.
		 * @param[in] u The vector to the second component.
		 * @param[in] v The vector to the third component.
		 * @param[in] material The material of the triangle.
		 */
		Triangle(const glm::vec3 origin, const glm::vec3 &u, const glm::vec3 &v, const std::shared_ptr<traceur::Material> material) :
			Primitive(origin, material), u(u), v(v) {}

		/**
		 * Determine whether the given ray intersects the shape.
		 *
		 * @param[in] ray The ray to intersect with this shape.
		 * @param[in] hit The intersection with this shape if it exists.
		 * @return <code>true</code> if the shape intersects the ray, otherwise
		 * <code>false</code>.
		 */
		inline virtual bool intersect(const traceur::Ray &ray, traceur::Hit &hit) const final
		{
			// Compute the plane's normal
			auto N = glm::cross(u, v);

			// Calculate angle of ray relative to plane's normal
			auto d = glm::dot(N, ray.direction);

			// No intersection if the ray is parallel to plane
			if (std::abs(d) < 0.000000f) {
				return false;
			}

			// Solve t for equation P = O + tD
			float t = glm::dot(origin - ray.origin, N) / d;

			// The triangle is behind the ray
			if (t < 0) {
				return false;
			}

			// Calculate the point of intersection
			auto p = ray.origin + t * ray.direction;

			float d00 = glm::dot(u, u);
			float d01 = glm::dot(u, v);
			float d11 = glm::dot(v, v);
			float d20 = glm::dot(p - origin, u);
			float d21 = glm::dot(p - origin, v);
			float invDenom = 1.0 / (d00 * d11 - d01 * d01);

			auto a = (d11 * d20 - d01 * d21) * invDenom;
			auto b = (d00 * d21 - d01 * d20) * invDenom;
	
			// Intersection with triangle's plane but outside triangle
			if (a < -0.000f || b < -0.000f || a + b > 1) {
				return false;
			}

			hit.primitive = this;
			hit.distance = t;
			hit.position = p;
			hit.normal = glm::normalize((ray.origin - origin) + t * ray.direction);

			return true;
		}
	};
}

#endif /* TRACEUR_CORE_SCENE_PRIMITIVE_TRIANGLE_H */

