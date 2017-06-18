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

#include <traceur/core/kernel/basic.hpp>
#include <traceur/core/scene/primitive/primitive.hpp>

traceur::Pixel traceur::BasicKernel::trace(const traceur::Scene &scene, const traceur::Ray &ray) const
{
	traceur::Hit hit;
	if (scene.graph->intersect(ray, hit)) {
		return hit.primitive->material->diffuse;
	}

	return traceur::Pixel();
}

std::unique_ptr<traceur::Film> traceur::BasicKernel::render(const traceur::Scene &scene, const traceur::Camera &camera) const
{
	/* TODO */
	return std::make_unique<traceur::DirectFilm>(0, 0);
}

void traceur::BasicKernel::render(const traceur::Scene &scene, const traceur::Camera &camera, traceur::Film &film, glm::vec2 &offset) const
{
	/* TODO */
}
