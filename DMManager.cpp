#include "DMManager.h"
#include "XStringMemory.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include <Windows.h>

namespace NDMManager
{
    static_assert(sizeof(void*) == 4, "The DirectMusic manager requires the original x86 layout");

    // Shared strings preserve identical pointers in the original tables.
    static constexpr char kMissionMidgard1[] = "Mission_Midgard1_Standard.sgt";
    static constexpr char kMissionMidgard2[] = "Mission_Midgard2_Standard.sgt";
    static constexpr char kMissionAddonFranken1[] = "Mission_AddOn_Franken1_Standard.sgt";
    static constexpr char kMissionAddonNordland[] = "Mission_AddOn_Nordland_Standard.sgt";
    static constexpr char kMissionAddonUnderworld[] = "Mission_AddOn_Underworld_Standard.sgt";
    static constexpr char kMissionAddonAsgard[] = "Mission_AddOn_Asgard_Standard.sgt";

    // Original data: 004fe4a8–004fe4d7.
    // Events 2–5; variants 1–3: Friendly, Neutral, Hostile.
    static constexpr const char* kThemeFileNames[] =
    {
        // Event 2
        "Theme_Viking_Friendly.sgt",
        "Theme_Viking_Neutral.sgt",
        "Theme_Viking_Hostile.sgt",

        // Event 3
        "Theme_Franken_Friendly.sgt",
        "Theme_Franken_Neutral.sgt",
        "Theme_Franken_Hostile.sgt",

        // Event 4
        "Theme_Byzanz_Friendly.sgt",
        "Theme_Byzanz_Neutral.sgt",
        "Theme_Byzanz_Hostile.sgt",

        // Event 5
        "Theme_Arabs_Friendly.sgt",
        "Theme_Arabs_Neutral.sgt",
        "Theme_Arabs_Hostile.sgt"
    };

    // Original data: 004fe4d8–004fe4e7.
    // Events 6–9; the variant is ignored.
    static constexpr const char* kAttackFileNames[] =
    {
        "Attack_Viking.sgt",
        "Attack_Franken.sgt",
        "Attack_Byzanz.sgt",
        "Attack_Arabs.sgt"
    };

    // Original data: 004fe4e8–004fe577.
    // Events 10–21; variants 4–6: Standard, Wealthy, Danger.
    static constexpr const char* kMissionFileNames[] =
    {
        // Event 10
        "Mission_Viking1_Standard.sgt",
        "Mission_Viking1_Wealthy.sgt",
        "Mission_Viking1_Danger.sgt",

        // Event 11
        "Mission_Franken1_Standard.sgt",
        "Mission_Franken1_Wealthy.sgt",
        "Mission_Franken1_Danger.sgt",

        // Event 12
        "Mission_Franken2_Standard.sgt",
        "Mission_Franken2_Wealthy.sgt",
        "Mission_Franken2_Danger.sgt",

        // Event 13
        "Mission_Byzanz1_Standard.sgt",
        "Mission_Byzanz1_Wealthy.sgt",
        "Mission_Byzanz1_Danger.sgt",

        // Event 14
        "Mission_Byzanz2_Standard.sgt",
        "Mission_Byzanz2_Wealthy.sgt",
        "Mission_Byzanz2_Danger.sgt",

        // Event 15
        "Mission_Byzanz3_Standard.sgt",
        "Mission_Byzanz3_Wealthy.sgt",
        "Mission_Byzanz3_Danger.sgt",

        // Event 16
        "Mission_Byzanz4_Standard.sgt",
        "Mission_Byzanz4_Wealthy.sgt",
        "Mission_Byzanz4_Danger.sgt",

        // Event 17
        "Mission_Arabs1_Standard.sgt",
        "Mission_Arabs1_Wealthy.sgt",
        "Mission_Arabs1_Danger.sgt",

        // Event 18
        "Mission_Arabs2_Standard.sgt",
        "Mission_Arabs2_Wealthy.sgt",
        "Mission_Arabs2_Danger.sgt",

        // Event 19
        "Mission_Arabs3_Standard.sgt",
        "Mission_Arabs3_Wealthy.sgt",
        "Mission_Arabs3_Danger.sgt",

        // Event 20
        kMissionMidgard1,
        kMissionMidgard1,
        kMissionMidgard1,

        // Event 21
        kMissionMidgard2,
        kMissionMidgard2,
        kMissionMidgard2
    };

    // Original data: 004fe578–004fe5d7.
    // Events 31–38; variants 4–6, including the original shared entries.
    static constexpr const char* kAddonMissionFileNames[] =
    {
        // Event 31
        "Mission_AddOn_Arabs1_Standard.sgt",
        "Mission_AddOn_Arabs1_Wealthy.sgt",
        "Mission_AddOn_Arabs1_Danger.sgt",

        // Event 32
        "Mission_AddOn_Arabs2_Standard.sgt",
        "Mission_AddOn_Arabs2_Wealthy.sgt",
        "Mission_AddOn_Arabs2_Danger.sgt",

        // Event 33
        kMissionAddonFranken1,
        kMissionAddonFranken1,
        kMissionAddonFranken1,

        // Event 34
        "Mission_AddOn_Franken2_Standard.sgt",
        "Mission_AddOn_Franken2_Wealthy.sgt",
        "Mission_AddOn_Franken2_Danger.sgt",

        // Event 35
        "Mission_AddOn_Franken3_Standard.sgt",
        "Mission_AddOn_Franken3_Wealthy.sgt",
        "Mission_AddOn_Franken3_Danger.sgt",

        // Event 36
        kMissionAddonNordland,
        kMissionAddonNordland,
        kMissionAddonNordland,

        // Event 37
        kMissionAddonUnderworld,
        kMissionAddonUnderworld,
        kMissionAddonUnderworld,

        // Event 38
        kMissionAddonAsgard,
        kMissionAddonAsgard,
        kMissionAddonAsgard
    };

    static_assert(sizeof(kThemeFileNames) / sizeof(kThemeFileNames[0]) == 12);
    static_assert(sizeof(kAttackFileNames) / sizeof(kAttackFileNames[0]) == 4);
    static_assert(sizeof(kMissionFileNames) / sizeof(kMissionFileNames[0]) == 36);
    static_assert(sizeof(kAddonMissionFileNames) / sizeof(kAddonMissionFileNames[0]) == 24);

    // Original data: 004fe484–004fe4a7; volume-recovery delays for events 22–30.
    static constexpr std::uint32_t kVolumeRecoveryDelayMs[9] =
    {
        2800,   // Event 22
        3700,   // Event 23
        5000,   // Event 24
        3200,   // Event 25
        3300,   // Event 26
        8200,   // Event 27
        8000,   // Event 28
        2800,   // Event 29
        3000    // Event 30
    };

    static_assert(sizeof(kVolumeRecoveryDelayMs) == 0x24);

    template<typename T>
    static T ReadField(const void* object, std::size_t offset)
    {
        T value;
        std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
        return value;
    }

    template<typename T>
    static void WriteField(void* object, std::size_t offset, T value)
    {
        std::memcpy(static_cast<std::byte*>(object) + offset, &value, sizeof(value));
    }

    template<typename T>
    static T DriverFunction(void* manager, std::size_t offset)
    {
        const void* table = ReadField<void*>(manager, 0x24);
        return ReadField<T>(table, offset);
    }

    // Signatures describe the arguments observed at the Windows call sites.
    using LoadCachedObjectFn = std::uint8_t(__stdcall*)(void*, std::uint32_t, void*, void*, const char*);
    using CreateAudiopathFn = std::uint8_t(__stdcall*)(void*, const std::uint32_t*, void*, void*);
    using ActivateAudiopathFn = std::uint8_t(__stdcall*)(void*, void*, std::uint32_t);
    using SetAudiopathVolumeFn = std::uint8_t(__stdcall*)(void*, void*, std::int32_t, std::uint32_t);
    using StartSegmentFn = std::uint8_t(__stdcall*)(void*, void*, void*, std::uint32_t, std::uint32_t, std::int32_t, std::uint32_t);
    using ResetSegmentFn = void(__stdcall*)(void*, void*, std::uint32_t);
    using GetPlaybackStateFn = std::uint8_t(__stdcall*)(void*, void*, std::uint8_t*);
    using DriverSlot6CFn = void(__stdcall*)(void*, void*);

    static void SetErrorMessage(void* manager, const char* message)
    {
        char* previous = ReadField<char*>(manager, 0x220);

        if (previous != nullptr)
        {
            // Match the allocation used by the reconstructed StringDuplicate().
            ::operator delete(previous);
        }

        WriteField<char*>(manager, 0x220, NXBasics::StringDuplicate(message));
    }

    // Original function: FUN_004039d8
    void ResetAllSegments(void* manager)
    {
        auto* entries = static_cast<std::byte*>(manager) + 0x3C;

        for (std::uint32_t index = 0; index < 20; ++index)
        {
            std::byte* entry = entries + index * 0x18u;

            if (ReadField<std::uint8_t>(entry, 0x00) != 0)
            {
                DriverFunction<ResetSegmentFn>(manager, 0x58)(ReadField<void*>(manager, 0x30), ReadField<void*>(entry, 0x04), 0);
            }
        }

        WriteField<std::uint32_t>(manager, 0x14, 0);
        WriteField<std::uint32_t>(manager, 0x18, 0);
    }

    // Original function: FUN_00403a08
    std::uint8_t PrepareEvent(void* manager, std::int32_t eventId, std::uint32_t eventParameter)
    {
        if (eventId <= 1 || eventId >= 39)
        {
            return 0;
        }

        if (eventId >= 22 && eventId < 31)
        {
            return 1;
        }

        if (FindSegmentEntry(manager, eventId, eventParameter) != nullptr)
        {
            return 1;
        }

        const char* fileName = GetEventFileName(eventId, eventParameter);

        if (fileName == nullptr)
        {
            return 0;
        }

        auto* entries = static_cast<std::byte*>(manager) + 0x3C;
        std::byte* entry = nullptr;

        for (std::uint32_t index = 0; index < 20; ++index)
        {
            std::byte* candidate = entries + index * 0x18u;

            if (ReadField<std::uint8_t>(candidate, 0x00) == 0)
            {
                entry = candidate;
                break;
            }
        }

        if (entry == nullptr)
        {
            SetErrorMessage(manager, "DMManager: No free segment entry found!");
            return 0;
        }

        std::memset(entry, 0, 0x18);

        WriteField<std::int32_t>(entry, 0x10, eventId);
        WriteField<std::uint32_t>(entry, 0x14, eventParameter);
        WriteField<std::uint32_t>(entry, 0x08, 0);
        WriteField<const char*>(entry, 0x0C, fileName);

        if (DriverFunction<LoadCachedObjectFn>(manager, 0x20)(ReadField<void*>(manager, 0x30), 0, entry + 0x08, entry + 0x04, ReadField<const char*>(manager, 0x21C)) == 0)
        {
            SetErrorMessage(manager, "DMManager: geLoadCachedObject failed.");
            return 0;
        }

        WriteField<std::uint8_t>(entry, 0x00, 1);

        const std::int32_t mode = ReadField<std::int32_t>(manager, 0x10);
        const std::uint32_t configuration[2] =
        {
            mode == 2 ? 1u : 3u,
            ReadField<std::uint32_t>(manager, 0x34)
        };

        if (ReadField<void*>(manager, 0x38) != nullptr)
        {
            return 1;
        }

        void* audiopathOutput = static_cast<std::byte*>(manager) + 0x38;
        std::uint8_t created = 0;

        if (mode != 2)
        {
            created = DriverFunction<CreateAudiopathFn>(manager, 0x34)(ReadField<void*>(manager, 0x30), configuration, audiopathOutput, ReadField<void*>(entry, 0x04));
        }

        if (created == 0)
        {
            created = DriverFunction<CreateAudiopathFn>(manager, 0x34)(ReadField<void*>(manager, 0x30), configuration, audiopathOutput, nullptr);
        }

        if (created == 0)
        {
            SetErrorMessage(manager, "DMManager: geCreateAudiopath failed.");
            DriverFunction<DriverSlot6CFn>(manager, 0x6C)(ReadField<void*>(manager, 0x30), ReadField<void*>(entry, 0x04));
            return 0;
        }

        if (DriverFunction<ActivateAudiopathFn>(manager, 0x38)(ReadField<void*>(manager, 0x30), ReadField<void*>(manager, 0x38), 1) == 0)
        {
            SetErrorMessage(manager, "DMManager: geActivateAudiopath failed.");
            DriverFunction<DriverSlot6CFn>(manager, 0x6C)(ReadField<void*>(manager, 0x30), ReadField<void*>(entry, 0x04));
            return 0;
        }

        // Preserve the original start-then-reset sequence and ignored result.
        StartEvent(manager, eventId, eventParameter);
        ResetAllSegments(manager);
        return 1;
    }

    // Original function: FUN_00403c0e
    std::uint8_t StartEvent(void* manager, std::int32_t eventId, std::uint32_t eventParameter)
    {
        if (eventId <= 1 || eventId >= 39)
        {
            return 0;
        }

        if (ReadField<std::int32_t>(manager, 0x14) == eventId && ReadField<std::uint32_t>(manager, 0x18) == eventParameter)
        {
            return 1;
        }

        const bool isVolumeEvent = eventId >= 22 && eventId < 31;
        std::byte* entry = nullptr;

        if (!isVolumeEvent)
        {
            entry = FindSegmentEntry(manager, eventId, eventParameter);

            if (entry == nullptr)
            {
                return 0;
            }

            std::uint8_t playing = 0;

            if (DriverFunction<GetPlaybackStateFn>(manager, 0x5C)(ReadField<void*>(manager, 0x30), ReadField<void*>(entry, 0x04), &playing) == 0)
            {
                SetErrorMessage(manager, "DMManager: geGetPlaybackStateOfSegment failed.");
            }

            // The original examines the output even when the query failed.
            if (playing != 0)
            {
                return 1;
            }
        }

        if (isVolumeEvent)
        {
            void* audiopath = ReadField<void*>(manager, 0x38);

            if (audiopath != nullptr && ReadField<std::uint8_t>(manager, 0x05) == 0)
            {
                if (DriverFunction<SetAudiopathVolumeFn>(manager, 0x3C)(ReadField<void*>(manager, 0x30), audiopath, -2000, 300) == 0)
                {
                    SetErrorMessage(manager, "DMManager: geSetVolumeOfAudiopath failed.");
                    return 0;
                }

                WriteField<std::uint32_t>(manager, 0x08, 0);
                WriteField<std::uint8_t>(manager, 0x05, 1);
            }

            const std::uint32_t delay = kVolumeRecoveryDelayMs[eventId - 22];
            const std::uint32_t now = static_cast<std::uint32_t>(::timeGetTime());

            WriteField<std::uint32_t>(manager, 0x08, now + delay);
            return 1;
        }

        void* audiopath = ReadField<void*>(manager, 0x38);

        if (audiopath == nullptr)
        {
            return 1;
        }

        if (DriverFunction<StartSegmentFn>(manager, 0x54)(ReadField<void*>(manager, 0x30), audiopath, ReadField<void*>(entry, 0x04), 0x2000, 0, -1, 1) == 0)
        {
            SetErrorMessage(manager, "DMManager: geStartSegmentPlayback failed.");
            return 0;
        }

        WriteField<std::int32_t>(manager, 0x14, eventId);
        WriteField<std::uint32_t>(manager, 0x18, eventParameter);
        return 1;
    }

    // Original function: FUN_00403d6c
    const char* GetEventFileName(std::int32_t eventId, std::uint32_t variant)
    {
        if (eventId >= 2 && eventId <= 5)
        {
            const std::uint32_t variantIndex = variant >= 1u && variant <= 3u ? variant - 1u : 0u;
            const std::uint32_t eventIndex = static_cast<std::uint32_t>(eventId - 2);

            return kThemeFileNames[eventIndex * 3u + variantIndex];
        }

        if (eventId >= 6 && eventId <= 9)
        {
            return kAttackFileNames[static_cast<std::uint32_t>(eventId - 6)];
        }

        if (eventId >= 10 && eventId <= 21)
        {
            const std::uint32_t variantIndex = variant >= 4u && variant <= 6u ? variant - 4u : 0u;
            const std::uint32_t eventIndex = static_cast<std::uint32_t>(eventId - 10);

            return kMissionFileNames[eventIndex * 3u + variantIndex];
        }

        if (eventId >= 31 && eventId <= 38)
        {
            const std::uint32_t variantIndex = variant >= 4u && variant <= 6u ? variant - 4u : 0u;
            const std::uint32_t eventIndex = static_cast<std::uint32_t>(eventId - 31);

            return kAddonMissionFileNames[eventIndex * 3u + variantIndex];
        }

        return nullptr;
    }

    // Original function: FUN_00403e10
    static std::byte* FindSegmentEntry(void* manager, std::int32_t eventId, std::uint32_t eventParameter)
    {
        auto* entries = static_cast<std::byte*>(manager) + 0x3C;

        for (std::uint32_t index = 0; index < 20; ++index)
        {
            std::byte* entry = entries + index * 0x18u;

            if (ReadField<std::uint8_t>(entry, 0x00) == 0)
            {
                continue;
            }

            if (ReadField<std::int32_t>(entry, 0x10) == eventId && ReadField<std::uint32_t>(entry, 0x14) == eventParameter)
            {
                return entry;
            }
        }

        return nullptr;
    }

    // Original function: FUN_004038bd
    std::uint8_t ProcessEvent(void* manager, std::int32_t eventId, std::uint32_t eventParameter, std::int32_t mode)
    {
        auto* managerBytes = static_cast<std::byte*>(manager);

        std::uint8_t stateFlag;
        std::memcpy(&stateFlag, managerBytes + 0x04, sizeof(stateFlag));

        if (stateFlag == 0)
        {
            return 1;
        }

        std::int32_t resolvedEventId = eventId;

        if ((eventId >= 10 && eventId < 22) || (eventId >= 31 && eventId < 39))
        {
            std::memcpy(managerBytes + 0x0C, &eventId, sizeof(eventId));
        }
        else if (eventId == 1)
        {
            std::memcpy(&resolvedEventId, managerBytes + 0x0C, sizeof(resolvedEventId));
        }

        std::uint8_t result = PrepareEvent(manager, resolvedEventId, eventParameter);

        if (mode == 1)
        {
            ResetAllSegments(manager);
        }
        else if (mode == 2)
        {
            // Read the current volume after the first helper call.
            std::int32_t volume;
            std::memcpy(&volume, managerBytes + 0x1C, sizeof(volume));

            if (volume > 0)
            {
                result = StartEvent(manager, resolvedEventId, eventParameter);
            }
        }

        return result;
    }
}