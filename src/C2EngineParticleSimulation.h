#pragma once

#include <cstdint>

namespace NC2E2
{
	void __fastcall E2ParticelManager_DoOneGameTick(void* manager, void* unusedEdx);
	void* E2ParticelManager_GetCleanParticel(void* manager);
	void E2ParticelManager_CreateFallingSmokeParticel(void* manager, const void* mapPoint, std::uint32_t height, std::uint32_t lifetime);
	void E2ParticelManager_CreateFallingSpawnParticel(void* manager, const void* source);
	void E2ParticelManager_CreateMunitionParticel(void* manager, const void* sourcePoint, std::uint32_t height, const void* targetPoint, const void* sourcePixel, const void* targetPixel, std::uint32_t munitionType, std::uint32_t duration);
	void E2ParticelManager_StartParticelEffect(void* manager, std::uint32_t effectId, const void* mapPoint, std::uint32_t player);
	void E2ParticelManager_StartParticelEffectIntoDirection(void* manager, std::uint32_t effectId, const void* sourcePoint, const void* targetPoint, std::uint32_t player);
	void E2ParticelManager_l_CreateEffectParticel(void* manager, std::uint32_t effectId, std::uint32_t mapPoint, std::uint32_t valency, std::uint32_t player);
	void E2ParticelManager_AddParticelGenerator(void* manager, std::uint32_t generatorId, const void* mapPoint, const void* pixelPoint, std::uint32_t chance, std::uint32_t identifier);
	void E2ParticelManager_EnableGenerator(void* manager, std::uint32_t identifier);
	void E2ParticelManager_DestroyGenerator(void* manager, std::uint32_t identifier);
	void E2ParticelManager_l_GeneratorTick(void* manager);
	void E2ParticelGenerator_GenerateParticel(void* generator, const void* mapPoint, const void* pixelPoint);
	void E2ParticelWind_ChangeWind(void* wind);
	void LogicParticelObject_SetParticelId(void* particle, std::uint32_t id);
	void LogicParticelObject_SetStartPosition(void* particle, const void* mapPoint, const void* pixelPoint, std::uint32_t height);
	void LogicParticelObject_SetTargetPosition(void* particle, const void* mapPoint, const void* pixelPoint, std::uint32_t height);
	void LogicParticelObject_CalculateMovingVector(void* particle);
	void LogicParticelObject_CalculateMovingVector(void* particle, std::uint32_t duration);
	void LogicParticelObject_CalculateParrableHeight(void* particle);
}
