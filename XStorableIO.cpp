#include "XFile.h"
#include "XStorableIO.h"
#include "XStringArray.h"

#include <cstdint>
#include <new>

namespace NXBasics
{
	// Original function: FUN_00410715
	void* XB_Storable_LoadObject(void* file)
	{
		std::uint32_t objectId;
		std::uint32_t version;

		File_Read(file, &objectId, 4);
		File_Read(file, &version, 4);

		switch (objectId)
		{
		case 0x3E9:
			return new (std::nothrow) CMemory(file, version);

		case 0x3F3:
		{
			void* allocation = ::operator new(0x54, std::nothrow);

			if (allocation == nullptr)
			{
				return nullptr;
			}

			return FUN_0040c6dc(allocation, file, version);
		}

		case 0x3F4:
		{
			void* allocation = ::operator new(0x3C, std::nothrow);

			if (allocation == nullptr)
			{
				return nullptr;
			}

			return FUN_0043997d(allocation, file, version);
		}

		case 0x3F5:
		{
			void* allocation = ::operator new(0x18, std::nothrow);

			if (allocation == nullptr)
			{
				return nullptr;
			}

			return FUN_004397ea(allocation, file, version);
		}

		case 0x3F6:
		{
			void* allocation = ::operator new(0x40C, std::nothrow);

			if (allocation == nullptr)
			{
				return nullptr;
			}

			return FUN_0040fa43(allocation, file, version);
		}

		case 0x3F7:
		{
			void* allocation = ::operator new(0x104, std::nothrow);

			if (allocation == nullptr)
			{
				return nullptr;
			}

			return FUN_0040fde0(allocation, file, version);
		}

		case 0x3FD:
			return new (std::nothrow) CStringArray(file, version);

		default:
			FUN_00406935("NXBasics::XB_Storable_LoadObject  cannot load unknown object - class id %d", objectId);
			return nullptr;
		}
	}

	// Original function: FUN_00410876
	void XB_Storable_GetObjectIdAndVersion(void* file, std::uint32_t* objectId, std::uint32_t* version)
	{
		if (File_GetSize(file) < 8)
		{
			*objectId = 0;
			*version = 0;
			return;
		}

		const std::uint32_t originalPosition = File_GetPosition(file);

		File_Read(file, objectId, 4);
		File_Read(file, version, 4);

		File_SeekToPosition(file, originalPosition);
	}

	// Original function: FUN_0041083d
	void* XB_Storable_LoadObject(const char* fileName)
	{
		CFile file(fileName, true);

		return XB_Storable_LoadObject(static_cast<void*>(&file));
	}

	// Original function: FUN_004108c5
	void XB_Storable_GetObjectIdAndVersion(const char* fileName, std::uint32_t* objectId, std::uint32_t* version)
	{
		CFile file(fileName, true);

		XB_Storable_GetObjectIdAndVersion(static_cast<void*>(&file), objectId, version);
	}
}