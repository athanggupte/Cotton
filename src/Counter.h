#pragma once
#include <atomic>
#include <cstdint>

namespace Cotton
{
	struct Counter
	{
		std::atomic_uint64_t m_atomic{0};
	};

	Counter& allocCounter();
	void freeCounter(Counter& counter);
	void incrementCounter(Counter& counter)
	{
		++counter.m_atomic;
	}

	void decrementCounter(Counter& counter)
	{
		--counter.m_atomic;
	}

	uint64_t operator++(Counter& counter)
	{
		return ++counter.m_atomic;
	}

	uint64_t operator++(Counter& counter, int)
	{
		return counter.m_atomic++;
	}

	uint64_t operator--(Counter& counter)
	{
		return --counter.m_atomic;
	}

	uint64_t operator--(Counter& counter, int)
	{
		return counter.m_atomic--;
	}
}
