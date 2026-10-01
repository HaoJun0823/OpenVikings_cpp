#include "C2IoHelper.h"
#include "XCrtFileAccess.h"
#include "XFile.h"
#include "XMemory.h"
#include "XMemoryChecksum.h"

#include <cstdarg>
#include <cstdint>
#include <cstring>

namespace NC2Logic
{
    // Original function: FUN_00429f34
    CIoHelper::CIoHelper()
    {
        std::memset(this, 0, sizeof(*this));
    }

    // Original function: FUN_00429f48
    CIoHelper::~CIoHelper()
    {
        if (mStringBuffer != nullptr)
            NXBasics::Runtime_Free(mStringBuffer);
    }

    // Original function: FUN_00429f57
    void CIoHelper::IO_File_Init(NXBasics::CFile* file)
    {
        mFile = file;
        mStringBuffer = static_cast<char*>(NXBasics::Runtime_AllocateWithNewHandler(200u, 1));

        mPayloadPosition = 0xFFFFFFFFu;
        mReadHeader.signature = 0x78696F68u;
        mReadHeader.marker = 0;
        mReadHeader.version = 0;
        mReadHeader.payloadSize = 0;
    }

    // Original function: FUN_00429f88
    void CIoHelper::IO_File_ReadBinary(void* destination, std::uint32_t byteCount)
    {
        mFile->Read(destination, byteCount);
    }

    // Original function: FUN_0042a02d
    std::uint32_t CIoHelper::IO_File_ReadLong()
    {
        std::uint32_t value;
        mFile->Read(&value, sizeof(value));
        return value;
    }

    // Original function: FUN_0042a059.
    const char* CIoHelper::IO_File_ReadString()
    {
        std::uint8_t length;
        mFile->Read(&length, 1);
        if (length >= 199)
        {
            const std::uint32_t position = mFile->GetPosition();
            mFile->SeekToPosition(position + length + 1u);
            std::strcpy(mStringBuffer, " ");
        }
        else
            mFile->Read(mStringBuffer, static_cast<std::uint32_t>(length) + 1u);
        return mStringBuffer;
    }

    // Original function: FUN_0042a13e
    bool CIoHelper::IO_File_Chunk_CheckMarkerAndGetVersion(std::uint32_t marker, std::uint32_t* version, std::uint32_t* payloadSize)
    {
        mFile->Read(&mReadHeader, sizeof(mReadHeader));
        mPayloadPosition = mFile->GetPosition();

        if (mReadHeader.signature != 0x78696F68u)
        {
            mPayloadPosition = 0xFFFFFFFFu;
            return false;
        }

        if (version != nullptr)
            *version = mReadHeader.version;

        if (payloadSize != nullptr)
            *payloadSize = mReadHeader.payloadSize;

        return mReadHeader.marker == marker;
    }

    // Original function: FUN_0042a194.
    bool CIoHelper::IO_File_Chunk_ReadHeader()
    {
        mFile->Read(&mReadHeader, sizeof(mReadHeader));
        mPayloadPosition = mFile->GetPosition();
        if (mReadHeader.signature != 0x78696F68u)
        {
            mPayloadPosition = 0xFFFFFFFFu;
            return false;
        }
        return mReadHeader.marker != 0x78656E64u;
    }

    // Original function: FUN_0042a1ce
    void CIoHelper::IO_File_Chunk_Skip()
    {
        if (mPayloadPosition == 0xFFFFFFFFu)
            return;

        mFile->SeekToPosition(mPayloadPosition + mReadHeader.payloadSize);

        std::memset(&mReadHeader, 0, sizeof(mReadHeader));
        mPayloadPosition = 0xFFFFFFFFu;
    }

    // Original function: FUN_0042a1fc
    void CIoHelper::IO_Memory_Init(NXBasics::CMemory* memory)
    {
        mMemoryUsage = 0;
        mMemory = memory;
    }

    // Original function: FUN_0042a244
    void CIoHelper::IO_Memory_WriteLong(std::uint32_t value)
    {
        if (mMemoryUsage + 4u > mMemory->mSize)
            mMemory->ChangeBufferSize(mMemory->mSize + 10000u, false, 0xEEu);

        std::memcpy(mMemory->mData + mMemoryUsage, &value, sizeof(value));
        mMemoryUsage += 4u;
    }

    // Original function: FUN_0042a2e4
    void CIoHelper::IO_Memory_WriteBinary(const void* data, std::uint32_t byteCount)
    {
        if (mMemoryUsage + byteCount > mMemory->mSize)
            mMemory->ChangeBufferSize(mMemory->mSize + byteCount + 10000u, false, 0xEEu);

        std::memcpy(mMemory->mData + mMemoryUsage, data, byteCount);
        mMemoryUsage += byteCount;
    }

    // Original function: FUN_0042a3c6
    void CIoHelper::IO_Tool_Chunk_StartCreation(std::uint32_t marker, std::uint32_t version)
    {
        if (mChunkDepth == 5u)
            return;

        if (mChunkDepth == 0)
            mMemoryUsage = 0;

        mChunkOffsets[mChunkDepth] = mMemory != nullptr ? mMemoryUsage : 0u;

        SIoChunkHeader header = {};
        header.signature = 0x78696F68u;
        header.marker = marker;
        header.version = version;
        header.payloadSize = 0;
        header.nestingDepth = mChunkDepth;

        IO_Memory_WriteBinary(&header, sizeof(header));

        ++mChunkDepth;
    }

    // Original function: FUN_0042a431
    void CIoHelper::IO_Tool_Chunk_FinishAndWrite()
    {
        if (mChunkDepth == 0)
            return;

        --mChunkDepth;

        const std::uint32_t headerOffset = mChunkOffsets[mChunkDepth];
        const std::uint32_t currentUsage = mMemory != nullptr ? mMemoryUsage : 0u;
        const std::uint32_t payloadSize = currentUsage - headerOffset - 0x20u;

        const std::uintptr_t bufferAddress = mMemory != nullptr ? reinterpret_cast<std::uintptr_t>(mMemory->mData) : 0u;
        auto* headerBytes = reinterpret_cast<std::uint8_t*>(bufferAddress + headerOffset);

        std::memcpy(headerBytes + 0x0C, &payloadSize, sizeof(payloadSize));

        const std::uint32_t checksum = NXBasics::XB_GetMemoryChecksum(headerBytes + 0x20, payloadSize);
        std::memcpy(headerBytes + 0x14, &checksum, sizeof(checksum));

        if (mChunkDepth == 0)
        {
            const void* data = mMemory != nullptr ? mMemory->mData : nullptr;
            const std::uint32_t byteCount = mMemory != nullptr ? mMemoryUsage : 0u;

            mFile->Write(data, byteCount);
            mMemoryUsage = 0;
        }
    }
}
