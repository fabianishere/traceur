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
#ifndef TRACEUR_CORE_SCENE_GRAPH_FACTORY_H
#define TRACEUR_CORE_SCENE_GRAPH_FACTORY_H

#include <memory>

#include <traceur/core/scene/graph/builder.hpp>

namespace traceur {
	/**
	 * A factory for the {@link SceneGraphBuilder} class which builds the scene
	 * graph.
	 */
	class SceneGraphBuilderFactory {
	public:
		/**
		 * Deconstruct the {@link SceneGraphBuilderFactory} instance.
		 */
		virtual ~SceneGraphBuilderFactory() {}

		/**
		 * Create a {@link SceneGraphBuilder}, which builds the scene graph.
		 *
		 * @return A unique pointer to the created {@link SceneGraphBuilder} to
		 * take ownership over.
		 */
		virtual std::unique_ptr<traceur::SceneGraphBuilder> create() const = 0;
	};

	/**
	 * Create a {@link SceneGraphBuilderFactory} instance for the given type
	 * <code>T</code>.
	 *
	 * @param[in] args The list of arguments with which an instance of
	 * <code>T</code> will be constructed.
	 */
	template<typename T, typename... Args>
	std::unique_ptr<SceneGraphBuilderFactory> make_factory(Args&&... args)
	{
		class Factory: public SceneGraphBuilderFactory {
			virtual std::unique_ptr<traceur::SceneGraphBuilder> create() const
			{
				return std::make_unique<T>(std::forward<Args>(args)...);
			}
		};

		return std::make_unique<Factory>();
	}
}

#endif /* TRACEUR_CORE_SCENE_GRAPH_FACTORY_H */
