#include "C2E2StaticDataHumanMisc.h"
#include "C2EngineObjectData.h"
#include "C2MissionStartObjects.h"
#include "C2ObjectLayout.h"
#include "C2StringIO.h"
#include "XPalette.h"
#include "XStringArray.h"

#include <cstring>

namespace NC2E2
{
	// Original function: FUN_004806c5; SE2DynamicDataHumanData::SetName.
	void DynamicHuman_SetName(void* human, const char* name)
	{
		auto* bytes = static_cast<char*>(human);
		// Original CRT entry 004e66a0 is strncpy, including zero padding.
		std::strncpy(bytes + NC2Layout::DynamicHumanOffset::Name, name, 99);
		bytes[NC2Layout::DynamicHumanOffset::NameTerminator] = '\0';
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
		if (text != nullptr) NC2E2::DynamicHuman_SetName(NC2E2::gDynamicDataHumans.entries + human * NC2Layout::RecordSize::DynamicHumanUnsigned, text);
		delete strings;
	}
	// Original function: FUN_00480975.
	void External_Human_SetRandomPalettePatch(std::uint32_t human, const char* palette)
	{
		const auto patch = NC2E2::StaticHumanMisc_RandomPalette_GetPatchId(palette);
		if (patch == -1) return;
		auto* data = NC2E2::gDynamicDataHumans.entries + human * NC2Layout::RecordSize::DynamicHumanUnsigned;
		NC2E2::StaticHumanMisc_RandomPalette_Execute(patch, reinterpret_cast<NXBasics::CPalette*>(data + NC2Layout::DynamicHumanOffset::BodyPalette), reinterpret_cast<NXBasics::CPalette*>(data + NC2Layout::DynamicHumanOffset::HeadPalette), false);
	}
}
