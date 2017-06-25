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
#ifndef TRACEUR_CORE_KERNEL_MULTITHREADED_H
#define TRACEUR_CORE_KERNEL_MULTITHREADED_H

#include <memory>
#include <thread>
#include <queue>
#include <future>
#include <mutex>
#include <condition_variable>

#include <traceur/core/kernel/kernel.hpp>
#include <traceur/core/kernel/pixel.hpp>

namespace traceur {
	/**
	 * A thread pool for the {@link MultithreadedKernel} class.
	 */
	class MultithreadedKernelPool {
	public:
		/**
		 * Construct a {@link MultithreadedKernelPool}.
		 *
		 * @param[in] workers The maximum amount of worker threads to spawn.
		 */
		MultithreadedKernelPool(int);

		/**
		 * Destruct a {@link MultithreadedKernelPool} instance.
		 */
		~MultithreadedKernelPool();

		/**
		 * Enqueue a render job to the job queue.
		 *
		 * @param[in] f The job to enqueue.
		 * @param[in] args The arguments passed to the job.
		 * @return A future result of the job.
		 */
		template<class F, class... Args>
		auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
		{
			using return_type = typename std::result_of<F(Args...)>::type;
			auto job = std::make_shared< std::packaged_task<return_type()> >(
				std::bind(std::forward<F>(f), std::forward<Args>(args)...)
			);

			std::future<return_type> res = job->get_future();
			{
				/* Enqueue the job into the queue */
				std::unique_lock<std::mutex> lock(mutex);
				jobs.emplace([job](){ (*job)(); });
			}

			/* Inform one of the workers */
			condition.notify_one();
			return res;
		}
	private:
		/**
		 * The {@link MultithreadedKernelWorker} can access the privates of this
		 * class.
		 */
		friend class MultithreadedKernelWorker;

		/**
		 * The render job queue.
		 */
		std::queue<std::function<void()>> jobs;

		/**
		 * Synchronisation primitives to prevent worker threads from colliding.
		 */
		std::mutex mutex;
		std::condition_variable condition;

		/**
		 * A flag to indicate the kernel wants to stop.
		 */
		bool stop;

		/**
		 * The thread pool we use.
		 */
		std::vector<std::thread> pool;
	};

	/**
	 * A worker for a {@link MultithreadedKernel} instance.
	 */
	class MultithreadedKernelWorker {
	public:
		/**
		 * Construct a {@link MultithreadedKernelWorker} instance.
		 *
		 * @param[in] pool The thread pool the worker is part of.
		 */
		MultithreadedKernelWorker(traceur::MultithreadedKernelPool &pool) : pool(pool) { }

		/**
		 * This method is invoked by the backing thread of the worker and uses
		 * the job queue to execute render jobs.
		 */
		void operator()();
	private:
		/**
		 * The thread pool the worker is part of.
		 */
		traceur::MultithreadedKernelPool &pool;
	};

	/**
	 * A scheduling {@link Kernel} that runs another kernel on multiple threads.
	 */
	class MultithreadedKernel: public Kernel {
	public:
		/**
		 * The amount of worker threads that the kernel spawns for a render job.
		 */
		int workers;

		/**
		 * The amount of partitions the render job is divided in.
		 */
		int partitions;

		/**
		 * Construct a {@link MultithreadedKernel}.
		 *
		 * @param[in] kernel The ray-tracing {@link Kernel} to use.
		 * @param[in] workers The maximum amount of worker threads to spawn.
		 * @param[in] partitions The maximum amount of partitions to divide
		 * the render job into.
		 */
		MultithreadedKernel(const std::shared_ptr<traceur::Kernel>, int, int);

		/**
		 * Render the camera view of the given {@link Scene} into a
		 * {@link Film}.
		 *
		 * @param[in] scene The {@link Scene} to render.
		 * @param[in] camera The {@link Camera} to use.
		 * @return A {@link Film} of the scene to take ownership over.
		 */
		virtual std::unique_ptr<traceur::Film> render(const traceur::Scene &,
													  const traceur::Camera &) const final;

		/**
		 * Render a part of the given {@link Scene} into the {@link Film}
		 * passed to this function.
		 *
		 * NOTE: This method does not work with multiple threads.
		 *
		 * @param[in] scene The {@link Scene} to render.
		 * @param[in] camera The {@link Camera} to use.
		 * @param[in] film The film to render the scene into.
		 * @param[in] offset The offset of the screen.
		 */
		virtual void render(const traceur::Scene &,
							const traceur::Camera &,
							traceur::Film &,
							const glm::ivec2 &) const final;

		/**
		 * Return the name of this kernel.
		 *
		 * @return A string representing the name of this kernel.
		 */
		virtual const std::string & name() const final
		{
			static const std::string name = kernel->name() + "-multithreaded-"
											+ std::to_string(workers) + "/"
											+ std::to_string(partitions);
			return name;
		}
	private:
		/**
		 * The underlying kernel to use.
		 */
		std::shared_ptr<traceur::Kernel> kernel;

		/**
		 * The thread pool the kernel uses.
		 */
		mutable traceur::MultithreadedKernelPool pool;
	};
}

#endif /* TRACEUR_CORE_KERNEL_MULTITHREADED_H */
