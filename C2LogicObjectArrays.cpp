#include "C2LogicObjectArrays.h"
#include "C2CallbackManager.h"
#include "C2IoHelper.h"
#include "XCrtFileAccess.h"

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace NC2Logic
{
    // Original function: FUN_0040211a; IDs are deliberately not range checked.
    const void* GetHumanPtr(std::int32_t id)
    {
        return reinterpret_cast<const void*>(reinterpret_cast<std::uintptr_t>(CHumanArray::mStaticVars) + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize);
    }

    // Original function: FUN_004077a7.
    const void* GetAnimalPtr(std::int32_t id)
    {
        return reinterpret_cast<const void*>(reinterpret_cast<std::uintptr_t>(CAnimalArray::mStaticVars) + static_cast<std::uint32_t>(id) * CAnimalArray::ElementSize);
    }

    // Original function: FUN_0042c62b.
    bool Human_IsJobSoldier(const void* human)
    {
        std::int32_t job;
        std::memcpy(&job, static_cast<const std::byte*>(human) + 0x18, sizeof(job));
        return job >= 0x1F && job <= 0x29;
    }

    // Original function: FUN_0042c63e.
    bool Human_IsJobHero(const void* human)
    {
        std::int32_t job;
        std::memcpy(&job, static_cast<const std::byte*>(human) + 0x18, sizeof(job));
        return job >= 0x2A && job <= 0x2F;
    }

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
        const std::uint32_t capacity = io->IO_File_ReadLong();
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

        if (io->mReadHeader.version != T::SaveVersion)
        {
            io->IO_File_Chunk_Skip();
            return;
        }

        Array_LoadStorage(array, io);
    }

    template<typename T>
    static void Array_Save(T* array, CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(T::SaveChunkId, T::SaveVersion);
        io->IO_Memory_WriteLong(static_cast<std::uint32_t>(array->mCapacity));

        array->IO_SaveArray(io);

        io->IO_Tool_Chunk_FinishAndWrite();
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

        if (!io->IO_File_Chunk_CheckMarkerAndGetVersion(0x78616861u, &version, nullptr))
            return;

        // The original reads all six serialized state fields without validating the version.
        io->IO_File_ReadBinary(&mCapacity, 0x18u);

        for (std::int32_t index = 0; index < mCapacity; ++index)
            l_Element_Clear(index);

        for (std::int32_t ordinal = 0; ordinal < mAllocatedCount; ++ordinal)
        {
            const std::uint32_t index = io->IO_File_ReadLong();
            std::byte* element = l_Element_GetPtr(index);

            if (mElementIO != nullptr)
                mElementIO(element, true, io);
            else
                io->IO_File_ReadBinary(element, mElementSize);
        }

        mInitialized = 1;
    }

    // Original function: FUN_00430db7
    void CArrayHandler::IO_SaveArray(CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x78616861u, 1u);
        io->IO_Memory_WriteBinary(&mCapacity, 0x18u);

        for (std::int32_t index = 0; index < mCapacity; ++index)
        {
            std::byte* element = l_Element_GetPtr(static_cast<std::uint32_t>(index));

            if (!mIsAllocated(element))
                continue;

            io->IO_Memory_WriteLong(static_cast<std::uint32_t>(index));

            if (mElementIO != nullptr)
                mElementIO(element, false, io);
            else
                io->IO_Memory_WriteBinary(element, mElementSize);
        }

        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_00430e90. The second argument is unused in Windows.
    bool CArrayHandler::Element_Free(std::int32_t index, bool /*clearElement*/)
    {
        if (!mIsAllocated(l_Element_GetPtr(static_cast<std::uint32_t>(index))))
        {
            return false;
        }

        mAllocatedCount = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(mAllocatedCount) - 1u);
        if (mAllocatedCount == 0)
        {
            mLastScanIndex = 0;
            mFirstScanIndex = 0;
            mNextFreeIndex = 0;
            return true;
        }
        if (index < mNextFreeIndex)
        {
            mNextFreeIndex = index;
        }
        if (index == mFirstScanIndex)
        {
            mFirstScanIndex = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(index) + 1u);
            while (mFirstScanIndex <= mLastScanIndex && !mIsAllocated(l_Element_GetPtr(static_cast<std::uint32_t>(mFirstScanIndex))))
            {
                mFirstScanIndex = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(mFirstScanIndex) + 1u);
            }
        }
        if (index == mLastScanIndex)
        {
            mLastScanIndex = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(index) - 1u);
            while (mLastScanIndex >= mFirstScanIndex && !mIsAllocated(l_Element_GetPtr(static_cast<std::uint32_t>(mLastScanIndex))))
            {
                mLastScanIndex = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(mLastScanIndex) - 1u);
            }
        }
        return true;
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

        if (!io->IO_File_Chunk_CheckMarkerAndGetVersion(SaveChunkId, &version, nullptr))
            return;

        if (version != SaveVersion)
        {
            io->IO_File_Chunk_Skip();
            return;
        }

        Array_LoadStorage(this, io);
    }
}
