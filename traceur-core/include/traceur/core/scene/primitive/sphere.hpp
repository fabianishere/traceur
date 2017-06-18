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
#ifndef TRACEUR_CORE_SCENE_PRIMITIVE_SPHERE_H
#define TRACEUR_CORE_SCENE_PRIMITIVE_SPHERE_H

#include <traceur/core/scene/primitive/primitive.hpp>

namespace traceur {
	/**
	 * A spherical primitive which has a center and a radius.
	 */
	class Sphere: public Primitive {
	public:
		/**
		 * The radius of the sphere.
		 */
		const double radius;

		/**
		 * Construct a {@link Sphere} instance.
		 *
		 * @param[in] center The center position of the sphere.
		 * @param[in] radius The radius of the sphere.
		 * @param[in] material The material of the sphere.
		 */
		Sphere(const glm::vec3 &center, double radius, const std::shared_ptr<traceur::Material> &material) :
			Primitive(center, material), radius(radius) {}

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
			auto v = origin - ray.origin;
			double b = glm::dot(v, ray.direction);
			double D = b*b - glm::dot(v, v) + radius * radius;

			if (D < 0)
				return false;

			double d = sqrt(D);
			double t2 = b + d;

			if (t2 < 0)
				return false;

			double t1 = b - d;
			float lambda = t1 > 0 ? t1 : t2;
			
			hit.primitive = this;
			hit.distance = lambda;
			hit.position = ray.origin + lambda * ray.direction;
			hit.normal = glm::normalize((ray.origin - origin) + lambda * ray.direction);
			
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
	};
}

#endif /* TRACEUR_CORE_SCENE_PRIMITIVE_SPHERE_H */

