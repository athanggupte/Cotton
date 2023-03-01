#include "Fiber.h"
#include "JobSystem.h"

#include <cassert>
#include <deque>
#include <mutex>
#include <vector>

#include "JobSystem-internal.h"

namespace Cotton {

	static void trampoline(PFN_JobEntryPoint entry_point, void* param)
	{
		entry_point(param);
		// The job's main function body has returned so the context need not be saved
		// Directly jump to the scheduler fiber
		set_context(&_getThreadFiberContext()->m_jobSchedulerFiber.m_context);
	}

	void initializeFiber(Fiber& fiber, const JobDecl& job)
	{
		assert((reinterpret_cast<uintptr_t>(fiber.m_pStack) & 15ui64) == 0); // stack must be 16 byte aligned!

		uintptr_t* stack_top = reinterpret_cast<uintptr_t*>(fiber.m_pStack + fiber.m_stackSize); // Pointing beyond the stack bound
		// -1 -> 8 byte aligned
		// -2 -> 16 byte aligned  <--- will start reading from here
		stack_top[-2] = 0; // Set the return address to 0x0
		fiber.m_context.rsp = &stack_top[-3]; // stack_top - 24

		// Set the Instruction Pointer (RIP) to the job's entry point
		fiber.m_context.rip = reinterpret_cast<void*>(trampoline);

#ifdef _WIN32
		fiber.m_context.rcx = reinterpret_cast<void*>(job.m_pEntryPoint);
		fiber.m_context.rdx = static_cast<void*>(job.m_pParam);
#else
		fiber.m_context.rdi = reinterpret_cast<void*>(job.m_pEntryPoint);
		fiber.m_context.rsi = static_cast<void*>(job.m_param);
#endif
	}

	void switchToFiber(Fiber& fiber)
	{
		assert(&fiber != _getThreadFiberContext()->m_currentRunningFiber);
		Fiber* currentFiber = _getThreadFiberContext()->m_currentRunningFiber;
		_getThreadFiberContext()->m_currentRunningFiber = &fiber;
		swap_context(&currentFiber->m_context, &fiber.m_context);
	}

	void yieldFiber()
	{
		assert(_getThreadFiberContext()->m_currentRunningFiber != nullptr);

		switchToFiber(_getThreadFiberContext()->m_jobSchedulerFiber);
	}

	struct JobQueue
	{
		std::deque<JobDecl*> m_jobs;
		std::mutex m_mutex;
		std::condition_variable m_cvar;

		JobDecl* popJob()
		{
			std::unique_lock lock(m_mutex);
			while (m_jobs.empty())
			{
				m_cvar.wait_for(lock, std::chrono::microseconds(25));
			}

			JobDecl* jobDecl = m_jobs.front();
			m_jobs.pop_front();
			return jobDecl;
		}

		void pushJob(JobDecl* job_decl)
		{
			std::unique_lock lock(m_mutex);
			m_jobs.push_back(job_decl);
			m_cvar.notify_one();
		}
	};

	struct FiberRef
	{
		uint32_t m_fiberId;
	};

	struct FiberList
	{
		std::vector<Fiber> m_fibers;
		std::deque<uint32_t> m_unusedIndices;

		std::mutex m_mutex;
		std::condition_variable m_cvar;

		void initialize(uint32_t size)
		{
			m_fibers.resize(size);

			for (uint32_t i = 0; i < size; i++)
			{
				m_unusedIndices.push_back(i);
			}
		}

		FiberRef getFiber()
		{
			std::unique_lock lock(m_mutex);
			while (m_unusedIndices.empty())
			{
				m_cvar.wait_for(lock, std::chrono::microseconds(100));
			}

			uint32_t fiberIdx = m_unusedIndices.front();
			m_unusedIndices.pop_front();
			return { fiberIdx };
		}

		void returnFiber(FiberRef fiber)
		{
			std::unique_lock lock(m_mutex);
			m_unusedIndices.push_back(fiber.m_fiberId);
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

		std::vector<std::thread> s_threads;

		std::atomic_bool s_bIsRunning{false};
	};

	inline JobQueue& _getJobQueue()
	{
		return detail::s_jobQueue;
	}

	inline FiberList& _getFiberList()
	{
		return detail::s_fiberList;
	}

	inline FiberWaitList& _getFiberWaitList()
	{
		return detail::s_waitList;
	}

	inline Fiber& fromFiberRef(FiberRef ref)
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

	void _initializeThreadForFibers()
	{
		Cotton::ThreadFiberContext* context = Cotton::_getThreadFiberContext();
		Cotton::Fiber& mainFiber = context->m_jobSchedulerFiber;
		context->m_currentRunningFiber = &mainFiber;
		context->m_bIsRunning = true;

		//get_context(&mainFiber.m_context);
	}

	void _shutdownWorkerThread(void*)
	{
		_getThreadFiberContext()->m_bIsRunning = false;
	}

	void _runWorkerThread()
	{
		_initializeThreadForFibers();

		while (_getThreadFiberContext()->m_bIsRunning) {
		//     Check if there is a waiting Fiber in the wait list that is ready to run, i.e. its wait counter is 0
		//         switch to the waiting Fiber if found
		//     Grab a JobDecl from the queue
		//     Pull a Fiber from the set of available fibers
		//         initialize the Fiber with the JobDecl
		//         switch to the Fiber
			JobDecl* jobDecl = _getJobQueue().popJob();
			if (jobDecl != nullptr)
			{
				FiberRef fiberRef = _getFiberList().getFiber();
				{
					Fiber& fiber = fromFiberRef(fiberRef);

					initializeFiber(fiber, *jobDecl);
					switchToFiber(fiber);
				}

				_getFiberList().returnFiber(fiberRef);
			}
			else
			{
				
			}
		}
	}

	void startJobSystem()
	{
		uint32_t numThreads = std::min(std::thread::hardware_concurrency(), 8u);

		detail::s_threads.reserve(numThreads);

		for (uint32_t i = 0; i < numThreads; i++)
		{
			detail::s_threads.emplace_back(_runWorkerThread);
		}
	}

	void _waitForAllJobs()
	{
		JobDecl jobDecl;
		jobDecl.m_pEntryPoint = _shutdownWorkerThread;

		for (uint32_t i = 0; i < detail::s_threads.size(); i++)
		{
			_getJobQueue().pushJob(&jobDecl);
		}

		for (uint32_t i = 0; i < detail::s_threads.size(); i++)
		{
			detail::s_threads[i].join();
		}
	}

	void runJob(JobDecl& job_decl)
	{
		_getJobQueue().pushJob(&job_decl);
	}

	void runJobs(int count, JobDecl job_decls[])
	{
		for (int i = 0; i < count; i++)
		{
			_getJobQueue().pushJob(&job_decls[i]);
		}
	}


}
