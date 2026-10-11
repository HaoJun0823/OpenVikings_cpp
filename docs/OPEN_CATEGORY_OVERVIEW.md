# Current category inventory - 2026-10-11

Fresh registered-source / Windows ASM / Game.exe counting gives **4352 / 5063 mapped entries (85.96%)**, **711 open (14.04%)**. The categories form a disjoint partition. Temporary link dummies and explicitly unfinished enclosing functions are excluded. These are native-entry source counts, not verified gameplay percentages.

## All open categories

| Category | Mapped / known | Mapped % | Open | Open % |
|---|---:|---:|---:|---:|
| Menschen, Beduerfnisse und Berufe | 530 / 603 | 87.89% | 73 | 12.11% |
| Menues, GUI und Bedienung | 1105 / 1182 | 93.49% | 77 | 6.51% |
| Karte, Ressourcen und Wegfindung | 167 / 260 | 64.23% | 93 | 35.77% |
| Engine, Grafik, Wetter und Partikel | 551 / 679 | 81.15% | 128 | 18.85% |
| System, Laufzeit und gemeinsame Hilfen | 541 / 695 | 77.84% | 154 | 22.16% |
| Computer-KI und Spielassistent | 52 / 238 | 21.85% | 186 | 78.15% |

## Categories with no open entries in the current inventory

| Category | Mapped / known | Mapped % | Open | Open % |
|---|---:|---:|---:|---:|
| Fahrzeuge, Schiffe und Passagiere | 181 / 181 | 100.00% | 0 | 0.00% |
| Gebaeude (Hausblock) | 116 / 116 | 100.00% | 0 | 0.00% |
| Gemeinsame Bewegung, Kampf und Objektablauf | 92 / 92 | 100.00% | 0 | 0.00% |
| Missionen und Kampagnen | 73 / 73 | 100.00% | 0 | 0.00% |
| Netzwerk und Mehrspieler | 164 / 164 | 100.00% | 0 | 0.00% |
| Spielsteuerung, Laden/Speichern und Spieldaten | 460 / 460 | 100.00% | 0 | 0.00% |
| Strassen, Palisaden und Wegweiser | 115 / 115 | 100.00% | 0 | 0.00% |
| Tiere | 67 / 67 | 100.00% | 0 | 0.00% |
| Ton, Musik und Videos | 138 / 138 | 100.00% | 0 | 0.00% |

## Interpretation and remaining work

**523 of 711 open entries** have provisional assignments from broad Windows address regions and still require individual identification. The **34 explicit unfinished boundaries** are included in the 711, not added on top.

All 24 originally selected mission/campaign gaps and their required dependencies are connected. The earlier 64/88 grouping is corrected to **73/73** after identifying nearby shared getters and constructors. All selected entries are implemented; none was hidden by reclassification. Global mapping increases by 43 entries with no denominator expansion. See [MISSIONS_CAMPAIGNS_COMPLETION.md](MISSIONS_CAMPAIGNS_COMPLETION.md). Earlier game-control work remains documented in [GAME_CONTROL_DATA_COMPLETION.md](GAME_CONTROL_DATA_COMPLETION.md); its category now includes additional startup/map-member dependencies from this block.

The next smallest open category is humans/needs/jobs (**73 open**): partial user-command, task startup/performance, job and condition updates. GUI retains destruction/lifetime and selection-window boundaries. Maps retain validation/walking/resistance/continent callbacks and unidentified routines. Engine work includes rendering, weather overlays and initialization. System/shared work includes numeric/runtime helpers and unidentified entries. Computer AI retains script loading, callback, update/repair and notification trees.

The reported green road pixels remain a pending graphics discrepancy relative to Windows. Their cause and intended appearance are unverified. Zero inventory gaps does not establish live equivalence or absence of bugs. Campaign playthroughs, actual music playback and other live behavior remain unverified.

The scan covers **279 CPP / 259 headers**, **2472** unique naming rows and **21** unchanged unmatched declaration signatures. Nineteen changed/dependent units passed MSVC Win32 `/Zs`; project/filter parity, naming uniqueness and whitespace checks pass. No build, tests or game run occurred. Private scripts and full per-address inventories stay under ignored `.codex-work/`.
