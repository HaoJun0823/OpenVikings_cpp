#include "SwitchIdentifiers.h"
#include "C2E2HumanSoundAccess.h"
#include "C2E2StaticDataSoundFX.h"
#include "C2EngineDisplayManager.h"
#include "C2EngineDisplayAccess.h"
#include "C2EngineMoveableDrawing.h"
#include "C2EngineParticleSimulation.h"
#include "C2HouseAccess.h"
#include "C2GameState.h"
#include "C2HumanIterator.h"
#include "C2GameMapAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2VehicleAccess.h"
#include "DSManager.h"
#include "XCrtFileAccess.h"
#include "C2E2StaticDataHumanMisc.h"
#include "C2EngineObjectData.h"
#include "C2EngineMapTerrainAccess.h"
#include "C2MapArea.h"
#include "C2MissionStartObjects.h"

namespace NC2E2
{
    using NC2Lifetime::Read;

    // Original function: FUN_0047ec5b.
    bool __cdecl E2Manager_sl_Callback_Misc(std::uint32_t type, std::uint32_t, std::uint32_t parameter1, std::uint32_t parameter2, std::uint32_t parameter3)
    {
        switch (type)
        {
        case SwitchIds::CallbackType::HouseWalkAreaChanged:
        {
            const void* house = NC2Logic::CHouseArray::mStaticVars + parameter1 * NC2Logic::CHouseArray::ElementSize;
            if (NC2Logic::House_GetHouseMainType(house) == 6) return true;
            const auto* area = static_cast<const NC2Logic::CMapArea*>(NC2Logic::House_Misc_WalkBlockArea_GetPtr(house));
            const auto root = Read<std::uint32_t>(house, 0x1C);
            for (std::uint32_t rowIndex = 0; rowIndex < area->rowCount; ++rowIndex)
            {
                const auto* row = area->rows + rowIndex;
                std::uint32_t point;
                NC2Logic::MapMigPoint_ConstructOffset(&point, &root, row->x, row->y, false);
                for (std::uint32_t column = 0; column < row->width; ++column)
                {
                    if (NC2Logic::MapMigPoint_IsInsideMap(&point))
                    {
                        EngineMap_OverlayPattern_HouseAreaChanged(&point, false);
                        auto neighbor = point;
                        NC2Logic::MapMigPoint_MoveCursorUnwrapped(neighbor, 4);
                        for (std::uint32_t direction = 0; direction < 6; ++direction)
                        {
                            if (NC2Logic::MapMigPoint_IsInsideMap(&neighbor)) EngineMap_OverlayPattern_HouseAreaChanged(&neighbor, false);
                            NC2Logic::MapMigPoint_MoveCursorUnwrapped(neighbor, direction);
                        }
                    }
                    NC2Lifetime::Write<std::uint16_t>(&point, 0, static_cast<std::uint16_t>(Read<std::uint16_t>(&point, 0) + 1u));
                }
            }
            std::int16_t rectangle[4];
            NC2Logic::MapMigRectangle_Construct(rectangle, static_cast<std::int16_t>(Read<std::uint16_t>(&root, 0) - 20u), static_cast<std::int16_t>(Read<std::uint16_t>(&root, 2) - 20u), 40, 40);
            C2DEngineDisplayManager::sTheObjectPtr->InformThatStaticMapChanged(rectangle, 0xFFFFFFFFu);
            return true;
        }
        case SwitchIds::CallbackType::ResetHouseDynamicGraphics:
            std::memset(static_cast<std::byte*>(gDynamicDataHouses.entries) + parameter1 * 0xD0u, 0, 0xCC);
            return true;
        case SwitchIds::CallbackType::HumanPaletteChanged:
        {
            std::int32_t patch;
            if (parameter2 == 2)
            {
                const void* human = NC2Logic::GetHumanPtr(static_cast<std::int32_t>(parameter1));
                if ((Read<std::uint32_t>(human, 0xB84) & 0x400u) != 0) return true;
                patch = StaticHumanMisc_ColorIds_GetPlayerRandomPatchId(Read<std::int32_t>(human, 0x10), NC2Logic::Human_IsSexFemale(human));
            }
            else if (parameter2 == 1)
            {
                // Windows dereferences the null result for an out-of-range good ID.
                const void* good = static_cast<std::int32_t>(parameter3) < 66 ? static_cast<const std::byte*>(gStaticGoodData) + parameter3 * 4u : nullptr;
                patch = Read<std::int32_t>(good, 0);
            }
            else return true;
            if (patch != -1)
            {
                auto* human = static_cast<std::byte*>(gDynamicDataHumans.entries) + parameter1 * 0x8C8u;
                StaticHumanMisc_RandomPalette_Execute(static_cast<std::uint32_t>(patch), reinterpret_cast<NXBasics::CPalette*>(human + 0x44), reinterpret_cast<NXBasics::CPalette*>(human + 0x450), false);
            }
            return true;
        }
        case SwitchIds::CallbackType::HumanMiscPaletteChanged:
        {
            char name[128];
            NXBasics::Runtime_PrintString(name, "human_misc_%3.3d", parameter2);
            const auto patch = StaticHumanMisc_RandomPalette_GetPatchId(name);
            if (patch != -1)
            {
                auto* human = static_cast<std::byte*>(gDynamicDataHumans.entries) + parameter1 * 0x8C8u;
                StaticHumanMisc_RandomPalette_Execute(static_cast<std::uint32_t>(patch), reinterpret_cast<NXBasics::CPalette*>(human + 0x44), reinterpret_cast<NXBasics::CPalette*>(human + 0x450), false);
            }
            return true;
        }
        case SwitchIds::CallbackType::HumanJobChanged: E2DynamicDataHumanManager_HumanJobChanged(parameter1, parameter2, parameter3); return true;
        case SwitchIds::CallbackType::HumanArmorChanged: E2DynamicDataHumanManager_HumanArmorChanged(parameter1, parameter2); return true;
        case SwitchIds::CallbackType::PlayPlayerJingle:
        {
            const void* display = C2DEngineDisplayManager::sTheObjectPtr->GetActiveDisplay();
            if (display != nullptr && Read<std::uint32_t>(display, 0x7C) == parameter1) NDSManager::CManager::Jingle_Play(parameter2);
            return true;
        }
        case SwitchIds::CallbackType::UpdateHouseOverlayParticles:
        {
            const void* house = NC2Logic::CHouseArray::mStaticVars + parameter1 * NC2Logic::CHouseArray::ElementSize;
            NC2Logic::House_GetHouseTypeDataPtr(house);
            const void* data = Engine2d_EntryPoint_GetHouseDataPtr(Read<std::uint32_t>(house, 0x10));
            bool found = false;
            if (parameter3 != 0)
            {
                for (std::uint32_t index = 0; index < Read<std::uint32_t>(data, 0x490); ++index)
                {
                    const void* entry = static_cast<const std::byte*>(Read<void*>(data, 0x494)) + index * 0xE4u;
                    if (Read<std::uint32_t>(entry, 4) != 3 || Read<std::uint32_t>(entry, 0) != Read<std::uint32_t>(house, 0x20)) continue;
                    auto position = Read<std::uint32_t>(house, 0x1C);
                    NC2Logic::MapMigPoint_MoveCursorUnwrapped(position, 2);
                    const std::int32_t pixel[2] = {NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(entry, 0xC) + 17u), NXBasics::Runtime_TimeSigned32(0u - Read<std::uint32_t>(entry, 0x10) + 19u)};
                    E2ParticelManager_AddParticelGenerator(gE2ParticelManager, 0, &position, pixel, 5, Read<std::uint32_t>(house, 8));
                    found = true;
                }
            }
            if (found) E2ParticelManager_EnableGenerator(gE2ParticelManager, Read<std::uint32_t>(house, 8));
            else E2ParticelManager_DestroyGenerator(gE2ParticelManager, Read<std::uint32_t>(house, 8));
            return true;
        }
        case SwitchIds::CallbackType::CreateMunitionParticle:
        {
            const void* data = reinterpret_cast<const void*>(static_cast<std::uintptr_t>(parameter1));
            E2ParticelManager_CreateMunitionParticel(gE2ParticelManager, data, Read<std::uint32_t>(data, 8), static_cast<const std::byte*>(data) + 4, static_cast<const std::byte*>(data) + 0xC, static_cast<const std::byte*>(data) + 0x14, Read<std::uint32_t>(data, 0x1C), Read<std::uint32_t>(data, 0x20));
            return true;
        }
        case SwitchIds::CallbackType::CreateFallingSmokeParticle: E2ParticelManager_CreateFallingSmokeParticel(gE2ParticelManager, &parameter1, parameter2, parameter3); return true;
        case SwitchIds::CallbackType::StartParticleEffect: E2ParticelManager_StartParticelEffect(gE2ParticelManager, parameter1, &parameter2, parameter3); return true;
        case SwitchIds::CallbackType::StartDirectionalParticleEffect: E2ParticelManager_StartParticelEffectIntoDirection(gE2ParticelManager, parameter1, &parameter2, &parameter3, 0); return true;
        case SwitchIds::CallbackType::StartDirectionalParticleEffectFromRecord:
        {
            const void* data = reinterpret_cast<const void*>(static_cast<std::uintptr_t>(parameter1));
            E2ParticelManager_StartParticelEffectIntoDirection(gE2ParticelManager, 1, static_cast<const std::byte*>(data) + 4, static_cast<const std::byte*>(data) + 8, Read<std::uint32_t>(data, 0x14));
            E2HumanSoundManager_PlayHumanScreamSound(gE2HumanSoundManager, Read<void*>(data, 0x18));
            return true;
        }
        }
        const auto soundType = parameter1, point = parameter2, skipRunning = parameter3;
        if (type != 0x17 || NDSManager::CManager::sTheObjectPtr == nullptr) return true;
        const auto* display = C2DEngineDisplayManager::sTheObjectPtr->GetActiveDisplay();
        const auto x = Read<std::int16_t>(&point, 0), y = Read<std::int16_t>(&point, 2);
        const auto left = Read<std::int16_t>(display, 0x97A), top = Read<std::int16_t>(display, 0x97C), width = Read<std::int16_t>(display, 0x97E), height = Read<std::int16_t>(display, 0x980);
        if (x < left || x > static_cast<std::int16_t>(left + width - 1) || y < top || y > static_cast<std::int16_t>(top + height - 1)) return true;
        const auto player = Read<std::uint32_t>(display, 0x7C);
        if ((Read<std::uint8_t>(display, 0x70) & 4u) != 0 && player < 16 && (Read<std::uint16_t>(NC2Logic::GameMap_GetConstMapMicroPointPtr(&point), 8) & (1u << player)) == 0) return true;
        const auto pan = (x - width / 2 - left) * 128 / width, vertical = (y - height / 2 - top) * 128 / height;
        const auto distance = ((pan < 0 ? -pan : pan) + (vertical < 0 ? -vertical : vertical)) / 2;
        if (distance > 64) return true;
        auto* sounds = CE2StaticDataSoundFXManager::sTheObjectPtr;
        const auto soundId = sounds->staticSoundIdsByType[soundType];
        if (soundId == 0xFFFFFFFFu) return true;
        const auto* sound = static_cast<const std::byte*>(sounds->staticSounds) + soundId * 0x43Cu;
        const auto count = Read<std::uint32_t>(sound, 0x28);
        if (count == 0) return true;
        const auto variant = static_cast<std::uint32_t>(NXBasics::Runtime_Rand()) % count;
        const auto wave = Read<std::uint32_t>(sound, 0x2Cu + variant * 8u);
        auto* manager = NDSManager::CManager::sTheObjectPtr;
        if (skipRunning != 0 && manager->PreloadWave_IsRunning(wave)) return true;
        manager->PreloadWave_Play(wave, ((64u - static_cast<std::uint32_t>(distance)) * Read<std::uint32_t>(sound, 0x30u + variant * 8u)) >> 6, 1, static_cast<std::uint32_t>(pan) + 64u, false, 0);
        return true;
    }

    // Original function: FUN_004856f9. This helper is inlined in the Mac reference.
    static std::uint32_t HumanSound_GetSexAgeIndex(const void* human)
    {
        if (!NC2Logic::Human_IsAgeAdult(human)) return 0;
        return NC2Logic::Human_IsSexMale(human) ? 2u : 1u;
    }

    // Original function: FUN_004856b4.
    void E2HumanSoundManager_PlayHumanScreamSound(const void* manager, const void* human)
    {
        const void* display = C2DEngineDisplayManager::sTheObjectPtr->GetActiveDisplay();
        const auto tribe = Read<std::uint32_t>(human, 0x14);
        const auto category = HumanSound_GetSexAgeIndex(human);
        const auto soundId = Read<std::uint32_t>(manager, 8u + (tribe * 3u + category) * 0x8Cu);
        Tool_Sound_PlaySoundOrientedOnDisplay(NC2Logic::Human_GetMapPositionRef(human), soundId, display, true, true);
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

    // Original function: FUN_0048575f; one chance value and at most one sound per game tick.
    void E2HumanSoundManager_PlayGenericSounds(void* manager, const void* display)
    {
        if (NDSManager::CManager::sTheObjectPtr == nullptr || Read<std::uint8_t>(NDSManager::CManager::sTheObjectPtr, 4) == 0) return;
        const auto tick = NC2Logic::gGameTime.tick;
        if (Read<std::uint32_t>(manager, 0) == tick) return;
        NC2Lifetime::Write(manager, 0, tick);
        std::int16_t rectangle[4]{};
        DisplayDescription_GetMapRectangle(display, Read<std::uint32_t>(display, 0x954), Read<std::uint32_t>(display, 0x958), Read<std::uint32_t>(display, 0x964), Read<std::uint32_t>(display, 0x968), rectangle);
        const std::uint32_t center = static_cast<std::uint16_t>(rectangle[0] + (rectangle[2] >> 1)) | (static_cast<std::uint32_t>(static_cast<std::uint16_t>(rectangle[1] + (rectangle[3] >> 1))) << 16);
        const auto player = Read<std::uint32_t>(display, 0x7C);
        const auto chance = static_cast<std::uint32_t>(NXBasics::Runtime_Rand() % 2000);
        for (std::uint32_t tribe = 0; tribe < 8; ++tribe)
        {
            for (std::uint32_t category = 0; category < 3; ++category)
            {
                const auto offset = (tribe * 3u + category) * 0x8Cu;
                if (chance >= Read<std::uint32_t>(manager, offset + 4u) || Read<std::uint32_t>(manager, offset + 0x0Cu) == 0xFFFFFFFFu) continue;
                NC2Logic::CHumanIterator iterator;
                const void* selected = nullptr;
                for (auto id = iterator.GetFirstSectorHumanId(player, center, rectangle[2], false); id != -1; id = iterator.GetNextSectorHumanId())
                {
                    const auto* human = NC2Logic::CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * NC2Logic::CHumanArray::ElementSize;
                    if (Read<std::uint32_t>(human, 0x14) == tribe && HumanSound_GetSexAgeIndex(human) == category) { selected = human; break; }
                }
                if (selected == nullptr && category == 2)
                {
                    for (auto id = iterator.GetFirstSectorHumanId(player, center, rectangle[2], true); id != -1; id = iterator.GetNextSectorHumanId())
                    {
                        const auto* human = NC2Logic::CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * NC2Logic::CHumanArray::ElementSize;
                        if (Read<std::uint32_t>(human, 0x14) == tribe && HumanSound_GetSexAgeIndex(human) == 2) { selected = human; break; }
                    }
                }
                if (selected == nullptr) continue;
                Tool_Sound_PlaySoundOrientedOnDisplay(NC2Logic::Human_GetMapPositionRef(selected), Read<std::uint32_t>(manager, offset + 0x0Cu), display, true, true);
                return;
            }
        }
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
