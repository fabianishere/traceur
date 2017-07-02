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
#ifndef TRACEUR_FRONTEND_GLUT_TRACKBALL_H
#define TRACEUR_FRONTEND_GLUT_TRACKBALL_H

#include <glm/glm.hpp>
#include <traceur/core/scene/camera.hpp>

namespace traceur {
	/**
	 * A virtual object mouse trackball implementation for GLUT, so the user can change the camera of position and
	 * direction within a scene.
	 */
	class GLUTTrackball {
	public:
		/**
		 * The speed of the trackball.
		 */
		float speed;

		/**
		 * The camera of the trackball uses.
		 */
		traceur::Camera camera;

		/**
		 * Construct a {@link GLUTTrackball} with the given speed.
		 *
		 * @param[in] camera The initial camera to use.
		 * @param[in] speed The speed of the trackball.
		 */
		GLUTTrackball(const traceur::Camera &, float);

		/**
		 * Transform the scene using the trackball input received.
		 */
		void transform();

		/**
		 * Apply mouse input to the virtual trackball.
		 *
		 * @param[in] button The button that is being used.
		 * @param[in] state The state of the button.
		 * @param[in] x The x-coordinate of the mouse in the window.
		 * @param[in] y The y-coordinate of the mouse in the window.
		 */
		void mouse(int, int, int, int);

		/**
		 * Apply motion input to the virtual trackball.
		 *
		 * @param[in] x The x-coordinate of the mouse in the window.
		 * @param[in] y The y-coordinate of the mouse in the window.
		 */
		void motion(int, int);
	private:
		/**
		 * The previous mouse coordinates.
		 */
		glm::ivec2 previous;

		/**
		 * A flag to indicate we want to rotate.
		 */
		bool rotate {false};

		/**
		 * A flag to indicate we want to translate.
		 */
		bool translate {false};

		/**
		 * A flag to indicate we want to zoom.
		 */
		bool zoom {false};
	};
}

#endif /* TRACEUR_FRONTEND_GLUT_TRACKBALL_H */
