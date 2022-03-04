#pragma once
#include "Context.h"

#include <cstdint>
#include <functional>
#include <memory>

namespace Cotton {

	constexpr size_t operator""_KiB(size_t x)
	{
		return x * 1024;
	}

	constexpr size_t operator""_MiB(size_t x)
	{
		return x * 1024 * 1024;
	}

	constexpr size_t DEFAULT_STACK_SIZE = 8_KiB;

	class FiberImpl
	{
	public:
		FiberImpl() = default;
		~FiberImpl();

		FiberImpl(const std::function<void()>& function, size_t stack_size);
		void SwitchTo(FiberImpl* other);

		uint32_t id = 0;
	private:
		static void Run(FiberImpl* fiber);

	private:
		Context m_Context{};
		uintptr_t* m_StackPtr = nullptr;
		size_t m_StackSize;
		std::function<void()> m_Function;
	};

}
