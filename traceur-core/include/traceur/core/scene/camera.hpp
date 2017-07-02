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
#include <glm/gtc/matrix_transform.hpp>

#include <traceur/core/kernel/ray.hpp>

namespace traceur {
	/**
	 * This class represents a camera that captures a {@link Scene}.
	 */
	class Camera {
	public:
		/**
		 * The viewport of the camera.
		 */
		glm::ivec4 viewport;

		/**
		 * Construct a {@link Camera} instance.
		 *
		 * @param[in] viewport The viewport of the camera.
		 */
		explicit Camera(const glm::ivec4 &viewport) noexcept : viewport(viewport) {}

		/**
		 * Construct a {@link Camera} instance from the given matrices.
		 *
		 * @param[in] viewport The viewport of the camera.
		 * @param[in] view The view matrix to use.
		 * @param[in] projection The projection matrix to use.
		 */
		Camera(const glm::ivec4 &viewport, const glm::mat4 &view, const glm::mat4 &projection) noexcept :
			viewport(viewport), m_view(view), m_projection(projection) {}

		/**
		 * Create a {@link Ray} instance for the given window coordinates
		 * (x, y).
		 *
		 * @param[in] win The window coordinates to create a ray for.
		 */
		traceur::Ray rayFrom(const glm::vec2 &win) const noexcept
		{
			auto origin = glm::unProject(glm::vec3(win, 0), m_view, m_projection, viewport);
			auto destination = glm::unProject(glm::vec3(win, 1), m_view, m_projection, viewport);
			return traceur::Ray(origin, glm::normalize(destination - origin));
		}

		/**
		 * Look into the given direction from the given position.
		 *
		 * @param[in] position The position of the camera.
		 * @param[in] direction The direction of the camera.
		 * @param[in] up The up vector of the camera.
		 * @return A camera looking in the given direction.
		 */
		traceur::Camera lookAt(const glm::vec3 &position, const glm::vec3 &direction,
							   const glm::vec3 &up) const noexcept
		{
			return traceur::Camera(
				viewport,
				glm::lookAt(position, position + direction, up),
				m_projection
			);
		}

		/**
		 * Create a camera from this camera using perspective projection.
		 *
		 * @param[in] fov The field of view of the camera.
		 * @param[in] aspect The aspect ratio of the camera.
		 * @param[in] near The z-coordinate of the near clipping plane of the
		 * camera.
		 * @param[in] far The z-coordinate of the far clipping plane of the
		 * camera.
		 * @return A camera using perspective projection.
		 */
		traceur::Camera perspective(float fov, float aspect,
									float near, float far) const noexcept
		{
			return traceur::Camera(
				viewport,
				m_view,
				glm::perspective(fov, aspect, near, far)
			);
		}

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
 		 * @return A camera using orthographic projection.
 		 */
		traceur::Camera orthographic(float left, float right,
									 float bottom, float top,
									 float near, float far) const noexcept
		{
			return traceur::Camera(
				viewport,
				m_view,
				glm::ortho(left, right, bottom, top, near, far)
			);
		}

		/**
		 * Translate the camera in the given direction.
		 *
		 * @param[in] direction The direction to translate the camera in.
		 * @return The translated camera.
		 */
		traceur::Camera translate(const glm::vec3 &direction) const noexcept
		{
			return traceur::Camera(
				viewport,
				glm::translate(m_view, direction),
				m_projection
			);
		}

		/**
		 * Rotate the camera with an angle of the given degrees on the given axis.
		 *
		 * @param[in] angle The angle in radians to rotate.
		 * @param[in] axis An axis vector representing the axis around which we rotate.
		 */
		traceur::Camera rotate(float angle, const glm::vec3 &axis) const noexcept
		{
			return traceur::Camera(
				viewport,
				glm::rotate(m_view, angle, axis),
				m_projection
			);
		}

		/**
		 * Return the view matrix of the camera as reference.
		 *
		 * @return The view matrix of the camera as reference.
		 */
		const glm::mat4 & view() const noexcept
		{
			return m_view;
		}

		/**
		 * Return the projection matrix of the camera as reference.
		 *
		 * @return The projection matrix of the camera as reference.
		 */
		const glm::mat4 & projection() const noexcept
		{
			return m_projection;
		}

		/**
		 * Return the position of the camera within the scene (in world space).
		 *
		 * @return The position of the camera within the scene.
		 */
		glm::vec3 position() const noexcept
		{
			return glm::inverse(m_view)[3];
		}
	private:
		/**
		 * The view matrix of the camera.
		 */
		glm::mat4 m_view;

		/**
		 * The projection matrix of the camera.
		 */
		glm::mat4 m_projection;
	};
}

#endif /* TRACEUR_CORE_SCENE_CAMERA_H */
