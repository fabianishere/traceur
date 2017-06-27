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
#ifndef TRACEUR_CORE_KERNEL_FILM_H
#define TRACEUR_CORE_KERNEL_FILM_H

#include <vector>
#include <algorithm>

#include <glm/glm.hpp>
#include <traceur/core/kernel/pixel.hpp>

namespace traceur {
	/**
	 * A raster on which a {@link Scene} is projected via a raytracing
	 * {@link Kernel}.
	 */
	class Film {
	public:
		/**
		 * The width of this film.
		 */
		const int width;

		/**
		 * The height of this film.
		 */
		const int height;

		/**
		 * Construct a {@link Film} instance.
		 *
		 * @param[in] width The width of the film.
		 * @param[in] height The height of the film.
		 */
		Film(int width, int height) :
			width(width), height(height) {}

		/**
		 * Deconstruct a {@link Film} instance.
		 */
		virtual ~Film() {}

		/**
		 * Return the reference to a {@link Pixel} in this film.
		 *
		 * @param[in] pos The position within the film.
		 * @return A reference to the {@link Pixel}.
		 */
		virtual traceur::Pixel & operator()(const glm::ivec2 &) = 0;

		/**
		 * Return the reference to a {@link Pixel} in this film.
		 *
		 * @param[in] x The x coordinate in the film.
		 * @param[in] y The y coordinate in the film.
		 * @return A reference to the {@link Pixel}.
		 */
		virtual traceur::Pixel & operator()(int x, int y)
		{
			return this->operator()(glm::ivec2(x, y));
		}

		/**
		 * Return the pixel value to a {@link Pixel} in this film.
		 *
		 * @param[in] x The x coordinate in the film.
		 * @param[in] y The y coordinate in the film.
		 * @return The pixel value.
		 */
		virtual traceur::Pixel operator()(const glm::ivec2 &) const = 0;

		/**
		 * Return the pixel value to a {@link Pixel} in this film.
		 *
		 * @param[in] pos The position within the film.
		 * @return The pixel value.
		 */
		virtual traceur::Pixel operator()(int x, int y) const
		{
			return this->operator()(glm::ivec2(x, y));
		}

	};

	/**
	 * A {@link Film} that is backed by a continuous memory region to allow
	 * fast read and writes.
	 */
	class DirectFilm: public Film {
		/**
		 * The buffer to which we write.
		 */
		std::vector<traceur::Pixel> buffer;
	public:
		/**
		 * Construct a {@link DirectFilm} instance.
		 *
		 * @param[in] width The width of the film.
		 * @param[in] height The height of the film.
		 */
		DirectFilm(int width, int height) :
			Film(width, height), buffer(static_cast<size_t>(std::max(0, width * height))) {}

		/**
		 * Return the reference to a {@link Pixel} in this film.
		 *
		 * @param[in] pos The position within the film.
		 * @return A reference to the {@link Pixel}.
		 */
		inline virtual traceur::Pixel & operator()(const glm::ivec2 &pos) final
		{
			return buffer[pos.y * width + pos.x];
		}

		/**
		 * Return the pixel value to a {@link Pixel} in this film.
		 *
		 *  @param[in] pos The position within the film.
		 * @return The pixel value.
		 */
		inline virtual traceur::Pixel operator()(const glm::ivec2 &pos) const final
		{
			return buffer[pos.y * width + pos.x];
		}

		/**
		 * Return the frame buffer of this film.
		 *
		 * @return the frame buffer of this film.
		 */
		traceur::Pixel * data()
		{
			return buffer.data();
		}

		/**
		 * Return the frame buffer of this film.
		 *
		 * @return the frame buffer of this film.
		 */
		const traceur::Pixel * data() const
		{
			return buffer.data();
		}
	};

	/**
	 * A {@link Film} that is partitioned in multiple subfilms on which parts
	 * of the scene are projected via a raytracing {@link Kernel}.
	 */
	template<typename T = DirectFilm, typename... Args>
	class PartitionedFilm: public Film {
		/**
		 * The partitions of this film.
		 */
		std::vector<std::unique_ptr<T>> partitions;

		/**
		 * The minimum width of a partition.
		 */
		int px;

		/**
		 * The minimum height of a partition.
		 */
		int py;

		/**
		 * The columns per row.
		 */
		int columns;

		/**
		 * The rows of the film.
		 */
		int rows;
	public:
		/**
		 * The amount of partitions in the film.
		 */
		const int n;

		/**
		 * Construct a {@link PartitionedFilm} instance.
		 *
		 * @param[in] width The width of the film.
		 * @param[in] height The height of the film.
		 * @param[in] n The amount of partitions to create.
		 * @param[in] args The arguments to further instantiate the underlying
		 * partition type.
		 */
		PartitionedFilm(int width, int height, int n, Args&&... args)
			: Film(width, height), n(n)
		{

			/*
			 * Algorithm for dividing rectangles into n partitions, which is
			 * basically finding the factors p and q in the equation n = p*q.
			 */
			columns = static_cast<int>(sqrt(n));
			while (n % columns != 0) {
				columns--;
			}
			rows = n / columns;

			/* Calculate size of partitions */
			px = width / columns;
			py = height / rows;
			int rx = width % columns;
			int ry = height % rows;

			/* Reserve space for n partitions */
			partitions.reserve(static_cast<size_t>(n));

			/* Create partitions of the specified size and amount of partitions */
			for (int row = 0; row < rows; row++) {
				for (int column = 0; column < columns; column++) {
					partitions.emplace_back(std::make_unique<T>(
						px + (column == columns - 1) * rx,
						py + (row == rows - 1) * ry,
						std::forward<Args>(args)...
					));
				}
			}
		}

		/**
		 * Return an unowned reference to a partition in this film.
		 *
		 * @param[in] n The number of partition to get.
		 * @return The partition to get.
		 */
		inline T & operator()(int n)
		{
			return *partitions[n];
		}

		/**
		 * Return the offset of a particular partition within the film.
		 *
		 * @param[in] n The partition to get the offset of.
		 * @return The offset within the film.
		 */
		inline glm::ivec2 offset(int n) const
		{
			return {
				(n % columns) * px,
				(n / columns) * py
			};
		}

		/**
		 * Return the reference to a {@link Pixel} in this film.
		 *
		 * @param[in] pos The position within the film.
		 * @return A reference to the {@link Pixel}.
		 */
		inline virtual traceur::Pixel & operator()(const glm::ivec2 &pos) final
		{
			int j = std::min(pos.x / px, columns - 1);
			int i = std::min(pos.y / py, rows - 1);
			int n = i * columns + j;
			return partitions[n]->operator()(pos - offset(n));
		}

		/**
		 * Return the pixel value to a {@link Pixel} in this film.
		 *
		 * @param[in] pos The position within the film.
		 * @return The pixel value.
		 */
		inline virtual traceur::Pixel operator()(const glm::ivec2 &pos) const final
		{
			int j = std::min(pos.x / px, columns - 1);
			int i = std::min(pos.y / py, rows - 1);
			int n = i * columns + j;
			return partitions[n]->operator()(pos - offset(n));
		}
	};
}
#endif /* TRACEUR_CORE_KERNEL_FILM_H */
