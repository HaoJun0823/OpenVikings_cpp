#pragma once

#include <cstddef>
#include <cstdint>

namespace NDMManager
{
	inline constexpr std::size_t ManagerSize = 0x224;
	void* Construct(void* manager);
	void Destruct(void* manager);
	// Remaster: game-thread initialization/command results mean request accepted;
	// driver failures are reported asynchronously to the debugger output.
	std::uint8_t Init(void* manager, std::int32_t mode, const char* directory);
	std::uint8_t Exit(void* manager);
	std::uint8_t Update(void* manager);
	// Remaster: includes queued playback requests while the music thread starts.
	bool HasPendingOrActiveEvent(const void* manager);
	std::int32_t MainVolume_Get(const void* manager);
	std::uint8_t MainVolume_Set(void* manager, std::int32_t volume);
	std::uint8_t ProcessEvent(void* manager, std::int32_t eventId, std::uint32_t eventParameter, std::int32_t mode);
	void ResetAllSegments(void* manager);
	std::uint8_t PrepareEvent(void* manager, std::int32_t eventId, std::uint32_t eventParameter);
	std::uint8_t StartEvent(void* manager, std::int32_t eventId, std::uint32_t eventParameter);
	const char* GetEventFileName(std::int32_t eventId, std::uint32_t variant);
}
