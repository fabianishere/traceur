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
		return shade(hit, scene, ray, i);
	}
	return traceur::Pixel(0,0,0);
}
// TODO: Make 0.2f and 0.8f variables.
traceur::Pixel traceur::BasicKernel::shade(const traceur::Hit &hit, const traceur::Scene &scene, const traceur::Ray &ray, int recursion) const {
	traceur::Pixel result = traceur::Pixel(0, 0, 0);
	glm::vec3 vertexPos = ray.origin + (hit.distance * ray.direction);

	for (traceur::Light lightSource : scene.lights) {

		result += hit.primitive->material->ambient * 0.2f;

		// const traceur::Light &lightSource, const traceur::Hit &hit, const traceur::Scene &scene
		float level = lightLevel(lightSource, hit, scene);

		// no diffuse or specular when in a shadow
		if (level <= 0) {
			continue;
		}

		// diffuse only
		glm::vec3 lightDir = lightSource - vertexPos;
		lightDir = glm::normalize(lightDir);
		result += level * diffuseOnly(hit, lightDir);

		// blinn phong
		result += level * blinnPhong(hit, scene, ray, vertexPos, lightDir);

	}
	if (recursion < 2 && hit.primitive->material->shininess > 0) {
		result += (hit.primitive->material->shininess) * reflectionOnly(hit, scene, ray, vertexPos, recursion + 1);
	}

	if (result.x > 1) result.x = 1;
	if (result.y > 1) result.y = 1;
	if (result.z > 1) result.z = 1;

	if (result.x < 0) result.x = 0;
	if (result.y < 0) result.y = 0;
	if (result.z < 0) result.z = 0;

	return result;
}
traceur::Pixel traceur::BasicKernel::diffuseOnly(const traceur::Hit &hit, const glm::vec3 &lightDir) const {
	float dot = glm::dot(hit.normal, lightDir); 
	if (dot < 0.0f) dot = 0.0f;

	return hit.primitive->material->diffuse * dot * 0.8f;
}

traceur::Pixel traceur::BasicKernel::blinnPhong(const traceur::Hit &hit, const traceur::Scene &scene, const traceur::Ray &ray, const glm::vec3 &vertexPos, const glm::vec3 &lightDir) const {
	traceur::Pixel pixel(0.0f, 0.0f, 0.0f);
	//double shininess = hit.primitive->material->shininess * 1000;

	if (hit.primitive->material->shininess == 0) {
		return pixel;
	}

	glm::vec3 viewDir = glm::normalize(ray.origin - vertexPos);
	glm::vec3 reflection = ray.direction - (2 * glm::dot(hit.normal, ray.direction) * hit.normal);
	
	double dot = glm::dot(viewDir, reflection);

	if (dot <= 0) {
		return pixel;
	}

	float specularity = pow(dot, hit.primitive->material->shininess*1000);

	return traceur::Pixel(specularity, specularity, specularity);
	/*glm::vec3 viewDir = scene.camera.position - vertexPos;
	viewDir = glm::normalize(viewDir);


	float specularity = 0.0f;
	if (glm::dot(hit.normal, lightDir) > 0) {
		glm::vec3 halfVector = glm::normalize(lightDir + viewDir);
		specularity = pow(glm::dot(hit.normal, halfVector), hit.primitive->material->shininess*1000);
	}
	return hit.primitive->material->specular * specularity * hit.primitive->material->diffuse;
	*/
	/*glm::vec3 halfVector = viewDir + lightDir;
	halfVector = glm::normalize(halfVector);

	float specularity = std::max(glm::dot(halfVector, hit.normal), 0.0f);
	if (specularity > 0) {
		specularity = pow(specularity, (hit.primitive->material->shininess));
		return specularity * hit.primitive->material->specular;
	}
	return traceur::Pixel(0, 0, 0);*/

}

traceur::Pixel traceur::BasicKernel::reflectionOnly(const traceur::Hit &hit, const traceur::Scene &scene, const traceur::Ray &ray, const glm::vec3 &vertexPos, int recursion) const {
	glm::vec3 reflection = ray.direction - (2 * glm::dot(hit.normal, ray.direction) * hit.normal);
	glm::vec3 vertexPosOffset = vertexPos;
	glm::vec3 destinationOffset = vertexPos + reflection;

	Offset(&vertexPosOffset, &destinationOffset);

	traceur::Ray newRay = traceur::Ray();
	newRay.origin = vertexPosOffset;
	newRay.direction = destinationOffset;

	return trace(scene, newRay, recursion);
}

void traceur::BasicKernel::Offset(glm::vec3* inter, glm::vec3* dest) const{
	glm::vec3 vector = (*dest) - (*inter);
	vector = glm::normalize(vector);
	vector *= 0.0001;
	*inter += vector;
}

float traceur::BasicKernel::lightLevel(const traceur::Light &lightSource, const traceur::Hit &hit, const traceur::Scene &scene) const {
	float resLevel = 0;
	for (int i = 0; i < 10; i++) {
		// run X fake light sources

		float LO = -0.15;
		float HI = 0.15;
		float offsetX = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
		float offsetY = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
		float offsetZ = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));

		float level = localLightLevel(glm::vec3(offsetX, offsetY, offsetZ) + lightSource, hit, scene);
		resLevel += (level / ((float)10));
	}

	return resLevel;
}
 
float traceur::BasicKernel::localLightLevel(const traceur::Light &lightSource, const traceur::Hit &hit, const traceur::Scene &scene) const{
	// Can be optimized by having a different intersect method that returns false upon first impact that is closer than the light.

	glm::vec3 origin = lightSource;
	glm::vec3 direction = hit.position - lightSource;
	direction = glm::normalize(direction);
	traceur::Ray newRay = traceur::Ray(origin, direction);

	traceur::Hit foundHit;
	if (scene.graph->intersect(newRay, foundHit)) {
		// check if foundHit is equal to hit
		glm::vec3 res = foundHit.position - hit.position;
		// epsilon comparison
		bool nothingBetween = ((abs(res[0]) < 0.001) && (abs(res[1]) < 0.001) && (abs(res[2]) < 0.001));
		bool inShadow = !nothingBetween;

		if (inShadow) {
			return 0;
		}
		return 1;
	}
	return 1;
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
	/*printf("X%d\n", offset[0]);
	printf("Y%d\n", offset[1]);
	printf("Height%d\n", film.height);
	printf("Width%d\n", film.width);*/

	//printf("[%6.4f%%]", 00.00);
	for (int y = 0; y < film.height; y++) {
		double percent = 100 * y/film.height;
		printf("\r[%6.4f%%]", percent);

		for (int x = 0; x < film.width; x++) {
			ray = camera.rayFrom(glm::vec2(x + offset[0], y + offset[1]));
			pixel = trace(scene, ray, 0);
			film(x, y) = pixel;
		}
	}
}
