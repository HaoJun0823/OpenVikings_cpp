# Building helpers, overlays and mission operations - 2026-10-10

## Scope and evidence

The 55 logic entries in [BUILDING_LIST.csv](BUILDING_LIST.csv) represent the None sentinel and 54 building types. The shared house lifecycle, construction/repair, stock, production, home, school/barracks, temple, defense and rendering implementations already existed at HEAD. This checkpoint closes the remaining known separate house-helper entries, the house-area engine callback and building tooltip/debug passes, and the missing building goal/result branches. Behavior comes exclusively from the Windows assembly and reference Game.exe. Matching Mac header declarations supply names only; Windows-only outlined helpers explicitly record that no Mac naming correspondence was established.

All **116 known source-address candidates** in the house block 00430FCE..00434CCF now have registered bodies. This is an entry-candidate inventory, not a proof that every unnamed internal label is a separate function or that every building works in the game. General AI, vehicle simulation and other campaign cases remain distinct unfinished dependencies.

## Reconstructed original entry points

| Windows address | Implementation and Windows-established behavior |
| --- | --- |
| 00431CD3 / 00431DC6 | Root/door map-micro registration; only empty fields change, flags retain other bits and IDs truncate to WORD. House_Init now calls the distinct helpers. |
| 004322B6 / 00432740 | Worker attachment predicate and slot-to-human pointer access; homes exclude worker attachments and the native slot lookup is unchecked. |
| 0043264F / 00433A95 / 00433A99 | Distinct door reference, owner and map-position getters. |
| 0043322F | Holy-fire predicate: no demolition, finished state and signed positive fire counter; connected to the prayer search. |
| 00434259 / 004230B4 | Ten overlay bytes and DWORD behavior flags; invalid overlay indices are ignored, shift counts retain x86's five-bit mask. |
| 004345F2 | Native sector-list type predicate used by both attachment and detachment. |
| 0042B7AE / 0042B7E4 | Separate player/radius and player/radius/engine-type house iterators; original flags and all-house scan order. |
| 004171E7 | Mission information setter; validates one of five entries, clears and writes all ten DWORDs, and broadcasts only to allocated players in native order. |
| 00482DC0 | House-area map overlay callback: clear 30h, consume exactly one random value only when enabling, choose 10h/20h and recalculate macro data. |
| 004986C5 | Complete tooltip field traversal; native row/column/height arithmetic, exploration filter, root-house display and following map-moveable iteration. |
| 00498F2A / 0049938D | Original bounded tooltip formatting and allocated-house debug pass, including circles, slot/attachment counts and food/wood input/output stock. |

These are 18 new registered bodies. The already reconstructed defense-mode predicate 00433621 had a comment form that the inventory did not recognize; its traceability comment and naming row are corrected without changing behavior. The stale partial exclusion for production event 004691CE is removed after comparing its complete ordinary/animal branch with Windows. The animal implementation 004692E9 was already connected. Production now queries stock-full status even when inputs are unavailable, retaining the original call order. Human event 20's obsolete job/action preflight no longer blocks the complete existing native construction/repair/road/wall/scout dispatcher.

## Building mission branches

The enclosing evaluators 0041E45E/0041F9F7 remain partial and excluded from completion counts. Added goals are 3 BuildHouses, 5 GoodsInHouses, 10 FindHouses, 16/17 FindHousesByHumans/Vehicles, 23 BuildHouseOnContinent, 25 HumansWithHome, 43 HumanAttachedToWorkHouse and 55/56 NumberOfHouses/GoodsInHousesInArea. BuildHouses writes every matching house's mission ID even after reaching the required count. Native signed versus unsigned thresholds, empty-iterator behavior, exploration reads and nested object order are preserved.

Added results are 31 AddGoodsToHouses, 71 SetHouseExtensionLevel, 75 InfoCountHousesInArea, 81/87 RemoveHPsOfHousesInArea/X, 88 SetHouseOverlayState and 93 SetHouseBehaviourFlag. Extension changes snapshot at most ten house IDs, remove the old house and create a completed replacement with the original owner/type/position and requested mission ID. Damage snapshots at most 100 IDs before applying native negated damage; X excludes the specified mission ID. Stock changes retain the original input/output eligibility and ABI adapter. Parameter getter 00422FDB's misaligned assembly text was checked against PE bytes: it returns parameters[index].

The refreshed 35-asset script audit connects **73 formerly missing goal occurrences and 49 result occurrences**. Remaining story-only gaps are **19 goal types / 150 occurrences** and **40 result types / 499 occurrences**; all assets: **23 / 187** and **41 / 517**. These are static branch-presence counts. Wonder overlays, ordinary building objectives and extension changes are connected; resource, object/combat, weather and marker conditions/actions can still prevent complete campaign progression. No additional per-wonder routine is invented.

## Verification and remaining boundaries

Follow-up linker correction: the foreground caller still declared 004986C5 and 0049938D in the global namespace after their real bodies moved into NC2E2. Both declarations now reside in C2EngineDisplayAccess.h inside NC2E2, shared by the caller and implementation; calling conventions and parameter types stay unchanged. The two production units passed Win32 /Zs. No full build/link run was performed. Source-address coverage is unchanged.

Fifteen distinct affected/integration units passed MSVC Win32 /Zs syntax-only checking. Project/filter parity remains 272 cpp / 252 headers; naming addresses are unique, registered files exist and whitespace checks pass. No build, tests or game run occurred. The untracked user-owned C2HumanCombatTasks files are untouched and excluded.

Current whole-project source-address coverage is **4157 / 5062 (82.12%)**, **905** known entries open, **2291** naming rows and **52** explicit unfinished boundaries. Three temporary display/callback dummies are removed. The 21 existing unmatched declaration signatures are unchanged; no raw call lacks a mapped body.

Broader AI attack/group notifications (including 00473102), vehicle/sea simulation and nonbuilding mission cases remain open. The common human dispatcher 00468678 and full landscape/creature drawing entry 004911D5 remain conservatively excluded pending their broader audits. The particle creation/tick/rendering checkpoint already connected building arrow/smoke/hit callbacks; older building-list visual-dependency wording is superseded. Runtime confirmation of all building types is still pending.
