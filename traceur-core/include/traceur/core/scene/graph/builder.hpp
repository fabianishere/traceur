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
#ifndef TRACEUR_CORE_SCENE_GRAPH_BUILDER_H
#define TRACEUR_CORE_SCENE_GRAPH_BUILDER_H

#include <memory>

#include <traceur/core/scene/graph/graph.hpp>
#include <traceur/core/scene/primitive/primitive.hpp>

namespace traceur {
	/**
	 * A builder for {@link SceneGraph} instances, which are immutable 
	 * representations of the scene.
	 */
	class SceneGraphBuilder {
	public:
		/**
		 * Deconstruct the {@link SceneGraphBuilder} instance.
		 */
		virtual ~SceneGraphBuilder() {}

		/**
		 * Add a node to the graph of the scene.
		 *
		 * @param[in] node The node to add to the scene.
		 */
		virtual void add(const std::shared_ptr<traceur::Primitive>) = 0;

		/**
		 * Add a node to the graph of the scene.
		 *
		 * @param[in] node The node to add to the scene.
		 */
		template<typename T>
		inline void add(const T &primitive)
		{
			add(std::shared_ptr<traceur::Primitive>(new T(primitive)));
		}
		/**
		 * Build a {@link SceneGraph} from the current geometry given to this
		 * builder.
		 *
		 * @return A unique pointer to the created {@link SceneGraph} to
		 * take ownership over.
		 */
		virtual std::unique_ptr<traceur::SceneGraph> build() const = 0;
	};
}

#endif /* TRACEUR_CORE_SCENE_GRAPH_BUILDER_H */
