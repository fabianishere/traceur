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
#ifndef TRACEUR_CORE_SCENE_GRAPH_VISITOR_H
#define TRACEUR_CORE_SCENE_GRAPH_VISITOR_H

namespace traceur {
	/* Forward declarations */
	class Node;
	class Primitive;
	class Sphere;
	class Triangle;
	class Box;

	/**
	 * A visitor of the {@link SceneGraph} that traverses the graph.
	 */
	class SceneGraphVisitor {
	public:
		/**
		 * Construct a {@link SceneGraphVisitor} instance.
		 */
		SceneGraphVisitor() {}

		/**
		 * Deconstruct the {@link SceneGraphVisitor} instance.
		 */
		virtual ~SceneGraphVisitor() {}

		/**
		 * Visit a {@link Node} in the scene graph.
		 *
		 * @param[in] node The node to visit.
		 */
		virtual void visit(const traceur::Node &) {}

		/**
		 * Visit a {@link Primitive} node in the scene graph.
		 *
		 * @param[in] node The node to visit.
		 */
		virtual void visit(const traceur::Primitive &) {}

		/**
		 * Visit a {@link Sphere} primitive in the scene graph.
		 *
		 * @param[in] node The node to visit.
		 */
		virtual void visit(const traceur::Sphere &) {}

		/**
		 * Visit a {@link Triangle} primitive in the scene graph.
		 *
		 * @param[in] node The node to visit.
		 */
		virtual void visit(const traceur::Triangle &) {}

		/**
		 * Visit a {@link Box} primitive in the scene graph.
		 *
		 * @param[in] node The node to visit.
		 */
		virtual void visit(const traceur::Box &) {}
	};
}

#endif /* TRACEUR_CORE_SCENE_GRAPH_VISITOR_H */
