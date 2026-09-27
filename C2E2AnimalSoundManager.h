#pragma once

#include <cstddef>
#include <cstdint>

namespace NC2E2
{
    class CE2AnimalSoundManager
    {
    public:
        CE2AnimalSoundManager();
        ~CE2AnimalSoundManager();

        void PlayAnimalSounds(void* display);

        std::uint32_t lastUpdate;
        std::uint32_t animalCounts[34];
    };

    static_assert(sizeof(CE2AnimalSoundManager) == 0x8C);
    static_assert(offsetof(CE2AnimalSoundManager, animalCounts) == 4);
}