# Windows human activity and freeze recovery - 2026-10-07

The user reports stationary humans specifically among women, children and scouts. These are runtime observations supplied by the user; the changes below have not been run in a newly built executable. Windows x86 ASM and the original Game.exe establish all behavior, values, layouts and control flow. Mac headers/source are used exclusively to recover corresponding names.

## Connected activity blocks

- **Women and household stock:** 0046C7BE and 0047553C..00475720 select household work, collect demanded goods and bring cargo home or find an alternative consumer/pile. 0045FBF6 preserves the native food/enhancer preference groups, map/stock/guide search order and failure message code. Empty stock requests record demand. Current/future reservations are released in all five original circulation modes.
- **Children and civilian goodies:** animal search/play, sleep, walk-around, talking, paired waits and passenger exits now have their native helpers. The civilian movement tick consumes steps and dispatches arrival/failure/stopped callbacks. Child/baby idle selection keeps the original job/age rules and random calls.
- **Scouts and equipment:** worn shoes/tools now queue the original replacement commands rather than holding job startup. 0046BA57..0046C39D contains equipment startup, shoes/tools/items/armor/weapons, fetch/drop, finish and failure leaves. 0042D507 preserves the distinction between unused recoverable equipment and worn equipment discarded without cargo. Tool/shoe conditions use the three Game.exe maxima of 10000. Inventory insertion/removal and slot selection preserve signed condition comparisons and original replacement order.
- **Civilian commands:** opcodes 4..12, 29, 41 and 52..87 dispatch through the translated branches. Goto-and-wait retains target reset, continent checks, fallback point selection and work-center update rules. Cheer uses the command's speaker id/revision fields at +4/+8, starts native listening and does not change a cargo-bearing listener's task. Go-home retains the zero-filled follow-up command and the native failure requeue. Unsupported command trees still retain their slots.
- **Goods and needs:** food search/collection, map/house/vehicle pickup and pileup, consumer/pile selection, guide goods searches, carried-cargo handling, secondary-target restore, temple/prayer search and task-7 finish/failure are connected. Walking preserves passenger exit and house leave order. Stock/future amounts and cargo callbacks are updated in native order.
- **House production:** ordinary indoor goods producers dispatch through complete fill/flush stock, input collection, production startup and output event leaves. Decimal output, tool wear, experience, production counters and overlays retain Windows ordering. Construction stock/future needs and builder work-point selection are available to the shared goods block.
- **Map lifecycle:** native landscape clear/set/transition, engine area blocking, bio-regrow, automatic transitions and bio-system rectangle/tick updates support consumption, placement and regrowth. GameTime uses the typed real tick methods; their previous link-only dummies are removed.
- **House attachment cleanup:** 004320CA, 004418EC, 00441925, 00442028, 0043CC88 and 0043CE31 now implement house/human mask cleanup and work/learning detachment. The old detach dummies are removed. Initial house attachment remains a separate unresolved dependency.

## Remaining functional boundaries

This is **not a completed reconstruction of every human workflow**. The following still require Windows reconstruction and remain excluded from completion:

1. Family/marriage/childbirth commands, paired marriage completion, birth and transformation events. Their startup/failure dependencies remain open even though some native finish table entries share the idle leaf.
2. Learning-house training (00473B06..00473EC3), initial house assignment/attachment, and the remaining default-action command branches.
3. Outdoor jobs, building/harvesting/cultivation, animal breeding (004692E9), commander/passenger vehicle work, and other creature AI.
4. Opponent target handling at the start of idle, attack/defense tasks, combat events and military new-position/enemy-search continuations. The existing civilian opponent-target boundary remains; no speculative target clearing or forced idle fallback is introduced.
5. Sector/guide route fallback: 00447CF1 still lacks 00450222 and the 0041B515/0041B6C7 routing overloads. A local search failure is not evidence that the original game has no route.
6. The AI NoWayFound notification and external death/cadaver/message dependencies still backed by link-only dummies.

Whole atomic animations containing untranslated events retain the reconstruction boundary before their clock advances. Task and command boundaries are inventoried; no timeout, teleport, invented success or substitute animation masks these gaps. Therefore continued freezes in those states are possible. Distinguish user-confirmed runtime improvements from source-level recovery.

## Verification and inventory

All 21 changed/new C++ translation units passed MSVC **Win32 `/Zs` syntax-only checking**. No Debug/Release build, test project, test execution or new game launch was performed. Syntax checking does not establish linking or gameplay equivalence. Project/filter manifests agree on **186 cpp / 177 headers**, without duplicate entries or missing files; no inline assembly remains in cpp/h files. `git diff --check` is clean.

`FUNCTION_NAME_MAP.csv` contains **670 rows**. New rows include previously unindexed named bodies in the touched files; this is a naming inventory, not a new completion/coverage measurement. `TEMPORARY_LINK_STUBS.csv` contains **121 unfinished entries** after removing 21 obsolete dummy/partial rows and recording newly exposed partial dispatch boundaries. The historical 2584/5054 address count remains historical and is not recomputed by this block.

Resume with the remaining native opponent/family/training trees and sector/guide routing, consulting current source and Git history before using older documentation. Never infer their behavior from Mac code.
