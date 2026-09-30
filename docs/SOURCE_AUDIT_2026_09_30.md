# Production source audit — 2026-09-30

## Scope and limits

Scanned all 159 C/C++ files, approximately 49,000 lines, for live `__asm`, `DAT_`, `PTR_` and `UNK_` references. Comments and quoted strings were excluded from the live-data inventory. Indexed Windows disassembly references for all 55 initial live `DAT_` addresses; inspected the relevant Windows routines and complete xref sets for the replacements below. Mac references supplied class and function names only.

Ran the Win32 C++17 compiler over all 77 production compilation units. After correcting the desktop error, 71 pass syntax checking; six retain incomplete reconstruction dependencies. Ran MSVC static analysis over those 71 compilable units. This is a repository-wide static audit with targeted assembly verification, not a proof that every game path is functionally equivalent. No tests, test projects, project-file changes or game runtime verification were added.

## Applied corrections

- `XGuiDesktop.cpp`: replaced the destructor-alias expressions rejected by MSVC with qualified `CList` destructor calls. Qualified dispatch remains essential because the original desktop destructor clears the object's embedded vtables before destroying its lists (`00477010`, `00477082`).
- `C2EngineDisplayAccess.cpp`: replaced the inline signed division with C++ over volatile signed operands. MSVC `/Od` and `/O2` both generate `CDQ; IDIV`, retaining processor division faults on the current Win32 toolchain. The invalid-input behavior is toolchain-specific, not a portable C++ guarantee.
- `C2EngineDisplayAccess.cpp`: replaced `FCOMP; FNSTSW; SAHF; JNE` with ordered less-than/greater-than comparisons. Equal and unordered operands both skip the branch; a plain `!=` would change the NaN case.
- `C2EngineTooltipAccess.cpp`: replaced the inline house-stock call/comparison with the declared Windows calling convention and C++ comparison. `00431A88` returns signed integer stock, zero, or unsigned integer stock multiplied by the recovered finite `0.1f` operand and added to signed integer stock. Its bounded values cannot overflow a float or turn a nonzero amount into zero by float underflow. The original test before storing the float therefore permits this replacement.
- `MasterControlProgram.cpp` and `MasterControlProgram_PS_System.cpp`: connected stale `DexterString::StringPrint` and path-resolution calls to the existing implementations of `004E5D8B` and `00406542`. Verified `004E5C30` as string concatenation before replacing the stale append name with the already-used `strcat` API.
- `NetworkRuntime.cpp`, `NetworkShutdown.h`, `MainMenuGuiManager.cpp`: exposed the existing `004DE4C4` IP-address routine to its menu caller and corrected the menu's namespace for the existing global network accessor. No new routine body or wrapper was invented.

The five directly changed compilable units initially checked (`C2EngineDisplayAccess`, `C2EngineTooltipAccess`, `XGuiDesktop`, `NetworkShutdown`, `C2E2StaticDataSoundFX`) also generated production object files under `/Od` and `/O2`, without warnings. The later network interface change passes Win32 syntax checking and `/Od`/`/O2` object generation with the existing deprecated Windows/Winsock API warnings. Six incomplete units were checked again after repairing their known stale interfaces.

## Resolved live data references

| Windows address | Replacement | Evidence |
| --- | --- | --- |
| `004F5254` | Local `cMenuGameSpeed = 12.0f` | Exact PE bytes `00 00 40 41`; menu call to `0040AEBD` |
| `004F7E48` | Local `cTcpIpServiceProviderGuid` | Exact 16-byte GUID matches `CLSID_DP8SP_TCPIP`; literal storage follows the existing network GUID convention and adds no SDK GUID link dependency |
| `004FE238` | `OriginalStrings::UnknownPlayerName` | Exact PE string `?`; missing-player-name use |
| `00500818` | `OriginalStrings::DirectoryAndFileNameFormat` | Exact PE string `%s\\%s`; path formatting uses |
| `0050716C` | `OriginalStrings::RightArrow` | Exact PE string `>`; chat prompt use |
| `00509910` | `OriginalStrings::FileTransferServiceUnavailable` | Exact PE diagnostic, including trailing LF |
| `00568A50` | `NC2E2::gStaticDataPatternManager` | Singleton written at `0048E268`, cleared at `0048E584`; pattern group entry points and recovered manager name |
| `00568AEC` | `NC2E2::gStaticDataLandscapesManager` | Singleton written at `0048F7B9`, cleared at `00490359`; landscape group entry points and recovered manager name |

The two named static-data manager pointers remain external dependencies; their manager bodies and neighboring static-data regions have not been reconstructed by this audit. Naming them does not make the full executable linkable.

## Remaining inline assembly

Three of nine initial blocks were replaced. Six remain deliberately visible:

| File / routine | Reason retained |
| --- | --- |
| `C2EngineDisplayAccess.cpp`, `DisplayDescription_ConvertScaledFloat` | Original `004E6F4C` changes/restores x87 rounding, converts to signed 64 bits, and exposes its low DWORD. Out-of-range and exception behavior is not supplied by an unchecked portable C++ cast. |
| `C2EngineTooltipAccess.cpp`, vehicle goods conversion | Original zero-extended DWORD is loaded with `FILD QWORD` and stored as a float under x87 control. A host conversion may use different rounding state or add another CRT helper. |
| `XCrtFileAccess.cpp`, `Runtime_InitializeFloatingPoint` | `FNCLEX` clears x87 exception state; no existing C++ runtime equivalent is available in the reconstructed runtime. |
| `XCrtFileAccess.cpp`, `Runtime_ProbePentiumDivision` | Original `004EB935` probes the actual x87 division behavior, including its intermediate precision and final double store. |
| `XCrtFileAccess.cpp`, `Runtime_ControlX87`, read block | `FWAIT; FNSTCW` reads the actual hardware control word. |
| `XCrtFileAccess.cpp`, `Runtime_ControlX87`, write block | `FLDCW` applies x87 masks, rounding and precision without replacing the project's runtime with another CRT. |

These are hardware operations or conversions with unresolved replacement requirements, not untranslated game algorithms. Replacing them requires a proven hardware/runtime abstraction; moving assembly into another file would merely hide the same dependency.

## Logic review and analyzer findings

MSVC reported 82 distinct static-analysis diagnostics after deduplicating repeated header reports and including SDK warnings. They are findings to classify, not 82 confirmed reconstruction bugs. The complete raw compiler/analysis outputs and 71 XML reports were kept in the system temporary directory; generated XML was moved out of the checkout.

Reviewed representative high-risk paths against current source and Windows ASM:

- `XFileAccess.h` unterminated-string warning: the normal `strncpy(..., 0x103)` path retains its final zero byte because `File_InitObject` first zeroes the entire `0x104`-byte name buffer. The independent unchecked system-path concatenation remains original behavior.
- `NetworkRuntime.cpp`, `StoreCommandMessageData` (`004E1310`): lengths above 200 are reported, but the original still reaches `memmove`. Adding an early return would change Windows behavior. This is a confirmed original bounds hazard.
- `NetworkRuntime.cpp`, `OpenNetworkLog` (`004DB8EA`): the original reserves a `0x100`-byte temporary path buffer and passes it to `GetTempFileNameA`; its API-size warning is an original hazard, not a translation-only difference.
- `DSManager.cpp`, buffer restoration: the original retries every nonzero `Restore` result. Changing this loop to retry only `DSERR_BUFFERLOST` would change its error path.
- `XCrtFileAccess.cpp`, signal callback warnings: the zero-action path calls `Runtime_QuickExit` before reaching the indirect callback. Analyzer flow through the reconstructed termination routine requires separate treatment.
- Remaining nullable-allocation findings in picture/string/memory/INI/network/CRT code need routine-specific assembly confirmation. They were not suppressed by speculative guards or allocator substitutions.
- No blanket changes were made to deprecated Windows APIs, original `MEM_DECOMMIT` paths, original indexing quirks or native allocation pairs merely to silence diagnostics.

The `datax` startup paths were checked against actual `Game.exe` strings and retained. The disassembly's human-readable string comments do not reliably reproduce those bytes; changing them to `data` based on comments alone would have been incorrect.

## Incomplete compilation units

After the initial interface repairs, six units reported 173 compiler diagnostics. The [property-options and directory block](PROPERTY_OPTIONS_IO.md) reduced that to 160; the [CD-audio manager block](CD_AUDIO_MANAGER.md) reduced it to 139; the [GUI desktop/resolution block](GUI_DESKTOP_RESOLUTION.md) reduced it to 129 and completed the resolution-change unit. The current 78-unit syntax pass has 73 passing units and these five incomplete units, primarily undeclared original routines/globals and unreconstructed manager methods:

| Unit | Diagnostics |
| --- | ---: |
| `MainMenuGuiManager.cpp` | 79 |
| `MasterControlProgram.cpp` | 14 |
| `MasterControlProgram_PS_InGame.cpp` | 24 |
| `MasterControlProgram_PS_MainMenu.cpp` | 11 |
| `MasterControlProgram_PS_System.cpp` | 1 |

Examples include DirectMusic methods, save/map managers and GUI startup/shutdown helpers. The global-options load/save/default paths and Windows CD-audio manager are now reconstructed. A full build/link is not verified. Translate those owning blocks and connect established methods; do not fabricate declarations or placeholder bodies solely to remove compiler errors.

## Remaining live DAT inventory

There were 47 distinct live `DAT_` symbols after the initial pass; the CD-audio follow-up resolved `0050F69C`, and the desktop block resolved `005690B8` and `0051261C`, leaving 44. No live `PTR_` or `UNK_` symbols were found. Address comments remain intentionally for traceability. The current per-file inventory follows.

| Live symbol | Files |
| --- | --- |
| `DAT_0050f620` | `MasterControlProgram.cpp` |
| `DAT_0050f634` | `MasterControlProgram_PS_TitleScreen.cpp` |
| `DAT_0050f63c` | `MasterControlProgram_PS_PreIngameFmv.cpp` |
| `DAT_00510838` | `C2E2AmbientSoundManagerAccess.cpp`, `C2E2AnimalSoundManager.cpp`, `C2EngineDrawingAccess.cpp` |
| `DAT_00510848` | `WorldDisplayElementAccess.cpp` |
| `DAT_0051089d` | `WorldDisplayElementAccess.cpp` |
| `DAT_005109c5` | `WorldDisplayElementAccess.cpp` |
| `DAT_005109d0` | `MasterControlProgram.cpp` |
| `DAT_005109ec` | `C2EngineDrawingAccess.cpp`, `MasterControlProgram_PS_InGame.cpp` |
| `DAT_00510a18` | `C2AnimalAccess.cpp`, `C2EngineTooltipAccess.cpp` |
| `DAT_00510d4c` | `C2AnimalIterator.cpp` |
| `DAT_00511240` | `GuiToolsBase.cpp` |
| `DAT_005114ac` | `MainMenuGuiManager.cpp` |
| `DAT_005114b4` | `MainMenuGuiManager.cpp` |
| `DAT_00511540` | `ApplicationGlobals.h`, `ApplicationStartup.h` |
| `DAT_00511570` | `ApplicationGlobals.h`, `ApplicationStartup.h`, `C2GameMapAccess.cpp`, `C2MapSectorAttachment.cpp`, `C2MapWalkSectorManager.cpp` |
| `DAT_00512620` | `ApplicationGlobals.h`, `ApplicationStartup.h`, `C2MapWalkSectorManager.cpp` |
| `DAT_00554d20` | `ApplicationGlobals.h`, `ApplicationStartup.h` |
| `DAT_00554d98` | `ApplicationGlobals.h`, `ApplicationStartup.h` |
| `DAT_00554da4` | `MasterControlProgram.cpp` |
| `DAT_00554da8` | `ApplicationGlobals.h`, `ApplicationStartup.h` |
| `DAT_005550d4` | `MasterControlProgram_PS_InGame.cpp`, `MasterControlProgram_PS_MainMenu.cpp`, `WorldDisplayElementAccess.cpp` |
| `DAT_005550d8` | `C2EngineTooltipAccess.cpp`, `MasterControlProgram_PS_InGame.cpp` |
| `DAT_005550e4` | `C2EngineDrawingAccess.cpp` |
| `DAT_005550f8` | `C2EngineDrawingAccess.cpp` |
| `DAT_00555110` | `C2EngineDisplayManager.cpp` |
| `DAT_00568358` | `ApplicationGlobals.h`, `ApplicationStartup.h` |
| `DAT_005683b0` | `C2EngineDisplayAccess.cpp` |
| `DAT_005683c0` | `C2EngineDisplayAccess.cpp` |
| `DAT_005683c4` | `C2EngineDisplayAccess.cpp` |
| `DAT_00568428` | `C2EngineDisplayAccess.cpp` |
| `DAT_005685e8` | `ApplicationGlobals.h`, `ApplicationStartup.h`, `C2EngineDrawingAccess.cpp` |
| `DAT_00568a64` | `C2EngineDrawingAccess.cpp` |
| `DAT_00568a7c` | `C2EngineDrawingAccess.cpp` |
| `DAT_00568b44` | `C2EngineDrawingAccess.cpp` |
| `DAT_00569078` | `ApplicationGlobals.h`, `ApplicationStartup.h` |
| `DAT_00569080` | `ApplicationGlobals.h`, `ApplicationStartup.h` |
| `DAT_00569088` | `ApplicationGlobals.h`, `ApplicationStartup.h` |
| `DAT_00569090` | `ApplicationGlobals.h`, `ApplicationStartup.h` |
| `DAT_00569098` | `ApplicationGlobals.h`, `ApplicationStartup.h` |
| `DAT_005690a0` | `ApplicationGlobals.h`, `ApplicationStartup.h` |
| `DAT_005690d8` | `C2EngineTooltipAccess.cpp` |
| `DAT_005690dc` | `InGameGuiManager.cpp` |
| `DAT_00569418` | `ApplicationGlobals.h`, `ApplicationStartup.h` |
