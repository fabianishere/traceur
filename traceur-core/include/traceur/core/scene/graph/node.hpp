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
#ifndef TRACEUR_CORE_SCENE_GRAPH_NODE_H
#define TRACEUR_CORE_SCENE_GRAPH_NODE_H

#include <traceur/core/scene/graph/visitor.hpp>
#include <traceur/core/kernel/ray.hpp>
#include <traceur/core/kernel/hit.hpp>

namespace traceur {
	/**
	 * A node in the {@link SceneGraph} which could be a primitive or a
	 * collection of primitives.
	 */
	class Node {
	public:
		/**
		 * The position of the node in the scene.
		 */
		glm::vec3 origin;

		/**
		 * Construct a {@link Node} instance.
		 */
		Node() : origin(glm::vec3()) {}

		/**
		 * Construct a {@link Node} instance.
		 *
		 * @param[in] The position of the node.
		 */
		Node(const glm::vec3 &origin) : origin(origin) {}

		/**
		 * Deconstruct the {@link Node} instance.
		 */
		virtual ~Node() {}

		/**
		 * Determine whether the given ray intersects a shape in the geometry
		 * of this graph.
		 *
		 * @param[in] ray The ray to intersect with a shape.
		 * @param[in] hit The intersection structure to which the details will
		 * be written to.
		 * @return <code>true</code> if a shape intersects the ray, otherwise
		 * <code>false</code>.
		 */
		inline virtual bool intersect(const traceur::Ray &, traceur::Hit &) const = 0;

		/**
		 * Accept a {@link SceneGraphVisitor} instance to visit this node in
		 * the graph of the scene.
		 *
		 * @param[in] visitor The visitor to accept.
		 */
		virtual void accept(traceur::SceneGraphVisitor &) const = 0;

		/**
		 * Return the midpoint of this node.
		 *
		 * @return The midpoint of this node.
		 */
		virtual glm::vec3 midpoint() const
		{
			return origin;
		}

		/**
		 * Return the bounding {@link Box} which encapsulates the whole node.
		 *
		 * @return A bounding {@link Box} of the node.
		 */
		virtual const Box & bounding_box() const = 0;
	};
}

#endif /* TRACEUR_CORE_SCENE_GRAPH_NODE_H */
