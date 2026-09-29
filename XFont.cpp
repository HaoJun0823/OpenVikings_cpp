#include "XFont.h"
#include "XBobManager.h"
#include "XFileAccess.h"
#include "XStorableIO.h"

#include <cstdint>

namespace NXBasics
{
    // Original function: FUN_00439690; deleting wrapper: FUN_00439674.
    CFont::~CFont()
    {
        if (mBobManager != nullptr)
        {
            delete mBobManager;
            mBobManager = nullptr;
        }
    }

    // Original function: FUN_004397ea; initialization helper: FUN_004396ad.
    CFont::CFont(void* file, std::uint32_t /*version*/) : mSerializedData{}, mCharacterSpacing(0), mBobManager(nullptr), mPalette(nullptr)
    {
        File_Read(file, mSerializedData, sizeof(mSerializedData));
        mBobManager = static_cast<CBobManager*>(XB_Storable_LoadObject(file));
    }

    // Original function: FUN_0043981a
    void CFont::Storable_SaveData(void* file)
    {
        File_Write(file, mSerializedData, sizeof(mSerializedData));
        mBobManager->Storable_Save(file);
    }
}