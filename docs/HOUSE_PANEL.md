# Building panel, upgrades and stock controls - 2026-10-10

## Windows comparison

InGameGuiHouseWindow.cpp was compared with 004A6F10..004A950B, together with the good-row element at 004A950C..004A9794 and the inhabitant element at 004CC045..004CC873. The complete 004F61F8..004F6467 descriptor/page region was checked in the reference Windows Game.exe; the eleven section descriptors match. Windows assembly/Game.exe establish behavior. The Mac header is used for matching names only.

The existing window already connects owner/opponent/trade visibility, persistent collapse states, bottom-right layout, native child order, health, construction progress/materials, quality-enhancer controls, family counts, stock categories, input/output quantities, minimum stock, inhabitants, diplomacy-dependent trade agreements, camera and help actions. Frame command 2030 refreshes construction/defense sections and stock ordering; rebuilding deletes old children and resets the dynamic-button handler before creating replacements. The Windows refresh rules are retained, including their limited stock-list comparison; no additional refresh behavior was invented.

The home section at 004A7E17..004A8134 contained one confirmed layout mismatch: C++ advanced by 16 pixels after each of three quality-enhancer rows and then added 20 pixels before the family count. Windows advances only between those rows, then adds 20 pixels after the third. SelectionHouseWindow_Draw now places the family count at section.y + 74 instead of +90 and derives the section body height as 78 instead of 94. This removes the extra empty line and aligns the subsequent sections/children with the native layout.

The stock-order comparison at 004A9490 calls 004E2BF0, StringToolMisc_StringSortCompare. The existing shared lsSortGoodsCompareFunction calls that same original helper, including its special-character handling; it was verified and kept. It is not replaced by a plain byte-string comparison. Initial stock page filling and most-stock zeroing also follow the original; no extra page clears were added.

## Existing command tree verified

| Window command | Native action |
| --- | --- |
| 10001 / 10002 | Upgrade start / cancel, house packets 0x5B / 0x5C |
| 10003 | Open removal confirmation window |
| 10004 / 10023 | Center map; preview click also ends follow mode |
| 10005 / 10006 | Main-worker list / building and stage help |
| 10007 / 10008 | Select inhabitant / select and show action buttons |
| 10009 / 10010 | Production counter change, packet 0x5F |
| 10011 / 10012 | Stop/start production flag, packet 0x5E |
| 10013 / 10014 | Stock category / selected good, with child rebuild |
| 10015 / 10016 | Decrease/increase selected minimum, packet 0x60; native modifier changes step from 1 to 10 |
| 10017..10022 | Toggle the three home quality enhancers, packet 0x5D |
| 10024 | Show selection action buttons |
| 10025 / 10026 | Enable/disable defense, packet 0x61 |
| 10027 | Release at most ten attached soldiers, human packet 0x1D and responding sound |

House command insertion supplies the revision, and execution validates allocation/revision before invoking the actual house routines. Upgrade start 0043290F initializes the next stage, refreshes workers and disables defense; cancel 0043293F returns unused delivered material inside one map-modification bracket, restores state 3 and refreshes workers. These routines already match the Windows branches and required no change.

Windows House_UserControl_GetCommands at 00433549..0043361B does not add production-counter/stop commands, and 004A8644 does not create controls for window messages 10009..10012. The presence of message handlers alone does not justify adding such controls. Production input/output rows and minimum-stock controls are present; Mac OldStyle helper names do not establish additional Windows UI behavior.

## Remaining rendering dependency and validation

The WorldDisplayElement preview is connected to the display engine, but the house/construction rendering branches of Display_l_DE_DrawLandscapeAndCreaturesField (004911D5) remain unfinished and are still excluded from completion coverage. The relevant foreground calls are house drawing at 00495B57 and construction-site drawing at 004956B7. Reconstruct that whole drawing tree next, including its native selection outlines, construction layers, animated overlays, door images, inhabitant signs and damage/demolition effects. Existing partial landscape/moveable rendering must not be described as a finished building preview. The unused final flag of image helper 00496594 is also unused by Windows; it is not itself a missing rendering branch.

Seven affected/integration translation units passed MSVC Win32 /Zs. Project/filter parity, source existence, naming inventory and git diff --check passed. No build, tests or game run were performed; interactive upgrades, stock controls and layout still require confirmation in the user's next run. No new original function was reconstructed in this checkpoint: coverage remains 4038 / 5062 (79.77%), 1024 known entries open, 266 cpp / 246 headers, 2191 naming rows and 62 tracked unfinished boundaries. Coverage measures source-address mapping, not verified gameplay completion. User-owned untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded.
