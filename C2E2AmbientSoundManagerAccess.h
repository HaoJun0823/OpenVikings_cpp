#pragma once
#include "C2MapSectorGridCursor.h"

namespace NC2E2
{
    void* __fastcall AmbientSoundManager_Construct(void* manager, void* unusedEdx);
    void AmbientSoundManager_AllocateMemory(void* manager);
    void AmbientSoundManager_RecalculateAmbientSoundSector(void* manager, const NC2Logic::SSectorGridCursor* sector, bool patterns, bool landscapes);
    void AmbientSoundManager_RecalculateAllAmbientSoundSector(void* manager);
    void AmbientSoundManager_UpdateAmbientSounds(void* manager, const void* display);
    void AmbientSoundManager_StopAllAmbientSounds(void* manager);
}
