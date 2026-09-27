#include "C2E2AnimalSoundManager.h"
#include "C2E2StaticDataAnimalSounds.h"
#include "C2E2StaticDataSoundFX.h"
#include "DSManager.h"

#include <cstring>

extern NDSManager::CManager* DAT_0050f6a4;
extern NC2E2::CE2StaticDataAnimalSoundManager* DAT_00568570;
extern void* DAT_00568a28;
extern void* DAT_00511554;
extern std::uint32_t DAT_00510838;

// Original data: DAT_0056841c
NC2E2::CE2AnimalSoundManager* DAT_0056841c = nullptr;

namespace NC2E2
{
    // Original function: FUN_004860ab
    CE2AnimalSoundManager::CE2AnimalSoundManager()
    {
        DAT_0056841c = this;
        std::memset(this, 0, sizeof(*this));
    }

    // Original function: FUN_004860c8
    CE2AnimalSoundManager::~CE2AnimalSoundManager()
    {
        DAT_0056841c = nullptr;
    }

    // Original function: FUN_004860d0
    void CE2AnimalSoundManager::PlayAnimalSounds(void* display)
    {
        const auto readWord = [] (const void* base, std::uint32_t offset) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, static_cast<const std::byte*>(base) + offset, sizeof(value));
            return value;
        };

        const auto readShort = [] (const void* base, std::uint32_t offset) -> std::int16_t
        {
            std::int16_t value;
            std::memcpy(&value, static_cast<const std::byte*>(base) + offset, sizeof(value));
            return value;
        };

        const auto addressAt = [] (void* base, std::uint32_t offset) -> std::byte*
        {
            const std::uint32_t address = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(base)) + offset;
            return reinterpret_cast<std::byte*>(static_cast<std::uintptr_t>(address));
        };

        if (DAT_0050f6a4 == nullptr)
            return;

        std::uint8_t initialized;
        std::memcpy(&initialized, reinterpret_cast<const std::byte*>(DAT_0050f6a4) + 4, sizeof(initialized));

        if (initialized == 0)
            return;

        const std::uint32_t update = DAT_00510838;

        if (lastUpdate == update)
            return;

        lastUpdate = update;

        // Inline initialization performed by 0042ae54.
        alignas(4) std::int16_t rectangle[4]{};

        FUN_004821c7(display, readWord(display, 0x954), readWord(display, 0x958), readWord(display, 0x964), readWord(display, 0x968), rectangle);

        // Preserve the signed 16-bit shifts and the final 16-bit stores.
        const std::uint16_t centerXBits = static_cast<std::uint16_t>(static_cast<std::int32_t>(rectangle[0]) + (static_cast<std::int32_t>(rectangle[2]) >> 1));
        const std::uint16_t centerYBits = static_cast<std::uint16_t>(static_cast<std::int32_t>(rectangle[1]) + (static_cast<std::int32_t>(rectangle[3]) >> 1));

        std::int16_t centerX;
        std::int16_t centerY;
        std::memcpy(&centerX, &centerXBits, sizeof(centerX));
        std::memcpy(&centerY, &centerYBits, sizeof(centerY));

        // 0042a98a receives false; 0040ad55 copies this point by value.
        const std::uint32_t centerPoint = static_cast<std::uint32_t>(centerXBits) | (static_cast<std::uint32_t>(centerYBits) << 16);
        const std::int32_t searchWidth = rectangle[2];
        const std::uint32_t playerId = readWord(display, 0x7C);

        for (std::uint32_t countIndex = 0; countIndex < 34u; ++countIndex)
        {
            if (animalCounts[countIndex] == 0)
                continue;

            const std::int32_t tribeType = static_cast<std::int32_t>(countIndex + 8u);
            SStaticAnimalSoundData* soundData = DAT_00568570->GetStaticAnimalSoundDataPtrByTribeType(tribeType);

            if (soundData == nullptr)
                continue;

            if (animalCounts[countIndex] < soundData->minimumCount)
                continue;

            const std::int32_t chance = FUN_004e8736() % 1000;

            if (static_cast<std::uint32_t>(chance) > soundData->probabilityThreshold)
                continue;

            std::byte* selectedAnimal = nullptr;

            for (std::uint32_t searchPass = 0; searchPass < 21u; ++searchPass)
            {
                alignas(4) std::byte iterator[0x2C];
                FUN_004402a3(iterator);

                std::int32_t animalIndex = FUN_00440329(iterator, searchPass, centerPoint, searchWidth, 0, 0);

                while (animalIndex != -1)
                {
                    std::byte* animal = addressAt(DAT_00511554, static_cast<std::uint32_t>(animalIndex) * 0x98u);
                    bool accepted = readWord(animal, 0x14) == static_cast<std::uint32_t>(tribeType);

                    if (accepted)
                    {
                        std::uint8_t displayFlags;
                        std::memcpy(&displayFlags, static_cast<const std::byte*>(display) + 0x70, sizeof(displayFlags));

                        if ((displayFlags & 4u) != 0)
                        {
                            const void* position = FUN_0042ea17(animal);
                            const void* visibilityData = FUN_004237d2(position);

                            if (playerId < 16u)
                            {
                                std::uint16_t visibility;
                                std::memcpy(&visibility, static_cast<const std::byte*>(visibilityData) + 8, sizeof(visibility));
                                accepted = (visibility & (1u << playerId)) != 0;
                            }
                        }
                    }

                    if (accepted)
                    {
                        selectedAnimal = animal;
                        break;
                    }

                    animalIndex = FUN_004403d0(iterator);
                }

                if (selectedAnimal != nullptr)
                    break;
            }

            if (selectedAnimal == nullptr)
                continue;

            auto* staticManager = static_cast<CE2StaticDataSoundFXManager*>(DAT_00568a28);
            std::byte* staticRecord = addressAt(staticManager->staticSounds, soundData->staticSoundId * 0x43Cu);

            if (readWord(staticRecord, 0x28) == 0)
                continue;

            const std::uint32_t randomValue = static_cast<std::uint32_t>(FUN_004e8736());
            const std::uint32_t variant = randomValue % readWord(staticRecord, 0x28);
            std::byte* soundEntry = addressAt(staticRecord, 0x2Cu + variant * 8u);

            const void* position = FUN_0042ea17(selectedAnimal);
            const std::int32_t horizontal = ((static_cast<std::int32_t>(readShort(position, 0)) - centerX) * 128) / rectangle[2];

            position = FUN_0042ea17(selectedAnimal);
            const std::int32_t vertical = ((static_cast<std::int32_t>(readShort(position, 2)) - centerY) * 128) / rectangle[3];

            const std::int32_t absoluteHorizontal = horizontal < 0 ? -horizontal : horizontal;
            const std::int32_t absoluteVertical = vertical < 0 ? -vertical : vertical;
            const std::int32_t distance = (absoluteHorizontal + absoluteVertical) / 2;

            if (distance > 64)
                continue;

            const std::uint32_t baseVolume = readWord(soundEntry, 4);

            if (DAT_0050f6a4->PreloadWave_IsRunning(readWord(soundEntry, 0)))
                continue;

            const std::uint32_t pan = static_cast<std::uint32_t>(horizontal + 64);
            const std::uint32_t volume = ((64u - static_cast<std::uint32_t>(distance)) * baseVolume) >> 6;

            DAT_0050f6a4->PreloadWave_Play(readWord(soundEntry, 0), volume, 1, pan, false, 0);
        }
    }
}