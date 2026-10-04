# Engine and game startup

Reviewed on 2026-10-01 against Windows x86 assembly, current C++/Git history and Mac symbol/parameter names. Behavior below is established from Windows; Mac is used only for naming.

The subsequent [campaign/static-string block](CAMPAIGN_AND_STATIC_STRINGS.md) implements the first two subordinate constructors and their direct load/save/warning paths. The subsequent [clean-map/savegame block](CLEAN_MAPS_AND_SAVEGAMES.md) implements two more constructors and GUID generation. Current remaining subordinate constructor count is 50; the validation and allocation sections below record the earlier startup/campaign snapshot.

## Scope and original entry points

This block reconstructs the startup orchestration and the directly established state/notification helpers. It does not complete every subordinate constructor or resource loader.

| Windows entry | Reconstructed interface |
| --- | --- |
| 00401985 | CMasterControlProgram::GameEngine_StartUpEngine |
| 004019DA | CMasterControlProgram::GameEngine_StartUpGame |
| 00406B70 | Game_Construct, CGame constructor |
| 00406F7D | Game_StartUp, CGame::StartUp |
| 004075AB | Misc_EngineIsInitialised, separate tail-forwarding entry |
| 0040796B | GameTime_Reset, CGame::l_GameTime_Reset |
| 00407BE0 | GameTime_SetSpeed |
| 00408F18 | AIManager_EnableAI, CAIManager::EnableAI |
| 00411BA6 | GameMapModifyManager_Construct |
| 00411BCC | GameMapModifyManager_StartGameMapModify |
| 00411BEF | GameMapModifyManager_StopGameMapModify |
| 00411C18 | GameMapModifyManager_ForceUpdate |
| 00415565 | StaticGoodTypeDataManager_Inform_EngineIsInitialised |
| 0047DCC4 | E2Manager_Construct, CE2Manager constructor |

There are 14 added original bodies. Existing InGame/MainMenu entry paths call the two named master-control methods. Mechanical allocation templates are C++ adapters, not invented implementations of unresolved original functions.

## Master-control startup order

Engine startup checks the engine flag at +0x28. When clear, it conditionally constructs the one-byte CGame and CE2Manager owners using Runtime_AllocateWithNewHandler(size, 1), then unconditionally informs static good data that the engine is initialized and sets the flag. It independently checks the game flag at +0x29, calls Game_StartUp(20, 20, false, nullptr, false) if needed, and sets that flag.

Game startup independently ensures the game flag. A nonnull start_map property, including an empty string, invokes original map loading with true/true/false, centers the GUI, removes the property and returns. No loader result is checked.

The other path captures whether the embedded startup type is 2 before clearing/transferring records. It resets the global 0x120-byte startup record, copies a nonzero-type member record into it and resets the member. It snapshots the global, invokes original 0040743A and restores every byte afterward. Writes made by that executor to the global record are deliberately overwritten, matching rep movsd.

Type 3 searches 20 twelve-byte entries, comparing entry +0x34 + 12*i against the saved local identifier at +0x28. Type 4 visits the same 20 entries using their +0x2C discriminator. Discriminator 1 copies the entry identifier to the game player at +0x14 and sets player +4 to 1. Other nonzero discriminators convert an existing +4 value of 1 into 2, clear +0x14 and enable both AI subobjects. It then invokes original 004079CF and searches the current global local identifier. Both searches retain player zero if no match is found. The GUI setter receives the selected player and true; centering is skipped only for the earlier captured type 2.

The game-player stride 0x4A80 and startup-record offsets come from Windows accesses. The player-role and startup discriminator values remain raw integers; no unsupported enum or complete player layout is invented. AI enable retains the original signed upper-bound-only check, active-player-byte gate and unchecked pair of subobject pointers.

## Canonical static regions

C2GameState.h represents the entire Windows CGame::mStaticVars region, 00510818..005109CF, as one packed-four-byte, 0x1B8-byte record. The name is supported by the corresponding Mac owner symbol; Windows constructor clearing and xrefs establish its extent. Initialization, the two-word random state at +0x94, game-clock fields and loader pointers now share this backing storage. Existing DAT labels are references into this record, not separate external variables.

| Windows address | Established representation / remaining identity |
| --- | --- |
| 00510818 | Initialized byte, also used by Game_ShutDown |
| 00510838 | Dword; cleared/reset to 1 by clock reset and incremented/consumed by game-time update paths |
| 00510840 | Double; unsigned timeGetTime value converted to double |
| 00510848 | Double; current speed in the reviewed clock paths, original DAT label retained |
| 00510850 | Double; 1000/speed when speed > 0.1, otherwise zero |
| 00510858 | Double; reset to 12 and replaced by old speed when old speed > 1 |
| 0051089D | Byte; cleared by SetSpeed and consumed by pause/display paths |
| 005108AC..005108B3 | Existing random current-state/seed pair |
| 005108B4 | Dword; both construction/startup write 1, full semantic identity unresolved |
| 005109BC | Existing owning CIniFileReader pointer; map-loading and shutdown xrefs |
| 005109C0 | Existing opaque owning pointer; map-loading/shutdown use established original destruction boundary |
| 005109C4 | Network-mode byte passed to Game_StartUp; downstream network/game xrefs |
| 005109C5 | Byte set by 0040B24A, consumed in network/input paths; complete identity unresolved |

All other bytes remain explicit opaque gaps pending their respective subsystem review. Field widths, random offset, pointer offsets and total size have x86 static assertions. Construction clears the entire region, whereas clock reset clears only +0x20..+0x87. Clock, loader and render callers are bound to the same record.

The complete four-byte CGameMapModifyManager::mStaticVars at 00510A24 is gGameMapModifyNestingDepth; its identity is inferred from all constructor/start/stop/force/modified-notification xrefs and supported by the Mac class/static symbol. Owner 00510A28 remains the established singleton. Start increments before the depth-one callback 0x31; Stop ignores zero, calls 0x32 with the true flag at depth one and decrements the actual global after callback return. ForceUpdate, when nonzero, calls 0x32/true followed by 0x31/false without rechecking depth. Callback results are ignored, and unsigned wrap/reentrant writes are preserved. The separate GameMapModified forwarding entries remain open.

CE2Manager clears its own one-byte object and the complete twelve-byte desktop region 00554D98..00554DA3. Its first eight bytes reuse the existing DAT block; its final pointer reuses the existing desktop Bob bitmap owner, now exported rather than duplicated. The two game-control raw references at 005109EC now use the canonical gGameControl owner.

## Allocation/constructor inventory

The three paths contain 65 conditional constructor allocations. Twelve already reconstructed native constructors and the new map-modification constructor are used directly. The other 52 constructor calls remain explicit original externals with verified x86 object/stack arguments. Allocation sizes and call order below come from Windows. Allocation failure skips only that constructor, without resetting its singleton or adding rollback.

### CGame construction (00406B70)

| Order | Original constructor | Windows bytes | Current implementation | Progress update after allocation |
| --- | --- | --- | --- | --- |
| 1 | 004170B5 | 0x1 | CStaticStringsManager | Yes |
| 2 | 00416C1E | 0x218 | CCampaignStaticDataManager | Yes |
| 3 | 004163B7 | 0x30 | Original external | Yes |
| 4 | 00415DF6 | 0x20 | Original external | Yes |
| 5 | 004159D0 | 0x1 | Original external | Yes |
| 6 | 00415596 | 0x1 | Original external | Yes |
| 7 | 00414FEE | 0x1 | Original external | Yes |
| 8 | 004149B2 | 0x1 | Original external | Yes |
| 9 | 00414462 | 0x1 | Original external | Yes |
| 10 | 0041418E | 0x1 | Original external | Yes |
| 11 | 004134D1 | 0x1 | Original external | Yes |
| 12 | 0041300A | 0x1 | Original external | Yes |
| 13 | 00412A5D | 0x1 | Original external | Yes |
| 14 | 0041277C | 0x1 | Original external | Yes |
| 15 | 00412299 | 0x1 | Original external | Yes |
| 16 | 004121A9 | 0x1 | Original external | Yes |
| 17 | 00411E81 | 0x1 | Original external | Yes |
| 18 | 00411CA8 | 0x1A0 | CCallbackManager | Yes |
| 19 | 00411BA6 | 0x1 | GameMapModifyManager_Construct (new) | Yes |
| 20 | 00410D14 | 0x9A0 | Original external | Yes |

### CE2Manager construction (0047DCC4)

| Order | Original constructor | Windows bytes | Current implementation | Progress update after allocation |
| --- | --- | --- | --- | --- |
| 1 | 004905B9 | 0x1C | Original external | Yes |
| 2 | 0049039E | 0x14 | Original external | Yes |
| 3 | 00480A20 | 0x1 | Original external | Yes |
| 4 | 0048F79F | 0xFA4 | Original external | Yes |
| 5 | 0048E651 | 0x1 | Original external | Yes |
| 6 | 0048E24E | 0xFA4 | Original external | Yes |
| 7 | 0048DC74 | 0x518 | CE2StaticDataSoundFXManager | Yes |
| 8 | 0048D714 | 0x1 | Original external | Yes |
| 9 | 0048D194 | 0x1 | Original external | Yes |
| 10 | 0048BD66 | 0x1BA58 | Original external | Yes |
| 11 | 0048ACFB | 0x1 | Original external | Yes |
| 12 | 0048AB5C | 0x1 | Original external | Yes |
| 13 | 0048A969 | 0x8 | CE2StaticDataAnimalSoundManager | Yes |
| 14 | 0048A4BA | 0x1 | Original external | Yes |
| 15 | 0048A028 | 0x1 | Original external | Yes |
| 16 | 00489F75 | 0x1 | CLevelOfDetailManager | No |
| 17 | 00489DE0 | 0x8 | Original external | No |
| 18 | 004811AE | 0x18 | C2DEngineDisplayManager | Yes |
| 19 | 00486336 | 0x2F7F80 | Original external | Yes |
| 20 | 0047F3C7 | 0x64 | Original external | Yes |
| 21 | 004860AB | 0x8C | CE2AnimalSoundManager | Yes |
| 22 | 00485AA1 | 0x1 | Original external | Yes |
| 23 | 00485453 | 0xD24 | Original external | Yes |
| 24 | 00484BD1 | 0xC | Original external | No |

### CGame startup (00406F7D)

| Order | Original constructor | Windows bytes | Current implementation | Progress update after allocation |
| --- | --- | --- | --- | --- |
| 1 | 00408FDD | 0x5D204 | Original external | No |
| 2 | 004231D7 | 0x1 | Original external | No |
| 3 | 0041CF58 | 0x81CAC | Original external | No |
| 4 | 00407C8B | 0xA0 | Original external | No |
| 5 | 0040BB10 | 0x30 | Original external | No |
| 6 | 0041CD2F | 0x30 | Original external | No |
| 7 | 0040B271 | 0x30 | CHumanArray | No |
| 8 | 0041C3AA | 0x5D8 | Original external | No |
| 9 | 0041C076 | 0x30 | CAnimalArray | No |
| 10 | 0040B5FC | 0x30 | CVehicleArray | No |
| 11 | 0040B94A | 0x30 | Original external | No |
| 12 | 0040AA24 | 0x30 | CHouseArray | No |
| 13 | 0041AE53 | 0x1368 | Original external | No |
| 14 | 0041AA59 | 0xA8 | Original external | No |
| 15 | 00419F41 | 0x34 | Original external | No |
| 16 | 00419DDB | 0x960 | Original external | No |
| 17 | 00419BC5 | 0x8 | Original external | No |
| 18 | 004196CD | 0xAA0 | Original external | No |
| 19 | 00419032 | 0x740 | Original external | No |
| 20 | 004175EC | 0x460 | Original external | No |
| 21 | 0041714B | 0xFA0 | Original external | No |

CGame construction makes 20 progress updates, including skipped constructors. CE2Manager makes 21, omitting LOD, MacroMap and the final weather-sector allocation. CGame startup makes none. Game construction publishes its owner before clearing statics. The fixed seed 0x18273645 has thirteen set bits, so original CRandom's adjustment branches are not taken; the original seed-then-current-state write order is retained in both construction/startup. Game startup preserves the 0x120-byte startup record around an existing game's shutdown, sets network mode before game-control creation and sets initialization after the final reset and state writes.

## Engine callback registration

Between MacroMap creation and display-manager allocation, CE2Manager registers exactly 35 callbacks through the canonical CCallbackManager singleton. Registration return values are ignored. User data is always zero; the original order and priorities are:

| Callback | Types, in registration order | Priority |
| --- | --- | --- |
| Existing E2Manager_CallbackSomethingInLogicXtructed, 0047E679 | 02, 03, 04, 05, 06, 07, 08, 09, 0A | 10 |
| Original external 0047E936 | 0E, 0F, 10, 11 | 10 |
| Original external 0047EA9F | 12, 13, 14, 15, 16 | 10 |
| Original external 0047EAF3 | 31, 32, 33, 0D | 2 |
| Original external 0047EC5B | 17, 1C, 21, 22, 1F, 20, 23, 27, 24, 1D, 18, 25, 26 | 10 |

The four callback bodies remain open, with signatures matching the established manager interface. The distinct 004075AB forwarding entry calls static-good initialization at 00415565. That method visits 56 entries, stride 0xD0 with inclusive last offset 0x2CB0, and resolves each nonnull +0x20 name through original 004807C8(name, true), storing its result at +0x24. Null results are retained.

## Game-clock reset and speed

Reset clears exactly 0x68 bytes from 00510838, writes counter 1, reads timeGetTime as an unsigned 32-bit value converted to double, stores 12 as the remembered speed, and selects speed 12 or 0. The enabled case then invokes original 004079CF. SetSpeed refreshes the timestamp when the old speed is below 0.1 or unordered, remembers old speeds above 1, stores the new speed, computes the interval only for an ordered speed above 0.1, and clears 0051089D. Windows PE double constants at 004F3400/004F3350/004F3410/004F3418 establish 12/1/1000/0.1. No clamping, timer-wrap correction or additional NaN branch is added.

## Preserved failure/continuation behavior

- Null owner/subowner allocations skip only their constructor. Startup flags are still set and subsequent progress/notification/registration calls continue; failed callback-manager creation can therefore leave later unchecked accesses. No rollback or success status is introduced. README OA046 / FP018.
- The start_map path accepts any nonnull string and proceeds to centering/removal without a checked load result. The other path restores the complete pre-executor snapshot, ignores callback/scheduling outcomes and retains player zero on an unmatched identifier. README OA047 / FP019.
- AI enabling retains the original upper-only signed bound check and unchecked subobject pointer writes. The reviewed caller supplies 0..19; broader caller reachability remains open. README FP019.
- Map-modification callbacks retain unchecked manager access, ignored results, post-callback depth decrement and forced callback ordering even if a callback changes depth. README OA048 / FP020.
- Existing runtime allocator/new-handler semantics, display-manager vtable clearing and list-node failure paths remain covered by their existing implementations and findings; subordinate constructor failures are not claimed to have been completely reviewed here.

## Explicit remaining boundaries

| Boundary | Remaining work |
| --- | --- |
| 52 subordinate constructors above | Reconstruct full owning initialization/resource-loading trees using canonical singleton/array/list storage |
| 0040743A | Startup-record executor and its load/network branches |
| 0040A68B | Map loader and its direct ownership/diagnostic tree |
| 004079CF | Network/game-time scheduling helper and its dependencies |
| 00416B2D | Saved-state initialization at CGame statics +8 |
| 004807C8 | Engine named-good/resource lookup |
| 0047E936, 0047EA9F, 0047EAF3, 0047EC5B | Four registered engine callback bodies |
| 004A0C6D, 004A2F86 | GUI controlling-player setter and centering entry |
| 00411C48, 00411C84 | Separate map-modified notification entries |

No substitute bodies or guessed standard-library implementations were supplied for those original calls. Next connected block after the campaign/static-string follow-up: clean-map/savegame constructors 004163B7 / 00415DF6, followed by the remaining static managers; then CE2Manager resource initialization and the startup executor/map loader. Existing names from Mac guide semantic identification only.

## Validation and project integration

- All ten directly checked units generate Win32 C++17 /Od and /O2 objects: C2GameEngineStartup, MasterControlProgram_GameStartup, C2EngineDisplayAccess, C2EngineDrawingAccess, C2EngineLifetimeAccess, C2LogicLifetimeAccess, C2E2AmbientSoundManagerAccess, C2E2AnimalSoundManager, WorldDisplayElementAccess and WinMain. Existing WinMain warnings remain.
- Complete production syntax inventory: 87/90 pass, with 103 diagnostics in MainMenuGuiManager (78), MasterControlProgram_PS_InGame (17), and MasterControlProgram_PS_MainMenu (8). Four previous caller diagnostics are resolved. This is compilation coverage, not full linking or runtime equivalence.
- Both new source files and both new headers are included in OpenVikings.vcxproj and OpenVikings.vcxproj.filters, under Logic and Application\States as appropriate. All 90 production implementations and 99 headers have synchronized entries.
- No tests or test projects were added. Full link/runtime validation remains open because original constructor/load/callback boundaries and unrelated incomplete units remain unresolved.

## Engine map IO follow-up - 2026-10-04

The historical boundary table above is superseded for 0040743A (all four startup map/savegame branches), 0047EA9F (engine MapIO) and 0047EAF3 (dirty rectangles/road overlays), now reconstructed in [ENGINE_MAP_IO.md](ENGINE_MAP_IO.md). The earlier ground block also replaced 00411C48/00411C84. A current direct-reference inventory finds eight still-dummy constructor names in C2GameEngineStartup.cpp; indirect callbacks and other missing gameplay methods are separate dependencies. No builds or runtime run.
