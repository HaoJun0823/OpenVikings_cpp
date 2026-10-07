# Human selection, world input, messages and voices

## Connected Windows paths - 2026-10-07

Behavior, offsets, constants, branch order and resource strings were established from `assembly_weltwunder_win.asm` and the original Windows `Game.exe`. The Mac header/source supplied corresponding names only. No Mac behavior was used.

The standard `gui_main_mode=1` path now connects left click, selection dragging, Shift addition, Control removal, double-click selection by job and right-click movement. The selection rectangle intersects the current animation sprites, including their offsets, rather than only a map tile or a fixed box. Limits remain 300 humans and 30 compatible type-five vehicles. Invalid generations, ownership changes and destruction events remove stale selections; single-member groups become single selections. Selected graphics now receive the original engine callback bits. The alternative mouse mode remains partial because its action-window toggles are missing.

`C2EngineSelectionAccess.cpp` implements the Windows search at 00484061: landscape/house/guide/animal/vehicle/human sprites, inhabitant signs and work-center markers, original hit priorities, the strict 20-pixel fallback, fog filtering and road/wall ownership. House signs, overlays and guide direction sprites retain their distinct native data paths. 0048B532 is the inhabitant-sign lookup; its previous soldier-sign name was incorrect. The separate soldier lookup is 0048B574. The sprite-area accessor already exists at 00439A8E in XFontMetrics.cpp and is reused, without a second implementation.

`InGameGuiSelectionAccess.cpp` connects 004B4A9F/004B5EB1, selection singles/groups at 004B6115..004B6852, reset/removal/validation at 004B6BB0..004B7478 and the normal cursor/default-action path at 004B767E/004B5CFF. Input mode zero and normal logic callbacks are connected. Human movement uses the existing original command queue and formation-target search. 0043623A supplies the visible single-human land command after the original inhibition, placement, continent and visibility checks. The existing group movement helper remains responsible for formation destinations. No substitute command queue or guessed generic interaction is introduced.

World frame updates now run hit testing, prepare the normal single-human land command and mark the hovered person's family, home and workplace. Hovering an owned house marks its attached humans. Secondary records at display +AC/+B0 use the real 0047CFEA/0047D06D helpers and generation values. Middle-button movement, wheel zoom and message camera commands connect to the existing display functions.

`InGameGuiMessageWindowAccess.cpp` replaces the presentation-window dummy at 004B4247 with the original reusable window, ownership, destruction and camera/object-selection path. A top-row message opens its text and human portrait; confirmation dismisses it. Control suppresses the camera jump as in Windows. Construction uses the complete 19-slot Windows virtual-table layout, base window ownership and the original allocator. Missing or removed messages queue window closure.

`InGameGuiMessageText.cpp` connects all 63 known message types at 004D21DE, including unlock categories, dead-human lookup, diplomacy and special items. Text IDs use the dedicated message string array (group 6, 004E1E5F), not the main GUI array. Diplomacy literals, format strings and the whole message jump table were verified in Game.exe. The separate 004E2218/004E22D8 helpers and player-tribe name lookup at 0040A231 are connected.

Human generic voices at 0048575F run at the end of foreground field drawing (00491BFF), preserving the one-tick gate, visible counters, one shared random chance and tribe/age/sex selection. Animation sound event 0x17 now reaches the original oriented sound playback branch in 0047EC5B, including fog, distance, volume, pan, variants and overlap suppression. Command acknowledgment uses the existing 00485A4E sound path. Other branches of 0047EC5B remain unfinished.

## Explicit remaining boundaries

- Human/vehicle/house selection windows, action-button creation/update and toggling remain link-only. Selecting a person updates the actual selection and graphics, but does not establish a complete information/action panel.
- Positive input modes, command eligibility/highlighting, modal GUI visibility and their cursor modes remain unfinished. Road/wall selection and construction/landscape validation also remain unfinished.
- Contextual default actions for buildings, other humans, animals, vehicles, roads, walls and landscapes, plus scout/work-center modifiers, remain unfinished. The implemented ordinary land order does not imply those interactions are available.
- Prepared vehicle command dispatch and the school-window branch of prepared human command dispatch remain unfinished. Automatic follow, keyboard and border scrolling remain unfinished.
- Presentation-window help commands 10200/10201 and house context buttons remain unfinished. The local graphics-button path is partial; the complete shared dynamic-subbutton handler is not counted reconstructed.
- The original game cursor drawing callback remains link-only.
- Guide-angle calculation uses C++ floating-point math for native 00495843/0049563F. Win32 double evaluation can differ from x87 extended intermediates at angular boundaries; this is recorded rather than introducing inline assembly. The Windows stored-float boundary between these distinct routines is retained.

These boundaries remain in TEMPORARY_LINK_STUBS.csv and are excluded from reconstruction-completion claims. Naming inventory size is not functional coverage. Existing untracked C2HumanCombatTasks.cpp/.h belong to preceding combat work and are excluded from this block.

## Verification

MSVC Win32 `/Zs` syntax checks cover the affected translation units. Project/filter source and header inventories match. No Debug/Release build, test project or gameplay run was performed. The actual game must still confirm click/drag selection, Shift/Control changes, group movement near obstructions, message focus/dismissal and audible conversation events. Syntax checking does not establish runtime equivalence or resolve all existing link-only dependencies.

Next: reconstruct the selection/action windows and the Windows contextual default-action tree, then positive input modes and remaining camera controls.
