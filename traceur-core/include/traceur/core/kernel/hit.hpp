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
#ifndef TRACEUR_CORE_KERNEL_HIT_H
#define TRACEUR_CORE_KERNEL_HIT_H

#include <glm/glm.hpp>

namespace traceur {
	/* Forward declaration */
	class Primitive;

	/**
	 * A mutable data structure which represents the intersection between a 
	 * {@link Ray} and a {@link Shape}.
	 */
	class Hit {
	public:
		/**
		 * The reference to the {@link Primitive} that has been hit.
		 */
		const traceur::Primitive *primitive;

		/**
		 * The distance to the intersection.
		 */
		float distance;

		/**
		 * The amount of recursions done
		 */
		int recursion;

		/**
		 * The position of the hit within the scene.
		 */
		glm::vec3 position;

		/**
		 * The normal of the hit.
		 */
		glm::vec3 normal;

		/**
		 * Construct a {@link Hit} instance.
		 */
		Hit() {}

		/**
		 * Construct a {@link Hit} instance.
		 *
		 * @param[in] primitive The primitive that has been hit.
		 * @param[in] distance The distance to the intersection.
		 * @param[in] position The position of the hit in the scene.
		 * @param[in] normal The normal of the hit.
		 * @param[in] recursion The amount of recursions.
		 */
		Hit(const traceur::Primitive &primitive, float distance, const glm::vec3 &position, const glm::vec3 &normal, int resursion)
			: primitive(&primitive), distance(distance), position(position), normal(normal), recursion(recursion) {}
	};
}

#endif /* TRACEUR_CORE_KERNEL_HIT_H */
