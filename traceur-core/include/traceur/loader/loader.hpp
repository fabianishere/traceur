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
#ifndef TRACEUR_LOADER_LOADER_H
#define TRACEUR_LOADER_LOADER_H

#include <memory>
#include <string>

#include <traceur/core/scene/scene.hpp>
#include <traceur/core/scene/graph/factory.hpp>

namespace traceur {
	/**
	 * Implementations of this class are able to load a 3D scene in a specific
	 * file format into a memory representation of the scene as {@link Scene}
	 * instance.
	 */
	class Loader {
	protected:
		/**
		 * The {@link SceneGraphBuilderFactory} instance which creates the 
		 * builder for the scene graph.
		 */
		const std::shared_ptr<traceur::SceneGraphBuilderFactory> factory;
	public:
		/**
		 * Deconstruct the {@link Loader} instance.
		 */
		virtual ~Loader() {}

		/**
		 * Construct a {@link Loader} instance.
		 *
		 * @param[in] factory The {@link SceneGraphBuilderFactory} to use to 
		 * create the scene graph builder.
		 */
		Loader(const std::shared_ptr<traceur::SceneGraphBuilderFactory> factory) : factory(factory) {}

		/**
		 * Load a scene from a file into memory.
		 *
		 * @param[in] path The path to the scene to load.
		 * @return A unique pointer to the created {@link Scene} to
		 * take ownership over.
		 */
		virtual std::unique_ptr<traceur::Scene> load(const std::string &) const = 0;
	};
}

#endif /* TRACEUR_LOADER_LOADER_H */
