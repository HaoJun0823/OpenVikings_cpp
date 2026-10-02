# Win32 full-build and unresolved-symbol audit

Snapshot: 2026-10-03, HEAD d64247c plus the user-requested checkpoint commit of menu/link and command reconstruction.

## Reproduced result

The reported Debug build produces 424 unresolved reference diagnostics plus LNK1120: 305 distinct decorated symbols. Several translation units reference the same missing symbol, so 425 error-list entries do not mean 425 different missing functions.

Both Debug|Win32 and Release|Win32 now compile all production units without compiler errors and reach linking. Each still fails with 153 unresolved reference diagnostics plus LNK1120: 147 distinct decorated symbols. There is no successfully linked executable or game-runtime verification.

| Measure | Reported baseline | Current Debug and Release |
| --- | ---: | ---: |
| Unresolved reference diagnostics (LNK2001/LNK2019) | 424 | 153 |
| Final LNK1120 distinct-symbol count | 305 | 147 |
| Unresolved system/DirectX imports and mouse-format symbol | 24 | 0 |
| Unresolved FUN decorated symbols | 258 | 147 |
| Distinct original FUN addresses | 256 | 147 |
| Remaining named non-FUN symbols | 47 | 0 |

The intermediate integration snapshot had 370 references / 267 symbols. Subsequent real reconstruction of inherited GUI methods, windows, desktop drawing/input, widgets, bitmap primitives, sound-token storage, the assertion callback and command-manager construction reduced it to the current set. See [GUI_LINK_CLOSURE.md](GUI_LINK_CLOSURE.md) and [NATIVE_LINK_SUPPORT.md](NATIVE_LINK_SUPPORT.md). Address 00410F00 now uses one native Windows-ABI entry; 00478A36 has one verified signature and body. The fully translated 00410FCC dispatcher exposed additional original dependencies rather than substituting fake definitions. No remaining raw target matches a currently mapped implementation.

## Corrections applied

- Both project configurations explicitly link winmm.lib (timeGetTime, MCI, mixer and MMIO), dsound.lib (DirectSoundCreate8), ws2_32.lib (Winsock), dxguid.lib and dinput8.lib (the existing mouse data format). Inherited dependencies/options are retained. No library pragmas were added.
- The installed SDK contains dinput8.lib but no legacy dinput.lib. LegacyDirectInput.def records the original DINPUT.dll export DirectInputCreateA. An incremental pre-Link target generates a Win32 import library with the installed MSVC librarian in IntDir. The linker alias binds the existing stdcall reference _DirectInputCreateA@16 to that library's thunk _DirectInputCreateA. Dumpbin confirms the imported DLL/name are DINPUT.dll / DirectInputCreateA, matching the installed 32-bit DLL export. The thunk forwards to the actual stdcall API; no source/interface migration to DirectInput8Create or runtime DLL wrapper was introduced.
- The two mapped but unresolved originals were rewired: tooltip sorting uses existing Runtime_QuickSort (004E7E40), and ToolGraphics_SetPalettePtr (004B95A4) moved from private menu storage to a shared GUI entry used by the overview and existing callers. Bodies retain their original behavior.
- Seven old DAT declarations now use existing canonical bindings: 00510D4C animal types, 00511240 good types, 00568428 particle manager, 005550E4 palette entries, 005550F8 palette pointer, 00568A64 house entries and 00568A7C landscape entries. SoundFX's two manager declarations now use canonical 00568A50/00568AEC owners; Windows 0048E115/0048E158 load those exact pointers before group lookup. No independent zero-filled pointer copies were created.
- Defined the previously missing MMX cache as the complete four-byte 00554D58..00554D5B region. The Windows PE places it in zero-filled virtual data; all five accesses use the two flag bytes in CPU_HasMMX, and the other two bytes have no exported references. Mac/SDL does not supply this Windows cache's identity or behavior.
- Defined the dynamic-network initial-update flag using the verified first byte at 0050AA2C: one, not a guessed zero. PE bytes are 01 00 00 00; all three Windows xrefs access the first byte at 004DF27F/004DF28B/004DF2AE.
- LegacyDirectInput.def is included in the project and Platform/Input filter. Generated Debug/Release outputs are ignored by Git. Production source/header inclusion, Win32 target, compiler settings and output-path settings are retained.

## Why syntax success did not imply a complete build

An extern declaration lets a caller compile without providing a definition. Original FUN boundaries were deliberately retained while their owning blocks remain open; the reconstructed game does not automatically call the old Game.exe at those addresses. Complete linking requires real definitions and matching declarations for every referenced function and data region.

The earlier isolated-object results proved compilation only. Full compilation now covers 136 production implementations and 138 headers with matching project/filter inclusion. Provisional source-address coverage is 1,819/5,049; it is neither a linkability nor a gameplay-completion percentage.

The user's examples 0041ACD6 and 00410F00 now use native PlayerStatisticManager_UpdateStatistics and UserNetworkCommandManager_AddCommand. Their connected sample/count/stock/iterator and command queue/send/receive/dispatch blocks are translated. Examples 00428736 and 0040BD33 remain real checksum and moveable-update reconstruction targets in GameTime_DoOneGameTick.

## Remaining inventory and next work

[UNRESOLVED_LINK_SYMBOLS.csv](UNRESOLVED_LINK_SYMBOLS.csv) contains all 147 decorated symbols and their referencing objects. Debug and Release currently produce the same symbol/reference sets. All 147 missing symbols are original functions at 147 addresses; there are no unresolved data or system-import symbols.

The sound token regions and assertion callback are complete. The title/pre-ingame FMV owners, Windows video-player tree, keyboard-assignment owner and particle-generator storage are connected. The particle clock byte retains an unresolved original name and writer. Other open families include per-tick managers/commands, deeper widgets/menu handlers and world/cursor drawing. No placeholder definitions were introduced.

## Reproduction and artifacts

MSBuild OpenVikings.vcxproj /t:Build /p:Configuration=Debug /p:Platform=Win32 /m

MSBuild OpenVikings.vcxproj /t:Build /p:Configuration=Release /p:Platform=Win32 /m

Complete logs and diagnostic CSVs are saved outside the repository in C:/Users/ravo/AppData/Local/Temp/OpenVikings-link-debug-before.log, OpenVikings-link-debug-command-actors.log and OpenVikings-link-release-command-actors.log. The parser is OpenVikings-link-audit.py; the PE/xref evidence script is OpenVikings-link-binding-evidence.py. Generated import-library headers were inspected with the x86 MSVC dumpbin tool. No tests/test projects were added, and the failed link was not presented as a successful build.
