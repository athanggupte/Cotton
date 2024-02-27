#include "Fiber.h"
#include "JobSystem.h"

#include <cassert>
#include <deque>
#include <mutex>
#include <vector>

#include "ThreadFiberContext.h"

namespace Cotton {

	JobDecl JobQueue::popJob()
	{
		std::unique_lock lock(m_mutex);
		int count = 0;
		while (count < kMaxRetries && m_jobs.empty())
		{
			++count;
			m_cvar.wait_for(lock, std::chrono::microseconds(25));
		}
		if (m_jobs.empty())
			return { nullptr };

		const JobDecl jobDecl = m_jobs.front();
		m_jobs.pop_front();
		return jobDecl;
	}

	void JobQueue::pushJob(JobDecl const& job_decl)
	{
		std::unique_lock lock(m_mutex);
		m_jobs.push_back(job_decl);
		m_cvar.notify_one();
	}

	struct FiberRef
	{
		uint32_t m_fiberId;
	};

	struct FiberList
	{
		std::vector<Fiber> m_fibers;
		std::deque<FiberRef> m_unusedFibers;

		std::mutex m_mutex;
		std::condition_variable m_cvar;

		void initialize(uint32_t size)
		{
			m_fibers.resize(size);

			for (uint32_t i = 0; i < size; i++)
			{
				m_unusedFibers.push_back({i});
			}
		}

		FiberRef getFiber()
		{
			std::unique_lock lock(m_mutex);
			while (m_unusedFibers.empty())
			{
				m_cvar.wait_for(lock, std::chrono::microseconds(100));
			}

			const FiberRef fiberIdx = m_unusedFibers.front();
			m_unusedFibers.pop_front();
			return fiberIdx;
		}

		void returnFiber(FiberRef fiber)
		{
			std::unique_lock lock(m_mutex);
			m_unusedFibers.push_back(fiber);
			m_cvar.notify_one();
		}
	};

	struct FiberWaitList
	{
		std::vector<FiberRef> m_fibers;

		bool checkFibersReady();
		bool addWaitingFiber();
	};

	namespace detail
	{
		JobQueue s_jobQueue;
		FiberList s_fiberList;
		FiberWaitList s_waitList;
		uint8_t *s_fiberStackMemory;

		std::mutex s_metricsMutex;
		std::vector<Metrics> s_metrics;

		std::vector<std::thread> s_threads;

		std::atomic_bool s_bIsRunning{false};
	};

	JobQueue& _getJobQueue()
	{
		return detail::s_jobQueue;
	}

	Metrics& _getMetrics()
	{
		std::lock_guard lock(detail::s_metricsMutex);
		return detail::s_metrics.emplace_back();
	}

	inline FiberList& _getFiberList()
	{
		return detail::s_fiberList;
	}

	inline FiberWaitList& _getFiberWaitList()
	{
		return detail::s_waitList;
	}

	inline Fiber& _fromFiberRef(FiberRef ref)
	{
		return detail::s_fiberList.m_fibers[ref.m_fiberId];
	}

	void initializeJobSystem()
	{
		using namespace detail;

		// s_jobQueue.m_jobs.resize(1024);
		s_fiberList.initialize(200);

		constexpr static uint64_t FIBER_STACK_SIZE = 8ui64 * 1024;
		s_fiberStackMemory = static_cast<uint8_t*>(malloc(FIBER_STACK_SIZE * s_fiberList.m_fibers.size()));

		uint8_t *pPtr = s_fiberStackMemory;
		for (Fiber& fiber : s_fiberList.m_fibers)
		{
			fiber.m_pStack = pPtr;
			fiber.m_stackSize = FIBER_STACK_SIZE;

			pPtr += FIBER_STACK_SIZE;
		}
	}

	void _initializeThreadForFibers(uint32_t thread_id)
	{
		Cotton::ThreadFiberContext* context = Cotton::_getThreadFiberContext();
		context->m_threadId = thread_id;
		Cotton::Fiber& mainFiber = context->m_jobSchedulerFiber;
		context->m_currentRunningFiber = &mainFiber;
		context->m_bIsRunning = true;

		//get_context(&mainFiber.m_context);
	}

	void _shutdownWorkerThread(void*)
	{
		//printf("Shutting down worker thread\n");
		// _getThreadFiberContext()->m_bIsRunning = false;
		_getThreadFiberContext()->m_bShouldExit = true;
	}

	void _runWorkerThread(uint32_t thread_id)
	{
		_initializeThreadForFibers(thread_id);

		auto ctx = *_getThreadFiberContext();

		while (ctx.m_bIsRunning) {
		//     Check if there is a waiting Fiber in the wait list that is ready to run, i.e. its wait counter is 0
		//         switch to the waiting Fiber if found
		//     Grab a JobDecl from the queue
		//     Pull a Fiber from the set of available fibers
		//         initialize the Fiber with the JobDecl
		//         switch to the Fiber
			
			JobDecl jobDecl = _getJobQueue().popJob();
			if (jobDecl.m_pEntryPoint != nullptr)
			{
				FiberRef fiberRef = _getFiberList().getFiber();
				{
					Fiber& fiber = _fromFiberRef(fiberRef);

					initializeFiber(fiber, jobDecl);
					switchToFiber(fiber);
				}

				_getFiberList().returnFiber(fiberRef);
			}
			else
			{
#define PRINT_THREAD_ID \
	printf("<%d", _getThreadFiberContext()->m_threadId); \
	for (int i = 0; i < _getThreadFiberContext()->m_threadId; i++) printf("*"); \
	printf(">")

				PRINT_THREAD_ID;
				printf(" : No job to run\n");
				if (_shouldExit() /*ctx.m_bShouldExit*/)
				{
					PRINT_THREAD_ID;
					printf(" : Exiting\n", _getThreadFiberContext()->m_threadId);
					break;
				}
				_mm_pause();
			}
		}

		_getThreadFiberContext()->submitMetrics();
		PRINT_THREAD_ID;
		printf(" : Exited\n");
	}

	void startJobSystem()
	{
		const uint32_t numThreads = std::min(std::thread::hardware_concurrency(), 8u);

		detail::s_threads.reserve(numThreads);
		detail::s_threads.clear();

		for (uint32_t i = 0; i < numThreads; i++)
		{
			detail::s_threads.emplace_back(_runWorkerThread, i);
		}
	}

	void _waitForAllJobs()
	{
		/*JobDecl jobDecl;
		jobDecl.m_pEntryPoint = _shutdownWorkerThread;

		while (!_getJobQueue().m_jobs.empty())
		{
			_mm_pause();
		}

		for (uint32_t i = 0; i < detail::s_threads.size(); i++)
		{
			_getJobQueue().pushJob(jobDecl);
		}*/

		detail::s_bIsRunning.store(false);

		for (uint32_t i = 0; i < detail::s_threads.size(); i++)
		{
			detail::s_threads[i].join();
		}
	}

	bool _shouldExit()
	{
		return !detail::s_bIsRunning.load();
	}

	void addJob(JobDecl const& job_decl)
	{
		_getJobQueue().pushJob(job_decl);
		//printf("Added job. Total number of waiting jobs = %llu\n", _getJobQueue().m_jobs.size());
	}

	void addJobs(int count, JobDecl job_decls[])
	{
		for (int i = 0; i < count; i++)
		{
			_getJobQueue().pushJob(job_decls[i]);
		}
	}


}
