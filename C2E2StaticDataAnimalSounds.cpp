#include "C2E2StaticDataAnimalSounds.h"
#include "C2E2StaticDataSoundFX.h"
#include "DSManager.h"
#include "XBTokenList.h"
#include "XCrtFileAccess.h"
#include "XIniFileReader.h"

#include <cstring>

extern NDSManager::CManager* DAT_0050f6a4;
extern void* DAT_00568a28;
extern const NXBasics::SToken DAT_004f4780[];
extern std::uint8_t DAT_00568568;
extern const NXBasics::SToken* DAT_0056856c;

// Original data: DAT_00568570
NC2E2::CE2StaticDataAnimalSoundManager* DAT_00568570 = nullptr;

namespace NC2E2
{
    static int __cdecl DestroyAnimalSoundTokenList();

    // Original function: FUN_0048a969
    CE2StaticDataAnimalSoundManager::CE2StaticDataAnimalSoundManager()
    {
        DAT_00568570 = this;
        std::memset(this, 0, sizeof(*this));

        if (DAT_0050f6a4 == nullptr)
            return;

        std::uint8_t initialized;
        std::memcpy(&initialized, reinterpret_cast<const std::byte*>(DAT_0050f6a4) + 4, sizeof(initialized));

        if (initialized == 0)
            return;

        NXBasics::CIniFileReader reader("data\\engine2d\\inis\\animals\\sounds.ini", nullptr, false, nullptr, nullptr);

        if (!reader.IsLoaded())
            return;

        count = reader.GetNumberOfGroups("sounds");

        if (count == 0)
            return;

        entries = static_cast<SStaticAnimalSoundData*>(NXBasics::Runtime_AllocateWithNewHandler(count << 4, 1));
        std::memset(entries, 0, count << 4);

        SStaticAnimalSoundData* entry = entries;

        for (char* group = reader.GetFirstGroup("sounds"); group != nullptr; group = reader.GetNextGroup("sounds"))
        {
            while (reader.GetNextCommandLineInGroup() != nullptr)
            {
                if ((DAT_00568568 & 1u) == 0)
                {
                    DAT_00568568 |= 1u;
                    DAT_0056856c = DAT_004f4780;
                    NXBasics::Runtime_AtExit(DestroyAnimalSoundTokenList);
                }

                NXBasics::CTokenList tokens(DAT_0056856c);
                const std::uint32_t command = tokens.FindToken(reader.GetCurrentName());

                switch (command)
                {
                    case 0x656E6773u:
                        {
                            auto* soundManager = static_cast<CE2StaticDataSoundFXManager*>(DAT_00568a28);
                            const char* soundName = reader.GetString();
                            entry->staticSoundId = soundManager->GetStaticSoundFxIdByNamePtr(soundName);
                            break;
                        }

                    case 0x6C747274u:
                        entry->logicTribeType = reader.GetInt();
                        break;

                    case 0x6D696E63u:
                        entry->minimumCount = static_cast<std::uint32_t>(reader.GetInt());
                        break;

                    case 0x70726F62u:
                        entry->probabilityThreshold = static_cast<std::uint32_t>(reader.GetInt());
                        break;
                }
            }

            ++entry;
        }
    }

    // Original function: FUN_0048ab10
    static int __cdecl DestroyAnimalSoundTokenList()
    {
        // The original callback performs no cleanup.
        return 0;
    }

    // Original function: FUN_0048ab11
    CE2StaticDataAnimalSoundManager::~CE2StaticDataAnimalSoundManager()
    {
        if (entries != nullptr)
        {
            NXBasics::Runtime_Free(entries);
            entries = nullptr;
        }

        DAT_00568570 = nullptr;
    }

    // Original function: FUN_0048ab2f
    SStaticAnimalSoundData* CE2StaticDataAnimalSoundManager::GetStaticAnimalSoundDataPtrByTribeType(std::int32_t tribeType)
    {
        const std::uint32_t entryCount = count;

        if (entryCount == 0)
            return nullptr;

        SStaticAnimalSoundData* firstEntry = entries;

        for (std::uint32_t index = 0; index < entryCount; ++index)
        {
            if (firstEntry[index].logicTribeType == tribeType)
                return firstEntry + index;
        }

        return nullptr;
    }
}