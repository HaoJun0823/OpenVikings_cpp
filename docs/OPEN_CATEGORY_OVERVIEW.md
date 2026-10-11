# Current category inventory - 2026-10-11

Fresh registered-source / Windows ASM / Game.exe counting gives **4631/5062 mapped entries (91.49%)**, **431 open (8.51%)**. Categories form a disjoint partition. Temporary link dummies and explicitly unfinished enclosing functions are excluded from mapped entries. These are native-entry source counts, not verified gameplay percentages.

## All open categories

| Category | Mapped / known | Mapped % | Open | Open % |
|---|---:|---:|---:|---:|
| Engine, Grafik, Wetter und Partikel | 552 / 679 | 81.30% | 127 | 18.70% |
| System, Laufzeit und gemeinsame Hilfen | 607 / 679 | 89.40% | 72 | 10.60% |
| Computer-KI und Spielassistent | 81 / 313 | 25.88% | 232 | 74.12% |

## Categories with no open entries in the current inventory

| Category | Mapped / known | Mapped % | Open | Open % |
|---|---:|---:|---:|---:|
| Fahrzeuge, Schiffe und Passagiere | 185 / 185 | 100.00% | 0 | 0.00% |
| Gebaeude (Hausblock) | 116 / 116 | 100.00% | 0 | 0.00% |
| Gemeinsame Bewegung, Kampf und Objektablauf | 93 / 93 | 100.00% | 0 | 0.00% |
| Karte, Ressourcen und Wegfindung | 201 / 201 | 100.00% | 0 | 0.00% |
| Menschen, Beduerfnisse und Berufe | 585 / 585 | 100.00% | 0 | 0.00% |
| Menues, GUI und Bedienung | 1183 / 1183 | 100.00% | 0 | 0.00% |
| Missionen und Kampagnen | 74 / 74 | 100.00% | 0 | 0.00% |
| Netzwerk und Mehrspieler | 164 / 164 | 100.00% | 0 | 0.00% |
| Spielsteuerung, Laden/Speichern und Spieldaten | 466 / 466 | 100.00% | 0 | 0.00% |
| Strassen, Palisaden und Wegweiser | 116 / 116 | 100.00% | 0 | 0.00% |
| Tiere | 70 / 70 | 100.00% | 0 | 0.00% |
| Ton, Musik und Videos | 138 / 138 | 100.00% | 0 | 0.00% |

## Interpretation and remaining work

**288 of 431 open entries** still have provisional broad-region assignments. The **19 explicit unfinished boundaries** are included in the 431, not added on top.

The original map selection is preserved: **92 entries = 34 completed map entries + three completed military-danger dependencies + 55 still-open AI entries**. Maps change from **167/259** to **201/201**. AI changes from **74/251** to **77/309**. Its denominator grows by 58 because all three implemented dependencies and 55 remaining AI entries previously carried the provisional map label. Reclassification leaves the global denominator at **5062** and preserves all 55 open addresses. See [MAP_REMAINDER_COMPLETION.md](MAP_REMAINDER_COMPLETION.md) and [MAP_REMAINDER_NATIVE_ENTRIES.csv](MAP_REMAINDER_NATIVE_ENTRIES.csv).

The user requests continuing shared/system helpers (72 open). Five newly reconstructed GUI string wrappers move from the provisional system inventory into GUI; all ten reconstructed entries remain in the global denominator. See [SHARED_HELPERS_CHECKPOINT.md](SHARED_HELPERS_CHECKPOINT.md). Sixteen additional string/memory primitives are reconstructed in [SHARED_STRING_MEMORY_COMPLETION.md](SHARED_STRING_MEMORY_COMPLETION.md). Fourteen further runtime wrapper/cleanup entries are reconstructed in [SHARED_RUNTIME_WRAPPERS_COMPLETION.md](SHARED_RUNTIME_WRAPPERS_COMPLETION.md). Five additional integer/search/classification leaves are reconstructed in [SHARED_INTEGER_COMPLETION.md](SHARED_INTEGER_COMPLETION.md). Five further point/list/font entries and two existing-operator counting corrections are documented in [SHARED_GEOMETRY_COMPLETION.md](SHARED_GEOMETRY_COMPLETION.md). The global denominator is unchanged; the tribute iterator is now counted in missions and the rectangle equality in shared basics. Additional shared progress is documented in [SHARED_HISTORY_HEAP_COMPLETION.md](SHARED_HISTORY_HEAP_COMPLETION.md). Additional shared progress is documented in [SHARED_STARTUP_GUI_COMPLETION.md](SHARED_STARTUP_GUI_COMPLETION.md). Additional shared progress is documented in [SHARED_MANTISSA_LIST_COMPLETION.md](SHARED_MANTISSA_LIST_COMPLETION.md). Additional shared progress is documented in [SHARED_DISPLAY_COMPLETION.md](SHARED_DISPLAY_COMPLETION.md). Additional shared progress is documented in [SHARED_EXIT_ITERATOR_COMPLETION.md](SHARED_EXIT_ITERATOR_COMPLETION.md). The engine category is **engine/graphics/weather/particles (127 open)**, including rendering, weather overlays and initialization. System/shared retains numeric/runtime helpers and unidentified entries. Computer AI retains task descriptions/conditions, single-player and high-level update/repair, village/scout/diplomacy, notifications and enclosing callbacks. Local formation/danger leaves do not complete that broader AI behavior.

Earlier GUI, human, mission and game-data work remains documented in [GUI_REMAINDER_COMPLETION.md](GUI_REMAINDER_COMPLETION.md), [HUMAN_ROUTINES_COMPLETION.md](HUMAN_ROUTINES_COMPLETION.md), [MISSIONS_CAMPAIGNS_COMPLETION.md](MISSIONS_CAMPAIGNS_COMPLETION.md) and [GAME_CONTROL_DATA_COMPLETION.md](GAME_CONTROL_DATA_COMPLETION.md).

The reported green road pixels remain an unverified graphics discrepancy relative to Windows. Zero inventory gaps does not establish live equivalence or absence of bugs. Terrain editing and continent splits/merges, navigation, GUI/human gameplay, campaign playthroughs and actual music playback remain unverified.

The scan covers **284 CPP / 260 headers**, **2735** unique naming rows and **18** unmatched declaration signatures.Six affected/integrating units pass MSVC Win32 `/Zs` without diagnostics. Project/filter parity, naming uniqueness, original selection accounting, category sums and whitespace checks pass. No build, tests or game run occurred. Private scripts and full inventories stay under ignored `.codex-work/`.
