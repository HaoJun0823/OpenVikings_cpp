#include "XFileAccess.h"
#include "XMemory.h"

#include <cstring>
#include <new>

namespace NXBasics
{
	// Original function: FUN_0042f805
	CMemory::CMemory(std::uint32_t size) : mSize(0), mData(nullptr)
	{
		l_AllocateMemory(size);
	}

	// Original function: FUN_0042f84b
	CMemory::CMemory(std::uint32_t size, std::uint8_t fillValue) : mSize(0), mData(nullptr)
	{
		l_AllocateMemory(size);
		std::memset(mData, fillValue, mSize);
	}

	// Original function: FUN_0042f8f4
	CMemory::~CMemory()
	{
		std::memset(mData, 0xED, mSize);

		// Release the buffer allocated by l_AllocateMemory().
		::operator delete(mData);
	}

	// Original function: FUN_0042f91a
	CMemory::CMemory(void* file, std::uint32_t /*version*/) : mSize(0), mData(nullptr)
	{
		File_Read(file, &mSize, 4);
		l_AllocateMemory(mSize);
		File_Read(file, mData, mSize);
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

	// Original function: FUN_0042fa37
	// Original function: FUN_00461eff
	void CMemory::Encrypt(std::int32_t parameter)
	{
		std::uint8_t* data = mData;
		const std::uint32_t size = mSize;

		if (size == 0 || parameter != 1)
		{
			return;
		}

		std::uint8_t xorValue = 0x47;
		std::uint8_t increment = 0x7E;

		for (std::uint32_t index = 0; index < size; ++index)
		{
			std::uint8_t value = static_cast<std::uint8_t>(data[index] ^ xorValue);

			xorValue = static_cast<std::uint8_t>(xorValue + increment);
			value = static_cast<std::uint8_t>(value + 1);
			increment = static_cast<std::uint8_t>(increment + 0x21);

			data[index] = value;
		}
	}

	// Original function: FUN_0042fa4c
	// Original function: FUN_00461f37
	void CMemory::Decrypt(std::int32_t parameter)
	{
		std::uint8_t* data = mData;
		const std::uint32_t size = mSize;

		if (size == 0 || parameter != 1)
		{
			return;
		}

		std::uint8_t xorValue = 0x47;
		std::uint8_t increment = 0x7E;

		for (std::uint32_t index = 0; index < size; ++index)
		{
			std::uint8_t value = static_cast<std::uint8_t>(data[index] - 1);
			value = static_cast<std::uint8_t>(value ^ xorValue);

			xorValue = static_cast<std::uint8_t>(xorValue + increment);
			increment = static_cast<std::uint8_t>(increment + 0x21);

			data[index] = value;
		}
	}

	// Original function: LAB_0042f9b0
	std::uint32_t CMemory::Storable_GetId()
	{
		return 0x3E9;
	}

	// Original function: FUN_0042f9b6
	void CMemory::Fill(std::uint8_t value)
	{
		std::memset(mData, value, mSize);
	}

	// Original function: FUN_0042f9cd
	bool CMemory::ChangeBufferSize(std::uint32_t newSize, bool fillAdditionalBytes, std::uint8_t fillValue)
	{
		if (mSize == newSize)
		{
			return true;
		}

		std::uint8_t* oldData = mData;

		mData = static_cast<std::uint8_t*>(::operator new(newSize, std::nothrow));

		if (fillAdditionalBytes && newSize > mSize)
		{
			std::memset(mData + mSize, fillValue, newSize - mSize);
		}

		const std::uint32_t copySize = mSize < newSize ? mSize : newSize;

		std::memcpy(mData, oldData, copySize);
		::operator delete(oldData);

		mSize = newSize;

		return true;
	}

	// Original function: LAB_0040c775
	std::uint32_t CMemory::Storable_GetVersion()
	{
		return 0;
	}

	// Original function: FUN_0042f95e
	void CMemory::Storable_SaveData(void* file)
	{
		File_Write(file, &mSize, 0x04);

		// Reload the size and data pointer after writing the size field.
		const std::uint32_t size = mSize;
		const auto* data = mData;

		File_Write(file, data, size);
	}

	// Original function: FUN_004e5e48
	void CMemory::operator delete(void* allocation) noexcept
	{
		::operator delete(allocation);
	}
}