#include "C2E2StaticDataSoundFX.h"
#include "C2LifetimeGlobals.h"
#include "C2E2StaticDataGroups.h"
#include "DSManager.h"
#include "XCrtFileAccess.h"
#include "XFile.h"
#include "XIniFileReader.h"
#include "XBTokenList.h"

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstring>


namespace
{
    // Complete Windows token array 004F4A20..004F4A57, including its terminator.
    const NXBasics::SToken gSoundFxTokens[] = {
        { "Name", 0x6E616D65u },
        { "SFX", 0x73667864u },
        { "PatternGroup", 0x70677270u },
        { "LandscapeGroup", 0x6C6E6467u },
        { "LogicSoundType", 0x6C737479u },
        { "MusicType", 0x6D747970u },
        { nullptr, 0 }
    };
    // Complete lazy token state 00568A20..00568A27; both fields start in PE BSS.
    const NXBasics::SToken* gSoundFxTokenTable = nullptr;
    std::uint8_t gSoundFxTokenInitFlags = 0;
}

namespace NC2E2
{
    static int __cdecl DestroySoundTokenList();

    // Original functions: FUN_0042b679, FUN_0042b689
    std::uint32_t CE2StaticDataSoundFXManager::l_GetSoundCommand(const char* name)
    {
        NXBasics::CTokenList tokens(gSoundFxTokenTable);
        return tokens.FindToken(name);
    }

    // Original function: FUN_0048dc74
    CE2StaticDataSoundFXManager::CE2StaticDataSoundFXManager()
    {
        sTheObjectPtr = this;

        std::memset(this, 0, sizeof(*this));

        for (std::uint32_t& value : patternGroupAmbientIds)
            value = 0xFFFFFFFFu;

        for (std::uint32_t& value : landscapeGroupAmbientIds)
            value = 0xFFFFFFFFu;

        for (std::uint32_t& value : staticSoundIdsByType)
            value = 0xFFFFFFFFu;

        if (NDSManager::CManager::sTheObjectPtr != nullptr)
        {
            std::uint8_t initialized;
            std::memcpy(&initialized, reinterpret_cast<const std::byte*>(NDSManager::CManager::sTheObjectPtr) + 4, sizeof(initialized));

            if (initialized != 0)
                l_LoadSoundFxFromIniFile("data\\engine2d\\inis\\soundfx\\soundfx.ini");
        }
    }

    // Original function: FUN_0048dccf
    CE2StaticDataSoundFXManager::~CE2StaticDataSoundFXManager()
    {
        if (staticSounds != nullptr)
        {
            NXBasics::Runtime_Free(staticSounds);
            staticSounds = nullptr;
        }

        if (jingles != nullptr)
        {
            NXBasics::Runtime_Free(jingles);
            jingles = nullptr;
        }

        if (ambientSounds != nullptr)
        {
            NXBasics::Runtime_Free(ambientSounds);
            ambientSounds = nullptr;
        }

        sTheObjectPtr = nullptr;
    }

    // Original function: FUN_0048dd23
    void CE2StaticDataSoundFXManager::l_LoadSoundFxFromIniFile(const char* fileName)
    {
        NXBasics::CIniFileReader reader(fileName, nullptr, false, nullptr, nullptr);

        if (!reader.IsLoaded())
            return;

        if ((gSoundFxTokenInitFlags & 1u) == 0)
        {
            gSoundFxTokenInitFlags |= 1u;
            gSoundFxTokenTable = gSoundFxTokens;
            NXBasics::Runtime_AtExit(DestroySoundTokenList);
        }

        const auto addressAt = [] (void* base, std::uint32_t offset) -> std::byte*
        {
            const std::uint32_t address = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(base)) + offset;
            return reinterpret_cast<std::byte*>(static_cast<std::uintptr_t>(address));
        };

        const auto readWord = [] (const void* source) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, source, sizeof(value));
            return value;
        };

        const auto writeWord = [] (void* destination, std::uint32_t value)
        {
            std::memcpy(destination, &value, sizeof(value));
        };

        std::uint32_t groupCount = reader.GetNumberOfGroups("SoundFXStatic");

        if (groupCount != 0)
        {
            const std::uint32_t allocationBytes = groupCount * 0x43Cu;
            staticSounds = NXBasics::Runtime_AllocateWithNewHandler(allocationBytes, 1);
            std::memset(staticSounds, 0, allocationBytes);

            for (char* group = reader.GetFirstGroup("SoundFXStatic"); group != nullptr; group = reader.GetNextGroup("SoundFXStatic"))
            {
                std::byte* record = addressAt(staticSounds, staticSoundCount * 0x43Cu);
                ++staticSoundCount;

                while (reader.GetNextCommandLineInGroup() != nullptr)
                {
                    const std::uint32_t command = l_GetSoundCommand(reader.GetCurrentName());

                    switch (command)
                    {
                        case 0x6C737479u:
                            {
                                const std::uint32_t recordIndex = staticSoundCount - 1u;
                                const std::uint32_t type = static_cast<std::uint32_t>(reader.GetInt());

                                // Writes staticSoundIdsByType using the original unchecked byte offset.
                                writeWord(addressAt(this, 0x08u + type * 4u), recordIndex);
                                break;
                            }

                        case 0x6E616D65u:
                            std::strcpy(reinterpret_cast<char*>(record), reader.GetString());
                            break;

                        case 0x73667864u:
                            {
                                const char* soundFileName = reader.GetString();

                                if (!NXBasics::CFile::FileSystem_Tool_FileExists(soundFileName, true))
                                    break;

                                std::uint32_t identifier;

                                if (!NDSManager::CManager::sTheObjectPtr->PreloadWave_Load(soundFileName, &identifier))
                                    break;

                                const std::uint32_t value = static_cast<std::uint32_t>(reader.GetInt());
                                const std::uint32_t soundCount = readWord(record + 0x28);

                                writeWord(addressAt(record, 0x2Cu + soundCount * 8u), identifier);
                                writeWord(addressAt(record, 0x30u + soundCount * 8u), value);
                                writeWord(record + 0x28, soundCount + 1u);
                                break;
                            }
                    }
                }
            }
        }

        groupCount = reader.GetNumberOfGroups("SoundFXJingle");

        if (groupCount != 0)
        {
            const std::uint32_t allocationBytes = groupCount << 7;
            jingles = static_cast<SJingleSoundData*>(NXBasics::Runtime_AllocateWithNewHandler(allocationBytes, 1));
            std::memset(jingles, 0, allocationBytes);

            for (char* group = reader.GetFirstGroup("SoundFXJingle"); group != nullptr; group = reader.GetNextGroup("SoundFXJingle"))
            {
                std::byte* record = addressAt(jingles, jingleCount << 7);
                ++jingleCount;

                while (reader.GetNextCommandLineInGroup() != nullptr)
                {
                    const std::uint32_t command = l_GetSoundCommand(reader.GetCurrentName());

                    switch (command)
                    {
                        case 0x6D747970u:
                            writeWord(record, static_cast<std::uint32_t>(reader.GetInt()));
                            break;

                        case 0x73667864u:
                            {
                                char* soundFileName = reinterpret_cast<char*>(record + 4);
                                std::strcpy(soundFileName, reader.GetString());

                                if (NXBasics::CFile::FileSystem_Tool_FileExists(soundFileName, true))
                                    writeWord(record + 0x7C, static_cast<std::uint32_t>(reader.GetInt()));

                                break;
                            }
                    }
                }
            }
        }

        groupCount = reader.GetNumberOfGroups("SoundFXAmbient");

        if (groupCount != 0)
        {
            const std::uint32_t allocationBytes = groupCount * 0x1570u;
            ambientSounds = NXBasics::Runtime_AllocateWithNewHandler(allocationBytes, 1);
            std::memset(ambientSounds, 0, allocationBytes);

            alignas(4) std::byte soundData[0x88];

            for (char* group = reader.GetFirstGroup("SoundFXAmbient"); group != nullptr; group = reader.GetNextGroup("SoundFXAmbient"))
            {
                std::byte* record = addressAt(ambientSounds, ambientSoundCount * 0x1570u);
                ++ambientSoundCount;

                while (reader.GetNextCommandLineInGroup() != nullptr)
                {
                    const std::uint32_t command = l_GetSoundCommand(reader.GetCurrentName());

                    switch (command)
                    {
                        case 0x6C6E6467u:
                            {
                                const char* groupName = reader.GetString();
                                const std::uint32_t recordIndex = ambientSoundCount - 1u;
                                const std::uint32_t groupIndex = static_cast<std::uint32_t>(StaticDataGroups_FindIndex(gE2StaticDataLandscapesManager, groupName));

                                writeWord(addressAt(this, 0x388u + groupIndex * 4u), recordIndex);
                                break;
                            }

                        case 0x6E616D65u:
                            std::strcpy(reinterpret_cast<char*>(record), reader.GetString());
                            break;

                        case 0x70677270u:
                            {
                                const char* groupName = reader.GetString();
                                const std::uint32_t recordIndex = ambientSoundCount - 1u;
                                const std::uint32_t groupIndex = static_cast<std::uint32_t>(StaticDataGroups_FindIndex(gE2StaticDataPatternManager, groupName));

                                writeWord(addressAt(this, 0x1F8u + groupIndex * 4u), recordIndex);
                                break;
                            }

                        case 0x73667864u:
                            {
                                char* soundFileName = reinterpret_cast<char*>(soundData);
                                std::strcpy(soundFileName, reader.GetString());

                                if (!NXBasics::CFile::FileSystem_Tool_FileExists(soundFileName, true))
                                    break;

                                std::uint32_t identifier;

                                if (!NDSManager::CManager::sTheObjectPtr->PreloadWave_Load(soundFileName, &identifier))
                                    break;

                                writeWord(soundData + 0x78, identifier);
                                writeWord(soundData + 0x80, static_cast<std::uint32_t>(reader.GetInt()));
                                writeWord(soundData + 0x7C, static_cast<std::uint32_t>(reader.GetInt()));
                                writeWord(soundData + 0x84, static_cast<std::uint32_t>(reader.GetInt()));

                                l_Ambient_AddSound(record, soundData);
                                break;
                            }
                    }
                }
            }
        }
    }

    // Original function: FUN_0048e21c
    void CE2StaticDataSoundFXManager::l_Ambient_AddSound(void* ambientRecord, const void* soundData)
    {
        auto* record = static_cast<std::byte*>(ambientRecord);
        const auto* source = static_cast<const std::byte*>(soundData);

        std::uint32_t count;
        std::memcpy(&count, record + 0x2C, sizeof(count));

        if (count >= 40u)
            return;

        std::byte* destination = record + 0x30 + count * 0x88u;

        // Preserve the forward DWORD copy performed by REP MOVSD.
        for (std::uint32_t offset = 0; offset < 0x88u; offset += 4u)
        {
            std::uint32_t value;
            std::memcpy(&value, source + offset, sizeof(value));
            std::memcpy(destination + offset, &value, sizeof(value));
        }

        std::memcpy(&count, record + 0x2C, sizeof(count));
        ++count;
        std::memcpy(record + 0x2C, &count, sizeof(count));

        std::uint32_t contribution;
        std::memcpy(&contribution, source + 0x80, sizeof(contribution));

        std::uint32_t total;
        std::memcpy(&total, record + 0x28, sizeof(total));
        total += contribution;
        std::memcpy(record + 0x28, &total, sizeof(total));
    }

    // Original function: FUN_0048e19a
    std::uint32_t CE2StaticDataSoundFXManager::GetStaticSoundFxIdByNamePtr(const char* name)
    {
        if (NDSManager::CManager::sTheObjectPtr == nullptr)
            return 0;

        std::uint8_t initialized;
        std::memcpy(&initialized, reinterpret_cast<const std::byte*>(NDSManager::CManager::sTheObjectPtr) + 4, sizeof(initialized));

        if (initialized == 0 || staticSoundCount == 0)
            return 0;

        std::uint32_t offset = 0;

        for (std::uint32_t index = 0; index < staticSoundCount; ++index)
        {
            const std::uint32_t recordAddress = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(staticSounds)) + offset;
            const auto* recordName = reinterpret_cast<const char*>(static_cast<std::uintptr_t>(recordAddress));

            if (NXBasics::Runtime_CompareStringsIgnoreCase(recordName, name) == 0)
                return index;

            offset += 0x43Cu;
        }

        return 0;
    }

    // Original function: FUN_0048e199
    static int __cdecl DestroySoundTokenList()
    {
        // The original callback performs no cleanup.
        return 0;
    }
}
