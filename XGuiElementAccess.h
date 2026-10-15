#pragma once
#include "XBasicTypes.h"

namespace NXBasics
{
	class CBitmap;
}
namespace NXBaseGui
{
	// Windows physical GUI prefixes; these models do not own or construct child lists.
	struct SBaseElementStorage
	{
		const void* virtualTable;
		NXBasics::SRectangle rectangle;
		std::uint32_t interactionState;
		std::uint32_t flags;
		std::uint32_t messageMask;
		void* parentWindow;
		void* messageHandler;
		std::uint32_t serial;
	};
	struct SBaseWindowStorage
	{
		SBaseElementStorage element;
		NXBasics::SRectangle clippingRectangle;
		std::byte drawChildren[20];
		std::byte inputChildren[20];
		void* activeChild;
		std::byte unresolved68[4];
	};

	extern const std::uintptr_t gBaseElementVirtualTable[18];
	inline NXBasics::SRectangle BaseElement_GetRectangle(const void* element)
	{
		NXBasics::SRectangle rectangle(0, 0, 0, 0);
		std::memcpy(&rectangle, static_cast<const std::byte*>(element) + 4, sizeof(rectangle));
		return rectangle;
	}
	void* BaseElement_Construct(void* element, const NXBasics::SRectangle* rectangle, std::uint32_t flags, std::uint32_t messageMask);
	void BaseElement_Destruct(void* element);
	bool __fastcall BaseElement_HasSubElements(void* element);
	bool __fastcall BaseElement_IsHit(void* element, void* unusedEdx, const std::int32_t* point);
	bool __fastcall BaseElement_ToolTip_AtOnce(void* element);
	std::uint32_t __fastcall BaseElement_UnresolvedVirtual40(void* element, void* unusedEdx, std::uint32_t param1, std::uint32_t param2);
	void __fastcall BaseElement_Notify(void* element);
	void __fastcall BaseWindow_DrawOverlays(void* window, void* unusedEdx, std::uint32_t bitmap, std::uint32_t rectangle);
	void* __fastcall BaseWindow_GetRealActiveElementPtr(void* window);
	void __fastcall BaseElement_Draw(void* element, void* unusedEdx, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle);
	void __fastcall BaseElement_Activate(void* element, void* unusedEdx, const std::int32_t* point);
	void __fastcall BaseElement_DeActivate(void* element);
	bool __fastcall BaseElement_ButtonPressed(void* element, void* unusedEdx, const std::int32_t* point);
	bool __fastcall BaseElement_ButtonReleased(void* element);
	void BaseElement_MouseEnteredArea(void* element);
	bool BaseElement_CanBeActivated(const void* element);
	bool BaseElement_CanBePressed(const void* element);
	bool __fastcall BaseElement_Message_Handle(void* element, void* unusedEdx, std::uint32_t message, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3, std::uint32_t param4);
	void __fastcall BaseElement_ParentToLocalPosition(void* element, void* unusedEdx, std::int32_t* point);
	void* __fastcall BaseElement_FindElementOnPosition(void* element, void* unusedEdx, const std::int32_t* point, bool includeDisabled);
	void* __fastcall BaseElement_GetRealActiveElementPtr(void* element);
	bool __fastcall BaseElement_ToolTip_Available(void* element, void* unusedEdx, std::uint32_t output);
	bool __fastcall BaseElement_Hide_CanBeHidden(void* element);
}
