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
#include <traceur/core/scene/primitive/box.hpp>

namespace traceur {
	/**
	 * A geometric primitive that represents a triangle shape.
	 */
	class Triangle: public Primitive {
		/**
		 * The bounding box of this triangle.
		 */
		traceur::Box box;
	public:
		/**
		 * The vector to the second vertex of the triangle from the origin of
		 * this triangle.
		 */
		glm::vec3 u;

		/**
		 * The vector tot the third vertex of the triangle from the origin of
		 * this triangle.
		 */
		glm::vec3 v;

		/**
		 * The normal of this triangle.
		 */
		glm::vec3 n;

		/**
		 * Construct a {@link Triangle} instance.
		 *
		 * @param[in] origin The first vertex of the triangle.
		 * @param[in] u The vector to the second component.
		 * @param[in] v The vector to the third component.
		 * @param[in] material The material of the triangle.
		 */
		Triangle(const glm::vec3 origin, const glm::vec3 &u, const glm::vec3 &v, const std::shared_ptr<traceur::Material> material) :
			Primitive(origin, material), u(u), v(v), box(Box(material))
		{
			box = calculate_bounding_box();
			n = calculateNormal();
		}

		inline glm::vec3 calculateNormal() {
			//glm::vec3 normal = glm::cross(u - origin, v - origin);
			//glm::vec3 normal = glm::cross(-u - (v-u), v - (v-u));
			//normal . vertexnormal
			//	normal = - normal
			//return glm::vec3(0, 0, 0);
			//return glm::normalize(normal);
			glm::vec3 normal = glm::vec3(0, 0, 0);
			normal.x = u.y*v.z - u.z*v.y;
			normal.y = u.z*v.x - u.x*v.z;
			normal.z = u.x*v.y - u.y*v.x;
			return glm::normalize(normal);
		}

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
			hit.normal = n;

			return true;
		}

		/**
		 * Accept a {@link SceneGraphVisitor} instance to visit this node in
		 * the graph of the scene.
		 *
		 * @param[in] visitor The visitor to accept.
		 */
		inline virtual void accept(traceur::SceneGraphVisitor &visitor) const final
		{
			visitor.visit(*this);
		}

		/**
		 * Return the midpoint of this node.
		 *
		 * @return The midpoint of this node.
		 */
		virtual glm::vec3 midpoint() const
		{
			return origin + u / 3.f + v / 3.f;
		}

		/**
		 * Return the bounding {@link Box} which encapsulates the whole
		 * primitive.
		 *
		 * @return The bounding {@link Box} instance.
		 */
		virtual const Box & bounding_box() const final
		{
			return box;
		}
	private:
		/**
		 * Calculate the bounding box of this primitive.
		 *
		 * @param[in] The material of the bounding box.
		 * @return The bounding {@link Box} of this primitive.
		 */
		Box calculate_bounding_box() const
		{
			float infinity = std::numeric_limits<float>::infinity();
			glm::vec3 min(infinity), max(-infinity);

			for (auto &vertex : {origin, origin + u, origin + v}) {
				min = glm::min(min, vertex);
				max = glm::max(max, vertex);
			}
			return traceur::Box(min, max, material);
		}
	};
}

#endif /* TRACEUR_CORE_SCENE_PRIMITIVE_TRIANGLE_H */

