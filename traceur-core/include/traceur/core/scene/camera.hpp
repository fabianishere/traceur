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

#include <traceur/core/kernel/ray.hpp>

namespace traceur {
	/**
	 * This class represents a camera that captures a {@link Scene}.
	 */
	class Camera {
		/**
		 * The model matrix of the camera.
		 */
		glm::mat4x4 model;

		/**
		 * The projection matrix of the camera.
		 */
		glm::mat4x4 projection;
	public:
		/**
		 * The viewport of the camera.
		 */
		glm::ivec4 viewport;

		/**
		 * The position of the camera
		 */
		glm::vec3 position;

		Camera() {}

		/**
		 * Construct a {@link Camera} instance.
		 *
		 * @param[in] viewport The viewport of the camera.
		 */
		Camera(const glm::ivec4 &viewport) : viewport(viewport) {}

		/**
		 * Create a {@link Ray} instance for the given window coordinates
		 * (x, y, z).
		 *
		 * @param[in] win The window coordinates to create a ray for.
		 */
		traceur::Ray rayFrom(const glm::vec2 &) const;

		/**
		 * Look into the given direction from the given position.
		 *
		 * @param[in] position The position of the camera.
		 * @param[in] direction The direction of the camera.
		 * @param[in] up The up vector of the camera.
		 */
		traceur::Camera lookAt(const glm::vec3 &, const glm::vec3 &,
							   const glm::vec3 &) const;

		/**
		 * Create a camera from this camera using perspective projection.
		 *
		 * @param[in] fov The field of view of the camera.
		 * @param[in] aspect The aspect ratio of the camera.
		 * @param[in] near The z-coordinate of the near clipping plane of the
		 * camera.
		 * @param[in] far The z-coordinate of the far clipping plane of the
		 * camera.
		 */
		traceur::Camera perspective(float fov, float aspect,
									float near, float far) const;

		/**
		 * Create a camera from this camera using orthographic projection.
 		 *
 		 * @param[in] left The coordinate for the left vertical clipping plane.
 		 * @param[in] right The coordinate for the right vertical clipping
 		 * plane.
 		 * @param[in] bottom The coordinate for the bottom horizontal
 		 * clipping plane.
 		 * @param[in] top The coordinate for the top horizontal
 		 * clipping plane.
 		 * @param[in] near The z-coordinate of the near clipping plane of the
 		 * camera.
 		 * @param[in] far The z-coordinate of the far clipping plane of the
 		 * camera.
 		 */
		traceur::Camera orthographic(float left, float right,
									 float bottom, float top,
									 float near, float far) const;
	};
}

#endif /* TRACEUR_CORE_SCENE_CAMERA_H */
