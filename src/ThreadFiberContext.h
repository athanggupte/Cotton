#pragma once
#include "JobSystem.h"

namespace Cotton {

	struct ThreadFiberContext
	{
		constexpr static size_t STACK_SIZE = 4ui64 * 1024;

		Fiber    m_jobSchedulerFiber{};
		Fiber*   m_currentRunningFiber{nullptr};
		alignas(16)
		uint8_t  m_aStack[STACK_SIZE]{};
		bool     m_bIsRunning{false};
		bool     m_bShouldExit{false};
		Metrics  m_metrics{};
		uint32_t m_threadId{};

		ThreadFiberContext()
		{
			m_jobSchedulerFiber.m_pStack = m_aStack;
			m_jobSchedulerFiber.m_stackSize = STACK_SIZE;
		}

		void submitMetrics();
	};

	ThreadFiberContext* _getThreadFiberContext();

}
