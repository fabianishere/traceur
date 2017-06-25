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
#ifndef TRACEUR_CORE_SCENE_GRAPH_VECTOR_H
#define TRACEUR_CORE_SCENE_GRAPH_VECTOR_H

#include <vector>
#include <limits>
#include <memory>

#include <glm/glm.hpp>

#include <traceur/core/scene/graph/graph.hpp>
#include <traceur/core/scene/graph/builder.hpp>
#include <traceur/core/scene/primitive/box.hpp>

namespace traceur {
	/**
	 * A {@link SceneGraph> which creates a linear representation of the scene
	 * in memory.
	 */
	class VectorSceneGraph: public SceneGraph, public Node {
		/**
		 * The nodes in the graph.
		 */
		const std::unique_ptr<std::vector<std::shared_ptr<traceur::Primitive>>> nodes;

		/**
		 * The bounding box of this graph.
		 */
		const traceur::Box box;
	public:
		/**
		 * Construct a {@link VectorSceneGraph} instance.
		 *
		 * @param[in] nodes The nodes in the graph.
		 * @param[in] box The bounding box of this graph.
		 */
		VectorSceneGraph(const std::vector<std::shared_ptr<traceur::Primitive>> &nodes,
						 const traceur::Box &box
		) : box(box),
			nodes(std::make_unique<std::vector<std::shared_ptr<traceur::Primitive>>>(nodes)) {}

		/**
		 * Determine whether the given ray intersects a node in the geometry
		 * of this container.
		 *
		 * @param[in] ray The ray to intersect with a shape.
		 * @param[in] hit The intersection structure to which the details will
		 * be written to.
		 * @return <code>true</code> if a shape intersects the ray, otherwise
		 * <code>false</code>.
		 */
		virtual bool intersect(const traceur::Ray &, traceur::Hit &) const final;

		/**
		 * Accept a {@link SceneGraphVisitor} instance to traverse this scene
		 * graph.
		 *
		 * @param[in] visitor The visitor to accept.
		 */
		virtual void accept(traceur::SceneGraphVisitor &) const final;

		/**
		 * Return the bounding {@link Box} which encapsulates the whole node.
		 *
		 * @return A bounding {@link Box} of the node.
		 */
		virtual const Box & bounding_box() const final
		{
			return box;
		}
	};

	/**
	 * A builder for {@link VectorSceneGraph} instances.
	 */
	class VectorSceneGraphBuilder: public SceneGraphBuilder {
		/**
		 * The current nodes in the partial graph.
		 */
		std::vector<std::shared_ptr<traceur::Primitive>> nodes;
		/**
		 * The minimum vertex in the scene.
		 */
		glm::vec3 min;
		/**
		 * The maximum vertex in the scene.
		 */
		glm::vec3 max;
	public:
		/**
		 * Construct a {@link VectorSceneGraphBuilder} instance.
		 */
		VectorSceneGraphBuilder()
		{
			min = glm::vec3(std::numeric_limits<float>::infinity());
			max = -min;
		}
		/**
		 * Add a node to the graph of the scene.
		 *
		 * @param[in] node The node to add to the scene.
		 */
		virtual void add(const std::shared_ptr<traceur::Primitive>) final;

		/**
		 * Build a {@link SceneGraph} from the current geometry given to this
		 * builder.
		 *
		 * @return A unique pointer to the created {@link SceneGraph} to
		 * take ownership over.
		 */
		virtual std::unique_ptr<traceur::SceneGraph> build() const;
	};
}

#endif /* TRACEUR_CORE_SCENE_GRAPH_VECTOR_H */
