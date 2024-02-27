#include "Fiber.h"

#include <cassert>

#include "ThreadFiberContext.h"


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
		assert((reinterpret_cast<uintptr_t>(fiber.m_pStack) & 0xfull) == 0); // stack must be 16 byte aligned!

		uintptr_t* stack_top = reinterpret_cast<uintptr_t*>(fiber.m_pStack + fiber.m_stackSize); // Pointing beyond the stack bound
		uintptr_t* stack_bot = reinterpret_cast<uintptr_t*>(fiber.m_pStack); // Pointing to the start of the stack
		// -1 -> 8 byte aligned
		// -2 -> 16 byte aligned  <--- will start reading from here
		stack_bot[0] = 0xC01109F1BE4AE00; // Set the stack canary
		stack_top[-1] = 0x00C01109F1BE4AE; // Set the stack canary
		stack_top[-2] = 0x2121212121212121;
		stack_top[-3] = 0x3131313131313131;
		stack_top[-4] = 0; // Set the return address to 0x0
		fiber.m_context.rsp = &stack_top[-5]; // stack_top - 40 bytes

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

	void __checkStackHealth(const Fiber& fiber)
	{
		uintptr_t* stack_top = reinterpret_cast<uintptr_t*>(fiber.m_pStack + fiber.m_stackSize); // Pointing beyond the stack bound
		uintptr_t* stack_bot = reinterpret_cast<uintptr_t*>(fiber.m_pStack); // Pointing to the start of the stack
		assert(stack_bot[0] == 0xC01109F1BE4AE00); // Check the stack canary
		assert(stack_top[-1] == 0x00C01109F1BE4AE); // Check the stack canary
	}

	void switchToFiber(Fiber& fiber)
	{
		assert(&fiber != _getThreadFiberContext()->m_currentRunningFiber);
		Fiber* currentFiber = _getThreadFiberContext()->m_currentRunningFiber;
		_getThreadFiberContext()->m_currentRunningFiber = &fiber;
		swap_context(&currentFiber->m_context, &fiber.m_context);

#if COTTON_ENABLE_STACK_CHECK
		__checkStackHealth(fiber);
#endif
	}

	void yieldFiber()
	{
		assert(_getThreadFiberContext()->m_currentRunningFiber != nullptr);

		switchToFiber(_getThreadFiberContext()->m_jobSchedulerFiber);
	}

}
