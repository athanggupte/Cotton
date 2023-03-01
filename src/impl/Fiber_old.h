#pragma once
#include "Context.h"
#include "Literals.h"

#include <cstdint>
#include <functional>
#include <memory>

namespace Cotton {

	using namespace Literals;

	constexpr size_t DEFAULT_STACK_SIZE = 8_KiB;

	class Fiber_old
	{
	public:
		Fiber_old() = default;
		Fiber_old(const std::function<void()>& function, size_t stack_size);
		~Fiber_old();

		void SwitchTo(Fiber_old* other);
		void Yield();

		uint32_t id = 0;

		bool IsAlive() const
		{
			return !m_IsFinished;
		}

	private:
		static void Run(Fiber_old* fiber);

	private:
		Context m_Context{};
		uintptr_t* m_StackPtr = nullptr;
		size_t m_StackSize = 0;
		std::function<void()> m_Function;

		bool m_IsFinished = false;
		Fiber_old* m_Caller = nullptr;

		friend class Scheduler;
	};


}
