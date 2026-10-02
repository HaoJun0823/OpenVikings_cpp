#include "InGameGuiManager.h"
#include "InGameGuiBaseAccess.h"
#include "InGameGuiStringOutputWindow.h"
#include "InGameGuiSupportAccess.h"
#include "ApplicationGlobals.h"
#include "C2CallbackManager.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "NetworkShutdown.h"
#include "XCrtFileAccess.h"
#include "XGuiDesktop.h"
#include "XList.h"

#include <cstring>
#include <new>
#include <Windows.h>
#include <mmsystem.h>

// Original widget construction, desktop input/rendering and logic callback bodies.
extern bool __cdecl FUN_004b3264(std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
extern void FUN_004b1dde(); // CStatisticsWindow::Statics_Init
extern void FUN_004b00da(); // CHumanListWindow::Statics_Init
extern void FUN_004af057(); // CDiplomacyWindow::Statics_Init
extern void FUN_004aba22(); // CSelectionSingleHumanWindow::Statics_Init
extern void FUN_004a9946(); // CSelectionSingleVehicleWindow::Statics_Init
extern void FUN_004a7142(); // CSelectionHouseWindow::Statics_Init
extern void FUN_004a5ff5(); // CTechTreeWindow::Statics_Init
extern void FUN_004a59ab(); // CMissionInfoWindow::MissionBriefingHistory_Init
extern void __fastcall FUN_004a299b(void* object, void* unusedEdx, bool update);
extern void __fastcall FUN_004a2a55(void* object, void* unusedEdx, bool update);
extern std::uint32_t __fastcall FUN_004a1bf0(void* object);
extern void __fastcall FUN_004a10cb(void* object);
extern bool __fastcall FUN_004b4854(void* object, void* unusedEdx, std::int32_t mode);
extern std::uint32_t __fastcall FUN_00477104(void* desktop, void* unusedEdx, bool clearBitmap, bool redraw);
extern NXBaseGui::CBaseElement* __fastcall FUN_004780fc(void* desktop);

namespace
{
    using namespace NC2Lifetime;
    using TMessage = std::uint32_t(__thiscall*)(void*, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);

    void SendGuiMessage(void* element, std::uint32_t command, std::uint32_t param1)
    {
        TMessage* methods = Read<TMessage*>(element, 0);
        methods[2](element, 0x800, command, param1, 0, 0);
    }

    // Original function: FUN_00479975; CFrameRateCalculator::CalculateFrameRate(tick).
    double CalculateFrameRate(void* calculator, std::uint32_t tick)
    {
        const auto previous = Read<std::uint32_t>(calculator, 0);
        if (tick < previous) tick = previous;
        const auto position = Read<std::uint32_t>(calculator, 0x7C);
        Write(calculator, 4u + position * 4u, tick - previous);
        const auto next = position + 1u;
        Write<std::uint32_t>(calculator, 0x7C, next == 30 ? 0 : next);
        Write(calculator, 0, tick);
        std::uint32_t total = 0;
        for (std::uint32_t index = 0; index < 30; ++index) total += Read<std::uint32_t>(calculator, 4u + index * 4u);
        double interval = static_cast<double>(total) * static_cast<double>(0.03333333507180214f);
        interval *= static_cast<double>(0.0010000000474974513f);
        const double rate = 1.0 / interval;
        Write<float>(calculator, 0x80, static_cast<float>(rate));
        return rate;
    }

    // Original function: FUN_00479962; CFrameRateCalculator::CalculateFrameRate().
    double CalculateFrameRate(void* calculator)
    {
        return CalculateFrameRate(calculator, timeGetTime());
    }
}

namespace NInGameGuiManager
{
    using namespace NC2Lifetime;

    // Original function: FUN_004a0184.
    void CInGameGuiManager::System_UpdateGui()
    {
        FrameCall_Do();
        l_Selection_ActionButtons_CheckUpdate();
        CalculateFrameRate(reinterpret_cast<std::byte*>(this) + 0x110);
        FUN_004a299b(sTheObjectPtr, nullptr, false);
        FUN_004a2a55(sTheObjectPtr, nullptr, false);
        sTheObjectPtr->l_UpdateElement0038();
        FUN_00477104(NXBaseGui::CDesktop::sTheObjectPtr, nullptr, false, mStaticVars.redrawRequested);
        auto* desktop = NXBaseGui::CDesktop::sTheObjectPtr;
        mStaticVars.redrawRequested = false;
        auto* underMouse = FUN_004780fc(desktop);
        if (underMouse != nullptr && underMouse == Read<NXBaseGui::CBaseElement*>(this, 4))
            NXBaseGui::CDesktop::sTheObjectPtr->Element_Activate(Read<NXBaseGui::CBaseElement*>(this, 4));
    }

    // Original function: FUN_004a0205; System_StartGame does not use ECX.
    void CInGameGuiManager::System_StartGame()
    {
        using namespace NC2InGameGuiInputManager;
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0xF7C, 1);
        if (allocation != nullptr) new (allocation) CInputManager;
        allocation = NXBasics::Runtime_AllocateWithNewHandler(0x18, 1);
        if (allocation != nullptr) new (allocation) NC2InGameGuiMessages::CMessageManager;
        allocation = NXBasics::Runtime_AllocateWithNewHandler(0x2F0C, 1);
        if (allocation != nullptr) new (allocation) NC2InGameGuiGroups::CObjectGroupManager;
        allocation = NXBasics::Runtime_AllocateWithNewHandler(0xC, 1);
        if (allocation != nullptr) new (allocation) NC2InGameGuiDeadHumanDataManager::CDeadHumanDataManager;
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x19, FUN_004b3264, 0, 10);
        FUN_004b1dde();
        FUN_004b00da();
        FUN_004af057();
        FUN_004aba22();
        FUN_004a9946();
        FUN_004a7142();
        FUN_004a5ff5();
        FUN_004a59ab();
    }

    // Original function: FUN_004a0295; reverse owner destruction order.
    void CInGameGuiManager::System_EndGame()
    {
        NC2Logic::CCallbackManager::sTheObjectPtr->UnRegisterCallback(0x19, FUN_004b3264, 0);
        auto* deadHumans = static_cast<NC2InGameGuiDeadHumanDataManager::CDeadHumanDataManager*>(NC2InGameGuiDeadHumanDataManager::gDeadHumanDataManager);
        if (deadHumans != nullptr)
        {
            deadHumans->~CDeadHumanDataManager();
            NXBasics::Runtime_Free(deadHumans);
        }
        auto* groups = static_cast<NC2InGameGuiGroups::CObjectGroupManager*>(NC2InGameGuiGroups::gObjectGroupManager);
        if (groups != nullptr)
        {
            groups->~CObjectGroupManager();
            NXBasics::Runtime_Free(groups);
        }
        auto* messages = static_cast<NC2InGameGuiMessages::CMessageManager*>(NC2InGameGuiMessages::gMessageManager);
        if (messages != nullptr)
        {
            messages->~CMessageManager();
            NXBasics::Runtime_Free(messages);
        }
        auto* input = static_cast<NC2InGameGuiInputManager::CInputManager*>(NC2InGameGuiInputManager::gInputManager);
        if (input != nullptr)
        {
            input->~CInputManager();
            NXBasics::Runtime_Free(input);
        }
    }

    // Original function: FUN_004a1bc3.
    void CInGameGuiManager::l_Selection_ActionButtons_CheckUpdate()
    {
        if (Read<std::uint32_t>(this, 0xCC) == 0) return;
        const auto checksum = FUN_004a1bf0(this);
        if (checksum == Read<std::uint32_t>(this, 0xCC)) return;
        l_Selection_RemoveAllGuiElements();
        FUN_004a10cb(this);
    }

    // Original function: FUN_004a2436; element role remains unresolved.
    bool CInGameGuiManager::l_IsElement0038Required() const
    {
        if (NNetwork::Network_IsInSession()) return true;
        return Read<std::uint8_t>(NC2Logic::gMultiPlayerGoalManager, 0) != 0;
    }

    // Original function: FUN_004a244e; complete table 004F4DA0..004F4DCF.
    void CInGameGuiManager::MiscWindows_CloseAllLargeOnes(bool resetInputMode)
    {
        static constexpr std::uint32_t commands[] = {0x7D8, 0x7D9, 0x7E3, 0x7E4, 0x7E5, 0x7E6, 0x7E7, 0x7E8, 0x7E9, 0x7EC, 0x7DA, 0xFFFFFFFF};
        for (auto command : commands)
        {
            if (command == 0xFFFFFFFFu) break;
            auto* window = NXBaseGui::CDesktop::sTheObjectPtr->Message_SendToAllWindows(command, 0, 0, 0);
            if (window != nullptr) NXBaseGui::CDesktop::sTheObjectPtr->Window_Close(reinterpret_cast<NXBaseGui::CBaseWindow*>(window));
        }
        if (resetInputMode) FUN_004b4854(NC2InGameGuiInputManager::gInputManager, nullptr, 0);
    }

    // Original function: FUN_004a2598.
    void CInGameGuiManager::MiscWindows_StringOutputWindow_Open()
    {
        if (NXBaseGui::CDesktop::sTheObjectPtr->Message_SendToAllWindows(0x7DC, 0, 0, 0) != nullptr) return;
        MiscWindows_CloseAllLargeOnes(false);
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x74, 1);
        void* window = allocation == nullptr ? nullptr : NC2InGameGuiMiscWindows::StringOutputWindow_Construct(allocation);
        NC2InGameGuiBase::BaseToolDesktop_AddWindow(static_cast<NXBaseGui::CBaseWindow*>(window), false);
    }

    // Original function: FUN_004a25e0.
    void CInGameGuiManager::MiscWindows_StringOutputWindow_PrintString(const char* text)
    {
        MiscWindows_StringOutputWindow_Open();
        void* window = NXBaseGui::CDesktop::sTheObjectPtr->Message_SendToAllWindows(0x7DC, 0, 0, 0);
        if (window != nullptr) SendGuiMessage(window, 0x4E20, static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(text)));
    }

    // Original function: FUN_004a2653.
    void CInGameGuiManager::MiscWindows_StringOutputWindow_ClearAll()
    {
        void* window = NXBaseGui::CDesktop::sTheObjectPtr->Message_SendToAllWindows(0x7DC, 0, 0, 0);
        if (window != nullptr) SendGuiMessage(window, 0x4E21, 0);
    }

    // Original function: FUN_004a2acd.
    void CInGameGuiManager::l_UpdateElement0038()
    {
        if (Read<void*>(this, 0x38) == nullptr || l_IsElement0038Required()) return;
        NXBaseGui::CDesktop::sTheObjectPtr->Element_Delete(Read<NXBaseGui::CBaseElement*>(this, 0x38));
        Write<void*>(this, 0x38, nullptr);
    }

    // Original function: FUN_004a2d80; callback mutation follows the original iterator.
    void CInGameGuiManager::FrameCall_Do()
    {
        auto* list = std::launder(reinterpret_cast<NXBasics::CListBase*>(mLegacyState + 0x4C));
        NXBasics::CListBase::Iterator iterator(*list);
        for (void* element = iterator.First(); element != nullptr; element = iterator.Next()) SendGuiMessage(element, 0x7EE, 0);
    }
}
