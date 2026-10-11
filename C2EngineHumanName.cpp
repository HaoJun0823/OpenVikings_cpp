#include "C2EngineObjectData.h"
#include "C2MissionStartObjects.h"
#include "C2StringIO.h"
#include "XStringArray.h"
#include "C2E2StaticDataHumanMisc.h"
#include "XPalette.h"

#include <cstring>

namespace NC2E2
{
    // Original function: FUN_004806c5; SE2DynamicDataHumanData::SetName.
    void DynamicHuman_SetName(void* human, const char* name)
    {
        auto* bytes = static_cast<char*>(human);
        // Original CRT entry 004e66a0 is strncpy, including zero padding.
        std::strncpy(bytes + 0x85C, name, 99);
        bytes[0x8BF] = '\0';
    }
}

namespace NC2LogicExternals
{
    // Original function: FUN_00480939; each call owns and releases its mission string array.
    void External_Human_SetName(std::uint32_t human, std::uint32_t name)
    {
        auto* strings = NC2Logic::StringIO_GetMissionStringArrayPtr();
        if (strings == nullptr) return;
        const auto* text = strings->GetStringPtr(name);
        if (text != nullptr) NC2E2::DynamicHuman_SetName(NC2E2::gDynamicDataHumans.entries + human * 0x8C8u, text);
        delete strings;
    }
    // Original function: FUN_00480975.
    void External_Human_SetRandomPalettePatch(std::uint32_t human, const char* palette)
    {
        const auto patch = NC2E2::StaticHumanMisc_RandomPalette_GetPatchId(palette);
        if (patch == -1) return;
        auto* data = NC2E2::gDynamicDataHumans.entries + human * 0x8C8u;
        NC2E2::StaticHumanMisc_RandomPalette_Execute(patch, reinterpret_cast<NXBasics::CPalette*>(data + 0x44), reinterpret_cast<NXBasics::CPalette*>(data + 0x450), false);
    }
}
