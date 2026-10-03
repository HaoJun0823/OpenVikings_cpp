# Guide-manager startup and ownership

Snapshot: 2026-10-04, HEAD d22ddbd with pending menu and guide-manager corrections.

## Reported failure

The user's screenshot has already entered the in-game GUI. `WorldOverviewElement_DrawGuides` calls `CGuideIterator::GetFirstGuideId`, which reads the array's first scan bound at +0x14. `CGuideArray::sTheObjectPtr` is null, producing the reported read at address 0x14.

`Game_StartUp` already calls the original guide-manager constructor between house-array and player-statistics initialization. That constructor was temporary dummy 0041AE53, which returned null without publishing the manager or constructing its array. The existing CGuideArray constructor correctly publishes both array globals; the iterator is not the source of this missing owner.

## Reconstructed owner boundary

| Windows address | Behavior |
| --- | --- |
| 0041AE53 | Publish manager, initialize link counter/scan cursor, allocate and construct the array, register four callbacks, allocate scan buffer |
| 0041AEDF | Existing destruction: free scan buffer, unregister callbacks, destroy/free array and clear manager |
| 0041AF52 | Save guide-manager chunk, nested guide-array chunk, then four-byte manager counter |
| 0041AF8A | Publish loading owner, reject unsupported version by skipping, construct/load array, read counter, reset cursor, allocate scan buffer and register callbacks |
| 004266A9 / 00426747 / 00426780 / 004267B4 | Existing array construction/destruction/save/load bodies, reused unchanged |

The Windows manager allocation is 0x1368 bytes. Array objects use 0x30 bytes; initial capacity is 400, record width 0x184 and entry storage size 0x25E40. The scan buffer pointer is +0x1364 and allocates one byte per array slot. The original constructor does not clear this buffer or the rest of the manager object. Runtime allocation/free ownership is preserved.

Ordinary startup registers callback types 1, 33, 31 and 32 hexadecimal, in that order, with user data 64 hexadecimal and priority zero, before allocating the scan buffer. Loading allocates the buffer before registration. The array constructor publishes its singleton/storage and emits callback 0B with forward=true and capacity before the manager registers its callbacks. Destruction retains the exact inverse ownership path already implemented in C2LogicLifetimeAccess.cpp.

The saved manager marker is 78646778, version 1; the nested array marker is 78646761. On an unsupported manager version, the loading constructor has already published its owner, skips the chunk and returns without further initialization. Allocation failure retains the original unchecked array/capacity access. No extra deletion of a previous singleton is invented.

## Complete static region

All Windows xrefs and the neighboring Mac guide-manager routines identify the former DAT_00511540 range:

| Region | Recovered role |
| --- | --- |
| 00511540..00511543 | Signed guide scan cursor; startup/loading reset to -1; callback advances through active guide records every twelve ticks |
| 00511544..00511547 | Two signed 16-bit coordinates: minimum changed-map point |
| 00511548..0051154B | Two signed 16-bit coordinates: maximum changed-map point |

The complete twelve-byte region is now SGuideManagerStaticVars/gGuideManagerStaticVars, with size and offset assertions. The application initializer preserves the original four WORD writes, including their order, and leaves the scan DWORD untouched. The Mac symbol confirms the guide-manager association; field roles follow Windows reads/writes. No supported xref establishes a separate identity for the neighboring alignment gap at 0051154C.

## Verification and limits

Windows ASM confirms constructor order, publication, allocation sizes, registration arguments, save/load ordering and the existing destruction path. Static source checks verify singleton setup, full-region naming, startup binding, stub/inventory removal, project/filter synchronization and README OA112/FP100/FP101. No build, tests or game runtime were invoked.

This removes one constructor dummy and adds three genuine mappings: 2,068/5,056 (40.90%), with 2,988 unmapped candidates and 122 tracked dummies. The existing 152 cpp / 149 header project inventory is unchanged; twelve startup constructor boundaries remain unfinished.

The guide callback 0041B936, insertion 0041B061 and removal 0041B0A5 remain explicitly tracked dummies. Their genuine connection/goods scan tree, including CMapPathfinderElement::SearchOverMap, is the next guide block. Save/load entry bodies are now available, but the broader game-save dispatcher remains outside this correction. This startup fix does not establish functioning guide routing, goods scans or complete savegame loading. The user's in-game screenshot confirms progression beyond menu startup; the corrected guide initialization remains unverified at runtime.

The retained callback dummy previously returned false, which aborts CCallbackManager::ExecuteCallback. Windows 0041BB4D returns true on every branch. The dummy now preserves that proven return to continue the callback chain while its guide work remains pending. It stays in the inventory and is excluded from genuine mapping counts; no reconstructed callback body is claimed.
