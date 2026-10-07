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
- **House attachment cleanup:** 004320CA, 004418EC, 00441925, 00442028, 0043CC88 and 0043CE31 now implement house/human mask cleanup and work/learning detachment. The old detach dummies are removed. The follow-up below replaces initial house attachment as well.

## Remaining functional boundaries

This is **not a completed reconstruction of every human workflow**. The following still require Windows reconstruction and remain excluded from completion:

1. Remaining default-action, work-house/learning-house assignment and job-change command branches. Family/marriage/childbirth commands, paired marriage completion and the birth event are connected in the follow-up below.
2. Work-house/learning-house command prerequisites (builder/vehicle detachment and queued job changes). Training itself (00473B06..00473EC3) and initial house attachment are connected.
3. Outdoor jobs, building/harvesting/cultivation, animal breeding (004692E9), commander/passenger vehicle work, and other creature AI.
4. Attack/defense tasks, combat events and military new-position/enemy-search continuations. Native opponent target validation/position/container redirection at the start of idle is connected; a valid nearby opponent transfers to the still-open task 14.
5. Sector/guide route fallback: 00447CF1 still lacks 00450222 and the 0041B515/0041B6C7 routing overloads. A local search failure is not evidence that the original game has no route.
6. The AI NoWayFound notification and external death/cadaver/message dependencies still backed by link-only dummies.

Whole atomic animations containing untranslated events retain the reconstruction boundary before their clock advances. Task and command boundaries are inventoried; no timeout, teleport, invented success or substitute animation masks these gaps. Therefore continued freezes in those states are possible. Distinguish user-confirmed runtime improvements from source-level recovery.

## Verification and inventory

All 21 changed/new C++ translation units passed MSVC **Win32 `/Zs` syntax-only checking**. No Debug/Release build, test project, test execution or new game launch was performed. Syntax checking does not establish linking or gameplay equivalence. Project/filter manifests agree on **186 cpp / 177 headers**, without duplicate entries or missing files; no inline assembly remains in cpp/h files. `git diff --check` is clean.

`FUNCTION_NAME_MAP.csv` contains **670 rows**. New rows include previously unindexed named bodies in the touched files; this is a naming inventory, not a new completion/coverage measurement. `TEMPORARY_LINK_STUBS.csv` contains **121 unfinished entries** after removing 21 obsolete dummy/partial rows and recording newly exposed partial dispatch boundaries. The historical 2584/5054 address count remains historical and is not recomputed by this block.

Resume with sector/guide routing, remaining default-action/assignment commands and combat/outdoor/vehicle trees, consulting current source and Git history before using older documentation. Never infer their behavior from Mac code.

## Family, training and opponent target follow-up - 2026-10-07

The household/goods/equipment block above is committed as `f5f12d4`. This follow-up closes further native civilian boundaries:

- **Family:** 0043C83B..0043CB67, 00443DEB, 00444BA8..00444E43, 00445469..00445A6D, 00445D23/00445D7B/00445D90 and 00469A16 connect marriage, love, family home choice, child commands/waits, birth and failure requeue. The birth event uses the real CHumanArray allocator. The warning string is read from Game.exe; 0045BFEC is an original no-op. Opcodes 13..16 and 26/27 now dispatch. Family home attachment rolls back previously attached relatives in the native order and preserves the distinct detach-failure return values.
- **House inhabitants:** 00431FFA, 00432316..0043264C, 00432690, 004326FD/00432720, 00432764/004328B7, 00433079 and 0043339A establish initial attachment, exact-mask checks, family grouping, worker/student/builder capacity, player/continent checks and builder eviction when a home needs a full slot array. Slots retain their training fields when reused. The Windows full-hitpoint predicate returns true at maximum, despite the recovered Mac method name IsDamaged.
- **Learning:** 00473B06/00473C9B/00473E4C connect approach, house entry, animations 89/90, overlays, education accumulation, unlock messages, job/production setup, learned-good selection, detachment and queued go-away. Event 28 gains working experience; event 29 selects training or exercise. Events 30/31/32 now execute their gold/wealth/counter leaves, including 0040A005's signed wrapped wealth check. Detach-learning opcode 33 is connected.
- **Opponent targets:** 0046184B..00461DD9 reconstructs the distinct 12-byte STargetData accessors. Hidden humans redirect to their house or carrier and transported vehicles redirect id/revision. Point targets contain microgrid coordinates, copied without macro conversion. Idle now performs the native validity/distance test using Game.exe's distance 50; invalid targets do not hold idle and distant valid targets clear their type. Nearby valid targets still require the separate combat startup.
- **Human sector iteration:** 0042C279/0042C376/0042C433 preserves sector head/list selection, WORD corner arithmetic, list order and distance filtering for the first marriage candidate. The random call is retained even for its single candidate.

All **13 touched/new C++ translation units** passed MSVC Win32 `/Zs` after corrections. No Debug/Release build or runtime confirmation. Manifests now agree on **187 cpp / 178 headers**. Naming inventory: **734 rows**, including previously unindexed existing named leaves; unfinished inventory: **118 rows**. Two additional link-only dummies (00431FFA/0043C8EA) are removed. This is still not a completed reconstruction of every human workflow.
