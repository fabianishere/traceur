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

#include <traceur/core/scene/camera.hpp>
#include <glm/gtc/matrix_transform.hpp>

traceur::Ray traceur::Camera::rayFrom(const glm::vec2 &win) const
{
	auto origin = glm::unProject(glm::vec3(win, 0), model, projection, viewport);
	auto destination = glm::unProject(glm::vec3(win, 1), model, projection, viewport);
	return traceur::Ray(origin, glm::normalize(destination - origin));
}

traceur::Camera traceur::Camera::lookAt(const glm::vec3 &position,
										const glm::vec3 &direction,
										const glm::vec3 &up) const
{
	traceur::Camera camera(viewport);
	camera.position = position;
	camera.model = glm::lookAt(position, position + direction, up);
	camera.projection = projection;
	return camera;
}


traceur::Camera traceur::Camera::perspective(float fov, float aspect,
											 float near, float far) const
{
	Camera camera(viewport);
	camera.position = position;
	camera.model = model;
	camera.projection = glm::perspective(fov, aspect, near, far);
	return camera;
}


traceur::Camera traceur::Camera::orthographic(float left, float right,
											  float bottom, float top,
											  float near, float far) const
{
	Camera camera(viewport);
	camera.position = position;
	camera.model = model;
	camera.projection = glm::ortho(left, right, bottom, top, near, far);
	return camera;
}