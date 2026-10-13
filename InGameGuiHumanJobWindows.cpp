#include "C2ObjectLayout.h"
#include "InGameGuiHumanJobWindows.h"
#include "InGameGuiBaseAccess.h"
#include "InGameGuiInputAccess.h"
#include "InGameGuiManager.h"
#include "C2E2HumanSoundAccess.h"
#include "C2EngineDrawingAccess.h"
#include "C2EngineObjectData.h"
#include "C2GameState.h"
#include "C2HumanJobAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2StaticTribeTypeData.h"
#include "C2StringAccess.h"
#include "GuiBitmapToolElementAccess.h"
#include "GuiListBoxElementAccess.h"
#include "GuiTextToolElementAccess.h"
#include "GuiToolsBase.h"
#include "NetworkShutdown.h"
#include "StringInputHandler.h"
#include "XBitmap.h"
#include "XCrtFileAccess.h"
#include "XGuiDesktop.h"
#include "XGuiWindowAccess.h"
#include <array>
#include <cstring>
#include <new>

namespace NC2InGameGuiSelectedAction
{
    using namespace NC2Lifetime;
    using NXBasics::SRectangle;
    namespace
    {
        void* Input() { return NC2InGameGuiInputManager::gInputManager; }
        std::int32_t SelectedId() { return NC2InGameGuiInputManager::Selection_GetSelectedSingleHumanId(Input()); }
        void* Human(std::int32_t id) { return NC2Logic::CHumanArray::mStaticVars + static_cast<std::intptr_t>(id) * NC2Logic::CHumanArray::ElementSize; }
        const char* Name(std::int32_t id) { return reinterpret_cast<const char*>(NC2E2::gDynamicDataHumans.entries) + static_cast<std::intptr_t>(id) * 0x8C8 + 0x85C; }
        const char* Misc(std::uint32_t id) { return NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(id); }
        auto* TextInput(void* window) { return reinterpret_cast<NC2InGameGuiBaseTextInputElement::CStringInputHandler*>(static_cast<std::byte*>(window) + 0x78); }
        void Close(void* window) { NXBaseGui::CDesktop::sTheObjectPtr->Window_AddToCloseList(static_cast<NXBaseGui::CBaseWindow*>(window)); }
        const NXBasics::SColorRGB* White() { return reinterpret_cast<const NXBasics::SColorRGB*>(reinterpret_cast<const std::byte*>(NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.fontWhitePalette) + 0x400); }
        void Queue(std::int32_t id, std::uint8_t opcode, std::uint32_t value)
        {
            std::byte command[20]{};
            Write(command, 0, opcode); Write(command, 4, id); Write(command, 0x0C, value);
            NC2Logic::UserNetworkCommand_AddHumanCommand(NC2Logic::gUserNetworkCommandManager, command);
        }
        void Respond(std::int32_t id) { NC2E2::E2HumanSoundManager_PlayRespondingSound(NC2E2::gE2HumanSoundManager, 0, id); }
        void Frame(void* window, std::uint32_t title)
        {
            const auto width = Read<std::int32_t>(window, 0x0C);
            void* memory = NXBasics::Runtime_AllocateWithNewHandler(248, 1);
            const SRectangle mover(2, 2, width - 22, 14);
            NXBaseGui::BaseWindow_Element_Add(window, memory == nullptr ? nullptr : NC2InGameGuiBase::WindowMoverButton_Construct(memory, &mover, Misc(title)));
            memory = NXBasics::Runtime_AllocateWithNewHandler(84, 1);
            const SRectangle closer(width - 18, 2, 16, 14);
            NXBaseGui::BaseWindow_Element_Add(window, memory == nullptr ? nullptr : NC2InGameGuiBase::WindowCloserButton_Construct(memory, nullptr, &closer));
        }
        void* List(const SRectangle& rectangle)
        {
            void* memory = NXBasics::Runtime_AllocateWithNewHandler(116, 1);
            return memory == nullptr ? nullptr : ToolListBoxElement_Construct(memory, nullptr, &rectangle);
        }
        void AddList(void* window, void* list, const SRectangle& rectangle)
        {
            NXBaseGui::BaseWindow_Element_Add(window, list);
            void* memory = NXBasics::Runtime_AllocateWithNewHandler(60, 1);
            NXBaseGui::BaseWindow_Element_Add(window, memory == nullptr ? nullptr : ToolListBoxControllerElement_Construct(memory, nullptr, &rectangle, list, false, true));
        }
        void Row(void* window, void* list, SRectangle& rectangle, const char* caption, std::uint32_t job, std::uint32_t good, bool pressed)
        {
            void* memory = NXBasics::Runtime_AllocateWithNewHandler(96, 1);
            void* element = memory == nullptr ? nullptr : NC2InGameGuiBase::BaseToolTextButton_Construct(memory, nullptr, &rectangle, caption, 10000, window);
            if (element != nullptr) { Write(element, 0x4C, job); Write(element, 0x50, good); if (pressed) Write<std::uint8_t>(element, 0x55, 1); }
            BaseListBoxElement_Element_Add(list, nullptr, element);
            rectangle.y += 24;
        }
        // Original function: FUN_004b1a6d; shared Windows job-name comparator.
        std::int32_t __cdecl lsSortCompareFunction(const void* first, const void* second)
        {
            const char* secondName = NC2Logic::StringAccess_GetJobNamePtr(*static_cast<const std::uint32_t*>(second), 1);
            return NXBasics::Runtime_CompareStringsIgnoreCase(NC2Logic::StringAccess_GetJobNamePtr(*static_cast<const std::uint32_t*>(first), 1), secondName);
        }
        // Original function: FUN_004c8238; folded single-human validation also used by the name and production windows.
        void SelectedSingleHumanLearnJobWindow_l_System_Validate(void* window)
        {
            const auto saved = Read<std::uint32_t>(window, 0x74);
            const auto current = Read<std::uint32_t>(Input(), 0xAB0);
            if (Read<std::uint32_t>(Input(), 4) != 1 || (saved != 0 && saved != current))
            {
                Write<std::uint32_t>(window, 0x74, 0); Write<std::uint8_t>(window, 0x70, 0); Close(window);
            }
            else { Write(window, 0x74, current); Write<std::uint8_t>(window, 0x70, 1); }
        }
        // Original function: FUN_004c7636.
        void SelectedSingleHumanChangeJobWindow_l_System_Validate(void* window)
        {
            const auto type = Read<std::uint32_t>(Input(), 4);
            const auto saved = Read<std::uint32_t>(window, 0x74);
            const auto current = Read<std::uint32_t>(Input(), 0xAB0);
            if ((type != 1 && type != 2) || (saved != 0 && saved != current))
            {
                Write<std::uint32_t>(window, 0x74, 0); Write<std::uint8_t>(window, 0x70, 0); Close(window);
            }
            else { Write(window, 0x74, current); Write<std::uint8_t>(window, 0x70, 1); }
        }
        // Original function: FUN_004c76bd.
        void SelectedSingleHumanChangeJobWindow_l_GuiButtons_Add(void* window)
        {
            Frame(window, 430);
            SelectedSingleHumanChangeJobWindow_l_System_Validate(window);
            if (Read<std::uint8_t>(window, 0x70) == 0) return;
            void* list = List(SRectangle(4, 22, 268, 276));
            Write(window, 0x7C, list);
            bool allowed[56]; std::memset(allowed, 1, sizeof(allowed));
            const auto selection = Read<std::uint32_t>(Input(), 4);
            std::uint32_t currentJob = 0;
            if (selection == 1)
            {
                const void* human = Human(SelectedId());
                for (std::uint32_t job = 0; job < 56; ++job) if (!NC2Logic::Human_IsAbleToDoJob(human, job)) allowed[job] = false;
                currentJob = Read<std::uint32_t>(human, 0x18);
            }
            else if (selection == 2)
            {
                for (std::uint32_t index = 0; index < NC2InGameGuiInputManager::Selection_GetSelectedMultipleHumanNumber(Input()); ++index)
                {
                    const void* human = Human(NC2InGameGuiInputManager::Selection_GetSelectedMultipleHumanId(Input(), index));
                    for (std::uint32_t job = 0; job < 56; ++job) if (!NC2Logic::Human_IsAbleToDoJob(human, job)) allowed[job] = false;
                }
            }
            else std::memset(allowed, 0, sizeof(allowed));
            std::uint32_t jobs[56], count = 0;
            for (std::uint32_t job = 0; job < 56; ++job) if (allowed[job]) jobs[count++] = job;
            if (count > 1) NXBasics::Runtime_QuickSort(jobs, count, 4, lsSortCompareFunction);
            SRectangle row(0, 0, Read<std::int32_t>(list, 0x0C), 22);
            // Complete native six-entry region 004F7680..004F76AF.
            constexpr std::uint32_t farmerGoods[6][2]{{5, 0}, {2, 0}, {3, 0}, {14, 0}, {6, 1}, {7, 1}};
            for (std::uint32_t index = 0; index < count; ++index)
            {
                const auto job = jobs[index];
                Row(window, list, row, NC2Logic::StringAccess_GetJobNamePtr(job, 1), job, 0, currentJob != 0 && job == currentJob);
                if (job != 8 || selection != 1) continue;
                const void* human = Human(SelectedId());
                const auto player = Read<std::int32_t>(human, 0x10);
                const auto tribe = player < 20 ? Read<std::uint32_t>(NC2Logic::gGameControl, player * NC2Layout::RecordSize::GameControlPlayerSigned + 0x0C) : 1u;
                for (const auto& entry : farmerGoods)
                {
                    if (entry[1] != 0 && !NC2Logic::Human_IsAbleToProduceGoodNow(human, entry[0])) continue;
                    if (Read<std::uint8_t>(NC2Logic::gGameControl, player * NC2Layout::RecordSize::GameControlPlayerSigned + 0x1278 + tribe * 66 + entry[0]) == 0) continue;
                    char caption[200];
                    NXBasics::Runtime_PrintStringN(caption, sizeof(caption), "%s - %s", NC2Logic::StringAccess_GetJobNamePtr(8, 1), NC2Logic::StringAccess_GetGoodNamePtr(entry[0], 1));
                    Row(window, list, row, caption, 8, entry[0], false);
                }
            }
            AddList(window, list, SRectangle(276, 22, 20, 276));
            Write(window, 0x78, static_cast<std::uint32_t>(timeGetTime()));
        }
        // Original function: FUN_004c767a.
        void SelectedSingleHumanChangeJobWindow_l_GuiButtons_Update(void* window)
        {
            const void* list = Read<void*>(window, 0x7C);
            const auto scroll = list == nullptr ? 0 : Read<std::int32_t>(list, 0x34);
            NXBaseGui::BaseWindow_Element_DeleteAll(window); Write<void*>(window, 0x7C, nullptr);
            SelectedSingleHumanChangeJobWindow_l_GuiButtons_Add(window);
            if (void* replacement = Read<void*>(window, 0x7C)) NC2InGameGuiBaseListBox::BaseListBoxElement_ScrollOffset_Set(replacement, 0, scroll);
        }
        // Original function: FUN_004c7415.
        bool __fastcall SelectedSingleHumanChangeJobWindow_MessageHandle(void* window, void*, std::uint32_t message, std::uint32_t command, std::uint32_t, std::uint32_t job, std::uint32_t good)
        {
            if (message == 0x1000)
            {
                if (command == 2013) { SelectedSingleHumanChangeJobWindow_l_GuiButtons_Update(window); return true; }
                if (command == 2029) SelectedSingleHumanChangeJobWindow_l_System_Validate(window);
                return false;
            }
            if (message != 0x800) return false;
            if (command == 2030)
            {
                if (static_cast<std::uint32_t>(timeGetTime()) > Read<std::uint32_t>(window, 0x78) + 2000u) SelectedSingleHumanChangeJobWindow_l_GuiButtons_Update(window);
            }
            else if (command == 10000)
            {
                const auto selection = Read<std::uint32_t>(Input(), 4);
                if (selection == 1)
                {
                    const auto id = SelectedId(); Queue(id, 21, job); if (good != 0) Queue(id, 22, good); Respond(id);
                    if (Read<std::uint8_t>(NC2Logic::gStaticJobTypeData, job * NC2Layout::RecordSize::JobType + 0x86u) != 0)
                    {
                        NC2InGameGuiInputManager::InputMode_Set(Input(), 5);
                        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->l_Selection_RemoveAllGuiElements();
                    }
                }
                else if (selection == 2)
                {
                    for (std::uint32_t index = 0; index < NC2InGameGuiInputManager::Selection_GetSelectedMultipleHumanNumber(Input()); ++index)
                    {
                        const auto id = NC2InGameGuiInputManager::Selection_GetSelectedMultipleHumanId(Input(), index); Queue(id, 21, job); Respond(id);
                    }
                }
                Close(window);
            }
            return true;
        }
        // Original function: FUN_004c75c1.
        void __fastcall SelectedSingleHumanChangeJobWindow_Draw(void* window, void*, NXBasics::CBitmap* bitmap, const SRectangle* rectangle)
        {
            NC2InGameGuiBase::BaseToolWindow_Draw(window, nullptr, bitmap, rectangle);
            const auto selection = Read<std::uint32_t>(Input(), 4);
            if (selection == 1 && SelectedId() != -1) bitmap->Text_Print(NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.font08, White(), 6, 300, Name(SelectedId()));
            else if (selection == 2) bitmap->Text_Print(NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.font08, White(), 6, 300, Misc(431));
        }
        // Original function: FUN_004c73df.
        void* __fastcall SelectedSingleHumanChangeJobWindow_Delete(void* window, void*, std::uint32_t flags);
        // Original function: FUN_004c7b39.
        void* __fastcall SelectedSingleHumanChangeNameWindow_Delete(void* window, void*, std::uint32_t flags);
        // Original function: FUN_004c7cd0.
        void SelectedSingleHumanChangeNameWindow_l_GuiButtons_Add(void* window)
        {
            Frame(window, 230); SelectedSingleHumanLearnJobWindow_l_System_Validate(window);
            if (Read<std::uint8_t>(window, 0x70) == 0) return;
            void* memory = NXBasics::Runtime_AllocateWithNewHandler(96, 1);
            const SRectangle rectangle(4, 76, 140, 20);
            NXBaseGui::BaseWindow_Element_Add(window, memory == nullptr ? nullptr : NC2InGameGuiBase::BaseToolTextButton_ConstructToggle(memory, nullptr, &rectangle, Misc(231), 10000, window, false));
        }
        // Original function: FUN_004c7db4.
        void SelectedSingleHumanChangeNameWindow_l_Misc_DoChangeName(void* window)
        {
            SelectedSingleHumanLearnJobWindow_l_System_Validate(window);
            if (Read<std::uint8_t>(window, 0x70) == 0 || Read<std::uint32_t>(Input(), 4) != 1 || SelectedId() == -1) return;
            const auto id = SelectedId(); const char* text = TextInput(window)->GetString();
            if (std::strlen(text) == 0) return;
            NC2E2::DynamicHuman_SetName(static_cast<std::byte*>(NC2E2::gDynamicDataHumans.entries) + id * 0x8C8, text);
            if (NC2Logic::gGameStaticVars.networkGame == 0 || !NNetwork::Network_IsInSession()) return;
            char formatted[100];
            NXBasics::Runtime_PrintStringN(formatted, sizeof(formatted), "%d:%d:%d:%s", Read<std::int32_t>(Human(id), 0x10), id, Read<std::uint32_t>(Human(id), 8), text);
            // The first UTF-16 code unit is the original rename record discriminator.
            wchar_t record[128]{}; record[0] = 2;
            NNetwork::CopyNetworkAnsiToWide(record + 1, formatted, -1);
            NNetwork::SendDynamicNetworkChatMessage(&gDynamicNetworkData, 0, record);
        }
        // Original function: FUN_004c7b6b.
        bool __fastcall SelectedSingleHumanChangeNameWindow_MessageHandle(void* window, void*, std::uint32_t message, std::uint32_t command, std::uint32_t key, std::uint32_t character, std::uint32_t)
        {
            if (message == 0x1000)
            {
                if (command == 2014) { NXBaseGui::BaseWindow_Element_DeleteAll(window); SelectedSingleHumanChangeNameWindow_l_GuiButtons_Add(window); return true; }
                if (command == 2029) SelectedSingleHumanLearnJobWindow_l_System_Validate(window);
                return false;
            }
            if (message != 0x800) return false;
            if (command == 10000) { SelectedSingleHumanChangeNameWindow_l_Misc_DoChangeName(window); Close(window); }
            else if (command == 0x8233)
            {
                if (Read<std::uint32_t>(window, 0x14) != 1) return false;
                if (key == 27) Close(window);
                else if (TextInput(window)->WorkOnKey(key, static_cast<char>(character)) == 3) { SelectedSingleHumanChangeNameWindow_l_Misc_DoChangeName(window); Close(window); }
            }
            return true;
        }
        // Original function: FUN_004c7bfa.
        void __fastcall SelectedSingleHumanChangeNameWindow_Draw(void* window, void*, NXBasics::CBitmap* bitmap, const SRectangle* rectangle)
        {
            NC2InGameGuiBase::BaseToolWindow_Draw(window, nullptr, bitmap, rectangle);
            if (Read<std::uint32_t>(Input(), 4) != 1 || SelectedId() == -1) return;
            NC2E2::DrawAtom_MapMoveable_DoDraw(Read<std::uint32_t>(Human(SelectedId()), 0x0C), bitmap, 290, 80, nullptr, true, -1);
            auto* font = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.font08;
            bitmap->Text_Print(font, White(), 10, 30, TextInput(window)->GetString());
            const std::uint8_t channel = ((timeGetTime() / 200u) & 1u) != 0 ? 255 : 100;
            const NXBasics::SColorRGB color(channel, channel, channel);
            bitmap->Draw_VerticalLine(TextInput(window)->GetCursorPixelPosition(font) + 10, 32, 12, color);
        }
        // Original function: FUN_004c8129.
        bool __fastcall SelectedSingleHumanChangeProducedGoodWindow_MessageHandle(void* window, void*, std::uint32_t message, std::uint32_t command, std::uint32_t, std::uint32_t, std::uint32_t)
        {
            if (message == 0x1000)
            {
                if (command == 2015) return true;
                if (command == 2029) SelectedSingleHumanLearnJobWindow_l_System_Validate(window);
                return false;
            }
            if (message != 0x800) return false;
            if (command >= 10000 && command < 11000)
            {
                const auto id = SelectedId(); if (id != -1) { Queue(id, 22, command - 10000); Respond(id); }
                Close(window);
            }
            return true;
        }
        // Original function: FUN_004c81dd.
        void __fastcall SelectedSingleHumanChangeProducedGoodWindow_Draw(void* window, void*, NXBasics::CBitmap* bitmap, const SRectangle* rectangle)
        {
            NC2InGameGuiBase::BaseToolWindow_Draw(window, nullptr, bitmap, rectangle);
            const auto id = SelectedId(); if (id != -1) bitmap->Text_Print(NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.font08, White(), 4, 244, Name(id));
        }
        // Original function: FUN_004c8ecc.
        bool __fastcall SelectedSingleHumanLearnJobWindow_MessageHandle(void* window, void*, std::uint32_t message, std::uint32_t command, std::uint32_t, std::uint32_t job, std::uint32_t good)
        {
            if (message == 0x1000)
            {
                if (command == 2017) return true;
                if (command == 2029) SelectedSingleHumanLearnJobWindow_l_System_Validate(window);
                return false;
            }
            if (message != 0x800) return false;
            if (command == 10000)
            {
                const auto id = SelectedId();
                if (id != -1) { Queue(id, 34, job); if (good != 0) Queue(id, 35, good); Respond(id); }
                Close(window);
            }
            return true;
        }
        // Original function: FUN_004c8fa9.
        void __fastcall SelectedSingleHumanLearnJobWindow_Draw(void* window, void*, NXBasics::CBitmap* bitmap, const SRectangle* rectangle)
        {
            NC2InGameGuiBase::BaseToolWindow_Draw(window, nullptr, bitmap, rectangle);
            if (Read<std::uint32_t>(Input(), 4) == 1 && SelectedId() != -1) bitmap->Text_PrintV(NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.font08, White(), 4, 284, "%s", Name(SelectedId()));
        }
        // Complete native nineteen-slot regions 004F76B0..004F76FB, 004F7710..004F775B and 004F7808..004F7853.
        const std::array<std::uintptr_t, 19> gJobWindowVirtualTable = []
        {
            std::array<std::uintptr_t, 19> table{}; std::memcpy(table.data(), NC2InGameGuiBase::gBaseToolWindowVirtualTable, sizeof(table));
            table[0] = reinterpret_cast<std::uintptr_t>(&SelectedSingleHumanChangeJobWindow_Delete); table[2] = reinterpret_cast<std::uintptr_t>(&SelectedSingleHumanChangeJobWindow_MessageHandle); table[3] = reinterpret_cast<std::uintptr_t>(&SelectedSingleHumanChangeJobWindow_Draw); return table;
        }();
        const std::array<std::uintptr_t, 19> gNameWindowVirtualTable = []
        {
            auto table = gJobWindowVirtualTable; table[0] = reinterpret_cast<std::uintptr_t>(&SelectedSingleHumanChangeNameWindow_Delete); table[2] = reinterpret_cast<std::uintptr_t>(&SelectedSingleHumanChangeNameWindow_MessageHandle); table[3] = reinterpret_cast<std::uintptr_t>(&SelectedSingleHumanChangeNameWindow_Draw); return table;
        }();
        const std::array<std::uintptr_t, 19> gLearnWindowVirtualTable = []
        {
            std::array<std::uintptr_t, 19> table{}; std::memcpy(table.data(), NC2InGameGuiBase::gBaseToolWindowVirtualTable, sizeof(table));
            table[2] = reinterpret_cast<std::uintptr_t>(&SelectedSingleHumanLearnJobWindow_MessageHandle); table[3] = reinterpret_cast<std::uintptr_t>(&SelectedSingleHumanLearnJobWindow_Draw); return table;
        }();
        // Complete native nineteen-slot region 004F775C..004F77A7.
        const std::array<std::uintptr_t, 19> gProducedGoodWindowVirtualTable = []
        {
            std::array<std::uintptr_t, 19> table{}; std::memcpy(table.data(), NC2InGameGuiBase::gBaseToolWindowVirtualTable, sizeof(table));
            table[2] = reinterpret_cast<std::uintptr_t>(&SelectedSingleHumanChangeProducedGoodWindow_MessageHandle); table[3] = reinterpret_cast<std::uintptr_t>(&SelectedSingleHumanChangeProducedGoodWindow_Draw); return table;
        }();
        // Original function: FUN_004c73df.
        void* __fastcall SelectedSingleHumanChangeJobWindow_Delete(void* window, void*, std::uint32_t flags)
        {
            Write(window, 0, gJobWindowVirtualTable.data()); NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->FrameCall_UnRegister(window);
            Write(window, 0, NC2InGameGuiBase::gBaseToolWindowVirtualTable + 0); NXBaseGui::BaseWindow_Destruct(window);
            if ((flags & 1u) != 0) NXBasics::Runtime_Free(window);
            return window;
        }
        // Original function: FUN_004c7b39.
        void* __fastcall SelectedSingleHumanChangeNameWindow_Delete(void* window, void*, std::uint32_t flags)
        {
            Write(window, 0, gNameWindowVirtualTable.data()); TextInput(window)->Exit();
            Write(window, 0, NC2InGameGuiBase::gBaseToolWindowVirtualTable + 0); NXBaseGui::BaseWindow_Destruct(window);
            if ((flags & 1u) != 0) NXBasics::Runtime_Free(window);
            return window;
        }
    }
    // Original function: FUN_004c736e.
    void* SelectedSingleHumanChangeJobWindow_Construct(void* window)
    {
        const auto& state = NInGameGuiManager::CInGameGuiManager::mStaticVars;
        const SRectangle rectangle((state.displayWidth - 300) / 2, (state.displayHeight - 320) / 2, 300, 320);
        NC2InGameGuiBase::BaseToolWindow_Construct(window, &rectangle); std::memset(static_cast<std::byte*>(window) + 0x70, 0, 16); Write(window, 0, gJobWindowVirtualTable.data());
        SelectedSingleHumanChangeJobWindow_l_GuiButtons_Add(window); NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->FrameCall_Register(window); return window;
    }
    // Original function: FUN_004c7a7d.
    void* SelectedSingleHumanChangeNameWindow_Construct(void* window)
    {
        const auto& state = NInGameGuiManager::CInGameGuiManager::mStaticVars;
        const SRectangle rectangle((state.displayWidth - 320) / 2, (state.displayHeight - 100) / 2, 320, 100);
        NC2InGameGuiBase::BaseToolWindow_Construct(window, &rectangle); new (TextInput(window)) NC2InGameGuiBaseTextInputElement::CStringInputHandler;
        std::memset(static_cast<std::byte*>(window) + 0x70, 0, 8); Write(window, 0, gNameWindowVirtualTable.data()); TextInput(window)->Init(12, true, true, 13, 0);
        if (Read<std::uint32_t>(Input(), 4) != 1 || SelectedId() == -1) Close(window);
        else { TextInput(window)->SetString(Name(SelectedId())); SelectedSingleHumanChangeNameWindow_l_GuiButtons_Add(window); }
        return window;
    }
    // Original function: FUN_004c7ea7.
    void* SelectedSingleHumanChangeProducedGoodWindow_Construct(void* window)
    {
        const auto& state = NInGameGuiManager::CInGameGuiManager::mStaticVars;
        const SRectangle rectangle((state.displayWidth - 200) / 2, (state.displayHeight - 260) / 2, 200, 260);
        NC2InGameGuiBase::BaseToolWindow_Construct(window, &rectangle); std::memset(static_cast<std::byte*>(window) + 0x70, 0, 8); Write(window, 0, gProducedGoodWindowVirtualTable.data());
        Frame(window, 440); SelectedSingleHumanLearnJobWindow_l_System_Validate(window);
        if (Read<std::uint8_t>(window, 0x70) == 0) return window;
        void* list = List(SRectangle(4, 22, 168, 216));
        const auto id = SelectedId();
        if (id != -1)
        {
            const void* human = Human(id); SRectangle row(0, 0, Read<std::int32_t>(list, 0x0C), 22);
            for (std::uint32_t good = 1; good < 66; ++good)
            {
                if (!NC2Logic::Human_IsCapableToProduceGood(human, good)) continue;
                const bool available = NC2Logic::Human_IsAbleToProduceGoodNow(human, good);
                void* memory = NXBasics::Runtime_AllocateWithNewHandler(available ? 0x60 : 0x100, 1);
                const char* caption = NC2Logic::StringAccess_GetGoodNamePtr(good, 1);
                void* element = nullptr;
                if (memory != nullptr) element = available ? NC2InGameGuiBase::BaseToolTextButton_ConstructToggle(memory, nullptr, &row, caption, 10000 + good, window, Read<std::uint32_t>(human, 0x1C) == good) : NC2InGameGuiBase::BaseToolTextElement_Construct(memory, nullptr, &row, caption, false, false);
                BaseListBoxElement_Element_Add(list, nullptr, element); row.y += 24;
            }
            if (Read<std::uint32_t>(human, 0x18) == 8)
            {
                void* memory = NXBasics::Runtime_AllocateWithNewHandler(96, 1);
                NXBaseGui::CBaseElement* element = static_cast<NXBaseGui::CBaseElement*>(memory == nullptr ? nullptr : NC2InGameGuiBase::BaseToolTextButton_Construct(memory, nullptr, &row, NC2Logic::StringAccess_GetGoodNamePtr(65, 1), 10065, window));
                BaseListBoxElement_Element_Add(list, nullptr, element);
            }
        }
        AddList(window, list, SRectangle(176, 22, 20, 216)); return window;
    }
    // Original function: FUN_004c8b04.
    void* SelectedSingleHumanLearnJobWindow_Construct(void* window)
    {
        const auto& state = NInGameGuiManager::CInGameGuiManager::mStaticVars;
        const SRectangle rectangle((state.displayWidth - 320) / 2, (state.displayHeight - 300) / 2, 320, 300);
        NC2InGameGuiBase::BaseToolWindow_Construct(window, &rectangle); std::memset(static_cast<std::byte*>(window) + 0x70, 0, 8); Write(window, 0, gLearnWindowVirtualTable.data());
        Frame(window, 450); SelectedSingleHumanLearnJobWindow_l_System_Validate(window);
        if (Read<std::uint8_t>(window, 0x70) == 0) return window;
        void* list = List(SRectangle(4, 22, 288, 256));
        const auto id = SelectedId();
        if (id != -1)
        {
            const void* human = Human(id); const auto player = Read<std::int32_t>(human, 0x10); const auto tribe = Read<std::uint32_t>(human, 0x14);
            std::uint32_t jobs[56], count = 0;
            for (std::uint32_t job = 7; job <= 30; ++job)
            {
                const auto offset = player * NC2Layout::RecordSize::GameControlPlayerSigned + tribe * 56 + job;
                if (Read<std::uint8_t>(NC2Logic::gStaticJobTypeData, job * NC2Layout::RecordSize::JobType + 0x87u) != 0 && Read<std::uint8_t>(NC2Logic::gGameControl, offset + 0x18) != 0 && Read<std::uint8_t>(NC2Logic::gGameControl, offset + 0x948) != 0) jobs[count++] = job;
            }
            if (count > 1) NXBasics::Runtime_QuickSort(jobs, count, 4, lsSortCompareFunction);
            SRectangle row(0, 0, Read<std::int32_t>(list, 0x0C), 22);
            for (std::uint32_t index = 0; index < count; ++index)
            {
                const auto job = jobs[index]; const auto& enables = NC2Logic::gStaticTribeTypeStorage.records[tribe].jobEnables[job];
                if (enables.goodCount > 1)
                {
                    for (std::uint32_t goodIndex = 0; goodIndex < enables.goodCount; ++goodIndex)
                    {
                        const auto good = enables.goods[goodIndex]; if (job == 8 && (good == 2 || good == 4 || good == 5 || good == 3)) continue;
                        const auto offset = player * NC2Layout::RecordSize::GameControlPlayerSigned + tribe * 66 + good;
                        if (Read<std::uint8_t>(NC2Logic::gGameControl, offset + 0x1278) == 0 || Read<std::uint8_t>(NC2Logic::gGameControl, offset + 0x1D4C) == 0) continue;
                        char caption[200]; NXBasics::Runtime_PrintStringN(caption, sizeof(caption), "%s (%s)", NC2Logic::StringAccess_GetJobNamePtr(job, 1), NC2Logic::StringAccess_GetGoodNamePtr(good, 1));
                        Row(window, list, row, caption, job, good, false);
                    }
                }
                else Row(window, list, row, NC2Logic::StringAccess_GetJobNamePtr(job, 1), job, Read<std::uint32_t>(human, 0x330), false);
            }
        }
        AddList(window, list, SRectangle(296, 22, 20, 256)); return window;
    }
}

namespace NInGameGuiManager
{
    // Original function: FUN_004a2683.
    void CInGameGuiManager::MiscWindows_HumanChangeJobWindow_Open()
    {
        if (NXBaseGui::CDesktop::sTheObjectPtr->Message_SendToAllWindows(2013, 0, 0, 0) != nullptr) return;
        void* memory = NXBasics::Runtime_AllocateWithNewHandler(128, 1);
        NC2InGameGuiBase::BaseToolDesktop_AddWindow(static_cast<NXBaseGui::CBaseWindow*>(memory == nullptr ? nullptr : NC2InGameGuiSelectedAction::SelectedSingleHumanChangeJobWindow_Construct(memory)), false);
    }
    // Original function: FUN_004a26c2.
    void CInGameGuiManager::MiscWindows_HumanChangeNameWindow_Open()
    {
        auto* desktop = NXBaseGui::CDesktop::sTheObjectPtr;
        if (desktop->Message_SendToAllWindows(2014, 0, 0, 0) != nullptr) return;
        void* memory = NXBasics::Runtime_AllocateWithNewHandler(148, 1);
        NC2InGameGuiBase::BaseToolDesktop_AddWindow(static_cast<NXBaseGui::CBaseWindow*>(memory == nullptr ? nullptr : NC2InGameGuiSelectedAction::SelectedSingleHumanChangeNameWindow_Construct(memory)), false);
        if (auto* window = desktop->Message_SendToAllWindows(2014, 0, 0, 0)) desktop->Window_BringToFront(reinterpret_cast<NXBaseGui::CBaseWindow*>(window));
    }
    // Original function: FUN_004a2723.
    bool CInGameGuiManager::MiscWindows_HumanChangeNameWindow_SendKey(std::uint32_t key, char character)
    {
        void* window = NXBaseGui::CDesktop::sTheObjectPtr->Message_SendToAllWindows(2014, 0, 0, 0);
        if (window == nullptr) return false;
        const auto table = NC2Lifetime::Read<const std::uintptr_t*>(window, 0);
        return reinterpret_cast<bool(__fastcall*)(void*, void*, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t)>(table[2])(window, nullptr, 0x800, 0x8233, key, static_cast<std::uint32_t>(static_cast<std::int32_t>(character)), 0);
    }
    // Original function: FUN_004a2761.
    void CInGameGuiManager::MiscWindows_HumanChangeProducedGoodWindow_Open()
    {
        if (NXBaseGui::CDesktop::sTheObjectPtr->Message_SendToAllWindows(2015, 0, 0, 0) != nullptr) return;
        void* memory = NXBasics::Runtime_AllocateWithNewHandler(120, 1);
        NC2InGameGuiBase::BaseToolDesktop_AddWindow(static_cast<NXBaseGui::CBaseWindow*>(memory == nullptr ? nullptr : NC2InGameGuiSelectedAction::SelectedSingleHumanChangeProducedGoodWindow_Construct(memory)), false);
    }
    // Original function: FUN_004a27ea.
    void CInGameGuiManager::MiscWindows_HumanLearnJobWindow_Open()
    {
        if (NXBaseGui::CDesktop::sTheObjectPtr->Message_SendToAllWindows(2017, 0, 0, 0) != nullptr) return;
        void* memory = NXBasics::Runtime_AllocateWithNewHandler(120, 1);
        NC2InGameGuiBase::BaseToolDesktop_AddWindow(static_cast<NXBaseGui::CBaseWindow*>(memory == nullptr ? nullptr : NC2InGameGuiSelectedAction::SelectedSingleHumanLearnJobWindow_Construct(memory)), false);
    }
}
