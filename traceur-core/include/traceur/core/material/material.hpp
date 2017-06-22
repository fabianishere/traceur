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
#ifndef TRACEUR_CORE_MATERIAL_MATERIAL_H
#define TRACEUR_CORE_MATERIAL_MATERIAL_H

#include <glm/glm.hpp>

namespace traceur {
	/**
	 * The {@link Material} class contains the information, such as the 
	 * diffuse, ambient and specular terms of the material.
	 */
	class Material {
	public:
		/**
		 * The diffuse, ambient and specular terms of this material.
		 */
		glm::fvec3 diffuse, ambient, specular;
		//glm::vec3 diffuse, ambient, specular;

		/**
		 * The specular exponent of the material.
		 */
		float shininess;

		/**
		 * Construct a {@link Material} instance.
		 */
		Material() : shininess(0.f) {}

		/**
		 * Construct a {@link Material} instance.
		 *
		 * @param diffuse The diffuse term of the material.
		 * @param ambient The ambient term of the material.
		 * @param specular The specular term of the material.
		 * @param shininess The specular exponent of the material.
		 */
		Material(
				const glm::vec3 &diffuse, 
				const glm::vec3 &ambient, 
				const glm::vec3 &specular,
				float shininess
		) :
			diffuse(diffuse),
			ambient(ambient),
			specular(specular),
			shininess(shininess) {}
	};
}

#endif /* TRACEUR_CORE_MATERIAL_H */

