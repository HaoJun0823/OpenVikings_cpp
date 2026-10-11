# Current category inventory - 2026-10-11

The refreshed registered-source / Windows ASM / Game.exe inventory contains **4267 / 5062 mapped entries (84.29%)**, with **795 open entries (15.71%)**. Categories below form a disjoint partition of those entries. Temporary link dummies and explicitly unfinished enclosing functions are excluded from mapped counts. This is native-entry source coverage, not a percentage of verified gameplay functionality.

## All open categories

| Category | Mapped / known | Mapped % | Open | Open % |
|---|---:|---:|---:|---:|
| Missionen und Kampagnen | 61 / 86 | 70.93% | 25 | 29.07% |
| Spielsteuerung, Laden/Speichern und Spieldaten | 437 / 466 | 93.78% | 29 | 6.22% |
| Menschen, Beduerfnisse und Berufe | 524 / 600 | 87.33% | 76 | 12.67% |
| Menues, GUI und Bedienung | 1105 / 1184 | 93.33% | 79 | 6.67% |
| Karte, Ressourcen und Wegfindung | 163 / 260 | 62.69% | 97 | 37.31% |
| Engine, Grafik, Wetter und Partikel | 541 / 674 | 80.27% | 133 | 19.73% |
| System, Laufzeit und gemeinsame Hilfen | 530 / 695 | 76.26% | 165 | 23.74% |
| Computer-KI und Spielassistent | 48 / 239 | 20.08% | 191 | 79.92% |

## Categories with no open entries in the current inventory

| Category | Mapped / known | Mapped % | Open | Open % |
|---|---:|---:|---:|---:|
| Fahrzeuge, Schiffe und Passagiere | 170 / 170 | 100.00% | 0 | 0.00% |
| Gebaeude (Hausblock) | 116 / 116 | 100.00% | 0 | 0.00% |
| Gemeinsame Bewegung, Kampf und Objektablauf | 91 / 91 | 100.00% | 0 | 0.00% |
| Netzwerk und Mehrspieler | 164 / 164 | 100.00% | 0 | 0.00% |
| Strassen, Palisaden und Wegweiser | 114 / 114 | 100.00% | 0 | 0.00% |
| Tiere | 65 / 65 | 100.00% | 0 | 0.00% |
| Ton, Musik und Videos | 138 / 138 | 100.00% | 0 | 0.00% |

## Interpretation and remaining work

Category assignment uses registered production source and established names where available. **591 of the 795 open entries** currently have provisional assignments based on broad Windows address regions. These entries still need individual identification; their present category is an orientation, not established behavior. System/shared entries also contain unresolved technical regions. The 39 explicitly tracked unfinished boundaries are part of the 795 open entries, not an additional total.

The next smallest open category is missions/campaigns (25 entries): its goal and result dispatchers remain partial. The game-control category includes the enclosing game-checksum path. Humans retain partial user-command, task start/perform, job and condition-update boundaries. Computer AI retains script loading, callbacks and notification branches. Map callbacks for validation, walking flags, resistance, continents and movable size remain open. Engine work includes weather overlays, initialization and landscape/creature rendering. GUI inventory includes selection-window deletion and tooltip-handler destruction. The system/shared inventory includes runtime numeric helpers and unidentified entries.

Zero open entries in a category does not establish live behavior, complete assets or absence of bugs. Actual music playback, live vehicle/AI behavior and remaining campaign branches still require runtime verification. The current [landscape caption correction](LANDSCAPE_TOOLTIP_TEXT_TABLE.md) fixes an already mapped function and leaves counts unchanged.

The scan reports 276 registered CPP files, 256 headers, 2388 unique naming rows and 21 existing unmatched declaration signatures. No build, tests or game run occurred. Detailed per-address inventory and generation scripts remain under ignored `.codex-work/`.
