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
#ifndef TRACEUR_CORE_SCENE_SCENE_H
#define TRACEUR_CORE_SCENE_SCENE_H

#include <vector>
#include <memory>

#include <traceur/core/scene/graph/graph.hpp>
#include <traceur/core/lightning/light.hpp>
#include <traceur/core/scene/camera.hpp>

namespace traceur {
	/**
	 * This class represents a scene which consists of primitives and lights, 
	 * which can be rendered by a {@link Kernel} instance.
	 */
	class Scene {
	public:
		/**
		 * The graph of the scene which contains the scene's geometry.
		 */
		std::shared_ptr<traceur::SceneGraph> graph;

		/**
		 * The lights in the scene.
		 */
		std::vector<traceur::Light> lights;

		/**
		 * The camera of the scene.
		 */
		traceur::Camera camera;
		
		/**
		 * Construct a {@link Scene} instance.
		 */
		Scene() {}

		/**
		 * Construct a {@link Scene} instance.
		 *
		 * @param[in] graph The graph of the scene.
		 */
		Scene(std::shared_ptr<traceur::SceneGraph> graph) : 
			graph(graph) {}

		/**
		 * Construct a {@link Scene} instance.
		 *
		 * @param[in] graph The graph of the scene.
		 * @param[in] lights The lights in the scene.
		 */
		Scene(std::shared_ptr<traceur::SceneGraph> graph, std::vector<traceur::Light> &lights) :
			graph(graph), lights(lights) {}
	};
}

#endif /* TRACEUR_CORE_SCENE_SCENE_H */
