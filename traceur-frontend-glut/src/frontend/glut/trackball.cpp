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

#include <cmath>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <glm/gtc/type_ptr.hpp>

#include <traceur/frontend/glut/trackball.hpp>

traceur::GLUTTrackball::GLUTTrackball(const traceur::Camera &camera, float speed) : camera(camera), speed(speed)
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(camera.projection()));
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(camera.view()));
}

void traceur::GLUTTrackball::transform()
{
	glMultMatrixf(glm::value_ptr(camera.view()));
}

void traceur::GLUTTrackball::mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        rotate = true;
        previous = glm::ivec2(x, y);
    } else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        rotate = false;
    }

	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
        zoom = true;
		previous = glm::ivec2(x, y);
    } else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
        zoom = false;
    } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        translate = true;
		previous = glm::ivec2(x, y);
    } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
        translate = false;
    }
}

void traceur::GLUTTrackball::motion(int x, int y)
{
	float dx, dy;
	glm::mat4 view = camera.view();

    if (rotate || translate || zoom) {
        dx = x - previous.x;
        dy = previous.y - y;

        if (rotate) {
			auto translation = glm::vec3(camera.view()[3]);
			float angle = std::sqrt(dx * dx + dy*dy + dx*dx + dy*dy) * speed;
			if (std::abs(angle) > 0.f) {
				view = glm::translate(view, -translation);
				view = glm::rotate(view, glm::radians(angle), glm::normalize(glm::vec3((int) -dy, (int) dx, 0)));
				view[3] = glm::vec4(translation, 1);
			}
        } else if (translate) {
			view[3][0] += dx / 100.f * speed;
			view[3][1] += dy / 100.f * speed;
        } else if (std::abs(dx) > std::abs(dy)) {
			auto translation = glm::vec3(view[3]);
			if (std::abs(dx) > 0.f) {
				view = glm::translate(view, -translation);
				view = glm::rotate(view, glm::radians(dx), glm::vec3(0, 0, -1));
				view[3] = glm::vec4(translation, 1);
			}
        } else if (std::abs(dy) > std::abs(dx)) {
			view[3][2] -= -dy / 100.0f * speed;
        }
		previous = glm::ivec2(x, y);
		camera = traceur::Camera(camera.viewport, view, camera.projection());
        glutPostRedisplay();
    }
}
