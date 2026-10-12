# Current shared/runtime remainder - 2026-10-12

Current production inventory: **653/670 shared/system entries (97.46%), 17 open**; globally **4697/5063 (92.77%), 366 open**. [SHARED_CURRENT_OPEN_ENTRIES.csv](SHARED_CURRENT_OPEN_ENTRIES.csv) contains the current 17 entries. [SHARED_32_ENTRY_CLOSURE_AUDIT.csv](SHARED_32_ENTRY_CLOSURE_AUDIT.csv) retains every entry from the user's requested 32-function block, including the 15 completed entries. **Finishing all 32 remains outstanding.**

The latest block connects native rounding, both parity helpers, pow-infinity cases, NaN handling, the two distinct argument loaders, half-pi replacement, three separate extended80 kernels and ordered hardware generation. The masked-response tree has no remaining dummy; its 004F2024 link-only implementation is removed. See [SHARED_SCALAR_MATH_BACKEND.md](SHARED_SCALAR_MATH_BACKEND.md) for evidence, interfaces, private-backend availability and runtime-validation limits.

The unary and binary error entries are subsequently implemented with the original record-write order, one hardware result store, project dispatcher, plain result load and conditional final control restoration. See [SHARED_X87_ERROR_ENTRY_COMPLETION.md](SHARED_X87_ERROR_ENTRY_COMPLETION.md). Their enclosing classification and completion parents remain open.

## Numeric dispatch and completion tree: 15 open

| Group | Open entries |
| --- | --- |
| Public/x87 entries | 004E6E44, 004E6E4D, 004E8300, 004E8774, 004E877D, 004E8824, 004E882D, 004E88D4, 004E88DD |
| Unary/binary classification dispatch | 004EB570, 004EB5D7 |
| Completion continuations | 004ED2B3, 004ED2BA |
| Exponential/arc-trig preparation | 004EB4BE, 004ED17C |

The common frame and descriptors need explicit interfaces preserving classification, precision, callback/error order and state restoration. Frame-dependent private helpers are not ordinary C++ no-argument callees. Current log10/sin/cos adapters stay partial and excluded; neither a modern CRT name nor the completed exception child tree establishes their completion.

The next tree is unary/binary dispatch through descriptor callbacks and completion continuations into the implemented error entries and public entries. Windows evidence is present locally; unresolved behavior stays explicit instead of being replaced with guessed host math. Mac remains naming-only.

## Compiler/unwind: 2 open

- **004E7D40 stack probe:** EAX size, page-touch order, caller-ESP and return-address relocation. The modern guarded probe is not a drop-in original-register binding.
- **004EC67E local unwind:** temporary FS registration, equality-based scope stop, pre-callback try-level updates, original frame/notification, collision behavior and funclet execution. A modern less-than-or-equal walker or callback with wrong EBP is not equivalent.

Global unwind, raw control, classification, domain-error leaf, decomposition, IEEE records and math-error dispatcher remain implemented. Earlier evidence documents are historical checkpoints; their counts do not override this 17-entry inventory. No original address was removed or dummy counted as completed.

Five integrating units pass Win32 syntax-only checks. Private unlinked listings, read-only code/constant comparisons, inventory, naming and project/filter checks validate this checkpoint. No game build, tests or game run occurred. Runtime exceptional behavior and remaining public integration still need confirmation.
