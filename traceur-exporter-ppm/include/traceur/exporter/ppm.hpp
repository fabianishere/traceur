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
#ifndef TRACEUR_EXPORTER_PPM_H
#define TRACEUR_EXPORTER_PPM_H

#include <traceur/exporter/exporter.hpp>

namespace traceur {
	/**
	 * A {@link Exporter} that exports the film in a .ppm file format.
	 */
	class PPMExporter : public Exporter {
	public:
		/**
		 * Export the given film to the the file at the given path in the format
		 * of the exporter implementation.
		 *
		 * @param[in] buffer The frame buffer to export to the given stream.
		 * @param[in] path The path to the file to write the film to.
		 */
		virtual void write(const traceur::Film &,
						   const std::string &) const final;
	};
}

#endif /* TRACEUR_EXPORTER_PPM_H */
