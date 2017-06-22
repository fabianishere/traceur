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
#include <algorithm>
#include <iostream>
#include <stdio.h>

traceur::Pixel traceur::BasicKernel::trace(const traceur::Scene &scene,
										   const traceur::Ray &ray,
											int i) const
{
	traceur::Hit hit;
	if (scene.graph->intersect(ray, hit)) {
		return shade(hit, scene, ray);
	}
	return traceur::Pixel();
}

traceur::Pixel traceur::BasicKernel::shade(const traceur::Hit &hit, const traceur::Scene &scene, const traceur::Ray &ray) const {
	traceur::Pixel pixel = traceur::Pixel(0, 0, 0);
	glm::vec3 vertexPos = ray.origin + (hit.distance * ray.direction);

	for (int i = 0; i < scene.lights.size(); i++) {
		glm::vec3 lightDir = scene.lights[i] - vertexPos;
		lightDir = glm::normalize(lightDir);
		pixel += hit.primitive->material->ambient;
		pixel = diffuseOnly(hit, lightDir);
		pixel += blinnPhong(hit, scene, ray, vertexPos, lightDir);
	}

	return pixel;
}
traceur::Pixel traceur::BasicKernel::diffuseOnly(const traceur::Hit &hit, const glm::vec3 &lightDir) const {
	float dot = glm::dot(hit.normal, lightDir); // glm::dot(hit.normal, lightDir);
	if (dot < 0.0f) dot = 0.0f;

	//else printf("%f\n", dot);
	//auto x = hit.primitive->material->diffuse;
	//std::cout << dot << " " << lightDir.y << " " << lightDir.z << std::endl;
	return hit.primitive->material->diffuse * dot;
}

traceur::Pixel traceur::BasicKernel::blinnPhong(const traceur::Hit &hit, const traceur::Scene &scene, const traceur::Ray &ray, const glm::vec3 &vertexPos, const glm::vec3 &lightDir) const {
	glm::vec3 viewDir = scene.camera.position - vertexPos;
	viewDir = glm::normalize(viewDir);

	glm::vec3 halfVector = viewDir + lightDir;
	halfVector = glm::normalize(halfVector);

	float specularity = std::max(glm::dot(halfVector, hit.normal), 0.0f);
	specularity = pow(specularity, hit.primitive->material->shininess);

	return specularity * hit.primitive->material->specular;
}

std::unique_ptr<traceur::Film> traceur::BasicKernel::render(const traceur::Scene &scene,
															const traceur::Camera &camera) const
{
	auto film = std::make_unique<traceur::DirectFilm>(camera.viewport[2],
													  camera.viewport[3]);
	render(scene, camera, *film, glm::ivec2());
	return std::move(film);
}

void traceur::BasicKernel::render(const traceur::Scene &scene,
								  const traceur::Camera &camera,
								  traceur::Film &film,
								  const glm::ivec2 &offset) const
{
	traceur::Ray ray;
	traceur::Pixel pixel;

	for (int y = 0; y < film.height; y++) {
		for (int x = 0; x < film.width; x++) {
			ray = camera.rayFrom(glm::vec2(x + offset[0], y + offset[1]));
			pixel = trace(scene, ray, 0);
			film(x, y) = pixel;
		}
	}
}
