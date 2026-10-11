# Scout appearance, breeder continuation and particle boundary fixes - 2026-10-11

## Confirmed Windows reconstruction fixes

The registered miscellaneous graphics callback `0047EC5B` previously omitted events `0x1F` and `0x20`. Windows branches `0047F119` and `0047F12C` call the complete job-change entry `00480576` and armor-change entry `00480676`. Both entries are now implemented and connected. The enclosing callback remains partial and excluded from completion counts.

`00480576` ignores old job zero. Otherwise it obtains the new job's graphics using the human's tribe, preserves an existing appearance variant where available, and uses the first entry when the variant is absent. It updates body/shadow and head managers without rewriting the variant identifiers. The job palette patch runs before a soldier's armor patch. `00480676` formats the exact `Game.exe` string `human_armor_%3.3d` at `00505E60`, resolves its patch ID and applies it to body/head dynamic data. The Mac header supplies only the names `CE2DynamicDataHumanManager::HumanJobChanged` and `HumanArmorChanged` (lines 5166/5167); all offsets, conditions and ordering come from Windows.

The user's pause stack on "Trostloses Erwachen" runs through `SearchOverMap -> Human_FindGood_Complex -> CollectWantedGood -> Breeder`. Windows breeder entry `0046CB2C` dispatches circulation mode 2 through `004753CB` (`FillStock_Fetch`), not the direct `CollectWantedGood` leaf. Mode 5 calls `00475A3A` (`FlushStock_Fetch`), not its cargo-bearing bring-away leaf. The full-stock branches `0046CE22..0046CE71` enter that same fetch dispatcher. These three incorrect C++ calls are corrected. This restores the arrival/pickup continuation before another search, and the fetch stage before flushing cargo. The observed map hang still requires a game retest; the pause stack establishes the affected branch, not a completed runtime reproduction.

## Explicit boundary protection

The crash screenshot points to the micro-map visibility read in particle sorter `0048688E`. Windows performs viewport clipping before deferred particle movement and then uses the unchecked micro-map accessor `004237D2`. Wind and deferred movement can change a particle's map coordinate before this read; camera clipping does not guarantee an in-map coordinate afterward.

The C++ renderer now uses the existing checked accessor `0042385B` after movement. A point outside the logical map skips visibility, brightness and drawing, retains its pool slot/lifetime, and updates the previous-slot bookkeeping as in the invisible-particle branch. Expired-arrow placement in tick entry `00486D61` uses the same checked accessor before testing terrain. Valid in-map paths retain their previous ordering and results. The terrain-height helper independently wraps macro coordinates as in Windows.

This is an explicit bounds-safety extension to the original unchecked invalid-coordinate paths, not a claim that Windows contained these guards. The exact crashing particle coordinates are unavailable; the user could not reproduce the crash again. Memory corruption from another source has not been ruled out.

## Music cause confirmed in the supplied map data

The user clarified that the wrong hostile Viking music occurs while viewing Franks, without combat. The combat callback therefore does not explain this report. Windows visible-player selector `00485BF0` counts displayed houses through `004964D0`, excludes the controlling player, and chooses the largest count (first player on ties). Humans do not increment these counters. Current clear/count/selection code matches these Windows xrefs.

`00485C3A` reads the selected player's tribe from game-control stride `0x4A80`, offset `0x0C`; tribe 2 requests event 3 and its diplomacy variant. `00403D6C` resolves hostile event 3 to `Theme_Franken_Hostile.sgt`. The player parser writes the same tribe field. The installed Viking/Franken hostile and attack segment files have distinct RIFF GUIDs. No speculative music remapping is introduced.

The user identified the map as "Die Rebellion" (`singleplayer_01_07`). Its supplied `map.ini` has `player 3 2 1 1` and `player 4 2 1 1`: both are configured as tribe 1 (Vikings). Player 3 has 13 Frankish houses and 45 humans; player 4 has 17 Frankish houses and 51 humans. The actual house/human definitions name Frankish types. String 54 identifies player 4 as "Franken". This explains why the original field-based music dispatcher selects Viking themes even for visually Frankish settlements.

A two-line patch changing both player tribe fields to 2 was prepared privately but was not applied. The user explicitly chose to retain the original data, then independently verified that the original Windows executable plays the same music. This report is resolved as confirmed original behavior, not a reconstruction defect. No music implementation or installed-map change is made. Changing these tribe fields would also change allowed-item initialization.

## Validation and inventory

All five changed C++ translation units pass MSVC Win32 C++17 syntax-only checks without diagnostics. Project/filter membership remains 286 CPP / 262 headers; naming inventory has 2814 unique rows. No build, test project or game run was performed. Pre-existing formatting edits in `C2EngineParticleDrawing.cpp` are preserved separately from the functional patch.

Registered-source inventory: **4670/5063 (92.24%)**, **393 open**. Engine: **556/680**, **124 open**. Shared/system: **626/670**, **44 open**. AI/assistant: **96/321**, **225 open**. The two new completed addresses are `00480576` and `00480676`; no denominator entry was removed. Functional corrections and boundary checks add no completion count. There remain 19 explicitly partial entries, 283 provisional open assignments, and 17 unmatched declaration signatures. Counts describe reconstruction coverage, not verified gameplay equivalence.
