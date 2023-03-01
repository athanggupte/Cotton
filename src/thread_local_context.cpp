#include "Fiber.h"
#include "JobSystem-internal.h"

namespace Cotton {

	thread_local ThreadFiberContext s_threadFiberContext{};

	ThreadFiberContext* _getThreadFiberContext()
	{
		return &s_threadFiberContext;
	}

}
