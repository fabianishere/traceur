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

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <traceur/frontend/glut/visitor.hpp>


void traceur::OpenGLSceneGraphVisitor::visit(const traceur::Node &node) {
	if (draw_bounding_box) {
		visit_bounding_box(node.bounding_box());
	}
}

void traceur::OpenGLSceneGraphVisitor::visit(const traceur::Sphere &sphere)
{
	auto o = sphere.origin;
	glPushMatrix();
	glTranslatef(o[0], o[1], o[2]);
	glColor3fv(glm::value_ptr(sphere.material->diffuse));
	glutSolidSphere(sphere.radius, 50, 50);
	glPopMatrix();

	if (draw_bounding_box) {
		visit_bounding_box(sphere.bounding_box());
	}
}

void traceur::OpenGLSceneGraphVisitor::visit(const traceur::Triangle &triangle)
{
	glBegin(GL_TRIANGLES);
		glColor3fv(glm::value_ptr(triangle.material->diffuse));

		auto n = glm::normalize(glm::cross(triangle.u, triangle.v));
		glNormal3fv(glm::value_ptr(n));
		glVertex3fv(glm::value_ptr(triangle.origin));
		glVertex3fv(glm::value_ptr(triangle.origin + triangle.u));
		glVertex3fv(glm::value_ptr(triangle.origin + triangle.v));
	glEnd();

	if (draw_bounding_box) {
		visit_bounding_box(triangle.bounding_box());
	}
}

void draw_box(const traceur::Box &box, const glm::vec3 &color) {
	auto min = box.bounding_box().min;
	auto max = box.bounding_box().max;

	glBegin(GL_QUADS);
		glColor3fv(glm::value_ptr(color));
		// front
		glVertex3f(max[0], max[1], max[2]);
		glVertex3f(min[0], max[1], max[2]);
		glVertex3f(min[0], min[1], max[2]);
		glVertex3f(max[0], min[1], max[2]);
		// back
		glVertex3f(max[0], max[1], min[2]);
		glVertex3f(max[0], min[1], min[2]);
		glVertex3f(min[0], min[1], min[2]);
		glVertex3f(min[0], max[1], min[2]);
		// right
		glVertex3f(max[0], max[1], max[2]);
		glVertex3f(max[0], min[1], max[2]);
		glVertex3f(max[0], min[1], min[2]);
		glVertex3f(max[0], max[1], min[2]);
		// left
		glVertex3f(min[0], max[1], max[2]);
		glVertex3f(min[0], max[1], min[2]);
		glVertex3f(min[0], min[1], min[2]);
		glVertex3f(min[0], min[1], max[2]);
		// top
		glVertex3f(max[0], max[1], max[2]);
		glVertex3f(max[0], max[1], min[2]);
		glVertex3f(min[0], max[1], min[2]);
		glVertex3f(min[0], max[1], max[2]);
		// bottom
		glVertex3f(max[0], min[1], max[2]);
		glVertex3f(min[0], min[1], max[2]);
		glVertex3f(min[0], min[1], min[2]);
		glVertex3f(max[0], min[1], min[2]);
	glEnd();
}


void traceur::OpenGLSceneGraphVisitor::visit(const traceur::Box &box)
{
	draw_box(box, box.material->diffuse);

	if (draw_bounding_box) {
		visit_bounding_box(box.bounding_box());
	}
}

void traceur::OpenGLSceneGraphVisitor::visit_bounding_box(const traceur::Box &box)
{
	/* Draw wireframe */
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	draw_box(box, glm::vec3(0, 1, 0));
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}
