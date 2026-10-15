#include "C2E2StaticDataGroups.h"
#include "XCrtFileAccess.h"

#include <cstddef>
#include <cstring>

namespace NC2E2
{
	// Original function: FUN_0048e5c5
	std::int32_t StaticDataGroups_FindIndex(const void* groupData, const char* name)
	{
		const auto* storage = static_cast<const std::byte*>(groupData);
		std::int32_t count;
		std::memcpy(&count, storage, sizeof(count));

		if (count <= 0)
			return -1;

		std::uint32_t entryAddress = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(groupData)) + 4u;

		for (std::int32_t index = 0; index < count; ++index)
		{
			const auto* entryName = reinterpret_cast<const char*>(static_cast<std::uintptr_t>(entryAddress));

			if (NXBasics::Runtime_CompareStringsIgnoreCase(name, entryName) == 0)
				return index;

			entryAddress += 0x28u;
			std::memcpy(&count, storage, sizeof(count));
		}

		return -1;
	}

	// Original function: FUN_0048e5f8
	std::int32_t StaticDataGroups_AddName(void* groupData, const char* name)
	{
		const std::int32_t existingIndex = StaticDataGroups_FindIndex(groupData, name);

		if (existingIndex != -1)
			return existingIndex;

		std::uint32_t count;
		std::memcpy(&count, groupData, sizeof(count));

		if (count == 100u)
			return -1;

		const std::uint32_t entryAddress = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(groupData)) + 4u + count * 0x28u;
		auto* destination = reinterpret_cast<char*>(static_cast<std::uintptr_t>(entryAddress));

		std::strncpy(destination, name, 39u);

		std::memcpy(&count, groupData, sizeof(count));
		++count;
		std::memcpy(groupData, &count, sizeof(count));

		const std::uint32_t indexBits = count - 1u;
		std::int32_t index;
		std::memcpy(&index, &indexBits, sizeof(index));
		return index;
	}
}