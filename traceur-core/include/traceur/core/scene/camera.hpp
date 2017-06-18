/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Fabian Mastenbroek <f.s.mastenbroek@student.tudelft.nl>
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
#ifndef TRACEUR_CORE_SCENE_CAMERA_H
#define TRACEUR_CORE_SCENE_CAMERA_H

#include <glm/glm.hpp>

namespace traceur {
	/**
	 * This class represents a camera that captures a {@link Scene}.
	 */
	class Camera {
	public:
		/**
		 * The position of the camera within the scene.
		 */
		const glm::vec3 position;

		/**
		 * The normalised direction of the camera within the scene.
		 */
		const glm::vec3 direction;

		/**
		 * Construct a {@link Camera} instance.
		 */
		Camera() : direction(glm::vec3(1, 0, 0)) {}

		/**
		 * Construct a {@link Camera} instance.
		 *
		 * @param[in] position The position of the camera.
		 * @param[in] direction The normalised direction of the camera.
		 */
		Camera(const glm::vec3 &position, const glm::vec3 &direction)
			: position(position), direction(direction) {}
	};
}

#endif /* TRACEUR_CORE_SCENE_CAMERA_H */
