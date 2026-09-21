#include "MasterControlProgram.h"

#include <new>

extern char* FUN_00403029(char* object);
extern int* DAT_0050f63c;
extern void __fastcall FUN_00403182(int* object);
extern void __fastcall FUN_0040313f(int* object);

namespace NMasterControlProgram
{

    // Original function: FUN_004022f0
    bool CMasterControlProgram::System_ProgramState_PreIngameFmv_StartUp()
    {
        mProgramState = 14;

        auto* object = static_cast<char*>(::operator new(0xD0, std::nothrow));

        if (object != nullptr)
        {
            FUN_00403029(object);
        }

        mProgramState = static_cast<std::int32_t>(TProgramState::PreIngameFmv);

        return true;
    }

    // Original function: FUN_00402319
    void CMasterControlProgram::System_ProgramState_PreIngameFmv_ShutDown()
    {
        mProgramState = 16;

        int* preIngameFmvManager = DAT_0050f63c;

        if (preIngameFmvManager != nullptr)
        {
            FUN_0040313f(preIngameFmvManager);
            ::operator delete(preIngameFmvManager);
        }

        mProgramState = static_cast<std::int32_t>(TProgramState::None);
    }

    // Original function: FUN_00402344
    void CMasterControlProgram::System_ProgramState_PreIngameFmv_Update()
    {
        FUN_00403182(DAT_0050f63c);
    }
}
