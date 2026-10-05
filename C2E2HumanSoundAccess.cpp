#include "C2E2HumanSoundAccess.h"
#include "C2E2StaticDataSoundFX.h"
#include "C2EngineDisplayManager.h"
#include "C2GameMapAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2VehicleAccess.h"
#include "DSManager.h"
#include "XCrtFileAccess.h"

namespace NC2E2
{
    using NC2Lifetime::Read;

    // Original function: FUN_004856f9. This helper is inlined in the Mac reference.
    static std::uint32_t HumanSound_GetSexAgeIndex(const void* human)
    {
        if (!NC2Logic::Human_IsAgeAdult(human)) return 0;
        return NC2Logic::Human_IsSexMale(human) ? 2u : 1u;
    }

    // Original function: FUN_0048571d; eight tribes, three sex/age counters each.
    void E2HumanSoundManager_ClearSeenHumanCounter(void* manager)
    {
        for (std::uint32_t tribe = 0; tribe < 8; ++tribe)
            for (std::uint32_t category = 0; category < 3; ++category) NC2Lifetime::Write<std::uint32_t>(manager, 4u + (tribe * 3u + category) * 0x8Cu, 0);
    }

    // Original function: FUN_00485735.
    void E2HumanSoundManager_IncreaseSeenCounter(void* manager, const void* human)
    {
        const auto tribe = Read<std::uint32_t>(human, 0x14);
        const auto offset = 4u + (tribe * 3u + HumanSound_GetSexAgeIndex(human)) * 0x8Cu;
        NC2Lifetime::Write(manager, offset, Read<std::uint32_t>(manager, offset) + 1u);
    }

    // Original function: FUN_00485a73. The Mac reference inlines this response-list access.
    static std::uint32_t HumanSound_GetRespondingSoundId(const void* record, std::uint32_t responseType, std::uint32_t selector)
    {
        const std::uint32_t offset = responseType * 0x40u;
        const std::uint32_t count = Read<std::uint32_t>(record, offset + 8u);
        return count == 0 ? 0xFFFFFFFFu : Read<std::uint32_t>(record, offset + 0x0Cu + (selector % count) * 4u);
    }

    // Original function: FUN_00485931; CE2HumanSoundManager::PlayRespondingSound.
    void E2HumanSoundManager_PlayRespondingSound(const void* manager, std::uint32_t responseType, const void* human, const void* display)
    {
        if (NDSManager::CManager::sTheObjectPtr == nullptr || Read<std::uint8_t>(NDSManager::CManager::sTheObjectPtr, 4) == 0) return;
        std::uint32_t selector = Read<std::uint32_t>(human, 4);
        if (NC2Logic::Human_IsJobHero(human)) selector = 0;
        const std::uint32_t tribe = Read<std::uint32_t>(human, 0x14);
        const std::uint32_t job = Read<std::uint32_t>(human, 0x18);
        if ((tribe == 2u && job == 0x2Au) || (tribe == 3u && (job == 0x2Au || job == 0x20u))) return;

        const auto* record = static_cast<const std::byte*>(manager) + 8u + (tribe * 3u + HumanSound_GetSexAgeIndex(human)) * 0x8Cu;
        const std::uint32_t soundId = HumanSound_GetRespondingSoundId(record, responseType, selector);
        if (soundId == 0xFFFFFFFFu) return;
        const auto* sound = static_cast<const std::byte*>(CE2StaticDataSoundFXManager::sTheObjectPtr->staticSounds) + soundId * 0x43Cu;
        if (Read<std::uint32_t>(sound, 0x28) == 0) return;
        for (std::uint32_t index = 0; index < Read<std::uint32_t>(sound, 0x28); ++index)
        {
            if (NDSManager::CManager::sTheObjectPtr->PreloadWave_IsRunning(Read<std::uint32_t>(sound, 0x2Cu + index * 8u))) return;
        }
        const auto* currentRecord = static_cast<const std::byte*>(manager) + 8u + (Read<std::uint32_t>(human, 0x14) * 3u + HumanSound_GetSexAgeIndex(human)) * 0x8Cu;
        const std::uint32_t currentSoundId = HumanSound_GetRespondingSoundId(currentRecord, responseType, selector);
        Tool_Sound_PlaySoundOrientedOnDisplay(NC2Logic::Human_GetMapPositionRef(human), currentSoundId, display, true, false);
    }

    // Original function: FUN_00485a4e; the human-id overload uses the active display.
    void E2HumanSoundManager_PlayRespondingSound(const void* manager, std::uint32_t responseType, std::int32_t humanId)
    {
        const void* display = C2DEngineDisplayManager::sTheObjectPtr->GetActiveDisplay();
        const void* human = static_cast<const std::byte*>(NC2Logic::CHumanArray::mStaticVars) + static_cast<std::uint32_t>(humanId) * NC2Logic::CHumanArray::ElementSize;
        E2HumanSoundManager_PlayRespondingSound(manager, responseType, human, display);
    }

    // Original function: FUN_00490eea. Visibility/attenuation are optional; pan is always calculated.
    void Tool_Sound_PlaySoundOrientedOnDisplay(const void* point, std::uint32_t soundId, const void* display, bool skipRunning, bool attenuate)
    {
        if (NDSManager::CManager::sTheObjectPtr == nullptr || Read<std::uint8_t>(NDSManager::CManager::sTheObjectPtr, 4) == 0 || soundId == 0xFFFFFFFFu) return;
        const std::int32_t x = Read<std::int16_t>(point, 0);
        const std::int32_t y = Read<std::int16_t>(point, 2);
        const std::int32_t left = Read<std::int16_t>(display, 0x97A);
        const std::int32_t top = Read<std::int16_t>(display, 0x97C);
        const std::int32_t width = Read<std::int16_t>(display, 0x97E);
        const std::int32_t height = Read<std::int16_t>(display, 0x980);
        if (attenuate)
        {
            const std::int16_t right = static_cast<std::int16_t>(left + width - 1);
            const std::int16_t bottom = static_cast<std::int16_t>(top + height - 1);
            if (x < left || x > right || y < top || y > bottom) return;
            if ((Read<std::uint8_t>(display, 0x70) & 4u) != 0)
            {
                const std::uint32_t player = Read<std::uint32_t>(display, 0x7C);
                if (player < 16u && (Read<std::uint16_t>(NC2Logic::GameMap_GetConstMapMicroPointPtr(point), 8) & (1u << player)) == 0) return;
            }
        }
        // The Windows routine requires a nonzero display width and height.
        std::int32_t pan = (x - width / 2 - left) * 128 / width;
        const std::int32_t vertical = (y - top - height / 2) * 128 / height;
        const std::int32_t distance = ((pan < 0 ? -pan : pan) + (vertical < 0 ? -vertical : vertical)) / 2;
        if (distance > 64 && attenuate) return;
        const auto* sound = static_cast<const std::byte*>(CE2StaticDataSoundFXManager::sTheObjectPtr->staticSounds) + soundId * 0x43Cu;
        const std::uint32_t count = Read<std::uint32_t>(sound, 0x28);
        if (count == 0) return;
        const std::uint32_t variant = static_cast<std::uint32_t>(NXBasics::Runtime_Rand()) % count;
        const std::uint32_t waveId = Read<std::uint32_t>(sound, 0x2Cu + variant * 8u);
        if (skipRunning && NDSManager::CManager::sTheObjectPtr->PreloadWave_IsRunning(waveId)) return;
        std::uint32_t volume = Read<std::uint32_t>(sound, 0x30u + variant * 8u);
        if (attenuate) volume = ((64u - static_cast<std::uint32_t>(distance)) * volume) >> 6;
        else
        {
            if (pan < -64) pan = -64;
            if (pan > 63) pan = 63;
        }
        NDSManager::CManager::sTheObjectPtr->PreloadWave_Play(waveId, volume, 1, static_cast<std::uint32_t>(pan) + 64u, false, 0);
    }
}
