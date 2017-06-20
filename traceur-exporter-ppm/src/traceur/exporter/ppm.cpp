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

#include <traceur/exporter/ppm.hpp>
#include <glm/gtc/type_ptr.hpp>

void traceur::PPMExporter::write(const traceur::Film &film,
								 const std::string &path) const
{
	auto file = fopen(path.c_str(), "w");

	if (!file) {
		/* TODO throw exception */
		perror("error: failed to export file\n");
		return;
	}

	static char rgb[3];

	// Write file header
	fprintf(file, "P6\n%lu %lu\n255\n", film.width, film.height);
	// Write the pixels
	for (int y = 0; y < film.height; y++) {
		for (int x = 0; x < film.width; x++) {
			auto pixel = film(x, y) * 255.0f;

			rgb[0] = (unsigned char) pixel[0];
			rgb[1] = (unsigned char) pixel[1];
			rgb[2] = (unsigned char) pixel[2];
			
 			fwrite(rgb, 3, 1, file);
		}
	}
	// Close the file
	fclose(file);
}