# Reverse-engineering guide

The mandatory rules are in [AGENTS.md](../AGENTS.md). Current counts live only in [project state](PROJECT_STATE.md); pending work lives in [open tasks](OPEN_TASKS.md).

## Source authority

1. Windows x86 assembly and the original Windows Game.exe establish control flow, constants, offsets, widths, calling conventions, callbacks, ownership and failure behavior.
2. Current C++ and Git history establish the implementation state. Inspect callers and consumers before changing a reconstruction.
3. Mac declarations/code supply names only after correspondence is established from Windows evidence. Never infer a Windows layout or behavior from a Mac layout.

The assembly-reference Game.exe has eight modified asset-path bytes compared with the installed original used in the path review. Those modifications are not original Windows evidence; see [build and assets](BUILD_AND_ASSETS.md).

## Evidence and naming

Distinguish Windows-confirmed behavior, matching Mac names, call-site/layout inference and unresolved identity. Prefer a supported original name; otherwise use a conservative Windows-derived description. Keep unknown fixed-width storage explicit. Initialization, size and alignment alone do not prove a gameplay role, padding or dead storage.

Review each whole region: readers/writers, indexed aliases, initialization, lifetime, serialization and checksums. Negative bounded searches do not prove that no reader exists. The [unknown member review](UNKNOWN_MEMBER_REVIEW.md) and [residual access analysis](RESIDUAL_ACCESS_EVIDENCE_LIMITS.md) record these limits.

Native code folding can share one address between different receiver meanings. Getters at `004230EA` (+0x10) and `00471F57` (+8) are now represented by receiver-specific reads, not one invented universal getter name. The [name map](FUNCTION_NAME_MAP.csv) marks folded reads and internal continuations separately from standalone functions.

Keep useful native address comments in CPP, preserve literal widths and signedness, and use the existing `NC2Layout`, `SwitchIds` and runtime helpers. Do not merge distinct original functions or change allocation families. Byte storage for embedded native objects/lists may be necessary to preserve placement construction and destruction order.

## Verification

Compare complete relevant branches, callbacks and error paths with Windows, not merely a matching prologue or successful compile. Syntax checks and static assertions establish compilation/layout properties; selected byte matches establish evidence provenance. Neither proves gameplay equivalence. No Debug/Release build or new tests are run without the authorization required by AGENTS.md.

Confirmed original defects, uncertain observations and ordinary failure handling remain separate in the [original-behavior register](RECONSTRUCTION_NOTES.md). Intentional compatibility changes must be labeled; reconstruction errors are corrected against Windows.
