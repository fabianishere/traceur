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

#include <traceur/core/scene/graph/kdtree.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>


bool traceur::KDTreeNode::intersect(const traceur::Ray &ray, traceur::Hit &hit) const
{
	/* Test if ray intersects the bounding box of the scene graph */
	if (!box.intersect(ray, hit)) {
		return false;
	}


	traceur::Hit nearest;
	double dist = std::numeric_limits<double>::infinity();
	bool intersection = false;

	/* If the node has left or right nodes, test for intersection on these */
	for (auto node : {&*left, &*right}) {
		/* Test if the ray intersects the node */
		if (node && node->intersect(ray, hit) && hit.distance < dist) {
			nearest = hit;
			dist = hit.distance;
			intersection = true;
		}
	}

	if (intersection) {
		hit = nearest;
		return true;
	}

	for (auto &primitive : primitives) {
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

	if (intersection) {
		hit = nearest;
		return true;
	}
	return false;
}

void traceur::KDTreeNode::accept(traceur::SceneGraphVisitor &visitor) const
{
	visitor.visit(*this);

	for (auto &primitive : primitives) {
		primitive->accept(visitor);
	}

	if (left) {
		left->accept(visitor);
	}

	if (right) {
		right->accept(visitor);
	}
}

bool traceur::KDTreeSceneGraph::intersect(const traceur::Ray &ray, traceur::Hit &hit) const
{
	return root->intersect(ray, hit);
}

void traceur::KDTreeSceneGraph::accept(traceur::SceneGraphVisitor &visitor) const
{
	return root->accept(visitor);
}

void traceur::KDTreeSceneGraphBuilder::add(const std::shared_ptr<traceur::Primitive> primitive)
{
	primitives.push_back(primitive);
}


std::unique_ptr<traceur::KDTreeNode> traceur::KDTreeSceneGraphBuilder::build(
	const std::vector<std::shared_ptr<traceur::Primitive>> &primitives,
	int depth) const
{
	std::unique_ptr<traceur::KDTreeNode> node = std::make_unique<traceur::KDTreeNode>();
	node->primitives = primitives;
	node->depth = depth;

	/* Optimize for small trees */
	if (primitives.size() == 0) {
		node->box = traceur::Box(std::make_shared<traceur::Material>());
		return std::move(node);
	} else if (primitives.size() == 1) {
		auto &primitive = primitives[0];
		node->origin = primitive->origin;
		node->box = primitive->bounding_box();
		return std::move(node);
	}

	/* Calculate the bounding box and origin for this node */
	for (auto &primitive : primitives) {
		node->box = node->box.expand(primitive->bounding_box());
		node->origin = node->origin + (primitive->midpoint() / (float) primitives.size());
	}

	int axis = static_cast<int>(node->bounding_box().longestAxis());
	std::vector<std::shared_ptr<traceur::Primitive>> left;
	std::vector<std::shared_ptr<traceur::Primitive>> right;

	/* Divide primitives */
	for (auto &primitive : primitives) {
		if (node->origin[axis] < primitive->midpoint()[axis]) {
			left.push_back(primitive);
		} else {
			right.push_back(primitive);
		}
	}

	if (left.size() == 0 && right.size() > 0) {
		left = right;
	}
	if (right.size() == 0 && left.size() > 0) {
		right = left;
	}

	int matches = 0;
	for (auto &leftPrimitive : left) {
		for (auto &rightPrimitive : right) {
			if (leftPrimitive == rightPrimitive)
				matches++;
		}
	}

	if ((float)matches / left.size() < 0.5 && (float)matches / right.size() < 0.5) {
		node->left = std::move(build(left, depth + 1));
		node->right = std::move(build(right, depth + 1));
	}
	return std::move(node);
}

std::unique_ptr<traceur::SceneGraph> traceur::KDTreeSceneGraphBuilder::build() const
{
	return std::make_unique<traceur::KDTreeSceneGraph>(build(primitives, 0));
}
