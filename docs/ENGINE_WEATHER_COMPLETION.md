# Windows weather rendering completion

Snapshot: 2026-10-12. The weather drawing block contains ten reconstructed native entries: eight engine entries and two general random-generator entries. The three former weather drawing dummies are removed. This is source reconstruction progress; no gameplay reproduction is claimed.

## Native entries

| Windows entry | Implementation | Confirmed Windows behavior |
| --- | --- | --- |
| 00410C2D | NXBasics::CRandom constructor | Calls SetSeed; eight-byte current-state/seed object |
| 00410C3F | NXBasics::CRandom::SetSeed | Masks bit 31, counts initial set bits, applies the original ordered bit table, stores seed then current state |
| 0047D37B | Display_l_DE_SpecialOverlay_Rain_Draw | Enable gate, center density, seed 2, doubled timer, three vertical pixel updates |
| 0047D64A | Display_l_DE_SpecialOverlay_Snow_Draw | Enable gate, center density, seed 1, white center; every even-index flake also updates four neighbors |
| 0047D918 | Display_l_DE_SpecialOverlay_Sand_Draw | Enable gate, center density, seed 1, twenty speed choices, four horizontal writes using the center's converted color |
| 00484D77 | E2MapWeatherManagerSectorManager_Snow_GetDensity | Sector interpolation using each record's +4 density |
| 00484E80 | E2MapWeatherManagerSectorManager_Rain_GetDensity | Sector interpolation using each record's +0 density |
| 00484F85 | E2MapWeatherManagerSectorManager_Sand_GetDensity | Sector interpolation using each record's +8 density |
| 0048534A | Coordinates_GetInSectorXPercentage | Signed `(x-5)/10`, sector bounds, percentage clamp |
| 0048538F | Coordinates_GetInSectorYPercentage | Separate original Y body and dimension, with the same signed arithmetic |

The three density functions remain distinct. They validate the packed map point, obtain X/Y percentages, fetch four sector pointers in original order, interpolate horizontally then vertically with signed divisions, and clamp the result to 0..10000. Wrapped 32-bit subtraction, multiplication and addition precede each division. The original functions pop twelve argument bytes but never read their Boolean or player arguments; the C++ interfaces retain those parameters without adding visibility filtering.

The existing sector pointer routine 004853D6, map initialization/INI/savegame routines, display projection 00482049, and random-number step 00410CCD/00410CD2 are reused. No existing native address is newly counted for those dependencies. `CRandom::Get` forwards to the existing eight-byte state implementation.

## Data, drawing and ownership

All behavior and layout below come from Windows assembly and the original Game.exe. Mac symbols supply names only.

- Complete speed regions: rain 004F4458..004F44A7 (ten pairs), snow 004F44A8..004F44F7 (ten pairs), sand 004F44F8..004F4597 (twenty pairs, including negative vertical speeds). The first component divides the vertical timer; the second divides the horizontal timer. Each mark consumes four random values in original order.
- Complete SetSeed bit-order region: 004F36CC..004F374B (32 DWORDs). The low/high population adjustments use the initial population count, including the original `32 - count` high-population rule. Weather restarts its local generator every frame; it does not advance the game's global random state.
- Display gates are bytes +0x928/+0x930/+0x938. Lazy 65536-WORD tables are stored at +0x92C/+0x934/+0x93C. Each table calls the original new-handler-enabled allocator 004E5DDD through Runtime_AllocateWithNewHandler(0x20000); existing Display_ExitSpecialOverlays (0047D335) releases all three through Runtime_Free and clears the fields. The new-handler retry and eventual unchecked null path are retained.
- Tables expand the original five-bit channel values by eight. Rain adds RGB 50/50/70; snow adds 170/170/170; sand uses `(channel + RGB 300/300/200)/3`. Each result saturates to 255 and passes through the existing C16BitPixelConverter::PackColor. These operations retain the original RGB565 green-channel handling.
- Density is sampled at the viewport center in world pixels, converted through the existing map projection. Bitmap width/height and row pixel count are read from +0x10/+0x14/+0x30. World scroll compensation uses display +0x954/+0x958 and half extents +0x964/+0x968, with signed division toward zero and 32-bit wrapped arithmetic.
- Original count constants are 004F45A0 = 0.0001, rain 004F4598 = 0.025, snow 004F45A8 = 0.02, sand 004F45B0 = 0.05. Zero count returns before random/timer/table setup. Negative nonzero count retains setup and skips the drawing loop. Bitmap depth 1 is checked after random construction, timer and scroll compensation, as in Windows.
- Pixel positions use unsigned wrapping additions/modulo after signed timer division. The original two-pixel border exclusion is retained. Rain updates above/center/below in order. Snow writes 0xFFFF to the center and updates left/right/above/below only for even original indices. Sand writes right/center/two-left/left using one converted center color. Existing foreground order remains rain, snow, sand.

## Validation and limits

Static checks match **1201 original instructions across all ten complete entries** against Game.exe. All **320 speed-table bytes** and **128 seed-table bytes** match the source constants. The density bodies' unused argument slots, distinct naming mappings and removal of all three dummies/exclusions are checked. Project/filter membership is synchronized: 290 CPP / 264 headers. All five changed integrating units pass MSVC Win32 `/Zs`; the new weather unit was checked again after call-order cleanup. No tests, full build, linking or game run occurred.

The original count calculation retains intermediate x87 extended precision. The C++ calculation uses the same binary64 constants and multiplication order, followed by the existing truncation helper; intermediate precision and floating-point status can differ. Exact boundary counts and live visual equivalence have not been validated. No inline assembly is introduced.

Fresh inventory: **4724/5063 (93.30%), 339 open**. Engine: **564/678 (83.19%), 114 open**. General helpers: **672/672**, still zero open. The latter denominator grows by two because the proven CRandom entries move out of their provisional graphics-region assignment; no Windows address is removed from the global denominator. Explicit unfinished boundaries fall from 16 to 13. See [OPEN_CATEGORY_OVERVIEW.md](OPEN_CATEGORY_OVERVIEW.md).

Next connected graphics block: the missing bitmap ZoomIn entry (0040EEFA) and its related bitmap drawing/storage operations. Remaining engine initialization, enclosing rendering callbacks, debug overlays and fish drawing retain their unfinished status.
