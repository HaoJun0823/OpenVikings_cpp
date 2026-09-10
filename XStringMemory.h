#pragma once

#include <cstring>
#include <new>

namespace NXBasics
{
	// Original function: FUN_00410b59
	inline char* StringDuplicate(const char* source)
	{
		if (source == nullptr)
		{
			return nullptr;
		}

		const std::size_t byteCount = std::strlen(source) + 1;
		auto* copy = static_cast<char*>(::operator new(byteCount, std::nothrow));

		// The original copies without checking allocation success.
		std::memcpy(copy, source, byteCount);
		return copy;
	}
}