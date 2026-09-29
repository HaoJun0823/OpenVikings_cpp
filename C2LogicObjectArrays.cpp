#include "C2LogicObjectArrays.h"
#include "C2CallbackManager.h"
#include "XCrtFileAccess.h"

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace NC2Logic
{
    // Original function: FUN_0040ad6c
    static bool __cdecl ArrayTool_IsObjectAllocated(void* object)
    {
        return *static_cast<const std::uint8_t*>(object) != 0;
    }

    template<typename T>
    static void Array_Construct(T* array)
    {
        T::sTheObjectPtr = array;
        std::memset(static_cast<CArrayHandler*>(array), 0, sizeof(CArrayHandler));

        T::mStaticVars = nullptr;

        constexpr std::uint32_t allocationSize = static_cast<std::uint32_t>(T::InitialCapacity) * T::ElementSize;

        T::mStaticVars = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(allocationSize, 1));

        // The original element constructors only clear fields that are overwritten by this clear.
        std::memset(T::mStaticVars, 0, allocationSize);

        array->System_InitArray(T::InitialCapacity, T::mStaticVars, T::ElementSize, ArrayTool_IsObjectAllocated, nullptr, nullptr, nullptr);

        CCallbackManager::sTheObjectPtr->ExecuteCallback(T::CallbackType, true, static_cast<std::uint32_t>(array->mCapacity), 0, 0);
    }

    template<typename T>
    static void Array_Destruct(T* array)
    {
        CCallbackManager::sTheObjectPtr->ExecuteCallback(T::CallbackType, false, 0, 0, 0);

        NXBasics::Runtime_Free(T::mStaticVars);

        T::mStaticVars = nullptr;
        T::sTheObjectPtr = nullptr;

        // CArrayHandler's destructor clears the initialized flag afterwards.
    }

    template<typename T>
    static void Array_PrepareLoad(T* array)
    {
        T::sTheObjectPtr = array;
        std::memset(static_cast<CArrayHandler*>(array), 0, sizeof(CArrayHandler));

        // The original does not reset mStaticVars before validating the input.
    }

    template<typename T>
    static void Array_LoadStorage(T* array, CIoHelper* io)
    {
        const std::uint32_t capacity = FUN_0042a02d(io);
        const std::uint32_t allocationSize = capacity * T::ElementSize;

        T::mStaticVars = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(allocationSize, 1));

        // The original element constructors only clear fields that are overwritten by this clear.
        std::memset(T::mStaticVars, 0, allocationSize);

        array->IO_InitAndLoadArray(T::mStaticVars, io, ArrayTool_IsObjectAllocated, nullptr, nullptr, nullptr);

        CCallbackManager::sTheObjectPtr->ExecuteCallback(T::CallbackType, true, static_cast<std::uint32_t>(array->mCapacity), 0, 0);
    }

    template<typename T>
    static void Array_LoadOpenedChunk(T* array, CIoHelper* io)
    {
        Array_PrepareLoad(array);

        std::uint32_t version;
        std::memcpy(&version, reinterpret_cast<const std::byte*>(io) + 0x10, sizeof(version));

        if (version != T::SaveVersion)
        {
            FUN_0042a1ce(io);
            return;
        }

        Array_LoadStorage(array, io);
    }

    template<typename T>
    static void Array_Save(T* array, CIoHelper* io)
    {
        FUN_0042a3c6(io, T::SaveChunkId, T::SaveVersion);
        FUN_0042a244(io, array->mCapacity);

        array->IO_SaveArray(io);

        FUN_0042a431(io);
    }

    // Original function: FUN_00430c90
    CArrayHandler::CArrayHandler()
    {
        std::memset(this, 0, sizeof(*this));
    }

    // Original function: FUN_00430ca4
    CArrayHandler::~CArrayHandler()
    {
        mInitialized = 0;
    }

    // Original function: FUN_00430ca8
    void CArrayHandler::System_InitArray(std::int32_t capacity, std::byte* entries, std::uint32_t elementSize, TArrayIsAllocated isAllocated, TArrayClearElement clearElement, TArrayElementIO elementIO, TArrayElementChecksum elementChecksum)
    {
        std::memset(this, 0, sizeof(*this));

        mEntries = entries;
        mElementSize = elementSize;
        mIsAllocated = isAllocated;
        mClearElement = clearElement;
        mElementIO = elementIO;
        mElementChecksum = elementChecksum;
        mCapacity = capacity;

        for (std::int32_t index = 0; index < mCapacity; ++index)
            l_Element_Clear(index);

        mInitialized = 1;
    }

    // Original function: FUN_00430d02
    void CArrayHandler::IO_InitAndLoadArray(std::byte* entries, CIoHelper* io, TArrayIsAllocated isAllocated, TArrayClearElement clearElement, TArrayElementIO elementIO, TArrayElementChecksum elementChecksum)
    {
        std::memset(this, 0, sizeof(*this));

        mEntries = entries;
        mIsAllocated = isAllocated;
        mClearElement = clearElement;
        mElementIO = elementIO;
        mElementChecksum = elementChecksum;

        std::uint32_t version = 0;

        if (!FUN_0042a13e(io, 0x78616861u, &version, 0))
            return;

        // The original reads all six serialized state fields without validating the version.
        FUN_00429f88(io, &mCapacity, 0x18u);

        for (std::int32_t index = 0; index < mCapacity; ++index)
            l_Element_Clear(index);

        for (std::int32_t ordinal = 0; ordinal < mAllocatedCount; ++ordinal)
        {
            const std::uint32_t index = FUN_0042a02d(io);
            std::byte* element = l_Element_GetPtr(index);

            if (mElementIO != nullptr)
                mElementIO(element, true, io);
            else
                FUN_00429f88(io, element, mElementSize);
        }

        mInitialized = 1;
    }

    // Original function: FUN_00430db7
    void CArrayHandler::IO_SaveArray(CIoHelper* io)
    {
        FUN_0042a3c6(io, 0x78616861u, 1u);
        FUN_0042a2e4(io, &mCapacity, 0x18u);

        for (std::int32_t index = 0; index < mCapacity; ++index)
        {
            std::byte* element = l_Element_GetPtr(static_cast<std::uint32_t>(index));

            if (!mIsAllocated(element))
                continue;

            FUN_0042a244(io, index);

            if (mElementIO != nullptr)
                mElementIO(element, false, io);
            else
                FUN_0042a2e4(io, element, mElementSize);
        }

        FUN_0042a431(io);
    }

    // Original function: FUN_00430f23
    void CArrayHandler::l_Element_Clear(std::int32_t index)
    {
        std::byte* element = l_Element_GetPtr(static_cast<std::uint32_t>(index));

        if (mClearElement != nullptr)
            mClearElement(element);
        else
            std::memset(element, 0, mElementSize);
    }

    std::byte* CArrayHandler::l_Element_GetPtr(std::uint32_t index)
    {
        const std::uint32_t baseAddress = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(mEntries));
        const std::uint32_t address = baseAddress + index * mElementSize;

        return reinterpret_cast<std::byte*>(static_cast<std::uintptr_t>(address));
    }

    // Original function: FUN_0040aa24
    CHouseArray::CHouseArray()
    {
        Array_Construct(this);
    }

    // Original function: FUN_0040aaec
    CHouseArray::~CHouseArray()
    {
        Array_Destruct(this);
    }

    // Original function: FUN_0040ab25
    void CHouseArray::IO_Savegame_DoSave(CIoHelper* io)
    {
        Array_Save(this, io);
    }

    // Original function: FUN_0040ab59
    CHouseArray::CHouseArray(CIoHelper* io)
    {
        Array_LoadOpenedChunk(this, io);
    }

    // Original function: FUN_0040b271
    CHumanArray::CHumanArray()
    {
        Array_Construct(this);
    }

    // Original function: FUN_0040b3c8
    CHumanArray::~CHumanArray()
    {
        Array_Destruct(this);
    }

    // Original function: FUN_0040b401
    void CHumanArray::IO_Savegame_DoSave(CIoHelper* io)
    {
        Array_Save(this, io);
    }

    // Original function: FUN_0040b435
    CHumanArray::CHumanArray(CIoHelper* io)
    {
        Array_LoadOpenedChunk(this, io);
    }

    // Original function: FUN_0040b5fc
    CVehicleArray::CVehicleArray()
    {
        Array_Construct(this);
    }

    // Original function: FUN_0040b740
    CVehicleArray::~CVehicleArray()
    {
        Array_Destruct(this);
    }

    // Original function: FUN_0040b779
    void CVehicleArray::IO_Savegame_DoSave(CIoHelper* io)
    {
        Array_Save(this, io);
    }

    // Original function: FUN_0040b7ad
    CVehicleArray::CVehicleArray(CIoHelper* io)
    {
        Array_LoadOpenedChunk(this, io);
    }

    // Original function: FUN_0041c076
    CAnimalArray::CAnimalArray()
    {
        Array_Construct(this);
    }

    // Original function: FUN_0041c16a
    CAnimalArray::~CAnimalArray()
    {
        Array_Destruct(this);
    }

    // Original function: FUN_0041c1a3
    void CAnimalArray::IO_Savegame_DoSave(CIoHelper* io)
    {
        Array_Save(this, io);
    }

    // Original function: FUN_0041c1d7
    CAnimalArray::CAnimalArray(CIoHelper* io)
    {
        Array_LoadOpenedChunk(this, io);
    }

    // Original function: FUN_004266a9
    CGuideArray::CGuideArray()
    {
        Array_Construct(this);
    }

    // Original function: FUN_00426747
    CGuideArray::~CGuideArray()
    {
        Array_Destruct(this);
    }

    // Original function: FUN_00426780
    void CGuideArray::IO_Savegame_DoSave(CIoHelper* io)
    {
        Array_Save(this, io);
    }

    // Original function: FUN_004267b4
    CGuideArray::CGuideArray(CIoHelper* io)
    {
        Array_PrepareLoad(this);

        std::uint32_t version = 0;

        if (!FUN_0042a13e(io, SaveChunkId, &version, 0))
            return;

        if (version != SaveVersion)
        {
            FUN_0042a1ce(io);
            return;
        }

        Array_LoadStorage(this, io);
    }
}