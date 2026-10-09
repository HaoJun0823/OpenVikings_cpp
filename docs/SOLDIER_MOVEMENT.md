# Soldier movement and job continuation

## Source-confirmed gap and correction - 2026-10-09

The user observes soldiers walking on the spot even without enemies. HEAD previously returned from Human_l_StartTask_ExecuteJob for every military job (31..47). A completed move/GotoAndWait can select task 8 with status 0 while the walking atomic remains 1. The unsupported job then yielded without starting another atomic, leaving the renderer drawing walking at a fixed position. This is a confirmed source-level gap and a plausible explanation of the report; the reported game instance has not been instrumented or rerun.

The Windows PE selectors 0046C747..0046C78E and 0046C78F..0046C7BD send all jobs 31..47 to the same body, 0046F41A. C2HumanMilitaryJobs.cpp now reconstructs that complete body and the ground-job dispatcher calls it. With no valid military target and no return walk needed, it selects task 1/status 0. The existing native idle startup replaces atomic 1 with the appropriate finite idle animation. No generic animation reset, artificial timeout or guessed path restart is added.

The soldier body retains tower ownership/detachment, weapon-specific search radii, the three military modes, primary/secondary target priority, landscape-target replacement, work-house door recentering, distance 18 for unattached soldiers, return walking and the second attempt after Tool_Map_GetNotBlockedPosition. The native fallback lookup's return is deliberately ignored. The body also retains its vehicle branch, although the owning ExecuteJob dispatcher still has an earlier unfinished vehicle-dependent boundary.

## Complete related Windows helpers

- Human_l_CheckForEnemyTarget, 0046B384: target switching only to a closer distinct secondary target, continent/range validation and landscape targets stored in the primary slot. Helper 0046BA2D validates the two military slots; its original semantic name is unresolved.
- CGameMapSectorAttachmentManager::CheckForEnemyTarget_Human, 0044EDD5, and l_CheckSectorForEnemyTarget_Human, 0044EF00: native 12-tick cache gate and strictly greater than 60-tick landscape gate, two ordered human/vehicle/house lists, conditional animal selection and AI landscape targets. Per-sector auxiliary storage is 21 records of 12 bytes: DWORD scan tick at 0, DWORD landscape scan tick at 4, result byte at 8. The other three bytes remain unexplained and unchanged. The clipped final row/column are excluded, and the source sector's cache is updated even when a different sector triggered the scan.
- Landscape search 0043AAE7/0043ABA9/0043ACE1/0043AD26 and list constructor/insertion 0043AB64/0043AD8B: original allocation/free wrappers, point/distance entries, stable unsigned-distance insertion and native random candidate choice. The list's original type/template name remains unresolved. Windows 0043AC66..0043ACB8 traverses maximumDistance steps per side on every ring; this unusual loop is confirmed in Game.exe bytes and preserved. Threshold overflow is not normalized.

All behavior, offsets, constants, call order and loop bounds come from Windows assembly and Game.exe. The Mac header supplies only the eight established names entered in FUNCTION_NAME_MAP.csv. Internal list/predicate names describe verified Windows operations and do not claim recovered original names.

## Validation and remaining scope

The six new/affected calling cpp units pass MSVC Win32 `/Zs`: C2HumanMilitaryJobs, C2MilitaryLandscapeTargets, C2MilitarySectorTargets, C2HumanTaskAccess, C2MapSectorAttachment and C2MilitaryTargetSearch. Project/filter parity and the production-only source audit pass. No Debug/Release build, tests or gameplay run occurred.

Attack/pursue startup, finish/failure and attack atomic events remain unfinished independently. The military branches of the intermediate-point callback 00463043 also remain open. A selected attack target can still reach an unsupported task 14/15; this correction does not establish complete combat behavior. User-owned untracked C2HumanCombatTasks.cpp/.h remain untouched and unregistered. Review their Windows correspondence separately before deciding how to connect the full fight tree, arrows and delayed hits.

Source-address checkpoint: **3806 / 5059 (75.23%)**, **1253** known candidates open; **248 cpp / 228 headers**, **1989 naming rows / 64 unfinished boundaries**. The eleven new complete bodies add eleven mapped addresses; the owning partial dispatchers are still excluded. Coverage measures source reconstruction, not verified gameplay completion.
