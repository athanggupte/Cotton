#pragma once

namespace Cotton::Literals {

	constexpr size_t operator""_KiB(size_t x)
	{
		return x * 1024;
	}

	constexpr size_t operator""_MiB(size_t x)
	{
		return x * 1024 * 1024;
	}

}