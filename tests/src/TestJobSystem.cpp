#include <gtest/gtest.h>
#include "Fiber.h"
#include "JobSystem.h"

TEST(TestJobSystem, TestFiberBasic)
{
	Cotton::_initializeThreadForFibers();

	Cotton::Fiber fiber;
	alignas(16) uint8_t stack[2048];

	fiber.m_pStack = stack;
	fiber.m_stackSize = 2048;

	auto lambda_fn = [](void* param)
	{
		printf("Inside fiber...\n");
		//Cotton::switchToFiber(*static_cast<Cotton::Fiber*>(param));
	};

	Cotton::JobDecl jobDecl{};
	jobDecl.m_pEntryPoint = lambda_fn;
	jobDecl.m_pParam = nullptr;

	Cotton::initializeFiber(fiber, jobDecl);
	Cotton::switchToFiber(fiber);

	printf("Back to main fiber!\n");
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

	auto starter_fn = [fiber0_fn, fiber1_fn](int array[])
	{
		Cotton::_initializeThreadForFibers();
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

	for (int i = 0; i < NUM_THREADS; i++)
	{
		threadpool.emplace_back(starter_fn, array[i]);
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

	constexpr static size_t NUM_ELEMS = 1000;

	int array[NUM_ELEMS]{0};

	struct JobPayload
	{
		int *pArray;
		uint32_t index;
	};

	auto lambda = [](void* param)
	{
		JobPayload* payload = static_cast<JobPayload*>(param);

		for (uint32_t i = 0; i < 100000; i++)
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

		jobDecls[i].m_pEntryPoint = entryPoint;
		jobDecls[i].m_pParam = &payloads[i];
	}

	Cotton::runJobs(NUM_ELEMS, jobDecls);

	// Cotton::_runWorkerThread();
	Cotton::_waitForAllJobs();


	for (size_t i = 0; i < NUM_ELEMS; i++)
	{
		EXPECT_EQ(array[i], 100000);
	}

}
