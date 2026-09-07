#pragma once

#include "XStorable.h"

#include <cstdint>

namespace NXBasics
{
	class CMemory : public CStorable
	{
	public:
		// Original function: FUN_0042f805
		explicit CMemory(std::uint32_t size);

		// Original function: LAB_0042f9b0
		std::uint32_t Storable_GetId() override;

		// Original function: LAB_0040c775
		std::uint32_t Storable_GetVersion() override;

		// Original function: FUN_0042f95e
		void Storable_SaveData(void* file) override;

		// Original function: FUN_0042f8f4; deleting wrapper: FUN_0042f82f.
		~CMemory() override;

		// Original function: FUN_004e5e48
		static void operator delete(void* allocation) noexcept;

	private:
		// Original function: FUN_0042f984
		void l_AllocateMemory(std::uint32_t size);

		std::uint32_t mSize;       // +0x04
		std::uint8_t* mData;       // +0x08
	};

	static_assert(sizeof(void*) == 4, "CMemory requires an x86 build");
	static_assert(sizeof(CMemory) == 0x0C, "CMemory must be 12 bytes");
}