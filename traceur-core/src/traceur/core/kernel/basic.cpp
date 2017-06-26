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
#include <iostream>
#include <glm/gtx/string_cast.hpp>

traceur::Pixel traceur::BasicKernel::shade(const traceur::TracingContext &context,
										   int depth) const
{
	auto result = traceur::Pixel(0, 0, 0);
    float ambientLight = 0.2f;
    int maxDepth = 8;

	auto &material = context.hit.primitive->material;

    if (material->illuminationModel > 0 && material->illuminationModel < 10) {
        // Ambient light
        result = material->ambient * ambientLight;

        // Setting up loop-over variables
        glm::vec3 diffuseReflectanceMultiples = glm::vec3(0,0,0);
        glm::vec3 specularReflectanceMultiples = glm::vec3(0,0,0);

        // For each light
        for (auto &light : context.scene.lights) {
            auto lightDir = glm::normalize(light - context.hit.position);

            // Fetch light level
            float lightCastIntensity = lightLevel(light, context.hit, context.scene);

            // Give lightLevel as raw output for the first light:
            // return glm::vec3(1,1,1) * lightCastIntensity;

            // Diffuse illumination model using Lambertian shading
            diffuseReflectanceMultiples += diffuse(context, lightDir) * lightCastIntensity;

            // Specular illumination model
            switch (material->illuminationModel) {
                case 2:
                case 3:
                case 4:
                case 6:
                case 8:
                case 9:
                    // Specular * ( {SUM specular() } ) : 2
                    // Specular * ( {SUM specular() } + reflection() ) : 3, 4, 6, 8, 9
                    specularReflectanceMultiples += specular(context, lightDir) * lightCastIntensity;
                    break;
                case 5:
                case 7:
                    // Specular * ( {SUM specular() * fresnelLight()} + fresnelFinal() ) : 5, 7
                    break;
                default:
                    break;
            }

        }

        // Finalising loop-over variables
        switch (material->illuminationModel) {
            case 3:
            case 4:
            case 6:
            case 8:
            case 9:
                // Specular * ( {SUM specular() } + reflection() ) : 3, 4, 6, 8, 9
                if (depth < maxDepth) {
                    specularReflectanceMultiples += reflection(context, depth + 1);
                }
                break;
            case 5:
            case 7:
                // Specular * ( {SUM specular() * fresnelLight()} + fresnelFinal() ) : 5, 7

                // Todo: replace this fallback! This is not Fresnel reflection but normal reflection
                if (depth < maxDepth) {
                    specularReflectanceMultiples += reflection(context, depth + 1);
                }
                break;
            default:
                break;
        }

        // Multiplying loop-over variables with multipliers
        diffuseReflectanceMultiples *= material->diffuse;
        specularReflectanceMultiples *= material->specular;

        // Add finalised values to result
        result += diffuseReflectanceMultiples;
        result += specularReflectanceMultiples;

        // Handling transparent objects
        switch (material->illuminationModel) {
            case 4:
                // Transparency mode
                if (depth < maxDepth) {
                    result *= 1.f - material->transparency;
                    result += material->transparency * transparent(context, depth + 1);
                }
                break;
            case 6:
                // Basic refraction
                // (1.0 - mat.specular) mat.transmissionFilter * refraction() : 6
                if (depth < maxDepth) {
                    result += (1.f - material->specular) * material->transmissionFilter * refraction(context, depth + 1);
                }
                break;
            case 7:
                // Fresnel refraction
                // (1.0 - Kx)Ft (N*V,(1.0-mat.specular),mat.shininess)mat.transmissionFilter * refraction() : 7

                // Todo: replace this fallback! This is not Fresnel refraction but normal refraction
                if (depth < maxDepth) {
                    result += (1.f - material->specular) * material->transmissionFilter * refraction(context, depth + 1);
                }
                break;
            default:
                break;
        }

    } else {
        // Direct color output on illuminationModel 0
        result = material->diffuse;
    }

    // Return final value
    return glm::clamp(result, 0.f, 1.f);
}

traceur::Pixel traceur::BasicKernel::diffuse(const traceur::TracingContext &context,
											 const glm::vec3 &lightDir) const
{
	float intensity = std::max(0.f, glm::dot(context.hit.normal, lightDir));
	return intensity * glm::vec3(1,1,1);
}

traceur::Pixel traceur::BasicKernel::specular(const traceur::TracingContext &context,
											  const glm::vec3 &lightDir) const
{
	auto &ray = context.ray;
	auto &hit = context.hit;
	auto &material = hit.primitive->material;

	auto viewDir = glm::normalize(context.ray.origin - hit.position);
	auto reflection = glm::reflect(context.ray.direction, hit.normal);

	float angle = std::max(0.f, glm::dot(viewDir, reflection));
	float intensity = powf(angle, material->shininess);

	return intensity * glm::vec3(1,1,1);
}

traceur::Pixel traceur::BasicKernel::reflection(const traceur::TracingContext &context,
                                                 int depth) const
{
    return reflection(context, depth, context.hit.normal);
}

traceur::Pixel traceur::BasicKernel::reflection(const traceur::TracingContext &context,
                                                 int depth, const glm::vec3 &normal) const
{
    glm::vec3 newDirection = glm::reflect(context.ray.direction, normal);
    glm::vec3 newOrigin = context.hit.position + globalOffset * newDirection;

    auto next = traceur::Ray(newOrigin, newDirection);
    return trace(context.scene, context.camera, next, depth);
}

traceur::Pixel traceur::BasicKernel::refraction(const traceur::TracingContext &context,
                                                int depth) const
{
    // eta = refractiveIndex(sourceMaterial) / refractiveIndex(destinationMaterial)
    float sourceDestRefraction;
    glm::vec3 refractionNormal;
    
    if(glm::dot(context.hit.normal, context.ray.direction) < 0) {
        // enter material
        sourceDestRefraction = 1.f / context.hit.primitive->material->opticalDensity;
        refractionNormal = context.hit.normal;
    } else {
        // exit material
        sourceDestRefraction = context.hit.primitive->material->opticalDensity / 1.f;
        refractionNormal = - context.hit.normal;
    }

    glm::vec3 newDirection = glm::refract(context.ray.direction, refractionNormal,
                                        sourceDestRefraction);

    if (isnan(newDirection.x) || isnan(newDirection.y) || isnan(newDirection.z)) {
        // Full internal ray reflection
        return reflection(context, depth, refractionNormal);
    }

    glm::vec3 newOrigin = context.hit.position + globalOffset * newDirection;

    auto next = traceur::Ray(newOrigin, newDirection);
    return trace(context.scene, context.camera, next, depth);

}

traceur::Pixel traceur::BasicKernel::transparent(const traceur::TracingContext &context,
                                                 int depth) const
{
    glm::vec3 newDirection = context.ray.direction;
    glm::vec3 newOrigin = context.hit.position + globalOffset * newDirection;
    auto next = traceur::Ray(newOrigin, newDirection);
    return trace(context.scene, context.camera, next, depth);
}

/*
 * This function is NOT used by the multithreading kernel.
*/
std::unique_ptr<traceur::Film> traceur::BasicKernel::render(const traceur::Scene &scene,
															const traceur::Camera &camera) const
{
	// the default camera viewport is a vec4 in the following format;
	// ivec4(0, 0, viewWidth, viewHeight)
	int width = camera.viewport.z;
	int height = camera.viewport.w;

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
			pixel = trace(scene, camera, ray, 0);

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
										   const traceur::Camera &camera,
										   const traceur::Ray &ray,
										   int depth) const
{
	traceur::Hit hit;
	// Find the intersection of ray with the nearest object.
	// The nearest object is stored in hit. The function intersect
	// returns true if there is an intersection, false otherwise.
	if (scene.graph->intersect(ray, hit)) {
		// hit.primitive returns the type, so for example a triangle,
		// sphere, etc... This object has a material. The material
		// contains the diffuse, Kd, Ks and shininess values.
		return shade(traceur::TracingContext(scene, camera, ray, hit), depth);
	}

	// return an empty pixel (0,0,0)
	return traceur::Pixel();
}

float traceur::BasicKernel::lightLevel(const traceur::Light &lightSource, const traceur::Hit &hit, const traceur::Scene &scene) const {
    float resLevel = 0;
    
    srand(1);
    for (int i = 0; i < 50; i++) {
        // run X fake light sources

        float LO = -0.05;
        float HI = 0.05;
        float offsetX = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
        float offsetY = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
        float offsetZ = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));

        float level = localLightLevel(glm::vec3(offsetX, offsetY, offsetZ) + lightSource, hit, scene);
        resLevel += (level / ((float)50));
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


        bool nothingBetween = ((std::abs(res[0]) < 0.001) && (std::abs(res[1]) < 0.001) && (std::abs(res[2]) < 0.001));
        bool inShadow = !nothingBetween;

        if (inShadow) {
            return 0;
        }
        return 1;
    }
    return 1;
}