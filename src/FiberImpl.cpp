#include "FiberImpl.h"

static void trampoline(void(*target)(void*), void* fiber)
{
	target(fiber);
}

namespace Cotton {

	FiberImpl::~FiberImpl()
	{
		delete[] m_StackPtr;
	}

	FiberImpl::FiberImpl(const std::function<void()>& function, size_t stack_size)
		: m_StackPtr((uintptr_t*)(new uint8_t[stack_size])), m_StackSize(stack_size), m_Function(function)
	{
		auto stack_top = (uintptr_t*)((uint8_t*)m_StackPtr + m_StackSize); // Pointing beyond the stack bound
		// -1 -> 8 byte aligned
		// -2 -> 16 byte aligned  <--- will start reading from here
		stack_top[-2] = 0; // Set the return address for trampoline to 0x0
		m_Context.rip = &trampoline;
		m_Context.rsp = &stack_top[-3]; // stack_top - 24
#ifdef _WIN32
		m_Context.rcx = (void*)reinterpret_cast<void(*)(void*)>(&FiberImpl::Run);
		m_Context.rdx = this;
#else
		m_Context.rdi = (void*)reinterpret_cast<void(*)(void*)>(&FiberImpl::Run);
		m_Context.rsi = this;
#endif
	}

	void FiberImpl::SwitchTo(FiberImpl* other)
	{
		swap_context(&m_Context, &other->m_Context);
	}

	void FiberImpl::Run(FiberImpl* fiber)
	{
		fiber->m_Function();
	}
}
