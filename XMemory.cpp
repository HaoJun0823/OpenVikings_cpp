#include "XMemory.h"

#include <cstring>
#include <new>

// Original allocation-release wrapper.
extern void FUN_004e5e48(void* allocation);

namespace NXBasics
{
	// Original function: FUN_0042f805
	CMemory::CMemory(std::uint32_t size) : mSize(0), mData(nullptr)
	{
		l_AllocateMemory(size);
	}

	// Original function: FUN_0042f8f4
	CMemory::~CMemory()
	{
		std::memset(mData, 0xED, mSize);

		// Reload the data pointer after overwriting the buffer.
		FUN_004e5e48(mData);
	}

	// Original function: FUN_0042f984
	void CMemory::l_AllocateMemory(std::uint32_t size)
	{
		mSize = size;

		if (size != 0)
		{
			auto* allocation = static_cast<std::uint8_t*>(::operator new(size, std::nothrow));
			const std::uint32_t fillSize = mSize;

			mData = allocation;
			std::memset(allocation, 0xEE, fillSize);
		}
	}

	// Original function: LAB_0042f9b0
	std::uint32_t CMemory::Storable_GetId()
	{
		return 0x3E9;
	}

	// Original function: LAB_0040c775
	std::uint32_t CMemory::Storable_GetVersion()
	{
		return 0;
	}

	// Original function: FUN_0042f95e
	void CMemory::Storable_SaveData(void* file)
	{
		FUN_004061bc(file, &mSize, 0x04);

		// Reload the size and data pointer after writing the size field.
		const std::uint32_t size = mSize;
		const auto* data = mData;

		FUN_004061bc(file, data, size);
	}

	// Original function: FUN_004e5e48
	void CMemory::operator delete(void* allocation) noexcept
	{
		FUN_004e5e48(allocation);
	}
}