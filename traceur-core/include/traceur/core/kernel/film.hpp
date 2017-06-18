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
		const size_t width;

		/**
		 * The height of this film.
		 */
		const size_t height;

		/**
		 * Construct a {@link Film} instance.
		 *
		 * @param[in] width The width of the film.
		 * @param[in] height The height of the film.
		 */
		Film(size_t width, size_t height) : 
			width(width), height(height) {}

		/**
		 * Deconstruct a {@link Film} instance.
		 */
		virtual ~Film() {}

		/**
		 * Return the reference to a {@link Pixel} in this film.
		 *
		 * @param[in] x The x coordinate in the film.
		 * @param[in] y The y coordinate in the film.
		 * @return A reference to the {@link Pixel}.
		 */
		inline virtual traceur::Pixel & operator()(size_t, size_t) = 0;

		/**
		 * Return the pixel value to a {@link Pixel} in this film.
		 *
		 * @param[in] x The x coordinate in the film.
		 * @param[in] y The y coordinate in the film.
		 * @return The pixel value.
		 */
		inline virtual traceur::Pixel operator()(size_t, size_t ) const = 0;
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
		DirectFilm(size_t width, size_t height) : 
			Film(width, height), buffer(std::vector<traceur::Pixel>(width * height)) {}
	
		/**
		 * Return the reference to a {@link Pixel} in this film.
		 *
		 * @param[in] x The x coordinate in the film.
		 * @param[in] y The y coordinate in the film.
		 * @return A reference to the {@link Pixel}.
		 */
		inline virtual traceur::Pixel & operator()(size_t x, size_t y) final
		{
			return buffer[y * width + x];
		}

		/**
		 * Return the pixel value to a {@link Pixel} in this film.
		 *
		 * @param[in] x The x coordinate in the film.
		 * @param[in] y The y coordinate in the film.
		 * @return The pixel value.
		 */
		inline virtual traceur::Pixel operator()(size_t x, size_t y) const final
		{
			return buffer[y * width + x];
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
		std::vector<T> partitions;

		/**
		 * The width of a partition.
		 */
		size_t px;

		/**
		 * The height of a partition.
		 */
		size_t py;
	public:
		/**
		 * The amount of partitions in the film.
		 */
		const size_t n;

		/**
		 * The amount columns in the paritioned film.
		 */
		const size_t columns;

		/**
		 * Construct a {@link PartitionedFilm} instance.
		 *
		 * @param[in] width The width of the film.
		 * @param[in] height The height of the film.
		 * @param[in] columns The amount of columns in the partitioned film.
		 * @param[in] args The arguments to further instantiate the underlying
		 * partition type.
		 */
		PartitionedFilm(size_t width, size_t height, size_t columns, Args&&... args) : Film(width, height), columns(columns), n(columns * columns)
		{
			partitions.reserve(n);

			px = width / columns;
			py = height / columns;
			size_t rx = width % columns;
			size_t ry = height % columns;

			/* Create partitions of the specified size and amount of partitions */
			for (int i = 0; i < n - 1; i++) {
				partitions.push_back(T(px, py, std::forward<Args>(args)...));
			}

			if (n > 1) {
				partitions.push_back(T(px + rx, py + ry, std::forward<Args>(args)...));
			}
		}

		/**
		 * Return a reference to a partition in this film.
		 *
		 * @param[in] n The number of partition to get.
		 * @return The partition to get.
		 */
		inline T & operator()(size_t n)
		{
			return partitions[n];
		}


		/**
		 * Return the reference to a {@link Pixel} in this film.
		 *
		 * @param[in] x The x coordinate in the film.
		 * @param[in] y The y coordinate in the film.
		 * @return A reference to the {@link Pixel}.
		 */
		inline virtual traceur::Pixel & operator()(size_t x, size_t y) final
		{
			size_t j = x / px;
			size_t i = y / py;
			size_t rx = x % px;
			size_t ry = y % py;

			return partitions[i * columns + j](rx, ry);
		}

		/**
		 * Return the pixel value to a {@link Pixel} in this film.
		 *
		 * @param[in] x The x coordinate in the film.
		 * @param[in] y The y coordinate in the film.
		 * @return The pixel value.
		 */
		inline virtual traceur::Pixel operator()(size_t x, size_t y) const final
		{
			size_t j = x / px;
			size_t i = y / py;
			size_t rx = x % px;
			size_t ry = y % py;

			return partitions[i * columns + j](rx, ry);
		}

	};
}
#endif /* TRACEUR_CORE_KERNEL_FILM_H */
