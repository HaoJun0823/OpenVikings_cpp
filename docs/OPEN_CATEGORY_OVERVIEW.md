# Current category inventory - 2026-10-12

Fresh registered-source / Windows ASM / Game.exe counting gives **5063/5063 mapped entries (100.00%)**, **0 open**. Categories form a disjoint partition. These are native-entry source counts, not verified gameplay percentages.

## Open categories

None in the current known-entry inventory. TEMPORARY_LINK_STUBS.csv is header-only; no temporary function bodies remain.

## All categories

| Category | Mapped / known | Mapped % | Open | Open % |
|---|---:|---:|---:|---:|
| Computer-KI und Spielassistent | 319 / 319 | 100.00% | 0 | 0.00% |
| Engine, Grafik, Wetter und Partikel | 665 / 665 | 100.00% | 0 | 0.00% |
| Fahrzeuge, Schiffe und Passagiere | 185 / 185 | 100.00% | 0 | 0.00% |
| Gebaeude (Hausblock) | 116 / 116 | 100.00% | 0 | 0.00% |
| Gemeinsame Bewegung, Kampf und Objektablauf | 94 / 94 | 100.00% | 0 | 0.00% |
| Karte, Ressourcen und Wegfindung | 206 / 206 | 100.00% | 0 | 0.00% |
| Menschen, Beduerfnisse und Berufe | 585 / 585 | 100.00% | 0 | 0.00% |
| Menues, GUI und Bedienung | 1184 / 1184 | 100.00% | 0 | 0.00% |
| Missionen und Kampagnen | 74 / 74 | 100.00% | 0 | 0.00% |
| Netzwerk und Mehrspieler | 164 / 164 | 100.00% | 0 | 0.00% |
| Spielsteuerung, Laden/Speichern und Spieldaten | 466 / 466 | 100.00% | 0 | 0.00% |
| Strassen, Palisaden und Wegweiser | 116 / 116 | 100.00% | 0 | 0.00% |
| System, Laufzeit und gemeinsame Hilfen | 681 / 681 | 100.00% | 0 | 0.00% |
| Tiere | 70 / 70 | 100.00% | 0 | 0.00% |
| Ton, Musik und Videos | 138 / 138 | 100.00% | 0 | 0.00% |

## Interpretation and remaining validation

All **225** originally open AI-block entries have native implementations. Its former **321-entry** set is fully implemented; the disjoint AI category is now **319/319**. Windows evidence assigns **00408FBB** to the shared vector-constructor loop and **0044E640** to fish-manager member construction. Shared/system becomes **681/681** and map/resources **206/206**. No unresolved entry was removed or hidden by reclassification. See [AI_REMAINDER_COMPLETION.md](AI_REMAINDER_COMPLETION.md) and [AI_REMAINDER_NATIVE_ENTRIES.csv](AI_REMAINDER_NATIVE_ENTRIES.csv).

The global denominator remains **5063**. There are **0 explicit unfinished boundaries** and **0 provisional open assignments**. All earlier completion checkpoints are historical; their category totals describe their respective snapshots.

Source closure does not establish live equivalence or absence of bugs. Build/link on explicit user request, campaign playthroughs, village/resource/worker progression, soldier formations/attacks, vehicles, wall/gate/guide automation and assistant phases are the next validation block. Existing graphics, audio/video and private numeric-backend deployment/precision limits remain unverified at runtime. Unknown fixed-width layouts and member names remain reverse-engineering targets.

The user traced green road pixels to a modified PCX; the original texture has no such pixels. No renderer correction is requested for that report. Original player/music data for Die Rebellion remain preserved: the user confirmed the same music in the original executable.

Project/filter parity is **307 CPP / 281 headers**, with **3151** unique naming records and **15** freshly counted unmatched declaration signatures (parser diagnostics, not linker results). All **24 affected C++ units** pass MSVC Win32 C++17 syntax-only checks. Native source inventories, unique naming, dummy replacement, project/filter registration and whitespace checks pass. No build, tests or game run occurred. Private tooling and full inventories remain in ignored `.codex-work/`.

## Naming checkpoint - 2026-10-12

All five active FUN identifiers and 17 of 21 DAT identities are cleaned. All remaining DAT members of the game-state struct are named; four globals have only proven initialization and remain unresolved. Registered production identifiers are FUN 0 / DAT 4 / PTR 0 / UNK 0; address comments remain. This does not change reconstruction counts or establish runtime equivalence. See [SYMBOL_NAMING_PASS.md](SYMBOL_NAMING_PASS.md).

## Subsequent user-authorized cleanup - 2026-10-12

The four initialization-only globals and their writes are removed at explicit user request, including the standalone clear-only startup callback. Active FUN / DAT / PTR / UNK identifier counts are now all zero. Original slot purposes remain unknown. This intentionally departs from the original writes; it is not a newly reconstructed function or evidence of live equivalence. See [SYMBOL_NAMING_PASS.md](SYMBOL_NAMING_PASS.md).
