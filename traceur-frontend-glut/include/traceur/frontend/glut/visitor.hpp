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
#ifndef TRACEUR_FRONTEND_GLUT_VISITOR_H
#define TRACEUR_FRONTEND_GLUT_VISITOR_H

#include <traceur/core/scene/graph/visitor.hpp>
#include <traceur/core/scene/primitive/sphere.hpp>
#include <traceur/core/scene/primitive/triangle.hpp>

namespace traceur {
	/**
	 * A visitor of the {@link SceneGraph} that traverses the graph and draws
	 * all elements of the scene into an OpenGL window.
	 */
	class OpenGLSceneGraphVisitor: public SceneGraphVisitor {
	public:
		/**
		 * Construct a {@link OpenGLSceneGraphVisitor} instance.
		 */
		OpenGLSceneGraphVisitor() {}
	
		/**
		 * Visit a {@link Sphere} primitive in the scene graph.
		 *
		 * @param[in] node The node to visit.
		 */
		virtual void visit(const traceur::Sphere &) final;

		/**
		 * Visit a {@link Triangle} primitive in the scene graph.
		 *
		 * @param[in] node The node to visit.
		 */
		virtual void visit(const traceur::Triangle &) final;
	};
}

#endif /* TRACEUR_FRONTEND_GLUT_VISITOR_H */
