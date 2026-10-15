#pragma once

#include "C2RuntimeConstants.h"

#include <cstddef>
#include <cstdint>

namespace NXBasics
{
	class CPalette; class CStringArray;
}

namespace NC2E2
{
	struct SHumanBodyGraphics
	{
		std::uint32_t variantId;
		void* bodyBobManager;
		void* shadowBobManager;
	};

	struct SHumanHeadGraphics
	{
		std::uint32_t variantId;
		void* headBobManager;
	};

	struct SHumanJobGraphics
	{
		std::uint32_t tribe;
		std::uint32_t job;
		std::uint32_t bodyCount;
		SHumanBodyGraphics bodies[NC2Runtime::HumanAppearanceLimits::GraphicsChoices];
		std::uint32_t headCount;
		SHumanHeadGraphics heads[NC2Runtime::HumanAppearanceLimits::GraphicsChoices];
	};

	struct SHumanJobBaseGraphics
	{
		SHumanJobGraphics graphics;
		std::uint32_t bodyPaletteCount;
		std::int32_t bodyPaletteIds[NC2Runtime::HumanAppearanceLimits::GraphicsChoices];
		std::uint32_t headPaletteCount;
		std::int32_t headPaletteIds[NC2Runtime::HumanAppearanceLimits::GraphicsChoices];
		std::uint32_t randomPatchCount;
		std::int32_t randomPatchIds[NC2Runtime::HumanAppearanceLimits::GraphicsChoices];
	};

	struct SHumanJobChangeGraphics
	{
		SHumanJobGraphics graphics;
		std::int32_t randomPatchId;
	};

	struct SRandomPalettePatch
	{
		std::uint32_t destinationRange;
		std::int32_t palette16Id;
		std::int32_t sourceRange;
		std::uint32_t weight;
	};

	struct SRandomPaletteSet
	{
		char name[40];
		std::uint32_t patchCount;
		SRandomPalettePatch* patches;
		std::uint32_t destinationRangeCount;
		std::uint32_t destinationRanges[NC2Runtime::HumanAppearanceLimits::DestinationRanges];
	};

	struct STribeHumanNameArrays
	{
		NXBasics::CStringArray* male;
		NXBasics::CStringArray* female;
	};

	struct SHumanPlayerColorGraphics
	{
		std::uint32_t color;
		std::int32_t maleRandomPatchId;
		std::int32_t femaleRandomPatchId;
		std::int32_t inhabitantSigns[NC2Runtime::HumanAppearanceLimits::InhabitantSigns];
		std::int32_t soldierSigns[NC2Runtime::HumanAppearanceLimits::SoldierSigns];
		std::int32_t roadBuildSignLandscapeId;
		std::int32_t wallBuildSignLandscapeId;
		std::int32_t playerPaletteId;
		std::int32_t shipPaletteId;
	};

	struct SHumanMiscData
	{
		std::uint32_t randomPaletteSetCount;
		SRandomPaletteSet* randomPaletteSets;
		std::uint32_t jobBaseGraphicsCount;
		SHumanJobBaseGraphics* jobBaseGraphics;
		std::uint32_t jobChangeGraphicsCount;
		SHumanJobChangeGraphics* jobChangeGraphics;
		STribeHumanNameArrays tribeNames[NC2Runtime::HumanAppearanceLimits::Tribes];
		SHumanPlayerColorGraphics playerColors[NC2Runtime::HumanAppearanceLimits::PlayerColors];
	};

	static_assert(sizeof(STribeHumanNameArrays) == 8);
	static_assert(sizeof(SHumanPlayerColorGraphics) == 0x58);
	static_assert(offsetof(SHumanPlayerColorGraphics, roadBuildSignLandscapeId) == 0x48);
	static_assert(offsetof(SHumanPlayerColorGraphics, playerPaletteId) == 0x50);
	static_assert(sizeof(SHumanMiscData) == 968);
	static_assert(offsetof(SHumanMiscData, tribeNames) == 0x18);
	static_assert(offsetof(SHumanMiscData, playerColors) == 0x58);
	static_assert(sizeof(SHumanBodyGraphics) == 12);
	static_assert(sizeof(SHumanHeadGraphics) == 8);
	static_assert(sizeof(SHumanJobGraphics) == 0x74);
	static_assert(offsetof(SHumanJobGraphics, bodies) == 0x0C);
	static_assert(offsetof(SHumanJobGraphics, headCount) == 0x48);
	static_assert(offsetof(SHumanJobGraphics, heads) == 0x4C);
	static_assert(sizeof(SHumanJobBaseGraphics) == 0xBC);
	static_assert(offsetof(SHumanJobBaseGraphics, bodyPaletteCount) == 0x74);
	static_assert(offsetof(SHumanJobBaseGraphics, headPaletteCount) == 0x8C);
	static_assert(offsetof(SHumanJobBaseGraphics, randomPatchCount) == 0xA4);
	static_assert(sizeof(SHumanJobChangeGraphics) == 0x78);
	static_assert(offsetof(SHumanJobChangeGraphics, randomPatchId) == 0x74);
	static_assert(sizeof(SRandomPalettePatch) == 16);
	static_assert(sizeof(SRandomPaletteSet) == 0xB4);
	static_assert(offsetof(SRandomPaletteSet, patchCount) == 0x28);
	static_assert(offsetof(SRandomPaletteSet, patches) == 0x2C);
	static_assert(offsetof(SRandomPaletteSet, destinationRangeCount) == 0x30);
	static_assert(offsetof(SRandomPaletteSet, destinationRanges) == 0x34);

	void* __fastcall E2StaticDataGoodManager_Construct(void* manager, void* unusedEdx);
	void* __fastcall E2StaticDataHumanMiscManager_Construct(void* manager, void* unusedEdx);
	std::int32_t StaticHumanMisc_RandomPalette_GetPatchId(const char* name);
	void StaticHumanMisc_RandomPalette_Execute(std::uint32_t patchId, NXBasics::CPalette* body, NXBasics::CPalette* head, bool deterministic);
	std::int32_t StaticHumanMisc_ColorIds_GetPlayerRandomPatchId(std::int32_t player, bool female);
	const char* StaticHumanMisc_TribeHumanNames_GetRandomNamePtr(std::uint32_t tribe, bool female);
	const void* StaticHumanMisc_JobGraphicsBase_GetDataPtr(std::uint32_t tribe, std::uint32_t job);
	const void* StaticHumanMisc_JobGraphicsChange_GetDataPtr(std::uint32_t tribe, std::uint32_t job);
}
