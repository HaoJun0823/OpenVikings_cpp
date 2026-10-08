# Human selection information window - 2026-10-08

## Windows evidence and connected behavior

`InGameGuiSelectionWindowAccess.cpp` connects the single-human information path of selection update 004A0E9D. The selection checksum at input+AB0 is compared with manager+88; native selection elements/windows are removed before broadcasting command 2029 and publishing the replacement at manager+8C. Allocation remains 0x3D78 bytes. The existing ABI adapter now lives with the manager implementation rather than in TemporaryLinkStubs.cpp.

The connected basic section uses the Windows bottom-right placement, 200-pixel width, 16-pixel header, collapse button, persistent collapse flags, animated 60x80 world display, name button, job/child-age text and five status bars. Health uses the real maximum-hitpoints getter. Food, stamina, social and religion maxima are 10000, as established from the complete PE DWORD region 004F3B38..004F3B47. The native eligibility helper at 00463AD2 checks **adult age**, not male sex: children and adults with behavior flag bit 0 omit the four need bars. Adult women retain their need bars. Religion uses the disabled palette when the job does not need religion. Percentage arithmetic, signed/unsigned clipping, palette entries and label IDs follow 004AC3BE..004AC96B. Tooltips report health as current/maximum and needs as percentages.

Preview centering 004AE25F converts the human's map-moveable position to world pixels and subtracts 25 vertically. Frame command 2030 refreshes this position and rebuilds children when the engine-owned name changes. Portrait left-click and focus button center the main display. Native base-window deletion removes preview children; frame registration/unregistration and persisted collapse flags retain their ownership.

The complete 19-slot Windows virtual table 004F6A90..004F6ADB and six 16-byte section descriptors 004F6770..004F67CF were read from Game.exe. Descriptor field +0C is zero; no unproven semantic name was assigned. The shared static tooltip handler at 004B9D0C/004B9D23/004B9D24/004B9D28/004B9DA3 retains all 100 rectangle/text entries, the terminator after 100 copied bytes and reverse hit-test priority. Selection rectangle/centered-string helpers 004CBF94/004CBFD9, icon drawing 004E25EC and bitmap color-area filling 0040DC23 have real Windows bodies. Mac headers were consulted only for corresponding names.

## Explicit remaining boundaries

This is the basic information panel, not completion of CSelectionSingleHumanWindow. Its constructor, destructor, draw, message and layout routines remain partial and excluded from reconstruction completion counts. The following native branches are not connected:

- Military, production, experience, family and trader sections, including their enable/height calculations and dynamic controls.
- Remaining basic-section relationship, equipment, home and vehicle rows after 004AC96B and their dynamic buttons.
- Rename popup (name command 10006), follow chooser/toggle and contextual action popup. The native name control is present, but its editing command remains an explicit no-op boundary.
- Multiple-human and all non-human selection-window constructors in manager update 004A0E9D.

These require reconstruction from Windows entry points rather than invented controls or Mac-derived behavior. The current basic layout uses the native default 136-pixel content height until the remaining rows and native 004AED3F height-overwrite path are connected.

## Startup briefing audit

The immediate blocker was callback propagation. CCallbackManager::ExecuteCallback (00411E1D) stops on false. Multiplayer-goal callback 004195A4 is registered at priority 50, before the priority-1 mission callback, but its link dummy returned false on every tick. AI, assistant and happiness dummies also returned false, potentially stopping later callbacks depending on registration order. The four returns now match the unconditional Windows true exits at 00408FA3/00418D76/004195D0/00419AE8. They remain partial in TemporaryLinkStubs.cpp: AI tick/player-death dispatch, assistant updates every 12 ticks, multiplayer updates every 120 ticks and happiness calculation every 720 ticks/event handling are still unresolved and excluded from completion counts. This correction restores propagation without claiming those subsystem side effects.

No new unconditional startup popup was added. The existing Windows chain is retained: mission script loading -> callback type 1 every 36 ticks (004230EE) -> active goal evaluation -> result 9/PlayCutscene (0041FCFE) -> GUI callback 0x1B -> MiscWindows_MissionInfoWindow_Open (004A20F8). The result retains the script ID, stops the outer mission scan and lets later results in the same mission execute. The window pauses simulation and resolves the current map GUID, language directory and `%4.4d.hlt` file.

Read-only inspection of the installed map.ini/map.cif files found 59 first-mission automatic briefings: 57 True predicates and two TimeGone predicates (1 and 5 seconds). Both predicate types already have Windows implementations. Campaign 03/01, tutorial 003 and multiplayer 201 special coop use PlayCutscene 0 1. Their native triggers must be retained; a map with no scripted briefing must not be given a guessed briefing ID. This audit establishes source/data correspondence, not a successful gameplay run or complete mission evaluation. Later scripted briefings may use unresolved goal/result cases.

## Validation

MSVC Win32 `/Zs` passed for the five affected translation units. Existing C4805 warnings in unrelated manager option handlers remain. Project/filter parity and whitespace were checked. No Debug/Release build, tests or gameplay run was performed. Existing untracked C2HumanCombatTasks.cpp/.h were excluded.
