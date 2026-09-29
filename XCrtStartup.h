#pragma once

#include "XCrtFileAccess.h"

#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <excpt.h>
#include <Windows.h>

namespace NXBasics
{
    using RuntimeApplicationEntry = int(__cdecl*)(HINSTANCE, HINSTANCE, LPSTR, int);

    inline int Runtime_RunApplication(RuntimeApplicationEntry applicationEntry, RuntimeVoidCallback* applicationBegin, RuntimeVoidCallback* applicationEnd);
    inline int Runtime_FilterException(std::uint32_t exceptionCode, EXCEPTION_POINTERS* information);
    inline RuntimeExceptionAction* Runtime_FindExceptionAction(std::uint32_t exceptionCode, RuntimeExceptionAction* actions);
    inline char* Runtime_GetWinMainCommandLine();
    inline void Runtime_InitializeArguments();
    inline void Runtime_ParseArguments(const char* commandLine, char** arguments, char* storage, std::uint32_t* pointerCount, std::uint32_t* byteCount);

    // Original data: DAT_00569a04.
    inline std::uint32_t gRuntimeOsVersion = 0;

    // Original data: DAT_00569a08.
    inline std::uint32_t gRuntimeWindowsVersion = 0;

    // Original data: DAT_00569a0c.
    inline std::uint32_t gRuntimeWindowsMajorVersion = 0;

    // Original data: DAT_00569a10.
    inline std::uint32_t gRuntimeWindowsMinorVersion = 0;

    // Original data: DAT_00569a14.
    inline std::int32_t gRuntimeArgumentCount = 0;

    // Original data: DAT_00569a18.
    inline char** gRuntimeArguments = nullptr;

    // Original data: DAT_00569a30.
    inline char* gRuntimeProgramPath = nullptr;

    // Original data: DAT_00569bbc.
    inline char gRuntimeExecutablePathBuffer[0x104] = {};

    // Original data: DAT_0056b714.
    inline char* gRuntimeCommandLine = nullptr;

    // Original function: FUN_004e709e
    // Hosted adaptation: the compiler CRT owns native C++ static initialization.
    // Explicit application callbacks must contain only separately owned initializers.
    // Normal cleanup returns to WinMain so the compiler CRT can finish its exit path.
    inline int Runtime_RunApplication(RuntimeApplicationEntry applicationEntry, RuntimeVoidCallback* applicationBegin, RuntimeVoidCallback* applicationEnd)
    {
        const DWORD version = GetVersion();

        gRuntimeWindowsMinorVersion = (version >> 8) & 0xFFu;
        gRuntimeWindowsMajorVersion = version & 0xFFu;
        gRuntimeWindowsVersion = (gRuntimeWindowsMajorVersion << 8) + gRuntimeWindowsMinorVersion;
        gRuntimeOsVersion = version >> 16;

        if (Runtime_InitializeHeap(1) == 0)
        {
            Runtime_StartupFatalExit(0x1C);
        }

        if (Runtime_InitializeThreading() == 0)
        {
            Runtime_StartupFatalExit(0x10);
        }

        std::int32_t exceptionCode = 0;

        __try
        {
            Runtime_InitializeDescriptors();

            gRuntimeCommandLine = GetCommandLineA();
            gRuntimeStartupEnvironmentBlock = Runtime_GetEnvironmentBlock();

            Runtime_InitializeArguments();
            Runtime_InitializeEnvironment();
            Runtime_InitializeCrt(applicationBegin, applicationEnd);

            STARTUPINFOA startupInformation{};
            GetStartupInfoA(&startupInformation);

            char* commandLine = Runtime_GetWinMainCommandLine();
            const int showCommand = (startupInformation.dwFlags & STARTF_USESHOWWINDOW) != 0 ? startupInformation.wShowWindow : SW_SHOWDEFAULT;
            HINSTANCE instance = GetModuleHandleA(nullptr);

            const int exitCode = applicationEntry(instance, nullptr, commandLine, showCommand);

            // Original normal exit: FUN_004ea136 -> FUN_004ea158.
            // Preserve normal cleanup while returning to the hosting compiler CRT.
            Runtime_PerformExit(exitCode, 0, 1);

            return exitCode;
        }
        __except ((exceptionCode = Runtime_TimeSigned32(GetExceptionCode()), Runtime_FilterException(static_cast<std::uint32_t>(exceptionCode), GetExceptionInformation())))
        {
            Runtime_QuickExit(exceptionCode);
        }

        return exceptionCode;
    }

    // Original function: FUN_004ebe61
    inline int Runtime_FilterException(std::uint32_t exceptionCode, EXCEPTION_POINTERS* information)
    {
        auto* threadBytes = reinterpret_cast<std::uint8_t*>(Runtime_GetThreadData());

        RuntimeExceptionAction* actions;
        std::memcpy(&actions, threadBytes + 0x50, sizeof(actions));

        RuntimeExceptionAction* entry = Runtime_FindExceptionAction(exceptionCode, actions);

        if (entry == nullptr || entry->action == 0)
        {
            return UnhandledExceptionFilter(information);
        }

        const std::uint32_t action = entry->action;

        if (action == 5)
        {
            entry->action = 0;
            return EXCEPTION_EXECUTE_HANDLER;
        }

        if (action == 1)
        {
            return EXCEPTION_CONTINUE_EXECUTION;
        }

        EXCEPTION_POINTERS* previousInformation;
        std::memcpy(&previousInformation, threadBytes + 0x54, sizeof(previousInformation));
        std::memcpy(threadBytes + 0x54, &information, sizeof(information));

        const std::int32_t signalNumber = entry->signalNumber;

        if (signalNumber == 8)
        {
            for (std::int32_t index = gRuntimeFloatingPointActionBegin; index < gRuntimeFloatingPointActionBegin + gRuntimeFloatingPointActionCount; ++index)
            {
                RuntimeExceptionAction* currentActions;
                std::memcpy(&currentActions, threadBytes + 0x50, sizeof(currentActions));

                currentActions[index].action = 0;
            }

            std::int32_t previousFloatingPointCode;
            std::memcpy(&previousFloatingPointCode, threadBytes + 0x58, sizeof(previousFloatingPointCode));

            std::int32_t floatingPointCode = previousFloatingPointCode;

            switch (entry->exceptionCode)
            {
                case 0xC000008Eu:
                    floatingPointCode = 0x83;
                    break;

                case 0xC0000090u:
                    floatingPointCode = 0x81;
                    break;

                case 0xC0000091u:
                    floatingPointCode = 0x84;
                    break;

                case 0xC0000093u:
                    floatingPointCode = 0x85;
                    break;

                case 0xC000008Du:
                    floatingPointCode = 0x82;
                    break;

                case 0xC000008Fu:
                    floatingPointCode = 0x86;
                    break;

                case 0xC0000092u:
                    floatingPointCode = 0x8A;
                    break;
            }

            std::memcpy(threadBytes + 0x58, &floatingPointCode, sizeof(floatingPointCode));

            using FloatingPointSignalCallback = void(__cdecl*)(std::int32_t, std::int32_t);

            const auto callback = reinterpret_cast<FloatingPointSignalCallback>(static_cast<std::uintptr_t>(action));
            callback(8, floatingPointCode);

            std::memcpy(threadBytes + 0x58, &previousFloatingPointCode, sizeof(previousFloatingPointCode));
        }
        else
        {
            entry->action = 0;

            using SignalCallback = void(__cdecl*)(std::int32_t);

            const auto callback = reinterpret_cast<SignalCallback>(static_cast<std::uintptr_t>(action));
            callback(signalNumber);
        }

        std::memcpy(threadBytes + 0x54, &previousInformation, sizeof(previousInformation));

        return EXCEPTION_CONTINUE_EXECUTION;
    }

    // Original function: FUN_004ebf9f
    inline RuntimeExceptionAction* Runtime_FindExceptionAction(std::uint32_t exceptionCode, RuntimeExceptionAction* actions)
    {
        RuntimeExceptionAction* current = actions;
        RuntimeExceptionAction* end = actions + gRuntimeExceptionActionCount;

        while (current < end)
        {
            if (current->exceptionCode == exceptionCode)
            {
                return current;
            }

            ++current;
        }

        return nullptr;
    }

    // Original function: FUN_004ebfd9
    inline char* Runtime_GetWinMainCommandLine()
    {
        if (gRuntimeMultibyteInitialized == 0)
        {
            Runtime_InitializeMultibyte();
        }

        auto* cursor = reinterpret_cast<std::uint8_t*>(gRuntimeCommandLine);

        if (*cursor == '"')
        {
            ++cursor;

            while (*cursor != '"' && *cursor != 0)
            {
                // Original helpers: FUN_004f0c9a -> FUN_004f0cab.
                if ((gRuntimeMultibyteTypes[*cursor + 1u] & 4u) != 0)
                {
                    ++cursor;
                }

                ++cursor;
            }

            if (*cursor == '"')
            {
                ++cursor;
            }
        }
        else
        {
            while (*cursor > 0x20u)
            {
                ++cursor;
            }
        }

        while (*cursor != 0 && *cursor <= 0x20u)
        {
            ++cursor;
        }

        return reinterpret_cast<char*>(cursor);
    }

    // Original function: FUN_004ec0ea
    inline void Runtime_InitializeArguments()
    {
        if (gRuntimeMultibyteInitialized == 0)
        {
            Runtime_InitializeMultibyte();
        }

        GetModuleFileNameA(nullptr, gRuntimeExecutablePathBuffer, sizeof(gRuntimeExecutablePathBuffer));

        gRuntimeProgramPath = gRuntimeExecutablePathBuffer;

        const char* commandLine = *gRuntimeCommandLine != '\0' ? gRuntimeCommandLine : gRuntimeProgramPath;

        std::uint32_t pointerCount;
        std::uint32_t byteCount;

        Runtime_ParseArguments(commandLine, nullptr, nullptr, &pointerCount, &byteCount);

        const std::uint32_t allocationSize = pointerCount * 4u + byteCount;
        auto* arguments = static_cast<char**>(Runtime_Allocate(allocationSize));

        if (arguments == nullptr)
        {
            Runtime_FatalExit(8);
        }

        char* storage = reinterpret_cast<char*>(arguments) + pointerCount * 4u;

        Runtime_ParseArguments(commandLine, arguments, storage, &pointerCount, &byteCount);

        gRuntimeArguments = arguments;
        gRuntimeArgumentCount = Runtime_TimeSigned32(pointerCount - 1u);
    }

    // Original function: FUN_004ec183
    inline void Runtime_ParseArguments(const char* commandLine, char** arguments, char* storage, std::uint32_t* pointerCount, std::uint32_t* byteCount)
    {
        const auto* cursor = reinterpret_cast<const std::uint8_t*>(commandLine);
        char** outputArgument = arguments;
        char* output = storage;

        *pointerCount = 1;
        *byteCount = 0;

        if (outputArgument != nullptr)
        {
            *outputArgument++ = output;
        }

        if (*cursor == '"')
        {
            ++cursor;

            while (*cursor != '"' && *cursor != 0)
            {
                if ((gRuntimeMultibyteTypes[*cursor + 1u] & 4u) != 0)
                {
                    ++*byteCount;

                    // Preserve the original sizing/copying asymmetry at 004ec1cb.
                    if (output != nullptr)
                    {
                        *output++ = static_cast<char>(*cursor++);
                    }
                }

                ++ * byteCount;

                if (output != nullptr)
                {
                    *output++ = static_cast<char>(*cursor);
                }

                ++cursor;
            }

            ++ * byteCount;

            if (output != nullptr)
            {
                *output++ = '\0';
            }

            if (*cursor == '"')
            {
                ++cursor;
            }
        }
        else
        {
            std::uint8_t character;

            do
            {
                ++*byteCount;

                if (output != nullptr)
                {
                    *output++ = static_cast<char>(*cursor);
                }

                character = *cursor++;

                if ((gRuntimeMultibyteTypes[character + 1u] & 4u) != 0)
                {
                    ++*byteCount;

                    if (output != nullptr)
                    {
                        *output++ = static_cast<char>(*cursor);
                    }

                    ++cursor;
                }
            } while (character != ' ' && character != '\t' && character != 0);

            if (character == 0)
            {
                --cursor;
            }
            else if (output != nullptr)
            {
                output[-1] = '\0';
            }
        }

        bool inQuotes = false;

        for (;;)
        {
            while (*cursor == ' ' || *cursor == '\t')
            {
                ++cursor;
            }

            if (*cursor == 0)
            {
                break;
            }

            if (outputArgument != nullptr)
            {
                *outputArgument++ = output;
            }

            ++ * pointerCount;

            for (;;)
            {
                bool copyCharacter = true;
                std::uint32_t backslashes = 0;

                while (*cursor == '\\')
                {
                    ++cursor;
                    ++backslashes;
                }

                if (*cursor == '"')
                {
                    if ((backslashes & 1u) == 0)
                    {
                        if (inQuotes && cursor[1] == '"')
                        {
                            ++cursor;
                        }
                        else
                        {
                            copyCharacter = false;
                        }

                        // The original toggles this even for doubled quotes.
                        inQuotes = !inQuotes;
                    }

                    backslashes >>= 1;
                }

                while (backslashes != 0)
                {
                    if (output != nullptr)
                    {
                        *output++ = '\\';
                    }

                    ++ * byteCount;
                    --backslashes;
                }

                if (*cursor == 0 || (!inQuotes && (*cursor == ' ' || *cursor == '\t')))
                {
                    break;
                }

                if (copyCharacter)
                {
                    if ((gRuntimeMultibyteTypes[*cursor + 1u] & 4u) != 0)
                    {
                        if (output != nullptr)
                        {
                            *output++ = static_cast<char>(*cursor);
                        }

                        ++cursor;
                        ++*byteCount;
                    }

                    if (output != nullptr)
                    {
                        *output++ = static_cast<char>(*cursor);
                    }

                    ++ * byteCount;
                }

                ++cursor;
            }

            if (output != nullptr)
            {
                *output++ = '\0';
            }

            ++ * byteCount;
        }

        if (outputArgument != nullptr)
        {
            *outputArgument = nullptr;
        }

        // Include the terminating null pointer in the allocation count.
        ++ * pointerCount;
    }
}
