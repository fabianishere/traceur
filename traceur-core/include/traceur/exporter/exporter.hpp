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
#ifndef TRACEUR_EXPORTER_EXPORTER_H
#define TRACEUR_EXPORTER_EXPORTER_H

#include <ostream>
#include <vector>

#include <glm/glm.hpp>

namespace traceur {
	/**
	 * This interface is implemented by classes that are able to export the
	 * result of a render in a specific file format.
	 */
	class Exporter {
	public:
		/**
		 * Deconstruct the {@link Exporter} instance.
		 */
		virtual ~Exporter() {}

		/**
		 * Export the given frame buffer to the the given stream in the format
		 * of the exporter implementation.
		 *
		 * @param[in] buffer The frame buffer to export to the given stream.
		 * @param[in] stream The stream to export the frame buffer to in the 
		 * given format.
		 */
		virtual void write(const std::vector<glm::vec3> &, std::ostream &) const = 0;
	};
}

#endif /* TRACEUR_EXPORTER_EXPORTER_H */
