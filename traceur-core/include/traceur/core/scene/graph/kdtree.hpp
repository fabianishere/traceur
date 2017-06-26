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
#ifndef TRACEUR_CORE_SCENE_GRAPH_KDTREE_H
#define TRACEUR_CORE_SCENE_GRAPH_KDTREE_H

#include <vector>
#include <limits>
#include <memory>

#include <glm/glm.hpp>

#include <traceur/core/scene/graph/graph.hpp>
#include <traceur/core/scene/graph/builder.hpp>
#include <traceur/core/scene/primitive/box.hpp>

namespace traceur {
	/* Forward Declarations */
	class KDTreeSceneGraphBuilder;

	/**
	 * A {@link Node} in the kd-tree.
	 */
	class KDTreeNode : public Node {
		/**
		 * The left sub node of this node.
		 */
		std::unique_ptr<KDTreeNode> left;

		/**
		 * The right sub node of this node.
		 */
		std::unique_ptr<KDTreeNode> right;

		/**
		 * The primitives contained in this node.
		 */
		std::vector<std::shared_ptr<traceur::Primitive>> primitives;

		/**
		 * The bounding {@link Box} of this node.
		 */
		traceur::Box box;

		/**
		 * Allow a {@link KDTreeSceneGraphBuilder} to access our privates.
		 */
		friend KDTreeSceneGraphBuilder;
	public:
		/**
		 * The depth of the node.
		 */
		int depth;

		/**
		 * Construct a {@link KDTreeNode} instance.
		 */
		KDTreeNode() : depth(0), box(traceur::Box::createBoundingBox()) {}

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
		inline virtual bool intersect(const traceur::Ray &, traceur::Hit &) const final;

		/**
		 * Accept a {@link SceneGraphVisitor} instance to visit this node in
		 * the graph of the scene.
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
	 * A {@link SceneGraph> which is represented by a kd-tree.
	 */
	class KDTreeSceneGraph: public SceneGraph {
		/**
		 * The root node of this tree.
		 */
		std::unique_ptr<traceur::KDTreeNode> root;
	public:
		/**
		 * Construct a {@link KDTreeSceneGraph} instance.
		 *
		 * @param[in] root The root node of the tree.
		 */
		KDTreeSceneGraph(std::unique_ptr<traceur::KDTreeNode> root) : root(std::move(root)) {}

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
	};

	/**
	 * A builder for {@link KDTreeSceneGraph} instances.
	 */
	class KDTreeSceneGraphBuilder: public SceneGraphBuilder {
		/**
		 * The primitives contained in this graph.
		 */
		std::vector<std::shared_ptr<traceur::Primitive>> primitives;

		/**
		 * Build a {@link KDTreeNode} recursively from the given primitives.
		 *
		 * @param[in] primitives The primitives to build the node from.
		 * @param[in] depth The depth of the node.
		 * @return The {@link KDTreeNode} to take ownership over.
		 */
		std::unique_ptr<traceur::KDTreeNode> build(const std::vector<std::shared_ptr<traceur::Primitive>> &, int) const;
	public:
		/**
		 * Construct a {@link KDTreeSceneGraphBuilder} instance.
		 */
		KDTreeSceneGraphBuilder() {}

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

#endif /* TRACEUR_CORE_SCENE_GRAPH_KDTREE_H */
