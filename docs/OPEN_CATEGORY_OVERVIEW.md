# Current category inventory - 2026-10-11

Fresh registered-source / Windows ASM / Game.exe counting gives **4426 / 5063 mapped entries (87.42%)**, **637 open (12.58%)**. The categories form a disjoint partition. Temporary link dummies and explicitly unfinished enclosing functions are excluded. These are native-entry source counts, not verified gameplay percentages.

## All open categories

| Category | Mapped / known | Mapped % | Open | Open % |
|---|---:|---:|---:|---:|
| Menues, GUI und Bedienung | 1106 / 1183 | 93.49% | 77 | 6.51% |
| Karte, Ressourcen und Wegfindung | 167 / 260 | 64.23% | 93 | 35.77% |
| Engine, Grafik, Wetter und Partikel | 551 / 679 | 81.15% | 128 | 18.85% |
| System, Laufzeit und gemeinsame Hilfen | 542 / 696 | 77.87% | 154 | 22.13% |
| Computer-KI und Spielassistent | 58 / 243 | 23.87% | 185 | 76.13% |

## Categories with no open entries in the current inventory

| Category | Mapped / known | Mapped % | Open | Open % |
|---|---:|---:|---:|---:|
| Fahrzeuge, Schiffe und Passagiere | 185 / 185 | 100.00% | 0 | 0.00% |
| Gebaeude (Hausblock) | 116 / 116 | 100.00% | 0 | 0.00% |
| Gemeinsame Bewegung, Kampf und Objektablauf | 93 / 93 | 100.00% | 0 | 0.00% |
| Menschen, Beduerfnisse und Berufe | 584 / 584 | 100.00% | 0 | 0.00% |
| Missionen und Kampagnen | 73 / 73 | 100.00% | 0 | 0.00% |
| Netzwerk und Mehrspieler | 164 / 164 | 100.00% | 0 | 0.00% |
| Spielsteuerung, Laden/Speichern und Spieldaten | 466 / 466 | 100.00% | 0 | 0.00% |
| Strassen, Palisaden und Wegweiser | 116 / 116 | 100.00% | 0 | 0.00% |
| Tiere | 67 / 67 | 100.00% | 0 | 0.00% |
| Ton, Musik und Videos | 138 / 138 | 100.00% | 0 | 0.00% |

## Interpretation and remaining work

**459 of 637 open entries** have provisional assignments from broad Windows address regions and require individual identification. The **24 explicit unfinished boundaries** are included in the 637, not added on top.

All 73 originally selected human/needs/job gaps and the additional vehicle-AI search dependency are reconstructed. The former **530/603** grouping is corrected to **584/584**, with nineteen selected entries identified in shared/GUI/vehicle/AI integration. Every selected entry is implemented; none was hidden by reclassification. Mapping increases by **74 entries** without denominator expansion. See [HUMAN_ROUTINES_COMPLETION.md](HUMAN_ROUTINES_COMPLETION.md). Missions and game-control work remain documented in [MISSIONS_CAMPAIGNS_COMPLETION.md](MISSIONS_CAMPAIGNS_COMPLETION.md) and [GAME_CONTROL_DATA_COMPLETION.md](GAME_CONTROL_DATA_COMPLETION.md).

The next smallest open category is menus/GUI/input (**77 open**): destruction/lifetime and selection-window boundaries. Maps retain validation/walking/resistance/continent callbacks and unidentified routines. Engine work includes rendering, weather overlays and initialization. System/shared work includes numeric/runtime helpers and unidentified entries. Computer AI retains script loading, callback, update/repair and notification trees; reconstructed vehicle-AI leaves do not complete that enclosing update.

The reported green road pixels remain a pending graphics discrepancy relative to Windows. Their cause and intended appearance are unverified. Zero inventory gaps does not establish live equivalence or absence of bugs. Human gameplay, campaign playthroughs, actual music playback and other live behavior remain unverified.

The scan covers **279 CPP / 259 headers**, **2536** unique naming rows and **21** unchanged unmatched declaration signatures. Twenty-three distinct changed units passed MSVC Win32 `/Zs`; project/filter parity, naming uniqueness, native coverage and whitespace checks pass. No build, tests or game run occurred. Private scripts and full per-address inventories stay under ignored `.codex-work/`.
