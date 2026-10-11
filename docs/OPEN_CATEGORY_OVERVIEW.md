# Current category inventory - 2026-10-11

Fresh registered-source / Windows ASM / Game.exe counting gives **4511/5062 mapped entries (89.11%)**, **551 open (10.89%)**. Categories form a disjoint partition. Temporary link dummies and explicitly unfinished enclosing functions are excluded from mapped entries. These are native-entry source counts, not verified gameplay percentages.

## All open categories

| Category | Mapped / known | Mapped % | Open | Open % |
|---|---:|---:|---:|---:|
| Karte, Ressourcen und Wegfindung | 167 / 259 | 64.48% | 92 | 35.52% |
| Engine, Grafik, Wetter und Partikel | 551 / 679 | 81.15% | 128 | 18.85% |
| System, Laufzeit und gemeinsame Hilfen | 542 / 696 | 77.87% | 154 | 22.13% |
| Computer-KI und Spielassistent | 74 / 251 | 29.48% | 177 | 70.52% |

## Categories with no open entries in the current inventory

| Category | Mapped / known | Mapped % | Open | Open % |
|---|---:|---:|---:|---:|
| Fahrzeuge, Schiffe und Passagiere | 185 / 185 | 100.00% | 0 | 0.00% |
| Gebaeude (Hausblock) | 116 / 116 | 100.00% | 0 | 0.00% |
| Gemeinsame Bewegung, Kampf und Objektablauf | 93 / 93 | 100.00% | 0 | 0.00% |
| Menschen, Beduerfnisse und Berufe | 584 / 584 | 100.00% | 0 | 0.00% |
| Menues, GUI und Bedienung | 1175 / 1175 | 100.00% | 0 | 0.00% |
| Missionen und Kampagnen | 73 / 73 | 100.00% | 0 | 0.00% |
| Netzwerk und Mehrspieler | 164 / 164 | 100.00% | 0 | 0.00% |
| Spielsteuerung, Laden/Speichern und Spieldaten | 466 / 466 | 100.00% | 0 | 0.00% |
| Strassen, Palisaden und Wegweiser | 116 / 116 | 100.00% | 0 | 0.00% |
| Tiere | 67 / 67 | 100.00% | 0 | 0.00% |
| Ton, Musik und Videos | 138 / 138 | 100.00% | 0 | 0.00% |

## Interpretation and remaining work

**376 of 551 open entries** have provisional assignments from broad Windows regions and need individual identification. The **24 explicit unfinished boundaries** are included in the 551, not added on top.

All 77 selected GUI gaps and nine direct dependencies have implementations; none was hidden through reclassification. The GUI category is corrected to **1175/1175**, with seven selected tower/build/upgrade entries identified as AI. One additional village dependency moves from maps to AI. The former human-list toggle address **004A1F8A** is part of a preceding epilogue; its real start is **004A1F8E**, decreasing the known denominator by one. See [GUI_REMAINDER_COMPLETION.md](GUI_REMAINDER_COMPLETION.md). Human, mission and game-data work remain documented in [HUMAN_ROUTINES_COMPLETION.md](HUMAN_ROUTINES_COMPLETION.md), [MISSIONS_CAMPAIGNS_COMPLETION.md](MISSIONS_CAMPAIGNS_COMPLETION.md) and [GAME_CONTROL_DATA_COMPLETION.md](GAME_CONTROL_DATA_COMPLETION.md).

The next smallest open category is **maps/resources/pathfinding (92 open)**, including validation, walking, resistance, continent callbacks and unidentified routines. Engine retains rendering, weather overlays and initialization; system/shared retains numeric/runtime helpers and unidentified entries. Computer AI retains enclosing script, callback, update/repair and notification trees. Reconstructed local tower/vehicle leaves do not complete that broader update.

The reported green road pixels remain an unverified graphics discrepancy relative to Windows. Zero inventory gaps does not establish live equivalence or absence of bugs. GUI/human gameplay, campaign playthroughs and actual music playback remain unverified.

The scan covers **282 CPP / 260 headers**, **2618** unique naming rows and **19** unmatched declaration signatures. Twenty-six changed units pass MSVC Win32 `/Zs`, with five existing byte/bool warnings. Project/filter parity, naming uniqueness, original native selection/callee coverage, category sums and whitespace checks pass. No build, tests or game run occurred. Private scripts and full inventories stay under ignored `.codex-work/`.
