#include "C2GameState.h"
#include "C2E2AmbientSoundManagerAccess.h"
#include "C2E2StaticDataSoundFX.h"
#include "C2EngineDisplayAccess.h"
#include "C2GameMapAccess.h"
#include "C2MapSectorGridCursor.h"
#include "DSManager.h"
#include "XCrtFileAccess.h"

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstring>


namespace NC2E2
{
    template<class T> static T Ambient_ReadField(const void* object, std::size_t offset)
    {
        T value;
        std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
        return value;
    }

    static std::byte* Ambient_AddressAt(void* object, std::uint32_t offset)
    {
        const std::uint32_t address = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(object)) + offset;
        return reinterpret_cast<std::byte*>(static_cast<std::uintptr_t>(address));
    }

    struct SAmbientLoopEntry
    {
        std::uint32_t ambientId;
        std::uint32_t volume;
    };

    struct SSoundLoopIds
    {
        std::int32_t count;
        SAmbientLoopEntry entries[10];
    };

    static_assert(sizeof(SSoundLoopIds) == 0x54);

    static bool Ambient_SoundDeviceIsReady()
    {
        return NDSManager::CManager::sTheObjectPtr != nullptr && Ambient_ReadField<std::uint8_t>(NDSManager::CManager::sTheObjectPtr, 4) != 0;
    }

    static void* Ambient_GetStaticRecord(std::uint32_t ambientId)
    {
        return Ambient_AddressAt(CE2StaticDataSoundFXManager::sTheObjectPtr->ambientSounds, ambientId * 0x1570u);
    }

    // Original function: FUN_0047f947; CE2AmbientSoundManager::l_UpdateLoopList.
    static void AmbientSoundManager_UpdateLoopList(void* manager, const SSoundLoopIds& requested)
    {
        if (!Ambient_SoundDeviceIsReady())
            return;

        auto* previous = reinterpret_cast<SSoundLoopIds*>(Ambient_AddressAt(manager, 0x0C));
        for (std::int32_t index = 0; index < requested.count; ++index)
        {
            const auto& entry = requested.entries[index];
            std::int32_t previousIndex = 0;
            while (previousIndex < previous->count && previous->entries[previousIndex].ambientId != entry.ambientId)
                ++previousIndex;

            if (previousIndex == previous->count || (previousIndex < previous->count && previous->entries[previousIndex].volume != entry.volume))
            {
                const std::uint32_t waveId = Ambient_ReadField<std::uint32_t>(Ambient_GetStaticRecord(entry.ambientId), 0xA8);
                if (NDSManager::CManager::sTheObjectPtr->PreloadWave_IsRunning(waveId))
                    NDSManager::CManager::sTheObjectPtr->PreloadWave_ChangeVolume(waveId, entry.volume, true, 1000);
                else
                    NDSManager::CManager::sTheObjectPtr->PreloadWave_Play(waveId, entry.volume, 0xFFFFFFFFu, 64, true, 1000);
            }

            if (previousIndex < previous->count)
                previous->entries[previousIndex].ambientId = 0xFFFFFFFFu;
        }

        for (std::int32_t index = 0; index < previous->count; ++index)
        {
            if (previous->entries[index].ambientId == 0xFFFFFFFFu)
                continue;
            const std::uint32_t waveId = Ambient_ReadField<std::uint32_t>(Ambient_GetStaticRecord(previous->entries[index].ambientId), 0xA8);
            if (NDSManager::CManager::sTheObjectPtr->PreloadWave_IsRunning(waveId))
                NDSManager::CManager::sTheObjectPtr->PreloadWave_ChangeVolume(waveId, 0, true, 1000);
        }
        std::memcpy(previous, &requested, sizeof(requested));
    }

    // Original predicate: FUN_004585a4, SSectorGridCursor::IsInsideMap.
    static bool Ambient_SectorIsInsideMap(std::int32_t x, std::int32_t y)
    {
        return x < NC2Logic::SSectorGridCursor::sGridWidth && x >= 0 && y < NC2Logic::SSectorGridCursor::sGridHeight && y >= 0;
    }

    static std::uint32_t Ambient_GetSectorCenter(std::int32_t x, std::int32_t y)
    {
        std::uint16_t pointX = static_cast<std::uint16_t>(static_cast<std::uint32_t>(x) * 20u + 10u);
        std::uint16_t pointY = static_cast<std::uint16_t>(static_cast<std::uint32_t>(y) * 20u + 10u);
        std::int16_t signedX;
        std::int16_t signedY;
        std::memcpy(&signedX, &pointX, sizeof(signedX));
        std::memcpy(&signedY, &pointY, sizeof(signedY));
        // 005125f8/005125fa mirror the map dimensions during map initialization.
        // The current reconstruction keeps their storage in the game-map block.
        if (signedX >= NC2Logic::GameMap_GetWidth())
            pointX = static_cast<std::uint16_t>(static_cast<std::uint32_t>(NC2Logic::GameMap_GetWidth()) - 1u);
        if (signedY >= NC2Logic::GameMap_GetHeight())
            pointY = static_cast<std::uint16_t>(static_cast<std::uint32_t>(NC2Logic::GameMap_GetHeight()) - 1u);
        return pointX | (static_cast<std::uint32_t>(pointY) << 16);
    }

    static bool Ambient_SectorIsVisible(const void* display, std::uint32_t point)
    {
        const std::uint32_t player = Ambient_ReadField<std::uint32_t>(display, 0x7C);
        if ((Ambient_ReadField<std::uint8_t>(display, 0x70) & 4u) == 0 || player >= 16u)
            return true;
        const void* microPoint = NC2Logic::GameMap_GetConstMapMicroPointPtr(&point);
        return (Ambient_ReadField<std::uint16_t>(microPoint, 8) & (1u << player)) != 0;
    }

    static void* Ambient_GetSector(void* manager, std::int32_t x, std::int32_t y)
    {
        const std::uint32_t index = Ambient_ReadField<std::uint32_t>(manager, 0) * static_cast<std::uint32_t>(y) + static_cast<std::uint32_t>(x);
        return Ambient_AddressAt(Ambient_ReadField<void*>(manager, 8), index * 0x38u);
    }

    // Original function: FUN_0047fdca; CE2AmbientSoundManager::l_UpdateAmbientLoops.
    static void AmbientSoundManager_UpdateAmbientLoops(void* manager, const NC2Logic::SSectorGridCursor& first, const NC2Logic::SSectorGridCursor& last, const void* display)
    {
        SSoundLoopIds requested;
        requested.count = 0;
        for (std::int32_t slot = 0; slot < 3; ++slot)
        {
            for (std::int32_t y = first.y; y <= last.y; ++y)
            {
                for (std::int32_t x = first.x; x <= last.x; ++x)
                {
                    if (!Ambient_SectorIsInsideMap(x, y) || !Ambient_SectorIsVisible(display, Ambient_GetSectorCenter(x, y)))
                        continue;

                    const void* sector = Ambient_GetSector(manager, x, y);
                    if (slot >= Ambient_ReadField<std::int32_t>(sector, 0))
                        continue;
                    const std::uint32_t ambientId = Ambient_ReadField<std::uint32_t>(sector, 4u + static_cast<std::uint32_t>(slot) * 8u);
                    if (ambientId == 0xFFFFFFFFu)
                        continue;
                    const std::uint32_t volume = Ambient_ReadField<std::uint32_t>(sector, 8u + static_cast<std::uint32_t>(slot) * 8u);
                    std::int32_t index = 0;
                    while (index < requested.count && requested.entries[index].ambientId != ambientId)
                        ++index;
                    if (index < requested.count)
                        requested.entries[index].volume += volume;
                    else if (requested.count < 10)
                        requested.entries[requested.count++] = { ambientId, volume };
                }
            }
        }
        // The Windows DIV uses the number of distinct loop IDs, not sector count.
        for (std::int32_t index = 0; index < requested.count; ++index)
            requested.entries[index].volume /= static_cast<std::uint32_t>(requested.count);
        AmbientSoundManager_UpdateLoopList(manager, requested);
    }

    // Original function: FUN_00480137; weighted selection within a static ambient record.
    static const void* Ambient_SelectLandscapeSound(const void* record)
    {
        std::uint32_t remaining = static_cast<std::uint32_t>(NXBasics::Runtime_Rand()) % Ambient_ReadField<std::uint32_t>(record, 0x28);
        std::uint32_t index = 0;
        for (;; ++index)
        {
            remaining -= Ambient_ReadField<std::uint32_t>(record, 0xB0u + index * 0x88u);
            if ((remaining & 0x80000000u) != 0)
                return static_cast<const std::byte*>(record) + 0x30u + index * 0x88u;
        }
    }

    // Original function: FUN_0047fb9d; CE2AmbientSoundManager::l_PlayLandscapeAmbientSounds.
    static void AmbientSoundManager_PlayLandscapeAmbientSounds(void* manager, const NC2Logic::SSectorGridCursor& first, const NC2Logic::SSectorGridCursor& last, const std::int16_t* rectangle, const void* display)
    {
        if (!Ambient_SoundDeviceIsReady() || Ambient_ReadField<std::uint32_t>(manager, 0x60) == DAT_00510838)
            return;

        const std::int32_t centerX = rectangle[0] + (static_cast<std::int32_t>(rectangle[2]) >> 1);
        const std::int32_t centerY = rectangle[1] + (static_cast<std::int32_t>(rectangle[3]) >> 1);
        for (std::int32_t y = first.y; y <= last.y; ++y)
        {
            for (std::int32_t x = first.x; x <= last.x; ++x)
            {
                if (!Ambient_SectorIsInsideMap(x, y))
                    continue;
                const std::uint32_t point = Ambient_GetSectorCenter(x, y);
                if (!Ambient_SectorIsVisible(display, point))
                    continue;
                const void* sector = Ambient_GetSector(manager, x, y);
                for (std::int32_t index = 0; index < Ambient_ReadField<std::int32_t>(sector, 0x1C); ++index)
                {
                    const std::uint32_t offset = static_cast<std::uint32_t>(index) * 8u;
                    const void* record = Ambient_GetStaticRecord(Ambient_ReadField<std::uint32_t>(sector, 0x20u + offset));
                    if (Ambient_ReadField<std::uint32_t>(record, 0x2C) == 0)
                        continue;
                    const void* sound = Ambient_SelectLandscapeSound(record);
                    const std::uint32_t chance = static_cast<std::uint32_t>(NXBasics::Runtime_Rand() % 10000);
                    const std::uint32_t threshold = Ambient_ReadField<std::uint32_t>(sector, 0x24u + offset) * Ambient_ReadField<std::uint32_t>(sound, 0x84);
                    if (chance >= threshold)
                        continue;

                    const std::int32_t pointX = Ambient_ReadField<std::int16_t>(&point, 0);
                    const std::int32_t pointY = Ambient_ReadField<std::int16_t>(&point, 2);
                    const std::int32_t horizontal = NXBasics::Runtime_TimeSigned32((static_cast<std::uint32_t>(pointX) - static_cast<std::uint32_t>(centerX)) << 7) / rectangle[2];
                    const std::int32_t vertical = NXBasics::Runtime_TimeSigned32((static_cast<std::uint32_t>(pointY) - static_cast<std::uint32_t>(centerY)) << 7) / rectangle[3];
                    const std::uint32_t absoluteX = horizontal < 0 ? 0u - static_cast<std::uint32_t>(horizontal) : static_cast<std::uint32_t>(horizontal);
                    const std::uint32_t absoluteY = vertical < 0 ? 0u - static_cast<std::uint32_t>(vertical) : static_cast<std::uint32_t>(vertical);
                    const std::int32_t distance = NXBasics::Runtime_TimeSigned32(absoluteX + absoluteY) / 2;
                    if (distance > 64)
                        continue;
                    const std::uint32_t volume = ((64u - static_cast<std::uint32_t>(distance)) * Ambient_ReadField<std::uint32_t>(sound, 0x7C)) >> 6;
                    NDSManager::CManager::sTheObjectPtr->PreloadWave_Play(Ambient_ReadField<std::uint32_t>(sound, 0x78), volume, 1, static_cast<std::uint32_t>(horizontal) + 64u, false, 0);
                }
            }
        }
        std::memcpy(static_cast<std::byte*>(manager) + 0x60, &DAT_00510838, sizeof(DAT_00510838));
    }

    // Original function: FUN_0047fae2; CE2AmbientSoundManager::UpdateAmbientSounds.
    void AmbientSoundManager_UpdateAmbientSounds(void* manager, const void* display)
    {
        if (!Ambient_SoundDeviceIsReady())
            return;

        std::int16_t rectangle[4]{};
        DisplayDescription_GetMapRectangle(display, Ambient_ReadField<std::uint32_t>(display, 0x954), Ambient_ReadField<std::uint32_t>(display, 0x958), Ambient_ReadField<std::uint32_t>(display, 0x964), Ambient_ReadField<std::uint32_t>(display, 0x968), rectangle);
        const std::uint32_t firstPoint = static_cast<std::uint16_t>(rectangle[0]) | (static_cast<std::uint32_t>(static_cast<std::uint16_t>(rectangle[1])) << 16);
        const std::uint16_t lastX = static_cast<std::uint16_t>(static_cast<std::int32_t>(rectangle[0]) + rectangle[2] - 1);
        const std::uint16_t lastY = static_cast<std::uint16_t>(static_cast<std::int32_t>(rectangle[1]) + rectangle[3] - 1);
        NC2Logic::SSectorGridCursor first(firstPoint);
        NC2Logic::SSectorGridCursor last(lastX | (static_cast<std::uint32_t>(lastY) << 16));
        AmbientSoundManager_UpdateAmbientLoops(manager, first, last, display);
        AmbientSoundManager_PlayLandscapeAmbientSounds(manager, first, last, rectangle, display);
    }
}
