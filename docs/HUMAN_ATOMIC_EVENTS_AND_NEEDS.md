# Human atomic events and needs - 2026-10-07

## Stock-event continuation - 2026-10-10

Event 27 is now reconstructed from Windows 004688EE..0046895C: work-house attachment, signed output / 10 and % 10, stock modification and native task success/failure state. It is admitted by the atomic preflight. It is not a particle event. Other building-event contexts retain their explicit reconstruction guard; the enclosing event dispatcher stays excluded. Particle callbacks and simulation are connected separately; see [PARTICLE_SIMULATION.md](PARTICLE_SIMULATION.md). No build or game run occurred.

## Collector continuation - 2026-10-09

Collector job 8 and its map collection/search/event tree are now connected. Generic event types 13/15/16/18 are reconstructed and admitted by the atomic guard. The shared movement/task/urgent-needs machinery is unchanged; unsupported other jobs remain explicit boundaries. User reports the previous soldier correction works well. Historical pending descriptions below may be superseded; consult HEAD and TEMPORARY_LINK_STUBS.csv. See [HUMAN_MAP_COLLECTION.md](HUMAN_MAP_COLLECTION.md). No gameplay run or Debug/Release build occurred for this correction.

## Evidence and naming boundary

The user explicitly clarified that Mac is a naming reference only. All behavior in this block comes from the Windows x86 disassembly and the original Windows `Game.exe`; Mac header declarations supply names only. No Mac function body establishes a value, offset, layout, condition, call order or side effect. `AGENTS.md` now records this restriction. There is no native build, test project, executable launch or new visual/runtime confirmation.

`81fb33d` committed the preceding goodie/inline-assembly-removal checkpoint. The six former inline assembly blocks remain replaced by C++/platform operations; their floating-point limitations remain documented in PROJECT_STATE. The following is the subsequent source reconstruction block, not a new claim that all civilian activities work.

## Connected Windows tree

`MapMoveableArray_DoOneGameTick` now calls the human condition update before turning/walking/AI, matching the human tick at 00439079. The array checks pending deletion after that tick, including early turning/walking exits, and invokes the existing `CHumanArray::FreeHuman` tree for humans. Animal/vehicle AI and deletion, active new path steps and their result callbacks remain pending. Existing home-detach/death-cleanup dummies still prevent complete condition/tick coverage.

`Human_CurrentTask_DoPerform` (0044314A) dispatches matching animation events in their stored order before incrementing atomic time. It rereads the current time for each event, retains the original animation record across callbacks and finishes only on exact length equality. Interrupt/reset inside an event does not invent an early return from this loop. A reconstruction preflight holds an entire animation if any event in its set is untranslated, avoiding partial execution followed by skipped production/combat effects. This preflight is an explicit implementation boundary, not original game behavior.

The complete selector is recovered from PE table 00468AD8..00468B67, including text-export alignment errors. The connected event types are:

| Event types | Windows effects |
| --- | --- |
| 1, 2, 3, 4, 5 | Energy, food, social, religion and hitpoints with the original age/ignore-needs and invulnerability conditions |
| 6, 7 | Original Windows diagnostic and circulation stop; the original game leaves these effects unimplemented |
| 8, 9, 10 | Send social/religion/hitpoints to the target human; preserve unsafe pointer/revision checks and message/interrupt order |
| 14, 17 | Original merge/shrink warning boxes |
| 22, 23 | Existing native house leave/enter operations |
| 26 and values outside 1..36 | Original no-op |
| 34, 35, 36 | Sound/animation/house-effect callbacks 0x17, 0x22 and 0x25 |

Types 11..13, 15..16, 18..21, 24..25 and 27..33 remain untranslated production, collection, building, birth, combat, stock, experience, training, economy and chest branches. These are not discarded or counted complete.

Windows energy events consume the home enhancer and double the event amount on success. Outside home, eligible jobs halve the signed amount with truncation toward zero. Windows food events double the amount in the home. The three home enhancer counters at house offsets 784/78C/794 use increments 100/100/1000, upper caps 500/500/5000, decrements 5/5/3 and rollback on a negative post-decrement value. Arithmetic retains DWORD wrapping and signed comparisons. The send-religion event checks the receiver against 8000 before addition; it does not clamp the addition.

`Human_Target_IsValid` (0042CED7) includes the Windows passenger/own-vehicle exception in addition to general target validation. `Human_IsAtTargetPoint` (0042CC40) uses the complete eight-entry PE table 0042CD3D..0042CD5C, including vehicle door/land checks. `Vehicle_Door_IsEntryPointOnLand` (004413B3) keeps the second door query and ignores its return value as Windows does. Home/work interior checks 0042CD5D/0042CD98 and sound callback 00468B68 were checked against PE bytes where the text export is misaligned.

## Conditions, urgent requests and inventory items

`Human_AI_DoUpdateConditions` (00463678) preserves Windows food/energy/social decreases, hitpoint recovery/starvation, upper limits, waiting/age updates, baby/child-to-adult transitions, parent/house changes, healing item attempts, death message/pending deletion/callback order and the twelve-tick warning cadence. Windows does not decrement the religion attribute in this routine. Ignoring-needs humans retain the original separate recovery branch. Existing `House_Inhabitants_DetachHuman` and death-cleanup dependencies remain unfinished and are explicitly tracked.

The entire contiguous Windows constant region 004F3B38..004F3B77 was read from PE: four maxima 10000, four refill values 8000, four urgent thresholds 2000 and four warnings 1000. Need attributes cap at 1.5 times their maxima; hitpoints use 1.5 times the original job/tribe maximum. No layout or value comes from Mac.

`Human_AI_DoCheckForUrgentNeeds` (00463509) now selects food before stamina before social before religion, preserving its interruptibility/user-command/once-per-tick guards, defense-house restrictions, already-fulfilling checks and refreshing-in-world gate. Existing suitable items are used before requesting task 7. Item useability ranks amulets/big/small potions 3/2/1 and adds 3 for an already used item; strict unsigned comparison retains the first tied slot.

PE item table 0045D084..0045D0A7 establishes the whole selector:

| Good | Attribute effect | Quantity effect |
| --- | --- | --- |
| 43 | Food +5000; energy +5000 | -50 |
| 44 / 45 | Food +10000 | -50 / -20 |
| 46 / 47 | Energy +10000 | -50 / -20 |
| 48 / 49 | Hitpoints +2000 | -50 / -20 |
| 50 / 51 | Food / energy +4000 | Unchanged |

Unknown goods retain the original no-op selector behavior followed by empty-item cleanup. Quantity/attribute arithmetic wraps; no new slot bounds guard is introduced. Need 1 accepts goods 43/44/45/50, need 2 accepts 43/46/47/51 and need 5 accepts 48/49. Amulet classification covers 50..55 independently of whether the item is usable for that need.

## Supported task-7 startup and finish

The native startup dispatcher clears waiting and selects the Windows food/stamina/social/religion routines for need 1/2/3/4.

- Food 0046F881 consumes already carried food, retains circulation/secondary-target handling, clears cargo before callback 0x21 and starts action 10 or candy action 11 according to the Windows job byte and goods 17/20. Non-refreshing and invalid-circulation branches are connected. Cargo storage, food collection and food search remain pending.
- Stamina 0046FC36 starts action 8 at an arrived target or inside sleeping vehicles of kind 3/4, otherwise selects the original home/map sleep route. Home helper 0046FDAD and map helper 0046FE29 are connected through the existing sleep search/walk start. Windows CanSleepAtTargetMapPosition performs a nullable map lookup and then returns true; it is not a terrain filter. Cargo storage and other passenger exits remain pending.
- Social 0046FE7F preserves close talk/listen/wait pairing, farther following, failed-walk fallback and passenger monologue. The separate original monologue function 0046FF51 remains separate. Cargo storage remains pending.
- Religion 0046FF69 preserves passenger failure and home-versus-temple selection. Reached home/valid prayer positions start action 12. Windows prayer eligibility checks micro-point kind/flags, house state/deletion, home ownership exception, house main type and the prayer counter. House next-prayer-point and temple search remain pending, with no guessed positions or successful fallback.

Finish 0047014C repeats a still urgent need, removes the corresponding user message otherwise, clears need/circulation state and returns to task 8. Home helper 0046F797 retains stamina/food/religion/spouse-social refill priority. A terminal finish with a valid secondary target is explicitly held before side effects because its restore subtree (0042D019, including walk-and-fetch) is still pending. This conservative preflight is a reconstruction boundary; it is not an original condition. Task failure dispatch/0047024A remain subsequent work.

Message removal 004A30A3 and `CMessageManager::Message_RemoveHumanMessage` 004B4144 preserve GUI presence, system update and deletion of only the first matching type/human message. Existing Message_Remove retains its native callback/compaction sequence.

## Inventory and verification

Two production cpp/header pairs are registered in the Visual Studio project and Logic filters. Project/filter parity is 181 cpp / 173 headers, with no duplicate or missing entries. FUNCTION_NAME_MAP has 364 rows, including 46 new names taken from explicit Mac header declarations with Windows-only behavior evidence. TEMPORARY_LINK_STUBS has 135 unfinished entries: the former urgent-needs staging entry is removed; nine new partial originals remain excluded. No new link dummy was added. Historical 2584/5054 coverage remains a historical checkpoint and was not recomputed.

Static verification checks project/filter XML parity, source presence, CSV uniqueness/names, absence of inline assembly and `git diff --check`. This does not establish compilation or runtime behavior.

Next: close food storage/collection/search, prayer-point/temple search, secondary-target restoration and task failure, then production events and active path-step/result callbacks. Child animal-play and general passenger/cargo goodie branches remain independent unfinished dependencies. Complete eating/sleeping/working/talking/praying and autonomous movement are not claimed.
