#include "Counter.h"

#include <memory>
#include <cassert>

namespace Cotton {

	template <typename T, typename SizeType = uint32_t>
	class FreeList
	{
	public:
		union FreeListElement
		{
			SizeType m_next;
			T		 m_data;
		};

		using element_type = FreeListElement;
		using value_type = T;

	public:
		FreeList() = default;

		FreeList(SizeType capacity)
			: m_capacity(capacity)
		{
			m_pElements = new element_type[capacity]{0};
			for (SizeType i = 0; i < capacity - 1; ++i)
			{
				m_pElements[i].m_next = i + 1;
			}
		}

		template <typename... Args>
		value_type& emplace_back(Args&&... args)
		{
			assert(m_size < m_capacity);

			SizeType index = m_freeListHead;
			m_freeListHead = m_pElements[index].m_next;

			std::construct_at(&m_pElements[index].m_data, std::forward<Args>(args)...);
			++m_size;

			return m_pElements[index].m_data;
		}

		void erase(value_type& value)
		{
			SizeType index = reinterpret_cast<ptrdiff_t>(std::addressof(value)) - reinterpret_cast<ptrdiff_t>(m_pElements);
			index /= sizeof(element_type);
			erase(index);
		}

		void erase(SizeType index)
		{
			assert(index < m_capacity);

			std::destroy_at(&m_pElements[index].m_data);

			m_pElements[index].m_next = m_freeListHead;
			m_freeListHead = index;
			--m_size;
		}

	private:
		element_type* m_pElements {nullptr};
		SizeType m_capacity {0};
		SizeType m_size {0};
		SizeType m_freeListHead {0};
	};

	namespace
	{
		FreeList<Counter> g_counters(256);
	}

	Counter& allocCounter()
	{
		return g_counters.emplace_back();
	}

	void freeCounter(Counter& counter)
	{
		g_counters.erase(counter);
	}
}
