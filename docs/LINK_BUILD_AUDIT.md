# Win32 full-build and unresolved-symbol audit

Snapshot: 2026-10-03, HEAD 3ea8c64 plus current uncommitted message/transport reconstruction.

## Reproduced result

The reported Debug build produces 424 unresolved reference diagnostics plus LNK1120: 305 distinct decorated symbols. Several translation units reference the same missing symbol, so 425 error-list entries do not mean 425 different missing functions.

Both Debug|Win32 and Release|Win32 now build and link successfully with the user's explicitly authorized temporary dummies. There are zero unresolved reference diagnostics and zero unresolved linker symbols. TemporaryLinkStubs.cpp supplies 146 unfinished original entries: void functions do nothing, pointers return nullptr, booleans return false and numeric results return zero. These are a compilable checkpoint, not faithful runtime implementations.

| Measure | Reported baseline | Before dummies | Current Debug and Release |
| --- | ---: | ---: | ---: |
| Unresolved reference diagnostics (LNK2001/LNK2019) | 424 | 152 | 0 |
| Distinct decorated symbols | 305 | 146 | 0 |
| Missing original addresses | 256 | 146 | 0 unresolved; 146 temporary dummies |
| Missing data/system imports | 24 system imports plus data | 0 | 0 |

[TEMPORARY_LINK_STUBS.csv](TEMPORARY_LINK_STUBS.csv) preserves all 146 addresses and their referencing objects. [UNRESOLVED_LINK_SYMBOLS.csv](UNRESOLVED_LINK_SYMBOLS.csv) is the current empty linker inventory. Provisional real reconstruction coverage remains 1,890/5,056; the dummies are excluded. The real GUI-message and transport-reservation blocks are described in [NATIVE_LINK_SUPPORT.md](NATIVE_LINK_SUPPORT.md). No game-runtime verification is claimed. Both complete Rebuild runs returned exit code zero with zero compiler/linker errors. Debug reported zero warnings; Release reported one cleanup warning for the old root-level Release/OpenVikings.exe (access denied). The newly generated OpenVikings/Release/OpenVikings.exe is present and verified as PE32/x86, as is the Debug executable.

## Corrections applied

- Both project configurations explicitly link winmm.lib (timeGetTime, MCI, mixer and MMIO), dsound.lib (DirectSoundCreate8), ws2_32.lib (Winsock), dxguid.lib and dinput8.lib (the existing mouse data format). Inherited dependencies/options are retained. No library pragmas were added.
- The installed SDK contains dinput8.lib but no legacy dinput.lib. LegacyDirectInput.def records the original DINPUT.dll export DirectInputCreateA. An incremental pre-Link target generates a Win32 import library with the installed MSVC librarian in IntDir. The linker alias binds the existing stdcall reference _DirectInputCreateA@16 to that library's thunk _DirectInputCreateA. Dumpbin confirms the imported DLL/name are DINPUT.dll / DirectInputCreateA, matching the installed 32-bit DLL export. The thunk forwards to the actual stdcall API; no source/interface migration to DirectInput8Create or runtime DLL wrapper was introduced.
- The two mapped but unresolved originals were rewired: tooltip sorting uses existing Runtime_QuickSort (004E7E40), and ToolGraphics_SetPalettePtr (004B95A4) moved from private menu storage to a shared GUI entry used by the overview and existing callers. Bodies retain their original behavior.
- Seven old DAT declarations now use existing canonical bindings: 00510D4C animal types, 00511240 good types, 00568428 particle manager, 005550E4 palette entries, 005550F8 palette pointer, 00568A64 house entries and 00568A7C landscape entries. SoundFX's two manager declarations now use canonical 00568A50/00568AEC owners; Windows 0048E115/0048E158 load those exact pointers before group lookup. No independent zero-filled pointer copies were created.
- Defined the previously missing MMX cache as the complete four-byte 00554D58..00554D5B region. The Windows PE places it in zero-filled virtual data; all five accesses use the two flag bytes in CPU_HasMMX, and the other two bytes have no exported references. Mac/SDL does not supply this Windows cache's identity or behavior.
- Defined the dynamic-network initial-update flag using the verified first byte at 0050AA2C: one, not a guessed zero. PE bytes are 01 00 00 00; all three Windows xrefs access the first byte at 004DF27F/004DF28B/004DF2AE.
- LegacyDirectInput.def is included in the project and Platform/Input filter. Generated Debug/Release outputs are ignored by Git. Production source/header inclusion, Win32 target, compiler settings and output-path settings are retained.

## Build-only checkpoint and subsequent replacement

Extern declarations alone do not provide executable behavior. The previous build compiled 140 production units but failed at 146 original addresses. The user authorized temporary dummy bodies on 2026-10-03 to establish a compilable checkpoint; all such definitions are isolated and explicitly marked. Three recently introduced GUI calls now use the Mac names directly. The subsequent naming pass replaces all 143 remaining legacy FUN identifiers consistently. See [FUNCTION_NAMES.md](FUNCTION_NAMES.md) for exact Mac correspondence and Windows-only naming provenance.

Project and filters match 141 implementation files and 140 headers, including the temporary unit. Shared S2DEngineDisplayRectangle moved unchanged into its owning header so the by-value stub uses the same 24-byte type. Main target remains Win32/x86. Executables are emitted into OpenVikings/Debug and OpenVikings/Release, as requested. No tests/test projects were added.

For each real replacement, preserve Windows ABI/behavior, remove its temporary definition and CSV row, and keep declarations/definitions consistent. Run Debug/Release builds only when explicitly requested by the user. Future new calls must use names established from the Mac source; uncertain correspondence must not be hidden by guessed naming. Zero linker errors must remain separate from the genuine reconstruction progress metric.

## Reproduction and artifacts

MSBuild OpenVikings.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=Win32 /m

MSBuild OpenVikings.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=Win32 /m

Logs are saved outside the repository in C:/Users/ravo/AppData/Local/Temp/OpenVikings-rebuild-debug-temporary-stubs.log and OpenVikings-rebuild-release-temporary-stubs.log. Pre-dummy comparison logs are OpenVikings-link-debug-gui-reservations.log and OpenVikings-link-release-gui-reservations.log. The read-only progress script explicitly excludes TemporaryLinkStubs.cpp. No game launch/runtime success is claimed.

## Subsequent semantic naming and output separation

The checkpoint is committed as a77fd47. All active FUN identifiers have semantic names and their temporary bodies remain unfinished. Current output settings separate EXE, symbols and intermediate products; these changes have static verification only, with no new build. See [FUNCTION_NAMES.md](FUNCTION_NAMES.md).
