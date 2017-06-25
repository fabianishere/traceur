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

#include <traceur/core/scene/graph/vector.hpp>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

inline bool traceur::VectorSceneGraph::intersect(const traceur::Ray &ray, traceur::Hit &hit) const
{
	/* Test if ray intersects the bounding box of the scene graph */
	if (!box.intersect(ray, hit)) {
		return false;
	}

	traceur::Hit nearest;
	double dist = std::numeric_limits<double>::infinity();
	bool intersection = false;

	for (auto &primitive : nodes) {
		/* Test if ray intersects bounding box of primitive */
		if (!primitive->bounding_box().intersect(ray, hit)) {
			continue;
		}

		/* Test if ray intersects primitive */
		if (primitive->intersect(ray, hit) && hit.distance < dist) {
			nearest = hit;
			dist = hit.distance;
			intersection = true;
		}
	}

	hit = nearest;
	return intersection;
}

void traceur::VectorSceneGraph::accept(traceur::SceneGraphVisitor &visitor) const
{
	/* Visit the root node */
	visitor.visit(*this);

	for (auto &primitive : nodes) {
		primitive->accept(visitor);
	}
}

void traceur::VectorSceneGraphBuilder::add(const std::shared_ptr<traceur::Primitive> primitive)
{
	nodes.push_back(primitive);
	box = box.expand(primitive->bounding_box());
}

std::unique_ptr<traceur::SceneGraph> traceur::VectorSceneGraphBuilder::build() const
{
	return std::make_unique<traceur::VectorSceneGraph>(
			nodes,
			box
	);
}


