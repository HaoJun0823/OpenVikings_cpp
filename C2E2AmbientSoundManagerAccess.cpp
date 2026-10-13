#include "XNumericConstants.h"
#include "C2GameState.h"
#include "ApplicationGlobals.h"
#include "C2LifetimeGlobals.h"
#include "C2LifetimeAccess.h"
#include "C2EngineLifetimeAccess.h"
#include "C2MapGeometry.h"
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
#include <initializer_list>


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

    static_assert(sizeof(SSoundLoopIds) == 84);

    static bool Ambient_SoundDeviceIsReady()
    {
        return NDSManager::CManager::sTheObjectPtr != nullptr && Ambient_ReadField<std::uint8_t>(NDSManager::CManager::sTheObjectPtr, 4) != 0;
    }

    static void* Ambient_GetStaticRecord(std::uint32_t ambientId)
    {
        return Ambient_AddressAt(CE2StaticDataSoundFXManager::sTheObjectPtr->ambientSounds, ambientId * 0x1570u);
    }

    static void* Ambient_GetSector(void* manager, std::int32_t x, std::int32_t y);

    // Complete contiguous scratch region: 00554DB4..005550D3, two 100-DWORD arrays.
    static std::uint32_t gLandscapeAmbientGroupCounts[100]{}; // 00554DB4..00554F43.
    static std::uint32_t gPatternAmbientGroupCounts[100]{}; // 00554F44..005550D3.

    // Original function: FUN_0047f3c7.
    void* __fastcall AmbientSoundManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        gE2AmbientSoundManager = manager;
        std::memset(manager, 0, 100);
        std::memset(&gAmbientSoundChangeState, 0, 10);
        if (NDSManager::CManager::sTheObjectPtr != nullptr)
        {
            for (const auto type : {2u, 0x16u, 0x31u, 0x32u, 0x33u}) NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(type, E2AmbientSoundManager_Callback, 0x32, 0xC8);
        }
        return manager;
    }

    // Original function: FUN_0047f4d6; CE2AmbientSoundManager::l_AllocateMemory.
    void AmbientSoundManager_AllocateMemory(void* manager)
    {
        using namespace NC2Lifetime;
        if (Read<void*>(manager, 8) != nullptr)
        {
            NXBasics::Runtime_Free(Read<void*>(manager, 8));
            Write<void*>(manager, 8, nullptr);
        }
        Write<std::int32_t>(manager, 0, (static_cast<std::int32_t>(static_cast<std::int16_t>(NC2Logic::GameMap_GetMacroWidth())) + 9) / 10);
        Write<std::int32_t>(manager, 4, (static_cast<std::int32_t>(static_cast<std::int16_t>(NC2Logic::GameMap_GetMacroHeight())) + 9) / 10);
        const auto size = Read<std::uint32_t>(manager, 0) * Read<std::uint32_t>(manager, 4) * 0x38u;
        void* sectors = NXBasics::Runtime_Allocate(size);
        Write<void*>(manager, 8, sectors);
        const auto clearSize = Read<std::uint32_t>(manager, 0) * Read<std::uint32_t>(manager, 4) * 0x38u;
        std::memset(sectors, 0, clearSize);
    }

    // Original function: FUN_0047f56e; l_RecalculatePatternAmbientSoundLoops.
    static void AmbientSoundManager_RecalculatePatternAmbientSoundLoops(void* sectorData, const NC2Logic::SSectorGridCursor* sector)
    {
        using namespace NC2Lifetime;
        Write<std::uint32_t>(sectorData, 0, 0);
        std::memset(gPatternAmbientGroupCounts, 0, sizeof(gPatternAmbientGroupCounts));
        const auto startX = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(sector->x) * 10u);
        const auto startY = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(sector->y) * 10u);
        const auto endX = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(startX) + 10u);
        const auto endY = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(startY) + 10u);
        for (auto y = startY; y < endY; ++y)
        {
            for (auto x = startX; x < endX; ++x)
            {
                const std::int16_t point[2] = {static_cast<std::int16_t>(x), static_cast<std::int16_t>(y)};
                if (NC2Logic::MapMagPoint_IsBorderPoint(point)) continue;
                const auto* data = Engine2d_EntryPoint_Tool_GetEngineMacroPointDataPtr(point);
                for (std::uint32_t triangle = 0; triangle < 2; ++triangle)
                {
                    const auto pattern = Read<std::uint16_t>(data, triangle * 2u);
                    // Original lookup entry: FUN_00480fe3, 70-byte static pattern records.
                    const auto* record = static_cast<const std::byte*>(gStaticPatternEntries) + static_cast<std::uint32_t>(pattern) * 0x70u;
                    for (std::uint32_t index = 0; index < Read<std::uint32_t>(record, 0x28); ++index)
                    {
                        const auto group = Read<std::uint32_t>(record, 0x2Cu + index * 4u);
                        // Windows 83 /7 sign-extends the FF immediate to FFFFFFFF.
                        if (CE2StaticDataSoundFXManager::sTheObjectPtr->patternGroupAmbientIds[group] != NXBasics::IntegerRange::UInt32Maximum) ++gPatternAmbientGroupCounts[group];
                    }
                }
            }
        }
        for (std::uint32_t group = 0; group < 100; ++group)
        {
            const auto weight = gPatternAmbientGroupCounts[group];
            if (weight == 0) continue;
            std::int32_t position = 0;
            auto count = Read<std::int32_t>(sectorData, 0);
            while (position < count && weight <= gPatternAmbientGroupCounts[Read<std::uint32_t>(sectorData, 4u + static_cast<std::uint32_t>(position) * 8u)]) ++position;
            if (position < count)
            {
                for (auto destination = 2; destination > position; --destination) Write<std::uint32_t>(sectorData, 4u + static_cast<std::uint32_t>(destination) * 8u, Read<std::uint32_t>(sectorData, 4u + static_cast<std::uint32_t>(destination - 1) * 8u));
                Write<std::uint32_t>(sectorData, 4u + static_cast<std::uint32_t>(position) * 8u, group);
                if (count < 3) Write<std::int32_t>(sectorData, 0, ++count);
            }
            if (position == count && count < 3)
            {
                Write<std::uint32_t>(sectorData, 4u + static_cast<std::uint32_t>(count) * 8u, group);
                Write<std::int32_t>(sectorData, 0, count + 1);
            }
        }
        for (std::int32_t index = 0; index < Read<std::int32_t>(sectorData, 0); ++index)
        {
            const auto offset = 4u + static_cast<std::uint32_t>(index) * 8u;
            const auto group = Read<std::uint32_t>(sectorData, offset);
            Write<std::uint32_t>(sectorData, offset + 4u, gPatternAmbientGroupCounts[group] * 100u / 200u);
            Write<std::uint32_t>(sectorData, offset, CE2StaticDataSoundFXManager::sTheObjectPtr->patternGroupAmbientIds[group]);
        }
    }

    // Original function: FUN_0047f760; l_RecalculateLandscapeAmbientSounds.
    static void AmbientSoundManager_RecalculateLandscapeAmbientSounds(void* sectorData, const NC2Logic::SSectorGridCursor* sector)
    {
        using namespace NC2Lifetime;
        Write<std::uint32_t>(sectorData, 0x1C, 0);
        std::memset(gLandscapeAmbientGroupCounts, 0, sizeof(gLandscapeAmbientGroupCounts));
        const auto startX = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(sector->x) * 20u);
        const auto startY = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(sector->y) * 20u);
        const auto endX = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(startX) + 20u);
        const auto endY = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(startY) + 20u);
        for (auto y = startY; y < endY; ++y)
        {
            for (auto x = startX; x < endX; ++x)
            {
                const std::int16_t point[2] = {static_cast<std::int16_t>(x), static_cast<std::int16_t>(y)};
                if (NC2Logic::MapMigPoint_IsBorderPoint(point)) continue;
                const auto* data = Engine2d_EntryPoint_Tool_GetEngineMicroPointDataPtr(point);
                const auto landscape = Read<std::uint16_t>(data, 0);
                if (landscape == NXBasics::IntegerRange::UInt16Maximum) continue;
                // Original lookup entry: FUN_00480ff1, 8C-byte static landscape records.
                const auto* record = static_cast<const std::byte*>(gStaticLandscapeEntries) + static_cast<std::uint32_t>(landscape) * 0x8Cu;
                for (std::uint32_t index = 0; index < Read<std::uint32_t>(record, 0x28); ++index)
                {
                    const auto group = Read<std::uint32_t>(record, 0x2Cu + index * 4u);
                    if (CE2StaticDataSoundFXManager::sTheObjectPtr->landscapeGroupAmbientIds[group] != NXBasics::IntegerRange::UInt32Maximum) ++gLandscapeAmbientGroupCounts[group];
                }
            }
        }
        for (std::uint32_t group = 0; group < 100; ++group)
        {
            const auto weight = gLandscapeAmbientGroupCounts[group];
            if (weight == 0) continue;
            std::int32_t position = 0;
            auto count = Read<std::int32_t>(sectorData, 0x1C);
            while (position < count && weight <= gLandscapeAmbientGroupCounts[Read<std::uint32_t>(sectorData, 0x20u + static_cast<std::uint32_t>(position) * 8u)]) ++position;
            if (position < count)
            {
                for (auto destination = 2; destination > position; --destination) Write<std::uint32_t>(sectorData, 0x20u + static_cast<std::uint32_t>(destination) * 8u, Read<std::uint32_t>(sectorData, 0x20u + static_cast<std::uint32_t>(destination - 1) * 8u));
                Write<std::uint32_t>(sectorData, 0x20u + static_cast<std::uint32_t>(position) * 8u, group);
                if (count < 3) Write<std::int32_t>(sectorData, 0x1C, ++count);
            }
            if (position == count && count < 3)
            {
                Write<std::uint32_t>(sectorData, 0x20u + static_cast<std::uint32_t>(count) * 8u, group);
                Write<std::int32_t>(sectorData, 0x1C, count + 1);
            }
        }
        for (std::int32_t index = 0; index < Read<std::int32_t>(sectorData, 0x1C); ++index)
        {
            const auto offset = 0x20u + static_cast<std::uint32_t>(index) * 8u;
            const auto group = Read<std::uint32_t>(sectorData, offset);
            Write<std::uint32_t>(sectorData, offset + 4u, gLandscapeAmbientGroupCounts[group]);
            Write<std::uint32_t>(sectorData, offset, CE2StaticDataSoundFXManager::sTheObjectPtr->landscapeGroupAmbientIds[group]);
        }
    }

    // Original function: FUN_0047f533.
    void AmbientSoundManager_RecalculateAmbientSoundSector(void* manager, const NC2Logic::SSectorGridCursor* sector, bool patterns, bool landscapes)
    {
        void* data = Ambient_GetSector(manager, sector->x, sector->y);
        if (patterns) AmbientSoundManager_RecalculatePatternAmbientSoundLoops(data, sector);
        if (landscapes) AmbientSoundManager_RecalculateLandscapeAmbientSounds(data, sector);
    }

    // Original function: FUN_0047f90b.
    void AmbientSoundManager_RecalculateAllAmbientSoundSector(void* manager)
    {
        for (std::uint32_t y = 0; y < Ambient_ReadField<std::uint32_t>(manager, 4); ++y)
        {
            for (std::uint32_t x = 0; x < Ambient_ReadField<std::uint32_t>(manager, 0); ++x)
            {
                NC2Logic::SSectorGridCursor sector;
                sector.x = NXBasics::Runtime_TimeSigned32(x);
                sector.y = NXBasics::Runtime_TimeSigned32(y);
                AmbientSoundManager_RecalculateAmbientSoundSector(manager, &sector, true, true);
            }
        }
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
                    NDSManager::CManager::sTheObjectPtr->PreloadWave_Play(waveId, entry.volume, NXBasics::IntegerRange::UInt32Maximum, 64, true, 1000);
            }

            if (previousIndex < previous->count)
                previous->entries[previousIndex].ambientId = NXBasics::IntegerRange::UInt32Maximum;
        }

        for (std::int32_t index = 0; index < previous->count; ++index)
        {
            if (previous->entries[index].ambientId == NXBasics::IntegerRange::UInt32Maximum)
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
                    if (ambientId == NXBasics::IntegerRange::UInt32Maximum)
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

    // Original function: FUN_004800c8; CE2AmbientSoundManager::StopAllAmbientSounds.
    void AmbientSoundManager_StopAllAmbientSounds(void* manager)
    {
        auto* soundManager = NDSManager::CManager::sTheObjectPtr;
        if (soundManager == nullptr || Ambient_ReadField<std::uint8_t>(soundManager, 4) == 0)
            return;

        auto* loops = reinterpret_cast<SSoundLoopIds*>(Ambient_AddressAt(manager, 0x0C));
        for (std::int32_t index = 0; index < loops->count; ++index)
        {
            if (index != 0) soundManager = NDSManager::CManager::sTheObjectPtr;
            auto& entry = loops->entries[index];
            if (entry.ambientId == NXBasics::IntegerRange::UInt32Maximum) continue;
            const std::uint32_t waveId = Ambient_ReadField<std::uint32_t>(Ambient_GetStaticRecord(entry.ambientId), 0xA8);
            if (soundManager->PreloadWave_IsRunning(waveId))
            {
                NDSManager::CManager::sTheObjectPtr->PreloadWave_Stop(waveId);
                entry.ambientId = NXBasics::IntegerRange::UInt32Maximum;
            }
        }
    }

    // Original function: FUN_00480137; weighted selection within a static ambient record.
    static const void* Ambient_SelectLandscapeSound(const void* record)
    {
        std::uint32_t remaining = static_cast<std::uint32_t>(NXBasics::Runtime_Rand()) % Ambient_ReadField<std::uint32_t>(record, 0x28);
        std::uint32_t index = 0;
        for (;; ++index)
        {
            remaining -= Ambient_ReadField<std::uint32_t>(record, 0xB0u + index * 0x88u);
            if ((remaining & NXBasics::IntegerRange::Bit31) != 0)
                return static_cast<const std::byte*>(record) + 0x30u + index * 0x88u;
        }
    }

    // Original function: FUN_0047fb9d; CE2AmbientSoundManager::l_PlayLandscapeAmbientSounds.
    static void AmbientSoundManager_PlayLandscapeAmbientSounds(void* manager, const NC2Logic::SSectorGridCursor& first, const NC2Logic::SSectorGridCursor& last, const std::int16_t* rectangle, const void* display)
    {
        if (!Ambient_SoundDeviceIsReady() || Ambient_ReadField<std::uint32_t>(manager, 0x60) == NC2Logic::gGameTime.tick)
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
        std::memcpy(static_cast<std::byte*>(manager) + 0x60, &NC2Logic::gGameTime.tick, sizeof(NC2Logic::gGameTime.tick));
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
