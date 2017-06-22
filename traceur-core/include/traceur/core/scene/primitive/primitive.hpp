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
#ifndef TRACEUR_CORE_SCENE_PRIMITIVE_PRIMITIVE_H
#define TRACEUR_CORE_SCENE_PRIMITIVE_PRIMITIVE_H

#include <memory>

#include <traceur/core/scene/graph/node.hpp>
#include <traceur/core/scene/graph/visitor.hpp>
#include <traceur/core/material/material.hpp>

namespace traceur {
	/**
	 * A geometric primitive is a shape for which a ray-shape intersection
	 * method has been implemented.
	 * More complex objects in the scene graph can be built out of these
	 * primitives.
	 */
	class Primitive : public Node {
	public:
		/**
		 * The material of the primitive.
		 */
		std::shared_ptr<traceur::Material> material;

		/**
		 * Construct a {@link Primitive} instance.
		 *
		 * @param[in] origin The origin of the primitive.
		 * @param[in] material The material of the primitive.
		 */
		Primitive(const glm::vec3 &origin, const std::shared_ptr<traceur::Material> material) :
			Node(origin), material(material) {}

		/**
		 * Deconstruct the {@link Primitive} instance.
		 */
		virtual ~Primitive() {}
	};
}

#endif /* TRACEUR_CORE_SCENE_PRIMITIVE_PRIMITIVE_H */

