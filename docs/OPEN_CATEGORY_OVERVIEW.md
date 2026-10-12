# Current category inventory - 2026-10-12

Fresh registered-source / Windows ASM / Game.exe counting gives **4695/5063 mapped entries (92.73%)**, **368 open (7.27%)**. Categories form a disjoint partition. Temporary link dummies and explicitly unfinished enclosing functions are excluded from mapped entries. These are native-entry source counts, not verified gameplay percentages.

## All open categories

| Category | Mapped / known | Mapped % | Open | Open % |
|---|---:|---:|---:|---:|
| Engine, Grafik, Wetter und Partikel | 556 / 680 | 81.76% | 124 | 18.24% |
| System, Laufzeit und gemeinsame Hilfen | 651 / 670 | 97.16% | 19 | 2.84% |
| Computer-KI und Spielassistent | 96 / 321 | 29.91% | 225 | 70.09% |

## Categories with no open entries in the current inventory

| Category | Mapped / known | Mapped % | Open | Open % |
|---|---:|---:|---:|---:|
| Fahrzeuge, Schiffe und Passagiere | 185 / 185 | 100.00% | 0 | 0.00% |
| Gebaeude (Hausblock) | 116 / 116 | 100.00% | 0 | 0.00% |
| Gemeinsame Bewegung, Kampf und Objektablauf | 93 / 93 | 100.00% | 0 | 0.00% |
| Karte, Ressourcen und Wegfindung | 201 / 201 | 100.00% | 0 | 0.00% |
| Menschen, Beduerfnisse und Berufe | 585 / 585 | 100.00% | 0 | 0.00% |
| Menues, GUI und Bedienung | 1184 / 1184 | 100.00% | 0 | 0.00% |
| Missionen und Kampagnen | 74 / 74 | 100.00% | 0 | 0.00% |
| Netzwerk und Mehrspieler | 164 / 164 | 100.00% | 0 | 0.00% |
| Spielsteuerung, Laden/Speichern und Spieldaten | 466 / 466 | 100.00% | 0 | 0.00% |
| Strassen, Palisaden und Wegweiser | 116 / 116 | 100.00% | 0 | 0.00% |
| Tiere | 70 / 70 | 100.00% | 0 | 0.00% |
| Ton, Musik und Videos | 138 / 138 | 100.00% | 0 | 0.00% |

## Interpretation and remaining work

**283 of 368 open entries** still have provisional broad-region assignments. The **19 explicit unfinished boundaries** are included in the 368, not added on top.

The original map selection is preserved: **92 entries = 34 completed map entries + three completed military-danger dependencies + 55 still-open AI entries**. Maps change from **167/259** to **201/201**. AI changes from **74/251** to **77/309**. Its denominator grows by 58 because all three implemented dependencies and 55 remaining AI entries previously carried the provisional map label. Reclassification leaves the global denominator at **5062** and preserves all 55 open addresses. See [MAP_REMAINDER_COMPLETION.md](MAP_REMAINDER_COMPLETION.md) and [MAP_REMAINDER_NATIVE_ENTRIES.csv](MAP_REMAINDER_NATIVE_ENTRIES.csv).

The user requests continuing shared/system helpers (19 open). Five newly reconstructed GUI string wrappers move from the provisional system inventory into GUI; all ten reconstructed entries remain in the global denominator. See [SHARED_HELPERS_CHECKPOINT.md](SHARED_HELPERS_CHECKPOINT.md). Sixteen additional string/memory primitives are reconstructed in [SHARED_STRING_MEMORY_COMPLETION.md](SHARED_STRING_MEMORY_COMPLETION.md). Fourteen further runtime wrapper/cleanup entries are reconstructed in [SHARED_RUNTIME_WRAPPERS_COMPLETION.md](SHARED_RUNTIME_WRAPPERS_COMPLETION.md). Five additional integer/search/classification leaves are reconstructed in [SHARED_INTEGER_COMPLETION.md](SHARED_INTEGER_COMPLETION.md). Five further point/list/font entries and two existing-operator counting corrections are documented in [SHARED_GEOMETRY_COMPLETION.md](SHARED_GEOMETRY_COMPLETION.md). The global denominator is unchanged; the tribute iterator is now counted in missions and the rectangle equality in shared basics. Additional shared progress is documented in [SHARED_HISTORY_HEAP_COMPLETION.md](SHARED_HISTORY_HEAP_COMPLETION.md). Additional shared progress is documented in [SHARED_STARTUP_GUI_COMPLETION.md](SHARED_STARTUP_GUI_COMPLETION.md). Additional shared progress is documented in [SHARED_MANTISSA_LIST_COMPLETION.md](SHARED_MANTISSA_LIST_COMPLETION.md). Additional shared progress is documented in [SHARED_DISPLAY_COMPLETION.md](SHARED_DISPLAY_COMPLETION.md). Additional shared progress is documented in [SHARED_EXIT_ITERATOR_COMPLETION.md](SHARED_EXIT_ITERATOR_COMPLETION.md). Additional shared progress is documented in [SHARED_AI_CONDITIONS_COMPLETION.md](SHARED_AI_CONDITIONS_COMPLETION.md). Additional shared progress is documented in [SHARED_FILE_EXCEPTION_COMPLETION.md](SHARED_FILE_EXCEPTION_COMPLETION.md). Additional shared progress is documented in [SHARED_RESOURCE_LISTS_COMPLETION.md](SHARED_RESOURCE_LISTS_COMPLETION.md). Additional shared progress is documented in [SHARED_TASK_PRECISION_COMPLETION.md](SHARED_TASK_PRECISION_COMPLETION.md). Additional shared progress is documented in [SHARED_NUMERIC_RECORDS_COMPLETION.md](SHARED_NUMERIC_RECORDS_COMPLETION.md). Additional shared progress is documented in [SHARED_X87_STATUS_CLEAR.md](SHARED_X87_STATUS_CLEAR.md). Additional shared progress is documented in [SHARED_RUNTIME_REMAINDER_AUDIT.md](SHARED_RUNTIME_REMAINDER_AUDIT.md). Additional shared progress is documented in [SHARED_GEOMETRY_COPY_COMPLETION.md](SHARED_GEOMETRY_COPY_COMPLETION.md). Additional shared progress is documented in [SHARED_NOTIFICATION_STATUS_COMPLETION.md](SHARED_NOTIFICATION_STATUS_COMPLETION.md). Additional shared progress is documented in [SHARED_EXPONENT_NOTIFICATION_COMPLETION.md](SHARED_EXPONENT_NOTIFICATION_COMPLETION.md). Additional shared progress is documented in [SHARED_IEEE_EXCEPTION_COMPLETION.md](SHARED_IEEE_EXCEPTION_COMPLETION.md). Additional shared progress is documented in [SHARED_CURRENT_REMAINDER.md](SHARED_CURRENT_REMAINDER.md). Additional shared progress is documented in [SHARED_CONTROL_CLASSIFICATION_UNWIND.md](SHARED_CONTROL_CLASSIFICATION_UNWIND.md). Additional shared progress is documented in [SHARED_MATH_ERROR_DISPATCH.md](SHARED_MATH_ERROR_DISPATCH.md). Additional shared progress is documented in [SHARED_MASKED_RESPONSE_VALUE_PATHS.md](SHARED_MASKED_RESPONSE_VALUE_PATHS.md). Additional shared progress is documented in [SHARED_X87_COMPARISON_COMPLETION.md](SHARED_X87_COMPARISON_COMPLETION.md). Additional shared progress is documented in [SHARED_ADJUSTED_MATH_BACKEND.md](SHARED_ADJUSTED_MATH_BACKEND.md). Additional shared progress is documented in [SHARED_SCALAR_MATH_BACKEND.md](SHARED_SCALAR_MATH_BACKEND.md). The engine category is **engine/graphics/weather/particles (124 open)**, including rendering, weather overlays and initialization. System/shared retains numeric/runtime helpers and unidentified entries. Computer AI retains task descriptions/conditions, single-player and high-level update/repair, village/scout/diplomacy, notifications and enclosing callbacks. Local formation/danger leaves do not complete that broader AI behavior.

Earlier GUI, human, mission and game-data work remains documented in [GUI_REMAINDER_COMPLETION.md](GUI_REMAINDER_COMPLETION.md), [HUMAN_ROUTINES_COMPLETION.md](HUMAN_ROUTINES_COMPLETION.md), [MISSIONS_CAMPAIGNS_COMPLETION.md](MISSIONS_CAMPAIGNS_COMPLETION.md) and [GAME_CONTROL_DATA_COMPLETION.md](GAME_CONTROL_DATA_COMPLETION.md).

The user traced the green road pixels to a modified PCX asset; the original texture has no green pixels. This is an asset difference and is not an open renderer bug. Original music/player data for Die Rebellion are also preserved: the user confirmed that the original executable chooses the same music. Zero inventory gaps does not establish live equivalence or absence of bugs. Terrain editing and continent splits/merges, navigation, GUI/human gameplay, campaign playthroughs and actual music playback remain unverified.

The scan covers **287 CPP / 262 headers**, **2814** unique naming rows and **17** unmatched declaration signatures. Five integrating units pass Win32 C++17 syntax-only checks. Private unlinked /Od and /O2 outputs verify numeric primitives, FP-free bridges and native ESP workspaces; read-only code/data comparisons, fresh inventory, naming, project/filter parity and whitespace checks pass. No build, tests or game run occurred. Private scripts and full inventories stay under ignored `.codex-work/`.
