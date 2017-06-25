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
#ifndef TRACEUR_CORE_SCENE_PRIMITIVE_BOX_H
#define TRACEUR_CORE_SCENE_PRIMITIVE_BOX_H

#include <limits>
#include <traceur/core/scene/primitive/primitive.hpp>

namespace traceur {
	/**
	 * A primitive that represents a box.
	 */
	class Box : public Primitive {
	public:
		/**
		 * The minimum vertex in the box.
		 */
		glm::vec3 min;

		/**
		 * The maximum vertex in the box.
		 */
		glm::vec3 max;

		/**
		 * Construct a {@link Box} instance.
		 *
		 * @param[in] material The material of the primitive.
		 */
		Box(const std::shared_ptr<traceur::Material> material) :
				Primitive(glm::vec3(), material), min(glm::vec3()), max(glm::vec3()) {}

		/**
		 * Construct a {@link Box} instance.
		 *
		 * @param[in] min The minimum vertex in the box.
		 * @param[in] max The maximum vertex in the box.
		 * @param[in] material The material of the primitive.
		 */
		Box(const glm::vec3 &min, const glm::vec3 max,
			const std::shared_ptr<traceur::Material> material) :
				Primitive((min + max) / 2.f, material), min(min), max(max) {}

		/**
		 * Construct a {@link Box} as bounding box.
		 *
		 * @return The bounding box instance.
		 */
		static Box createBoundingBox()
		{
			auto min = glm::vec3(std::numeric_limits<float>::infinity());
			auto max = -min;
			return createBoundingBox(min, max);
		}

		/**
		 * Construct a {@link Box} as bounding box.
		 *
		 * @param[in] min The minimum vertex in the box.
		 * @param[in] max The maximum vertex in the box.
		 * @return The bounding box instance.
		 */
		static Box createBoundingBox(const glm::vec3 &min, const glm::vec3 &max)
		{
			return Box(min, max, std::make_shared<traceur::Material>());
		}

		/**
		 * Determine whether the given ray intersects the shape.
		 *
		 * @param[in] ray The ray to intersect with this shape.
		 * @param[in] hit The intersection structure to which the details will
		 * be written to.
		 * @return <code>true</code> if the shape intersects the ray, otherwise
		 * <code>false</code>.
		 */
		inline virtual bool intersect(const traceur::Ray &ray, traceur::Hit &hit) const final
		{
			/* TODO precalculate inverse */
			glm::vec3 inverse = 1.0f / ray.direction;
			auto u = (min - ray.origin) * inverse;
			auto v = (max - ray.origin) * inverse;

			float tmin = std::fmax(std::fmax(std::fmin(u[0], v[0]), std::fmin(u[1], v[1])), std::fmin(u[2], v[2]));
			float tmax = std::fmin(std::fmin(std::fmax(u[0], v[0]), std::fmax(u[1], v[1])), std::fmax(u[2], v[2]));

			if (tmax < 0)
				return false;
			if (tmin > tmax)
				return false;

			hit.primitive = this;
			hit.distance = tmin;
			hit.position = ray.origin + tmin * ray.direction;
			return true;
		}

		/**
		 * Expand this {@link Box} with another box.
		 *
		 * @param[in] other The other box to expand with.
		 * @return The next expanded box with the material properties of this
		 * instance.
		 */
		traceur::Box expand(const traceur::Box &other) const
		{
			return traceur::Box(glm::min(min, other.min), glm::max(max, other.max), material);
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
		 * Return the bounding {@link Box} which encapsulates the whole
		 * primitive.
		 *
		 * @return The bounding {@link Box} instance.
		 */
		virtual const traceur::Box & bounding_box() const final
		{
			return *this;
		}
	};
}

#endif /* TRACEUR_CORE_SCENE_PRIMITIVE_PRIMITIVE_H */

