#include "InGameGuiHouseInhabitantsElement.h"
#include "InGameGuiManager.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2HouseAccess.h"
#include "C2HumanIterator.h"
#include "C2HumanJobAccess.h"
#include "C2MissionStartObjects.h"
#include "C2EngineMoveableDrawing.h"
#include "C2EngineDrawingAccess.h"
#include "C2EngineObjectData.h"
#include "C2StringAccess.h"
#include "GuiToolsBase.h"
#include "XGuiWindowAccess.h"
#include "XBobRendering.h"
#include "XCrtFileAccess.h"
#include <array>
#include <cstring>

namespace NC2InGameGuiSelection
{
    using namespace NC2Lifetime;
    using TMessage = bool(__thiscall*)(void*,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t);
    static void* House(std::int32_t id) { return NC2Logic::CHouseArray::mStaticVars + static_cast<std::intptr_t>(id) * NC2Logic::CHouseArray::ElementSize; }
    static const void* Human(std::int32_t id) { return NC2Logic::CHumanArray::mStaticVars + static_cast<std::intptr_t>(id) * NC2Logic::CHumanArray::ElementSize; }
    static void* Slot(void* element,std::int32_t index) { return static_cast<std::byte*>(element) + 0x38 + index * 20; }
    static void* __fastcall SelectionHouseInhabitantsElement_Delete(void* element,void*,std::uint32_t flags);
    static bool __fastcall SelectionHouseInhabitantsElement_Message(void* element,void*,std::uint32_t message,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t);
    static void __fastcall SelectionHouseInhabitantsElement_Draw(void* element,void*,NXBasics::CBitmap* bitmap,const NXBasics::SRectangle* rectangle);
    static bool __fastcall SelectionHouseInhabitantsElement_IsHit(void* element,void*,const std::int32_t* point);
    static bool __fastcall SelectionHouseInhabitantsElement_ToolTip_Available(void* element,void*,const char** output);
    static void __fastcall SelectionHouseInhabitantsElement_DoPressedAction(void* element);
    // Complete native table 004F79A8..004F79EF.
    static const std::array<std::uintptr_t,18> gHouseInhabitantsTable = [] { std::array<std::uintptr_t,18> t{}; std::memcpy(t.data(),NXBaseGui::gBaseElementVirtualTable,sizeof(t)); t[0] = reinterpret_cast<std::uintptr_t>(&SelectionHouseInhabitantsElement_Delete); t[2] = reinterpret_cast<std::uintptr_t>(&SelectionHouseInhabitantsElement_Message); t[3] = reinterpret_cast<std::uintptr_t>(&SelectionHouseInhabitantsElement_Draw); t[8] = reinterpret_cast<std::uintptr_t>(&SelectionHouseInhabitantsElement_IsHit); t[13] = reinterpret_cast<std::uintptr_t>(&SelectionHouseInhabitantsElement_ToolTip_Available); t[14] = reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Hide_CanBeHidden); t[17] = reinterpret_cast<std::uintptr_t>(&SelectionHouseInhabitantsElement_DoPressedAction); return t; }();
    // Original function: FUN_004cc045; native allocation 0x8E4, one hundred 0x14-byte slots.
    void* SelectionHouseInhabitantsElement_Construct(void* element, const NXBasics::SRectangle* rectangle, void* receiver, std::uint32_t selectCommand, std::uint32_t actionCommand, std::int32_t houseId) { NXBaseGui::BaseElement_Construct(element,rectangle,3,0x20); std::memset(static_cast<std::byte*>(element) + 0x2C,0,0x8B8); Write(element,0,gHouseInhabitantsTable.data()); Write(element,0x2C,houseId); if (houseId != -1) { const auto* house = House(houseId); if (Read<std::uint8_t>(house,0)) Write(element,0x30,Read<std::uint32_t>(house,8)); else Write<std::int32_t>(element,0x2C,-1); } Write<std::int32_t>(element,0x80C,-1); Write(element,0x8D8,selectCommand); Write(element,0x8DC,actionCommand); Write(element,0x8E0,receiver); Write(element,0x808,rectangle->height - 15); return element; }
    // Original function: FUN_004cc0d0.
    static void* __fastcall SelectionHouseInhabitantsElement_Delete(void* element,void*,std::uint32_t flags) { Write(element,0,gHouseInhabitantsTable.data()); NXBaseGui::BaseElement_Destruct(element); if (flags & 1u) NXBasics::Runtime_Free(element); return element; }
    // Original function: FUN_004cc0f2.
    static bool __fastcall SelectionHouseInhabitantsElement_Message(void* element,void*,std::uint32_t message,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t) { if (message != 0x20) return false; const auto index = Read<std::int32_t>(element,0x80C); if (index != -1) { auto* receiver = Read<void*>(element,0x8E0); Read<TMessage*>(receiver,0)[2](receiver,0x800,Read<std::uint32_t>(element,0x8DC),Read<std::uint32_t>(Slot(element,index),0),0,0); } return true; }
    // Original function: FUN_004cc1a8.
    static void __fastcall SelectionHouseInhabitantsElement_DoPressedAction(void* element) { const auto index = Read<std::int32_t>(element,0x80C); if (index == -1) return; auto* receiver = Read<void*>(element,0x8E0); Read<TMessage*>(receiver,0)[2](receiver,0x800,Read<std::uint32_t>(element,0x8D8),Read<std::uint32_t>(Slot(element,index),0),0,0); NC2GuiToolsBase::FXTool_ClickConfirm(); }
    // Original function: FUN_004cc4db.
    static bool l_ValidateHouse(void* element) { const auto id = Read<std::int32_t>(element,0x2C); if (id == -1) return false; const auto* house = House(id); if (Read<std::uint8_t>(house,0) && Read<std::uint32_t>(house,8) == Read<std::uint32_t>(element,0x30)) return true; Write<std::int32_t>(element,0x2C,-1); Write<std::int32_t>(element,0x80C,-1); return false; }
    // Original function: FUN_004cc50d; attached roles, traders, then nearby player humans.
    static std::int32_t l_UpdateAttachedHumans(void* element)
    {
        auto* house = House(Read<std::int32_t>(element,0x2C)); std::int32_t count = 0; Write(element,0x34,count);
        for (std::uint32_t index = 0; index < 20; ++index) { const auto* attachment = NC2Logic::House_Inhabitants_GetAttachedHumanSlotPtrBySlotId(house,index); const auto flags = Read<std::uint8_t>(attachment,0); if (!flags) continue; const auto id = Read<std::int32_t>(attachment,4); const auto* human = Human(id); auto* slot = Slot(element,count); Write(slot,0,id); Write<std::uint32_t>(slot,4,flags); std::int32_t priority = 1;
            if (flags & 1u) { std::uint32_t families[20]; NC2Logic::House_Inhabitants_GetNumberOfFamiliesLivingInHome(house,families); if (families[index] != 0xFFFFFFFFu) priority = static_cast<std::int32_t>((families[index] + 1) * 10); if (!NC2Logic::Human_IsAgeAdult(human)) priority += 2; else if (NC2Logic::Human_IsSexFemale(human)) ++priority; }
            else if (flags & 2u) priority = Read<std::uint32_t>(human,0x18) == 24 ? 200 : 100; else if (flags & 4u) priority = 500;
            Write(slot,12,priority); Write(element,0x34,++count); if (count == 100) break;
        }
        if (count < 100) { NC2Logic::CHumanIterator iterator; for (auto id = iterator.GetFirstHumanId(Read<std::uint32_t>(house,0xC)); id != -1 && count < 100; id = iterator.GetNextHumanId()) { const auto* human = Human(id); if (!NC2Logic::Human_IsJobTrader(human)) continue; auto* merchant = NC2Logic::HumanMerchantArray_GetMerchantDataPtr(NC2Logic::gHumanMerchantArray,Read<std::int32_t>(human,0xB44)); if (merchant == nullptr) continue; for (std::uint32_t index = 0; index < 2; ++index) { const auto* data = NC2Logic::MerchantData_GetHouseDataPtr(merchant,index); if (!NC2Logic::MerchantHouseData_IsValid(data) || Read<std::uint32_t>(data,0) != Read<std::uint32_t>(house,4)) continue; auto* slot = Slot(element,count++); Write(slot,0,id); Write<std::uint32_t>(slot,4,0); Write<std::uint8_t>(slot,16,1); Write<std::int32_t>(slot,12,50); Write(element,0x34,count); if (count == 100) break; } if (count == 100) break; } }
        if (count < 100) { NC2Logic::CHumanIterator iterator; for (auto id = iterator.GetFirstHumanId(Read<std::uint32_t>(house,0xC),static_cast<std::byte*>(house) + 0x24,1); id != -1 && count < 100; id = iterator.GetNextHumanId()) { std::int32_t index = 0; while (index < count && Read<std::int32_t>(Slot(element,index),0) != id) ++index; if (index != count) continue; auto* slot = Slot(element,count++); Write(slot,0,id); Write<std::uint32_t>(slot,4,0); Write<std::int32_t>(slot,12,1000); Write(element,0x34,count); if (count == 100) break; } }
        for (auto end = count - 1; end >= 0; --end) for (std::int32_t index = 0; index < end; ++index) if (Read<std::int32_t>(Slot(element,index),12) > Read<std::int32_t>(Slot(element,index + 1),12)) { std::byte temporary[20]; std::memcpy(temporary,Slot(element,index),20); std::memcpy(Slot(element,index),Slot(element,index + 1),20); std::memcpy(Slot(element,index + 1),temporary,20); }
        if (count > 1) { std::int32_t gaps = 0; for (std::int32_t index = 0; index < count - 1; ++index) { auto delta = Read<std::int32_t>(Slot(element,index),12) - Read<std::int32_t>(Slot(element,index + 1),12); if (delta < 0) delta = -delta; if (delta > 3) ++gaps; } double step = (static_cast<double>(Read<std::int32_t>(element,0xC) - 20) - gaps * 15.0 - 20.0) / (count - 1); if (step > 30.0) step = 30.0; double position = 20.0;
            // Windows accumulates both values in x87 extended precision; the numeric adapter uses double.
            for (std::int32_t index = 0; index < count; ++index) { Write(Slot(element,index),8,static_cast<std::int32_t>(NXBasics::Runtime_TruncateDoubleToInt64(position))); position += step; if (index == count - 1) continue; auto delta = Read<std::int32_t>(Slot(element,index),12) - Read<std::int32_t>(Slot(element,index + 1),12); if (delta < 0) delta = -delta; if (delta > 3) position += 15.0; }
        } else if (count == 1) Write<std::int32_t>(Slot(element,0),8,20); return count;
    }
    // Original function: FUN_004cc132.
    static void __fastcall SelectionHouseInhabitantsElement_Draw(void* element,void*,NXBasics::CBitmap* bitmap,const NXBasics::SRectangle* rectangle) { NC2GuiToolsBase::DrawTool_DrawFrame(element,bitmap,rectangle,2); if (!l_ValidateHouse(element)) return; l_UpdateAttachedHumans(element); for (std::int32_t index = 0; index < Read<std::int32_t>(element,0x34); ++index) { const auto* slot = Slot(element,index); NC2E2::DrawAtom_MapMoveable_DoDraw(Read<std::uint32_t>(Human(Read<std::int32_t>(slot,0)),0xC),bitmap,Read<std::int32_t>(slot,8),Read<std::int32_t>(element,0x808),nullptr,true,-1); } }
    // Original function: FUN_004cc1de.
    static bool __fastcall SelectionHouseInhabitantsElement_IsHit(void* element,void*,const std::int32_t* point) { Write<std::int32_t>(element,0x80C,-1); if (!l_ValidateHouse(element)) return false; l_UpdateAttachedHumans(element); for (auto index = Read<std::int32_t>(element,0x34) - 1; index >= 0; --index) { const auto* slot = Slot(element,index); NC2E2::S2DEngineDisplaySearchObjectBobData data; NC2E2::DrawAtom_MapMoveable_GetBobData(Read<std::uint32_t>(Human(Read<std::int32_t>(slot,0)),0xC),nullptr,&data,Read<std::int32_t>(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr,0)); const std::int32_t local[2]{point[0] - Read<std::int32_t>(slot,8),point[1] - Read<std::int32_t>(element,0x808)}; for (std::uint32_t bob = 0; bob < data.count; ++bob) if (NXBasics::BobManager_IsBobHit(data.bobs[bob].manager,data.bobs[bob].id,local)) { Write(element,0x80C,index); return true; } } return false; }
    // Original function: FUN_004cc2c1.
    static bool __fastcall SelectionHouseInhabitantsElement_ToolTip_Available(void* element,void*,const char** output)
    {
        const auto index = Read<std::int32_t>(element,0x80C); if (index == -1) return false; const auto* slot = Slot(element,index); const auto id = Read<std::int32_t>(slot,0); const auto* human = Human(id); auto* house = House(Read<std::int32_t>(element,0x2C)); const auto flags = Read<std::uint32_t>(slot,4); auto* text = reinterpret_cast<char*>(element) + 0x810; std::strcpy(text,reinterpret_cast<const char*>(NC2E2::gDynamicDataHumans.entries) + static_cast<std::intptr_t>(id) * 0x8C8 + 0x85C); std::strcat(text," - "); std::strcat(text,NC2Logic::StringAccess_GetJobNamePtr(Read<std::uint32_t>(human,0x18),1)); if (Read<std::uint32_t>(human,0x1C)) { std::strcat(text," ("); std::strcat(text,NC2Logic::StringAccess_GetGoodNamePtr(Read<std::uint32_t>(human,0x1C),1)); std::strcat(text,")"); }
        if (flags & 2u) { std::strcat(text," - "); std::strcat(text,NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(340)); } if (flags & 4u) { std::strcat(text," - "); std::strcat(text,NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(341)); }
        if (flags & 1u) { std::uint32_t families[20]; NC2Logic::House_Inhabitants_GetNumberOfFamiliesLivingInHome(house,families); const auto slotId = NC2Logic::House_Inhabitants_GetAttachedHumanSlotIdByHumanId(house,id); if (slotId != -1) { char family[100]; NXBasics::Runtime_PrintString(family,"%s %d",NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(342),families[slotId] + 1); std::strcat(text," - "); std::strcat(text,family); } const auto spouse = Read<std::int32_t>(human,0xB20); if (spouse != -1 && Read<std::int32_t>(Human(spouse),0xB28) == Read<std::int32_t>(element,0x2C)) std::strcat(text," **"); }
        if (!flags && !Read<std::uint8_t>(slot,16)) { std::strcat(text," ("); std::strcat(text,NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(343)); std::strcat(text,")"); } *output = text; return true;
    }
}
