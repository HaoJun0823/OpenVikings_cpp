#include "InGameGuiManager_MessageHandlers.h"
#include "C2LifetimeAccess.h"
#include "XGuiElementAccess.h"
#include "XCrtFileAccess.h"

namespace
{
    // Complete original table: 004F4DE0..004F4E27. Slot two remains partially reconstructed.
    const std::uintptr_t gInGameDefaultMessageHandlerVirtualTable[18] =
    {
        reinterpret_cast<std::uintptr_t>(&InGameGuiManagerDefaultMessageHandlerElement_Delete),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_HasSubElements),
        reinterpret_cast<std::uintptr_t>(&InGameGuiManagerDefaultMessageHandlerElement_XGui_BE_Message_Handle),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Draw),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Activate),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_DeActivate),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ButtonPressed),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ButtonReleased),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_IsHit),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_GetRealActiveElementPtr),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_FindElementOnPosition),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ParentToLocalPosition),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Notify),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ToolTip_Available),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ToolTip_AtOnce),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Hide_CanBeHidden),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_UnresolvedVirtual40),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Notify)
    };
    static_assert(sizeof(gInGameDefaultMessageHandlerVirtualTable) == 0x48);
}

// Original function: FUN_004a0145; flags zero destroy the embedded handler without freeing it.
void* __fastcall InGameGuiManagerDefaultMessageHandlerElement_Delete(void* element, void* /*unusedEdx*/, std::uint32_t flags)
{
    NC2Lifetime::Write(element, 0, gInGameDefaultMessageHandlerVirtualTable + 0);
    NXBaseGui::BaseElement_Destruct(element);
    if ((flags & 1u) != 0) NXBasics::Runtime_Free(element);
    return element;
}

// Original function: FUN_004a3eab; CGuiManagerDefaultMessageHandlerElement constructor.
void* __fastcall InGameGuiManagerDefaultMessageHandlerElement_Construct(void* element)
{
    const NXBasics::SRectangle rectangle(0, 0, 0, 0);
    NXBaseGui::BaseElement_Construct(element, &rectangle, 0, 0x800);
    NC2Lifetime::Write(element, 0, gInGameDefaultMessageHandlerVirtualTable + 0);
    return element;
}
