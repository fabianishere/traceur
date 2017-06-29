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
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <traceur/frontend/glut/debug.hpp>

void traceur::DebugTracer::reflection(const traceur::Ray &ray, const traceur::Hit &hit, int depth)
{
	auto reflect = glm::reflect(ray.direction, hit.normal);
	bool intersects = trace(traceur::Ray(
		hit.position + reflect * 0.000001f,
		reflect
	), depth + 1);

	// Also show reflection rays that do not intersect a primitive
	if (!intersects) {
		rays.emplace_back(hit.position, hit.position + reflect, hit.primitive, depth + 1);
	}
}

void traceur::DebugTracer::refraction(const traceur::Ray &ray, const traceur::Hit &hit, int depth)
{
	float eta;
	glm::vec3 normal;

	if(glm::dot(hit.normal, ray.direction) < 0.f) {
		// enter material
		eta = 1.f / hit.primitive->material->opticalDensity;
		normal = hit.normal;
	} else {
		// exit material
		eta = hit.primitive->material->opticalDensity / 1.f;
		normal = -hit.normal;
	}

	glm::vec3 refract = glm::refract(ray.direction, normal, eta);

	if (!glm::isnan(refract).length()) {
		traceur::Hit next(hit);
		// Full internal ray reflection
		next.normal = normal;
		return reflection(ray, next, depth + 1);
	}
	rays.emplace_back(hit.position, hit.position +  0.000001f * refract, hit.primitive, depth);
}

bool traceur::DebugTracer::trace(const traceur::Ray &ray, int depth)
{
	if (depth > maximum_depth) {
		return false;
	}

	traceur::Hit hit;
	if (scene->graph->intersect(ray, hit)) {
		rays.emplace_back(ray.origin, hit.position, hit.primitive, depth);
		reflection(ray, hit, depth);
		refraction(ray, hit, depth);
		return true;
	}

	return false;
}

void traceur::DebugTracer::trace(int x, int y)
{
	// Clear the previous test rays
	rays.clear();

	// Trace ray into scene
	trace(rayFrom(x, y), 0);
}

void traceur::DebugTracer::render() const
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	for (auto &ray : rays) {
		if (ray.primitive) {
			glColor3fv(glm::value_ptr(ray.primitive->material->diffuse));
		} else {
			glColor3f(0, 1, ray.depth / 25);
		}
		glVertex3fv(glm::value_ptr(ray.origin));
		glVertex3fv(glm::value_ptr(ray.destination));
	}
	glEnd();
	glPopAttrib();
}

traceur::Ray traceur::DebugTracer::rayFrom(int x, int y) const
{
	glm::ivec4 viewport;
	glm::mat4x4 model;
	glm::mat4x4 projection;
	glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(model));
	glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(projection));
	glGetIntegerv(GL_VIEWPORT, glm::value_ptr(viewport));
	int y_new = viewport[3] - y;

	auto origin = glm::unProject(glm::vec3(x, y_new, 0), model, projection, viewport);
	auto destination = glm::unProject(glm::vec3(x, y_new, 1), model, projection, viewport);

	return traceur::Ray(origin, glm::normalize(destination - origin));
}
