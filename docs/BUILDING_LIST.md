# Building list and defense checkpoint

The building list covers all 55 logic entries, including the None sentinel and 54 actual building types. IDs, German names, worker/capacity pairs, production goods and vehicle results are inventoried in BUILDING_LIST.csv. These values are read from the installed Windows game assets; they are runtime configuration, not guessed constants added to C++. Every row lists its current source areas and remaining dependencies. No building type is claimed runtime-verified.

## Evidence and scope

Windows assembly/Game.exe remain authoritative for behavior. The Mac header supplies names only. The Windows constructor 004149B2 and statistics list 004B28C8 establish the 55-entry bound; 0042F747 establishes singular/plural string indexing. The installed assets are data/logic/housetypes.cif, data/text/ger/strings/gameobjects/houses.cif and data/GameSourceIncludes/logicdefines.inc. The CIF string-array loading/decryption sequence was checked at 0042BB02, 0042F91A and 00461F37 before extracting metadata.

The installed Steam Game.exe SHA-256 differs from the assembly-reference Game.exe (6582a493406c1b0c3badc624ec88e39745435983ec9d2c6a0b313092e54ad870 versus 270e82f454e186ef9a70c90eddf5b5b2eefdfc16e62019bd1c7ef5c0466898cf). Installed assets provide only names/configuration for this list; behavioral reconstruction continues to use the assembly-reference executable. No binary equivalence or version equivalence is assumed.

## Connected building defense

The remaining 00433694 branch and complete 00433A9D arrow routine are reconstructed. Defender count comes from the civilian sector list at the door with radius zero. Shots per tick are count/24 plus one while count%24 exceeds tick%24. Native searches run even with a zero shot budget and follow this order: military humans, first-list vehicles, aggressive animals, first-list houses, civilian humans, second-list vehicles, second-list houses. A search pass repeats only if it fired and still has shots left. The native five-nearest/threshold-10 selection and random-call order remain intact.

Weapon 20, optional randomized launch-point graphics, signed low-word y-before-x scatter, staggered map offsets, flight time, callback 0x23 and delayed attacker-type-4 damage scheduling follow Windows. Building shots do not gain a new exploration side effect or moving-target prediction. Destruction, vehicle-site conversion and first-contact reports retain their order before defense. The delayed manager applies real damage. The later particle checkpoint connects projectile/hit/smoke callbacks; broader AI group notification bodies remain open dependencies.

The existing GUI building statistics list was compared with 004B28C8..004B2A48: it already enumerates/sorts 55 types, distinguishes finished/unfinished counts, restores scroll offset, and refreshes its timestamp. It required no behavioral change in this checkpoint.

## Type inventory

| ID | Windows building name | Main type | Workers: job/capacity |
|---:|---|---|---|
| 0 | <Kein> | None | - |
| 1 | Hauptquartier | Stock | 24 3;8 3;22 3;15 3 |
| 2 | Wohnhaus 1 | Home | - |
| 3 | Wohnhaus 2 | Home | - |
| 4 | Wohnhaus 3 | Home | - |
| 5 | Wohnhaus 4 | Home | - |
| 6 | Wohnhaus 5 | Home | - |
| 7 | Lagerhaus 1 | Stock | 24 3;8 3;22 3;15 3 |
| 8 | Lagerhaus 2 | Stock | 24 3;8 3;22 3;15 3 |
| 9 | Lagerhaus 3 | Stock | 24 3;8 3;22 3;15 3 |
| 10 | Brunnen | Work | 24 1 |
| 11 | Bienenstock | Work | 24 1 |
| 12 | Farm | Work | 18 4;24 1 |
| 13 | Mühle | Work | 19 2;24 1 |
| 14 | Bäckerei 1 | Work | 20 1;24 1 |
| 15 | Bäckerei 2 | Work | 20 2;24 1 |
| 16 | Brauerei | Work | 21 2;24 1 |
| 17 | Viehzüchterei | Work | 16 2;24 1 |
| 18 | Näherei 1 | Work | 17 1;24 1 |
| 19 | Näherei 2 | Work | 17 2;24 1 |
| 20 | Töpferei 1 | Work | 11 1;24 1;8 1 |
| 21 | Töpferei 2 | Work | 11 2;24 1;8 2 |
| 22 | Töpferei 3 | Work | 11 3;24 1;8 2 |
| 23 | Schreinerei 1 | Work | 9 1;24 1;8 1 |
| 24 | Schreinerei 2 | Work | 9 2;24 1;8 1 |
| 25 | Schreinerei 3 | Work | 9 3;24 1;8 1 |
| 26 | Schreinerei 4 | Work | 9 3;24 1;8 1 |
| 27 | Waffenhütte 1 | Work | 10 1;24 1;8 1 |
| 28 | Waffenhütte 2 | Work | 10 2;24 1;8 2 |
| 29 | Steinmetzhütte 1 | Work | 12 1;24 1;8 1 |
| 30 | Steinmetzhütte 2 | Work | 12 2;24 1;8 1 |
| 31 | Schmiede 1 | Work | 13 1;24 1;8 2 |
| 32 | Schmiede 2 | Work | 13 2;24 1;8 2 |
| 33 | Münzpräge | Work | 14 2;24 1;8 2 |
| 34 | Kräuterhütte | Work | 29 3;24 1 |
| 35 | Alchemistenhütte 1 | Work | 30 1;24 1;8 1 |
| 36 | Alchemistenhütte 2 | Work | 30 2;24 1;8 2 |
| 37 | Tempel | Work | - |
| 38 | Schule | Learn | - |
| 39 | Kaserne | Learn | 24 3 |
| 40 | Wachturm 1 | Fight | 40 3;41 3;24 3 |
| 41 | Wachturm 2 | Fight | 40 4;41 4;24 4 |
| 42 | Handkarre | Vehicle | 24 3 |
| 43 | Ochsenkarre | Vehicle | 24 3 |
| 44 | Schiff | Vehicle | 24 3 |
| 45 | Schiff | Vehicle | 24 3 |
| 46 | Katapult | Vehicle | 24 3 |
| 47 | Leuchtturm | Wonder | - |
| 48 | Garten | Wonder | - |
| 49 | Kolloss | Wonder | - |
| 50 | Mausoleum | Wonder | - |
| 51 | Artemis Tempel | Wonder | - |
| 52 | Zeus Statue | Wonder | - |
| 53 | Pyramide | Wonder | - |
| 54 | 8te Wunder | Wonder | - |

## Current completion checkpoint - 2026-10-10

All 116 known entry candidates in 00430FCE..00434CCF now have registered bodies, including the separately outlined helpers previously listed here. Shared house logic, root/door registration, holy fire, worker/sector access, building map overlays and tooltip/debug drawing are connected. Ten building goal branches and seven building result branches now support stock/construction/discovery/area checks, extension replacement, damage, onscreen counting and overlay/behavior flags. See [BUILDING_COMPLETION.md](BUILDING_COMPLETION.md) for addresses and preserved Windows semantics.

The existing particle creation/tick/rendering block supplies building projectile/hit/smoke callbacks. General AI notifications, vehicle/sea consumers and remaining nonbuilding campaign predicates/actions are still open. No building type is claimed runtime-verified. Whole-project source-address coverage is 4157 / 5062 (82.12%), not a gameplay-completion percentage.

## Validation

Affected production units and house/damage/GUI consumers passed MSVC Win32 /Zs syntax-only checking. Project/filter parity, unique naming addresses and whitespace were checked. No build, tests or game run occurred. User-owned untracked C2HumanCombatTasks files remain untouched and excluded.
