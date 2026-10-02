#include "XMMessageDialogs.h"
#include "X2DApplication.h"
#include "XSystemMouseCursor.h"
#include "XCrtFileAccess.h"

#include <Windows.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <intrin.h>

namespace NXSysMisc
{
    // Original function: FUN_00479e40. Windows-specific HRESULT names and format.
    static void FormatDirectDrawError(char* buffer, std::int32_t result, const char* sourceFile, std::int32_t line)
    {
        const char* name;
        switch (static_cast<std::uint32_t>(result))
        {
            case 0x80004001u: name = "DDERR_UNSUPPORTED"; break;
            case 0x80004005u: name = "DDERR_GENERIC"; break;
            case 0x800401F0u: name = "DDERR_NOTINITIALIZED"; break;
            case 0x8007000Eu: name = "DDERR_OUTOFMEMORY"; break;
            case 0x80070057u: name = "DDERR_INVALIDPARAMS"; break;
            case 0x88760005u: name = "DDERR_ALREADYINITIALIZED"; break;
            case 0x8876000Au: name = "DDERR_CANNOTATTACHSURFACE"; break;
            case 0x88760014u: name = "DDERR_CANNOTDETACHSURFACE"; break;
            case 0x88760028u: name = "DDERR_CURRENTLYNOTAVAIL"; break;
            case 0x88760037u: name = "DDERR_EXCEPTION"; break;
            case 0x8876005Au: name = "DDERR_HEIGHTALIGN"; break;
            case 0x8876005Fu: name = "DDERR_INCOMPATIBLEPRIMARY"; break;
            case 0x88760064u: name = "DDERR_INVALIDCAPS"; break;
            case 0x8876006Eu: name = "DDERR_INVALIDCLIPLIST"; break;
            case 0x88760078u: name = "DDERR_INVALIDMODE"; break;
            case 0x88760082u: name = "DDERR_INVALIDOBJECT"; break;
            case 0x88760091u: name = "DDERR_INVALIDPIXELFORMAT"; break;
            case 0x88760096u: name = "DDERR_INVALIDRECT"; break;
            case 0x887600A0u: name = "DDERR_LOCKEDSURFACES"; break;
            case 0x887600AAu: name = "DDERR_NO3D"; break;
            case 0x887600B4u: name = "DDERR_NOALPHAHW"; break;
            case 0x887600CDu: name = "DDERR_NOCLIPLIST"; break;
            case 0x887600D2u: name = "DDERR_NOCOLORCONVHW"; break;
            case 0x887600D4u: name = "DDERR_NOCOOPERATIVELEVELSET"; break;
            case 0x887600D7u: name = "DDERR_NOCOLORKEY"; break;
            case 0x887600DCu: name = "DDERR_NOCOLORKEYHW"; break;
            case 0x887600DEu: name = "DDERR_NODIRECTDRAWSUPPORT"; break;
            case 0x887600E1u: name = "DDERR_NOEXCLUSIVEMODE"; break;
            case 0x887600E6u: name = "DDERR_NOFLIPHW"; break;
            case 0x887600F0u: name = "DDERR_NOGDI"; break;
            case 0x887600FAu: name = "DDERR_NOMIRRORHW"; break;
            case 0x887600FFu: name = "DDERR_NOTFOUND"; break;
            case 0x88760104u: name = "DDERR_NOOVERLAYHW"; break;
            case 0x88760118u: name = "DDERR_NORASTEROPHW"; break;
            case 0x88760122u: name = "DDERR_NOROTATIONHW"; break;
            case 0x88760136u: name = "DDERR_NOSTRETCHHW"; break;
            case 0x8876013Cu: name = "DDERR_NOT4BITCOLOR"; break;
            case 0x8876013Du: name = "DDERR_NOT4BITCOLORINDEX"; break;
            case 0x88760140u: name = "DDERR_NOT8BITCOLOR"; break;
            case 0x8876014Au: name = "DDERR_NOTEXTUREHW"; break;
            case 0x8876014Fu: name = "DDERR_NOVSYNCHW"; break;
            case 0x88760154u: name = "DDERR_NOZBUFFERHW"; break;
            case 0x8876015Eu: name = "DDERR_NOZOVERLAYHW"; break;
            case 0x88760168u: name = "DDERR_OUTOFCAPS"; break;
            case 0x8876017Cu: name = "DDERR_OUTOFVIDEOMEMORY"; break;
            case 0x8876017Eu: name = "DDERR_OVERLAYCANTCLIP"; break;
            case 0x88760180u: name = "DDERR_OVERLAYCOLORKEYONLYONEACTIVE"; break;
            case 0x88760183u: name = "DDERR_PALETTEBUSY"; break;
            case 0x88760190u: name = "DDERR_COLORKEYNOTSET"; break;
            case 0x8876019Au: name = "DDERR_SURFACEALREADYATTACHED"; break;
            case 0x887601A4u: name = "DDERR_SURFACEALREADYDEPENDENT"; break;
            case 0x887601AEu: name = "DDERR_SURFACEBUSY"; break;
            case 0x887601B3u: name = "DDERR_CANTLOCKSURFACE"; break;
            case 0x887601B8u: name = "DDERR_SURFACEISOBSCURED"; break;
            case 0x887601C2u: name = "DDERR_SURFACELOST"; break;
            case 0x887601CCu: name = "DDERR_SURFACENOTATTACHED"; break;
            case 0x887601D6u: name = "DDERR_TOOBIGHEIGHT"; break;
            case 0x887601E0u: name = "DDERR_TOOBIGSIZE"; break;
            case 0x887601EAu: name = "DDERR_TOOBIGWIDTH"; break;
            case 0x887601FEu: name = "DDERR_UNSUPPORTEDFORMAT"; break;
            case 0x88760208u: name = "DDERR_UNSUPPORTEDMASK"; break;
            case 0x88760219u: name = "DDERR_VERTICALBLANKINPROGRESS"; break;
            case 0x8876021Cu: name = "DDERR_WASSTILLDRAWING"; break;
            case 0x88760230u: name = "DDERR_XALIGN"; break;
            case 0x88760231u: name = "DDERR_INVALIDDIRECTDRAWGUID"; break;
            case 0x88760232u: name = "DDERR_DIRECTDRAWALREADYCREATED"; break;
            case 0x88760233u: name = "DDERR_NODIRECTDRAWHW"; break;
            case 0x88760234u: name = "DDERR_PRIMARYSURFACEALREADYEXISTS"; break;
            case 0x88760235u: name = "DDERR_NOEMULATION"; break;
            case 0x88760236u: name = "DDERR_REGIONTOOSMALL"; break;
            case 0x88760237u: name = "DDERR_CLIPPERISUSINGHWND"; break;
            case 0x88760238u: name = "DDERR_NOCLIPPERATTACHED"; break;
            case 0x88760239u: name = "DDERR_NOHWND"; break;
            case 0x8876023Au: name = "DDERR_HWNDSUBCLASSED"; break;
            case 0x8876023Bu: name = "DDERR_HWNDALREADYSET"; break;
            case 0x8876023Cu: name = "DDERR_NOPALETTEATTACHED"; break;
            case 0x8876023Du: name = "DDERR_NOPALETTEHW"; break;
            case 0x8876023Eu: name = "DDERR_BLTFASTCANTCLIP"; break;
            case 0x8876023Fu: name = "DDERR_NOBLTHW"; break;
            case 0x88760240u: name = "DDERR_NODDROPSHW"; break;
            case 0x88760241u: name = "DDERR_OVERLAYNOTVISIBLE"; break;
            case 0x88760242u: name = "DDERR_NOOVERLAYDEST"; break;
            case 0x88760243u: name = "DDERR_INVALIDPOSITION"; break;
            case 0x88760244u: name = "DDERR_NOTAOVERLAYSURFACE"; break;
            case 0x88760245u: name = "DDERR_EXCLUSIVEMODEALREADYSET"; break;
            case 0x88760246u: name = "DDERR_NOTFLIPPABLE"; break;
            case 0x88760247u: name = "DDERR_CANTDUPLICATE"; break;
            case 0x88760248u: name = "DDERR_NOTLOCKED"; break;
            case 0x88760249u: name = "DDERR_CANTCREATEDC"; break;
            case 0x8876024Au: name = "DDERR_NODC"; break;
            case 0x8876024Bu: name = "DDERR_WRONGMODE"; break;
            case 0x8876024Cu: name = "DDERR_IMPLICITLYCREATED"; break;
            case 0x8876024Du: name = "DDERR_NOTPALETTIZED"; break;
            case 0x8876024Eu: name = "DDERR_UNSUPPORTEDMODE"; break;
            case 0x8876024Fu: name = "DDERR_NOMIPMAPHW"; break;
            case 0x88760250u: name = "DDERR_INVALIDSURFACETYPE"; break;
            case 0x8876026Cu: name = "DDERR_DCALREADYCREATED"; break;
            case 0x88760280u: name = "DDERR_CANTPAGELOCK"; break;
            case 0x88760294u: name = "DDERR_CANTPAGEUNLOCK"; break;
            case 0x887602A8u: name = "DDERR_NOTPAGELOCKED"; break;
            default: name = "Unknown DD Error"; break;
        }
        NXBasics::Runtime_PrintString(buffer, "NXSysMisc: DirectDraw error occured at %s - %d: <%s>\n", sourceFile, line, name);
    }

    // Original function: FUN_0047a567; corrected entry decoded from Game.exe.
    void __cdecl XWS_DirectDrawErrorBox(std::int32_t result, const char* sourceFile, std::int32_t line)
    {
        if (result == 0) return;
        char buffer[200];
        FormatDirectDrawError(buffer, result, sourceFile, line);
        OutputDebugStringA(buffer);
        XWS_WarningBox(buffer);
    }

    // Original function: FUN_0047aaa3.
    void __cdecl XWS_AbortBox(const char* message)
    {
        XWS_SystemMouseCursor_Show();
        ClipCursor(nullptr);
        auto* application = NXSys2DApp::CApplication::sTheObjectPtr;
        application->FlipToGDISurface();
        const HWND windowHandle = application->GetWindowHandle();
        char buffer[1000];
        std::strcpy(buffer, message);
        std::strcat(buffer, "\n\n(press cancel to debug)");
        if (MessageBoxA(windowHandle, buffer, "Fatal error!", MB_OKCANCEL | MB_ICONHAND) == IDCANCEL) __debugbreak();
        NXBasics::Runtime_RaiseSignal(22);
    }

    // Original function: FUN_0047ab1a. The Windows callback receives three arguments.
    void __cdecl XWS_AssertBox(const char* message, const char* sourceFile, std::int32_t line)
    {
        XWS_SystemMouseCursor_Show();
        ClipCursor(nullptr);
        auto* application = NXSys2DApp::CApplication::sTheObjectPtr;
        application->FlipToGDISurface();
        const HWND windowHandle = application->GetWindowHandle();
        char buffer[1000];
        NXBasics::Runtime_PrintStringN(buffer, sizeof(buffer) - 1, "%s\n\nFile %s - Line %d\n\n(press retry to debug)", message, sourceFile, line);
        buffer[sizeof(buffer) - 1] = '\0';
        const int selection = MessageBoxA(windowHandle, buffer, "XWS assert", MB_ABORTRETRYIGNORE);
        if (selection == IDABORT) NXBasics::Runtime_RaiseSignal(22);
        if (selection == IDABORT || selection == IDRETRY) __debugbreak();
    }

    // Original function: FUN_0047a9ea
    void __cdecl XWS_WarningBoxV(const char* format, ...)
    {
        char buffer[1000];
        va_list arguments;
        va_start(arguments, format);
        // Match the legacy _vsnprintf limit and explicit final terminator.
#pragma warning(suppress: 4996)
        _vsnprintf(buffer, sizeof(buffer) - 1, format, arguments);
        va_end(arguments);
        buffer[sizeof(buffer) - 1] = '\0';
        XWS_WarningBox(buffer);
    }

    // Original function: FUN_0047aa20
    void __cdecl XWS_WarningBox(const char* message)
    {
        const bool restoreHiddenCursor = !XWS_SystemMouseCursor_IsShown();
        if (restoreHiddenCursor)
        {
            XWS_SystemMouseCursor_Show();
        }

        ClipCursor(nullptr);
        auto* application = NXSys2DApp::CApplication::sTheObjectPtr;
        application->FlipToGDISurface();
        const HWND windowHandle = application->GetWindowHandle();

        // The original builds a suffixed copy but displays the original message.
        // Omit the unused, unbounded copy while preserving the displayed text.
        if (MessageBoxA(windowHandle, message, "Warning!", MB_OKCANCEL | MB_ICONWARNING) == IDCANCEL)
        {
            __debugbreak();
        }

        if (restoreHiddenCursor)
        {
            XWS_SystemMouseCursor_Hide();
        }
    }
}
