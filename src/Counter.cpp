#include "Counter.h"

namespace Cotton {

	namespace
	{
		Counter g_counters[256];

		std::atomic_uint64_t g_countersUsed{0};
	}

	Counter* allocCounter()
	{
		return &g_counters[g_countersUsed++];
	}

	void freeCounter(Counter* p_counter)
	{
		// delete p_counter;
	}
}
