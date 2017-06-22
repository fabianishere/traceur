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

/*
 * This function is NOT used by the multithreading kernel.
*/
std::unique_ptr<traceur::Film> traceur::BasicKernel::render(const traceur::Scene &scene,
															const traceur::Camera &camera) const
{
	// the default camera viewport is a vec4 in the following format;
	// ivec4(0, 0, viewWidth, viewHeight) 
	int width = camera.viewport[2];
	int height = camera.viewport[3];

	// create a unique instance of the DirectFilm given the width and height
	auto film = std::make_unique<traceur::DirectFilm>(width, height);

	// film is a pointer, so pass the reference using the *
	// call the render function with offset(0,0), the multithreading
	// kernel will pass different values here
	render(scene, camera, *film, glm::ivec2());

	// return the film and move the responsibility of deallocation
	// to the caller using std::move
	return std::move(film);
}

/*
* This function is used by the multithreading kernel.
*/
void traceur::BasicKernel::render(const traceur::Scene &scene,
								  const traceur::Camera &camera,
								  traceur::Film &film,
								  const glm::ivec2 &offset) const
{
	traceur::Ray ray;
	traceur::Pixel pixel;

	// loop through all pixels on the film
	// for performance, loop over y first
	for (int y = 0; y < film.height; y++) {
		for (int x = 0; x < film.width; x++) {
			// create a ray from camera to (x + offsetX, y + offsetY)
			ray = camera.rayFrom(glm::ivec2(x, y) + offset);

			// trace the ray through the scene, this returns a Pixel.
			// a Pixel is equivalent to a ivec3, containing the color
			// of the pixel as R,G,B values. The location of the
			// intersection point is NOT known!
			pixel = trace(scene, ray);

			// write the pixel color to the array
			film(x, y) = pixel;
		}
	}
}

/* 
 * This function is responsible for the tracing of a Ray through a Scene.
 *
 * This function only returns the color of the point of intersection with
 * the nearest object. The location of the intersection is not returned.
 *
 * An empty pixel (0,0,0) is returned when there is no intersection.
*/
traceur::Pixel traceur::BasicKernel::trace(const traceur::Scene &scene,
											const traceur::Ray &ray) const
{
	traceur::Hit hit;

	// Find the intersection of ray with the nearest object.
	// The nearest object is stored in hit. The function intersect
	// returns true if there is an intersection, false otherwise.
	if (scene.graph->intersect(ray, hit)) {

		// hit.primitive returns the type, so for example a triangle,
		// sphere, etc... This object has a material. The material
		// contains the diffuse, Kd, Ks and shininess values.
		// For now we only need the diffuse, which is an RGB color.
		return hit.primitive->material->diffuse;
	}

	// return an empty pixel (0,0,0)
	return traceur::Pixel();
}
