#include "Fiber.h"
#include "ThreadFiberContext.h"

#include <mutex>

#include "JobSystem.h"

namespace Cotton {

	thread_local ThreadFiberContext s_threadFiberContext{};

	void ThreadFiberContext::submitMetrics()
	{
		auto& metrics = _getMetrics();
		memcpy(&metrics, &m_metrics, sizeof(Metrics));
	}

	ThreadFiberContext* _getThreadFiberContext()
	{
		return &s_threadFiberContext;
	}

}
