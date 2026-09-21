#include "MasterControlProgram.h"

#include <new>

// Constructor/initializer called with the object pointer in ECX.
extern int* __fastcall FUN_004027d2(int* object);
extern int* DAT_0050f634;
extern void __fastcall FUN_00402a58(int* object);
extern void __fastcall FUN_00402a15(int* object);

namespace NMasterControlProgram
{

    // Original function: FUN_00402773
    bool CMasterControlProgram::System_ProgramState_TitleScreen_StartUp()
    {
        mProgramState = 4;

        auto* object = static_cast<int*>(::operator new(0xE8, std::nothrow));

        if (object != nullptr)
        {
            FUN_004027d2(object);
        }

        mProgramState = static_cast<std::int32_t>(TProgramState::TitleScreen);

        return true;
    }

    // Original function: FUN_0040279c
    void CMasterControlProgram::System_ProgramState_TitleScreen_ShutDown()
    {
        mProgramState = 6;

        int* titleScreenManager = DAT_0050f634;

        if (titleScreenManager != nullptr)
        {
            FUN_00402a15(titleScreenManager);
            ::operator delete(static_cast<void*>(titleScreenManager));
        }

        mProgramState = static_cast<std::int32_t>(TProgramState::None);
    }

    // Original function: FUN_004027c7
    void CMasterControlProgram::System_ProgramState_TitleScreen_Update()
    {
        FUN_00402a58(DAT_0050f634);
    }
}
