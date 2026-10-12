# Current shared/runtime remainder - 2026-10-12

Current production inventory: **668/670 shared/system entries (99.70%), two open**; globally **4712/5063 (93.07%), 351 open**. [SHARED_CURRENT_OPEN_ENTRIES.csv](SHARED_CURRENT_OPEN_ENTRIES.csv) retains the two compiler entries. [SHARED_32_ENTRY_CLOSURE_AUDIT.csv](SHARED_32_ENTRY_CLOSURE_AUDIT.csv) retains all 32 requested entries: **30 implemented, two open**. Finishing the complete block remains outstanding.

The numerical group is implemented through both classification dispatchers, the completion continuations, exponential/arc-trig preparation and all nine public/x87 entries. Three partial standard-CRT adapters are replaced by the Windows implementations. All sixteen atan2 callbacks and the original positive-zero axis behavior are connected. Existing scalar kernels and the earlier unary/binary error entries remain connected to the project dispatcher. See [SHARED_X87_DISPATCH_COMPLETION.md](SHARED_X87_DISPATCH_COMPLETION.md) for complete evidence, compiler ABI contracts and private-backend/runtime-validation limits.

## Compiler/unwind: two open

- **004E7D40, Runtime_ProbeStack:** EAX size, exact page-touch order, allocation in the caller's ESP and relocation of its return address. An ordinary C++ function allocating its own stack frame does not provide that contract.
- **004EC67E, Runtime_LocalUnwind:** temporary FS registration and valid handler metadata, equality-based scope stop, pre-callback try-level update, original EBP funclets/notification order and collided-unwind behavior. A modern scope walker or callback with the wrong frame is not silently substituted.

Windows evidence is available locally. No native address is removed from the denominator or dummy counted as complete. The next block is these two compiler bindings.

Five integrating units pass Win32 syntax-only checks. Private unlinked `/Od` and `/O2` listings verify new instruction bridges, frame/callback boundaries, original argument/control order and existing scalar/error entries. All 30 complete code identities, literal regions, CFG metadata, scaling constants, inventory, naming and project/filter checks pass. No game build, tests or game run occurred; exceptional runtime resumption is not claimed verified.
