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

#include <glm/gtc/type_ptr.hpp>

#include <traceur/core/kernel/kernel.hpp>
#include <traceur/frontend/glut/preview.hpp>

void traceur::GLUTPreviewObserver::render()
{
	// Free textures on the GLUT main thread
	if (reset) {
		for (auto &part : partitions) {
			GLuint texture = part.second.texture;
			if (texture != 0) {
				glDeleteTextures(1, &texture);
			}
		}
		reset.store(false);
		condition.notify_all();
	}

	if (!enabled) {
		return;
	}

	// Change projection such that the preview is drawn as an overlay in
	// front of the camera.
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_WIDTH));
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslated(0, 0, 0);

	/* Draw render progress */
	std::unique_lock<std::mutex> lock(mutex);
	for (auto &part : partitions) {
		auto id = part.first;
		auto &partition = part.second;
		auto size = partition.size;
		auto offset = partition.offset;

		if (partition.finished) {
			// If there does not exist a texture for this partition, create a new one with the data in the film buffer.
			if (partition.texture == 0) {
				GLuint tex;
				glGenTextures(1, &tex);
				glBindTexture(GL_TEXTURE_2D, tex);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_FLOAT, glm::value_ptr(*partition.film->data()));
				partition.texture = tex;
			}
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, partition.texture);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glColor3f(255.f, 0.f, 0.f);
		}

		glBegin(GL_QUADS);
		float depth = !partition.finished / 10000.f;

		glTexCoord2i(1, 1);
		glVertex3f(offset.x + size.x, offset.y + size.y, depth);
		glTexCoord2i(0, 1);
		glVertex3f(offset.x, offset.y + size.y, depth);
		glTexCoord2i(0, 0);
		glVertex3f(offset.x, offset.y, depth);
		glTexCoord2i(1, 0);
		glVertex3f(offset.x + size.x, offset.y, depth);
		glEnd();

		if (partition.finished) {
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void traceur::GLUTPreviewObserver::renderStarted(const traceur::Kernel &kernel,
												 const traceur::Scene &,
												 const traceur::Camera &,
												 int partitions)
{
	reset.store(true);
	std::unique_lock<std::mutex> lock(mutex);
	condition.wait(lock);
}


void traceur::GLUTPreviewObserver::partitionStarted(const traceur::Kernel &,
													int id,
													const traceur::Film &film,
													const glm::ivec2 &offset)
{
	std::unique_lock<std::mutex> lock(mutex);
	// XXX We assume that a partition is always of type DirectFilm, so nested partitions will crash the program.
	partitions[id].film = dynamic_cast<const DirectFilm *>(&film);
	partitions[id].size = glm::ivec2(film.width, film.height);
	partitions[id].offset = offset;
	partitions[id].finished = false;
	partitions[id].texture = 0;
}

void traceur::GLUTPreviewObserver::partitionFinished(const traceur::Kernel &kernel,
													 int id,
													 const traceur::Film &film,
													 const glm::ivec2 &offset)
{
	std::unique_lock<std::mutex> lock(mutex);
	partitions[id].finished = true;
}

void traceur::GLUTPreviewObserver::renderFinished(const traceur::Kernel &kernel,
												  const traceur::Film &) {}
