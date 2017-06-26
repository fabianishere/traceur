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
		glm::vec3 diffuse, ambient, specular, transmissionFilter;

		/**
		 * The specular exponent of the material.
		 */
		float shininess;

		/**
		 * The optical density / index of refraction of the material.
		 */
		float opticalDensity;

        /**
         * The transparency value (as 0 opaque..1 transparent) of the material.
         */
        float transparency;

        /**
         * The illumination model of the material.
         */
        int illuminationModel;

		/**
		 * Construct a {@link Material} instance.
		 */
		Material() : shininess(0.f), opticalDensity(1.f), transparency(0.f), illuminationModel(1) {}

		/**
		 * Construct a {@link Material} instance.
		 *
		 * @param diffuse The diffuse term of the material.
		 * @param ambient The ambient term of the material.
		 * @param specular The specular term of the material.
		 * @param shininess The specular exponent of the material.
		 * @param optical_density The optical density of the material.
		 * @param transparency The transparency of the material.
		 * @param illumination_model The illumination model of the material.
		 */
		Material(
				const glm::vec3 &diffuse,
				const glm::vec3 &ambient,
				const glm::vec3 &specular,
                const glm::vec3 &transmissionFilter,
                float shininess,
				float optical_density,
                float transparency,
                int illumination_model
		) :
			diffuse(diffuse),
			ambient(ambient),
			specular(specular),
            transmissionFilter(transmissionFilter),
            shininess(shininess),
			opticalDensity(optical_density),
            transparency(transparency),
            illuminationModel(illumination_model) {}

    };
}

#endif /* TRACEUR_CORE_MATERIAL_H */

