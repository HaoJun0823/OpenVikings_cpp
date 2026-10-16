# Project state

Reviewed October 16, 2026 against source baseline `e385046` and the subsequent source-directory reorganization.

OpenVikings reconstructs the Windows version of Cultures: 8th Wonder of the World. The main target is **Windows x86, C++17, MSVC v145**. Original Windows assembly and Game.exe determine behavior; Mac references supply names only.

| Inventory | State |
| --- | --- |
| Known native entries | **5,063 / 5,063 implemented**, completion checkpoint October 12 |
| Temporary link stubs | **0**, [ledger](TEMPORARY_LINK_STUBS.csv) is header-only |
| Registered production files | **307 CPP + 288 headers = 595**, all in [`src/`](../src/) |
| Function naming map | **3,151 distinct Windows address rows** |
| Fields identified in recent member-analysis passes | **95**, [evidence](RECOVERED_MEMBER_FIELDS.csv) |
| Explicit unknown member declarations/regions | **134 in 56 files**, [triage](UNKNOWN_MEMBER_TRIAGE.csv) |
| Numeric-access scan | **122 operands: 12 unresolved, 110 established operations**, [inventory](REMAINING_NUMERIC_ACCESS_INVENTORY.csv) |

The entry total is a source-reconstruction milestone, not a whole-program equivalence proof. The naming inventories measure separate scopes and must not be added together. Small unknown regions are not automatically padding; unknown fields included in savegames/checksums cannot be removed merely because a direct gameplay reader is unproved.

The latest member-naming pass recorded twelve implementation-unit Win32 syntax checks plus a private assertion unit, layout assertions and selected native byte comparisons. It did not build or run the game. Earlier build successes apply to their revisions; no current build/link or gameplay pass rate is established.

Current work is naming/layout analysis and runtime regression validation. [Open tasks](OPEN_TASKS.md) separates those from future modernization. The [gameplay review](GAMEPLAY_RENDERING_BUG_REVIEW.md) distinguishes source corrections from user-confirmed fixes.

The working tree contains a user edit in [C2HumanJobAccess.cpp](../src/C2HumanJobAccess.cpp) setting scout exploration radius to **500**; the native/committed value is **20**. It remains excluded from commits. Other authorized compatibility changes are summarized in [controls](CONTROLS.md), [assets](BUILD_AND_ASSETS.md) and the gameplay review.

Start with the [documentation index](README.md); detailed reconstruction history belongs in Git, not repeated progress snapshots.
