# Project state

On branch `remaster`, the first authorized modernization replaces exclusive DirectDraw window presentation with a Unicode, DPI-aware Win32 window and D3D11/DXGI borderless presentation. Internal resolution changes retain the desktop mode. See [remaster window and presentation](REMASTER_WINDOW_PRESENTATION.md) for scope and validation limits. The reconstruction inventories below describe the pre-remaster baseline; they do not certify the new frontend's runtime behavior or native layout.

Reviewed October 16, 2026 against source baseline `e385046` and the subsequent source-directory reorganization.

OpenVikings reconstructs the Windows version of Cultures: 8th Wonder of the World. The main target is **Windows x86, C++17, MSVC v145**. Original Windows assembly and Game.exe determine behavior; Mac references supply names only.

| Inventory | State |
| --- | --- |
| Known native entries | **5,063 / 5,063 implemented**, completion checkpoint October 12 |
| Registered production files | **303 CPP + 285 headers = 588**, all in [`src/`](../src/) |
| Function naming map | **3,151 distinct Windows address rows** |
| Fields identified in recent member-analysis passes | **95**, [evidence](RECOVERED_MEMBER_FIELDS.csv) |
| Explicit unknown member declarations/regions | **134 in 56 files**, [triage](UNKNOWN_MEMBER_TRIAGE.csv) |
| Numeric-access scan | **122 operands: 12 unresolved, 110 established operations**, [inventory](REMAINING_NUMERIC_ACCESS_INVENTORY.csv) |

The entry total is a historical source-reconstruction milestone, not a whole-program equivalence proof or a count of native routines retained by the remaster. The unused DirectInput module and reconstructed CRT/compiler/x87 implementation units have been removed from the current production count; the remaster uses Windows Raw Input and a supported-runtime facade. Semantic naming/member inventories below remain historical reconstruction snapshots until separately refreshed. The naming inventories measure separate scopes and must not be added together. Small unknown regions are not automatically padding; unknown fields included in savegames/checksums cannot be removed merely because a direct gameplay reader is unproved.

The latest member-naming pass recorded twelve implementation-unit Win32 syntax checks plus a private assertion unit, layout assertions and selected native byte comparisons. It did not build or run the game. Earlier build successes apply to their revisions; no current build/link or gameplay pass rate is established.

Current work includes intentional remaster corrections and modernization through RM033 and runtime regression validation. The user has frozen `reversed-game-authentic-behaviour` as the legacy reference with its original defects. The historical OB/OA register remains unchanged; [the remaster ledger](REMASTER_BUGFIXES.md) records the community comparison, exact correction scope and retained contracts. All 303 implementation units passed Win32 syntax checks at the RM028 checkpoint; two pre-existing aggregate warnings remain. RM029's changed loader also passes warning-free `/W4` syntax checking and restores the original wooden-spearman sleep list's missing resting phase without changing sleep logic. RM030 restores the Windows action-label and tooltip IDs in the shared vehicle window; its syntax check passes with two existing unused-variable warnings. RM031 applies temporary grey material palettes during Viking mason building-work phases; the drawing unit passes warning-free `/W4` syntax checking. RM032/RM033 repair missing Viking pottery-fire and cake-visibility data plus the cake carry graphic; both loader units pass warning-free `/W4` syntax checking. The reconstructed CRT/compiler/x87 backends have been replaced by supported MSVC/UCRT services; see [runtime scope](RUNTIME_COMPATIBILITY.md). RM028 also removes the remaining MMX dispatch/names and uses SSE2 for packed sprite and Gouraud arithmetic. No current build/link or agent-run gameplay result is claimed. Networking is explicitly deferred to a separate later modernization project. [The player changelog](../CHANGELOG.md) now contains dated English articles, linked from the concise root README. [Open tasks](OPEN_TASKS.md) tracks remaining work, and the [gameplay review](GAMEPLAY_RENDERING_BUG_REVIEW.md) distinguishes reconstruction corrections from user-confirmed fixes.

The working tree contains a user edit in [C2HumanJobAccess.cpp](../src/C2HumanJobAccess.cpp) setting scout exploration radius to **500**; the native/committed value is **20**. It remains excluded from commits. Other authorized compatibility changes are summarized in [controls](CONTROLS.md), [assets](BUILD_AND_ASSETS.md) and the gameplay review.

Start with the [documentation index](README.md); detailed reconstruction history belongs in Git, not repeated progress snapshots.
