# Windows bitmap zoom and half-size copy completion

Snapshot: 2026-10-12. Seven native entries are reconstructed: six bitmap entries and their rectangle containment dependency. The former ZoomIn dummy is removed and the existing display update now calls the original half-size copy. These are source reconstruction counts; live equivalence remains unverified.

## Native entries

| Windows entry | Implementation | Confirmed Windows behavior |
| --- | --- | --- |
| 0040C5E5 | NXBasics::CBitmap::Delete | Ordinary destructor first; flag bit 0 controls Runtime_Free; returns the original pointer |
| 0040C682 | NXBasics::CBitmap constructor | Initialize object, construct local rectangle from four coordinates, construct virtual view |
| 0040D1F8 | CopyIntoBitmap_HalfSize | Equal depths, non-null raw storage, inclusive containment, every second source pixel and row |
| 0040D3F8 | CopyIntoBitmap_HalfSize_Optimized | Native eligibility gates; packed 16-bit or paired 32-bit copying; ordinary half-size fallback |
| 0040EEFA | ZoomIn | In-place quadrant expansion for 8-, 16- and 32-bit pixels; no allocation or bitmap-field changes |
| 0040F522 | GetUnclippedPixelPtr | Wrapped stride/address arithmetic; no clipping or pointer validation; unknown depth returns null |
| 00410601 | NXBasics::SRectangle::IsInside | Signed inclusive endpoint comparisons with wrapped DWORD arithmetic |

The complete ranges are 0040C5E5..0040C600, 0040C682..0040C6BE, 0040D1F8..0040D3F7, 0040D3F8..0040D56A, 0040EEFA..0040F521, 0040F522..0040F56E and 00410601..00410653. Windows assembly and Game.exe establish all behavior, offsets and constants. Corresponding Mac symbols supply names only. The scalar deleting wrapper uses the proven bitmap destructor identity and its Windows flags semantics.

## Storage, copying and original observations

The virtual constructor reuses the existing non-owning view machinery. The deleting wrapper reuses the existing ordinary destructor and project allocator family. No layout, vtable slot, palette ownership or allocation family changes are introduced.

The ordinary half-size copy requires equal depths before its storage checks. Negative destination coordinates change source counters and reset the destination rectangle origin; the far edges are checked for full containment, rather than clipped. The original negative-X path includes sourceX both in the initial source pointer and column counter, and keeps that column counter in the destination offset. The resulting extra source displacement and destination gap are preserved. Row and column bounds use signed comparisons; address arithmetic wraps at 32 bits.

The optimized entry requires doubled dimensions, equal 16-bit or 32-bit depth, source width divisible by 64 and even source height. Other cases call the ordinary entry at (0, 0). Its 16-bit loop packs alternate source words through overlapping DWORD reads; its 32-bit loop copies two alternate DWORDs per group. Both retain original row-stride adjustments and decrementing do/while counters. Empty dimensions are not guarded on this native fast path; zero counters underflow as in Windows.

The 16-bit fast path also performs actual MOV reads 128 bytes ahead of the source and 64 bytes ahead of the destination before each group. These are not CPU prefetch instructions. Volatile DWORD reads retain those memory accesses, including the original absence of bounds or null checks. Actual crash triggers and allocation-boundary effects have not been reproduced. These original observations are registered as OA164 in Readme.md; no modernization is applied.

ZoomIn reads and writes each quadrant in original order so overlapping pixels retain their native in-place behavior. Each selected pixel becomes a 2x2 block. Width/height divisions truncate toward zero, quarter-sized loops leave remainder areas as in Windows, and the raw-storage null check precedes depth dispatch. No temporary image or guessed resampling is introduced.

## Display integration

The existing foreground zoom loop calls CBitmap::ZoomIn directly. In Display_UpdateDisplay (0048162F), the differing work/output bitmap branch now calls CopyIntoBitmap_HalfSize_Optimized: the original call at 00481683 targets 0040D3F8. This replaces the previous general FitIn substitution and restores the original choice of every second pixel on the eligible half-size path. Other display orchestration remains unchanged.

## Verification and inventory

- 994 decoded instructions across all seven ranges and the existing display integration range match original Game.exe bytes. The look-ahead MOVs and display half-size target are confirmed separately.
- XBitmap.cpp, XBasicTypes.cpp, C2EngineDisplayAccess.cpp and TemporaryLinkStubs.cpp pass MSVC Win32 C++17 syntax-only checks without diagnostics.
- Project/filter membership remains 290 CPP / 264 headers. Naming inventory has 2827 unique rows; the removed ZoomIn dummy is absent from the unfinished inventory. No inline assembly is introduced.
- No Debug/Release build, linking, tests or game run occurred. Visual zoom, half-size output, aliasing in gameplay and unusual dimensions still require runtime verification.

Global inventory: **4731/5063 (93.44%), 332 open**. Engine: **570/677 (84.19%), 107 open**. The rectangle predicate moves from the provisional engine region to shared basics, which is now **673/673**; no native address leaves the global denominator. AI remains **96/321, 225 open**. There are **12** explicit unfinished boundaries, **270** provisional open assignments and **17** unmatched declaration signatures.

Continue with the remaining graphics-engine initialization and related rendering dependencies. Private inspection, inventory and handoff files remain under ignored `.codex-work/`.
