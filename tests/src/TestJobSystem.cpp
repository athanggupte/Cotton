#include <format>

#include "Fiber.h"
#include "JobSystem.h"
#include <gtest/gtest.h>

#include "ThreadFiberContext.h"

TEST(TestJobSystem, TestFiberBasic)
{
	static char SBUF[1024];

	Cotton::_initializeThreadForFibers(0);

	Cotton::Fiber fiber;
	alignas(16) uint8_t stack[2048];

	fiber.m_pStack = stack;
	fiber.m_stackSize = sizeof(stack);

	auto lambda_fn = +[](void* param)
	{
		//sprintf_s(SBUF, "Inside fiber...\n");
		//printf("Inside fiber...\n");
		// Cotton::switchToFiber(*static_cast<Cotton::Fiber*>(param));
		int a[10]{};
		for (int i = 0; i < 10; i++)
		{
			a[i] = 0xfed21399i32;
		}
	};

	Cotton::JobDecl jobDecl{};
	jobDecl.m_pEntryPoint = lambda_fn;
	jobDecl.m_pParam = nullptr;

	Cotton::initializeFiber(fiber, jobDecl);
	Cotton::switchToFiber(fiber);

	for (int i = 0; i < 20; i++)
	{
		printf("a[%d] = %x\n", i, stack[2048-100-i]);
		//EXPECT_EQ(stack[i], 0xfed21399i32);
	}
}

TEST(TestJobSystem, TestThreadLocalFiberContext)
{
	struct Payload
	{
		int           *array;
		Cotton::Fiber *fiber;
	};

	auto fiber0_fn = [](void* param)
	{
		Payload* payload = static_cast<Payload*>(param);
		// add 1 to even places
		for (int i = 0; i < 10; i += 2)
		{
			payload->array[i] += 1;
			Cotton::switchToFiber(*payload->fiber);
			uint32_t tid = Cotton::_getThreadFiberContext()->m_threadId;
			//printf("[%u] <fiber0_fn>\n", tid);
		}
	};

	auto fiber1_fn = [](void* param)
	{
		Payload* payload = static_cast<Payload*>(param);
		// sub 1 from odd places
		for (int i = 1; i < 10; i += 2)
		{
			payload->array[i] -= 1;
			Cotton::switchToFiber(*payload->fiber);
		}
	};

	auto starter_fn = [fiber0_fn, fiber1_fn](uint32_t tid, int array[])
	{
		Cotton::_initializeThreadForFibers(tid);
		alignas(16) uint8_t stack[2][2048]{};

		Cotton::Fiber fibers[2];
		Payload payloads[2];
		Cotton::JobDecl jobDecls[2];

		fibers[0].m_pStack = stack[0];
		fibers[0].m_stackSize = 2048;

		fibers[1].m_pStack = stack[1];
		fibers[1].m_stackSize = 2048;


		payloads[0].array = array;
		payloads[0].fiber = &fibers[1];

		payloads[1].array = array;
		payloads[1].fiber = &fibers[0];


		jobDecls[0].m_pEntryPoint = fiber0_fn;
		jobDecls[0].m_pParam = &payloads[0];

		jobDecls[1].m_pEntryPoint = fiber1_fn;
		jobDecls[1].m_pParam = &payloads[1];

		Cotton::initializeFiber(fibers[0], jobDecls[0]);
		Cotton::initializeFiber(fibers[1], jobDecls[1]);

		Cotton::switchToFiber(fibers[0]);
	};

	const size_t NUM_THREADS = 8;

	int array[NUM_THREADS][10]{};

	std::vector<std::thread> threadpool;
	threadpool.reserve(NUM_THREADS);

	for (uint32_t i = 0; i < NUM_THREADS; i++)
	{
		threadpool.emplace_back(starter_fn, i, array[i]);
	}

	for (int i = 0; i < NUM_THREADS; i++)
	{
		threadpool[i].join();
	}

	for (int i = 0; i < NUM_THREADS; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			EXPECT_EQ(array[i][j], 1 - (j & 1) * 2);
		}
	}

}

TEST(TestJobSystem, TestPrescheduleJobs)
{
	Cotton::initializeJobSystem();
	Cotton::startJobSystem();

	FILE * fp;
	fopen_s(&fp, "log.txt", "w");

	constexpr static size_t NUM_ELEMS = 100;

	int array[NUM_ELEMS]{0};

	static std::mutex io_mutex;
	struct JobPayload
	{
		int *pArray;
		uint32_t index;
		std::mutex* io_mutex;
		FILE* fp;
	};

	auto lambda = [](void* param)
	{
		JobPayload* payload = static_cast<JobPayload*>(param);

		{
			//std::lock_guard lock(*payload->io_mutex);
			// fprintf(payload->fp, "[job:%d] thread:%s\n", 0, tid_ss.str().c_str());
			//printf("[job:%d] thread:%u\n", 1, Cotton::_getThreadFiberContext()->m_threadId);
		}

		for (uint32_t i = 0; i < 100000u; i++)
		{
			++payload->pArray[payload->index];
		}
	};
	Cotton::PFN_JobEntryPoint entryPoint = lambda;

	JobPayload payloads[NUM_ELEMS]{};

	Cotton::JobDecl jobDecls[NUM_ELEMS];

	for (int i = 0; i < NUM_ELEMS; i++)
	{
		payloads[i].pArray = array;
		payloads[i].index = i;
		payloads[i].io_mutex = &io_mutex;
		payloads[i].fp = fp;

		jobDecls[i].m_pEntryPoint = entryPoint;
		jobDecls[i].m_pParam = &payloads[i];
	}

	Cotton::addJobs(NUM_ELEMS, jobDecls);

	// Cotton::_runWorkerThread();
	Cotton::_waitForAllJobs();


	for (size_t i = 0; i < NUM_ELEMS; i++)
	{
		EXPECT_EQ(array[i], 100000);
	}

}

TEST(TestJobSystem, TestOnTheFlyJobs)
{
	Cotton::initializeJobSystem();
	Cotton::startJobSystem();

	int results[1000]{};

	Cotton::JobDecl mainJob;
	mainJob.m_pEntryPoint = +[](void* param)
	{
		int* results = static_cast<int*>(param);

		Cotton::JobDecl job;
		job.m_pEntryPoint = +[](void* param)
		{
			int * pResult = static_cast<int*>(param);
			*pResult = Cotton::_getThreadFiberContext()->m_threadId;
			//std::this_thread::sleep_for(std::chrono::milliseconds(100));
			for (int i = 0; i < 1000000; i++);
		};

		for (int i = 0; i < 100; i++)
		{
			job.m_pParam = results + i;
			Cotton::addJob(job);
		}

		results[100] = Cotton::_getThreadFiberContext()->m_threadId;
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
		for (int i = 0; i < 1000000; i++);
	};
	mainJob.m_pParam = results;

	Cotton::addJob(mainJob);

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	//printf("Jobs in queue : %llu\n", Cotton::_getJobQueue().m_jobs.size());

	for (int i = 0; i < 101; i++)
	{
		printf("[%d] :: %d\n", i, results[i]);
	}

	Cotton::_waitForAllJobs();
}

DECLARE_FUNCTION(testFunction, int a, int b, int *res)
{
	if (b <= 0)
		return;

	Cotton::addJob(testFunction.job(a + 1, b - 1, res + 1));
	// printf("a:%d, b:%d\n", a, b);
	*res = a + b;
}

TEST(TestJobSystem, TestFunctionDecl)
{
	Cotton::initializeJobSystem();
	Cotton::startJobSystem();

	int results[10]{};

	Cotton::addJob(testFunction.job(0, 10, &results[0]));

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	Cotton::_waitForAllJobs();

	for (int i = 0; i < 10; i++)
	{
		EXPECT_EQ(results[i], 10);
	}
}

