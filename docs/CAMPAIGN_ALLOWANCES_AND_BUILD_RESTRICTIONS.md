# Campaign allowances, availability and building restrictions

Checkpoint: 2026-10-10. Windows assembly and the original Windows Game.exe establish behavior, offsets, parameter widths, branches and callback order. The Mac header supplies names only. The enclosing goal/result dispatchers remain partial and excluded from completion counts.

## Connected script branches

| Kind/ID | Windows branch | Native operation |
| --- | --- | --- |
| Result 29 AllowJob | 004203EB | Set a previously zero job allowance byte; update capabilities of allocated humans belonging to that player and tribe. |
| Result 30 AllowHouse | 00420457 | Set a previously zero house allowance byte; perform the same native human update scan. |
| Result 37 EnableJob | 004204C9 | Set a previously zero job availability byte. No human scan or extra producer-good enable is inserted. |
| Result 38 EnableHouse | 004204FB | For a previously zero house availability byte, call 00409A3B on the player's allowed-things region. Preserve its extra good-availability writes for houses 10, 11 and 17. |
| Result 39 DisableAll | 00420534 | Deactivate every mission group through 00449F2D. It does not disable units, jobs, production or every player allowance. The signed mission count is re-read for each iteration. |
| Result 42 AllowGood | 00420687 | Set a previously zero good allowance byte; update matching player/tribe humans. |
| Result 43 EnableGood | 004206F9 | Set a previously zero good availability byte, without granting its producer job. This distinct branch is implemented even though no occurrence exists in the 35 audited assets. |
| Result 91 SetHouseBuildForbiddenArea | 004224EE | Construct the two-WORD point without wrapping, interpret the radius as signed and normalize the fourth parameter to a Boolean; call the by-value Windows area helper. |
| Goal 38 JobEnabled | 0041EF4C | Test the exact job availability byte. |
| Goal 39 GoodProduceable | 0041EF72 | Test the exact good availability byte; the original script spelling is retained. |

The first three Allow branches retain player, tribe and item values before callbacks, skip the scan when their allowance byte is already nonzero, and scan in the original allocated-human ID order. Enable branches do not rerun allowance or capability initialization. No index/player/activity guards were added where the Windows branch has none. These actions grant permissions; no unobserved ForbidJob/ForbidGood script result was invented.

## Player flags and dependencies

The inspected player-relative region is owned by the existing GameControl allocation, with a 4A80h player stride and startup zero-initialization. Native indexing and adjacent boundaries establish these byte arrays; this table also records neighboring arrays rather than assigning a new type from one isolated offset.

| Array | Player-relative base | Tribe stride | Adjacent boundary |
| --- | --- | ---: | --- |
| Job allowed / available | 0018h / 0948h | 56 | 0948h / 1278h |
| Good allowed / available | 1278h / 1D4Ch | 66 | 1D4Ch / 2820h |
| House allowed / available | 2820h / 3126h | 55 | 3126h / 3A2Ch |
| Vehicle allowed / available | 3A2Ch / 3B52h | 7 | 3B52h / 3C78h |
| Equipment allowed | 3C78h | 56 | Next player diplomacy storage starts at 45A8h |

The paired extents correspond to 42 tribe slots, derived from the adjacent boundaries and confirmed by native initialization's tribe range. Existing initialization deliberately indexes 66 equipment flags with a 56-byte tribe stride; that difference is retained, not repaired from a presumed table shape. This block does not introduce a replacement aggregate layout or claim unresolved player fields are identified. Constructor allowedthings tokens, allowance initialization, the existing human job/production setup and the native house-availability leaf were inspected against current source/history and Windows callers.

## Human capability refresh

The complete **0044A07C Human_UpdatePlayerSpecificStuffAndCaps** is now reconstructed. Its only Windows callers are the three Allow result scans. It constructs the native 304h message, records human/player IDs and type 18, and retains the original player-record pointer through subsequent messages.

For a male human without +B84h bit 6, jobs 6..41 are checked in order: denied global allowances clear local +15Ch flags; an allowed, locally absent job is granted only when the existing 0042D798 experience predicate passes, and appended to the message's +138h list. There is no age, education or AI-player bypass added to this branch. The alternative branch copies the job-5 allowance to local +161h. The exact original name of behavior bit 6 remains unresolved; only its established branch effect is used.

Soldiers additionally gain allowed jobs 31..41 and then newly allowed equipment 37..42 followed by 33..36. Here Windows checks the **good allowance** array at +1278h with stride 66, not the neighboring equipment array. Each newly granted equipment flag emits **0045C1EA UserMessage_SendToUserMessage_CanEquipNewItem**. Heroes instead grant jobs 42..47 and equipment 33..36 without these equipment notifications. Previously granted flags are retained where the native path has no clearing branch.

The aggregate message is sent only if one of the four native lists is nonempty, followed by callback 18h, forward=true, current player, parameter 8, zero. SetupCurrentJobType (0044A45D) and UpdateCurrentJobType (0044AD2E) then always run in that order. Their existing complete requirement/message/production helpers remain the integration boundary. The three Windows equipment-message xrefs were checked; the previously inlined message inside UpdateCurrentJobType now calls the separate real 0045C1EA body. Message construction, field values, finish/send order and stack lifetime are preserved.

## Physical house-build restriction

The complete **0044DCAC Map_HouseBuildForbiddenFlagTool_SetArea** takes the packed center by value on Windows. It updates only micro-point byte +0Dh bit 20h, using the low bit of the supplied byte and preserving every other bit. The center is visited even for zero/negative radii. For each positive ring, the cursor first steps in direction 4, then visits ring-length segments in directions 0..5, advancing after each point. Out-of-map points are skipped individually; coordinates retain the existing WORD cursor arithmetic. Signed radius comparisons and DWORD wrap in the ring counter are preserved. No modification scope, object deletion, redraw callback or allocation is added to this native flag-only helper.

Existing 00440F70 house-build validation and 00453347 CMapArea::Check_NoHouseBuildForbiddenFlag read that same bit, including the house footprint. Their current source was inspected; the new script action reaches those checks through the shared map state. It can both apply and remove the original restriction.

## Audit and verification

Eight result branches and two goal branches connect **160 result occurrences / 17 goal occurrences** across 35 audited assets. In the 27 story assets, the new result occurrences are **137**: 124 DisableAll and 13 house-build areas; the other allowance/goal occurrences are in tutorial assets. Remaining story omissions are **21 goal types / 185 occurrences** and **45 result types / 541 occurrences**. Across all assets, omissions are **28 / 260** and **46 / 566** respectively. Static source-branch counts do not establish runtime campaign or tutorial completion.

Three native entries are newly covered: **4137 / 5062 (81.73%)**, 925 known entries open, 2275 naming rows, the same 56 unfinished boundaries and 272 cpp / 252 headers. Seven affected/integration units passed MSVC Win32 `/Zs`; project/filter parity, unique naming addresses and whitespace checks passed. No build, tests or game run occurred. The inventory parser's existing 21 unmatched signatures are unchanged; no new link dummy or unresolved body was introduced. User-owned untracked CombatTasks remain untouched and excluded.

Next: distinct queued vehicle boarding/detachment actions with available-command checks, then remaining object/resource/combat/wonder predicates and actions. Periodic PlayerDied production and broader AI/vehicle/weather/drawing remain open.
