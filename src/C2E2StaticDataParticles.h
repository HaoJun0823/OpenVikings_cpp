#pragma once

#include <cstdint>

namespace NC2E2
{
	void* __fastcall E2StaticDataParticelManager_Construct(void* manager, void* unusedEdx);
	void* __fastcall E2StaticDataParticelEffectManager_Construct(void* manager, void* unusedEdx);
	std::int32_t E2StaticDataParticelManager_GetParticelId(const char* name);
	void* ParticelGraphics_GetValency(const void* list, std::uint32_t valency);
}
