#pragma once

namespace NXBasics
{
    // Shared declaration previously local to X2DApplication.cpp.
    // Constructor/destructor bodies remain pending; no substitute startup is supplied.
    class CXBSystemManager
    {
    public:
        // Original function: FUN_004069f3
        CXBSystemManager();

        // Original function: FUN_00406ab5
        ~CXBSystemManager();

        // Original data: 005106d8; Mac: CXBSystemManager::sTheObjectPtr.
        inline static CXBSystemManager* sTheObjectPtr = nullptr;
        // Original data: 005106dc; Mac: CXBSystemManager::sPalettePtr.
        // The complete CPalette class is not reconstructed yet.
        inline static void* sPalettePtr = nullptr;
        // Original data: 005106e8; Mac: CXBSystemManager::sFatalErrorFunctionPtr.
        inline static void* sFatalErrorFunctionPtr = nullptr;
        // Original data: 005106ec; Mac: CXBSystemManager::sWarningErrorFunctionPtr.
        inline static void* sWarningErrorFunctionPtr = nullptr;
        // Original data: 005106f0; Mac: CXBSystemManager::sAssertFunctionPtr.
        inline static void* sAssertFunctionPtr = nullptr;
    };
}
