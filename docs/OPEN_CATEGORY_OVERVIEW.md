# Current category inventory - 2026-10-11

The refreshed registered-source / Windows ASM / Game.exe inventory contains **4309 / 5063 mapped entries (85.11%)**, with **754 open entries (14.89%)**. Categories below form a disjoint partition of those entries. Temporary link dummies and explicitly unfinished enclosing functions are excluded from mapped counts. This is native-entry source coverage, not a percentage of verified gameplay functionality.

## All open categories

| Category | Mapped / known | Mapped % | Open | Open % |
|---|---:|---:|---:|---:|
| Missionen und Kampagnen | 64 / 88 | 72.73% | 24 | 27.27% |
| Menschen, Beduerfnisse und Berufe | 527 / 603 | 87.40% | 76 | 12.60% |
| Menues, GUI und Bedienung | 1105 / 1184 | 93.33% | 79 | 6.67% |
| Karte, Ressourcen und Wegfindung | 166 / 259 | 64.09% | 93 | 35.91% |
| Engine, Grafik, Wetter und Partikel | 544 / 677 | 80.35% | 133 | 19.65% |
| System, Laufzeit und gemeinsame Hilfen | 533 / 695 | 76.69% | 162 | 23.31% |
| Computer-KI und Spielassistent | 52 / 239 | 21.76% | 187 | 78.24% |

## Categories with no open entries in the current inventory

| Category | Mapped / known | Mapped % | Open | Open % |
|---|---:|---:|---:|---:|
| Fahrzeuge, Schiffe und Passagiere | 173 / 173 | 100.00% | 0 | 0.00% |
| Gebaeude (Hausblock) | 116 / 116 | 100.00% | 0 | 0.00% |
| Gemeinsame Bewegung, Kampf und Objektablauf | 92 / 92 | 100.00% | 0 | 0.00% |
| Netzwerk und Mehrspieler | 164 / 164 | 100.00% | 0 | 0.00% |
| Spielsteuerung, Laden/Speichern und Spieldaten | 454 / 454 | 100.00% | 0 | 0.00% |
| Strassen, Palisaden und Wegweiser | 115 / 115 | 100.00% | 0 | 0.00% |
| Tiere | 66 / 66 | 100.00% | 0 | 0.00% |
| Ton, Musik und Videos | 138 / 138 | 100.00% | 0 | 0.00% |

## Interpretation and remaining work

Category assignment uses registered production source and established names where available. **554 of the 754 open entries** currently have provisional assignments based on broad Windows address regions. These entries still need individual identification; their present category is an orientation, not established behavior. The 38 explicitly tracked unfinished boundaries are part of the 754 open entries, not an additional total.

All 29 original game-control/save-data gaps have been implemented together with their required dependencies. Individual identification assigns some original entries to humans, vehicles, missions, maps and shared helpers; none was closed merely by changing categories. The corrected game-control category is **454 / 454**. The previous 437 / 466 grouping is superseded by this disjoint partition. Following the previously incomplete checksum tree also expanded the known-entry inventory by one. See [GAME_CONTROL_DATA_COMPLETION.md](GAME_CONTROL_DATA_COMPLETION.md).

The next smallest open category is missions/campaigns (24 entries): its goal and result dispatchers remain partial. Humans retain partial user-command, task start/perform, job and condition-update boundaries. Computer AI retains script loading, callbacks and notification branches. Map callbacks for validation, walking flags, resistance, continents and movable size remain open. Engine work includes weather overlays, initialization and landscape/creature rendering. GUI inventory includes selection-window deletion and tooltip-handler destruction. System/shared work includes runtime numeric helpers and unidentified entries.

The reported green pixels on roads are a pending graphics discrepancy relative to the Windows original. A screenshot alone does not establish the cause or intended moss; inspect texture, blending and color conversion against Windows evidence before changing rendering.

Zero open entries in a category does not establish live behavior, complete assets or absence of bugs. Music playback, vehicle/AI behavior, checksums and remaining campaign branches still require runtime verification.

The scan reports 278 registered CPP files, 258 headers, 2429 unique naming rows and 21 existing unmatched declaration signatures. Nineteen changed/dependent source units passed MSVC Win32 `/Zs`; project/filter parity, naming uniqueness and whitespace checks passed. No build, tests or game run occurred. Detailed per-address inventory and generation scripts remain under ignored `.codex-work/`.
