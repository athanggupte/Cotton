#pragma once
#include <cstdint>

#include "impl/Context.h"

#define COTTON_ENABLE_STACK_CHECK 1

namespace Cotton {
	struct JobDecl;

	struct Fiber
	{
		Context m_context{};
		uint8_t* m_pStack{nullptr};
		size_t m_stackSize{0};
	};

	void initializeFiber(Fiber& fiber, const JobDecl& job);
	void switchToFiber(Fiber& fiber);
	void yieldFiber();
}
