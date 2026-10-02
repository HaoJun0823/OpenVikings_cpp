#include "C2HumanCommandAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include <cstring>
namespace NC2Logic
{
    extern void __fastcall FUN_00441cb4(void* human, void* unusedEdx);

    // Original function: FUN_00443dab.
    bool Human_StopWaitingForOtherHuman(void* human)
    {
        using namespace NC2Lifetime;
        const auto waiting = Read<std::uint32_t>(human, 0x378);
        if (waiting == 0)
        {
            Write<std::uint32_t>(human, 0xB1C, 0xFFFFFFFFu);
            return false;
        }
        Write(human, 0x378, waiting - 1u);
        if (waiting == 1)
        {
            Write<std::uint32_t>(human, 0xB1C, 0xFFFFFFFFu);
            if (Read<std::uint32_t>(human, 0x368) == 9) Write<std::uint32_t>(human, 0x368, 1);
        }
        return true;
    }

    // Original function: FUN_00438d0a; the other human's occupied flag is checked.
    static void Human_StopWaitingForOtherHuman_Detach(void* human)
    {
        using namespace NC2Lifetime;
        const auto otherId = Read<std::uint32_t>(human, 0xB1C);
        if (otherId == 0xFFFFFFFFu) return;
        auto* other = CHumanArray::mStaticVars + otherId * CHumanArray::ElementSize;
        if (Read<std::uint8_t>(other, 0) != 0) Human_StopWaitingForOtherHuman(other);
        Write<std::uint32_t>(human, 0xB1C, 0xFFFFFFFFu);
    }

    // Original function: FUN_004355fc.
    static std::uint32_t Human_UserControl_GetLastCommandType(const void* human)
    {
        return NC2Lifetime::Read<std::uint32_t>(human, 0xA5C) + 2u;
    }

    // Original function: FUN_0044320e; Windows tests atomic interruptibility once.
    void Human_CurrentTask_DoInterrupt(void* human)
    {
        using namespace NC2Lifetime;
        if (!Human_l_IsAtomicInterruptable(human)) return;
        Write<std::uint32_t>(human, 0x37C, 0);
        Write<std::uint32_t>(human, 0x378, 0);
        Write<std::uint32_t>(human, 0x380, 0);
        switch (Read<std::uint32_t>(human, 0x368))
        {
        case 2:
        case 8:
            std::memcpy(static_cast<std::byte*>(human) + 0xA84, static_cast<const std::byte*>(human) + 0xA68, 28);
            Write(human, 0xA94, Read<std::uint32_t>(human, 0xAD0));
            Write(human, 0xAD8, Read<std::uint32_t>(human, 0xACC));
            break;
        case 3:
            Human_DoAddNewUserCommand(human, 6, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), 0, 0);
            break;
        case 4:
            Write<std::uint32_t>(human, 0xA84, 1);
            Write(human, 0xA88, Read<std::uint32_t>(human, 0xA6C));
            break;
        case 5:
        {
            const auto otherId = Read<std::uint32_t>(human, 0xB1C);
            if (otherId != 0xFFFFFFFFu)
            {
                auto* other = CHumanArray::mStaticVars + otherId * CHumanArray::ElementSize;
                Human_StopWaitingForOtherHuman(other);
                Human_StopWaitingForOtherHuman_Detach(other);
            }
            Human_DoAddNewUserCommand(human, 14, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), 0, 0);
            break;
        }
        case 11:
            Human_DoAddNewUserCommand(human, 15, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), 0, 0);
            break;
        case 12:
            Human_DoAddNewUserCommand(human, 16, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), 0, 0);
            break;
        case 16:
        case 17:
            Write(human, 0x390u + Read<std::uint32_t>(human, 0xA5C) * 20u, static_cast<std::uint8_t>(Human_UserControl_GetLastCommandType(human)));
            break;
        case 18:
            Human_DoAddNewUserCommand(human, 18, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), MapMigPoint_GetLong(static_cast<const std::byte*>(human) + 0xA70), 0);
            break;
        case 20:
            Human_DoAddNewUserCommand(human, 20, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), MapMigPoint_GetLong(static_cast<const std::byte*>(human) + 0xB30), 0);
            break;
        default: break;
        }
        FUN_00441cb4(human, nullptr);
        Write<std::uint32_t>(human, 0xAD0, 0);
        Write<std::uint32_t>(human, 0x370, 0);
        Human_StopWaitingForOtherHuman_Detach(human);
        Write<std::uint32_t>(human, 0x36C, 0);
        Write<std::uint32_t>(human, 0x368, 1);
    }

    // Original function: FUN_0045cc64; an inactive non-moveable atomic is interruptable.
    bool Human_l_IsAtomicInterruptable(const void* human)
    {
        using namespace NC2Lifetime;
        if (Read<std::uint32_t>(human, 0xAB8) == 1)
        {
            const auto* moveable = gMapMoveableArrayStorage.entries + Read<std::uint32_t>(human, 0x0C) * 0x58u;
            return Read<std::uint8_t>(moveable, 0x2C) == 0;
        }
        if (Read<std::uint8_t>(human, 0xABC) == 0) return true;
        return Read<std::uint8_t>(gStaticAtomicAnimationData, Read<std::uint32_t>(human, 0xAC0) * 0x138u + 0x38u) != 0;
    }

    // Original function: FUN_0043bb2d; the structured-command overload delegates.
    bool Human_DoAddNewUserCommand(void* human, const void* command)
    {
        using namespace NC2Lifetime;
        return Human_DoAddNewUserCommand(human, Read<std::uint8_t>(command, 0), Read<std::uint8_t>(command, 1), Read<std::uint32_t>(command, 4), Read<std::uint32_t>(command, 8), Read<std::uint32_t>(command, 12), Read<std::uint32_t>(command, 16));
    }

    // Original function: FUN_0043bb4e; the opcode selects an unchecked 20-byte slot.
    bool Human_DoAddNewUserCommand(void* human, std::uint8_t opcode, std::uint8_t player, std::uint32_t id, std::uint32_t revision, std::uint32_t parameter1, std::uint32_t parameter2)
    {
        using namespace NC2Lifetime;
        if (opcode == 0) return true;
        if (opcode == 4)
        {
            if (Read<std::uint32_t>(human, 0xAB8) == 1 && Read<std::uint32_t>(human, 0x368) != 15)
            {
                std::uint32_t target;
                MapMigPoint_AssignLong(&target, parameter1);
                if (target == Read<std::uint32_t>(human, 0xA6C)) return true;
            }
            Write<std::uint32_t>(human, 0x37C, 0);
            Write<std::uint8_t>(human, 0x4F8, 0);
            Write<std::uint8_t>(human, 0x4E4, 0);
        }
        if (opcode >= 0x2B && opcode <= 0x2F)
        {
            Write<std::uint32_t>(human, 0x37C, 0);
            for (const auto offset : {0x9D0u, 0x9E4u, 0x9F8u, 0xA0Cu, 0xA20u, 0xA34u, 0x994u, 0x9A8u, 0x9BCu}) Write<std::uint8_t>(human, offset, 0);
        }
        const auto offset = 0x390u + (static_cast<std::uint32_t>(opcode) - 2u) * 20u;
        Write(human, offset, opcode);
        Write(human, offset + 1, player);
        Write(human, offset + 4, id);
        Write(human, offset + 8, revision);
        Write(human, offset + 12, parameter1);
        Write(human, offset + 16, parameter2);
        if (Read<std::uint32_t>(human, 0x368) == 1 && Human_l_IsAtomicInterruptable(human)) Human_CurrentTask_DoInterrupt(human);
        return true;
    }
}
