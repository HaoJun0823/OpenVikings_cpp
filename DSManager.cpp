#include "DSManager.h"
#include "XCrtFileAccess.h"
#include "XFile.h"
#include "MasterControlProgram.h"
#include "MasterPropertyManager.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

extern void* DAT_0050f698;
extern void* DAT_00568a28;

// Original data: DAT_0050f6a4
NDSManager::CManager* DAT_0050f6a4 = nullptr;

namespace NDSManager
{
    // Original function: FUN_0040471f
    CManager::CManager(void* window, bool initialize)
    {
        std::memset(this, 0, sizeof(*this));

        DAT_0050f6a4 = this;
        mWindow = window;
        mMainVolume = 100;
        mNextSequence = 1;
        mFrequencyPercent = 100;

        if (initialize)
            Quality_Set(1);
    }

    // Original function: FUN_00404762
    CManager::~CManager()
    {
        if (mInitialized != 0)
        {
            for (std::uint32_t identifier = 0; identifier < 1000u; ++identifier)
                PreloadWave_Unload(identifier);

            FireAndForget_StopAll();
            l_Device_Exit();
        }

        DAT_0050f6a4 = nullptr;
    }

    // Original function: FUN_00404797
    void CManager::System_Update()
    {
        static_assert(offsetof(CManager, mFireAndForgetWaves) == 0x3E9C);
        static_assert(offsetof(CManager, mVolumeTransitions) == 0x41C0);

        if (mInitialized == 0)
            return;

        for (SFireAndForgetWave& slot : mFireAndForgetWaves)
        {
            if (slot.active != 0)
            {
                if (slot.sound == nullptr || Sound_IsPlaying(slot.sound) == 0)
                    slot.Exit();
            }
        }

        for (SVolumeTransition& transition : mVolumeTransitions)
        {
            if (transition.active == 0)
                continue;

            if (!PreloadWave_IsRunning(transition.identifier))
            {
                transition.active = 0;
                continue;
            }

            const std::int32_t targetVolume = transition.targetVolume;
            const double target = static_cast<double>(targetVolume);
            const double current = transition.currentVolume;
            const double rate = transition.changePerMillisecond;

            // The negated comparison also advances an unordered current value.
            const bool advance = !(current + 1.0 > target) || current - 1.0 >= target;

            if (!advance)
            {
                // Both comparisons are false for zero and NaN.
                if (rate > 0.0 || rate < 0.0)
                {
                    PreloadWave_ChangeVolume(transition.identifier, static_cast<std::uint32_t>(targetVolume), false, 0u);
                    transition.changePerMillisecond = 0.0;

                    if (transition.targetVolume == 0)
                        transition.active = 0;
                }

                continue;
            }

            const std::uint32_t now = ::timeGetTime();
            const std::uint32_t elapsed = now - transition.lastUpdate;
            transition.lastUpdate = now;

            if (elapsed == 0)
                continue;

            const double next = static_cast<double>(elapsed) * rate + current;
            transition.currentVolume = next;

            if (rate > 0.0)
            {
                if (next > target)
                    transition.currentVolume = target;
            }
            else
            {
                if (!(next >= target))
                    transition.currentVolume = target;
            }

            if (transition.currentVolume > 1.0)
            {
                std::uint32_t volume = 0;

                // Avoid an out-of-range floating-to-integer conversion.
                // The original invalid conversion supplies a zero low DWORD.
                if (transition.currentVolume < 9223372036854775808.0)
                    volume = static_cast<std::uint32_t>(static_cast<std::int64_t>(transition.currentVolume));

                PreloadWave_ChangeVolume(transition.identifier, volume, false, 0u);
            }
        }
    }

    // Original function: FUN_004048d7
    bool CManager::Quality_Set(std::int32_t quality)
    {
        if (mInitialized != 0)
        {
            FireAndForget_StopAll();

            for (SPreloadWave& entry : mPreloadWaves)
            {
                if (entry.active != 0)
                    entry.FreeData();
            }

            l_Device_Exit();
        }

        mQuality = quality;

        if (!l_Device_Init())
            return false;

        for (SPreloadWave& entry : mPreloadWaves)
        {
            if (entry.active == 0)
                continue;

            char resolvedFileName[0x104];

            if (!NXBasics::CFile::FileSystem_Tool_GetRealFilename(entry.fileName, resolvedFileName))
            {
                entry.Exit();
                continue;
            }

            if (!ls_WaveFile_Load(mSoundDevice, resolvedFileName, &entry.sound, &entry.waveData))
            {
                entry.Exit();
                continue;
            }

            if (entry.sound != nullptr)
            {
                const std::uint32_t attenuationBits = (mMainVolume - 100u) * 5u * 4u;
                Sound_SetVolumeAndPan(entry.sound, NXBasics::Runtime_TimeSigned32(attenuationBits), 0);
            }
        }

        return true;
    }

    // Original function: FUN_0040499b
    bool ls_WaveFile_Load(void* soundDevice, const char* fileName, void** sound, std::uint8_t** waveData)
    {
        *sound = nullptr;
        *waveData = nullptr;

        if (!NXBasics::CFile::FileSystem_Tool_FileExists(fileName, true))
            return false;

        NXBasics::CFile file(fileName, true);
        const std::uint32_t fileSize = file.GetSize();

        *waveData = static_cast<std::uint8_t*>(NXBasics::Runtime_AllocateWithNewHandler(fileSize, 1));
        file.Read(*waveData, fileSize);

        if (fileSize > 0x38u)
        {
            const auto readTag = [] (const std::uint8_t* source) -> std::uint32_t
            {
                std::uint32_t value;
                std::memcpy(&value, source, sizeof(value));
                return value;
            };

            if (readTag(*waveData) == 0x46464952u && readTag(*waveData + 0x08) == 0x45564157u && readTag(*waveData + 0x0C) == 0x20746D66u)
            {
                static_assert(sizeof(WAVEFORMATEX) == 0x12);
                static_assert(offsetof(WAVEFORMATEX, cbSize) == 0x10);

                WAVEFORMATEX format;
                std::memset(&format, 0, sizeof(format));

                std::memcpy(&format.wFormatTag, *waveData + 0x14, sizeof(format.wFormatTag));
                std::memcpy(&format.nChannels, *waveData + 0x16, sizeof(format.nChannels));
                std::memcpy(&format.nSamplesPerSec, *waveData + 0x18, sizeof(format.nSamplesPerSec));
                std::memcpy(&format.nAvgBytesPerSec, *waveData + 0x1C, sizeof(format.nAvgBytesPerSec));
                std::memcpy(&format.nBlockAlign, *waveData + 0x20, sizeof(format.nBlockAlign));
                std::memcpy(&format.wBitsPerSample, *waveData + 0x22, sizeof(format.wBitsPerSample));

                if (format.wFormatTag == WAVE_FORMAT_PCM)
                {
                    // The original assumes sample data starts at the fixed offset 0x38.
                    const std::int32_t result = Sound_CreateFromMemory(soundDevice, sound, *waveData + 0x38, fileSize - 0x38u, &format, 0xE0u);

                    if (result < 0)
                        *sound = nullptr;
                }
            }
        }

        if (*sound == nullptr)
        {
            NXBasics::Runtime_Free(*waveData);
            *waveData = nullptr;
            return false;
        }

        return true;
    }

    // Original function: FUN_00404aac
    void CManager::MainVolume_Set(std::int32_t volume)
    {
        if (volume < 0)
            volume = 0;
        else if (volume > 100)
            volume = 100;

        mMainVolume = static_cast<std::uint32_t>(volume);

        if (mInitialized == 0)
            return;

        for (std::uint32_t identifier = 0; identifier < 1000u; ++identifier)
        {
            SPreloadWave& entry = mPreloadWaves[identifier];

            if (entry.active == 0)
                continue;

            bool transitionActive = false;

            for (const SVolumeTransition& transition : mVolumeTransitions)
            {
                if (transition.active != 0 && transition.identifier == identifier)
                {
                    transitionActive = true;
                    break;
                }
            }

            if (transitionActive)
                continue;

            if (entry.sound != nullptr)
            {
                const std::uint32_t attenuationBits = (mMainVolume - 100u) * 5u * 4u;
                Sound_SetVolumeAndPan(entry.sound, NXBasics::Runtime_TimeSigned32(attenuationBits), 0);
            }
        }

        for (SFireAndForgetWave& slot : mFireAndForgetWaves)
        {
            if (slot.active != 0 && slot.sound != nullptr)
            {
                const std::uint32_t attenuationBits = (mMainVolume - 100u) * 5u * 4u;
                Sound_SetVolumeAndPan(slot.sound, NXBasics::Runtime_TimeSigned32(attenuationBits), 0);
            }
        }
    }

    // Original function: FUN_00404b62
    // Descriptive reconstruction name; the original method name is unknown.
    bool CManager::l_Device_Init()
    {
        std::uint32_t channels = 1;
        std::uint32_t samplesPerSecond = 11025;
        std::uint32_t bitsPerSample = 8;

        if (mQuality == 1)
        {
            samplesPerSecond = 22050;
            bitsPerSample = 16;
        }
        else if (mQuality == 2)
        {
            channels = 2;
            samplesPerSecond = 44100;
            bitsPerSample = 16;
        }

        void* wrapper = NXBasics::Runtime_AllocateWithNewHandler(4u, 1);

        if (wrapper != nullptr)
        {
            // Inline the four-byte initialization from FUN_004dffdf.
            IDirectSound8* device = nullptr;
            std::memcpy(wrapper, &device, sizeof(device));
        }

        mSoundDevice = wrapper;

        // The original calls the initializer even when allocation returned null.
        const std::int32_t result = SoundDevice_Initialize(wrapper, mWindow, 2u, channels, samplesPerSecond, bitsPerSample);

        if (result < 0)
            return false;

        mInitialized = 1;
        return true;
    }

    // Original function: FUN_00404bdb
    // Descriptive reconstruction name; the original method name is unknown.
    void CManager::l_Device_Exit()
    {
        void* deviceToRelease = mSoundDevice;

        if (deviceToRelease != nullptr)
        {
            auto* storage = static_cast<std::byte*>(deviceToRelease);

            // Inline the device-wrapper destruction from FUN_0040523c.
            IDirectSound8* device;
            std::memcpy(&device, storage, sizeof(device));

            if (device != nullptr)
            {
                device->Release();

                device = nullptr;
                std::memcpy(storage, &device, sizeof(device));
            }

            NXBasics::Runtime_Free(deviceToRelease);
        }

        mSoundDevice = nullptr;
        mInitialized = 0;
    }

    // Original function: FUN_00404bff
    bool CManager::PreloadWave_Load(const char* fileName, std::uint32_t* identifier)
    {
        *identifier = 0xFFFFFFFFu;

        if (mInitialized == 0)
            return false;

        char resolvedFileName[0x104];

        if (!NXBasics::CFile::FileSystem_Tool_GetRealFilename(fileName, resolvedFileName))
            return false;

        std::uint32_t index = 0;

        for (; index < 1000u; ++index)
        {
            const SPreloadWave& entry = mPreloadWaves[index];

            // The original stops searching at the first inactive entry.
            if (entry.active == 0)
                break;

            if (NXBasics::Runtime_CompareStringsIgnoreCase(fileName, entry.fileName) == 0)
            {
                *identifier = index;
                return true;
            }
        }

        if (index == 1000u)
            return false;

        SPreloadWave& entry = mPreloadWaves[index];
        std::memset(&entry, 0, sizeof(entry));

        if (!ls_WaveFile_Load(mSoundDevice, resolvedFileName, &entry.sound, &entry.waveData))
        {
            entry.Exit();
            return false;
        }

        if (entry.sound != nullptr)
        {
            const std::uint32_t attenuationBits = (mMainVolume - 100u) * 5u * 4u;
            Sound_SetVolumeAndPan(entry.sound, NXBasics::Runtime_TimeSigned32(attenuationBits), 0);
        }

        entry.active = 1;

        // Inline FUN_00410b59 with its original runtime allocation semantics.
        char* duplicatedName = nullptr;

        if (fileName != nullptr)
        {
            const std::uint32_t nameBytes = static_cast<std::uint32_t>(std::strlen(fileName)) + 1u;
            duplicatedName = static_cast<char*>(NXBasics::Runtime_AllocateWithNewHandler(nameBytes, 1));

            // The original copies without checking allocation success.
            std::strcpy(duplicatedName, fileName);
        }

        entry.fileName = duplicatedName;
        *identifier = index;

        return true;
    }

    // Original function: FUN_00404ced
    bool CManager::PreloadWave_Play(std::uint32_t identifier, std::uint32_t volume, std::uint32_t repeatCount, std::uint32_t pan, bool transitionRequested, std::uint32_t durationMilliseconds)
    {
        if (mInitialized == 0)
            return false;

        const std::uint32_t entryOffset = identifier << 4;
        auto* storage = reinterpret_cast<std::byte*>(this);
        auto* entry = reinterpret_cast<SPreloadWave*>(storage + entryOffset + 0x1C);

        if (entry->active == 0)
            return false;

        const std::uint32_t volumeProduct = mMainVolume * volume;
        const std::uint32_t effectiveVolume = volumeProduct / 100u;

        if (effectiveVolume == 0)
            return true;

        if (entry->sound != nullptr && Sound_IsPlaying(entry->sound) != 0)
            return false;

        bool transitionCreated = false;

        if (transitionRequested)
        {
            std::uint32_t index = 0;

            while (index < 10u && mVolumeTransitions[index].active != 0)
                ++index;

            if (index != 10u)
            {
                SVolumeTransition* transition = &mVolumeTransitions[index];
                const std::uint32_t now = ::timeGetTime();

                transition->currentVolume = 0.0;
                transition->lastUpdate = now;
                transition->identifier = identifier;
                transition->targetVolume = NXBasics::Runtime_TimeSigned32(volume);
                transition->active = 1;

                // The original does not reject a zero duration.
                transition->changePerMillisecond = static_cast<double>(volume) / static_cast<double>(durationMilliseconds);

                transitionCreated = true;
            }
        }

        if (entry->sound != nullptr)
        {
            const std::uint32_t panBits = (pan - 64u) * 156u;
            const std::uint32_t attenuationBits = transitionCreated ? 0xFFFFF830u : (effectiveVolume * 5u - 500u) * 4u;

            Sound_SetVolumeAndPan(entry->sound, NXBasics::Runtime_TimeSigned32(attenuationBits), NXBasics::Runtime_TimeSigned32(panBits));
        }

        if (entry->sound != nullptr)
        {
            const std::uint32_t frequencyProduct = mFrequencyPercent * 22050u;
            Sound_SetFrequency(entry->sound, frequencyProduct / 100u);
        }

        if (entry->sound != nullptr)
        {
            const std::uint32_t looping = NXBasics::Runtime_TimeSigned32(repeatCount) > 1 ? 1u : 0u;
            Sound_Play(entry->sound, 0u, looping);
        }

        return true;
    }

    // Original function: FUN_00404e2d
    bool CManager::PreloadWave_IsRunning(std::uint32_t identifier)
    {
        if (mInitialized == 0)
            return false;

        // Preserve the original index calculation without adding a range check.
        const std::uint32_t entryOffset = identifier << 4;
        const auto* storage = reinterpret_cast<const std::byte*>(this);

        std::uint8_t active;
        std::memcpy(&active, storage + entryOffset + 0x1C, sizeof(active));

        if (active == 0)
            return false;

        void* sound;
        std::memcpy(&sound, storage + entryOffset + 0x28, sizeof(sound));

        if (sound == nullptr)
            return false;

        return Sound_IsPlaying(sound) != 0;
    }

    // Original function: FUN_00404e5c
    bool CManager::PreloadWave_ChangeVolume(std::uint32_t identifier, std::uint32_t volume, bool transitionRequested, std::uint32_t durationMilliseconds)
    {
        if (!PreloadWave_IsRunning(identifier))
            return false;

        if (transitionRequested)
        {
            std::uint32_t firstFree = 10;
            std::uint32_t index = 0;

            for (; index < 10u; ++index)
            {
                const SVolumeTransition& entry = mVolumeTransitions[index];

                if (entry.active != 0)
                {
                    if (entry.identifier == identifier)
                        break;
                }
                else if (firstFree == 10u)
                {
                    firstFree = index;
                }
            }

            if (index != 10u)
            {
                SVolumeTransition* entry = &mVolumeTransitions[index];
                entry->lastUpdate = ::timeGetTime();
                entry->targetVolume = NXBasics::Runtime_TimeSigned32(volume);

                // The original does not reject a zero duration.
                entry->changePerMillisecond = (static_cast<double>(volume) - entry->currentVolume) / static_cast<double>(durationMilliseconds);

                return true;
            }

            if (firstFree != 10u)
            {
                SVolumeTransition& entry = mVolumeTransitions[firstFree];
                entry.lastUpdate = ::timeGetTime();
                entry.targetVolume = NXBasics::Runtime_TimeSigned32(volume);
                entry.active = 1;
                entry.identifier = identifier;
                entry.currentVolume = static_cast<double>(volume);

                // The original leaves changePerMillisecond unchanged here.
                return true;
            }

            // A full transition table falls through to immediate application.
        }

        if (volume != 0)
        {
            const std::uint32_t entryOffset = identifier << 4;
            const auto* storage = reinterpret_cast<const std::byte*>(this);

            void* sound;
            std::memcpy(&sound, storage + entryOffset + 0x28, sizeof(sound));

            if (sound != nullptr)
            {
                const std::uint32_t product = mMainVolume * volume;
                const std::uint32_t effectiveVolume = product / 100u;
                const std::uint32_t attenuationBits = (effectiveVolume - 100u) * 5u * 4u;

                Sound_SetVolumeAndPan(sound, NXBasics::Runtime_TimeSigned32(attenuationBits), 0);
            }
        }
        else
        {
            PreloadWave_Stop(identifier);
        }

        return true;
    }

    // Original function: FUN_00404f5c
    bool CManager::PreloadWave_Stop(std::uint32_t identifier)
    {
        if (mInitialized == 0)
            return false;

        // Preserve the original index calculation without adding a range check.
        const std::uint32_t entryOffset = identifier << 4;
        const auto* storage = reinterpret_cast<const std::byte*>(this);

        std::uint8_t active;
        std::memcpy(&active, storage + entryOffset + 0x1C, sizeof(active));

        if (active == 0)
            return false;

        void* sound;
        std::memcpy(&sound, storage + entryOffset + 0x28, sizeof(sound));

        if (sound != nullptr)
        {
            Sound_Stop(sound);
            Sound_ResetPosition(sound);
        }

        return true;
    }

    // Original function: FUN_00404f91
    bool CManager::PreloadWave_Unload(std::uint32_t identifier)
    {
        static_assert(offsetof(CManager, mPreloadWaves) == 0x1C);
        static_assert(sizeof(mPreloadWaves) == 0x3E80);

        if (mInitialized == 0)
            return false;

        // Preserve the original index calculation without adding a range check.
        const std::uint32_t entryOffset = identifier << 4;
        auto* storage = reinterpret_cast<std::byte*>(this);
        auto* entry = reinterpret_cast<SPreloadWave*>(storage + entryOffset + 0x1C);

        if (entry->active != 0)
            entry->Exit();

        return true;
    }

    // Original function: FUN_00404fb7
    void SPreloadWave::Exit()
    {
        FreeData();

        char* nameToRelease = fileName;

        if (nameToRelease != nullptr)
        {
            NXBasics::Runtime_Free(nameToRelease);
            fileName = nullptr;
        }

        active = 0;
    }

    // Original function: FUN_00404fd6
    void SPreloadWave::FreeData()
    {
        void* soundToRelease = sound;

        if (soundToRelease != nullptr)
        {
            Sound_Destroy(soundToRelease);
            NXBasics::Runtime_Free(soundToRelease);
            sound = nullptr;
        }

        std::uint8_t* dataToRelease = waveData;

        if (dataToRelease != nullptr)
        {
            NXBasics::Runtime_Free(dataToRelease);
            waveData = nullptr;
        }
    }

    // Original function: FUN_00405008
    bool CManager::FireAndForget_LoadAndPlay(const char* fileName, std::uint32_t volume, std::uint32_t identifier, std::uint32_t repeatCount, bool deferPlayback, bool useVolumeOverride, std::uint32_t volumeOverride)
    {
        static_assert(offsetof(CManager, mInitialized) == 0x04);
        static_assert(offsetof(CManager, mSoundDevice) == 0x08);
        static_assert(offsetof(CManager, mMainVolume) == 0x10);
        static_assert(offsetof(CManager, mFrequencyPercent) == 0x14);
        static_assert(offsetof(CManager, mNextSequence) == 0x18);

        if (mInitialized == 0)
            return false;

        char resolvedFileName[0x104];

        if (!NXBasics::CFile::FileSystem_Tool_GetRealFilename(fileName, resolvedFileName))
            return false;

        // Preserve 32-bit multiplication before unsigned division.
        const std::uint32_t volumeMultiplier = useVolumeOverride ? volumeOverride : mMainVolume;
        const std::uint32_t volumeProduct = volume * volumeMultiplier;
        const std::uint32_t effectiveVolume = volumeProduct / 100u;

        if (effectiveVolume == 0)
            return true;

        const std::uint32_t slotIndex = l_FireAndForget_GetFreeSlot();
        SFireAndForgetWave& slot = mFireAndForgetWaves[slotIndex];

        if (!ls_WaveFile_Load(mSoundDevice, resolvedFileName, &slot.sound, &slot.waveData))
        {
            slot.Exit();
            return false;
        }

        if (slot.sound != nullptr)
        {
            const std::uint32_t attenuationBits = (effectiveVolume * 5u - 500u) * 4u;
            const std::int32_t attenuation = NXBasics::Runtime_TimeSigned32(attenuationBits);

            Sound_SetVolumeAndPan(slot.sound, attenuation, 0);
        }

        if (!deferPlayback)
        {
            if (slot.sound != nullptr)
            {
                const std::uint32_t frequencyProduct = mFrequencyPercent * 22050u;
                const std::uint32_t frequency = frequencyProduct / 100u;

                Sound_SetFrequency(slot.sound, frequency);
            }

            if (slot.sound != nullptr)
            {
                // The original tests the repeat count with a signed comparison.
                const std::uint32_t looping = NXBasics::Runtime_TimeSigned32(repeatCount) > 1 ? 1u : 0u;

                Sound_Play(slot.sound, 0u, looping);
            }
        }

        slot.active = 1;
        slot.sequence = mNextSequence;
        slot.identifier = identifier;
        ++mNextSequence;

        return true;
    }

    // Original function: FUN_00405119
    bool CManager::FireAndForget_IsRunning(std::uint32_t identifier)
    {
        for (const SFireAndForgetWave& slot : mFireAndForgetWaves)
        {
            if (slot.active != 0 && slot.identifier == identifier)
                return true;
        }

        return false;
    }

    // Original function: FUN_00405141
    bool CManager::FireAndForget_StopAll()
    {
        if (mInitialized == 0)
            return false;

        for (SFireAndForgetWave& slot : mFireAndForgetWaves)
        {
            if (slot.active != 0)
                slot.Exit();
        }

        return true;
    }

    // Original function: FUN_0040516c
    bool CManager::FireAndForget_Stop(std::uint32_t identifier)
    {
        if (mInitialized == 0)
            return false;

        for (SFireAndForgetWave& slot : mFireAndForgetWaves)
        {
            if (slot.active != 0 && slot.identifier == identifier)
                slot.Exit();
        }

        return true;
    }

    // Original function: FUN_004051a3
    std::uint32_t CManager::l_FireAndForget_GetFreeSlot()
    {
        static_assert(offsetof(CManager, mFireAndForgetWaves) == 0x3E9C);

        std::uint32_t oldestIndex = 0;
        std::uint32_t oldestSequence = 0x7FFFFFFFu;
        std::uint32_t index = 0;

        while (index < 40u)
        {
            const SFireAndForgetWave& slot = mFireAndForgetWaves[index];

            if (slot.active == 0)
                break;

            // Preserve the unsigned comparison and the original initial limit.
            if (slot.sequence < oldestSequence)
            {
                oldestSequence = slot.sequence;
                oldestIndex = index;
            }

            ++index;
        }

        if (index == 40u)
        {
            mFireAndForgetWaves[oldestIndex].Exit();
            index = oldestIndex;
        }

        SFireAndForgetWave& slot = mFireAndForgetWaves[index];

        std::memset(&slot, 0, sizeof(slot));
        slot.identifier = 0xFFFFFFFFu;

        return index;
    }

    // Original function: FUN_00405207
    void SFireAndForgetWave::Exit()
    {
        void* soundToRelease = sound;

        if (soundToRelease != nullptr)
        {
            Sound_Destroy(soundToRelease);
            NXBasics::Runtime_Free(soundToRelease);
            sound = nullptr;
        }

        std::uint8_t* dataToRelease = waveData;

        if (dataToRelease != nullptr)
        {
            NXBasics::Runtime_Free(dataToRelease);
            waveData = nullptr;
        }

        active = 0;
    }

    // Original function: FUN_00405250
    // Descriptive reconstruction name; the original Windows class name is unknown.
    std::int32_t SoundDevice_Initialize(void* soundDevice, void* window, std::uint32_t cooperativeLevel, std::uint32_t channels, std::uint32_t samplesPerSecond, std::uint32_t bitsPerSample)
    {
        auto* storage = static_cast<std::byte*>(soundDevice);

        IDirectSound8* device;
        std::memcpy(&device, storage, sizeof(device));

        if (device != nullptr)
        {
            device->Release();

            device = nullptr;
            std::memcpy(storage, &device, sizeof(device));
        }

        HRESULT result = ::DirectSoundCreate8(nullptr, &device, nullptr);
        std::memcpy(storage, &device, sizeof(device));

        if (result < 0)
            return result;

        result = device->SetCooperativeLevel(static_cast<HWND>(window), cooperativeLevel);

        if (result < 0)
            return result;

        const std::int32_t formatResult = SoundDevice_SetPrimaryFormat(soundDevice, channels, samplesPerSecond, bitsPerSample);

        if (formatResult < 0)
            return formatResult;

        return 0;
    }

    // Original function: FUN_004052a0
    // Descriptive reconstruction name; the original Windows class name is unknown.
    std::int32_t SoundDevice_SetPrimaryFormat(void* soundDevice, std::uint32_t channels, std::uint32_t samplesPerSecond, std::uint32_t bitsPerSample)
    {
        static_assert(sizeof(DSBUFFERDESC) == 0x24);
        static_assert(sizeof(WAVEFORMATEX) == 0x12);

        IDirectSound8* device;
        std::memcpy(&device, soundDevice, sizeof(device));

        if (device == nullptr)
            return NXBasics::Runtime_TimeSigned32(0x800401F0u);

        IDirectSoundBuffer* primaryBuffer = nullptr;

        DSBUFFERDESC description;
        std::memset(&description, 0, sizeof(description));

        description.dwSize = 0x24;
        description.dwFlags = 1;
        description.dwBufferBytes = 0;
        description.lpwfxFormat = nullptr;

        const HRESULT createResult = device->CreateSoundBuffer(&description, &primaryBuffer, nullptr);

        if (createResult < 0)
            return createResult;

        WAVEFORMATEX format;
        std::memset(&format, 0, sizeof(format));

        format.wFormatTag = WAVE_FORMAT_PCM;
        format.nChannels = static_cast<WORD>(channels);
        format.nSamplesPerSec = samplesPerSecond;
        format.wBitsPerSample = static_cast<WORD>(bitsPerSample);

        // Preserve the original low-word shift and truncation before byte-rate calculation.
        const std::uint32_t bytesPerSample = static_cast<std::uint16_t>(bitsPerSample) >> 3;
        format.nBlockAlign = static_cast<WORD>(bytesPerSample * channels);
        format.nAvgBytesPerSec = static_cast<std::uint32_t>(format.nBlockAlign) * samplesPerSecond;

        const HRESULT formatResult = primaryBuffer->SetFormat(&format);

        if (formatResult < 0)
        {
            // The original returns without releasing the primary buffer.
            return formatResult;
        }

        if (primaryBuffer != nullptr)
            primaryBuffer->Release();

        return 0;
    }

    // Original function: FUN_00405358
    // Descriptive reconstruction name; the original Windows class name is unknown.
    std::int32_t Sound_CreateFromMemory(void* soundDevice, void** sound, std::uint8_t* samples, std::uint32_t sampleBytes, const void* waveFormat, std::uint32_t flags)
    {
        static_assert(sizeof(DSBUFFERDESC) == 0x24);
        static_assert(offsetof(DSBUFFERDESC, guid3DAlgorithm) == 0x14);
        static_assert(sizeof(WAVEFORMATEX) == 0x12);

        auto* deviceStorage = static_cast<std::byte*>(soundDevice);

        IDirectSound* device;
        std::memcpy(&device, deviceStorage, sizeof(device));

        if (device == nullptr)
            return NXBasics::Runtime_TimeSigned32(0x800401F0u);

        if (samples == nullptr || sound == nullptr)
            return NXBasics::Runtime_TimeSigned32(0x80070057u);

        auto* context = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(0x54u, 1));

        if (context == nullptr)
            return NXBasics::Runtime_TimeSigned32(0x8007000Eu);

        // Inline initialization from FUN_0040575a; other bytes remain untouched.
        std::memset(context, 0, 0x12);

        HMMIO file = nullptr;
        const std::uint32_t zero = 0;
        const std::uint32_t one = 1;

        std::memcpy(context + 0x14, &file, sizeof(file));
        std::memcpy(context + 0x40, &zero, sizeof(zero));
        std::memcpy(context + 0x44, &zero, sizeof(zero));

        std::memcpy(context, waveFormat, 0x12);
        std::memcpy(context + 0x48, &samples, sizeof(samples));
        std::memcpy(context + 0x4C, &samples, sizeof(samples));
        std::memcpy(context + 0x50, &sampleBytes, sizeof(sampleBytes));
        std::memcpy(context + 0x44, &one, sizeof(one));

        DSBUFFERDESC description;
        std::memset(&description, 0, sizeof(description));

        description.dwSize = 0x24;
        description.dwFlags = flags;
        description.dwBufferBytes = sampleBytes;
        description.lpwfxFormat = const_cast<WAVEFORMATEX*>(static_cast<const WAVEFORMATEX*>(waveFormat));
        // Original data: 004f3358; all 16 GUID bytes are zero.
        description.guid3DAlgorithm = GUID{};

        IDirectSoundBuffer* buffer = nullptr;

        // Reload the device pointer immediately before the COM call.
        std::memcpy(&device, deviceStorage, sizeof(device));

        const HRESULT result = device->CreateSoundBuffer(&description, &buffer, nullptr);

        if (result < 0)
        {
            // The original does not release the context on this failure path.
            return result;
        }

        auto* wrapper = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(0x0Cu, 1));

        if (wrapper != nullptr)
        {
            std::memcpy(wrapper, &buffer, sizeof(buffer));
            std::memcpy(wrapper + 0x04, &sampleBytes, sizeof(sampleBytes));
            std::memcpy(wrapper + 0x08, &context, sizeof(context));

            Sound_FillBuffer(wrapper, buffer);

            // The original ignores the fill result and reloads the buffer.
            std::memcpy(&buffer, wrapper, sizeof(buffer));
            buffer->SetCurrentPosition(0);
        }

        // A failed wrapper allocation still returns success in the original.
        *sound = wrapper;
        return 0;
    }

    // Original function: FUN_00405464
    // Descriptive reconstruction name; the original Windows class name is unknown.
    void Sound_Destroy(void* sound)
    {
        auto* storage = static_cast<std::byte*>(sound);

        IDirectSoundBuffer* buffer;
        std::memcpy(&buffer, storage, sizeof(buffer));

        if (buffer != nullptr)
            buffer->Stop();

        // The original reloads the interface pointer before each operation.
        std::memcpy(&buffer, storage, sizeof(buffer));

        if (buffer != nullptr)
            buffer->SetCurrentPosition(0);

        std::memcpy(&buffer, storage, sizeof(buffer));

        if (buffer != nullptr)
        {
            buffer->Release();

            buffer = nullptr;
            std::memcpy(storage, &buffer, sizeof(buffer));
        }

        void* waveContext;
        std::memcpy(&waveContext, storage + 0x08, sizeof(waveContext));

        if (waveContext != nullptr)
        {
            auto* contextStorage = static_cast<std::byte*>(waveContext);

            HMMIO file;
            std::memcpy(&file, contextStorage + 0x14, sizeof(file));

            // The original calls mmioClose even when the handle is null.
            ::mmioClose(file, 0);

            file = nullptr;
            std::memcpy(contextStorage + 0x14, &file, sizeof(file));

            NXBasics::Runtime_Free(waveContext);

            waveContext = nullptr;
            std::memcpy(storage + 0x08, &waveContext, sizeof(waveContext));
        }
    }

    // Original function: FUN_004054b4
    // Descriptive reconstruction name; the original Windows class name is unknown.
    std::int32_t Sound_FillBuffer(void* sound, void* buffer)
    {
        auto* soundStorage = static_cast<std::byte*>(sound);
        auto* directSoundBuffer = static_cast<IDirectSoundBuffer*>(buffer);

        void* destination = nullptr;
        DWORD lockedBytes = 0;
        std::uint32_t bytesRead = 0;

        if (directSoundBuffer == nullptr)
            return NXBasics::Runtime_TimeSigned32(0x800401F0u);

        const std::int32_t restoreResult = Sound_RestoreBuffer(directSoundBuffer, nullptr);

        if (restoreResult < 0)
            return restoreResult;

        std::uint32_t bufferBytes;
        std::memcpy(&bufferBytes, soundStorage + 0x04, sizeof(bufferBytes));

        const HRESULT lockResult = directSoundBuffer->Lock(0, bufferBytes, &destination, &lockedBytes, nullptr, nullptr, 0);

        if (lockResult < 0)
            return lockResult;

        void* waveContext;
        std::memcpy(&waveContext, soundStorage + 0x08, sizeof(waveContext));

        // The original ignores the reset result.
        Wave_Reset(waveContext);

        std::memcpy(&waveContext, soundStorage + 0x08, sizeof(waveContext));

        const std::int32_t readResult = Wave_Read(waveContext, destination, lockedBytes, &bytesRead);

        if (readResult < 0)
        {
            // Preserve the original return without unlocking the buffer.
            return readResult;
        }

        if (bytesRead == 0 || bytesRead < lockedBytes)
        {
            std::memcpy(&waveContext, soundStorage + 0x08, sizeof(waveContext));

            std::uint16_t bitsPerSample;
            std::memcpy(&bitsPerSample, static_cast<const std::byte*>(waveContext) + 0x0E, sizeof(bitsPerSample));

            const int silence = bitsPerSample == 8u ? 0x80 : 0;
            void* fillDestination = destination;

            if (bytesRead != 0)
                fillDestination = static_cast<std::byte*>(destination) + bytesRead;

            std::memset(fillDestination, silence, lockedBytes - bytesRead);
        }

        // The original ignores the unlock result.
        directSoundBuffer->Unlock(destination, lockedBytes, nullptr, 0);

        return 0;
    }

    // Original function: FUN_00405591
    // The original does not use its this pointer.
    std::int32_t Sound_RestoreBuffer(void* buffer, std::uint32_t* restored)
    {
        auto* directSoundBuffer = static_cast<IDirectSoundBuffer*>(buffer);

        if (directSoundBuffer == nullptr)
            return NXBasics::Runtime_TimeSigned32(0x800401F0u);

        if (restored != nullptr)
            *restored = 0;

        DWORD status;
        const HRESULT statusResult = directSoundBuffer->GetStatus(&status);

        if (statusResult < 0)
            return statusResult;

        if ((status & 0x02u) == 0)
            return 1;

        for (;;)
        {
            const HRESULT restoreResult = directSoundBuffer->Restore();

            if (restoreResult == NXBasics::Runtime_TimeSigned32(0x88780096u))
                ::Sleep(10);

            // Every nonzero result is retried, not only DSERR_BUFFERLOST.
            if (restoreResult == 0)
                break;
        }

        if (restored != nullptr)
            *restored = 1;

        return 0;
    }

    // Original function: FUN_004055f7
    // Descriptive reconstruction name; the original Windows class name is unknown.
    std::int32_t Sound_Play(void* sound, std::uint32_t priority, std::uint32_t flags)
    {
        auto* storage = static_cast<std::byte*>(sound);

        IDirectSoundBuffer* buffer;
        std::memcpy(&buffer, storage, sizeof(buffer));

        if (buffer == nullptr)
            return NXBasics::Runtime_TimeSigned32(0x800401F0u);

        std::uint32_t restored;
        const std::int32_t restoreResult = Sound_RestoreBuffer(buffer, &restored);

        if (restoreResult < 0)
            return restoreResult;

        if (restored != 0)
        {
            const std::int32_t fillResult = Sound_FillBuffer(sound, buffer);

            if (fillResult < 0)
                return fillResult;

            IDirectSoundBuffer* currentBuffer;
            std::memcpy(&currentBuffer, storage, sizeof(currentBuffer));

            if (currentBuffer != nullptr)
                currentBuffer->SetCurrentPosition(0);
        }

        // The final operation uses the buffer saved on entry.
        return buffer->Play(0, priority, flags);
    }

    // Original function: FUN_00405650
    // Descriptive reconstruction name; the original Windows class name is unknown.
    std::int32_t Sound_SetFrequency(void* sound, std::uint32_t frequency)
    {
        auto* storage = static_cast<std::byte*>(sound);

        IDirectSoundBuffer* buffer;
        std::memcpy(&buffer, storage, sizeof(buffer));

        if (buffer == nullptr)
            return NXBasics::Runtime_TimeSigned32(0x800401F0u);

        std::uint32_t restored;
        const std::int32_t restoreResult = Sound_RestoreBuffer(buffer, &restored);

        if (restoreResult < 0)
            return restoreResult;

        if (restored != 0)
        {
            const std::int32_t fillResult = Sound_FillBuffer(sound, buffer);

            if (fillResult < 0)
                return fillResult;

            IDirectSoundBuffer* currentBuffer;
            std::memcpy(&currentBuffer, storage, sizeof(currentBuffer));

            if (currentBuffer != nullptr)
                currentBuffer->SetCurrentPosition(0);
        }

        // The final operation uses the buffer saved on entry.
        return buffer->SetFrequency(frequency);
    }

    // Original function: FUN_004056b1
    // Descriptive reconstruction name; the original Windows class name is unknown.
    std::int32_t Sound_SetVolumeAndPan(void* sound, std::int32_t volume, std::int32_t pan)
    {
        auto* storage = static_cast<std::byte*>(sound);

        IDirectSoundBuffer* buffer;
        std::memcpy(&buffer, storage, sizeof(buffer));

        if (buffer == nullptr)
            return NXBasics::Runtime_TimeSigned32(0x800401F0u);

        std::uint32_t restored;
        const std::int32_t restoreResult = Sound_RestoreBuffer(buffer, &restored);

        if (restoreResult < 0)
            return restoreResult;

        if (restored != 0)
        {
            const std::int32_t fillResult = Sound_FillBuffer(sound, buffer);

            if (fillResult < 0)
                return fillResult;

            IDirectSoundBuffer* currentBuffer;
            std::memcpy(&currentBuffer, storage, sizeof(currentBuffer));

            if (currentBuffer != nullptr)
                currentBuffer->SetCurrentPosition(0);
        }

        // Both operations use the saved buffer; their results are ignored.
        buffer->SetVolume(volume);
        buffer->SetPan(pan);

        return 0;
    }

    // Original function: FUN_00405710
    // Descriptive reconstruction name; the original Windows class name is unknown.
    std::int32_t Sound_Stop(void* sound)
    {
        IDirectSoundBuffer* buffer;
        std::memcpy(&buffer, sound, sizeof(buffer));

        if (buffer == nullptr)
            return NXBasics::Runtime_TimeSigned32(0x800401F0u);

        return buffer->Stop();
    }

    // Original function: FUN_00405723
    // Descriptive reconstruction name; the original Windows class name is unknown.
    std::int32_t Sound_ResetPosition(void* sound)
    {
        IDirectSoundBuffer* buffer;
        std::memcpy(&buffer, sound, sizeof(buffer));

        if (buffer == nullptr)
            return NXBasics::Runtime_TimeSigned32(0x800401F0u);

        return buffer->SetCurrentPosition(0);
    }

    // Original function: FUN_00405738
    // Descriptive reconstruction name; the original Windows class name is unknown.
    std::uint32_t Sound_IsPlaying(void* sound)
    {
        IDirectSoundBuffer* buffer;
        std::memcpy(&buffer, sound, sizeof(buffer));

        if (buffer == nullptr)
            return 0;

        DWORD status = 0;

        // The original ignores the HRESULT and evaluates the status output.
        buffer->GetStatus(&status);

        return status & 1u;
    }

    // Original function: FUN_0040577a
    // Descriptive reconstruction name; the original Windows class name is unknown.
    std::int32_t Wave_Reset(void* waveContext)
    {
        static_assert(sizeof(MMCKINFO) == 0x14);

        auto* storage = static_cast<std::byte*>(waveContext);

        std::uint32_t memoryMode;
        std::memcpy(&memoryMode, storage + 0x44, sizeof(memoryMode));

        if (memoryMode != 0)
        {
            std::uint32_t startAddress;
            std::memcpy(&startAddress, storage + 0x48, sizeof(startAddress));
            std::memcpy(storage + 0x4C, &startAddress, sizeof(startAddress));
            return 0;
        }

        HMMIO file;
        std::memcpy(&file, storage + 0x14, sizeof(file));

        if (file == nullptr)
            return NXBasics::Runtime_TimeSigned32(0x800401F0u);

        std::uint32_t parentDataOffset;
        std::memcpy(&parentDataOffset, storage + 0x38, sizeof(parentDataOffset));

        const std::int32_t seekPosition = NXBasics::Runtime_TimeSigned32(parentDataOffset + 4u);

        if (::mmioSeek(file, seekPosition, SEEK_SET) == -1)
            return NXBasics::Runtime_TimeSigned32(0x80004005u);

        const std::uint32_t dataTag = 0x61746164u;
        std::memcpy(storage + 0x18, &dataTag, sizeof(dataTag));

        MMCKINFO chunk;
        MMCKINFO parent;
        std::memcpy(&chunk, storage + 0x18, sizeof(chunk));
        std::memcpy(&parent, storage + 0x2C, sizeof(parent));
        std::memcpy(&file, storage + 0x14, sizeof(file));

        const MMRESULT result = ::mmioDescend(file, &chunk, &parent, MMIO_FINDCHUNK);

        // Preserve output changes even when mmioDescend reports failure.
        std::memcpy(storage + 0x18, &chunk, sizeof(chunk));

        if (result != 0)
            return NXBasics::Runtime_TimeSigned32(0x80004005u);

        return 0;
    }

    // Original function: FUN_004057d4
    // Descriptive reconstruction name; the original Windows class name is unknown.
    std::int32_t Wave_Read(void* waveContext, void* destination, std::uint32_t requestedBytes, std::uint32_t* bytesRead)
    {
        static_assert(sizeof(void*) == 4);
        static_assert(sizeof(MMIOINFO) == 0x48);
        static_assert(offsetof(MMIOINFO, pchNext) == 0x1C);
        static_assert(offsetof(MMIOINFO, pchEndRead) == 0x20);

        auto* storage = static_cast<std::byte*>(waveContext);

        std::uint32_t memoryMode;
        std::memcpy(&memoryMode, storage + 0x44, sizeof(memoryMode));

        if (memoryMode != 0)
        {
            std::uint32_t cursorAddress;
            std::memcpy(&cursorAddress, storage + 0x4C, sizeof(cursorAddress));

            if (cursorAddress == 0)
                return NXBasics::Runtime_TimeSigned32(0x800401F0u);

            if (bytesRead != nullptr)
                *bytesRead = 0;

            std::uint32_t totalBytes;
            std::uint32_t startAddress;
            std::memcpy(&totalBytes, storage + 0x50, sizeof(totalBytes));
            std::memcpy(&startAddress, storage + 0x48, sizeof(startAddress));
            std::memcpy(&cursorAddress, storage + 0x4C, sizeof(cursorAddress));

            // Preserve unsigned 32-bit address wraparound and comparison.
            const std::uint32_t endAddress = startAddress + totalBytes;
            const std::uint32_t requestedEnd = cursorAddress + requestedBytes;
            std::uint32_t copiedBytes = requestedBytes;

            if (requestedEnd > endAddress)
                copiedBytes = startAddress - cursorAddress + totalBytes;

            const void* source = reinterpret_cast<const void*>(static_cast<std::uintptr_t>(cursorAddress));
            std::memmove(destination, source, copiedBytes);

            if (bytesRead != nullptr)
                *bytesRead = copiedBytes;

            // The original does not advance the memory cursor.
            return 0;
        }

        HMMIO file;
        std::memcpy(&file, storage + 0x14, sizeof(file));

        if (file == nullptr)
            return NXBasics::Runtime_TimeSigned32(0x800401F0u);

        if (destination == nullptr || bytesRead == nullptr)
            return NXBasics::Runtime_TimeSigned32(0x80070057u);

        *bytesRead = 0;

        MMIOINFO info;

        if (::mmioGetInfo(file, &info, 0) != 0)
            return NXBasics::Runtime_TimeSigned32(0x80004005u);

        std::uint32_t remainingBytes;
        std::memcpy(&remainingBytes, storage + 0x1C, sizeof(remainingBytes));

        const std::uint32_t copiedBytes = requestedBytes < remainingBytes ? requestedBytes : remainingBytes;
        remainingBytes -= copiedBytes;

        // The original commits this change before attempting the read.
        std::memcpy(storage + 0x1C, &remainingBytes, sizeof(remainingBytes));

        auto* output = static_cast<std::uint8_t*>(destination);

        for (std::uint32_t index = 0; index < copiedBytes; ++index)
        {
            if (info.pchNext == info.pchEndRead)
            {
                std::memcpy(&file, storage + 0x14, sizeof(file));

                if (::mmioAdvance(file, &info, MMIO_READ) != 0)
                    return NXBasics::Runtime_TimeSigned32(0x80004005u);

                if (info.pchNext == info.pchEndRead)
                    return NXBasics::Runtime_TimeSigned32(0x80004005u);
            }

            const auto value = static_cast<std::uint8_t>(*info.pchNext);
            char* next = info.pchNext + 1;

            output[index] = value;
            info.pchNext = next;
        }

        std::memcpy(&file, storage + 0x14, sizeof(file));

        if (::mmioSetInfo(file, &info, 0) != 0)
            return NXBasics::Runtime_TimeSigned32(0x80004005u);

        *bytesRead = copiedBytes;
        return 0;
    }

    // Original function: FUN_00485fdd
    void CManager::Jingle_Play(std::uint32_t event)
    {
        std::int32_t programState;
        std::memcpy(&programState, NMasterControlProgram::CMasterControlProgram::sTheObjectPtr, sizeof(programState));

        if (programState == 7 || programState == 8 || programState == 9)
            return;

        if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetIntegerValue("fx_jingles_off") != 0)
            return;

        const std::uint32_t jingleEvent = event + 0x15u;

        if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetIntegerValue("music_mode") == 2)
        {
            if (DAT_0050f698 != nullptr)
                FUN_004038bd(DAT_0050f698, jingleEvent, 0, 2);
        }

        if (DAT_0050f6a4 == nullptr || DAT_0050f6a4->mInitialized == 0)
            return;

        const std::uint32_t index = l_Jingle_FindIndex(DAT_00568a28, jingleEvent);
        const std::uint32_t identifier = index + 0x87E85u;

        if (DAT_0050f6a4->FireAndForget_IsRunning(identifier))
            return;

        std::uint32_t recordsAddress;
        std::memcpy(&recordsAddress, static_cast<const std::byte*>(DAT_00568a28) + 0x1EC, sizeof(recordsAddress));

        const std::uint32_t recordAddress = recordsAddress + (index << 7);

        if (recordAddress == 0)
            return;

        const auto* record = reinterpret_cast<const std::byte*>(static_cast<std::uintptr_t>(recordAddress));
        std::uint32_t volume;
        std::memcpy(&volume, record + 0x7C, sizeof(volume));

        DAT_0050f6a4->FireAndForget_LoadAndPlay(reinterpret_cast<const char*>(record + 4), volume, identifier, 1, false, false, 100);
    }

    // Original function: FUN_0048e1e1
    std::uint32_t CManager::l_Jingle_FindIndex(const void* staticSoundData, std::uint32_t event)
    {
        if (DAT_0050f6a4 == nullptr || DAT_0050f6a4->mInitialized == 0)
            return 0;

        const auto* storage = static_cast<const std::byte*>(staticSoundData);
        std::uint32_t count;
        std::memcpy(&count, storage + 0x1E8, sizeof(count));

        if (count == 0)
            return 0;

        std::uint32_t recordAddress;
        std::memcpy(&recordAddress, storage + 0x1EC, sizeof(recordAddress));

        for (std::uint32_t index = 0; index < count; ++index)
        {
            std::uint32_t recordEvent;
            std::memcpy(&recordEvent, reinterpret_cast<const void*>(static_cast<std::uintptr_t>(recordAddress)), sizeof(recordEvent));

            if (recordEvent == event)
                return index;

            recordAddress += 0x80u;
        }

        return 0;
    }
}