#include "GuiBitmapToolElementAccess.h"
#include "XBitmap.h"
#include "XCrtFileAccess.h"

#include <cstring>
#include <new>

// Untranslated original base/widget interfaces. EDX is unused so the stack
// parameters and callee cleanup match Windows __thiscall dispatch.
extern void* __fastcall FUN_004788c6(void* object, void* unusedEdx, const NXBasics::SRectangle* rectangle, std::uint32_t flags, std::uint32_t elementId);
extern void __fastcall FUN_00478917(void* object);
extern bool __fastcall FUN_00478a36(void* object, void* unusedEdx, std::int32_t message, std::int32_t param1, std::uint32_t param2, std::uint32_t param3, std::uint32_t param4);
extern bool __fastcall FUN_004788a7(void* object);
extern void __fastcall FUN_00478986(void* object, void* unusedEdx, const std::int32_t* point);
extern void __fastcall FUN_00478994(void* object);
extern bool __fastcall FUN_0047899f(void* object, void* unusedEdx, const std::int32_t* point);
extern bool __fastcall FUN_004789af(void* object);
extern bool __fastcall FUN_004788b1(void* object, void* unusedEdx, const std::int32_t* point);
extern void* __fastcall FUN_0048e569(void* object);
extern void* __fastcall FUN_00478ad8(void* object, void* unusedEdx, const std::int32_t* point, bool flag);
extern void __fastcall FUN_00478a65(void* object, void* unusedEdx, std::int32_t* point);
extern void __fastcall FUN_004788be(void* object);
extern bool __fastcall FUN_004b8659(void* object, void* unusedEdx, std::uint32_t argument);
extern bool __fastcall FUN_004788b6(void* object);

namespace NC2InGameGuiBase
{
    template<class T> static T BitmapTool_Read(const void* object, std::size_t offset)
    {
        T value;
        std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
        return value;
    }

    template<typename T> static void BitmapTool_Write(void* object, std::size_t offset, T value)
    {
        std::memcpy(static_cast<std::byte*>(object) + offset, &value, sizeof(value));
    }

    // Original virtual entry: FUN_004b8af7.
    static void* __fastcall BaseBitmapToolElement_DeletingDestructor(void* element, void* unusedEdx, std::uint32_t flags)
    {
        (void)unusedEdx;
        BaseBitmapToolElement_Destruct(element);
        if ((flags & 1u) != 0)
            NXBasics::Runtime_Free(element);
        return element;
    }

    // ABI adapter for FUN_004b8b33.
    static void __fastcall BaseBitmapToolElement_VirtualDraw(void* element, void* unusedEdx, NXBasics::CBitmap* bitmap, NXBasics::SRectangle* rectangle)
    {
        (void)unusedEdx;
        BaseBitmapToolElement_Draw(element, bitmap, rectangle);
    }

    // Complete original base table: 004F6D58..004F6D93.
    static const std::uintptr_t gBaseBitmapToolElementVirtualTable[] =
    {
        reinterpret_cast<std::uintptr_t>(&BaseBitmapToolElement_DeletingDestructor),
        reinterpret_cast<std::uintptr_t>(&FUN_004788a7),
        reinterpret_cast<std::uintptr_t>(&FUN_00478a36),
        reinterpret_cast<std::uintptr_t>(&BaseBitmapToolElement_VirtualDraw),
        reinterpret_cast<std::uintptr_t>(&FUN_00478986),
        reinterpret_cast<std::uintptr_t>(&FUN_00478994),
        reinterpret_cast<std::uintptr_t>(&FUN_0047899f),
        reinterpret_cast<std::uintptr_t>(&FUN_004789af),
        reinterpret_cast<std::uintptr_t>(&FUN_004788b1),
        reinterpret_cast<std::uintptr_t>(&FUN_0048e569),
        reinterpret_cast<std::uintptr_t>(&FUN_00478ad8),
        reinterpret_cast<std::uintptr_t>(&FUN_00478a65),
        reinterpret_cast<std::uintptr_t>(&FUN_004788be),
        reinterpret_cast<std::uintptr_t>(&FUN_004b8659),
        reinterpret_cast<std::uintptr_t>(&FUN_004788b6)
    };
    static_assert(sizeof(gBaseBitmapToolElementVirtualTable) == 0x3C);

    // Original function: FUN_004b8aa0; CBaseBitmapToolElement constructor.
    void* BaseBitmapToolElement_Construct(void* element, const NXBasics::SRectangle* rectangle, std::uint32_t flags, std::uint32_t elementId, std::int32_t depth)
    {
        FUN_004788c6(element, nullptr, rectangle, flags, elementId);
        BitmapTool_Write(element, 0x00, gBaseBitmapToolElementVirtualTable + 0);
        BitmapTool_Write<void*>(element, 0x2C, nullptr);
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x54, 1);
        NXBasics::CBitmap* bitmap = allocation == nullptr ? nullptr : ::new (allocation) NXBasics::CBitmap(rectangle->width, rectangle->height, depth);
        BitmapTool_Write(element, 0x2C, bitmap);
        return element;
    }

    // Original function: FUN_004b8b13; CBaseBitmapToolElement destructor.
    void BaseBitmapToolElement_Destruct(void* element)
    {
        auto* bitmap = BitmapTool_Read<NXBasics::CBitmap*>(element, 0x2C);
        BitmapTool_Write(element, 0x00, gBaseBitmapToolElementVirtualTable + 0);
        if (bitmap != nullptr)
            delete bitmap;
        FUN_00478917(element);
    }

    // Original function: FUN_004b8b33.
    // Original method: NC2InGameGuiBase::CBaseBitmapToolElement::XGui_BE_Element_Draw.
    void BaseBitmapToolElement_Draw(const void* element, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle)
    {
        auto* source = BitmapTool_Read<NXBasics::CBitmap*>(element, 0x2C);
        const std::int32_t x = NXBasics::Runtime_TimeSigned32(0u - static_cast<std::uint32_t>(rectangle->x));
        const std::int32_t y = NXBasics::Runtime_TimeSigned32(0u - static_cast<std::uint32_t>(rectangle->y));
        source->CopyIntoBitmap(bitmap, x, y);
    }

}
