# Open Tasks

## Human attack record continuation — 2026-10-15

Completed the [human attack records review](HUMAN_ATTACK_RECORDS_REVIEW.md): footprint rows, coordinate copies, landscape-target direction/attempt fields, pursued-object moveable ID, task statuses, selectors and attack limits. All 49 raw numeric accesses in the owning file are named. Windows control flow, search ties, wrapping, callback/RNG order and vehicle bounds-check behavior are retained. **54 value/type comparisons, 22 physical assertions and incremental Win32 syntax checks** pass; no build/test/game run.

Latest scan: **595 registered files (307 CPP / 288 headers), 155,024 lines, 3,023 raw numeric-access operands, 74 heuristic unknown mentions, 1,050 long compound lines and 5,014 original-address annotations**. Cumulative reduction **2,404 / 5,427 (44.30%)**, twenty-eight completed naming/layout blocks. These are refactoring heuristics, not missing functions or gameplay completion. The broad request and full per-function review remain open. Independent pending documentation is preserved. Next owning block: human job windows (48 raw operands), followed by multiplayer goals (47).

## Game-control startup record continuation — 2026-10-15

Completed the [game-control startup records review](GAME_CONTROL_STARTUP_RECORDS_REVIEW.md), following mission-start records (`b10fa39`). Replaced the unknown 288-byte startup block with verified fields; named player identity, diplomacy, death/message state, wealth, path counters, modes and availability counts. Windows call order, unchecked bounds, equality checks, name-copy lengths and equipment stride remain unchanged. Nineteen tokens plus four terminators match Game.exe. **50 value/type comparisons, 23 physical assertions and incremental Win32 syntax checks** pass; no build/test/game run.

Latest scan: **595 registered files (307 CPP / 288 headers), 154,836 lines, 3,072 raw numeric-access operands, 74 heuristic unknown mentions, 1,057 long compound lines and 5,014 original-address annotations**. This block removes 67 candidates; together with mission-start, this continuation removes 117. Cumulative reduction **2,355 / 5,427 (43.39%)**, twenty-seven completed naming/layout blocks. These are refactoring heuristics, not missing functions or gameplay completion. The broad request and full per-function review remain open. Independent pending documentation is preserved. Next owning block: human attack tasks (49 raw operands), then human job windows (48).

## Mission-start record continuation — 2026-10-14

Completed the [mission-start records review](MISSION_START_RECORDS_REVIEW.md): family/home/vehicle fields, age and production state, experience stride/limit, current-object kind/IDs and token identifiers. All **50** raw numeric accesses in the owning file are now named. Windows stores, unchecked indices, unsigned wrap, tie behavior and parser consumption remain unchanged. Seventeen tokens plus three terminators match the Windows PE. **50 value/type comparisons, 12 physical assertions and incremental Win32 syntax checks** pass; no build/test/game run.

Latest scan: **595 registered files (307 CPP / 288 headers), 154,699 lines, 3,139 raw numeric-access operands, 74 heuristic unknown mentions, 1,058 long compound lines and 5,014 original-address annotations**. Cumulative reduction **2,288 / 5,427 (42.16%)**, twenty-six completed naming/layout blocks. These are refactoring heuristics, not missing functions or gameplay completion. The broad request and full per-function review remain open. Independent pending docs remain preserved. Next block: game-control startup (49 raw operands), followed by human attacks (49).

## Mouse/world cursor continuation — 2026-10-14

Completed the [mouse and world cursor records block](MOUSE_CURSOR_RECORDS_REVIEW.md), following single-player AI (`30d3e9f`) and character selection (`e2e0469`). Named tooltip cache, mouse callback/button state, hardware enable, hovered-object/command fields, marker limits, frames and timing. In-game/main-menu routines keep their different hover-cache behavior and repeated callback evaluation. **51 value/type comparisons, 19 physical assertions and incremental Win32 syntax checking** pass.

Fresh scan: **595 registered files (307 CPP / 288 headers), 154,621 lines, 3,189 raw numeric-access operands, 74 heuristic unknown mentions, 1,061 long compound lines and 5,014 original-address annotations**. Three blocks this continuation remove **187** candidates; cumulative reduction **2,238 / 5,427 (41.24%)**, twenty-five naming/layout blocks. These counts are refactoring heuristics, not unfinished functions or gameplay percentages. The broad request and full per-function behavioral-review ledger remain open. This continuation's eleven changed production units and three new private assertion units pass syntax checks. No build/test/game run; independent pending documentation is preserved. Next owning block: mission-start objects (50), then game-control startup/human attacks (49 each).

## Character selection record continuation — 2026-10-14

Completed the [character selection window records block](SELECTION_WINDOW_RECORDS_REVIEW.md): trader route rows, experience pages, graphic/text control fields, tooltip limits, family selection, world reference and command payloads. The source keeps the Windows calls, arithmetic, branch order and ownership. **60 value/type comparisons, 44 physical assertions and Win32 syntax checking** pass. The preceding [single-player AI block](SINGLE_AI_RECORDS_REVIEW.md) is committed as `30d3e9f`.

Fresh scan: **595 registered files (307 CPP / 288 headers), 154,375 lines, 3,261 raw numeric-access operands, 74 heuristic unknown mentions, 1,078 long compound lines and 5,014 original-address annotations**. These two blocks remove 115 candidates. Cumulative reduction: **2,166 / 5,427 (39.91%)**, twenty-four completed naming/layout blocks. These figures count heuristic refactoring candidates, not functions or gameplay completion. The broad whole-source request and complete behavioral-review ledger remain open. Six changed production units and two new private compiler assertion units pass incrementally; no build/test/game run. Next owning block: mission-start objects or mouse cursor (50 candidates each).

## Single-player AI record continuation — 2026-10-14

Completed the [single-player AI records block](SINGLE_AI_RECORDS_REVIEW.md): initialization flags, wall/gate requests and consumers, house plans, population samples, stock/stage strides and update cadence. Windows behavior is retained, including the gate ownership/state asymmetries, unsigned population/stock arithmetic, inclusive task index and stale request flags. Frozen comparisons and Win32 syntax checking pass **85 value/type comparisons and 31 physical assertions** across five changed production units.

Latest scan: **595 registered files (307 CPP / 288 headers), 154,153 lines, 3,313 raw numeric-access operands, 74 heuristic unknown mentions, 1,091 long compound lines and 5,014 original-address annotations**. This block removes 63 candidates; cumulative reduction is **2,114 / 5,427 (38.95%)** across twenty-three naming/layout blocks. These are refactoring heuristics, not missing functions or gameplay percentages. A complete per-function behavioral-review ledger remains open. No build/test/game run; assistant-only artifacts remain ignored. Next block: selection GUI (52 raw operands), then mission-start/mouse (50 each).

## Options/pause and military range continuation — 2026-10-14

Two further blocks are committed: [options, safety dialogs, settings and pause](OPTIONS_WINDOW_RECORDS_REVIEW.md) (`b5ba513`) and [object/landscape military range storage](MILITARY_RANGE_RECORDS_REVIEW.md) (`85b3839`). The broad source review now has twenty-two completed naming/layout blocks; eight were completed in this continuation, alongside the separate confirmed title-palette correction. The request to review all remaining sites remains open.

| Block | Value/type comparisons | Physical relationships | Byte-verified instruction instances | Named references |
| --- | ---: | ---: | ---: | ---: |
| Options / safety / settings / pause / clock | 67 | 24 | 3662 | 111 |
| Military object / landscape range records | 15 | 9 | 1203 | 105 |

Latest scan: **595 registered files (307 CPP / 288 headers), 153,974 lines, 3,376 raw numeric access operands, 74 heuristic unknown-member mentions, 1,084 long compound lines and 5,014 original-address annotations**. These two blocks removed 129 further candidates; all eight blocks in this continuation removed 461. Cumulative reduction is **2,051 / 5,427 (37.79%)**. This counts heuristic numeric access operands, including normal indices and intentionally unresolved fields; it is not a missing-function count, exhaustive unresolved-field inventory or gameplay-completion percentage. A complete per-function behavioral-review ledger remains open.

Options and safety owners retain distinct page/mode fields; the manager's typed documentary view keeps unrelated spans explicitly unclassified. Expert mode is separate from scroll speed. Clock mode is analog wall-clock rendering. The speed comparison's invalid sentinel, pause/resume command order, integer truncation, unordered comparison and all four campaign branches remain native. Range records distinguish object IDs from packed points; equal distances retain discovery order, neutral animals retain player 20, actor eligibility starts at one while landscape eligibility starts at zero, and landscape stepping retains the maximum-distance-per-ring behavior. No corrective guard or common sorting implementation was invented.

Windows assembly and `Game.exe` establish the evidence; Mac supplies names only. Frozen-stage comparisons verify unchanged C++ operations, literal values and types. The latest source passes Win32 `/Zs` for **all 307 production units and 37 private compiler assertion units**, without errors or warnings. No build, test project or game run was performed. Project registration remains 307/288; assistant-only scripts/logs/assertions/handoff remain under ignored `.codex-work/`. Independent pending documentation remains untouched.

Next owning block: single-player AI and its remaining initialization/state consumers (**53** raw candidates). Other large blocks: selection GUI (52), mission-start and mouse cursor (50 each), game-control startup/human attacks (49 each), human job windows (48), multiplayer goals (47), map resources (44), house and construction-selection/needs (43 each). Do not guess identities to reduce the counter.

## Trader GUI and title-screen continuation — 2026-10-14

Two further naming/layout blocks are committed after `0301fc6`: [trader configuration and graphic buttons](TRADER_WINDOW_RECORDS_REVIEW.md) (`36cdb27`) and [title-screen FMV/picture/resource state](TITLE_SCREEN_RECORDS_REVIEW.md) (`06c3b4e`). A separately confirmed Windows translation mismatch was corrected in `120e8de`: [the title palette now uses equal RGB channels](TITLE_PALETTE_TRANSLATION_FIX.md), preserving the original grayscale values and fourth bytes. This is not attributed to the user's earlier GUI color reports.

| Block | Value/type comparisons | Layout relationships | Byte-verified instruction instances | Named references |
| --- | ---: | ---: | ---: | ---: |
| Trader configuration / graphic buttons | 86 | 35 | 2169 | 179 |
| Title-screen owner / ABI consumers | 35 | 22 | 712 | 70 |

Current scan: **595 registered files (307 CPP / 288 headers), 153,703 lines, 3,505 raw numeric access operands, 74 heuristic unknown-member mentions, 1,087 long compound lines and 5,014 original-address annotations**. These two blocks removed 140 further raw candidates; all six blocks in this continuation removed 332. Cumulative reduction from the 5,427-candidate checkpoint is **1,922 / 5,427 (35.42%)**, across twenty named/reviewed blocks. This remains a refactoring heuristic, not a missing-function count, exhaustive unresolved-field inventory or runtime-completion percentage. The whole-source review remains open and lacks a complete per-function behavioral ledger.

The trader owner's two import regions retain explicit unclassified tails; only observed goods 1–55 receive button-slot identities. Graphic/text buttons keep distinct parameter layouts and tooltip ownership. Network packing, route-slot lookup, agreement direction, parent notification and per-frame validation are preserved. The complete 232-byte title owner has named FMV flags, picture timing/resources and verified application/display/font fields. Existing requested DataX/TrueColor/post-sequence adaptations remain as HEAD and are explicitly distinguished from native Windows behavior. The unused cleared byte in the shared title region remains unclassified.

Frozen comparisons prove unchanged naming-stage operations against their respective baselines; the palette correction has a separate exact one-line/native-argument audit. Windows assembly and `Game.exe` establish behavior/layout evidence; Mac supplies names only. The latest source passes Win32 `/Zs` for **all 307 production units and 35 private compiler assertion units**, without errors or warnings. No build, test project or game run was performed; no production files were added.

Next owning block: options/settings GUI and its safety/pause helpers. Largest raw categories: options GUI and military target search (54 each), single-player AI (53), selection GUI (52), mission-start/mouse cursor (50 each), game-control startup/human attacks (49 each), human job windows (48), multiplayer goals (47). Assistant-only material remains ignored under `.codex-work/`; independent pending documentation is preserved.

## Graphics lifetimes, mission GUI and central AI continuation — 2026-10-14

Four additional naming/layout blocks are committed: [Bob-manager cache](BOB_CACHE_RECORDS_REVIEW.md) (`91612af`), [ambient sound sectors and weighted waves](AMBIENT_STORAGE_RECORDS_REVIEW.md) (`44fc596`), [mission information window and hyperlink adapter](MISSION_WINDOW_RECORDS_REVIEW.md) (`bb116e1`), and [central AI-manager ownership](AI_MANAGER_ROOT_RECORDS_REVIEW.md) (`10eefac`). This brings the ongoing source review to eighteen completed blocks; the broad request to review all remaining sites is still open.

| Block | Value/type comparisons | Layout relationships | Byte-verified instruction instances | Named references |
| --- | ---: | ---: | ---: | ---: |
| Bob-manager cache | 6 | 6 | 149 | 15 |
| Ambient storage / lifetime | 55 | 35 | 1543 | 137 |
| Mission window / hyperlink adapter | 80 | 33 | 1837 | 119 |
| AI-manager root / scheduling | 14 | 7 | 637 | 67 |

The latest scan contains **595 registered files (307 CPP / 288 headers), 153,374 lines, 3,645 raw numeric access operands, 74 heuristic unknown-member mentions and 1,087 long compound lines**. These four blocks removed **192 further raw candidates**; cumulative progress from the 5,427-candidate checkpoint is **1,782 / 5,427 (32.84%)**. Numeric indices and deliberately unclassified fields contribute to the remaining count. The scanner is a refactoring heuristic, not an exhaustive field inventory, missing-function count or runtime-completion percentage. A complete per-function Windows behavioral review ledger is still absent.

The complete cache payload/manager, ambient manager/sector/wave storage, mission-window suffix, hyperlink element and two AI-handler pointer arrays have physical models with explicit unresolved bytes. Named cache fields distinguish owning payload pointers from borrowed fish/bubble pointers. Ambient pattern loops retain the first-wave view and landscape selection retains weighted waves. Mission previews, independent goal strings, callback data, signed camera averaging and fifty-entry history keep their original lifetimes and data widths. The AI manager retains its paired-handler ownership gate, save versions and sixty-/three-tick schedule; task/parser payloads and complete handler internals remain separate open work.

Windows assembly and `Game.exe` establish the documented layouts/uses; Mac declarations supply names only. Frozen comparisons verify unchanged C++ operations, literal values and literal types against each block's baseline. This evidence does not establish complete runtime equivalence for the whole project.

Win32 `/Zs` passed for **all 307 production units and 33 private compiler assertion units**, without errors or warnings. No build, test project or game execution was performed. Production registration remains 307/288; no production files were added. Assistant scripts, assertion units, logs and handoff remain under ignored `.codex-work/`. Independent pending changes remain outside these commits.

Next complete owning block: trader configuration GUI (**56** raw candidates). Other large blocks are title screen (55), options and military target search (54 each), single-player AI (53), selection GUI (52), mission-start and mouse cursor (50 each), game-control startup and human attack tasks (49 each). Do not guess semantic identities to reduce the count.

## Goods, animal/diplomacy and speech continuation — 2026-10-14

Three further blocks after `e0ef520` are committed: [global/local AI goods planning](AI_GOODS_PLANNING_RECORDS_REVIEW.md) (`4c25fce`), [animal catcher and diplomacy](AI_ANIMAL_DIPLOMACY_RECORDS_REVIEW.md) (`f47c7ab`) and [human speech/static wave storage](HUMAN_SOUND_STORAGE_RECORDS_REVIEW.md) (`402e98c`). The current continuation has completed **fourteen blocks**, while the request to review all remaining roughly five thousand sites is still open.

| Block | Value/type comparisons | Physical relationships | Byte-verified instruction instances | Named references |
| --- | ---: | ---: | ---: | ---: |
| AI goods planning | 71 | 25 | 1114 | 251 |
| Animal catcher / diplomacy | 26 | 19 | 559 | 85 |
| Human speech / wave storage | 28 | 17 | 813 | 68 |

Fresh scan: **595 registered files (307 CPP / 288 headers), 152,930 lines, 3,837 raw numeric access operands, 74 heuristic unknown-member mentions and 1,087 long compound lines**. This continuation has removed **1,590 / 5,427 numeric-access candidates (29.30%)**. Dynamic numeric indices and explicitly unclassified fields remain in this scan; these are not counts of missing implementations or a runtime-completion percentage. There is still no complete per-function Windows behavioral review ledger.

The local good row is forty bytes, its producer twenty and the global good row eighty; importing villages remain distinct from production candidates. Tribe-specific forced-production addresses are proved constant expressions with explicit good indices. Catcher quantities/request bytes/target/military point and diplomacy cursor/intrusion counts retain independent fields. The 3,364-byte speech manager retains the tick gate, twenty-four 140-byte rows and separate biased response view. Static wave slot extent is explicitly marked as inferred from Windows allocation/indexing; unchecked parser behavior is preserved. Windows establishes effects/layout evidence; Mac supplies names only.

The latest source state passes Win32 `/Zs` for **all 307 production units and 29 private compiler assertion units**, without errors or warnings. Project/filter registration remains 307/288 and the naming map 3,151 unique addresses. Frozen-stage operations/value/type/layout audits pass. No build, tests or game execution occurred. Private scripts/logs/handoff remain gitignored; unrelated pending changes are preserved.

Largest remaining owner blocks: engine lifetime and mission-info GUI (57 candidates each), trader GUI (56), title screen (55), single-player AI/options GUI/military target search (54 each), selection windows (52), mission-start and mouse cursor (50 each), game-control startup and human attack tasks (49 each). Continue complete owner blocks and their proved consumers; do not manufacture semantic names for unclassified data.


## Whole-source readability continuation — 2026-10-14

The request to finish the remaining roughly five thousand sites remains open. Eleven further blocks are committed after `432bbe2`; this is a partial readability and Windows-layout review, not a claim of complete behavioral equivalence.

| Block / commit | Value/type comparisons | Physical relationships | Byte-verified instruction instances | Named references |
| --- | ---: | ---: | ---: | ---: |
| [Main-menu network/lobby records](MAIN_MENU_NETWORK_RECORDS_REVIEW.md) / `907e31d` | 79 | 33 | 2384 | 244 |
| [First map moveable identity](FIRST_MAP_MOVEABLE_NAMES_REVIEW.md) / `a129ac2` | 4 | 2 | 90 | 8 |
| [Typed logic micro-cell consumers](PROVEN_LOGIC_MICRO_CONSUMERS_REVIEW.md) / `345a431` | 18 | 8 | 994 | 508 |
| [House inhabitants widget](HOUSE_INHABITANTS_WIDGET_REVIEW.md) / `a2a134c` | 28 | 18 | 732 | 77 |
| [Typed actor consumers](PROVEN_ACTOR_CONSUMERS_REVIEW.md) / `4a3ee7f` | 29 | 0 | 7255 | 161 |
| [Text widgets/buttons](GUI_TEXT_RECORDS_REVIEW.md) / `81845c3` | 36 | 29 | 774 | 81 |
| [AI guides/scouts/grid/lifetime](AI_GUIDE_RECORDS_REVIEW.md) / `0449466` | 46 | 21 | 1138 | 135 |
| [Proven coordinate consumers](PROVEN_MAP_COORDINATES_REVIEW.md) / `b0c235f` | 3 | 4 | 70 | 253 |
| [GUI widget consumers](GUI_WIDGET_CONSUMERS_REVIEW.md) / `243dd85` | 8 | 0 | 1505 | 64 |
| [Remaining macro-map storage](MACRO_MAP_REMAINING_RECORDS_REVIEW.md) / `ca476a5` | 26 | 36 | 3288 | 94 |
| [Engine map owner/cells](ENGINE_MAP_STATIC_RECORDS_REVIEW.md) / `4a4532e` | 29 | 21 | 1792 | 117 |

Fresh registered-source scan: **595 files (307 CPP / 288 headers), 152,507 lines, 4,061 raw numeric access operands, 74 heuristic unknown-member mentions and 1,084 long compound lines**. The current continuation removed **1,366 / 5,427 numeric-access candidates (25.17%)**. Dynamic array-index operands also contribute to this scan. The count is neither a reviewed-function denominator nor a gameplay-completion percentage. Explicit unclassified fields are retained even where the heuristic does not count their spelling.

Windows evidence corrected a prior semantic error: logic micro-cell WORD `0x0A` is the first map-moveable list ID, not a fog mask. Its iterator, linkage and save-chunk identities are synchronized; visibility behavior is unchanged. The fixed text terminator at `0xF7` is likewise identified from the 199-character Windows copy. The guide-manager destructor at `00429546` now has its confirmed owner name, and the function-name/shutdown inventories are synchronized. Mac material supplies names only.

All **307 production units and 26 private compiler assertion units** pass Win32 `/Zs` without errors or warnings. Project/filter registration remains **307 CPP / 288 headers** and the function-name map retains **3,151 unique addresses**. Per-block frozen-stage/value/type/layout checks pass; native byte comparisons establish evidence provenance, not full runtime equivalence. No build, tests or game execution occurred.

Remaining high-count owner blocks: engine lifetime (62 candidates), global AI goods (58), mission-info GUI (57), AI production (56), trader configuration GUI (56), title screen (55), single-player AI / animal diplomacy / options GUI / military target search (54 each), selection windows (52) and mission starting objects (50). Continue complete owner blocks and then their proven consumers; do not replace uncertain offsets with guessed names. A per-function Windows behavioral review ledger is still absent.


## Network, missions, vehicles, GUI support and pathfinder continuation - 2026-10-14

Five further owning blocks are completed and committed: network commands/checksum queues (`1308184`), mission fixed-field consumers/player statistics (`ca6f7cf`), vehicle simulation/passenger and target records (`98bde8e`), selection groups/retained-human records and their save/message consumers (`b6fbc38`), and pathfinder fields/search budgets/directions (`03c1074`). Evidence: [NETWORK_COMMAND_RECORDS_REVIEW.md](NETWORK_COMMAND_RECORDS_REVIEW.md), [MISSION_CONSUMER_FIELDS_REVIEW.md](MISSION_CONSUMER_FIELDS_REVIEW.md), [VEHICLE_SIMULATION_FIELDS_REVIEW.md](VEHICLE_SIMULATION_FIELDS_REVIEW.md), [GUI_SUPPORT_RECORDS_REVIEW.md](GUI_SUPPORT_RECORDS_REVIEW.md), and [PATHFINDER_SEARCH_FIELDS_REVIEW.md](PATHFINDER_SEARCH_FIELDS_REVIEW.md). The wider whole-source readability and Windows behavior review remains incomplete.

| Block | Value/type comparisons | Physical relationships | Byte-verified instruction instances | Named references |
| --- | ---: | ---: | ---: | ---: |
| Network command records | 67 | 20 | 1286 | 130 |
| Mission consumer fields | 62 | 16 | 6141 | 131 |
| Vehicle simulation | 71 | 20 | 2769 | 140 |
| GUI support records | 70 | 27 | 1402 | 180 |
| Pathfinder search fields | 46 | 34 | 3132 | 100 |

Each block has exact frozen-baseline/stage/source-token checks, literal/type and physical layout assertions, Windows instruction-byte provenance and targeted Win32 `/Zs` checks. A final check of all 307 production translation units plus fifteen private assertion units passed without errors or warnings. The new direction names additionally use both complete eight-entry Windows cursor tables; inline jump tables are excluded from instruction counts. No build, tests or game execution occurred. Project/filter parity remains 307 CPP / 288 headers and the naming map has 3151 unique addresses. The independent CFile bounds/prefix-move commit `b223fff` is preserved.

Fresh scan: 595 registered files, 151382 lines, **5427 raw numeric access operands**, **75 unknown-member mentions**, 1095 long multi-statement lines and 5014 native address annotations. This removes 406 candidates from 5833. Relative to the initial 14730, 9303 candidates are removed (63.16% text-scan reduction). These are review candidates, not missing-function counts or an overall completion percentage; numeric mission parameter-array indices can remain flagged after fixed offsets are named. The known-entry reconstruction ledger remains 5063/5063 and does not certify full behavioral equivalence. No complete reviewed-function x/y ledger exists.

Next file-level candidates: main-menu commands 75; engine lifetime, AI guides and engine macro maps 72 each; map resources and terrain access 71 each; military target search 70; human attack tasks 69; house-inhabitants GUI 68; animal tasks 65; single-player AI and house access 61 each. Preserve documented unresolved fields and original Windows behavior. New explicit physical targets include the group manager's four-byte prefix and retained-human record bytes `2C..2F`; the one-byte input static region and callback selector `2E` also remain unresolved. Mission parameter meanings/repath thresholds require further review. Do not infer identities from Mac layouts or claim naming as runtime completion.

## Military AI and vehicle inhabitants continuation - 2026-10-14

Two further owner blocks are committed: military AI equipment/targets (`40dd7c5`) and both vehicle inhabitants widgets (`a7b644f`). See [AI_MILITARY_CONSUMERS_REVIEW.md](AI_MILITARY_CONSUMERS_REVIEW.md) and [VEHICLE_INHABITANTS_FIELDS_REVIEW.md](VEHICLE_INHABITANTS_FIELDS_REVIEW.md). Together with the eight blocks below, this continuation has completed ten further blocks. The broad whole-source review remains incomplete.

Military AI: 50 value/type comparisons, 28 physical layout relationships, 2679 byte-verified native instructions and 133 named references. Its complete 8040-byte manager, 12-byte weapon/armor rows and distinct village views are described. The sole remaining raw field is the explicitly unresolved DWORD at `1454`; only its initialization to `-1` is established. Vehicle widgets: 21 value/type comparisons, 17 layout assertions, 769 byte-verified instructions and 103 references; all 87 raw accesses are named. Both 756-byte widgets and thirty 16-byte rows retain distinct functions/tables and original ownership. Compound function lines are split with unchanged normalized tokens. The existing x87/double precision limitation is preserved and documented.

Both affected production units and private assertion units pass Win32 `/Zs`. The earlier all-307-production-unit plus ten-private-unit check at `f156cb2` passed without errors/warnings; selection and these later blocks pass targeted checks. No build, tests or game execution occurred. Project/filter registration remains unchanged.

Fresh scan: 595 files, 150693 lines, **5833 raw numeric access operands**, **75 unknown-member mentions**, 1090 long multi-statement lines and 5014 native address annotations. This removes 177 additional candidates from 6010, or 906 since the prior 6739 session checkpoint. Initial reduction: 8897/14730 (60.40% text-scan reduction), not a functional-completion percentage. No complete reviewed-function x/y ledger exists.

Next: mission execution 84, network commands 82, GUI support/vehicle simulation 81 each, pathfinder/main-menu commands 75 each, then remaining lifetime/guide/macro-map/resource consumers. Preserve all documented unresolved fields and original Windows behavior.

## AI, graphics, actor and input review checkpoint - 2026-10-14

Eight further blocks are completed and committed through `a2e9ad6`: AI movement/no-way corrections (`b2b0df6`), an actual house-door overlay correction (`d9c945a`), complete house graphics/fire/selection records (`2091dbc`), AI condition records and script producers (`4bb2ac6`), remaining human goods consumers (`e824c51`), merchant routes/agreements with ownership and save/load (`7d7b807`), human initialization/global suppression flags (`f156cb2`), and remaining selection/input consumers (`a2e9ad6`). Evidence: [AI_MOVEMENT_FIELDS_REVIEW.md](AI_MOVEMENT_FIELDS_REVIEW.md), [HOUSE_DOOR_OVERLAY_FLAG_FIX.md](HOUSE_DOOR_OVERLAY_FLAG_FIX.md), [ENGINE_HOUSE_RECORDS_REVIEW.md](ENGINE_HOUSE_RECORDS_REVIEW.md), [AI_CONDITION_RECORDS_REVIEW.md](AI_CONDITION_RECORDS_REVIEW.md), [HUMAN_GOODS_FIELDS_REVIEW.md](HUMAN_GOODS_FIELDS_REVIEW.md), [MERCHANT_RECORDS_REVIEW.md](MERCHANT_RECORDS_REVIEW.md), [HUMAN_INITIALIZATION_FIELDS_REVIEW.md](HUMAN_INITIALIZATION_FIELDS_REVIEW.md), and [SELECTION_CONSUMER_FIELDS_REVIEW.md](SELECTION_CONSUMER_FIELDS_REVIEW.md).

The door loader previously wrote stage-enabled BYTEs at `258+stage*12`, while Windows writes `24C+stage*12` and drawing reads that location. The corrected base restores the original activation slots without shifting into the next stage/layer block. Other naming changes preserve exact baseline source operations after resolving constants and original literal types. Mac is used only for names; unknown domains remain explicit.

All 307 production translation units and ten existing private GUI/AI assertion units pass Win32 `/Zs` at `f156cb2`, without errors or warnings. The subsequent selection/input unit and private assertions pass their targeted check. Every new naming block has its own exact source-stage audit, literal/type/layout assertions and Game.exe instruction-byte provenance checks. No build, tests or game execution occurred. Project/filter parity is 307 CPP / 288 headers; the function-name map still has 3151 unique addresses.

Fresh scan: 595 files, 150301 lines, **6010 raw numeric access operands**, **75 unknown-member mentions**, 1094 long multi-statement lines and 5014 native address annotations. This removes 729 candidates from the preceding 6739 checkpoint. Relative to the initial 14730, 8720 candidates have been removed (59.20% text-scan reduction). Unknown mentions rise by one because the opaque game-static tail is split around two confirmed BYTE flags; the unresolved bytes remain intact. These numbers are review candidates, not unfinished functions or an overall completion percentage. The known-entry implementation inventory remains 5063/5063; no complete reviewed-function x/y ledger exists and full behavioral equivalence is not established.

Next remaining file-level candidates: military AI 91, vehicle inhabitants GUI 87, mission execution 84, network commands 82, GUI support 81, vehicle simulation 81, pathfinder search 75, main-menu commands 75, engine lifetime 72, AI guides 72, engine macro maps 72 and map-resource managers 71. Explicit unresolved targets include experience slot 70, seven-entry selection domain, single-good search byte 10, house arrow-slot parameter, merchant house tail, player allowance +3B52, build-point byte 5, statistics/map tails and unclassified callback/payload bytes. Whole-source readability and Windows behavior review remains incomplete. Continue complete owning blocks without guessing names or changing original behavior.

## GUI, village AI, mission and map record review checkpoint - 2026-10-13

Completed twelve further naming/review blocks: village/resource AI, human selection window, shared GUI inheritance fields, world-display widget, vehicle window, message manager, input selection consumers, building/help windows, list boxes/technology tree, diplomacy/inhabitants/statistics windows, mission records, and logic map/continent records. See [VILLAGE_RESOURCE_RECORDS_REVIEW.md](VILLAGE_RESOURCE_RECORDS_REVIEW.md), [HUMAN_SELECTION_RECORDS_REVIEW.md](HUMAN_SELECTION_RECORDS_REVIEW.md), [SHARED_GUI_RECORDS_REVIEW.md](SHARED_GUI_RECORDS_REVIEW.md), [WORLD_GUI_RECORDS_REVIEW.md](WORLD_GUI_RECORDS_REVIEW.md), [VEHICLE_WINDOW_RECORDS_REVIEW.md](VEHICLE_WINDOW_RECORDS_REVIEW.md), [GUI_MESSAGE_RECORDS_REVIEW.md](GUI_MESSAGE_RECORDS_REVIEW.md), [INPUT_CONSUMER_RECORDS_REVIEW.md](INPUT_CONSUMER_RECORDS_REVIEW.md), [HOUSE_HELP_RECORDS_REVIEW.md](HOUSE_HELP_RECORDS_REVIEW.md), [LIST_TECH_RECORDS_REVIEW.md](LIST_TECH_RECORDS_REVIEW.md), [MISC_WINDOW_RECORDS_REVIEW.md](MISC_WINDOW_RECORDS_REVIEW.md), [MISSION_RECORDS_REVIEW.md](MISSION_RECORDS_REVIEW.md) and [LOGIC_MAP_RECORDS_REVIEW.md](LOGIC_MAP_RECORDS_REVIEW.md). Production blocks are committed through `b609cfc`.

The all-307-production-unit Win32 `/Zs` check at `b52b2ef`, plus ten private GUI/AI assertion units, passed without errors or warnings. Later mission/map changes pass affected-unit checks and their literal/layout assertions. Every block has exact baseline/stage/source-token comparisons, original literal type checks and Game.exe instruction-byte provenance checks. No build, tests or game execution occurred. Project/filter parity remains 307 CPP / 288 headers, and the naming map has 3151 unique addresses. Independent CRT metadata/MMX and GUI warning fixes in newer HEAD commits are preserved.

Fresh scan: 595 registered files, 149494 lines, **6739 raw numeric access operands**, **74 unknown-member mentions**, 1063 long multi-statement lines and 5014 native address annotations. This removes 1636 candidates from the preceding 8375 checkpoint. Relative to the initial 14730, the reduction is 7991 (54.25% text-scan reduction). These are review candidates, not missing-function counts or a whole-review completion percentage. The known-entry reconstruction inventory remains 5063/5063 with zero tracked unfinished implementations; that does not prove complete behavioral equivalence. No complete reviewed-function x/y ledger exists for this pass.

Next priorities by remaining raw operands: AI movement messages 98, GUI selection access 95, engine house drawing 94, human goods 92, military AI 91, AI conditions 89, human initialization 87, vehicle inhabitants GUI 87, mission predicates/results 86, human merchant access 84, network commands 82 and GUI support 81. Preserve explicit unresolved regions: player allowance +0x3B52, build-point byte 5, statistics +0xDC tail, map +0xFE0 tail and unclassified filter/payload bytes. Whole-source readability/Windows behavior review remains incomplete.

## Goods, guides, construction and human capabilities review checkpoint - 2026-10-13

Completed and committed three further record naming/review blocks: human goods search and guide routing/ownership (`d2fbc3b`), road/wall build queues, workers, serialization and consumers (`27884cc`), and human capability/player allowance/notification fields (`1b4bd95`). See [GOODS_GUIDE_RECORDS_REVIEW.md](GOODS_GUIDE_RECORDS_REVIEW.md), [WALL_ROAD_RECORDS_REVIEW.md](WALL_ROAD_RECORDS_REVIEW.md) and [HUMAN_CAPABILITY_RECORDS_REVIEW.md](HUMAN_CAPABILITY_RECORDS_REVIEW.md). Windows behavior and original literal types are retained; explicit unresolved regions remain documented.

All affected production translation units and the block-specific literal/layout assertion units pass targeted Win32 `/Zs`. Audits cover 84/50/68 literal value/type pairs, 23/23/31 layout assertions and 5326/2280/2139 byte-verified native instructions respectively. Source token comparisons retain original operations after resolving constants. The preceding all-307-unit syntax checkpoint is historical; this pass ran the affected units. No build, tests or game execution occurred. Project/filter parity remains 307 CPP / 288 headers, with 3151 unique naming-map addresses.

Fresh scan: 595 registered files, 146144 lines, **8375 raw numeric access operands**, **74 unknown-member mentions**, 1219 long multi-statement lines and 5014 native address annotations. This removes 607 raw-access candidates from the preceding 8982 checkpoint. Relative to the initial 14730, 6355 raw candidates have been removed (43.14% text-scan reduction). These are review candidates, not counts of missing functions or a whole-review completion percentage. The known-entry reconstruction inventory remains 5063/5063 with zero tracked unfinished implementations; that does not prove complete behavioral equivalence. No complete reviewed-function x/y ledger exists for this pass.

Next priorities by remaining raw operands: selection window 168, village AI 163, world display 142, vehicle window 136, resource AI 132, GUI selection access 131, mission execution 116, list box 100, house window 100, tech tree 99, message manager 98 and AI movement messages 98. These are file-level text-scan counts, not subsystem function counts. Remaining explicit targets include player +0x3B52, build-entry byte 5 and other documented unknowns. Whole-source readability/Windows behavior review remains incomplete.

## Previous particle, macro-map and AI record review checkpoint - 2026-10-13

Completed the three requested naming/review blocks: particle simulation/drawing/generators/effects and ownership, all thirteen macro-map sections and capture/placement paths, and AI soldier/vehicle/main-task/combined-task/military records with their construction, message and relevant savegame consumers. See [PARTICLE_RECORDS_REVIEW.md](PARTICLE_RECORDS_REVIEW.md), [MACRO_MAP_RECORDS_REVIEW.md](MACRO_MAP_RECORDS_REVIEW.md) and [AI_RECORDS_REVIEW.md](AI_RECORDS_REVIEW.md). Windows behavior and literal types are retained; Mac was used only for original name spelling. Explicit unknowns, including military manager +0x1454, remain unresolved.

All 307 production C++ units pass Win32 `/Zs`, with five existing GUI C4805 warnings unchanged. Block-specific compiler assertions verify 156 particle, 113 macro-map, 205 AI and three token-domain value/type pairs; AI models have 38 physical layout/pool assertions. Exact staged source audits confirm unchanged token streams after constant substitution. Native instruction byte checks and semantic evidence are described in the block documents. No builds, tests or game runs occurred.

Fresh registered-source scan: 595 files (307 CPP / 288 headers), 145810 lines, 8982 remaining raw numeric access operands, 74 unknown-member mentions and 1208 long multi-statement lines. This pass removed 1358 raw-access candidates since the 10340 checkpoint. Relative to the initial 14730, the reduction is 5748 (39.02%); this is a text-scan reduction, not whole-source review completion. The 5063/5063 known-entry reconstruction inventory still has zero tracked unfinished implementations; it does not establish complete behavioral correctness. No reliable reviewed-function x/y ledger exists for this pass. The naming map remains 3151 unique addresses and project/filter parity is unchanged.

Next review priorities: human goods/job paths, road/wall construction, village/resource AI, selection/vehicle/house GUI, world display and remaining layout owners. Some raw operands are established array strides or generic helper parameters; the scan does not count unresolved functions. Whole-source readability and behavior review remains incomplete.

## Previous input cursor and whole-source review checkpoint - 2026-10-13

Named the complete input manager cursor/road-wall/300-target-point storage, preserving the unaligned cursor at AB6, relative constructor receivers, retained last-valid coordinate and original search/callback order. Seven source/header files, 26 value/type pairs and layout assertions pass targeted Win32 syntax checks; 1216 native instruction byte sequences verify against Game.exe. See [INPUT_CURSOR_LAYOUT_REVIEW.md](INPUT_CURSOR_LAYOUT_REVIEW.md). No build, tests or runtime run occurred.

The known-entry reconstruction inventory remains 5063/5063 with zero tracked unfinished implementations; this is distinct from the ongoing whole-source readability and behavior review. The refreshed registered-source scan covers 595 files (307 CPP / 288 headers) and finds 10340 raw numeric access operands, 74 unknown-member mentions and 1210 long multi-statement lines. Raw numeric accesses have decreased by 4390 from the initial 14730 (29.80%); that is a text-scan reduction, not a 29.80% completion claim for the entire review. There is not yet a complete reviewed-function ledger for this pass. The naming map still contains 3151 unique addresses.

Recently completed related blocks: weather, display scrolling/projection, object references/search hits, downstream selection/default interaction, camera/tooltips, input selection storage and draw counters. Remaining priorities include particle simulation, engine macro maps, AI soldier/military data and remaining GUI/actor/layout regions. Explicit unknowns must continue to be resolved from Windows evidence rather than guessed.


## Static moveable lookup layouts - 2026-10-13

Named the 42×56 animation/walk access tables, corrected misleading Shadow helper names to Walk, and separated the 32-byte data region from its adjacent token object. Sorting, independent random choice and frame/list destruction order are retained. Modified units and consumer plus 52 value/type pairs pass Win32 `/Zs`; 1449 native instruction bytes verify against Game.exe. No build or runtime run occurred. See [FAST_MOVEABLE_ACCESS_REVIEW.md](FAST_MOVEABLE_ACCESS_REVIEW.md). Whole-source review continues with display/weather and GUI/control/data structures.

## Human appearance data - 2026-10-13

Named base/change job graphics, weighted palette records, names, the complete 968-byte static region and color/good tables. Windows lookup differences, random/read/free order and arithmetic are retained. All 307 C++ units pass Win32 `/Zs`; final value/type checks cover 85 pairs. Native-byte provenance checks cover 1605 instructions. No build or runtime run occurred. See [HUMAN_APPEARANCE_DATA_REVIEW.md](HUMAN_APPEARANCE_DATA_REVIEW.md). Whole-source review continues with static animation/walk access structures and display/GUI/control/data layouts.

## Compound moveable animation records - 2026-10-13

Replaced generic part parameters with type-specific signed members, named modes/part types and retained parser read order, 28-byte layout, capacities and overlay ordering. Both affected C++ units, 16 value/type pairs and layout/trivial-copy assertions pass Win32 `/Zs`; 2205 native instruction bytes verify against Game.exe. No build or runtime run occurred. See [COMPOUND_MOVEABLE_ANIMATION_REVIEW.md](COMPOUND_MOVEABLE_ANIMATION_REVIEW.md). Whole-source review continues with static appearance/job-graphics data and remaining engine/GUI/control/data structures.

## Dynamic actor drawing layouts - 2026-10-13

Named human appearance, palette/name and display-state storage, common sprite-cache members and distinct human/animal/vehicle tails. Corrected the animation-cache name to atomic start count. Original copying, callback order, palette lifetime and serialization boundaries are retained. All 21 affected C++ units, 36 value/type pairs and layout assertions pass Win32 `/Zs`; 3011 native instruction bytes verify against Game.exe. No build or runtime run occurred. See [DYNAMIC_ACTOR_LAYOUT_REVIEW.md](DYNAMIC_ACTOR_LAYOUT_REVIEW.md). Whole-source review continues with compound animation records and remaining engine/GUI/control/data layouts.

## Interaction and military target records - 2026-10-13

Named 28 human interaction-overlay accesses and 29 relative target accesses with distinct 28-/12-byte formats. Original target validation, hidden/container redirection and interruption/resume order are retained. All eight affected C++ units, 32 value/type pairs and eight layout assertions pass Win32 `/Zs`; 1257 native instruction bytes verify against Game.exe. No build or runtime run occurred. See [TARGET_RECORD_LAYOUT_REVIEW.md](TARGET_RECORD_LAYOUT_REVIEW.md). Whole-source review continues with remaining task-local storage and engine/GUI/control/data layouts.

## Human task counters and sector links - 2026-10-13

Named 49 accesses to idle-start, task-failure and position-reached counters, urgent-needs state and human sector links. Shared iterator current-ID naming now covers animals, humans and vehicles. Original non-head link removal and unsigned recovery arithmetic are retained. All 13 affected C++ units, ten value/type pairs and iterator layout assertions pass Win32 `/Zs`; 2008 native instruction bytes verify against Game.exe. No build or runtime run occurred. See [HUMAN_TASK_COUNTER_REVIEW.md](HUMAN_TASK_COUNTER_REVIEW.md). Whole-source review continues with remaining overlays and engine/GUI/control/data helpers.

## Human military and assistant naming - 2026-10-13

Corrected command 42 naming to world refreshment, preserving its behavior. Named 85 military/assistant human accesses, separate targets and the ten-record assistant history. All 17 affected C++ units and 26 value/type pairs pass Win32 `/Zs`; 921 native instruction byte sequences verify against Game.exe. No build or runtime run occurred. See [HUMAN_MILITARY_AND_ASSISTANT_REVIEW.md](HUMAN_MILITARY_AND_ASSISTANT_REVIEW.md). The full-source review continues with task counters, sector links and engine/GUI/control/data layouts.

## DataX loose-asset paths - 2026-10-13

Pictures, title/pre-ingame FMV, DM2 and Mouse now use the user's current DataX directories. Seven loose picture/cursor/film assets and all 64 referenced SGT names exist; three units pass Win32 `/Zs`. No build, tests or playback occurred. Next validation: intro/mission film, pre/post pictures, hardware cursors and DirectMusic after rebuilding. See [ORIGINAL_WINDOWS_ASSET_PATHS.md](ORIGINAL_WINDOWS_ASSET_PATHS.md).

## Extras assistant-page text correction - 2026-10-13

All nine Extras assistant-page lookup expressions now use the Windows Wonders string wrapper. Native verification checks all 25 expanded lookups/IDs and 1204 instruction byte sequences; two C++ units pass Win32 `/Zs`. No build, tests or runtime run occurred. Next validation: reopen Extras, switch both tabs and check equipment-toggle labels/states. See [MENUS_KEYBOARD_COMPLETION.md](MENUS_KEYBOARD_COMPLETION.md).

## Construction-marker crash correction - 2026-10-13

The road/wall sign-ID getters now use the Windows-confirmed +0x48/+0x4C color-record fields instead of the RGB color and palette patch ID. Three relevant units pass Win32 `/Zs`; 229 native instruction byte sequences verify the constructor/getters/renderer evidence. No build, tests or runtime run occurred. Next validation is a fresh main-menu world preview and road/wall construction-marker rendering. See [GUIDE_AND_CONSTRUCTION_MARKERS.md](GUIDE_AND_CONSTRUCTION_MARKERS.md).

## Pathfinder layout review - 2026-10-13

Named 240 ordinary/sector pathfinder accesses across nine units, with separate 132-/96-byte records. Policy flags, run records, search states, calculation masks and player statistics now have Windows-supported names. Original special count-read ordering is retained. Syntax/value/type/layout and native-byte checks pass; no runtime verification occurred. See [PATHFINDER_LAYOUT_REVIEW.md](PATHFINDER_LAYOUT_REVIEW.md) and its four inventories. The full-source review remains ongoing.

## Shared movement layout review - 2026-10-13

Named 285 moveable field accesses in 37 sources, walking thresholds and pathfinder outcomes. Human/animal/vehicle tick functions remain distinct and preserve Windows callback/early-return order. Value/type and syntax checks pass; no runtime verification occurred. See [MOVEABLE_LAYOUT_REVIEW.md](MOVEABLE_LAYOUT_REVIEW.md) and its inventories. Remaining pathfinder, human task/military, engine, GUI, control/data code is still under review.

## Animal layout and state-constructor correction - 2026-10-13

Named 400 whole-animal field accesses in 25 sources. Corrected the array helper to pass animal +0x18 to the Windows state constructor, as proven by both allocation and resize call sites. Relative constructor offsets, herd/food/age/aggression/atomic fields and the slaughter sentinel are named. Syntax, value/type, layout and native-byte checks pass; no runtime verification occurred. See [ANIMAL_LAYOUT_REVIEW.md](ANIMAL_LAYOUT_REVIEW.md) and its inventories. Entire-source review remains active.

## Vehicle layout review - 2026-10-13

Named 508 vehicle scalar/base expressions in 42 sources, 40 passenger/command strides, reservation/reference members and distinct carrier commands. Requested-delta arithmetic and retained reservation amounts preserve Windows behavior. Value/type and Win32 syntax checks pass. See [VEHICLE_LAYOUT_REVIEW.md](VEHICLE_LAYOUT_REVIEW.md) and its field/access inventories. The whole-source review remains in progress.

## Human layout review - 2026-10-13

Named 1148 human scalar/base expressions in 76 sources plus 41 inventory/command strides. Family links, task/atomic state, experience, equipment condition and vehicle-builder workshop references now have Windows-supported names. All affected value/type and syntax checks pass; no build or runtime test occurred. See [HUMAN_LAYOUT_REVIEW.md](HUMAN_LAYOUT_REVIEW.md), [HUMAN_LAYOUT_FIELDS.csv](HUMAN_LAYOUT_FIELDS.csv) and [HUMAN_LAYOUT_ACCESS_MAP.csv](HUMAN_LAYOUT_ACCESS_MAP.csv). Whole-source review remains in progress; next are vehicle records and the remaining engine/GUI/control/data blocks.

## House layout block reviewed - 2026-10-13

Named 710 house scalar/base expressions in 72 source files, 88 subrecord accesses and 30 native row strides. Construction-good anonymous arrays and static worker/output-stock placeholders now have verified field names; all native structure widths are retained. The fractional stock area (0x220) is distinguished from projected stock (0x328), and 0x7CC is the mission tag. See [HOUSE_LAYOUT_REVIEW.md](HOUSE_LAYOUT_REVIEW.md), [HOUSE_LAYOUT_FIELDS.csv](HOUSE_LAYOUT_FIELDS.csv) and [HOUSE_LAYOUT_ACCESS_MAP.csv](HOUSE_LAYOUT_ACCESS_MAP.csv).

Windows Game.exe checks verify 5438 house-range instruction byte sequences and separate outlined getters. Win32 syntax-only integration and value/type/layout checks pass; no build, tests or game run occurred. Field identities are backed by Windows getter/setter/dataflow evidence; receiver identities are inferred from current C++ pointer flow. Mac behavior and offsets were not used. The whole-source review remains in progress; human/vehicle layouts are the next active block. Earlier checkpoints below are historical.

## Constant domains and whole-source review started - 2026-10-13

Removed the generic ReadableValues collection and organized its 434 unchanged definitions into five semantic headers/namespaces. See [CONSTANT_DOMAINS.md](CONSTANT_DOMAINS.md). Exact namespace/include reversal passes for all 230 integrating source/header files; all 307 production C++ units and 434 compiler value/type pairs pass Win32 syntax-only checks. Project/filter parity: 307 CPP / 288 headers. No build, tests or game run occurred.

The requested whole-source readability and Windows-ASM review remains in progress. The complete registered scope is 307 CPP / 288 headers, 143127 current lines. The initial inventory identifies 14730 numeric access operands, 94 unknown-member mentions and 1071 condensed multi-statement lines. Those are review candidates, not counts of missing functions. Object layouts and directly related helpers are being reviewed as functional blocks; none of these scan counts alone establishes behavior or meaning. The next active block is the house lifecycle/construction/stock/attachments layout and its Windows xrefs. Earlier checkpoints below are historical.

## House position components; non-exhaustive hex pass - 2026-10-13

The previous hex pass was **not exhaustive**. The user identified a missed, established coordinate component: `house + 0x1E` is the 16-bit Y component of the packed position at `0x1C`. Added `HouseOffset::MapPositionX/Y` and `DoorEntryPointX/Y` (`1C/1E`, `24/26`), retaining the whole-point `MapPosition` and `DoorEntryPoint` constants for DWORD reads and pointer references.

Corrected **63 expressions in 22 source files**, including **57 previously raw offsets** and six existing whole-point names used for word accesses. All explicit `house` accesses/pointer additions at these four hex offsets are named. The complete native constructor at `0040AAD7`, packed point store at `00431018/0043101F` and coordinate call sites establish the Windows layout. Exact source reversal to `882dd1f` and Win32 syntax-only checks for all 22 affected units plus seven compiler value/type pairs pass. No build, tests or game run occurred. See [HOUSE_POSITION_OFFSETS.csv](HOUSE_POSITION_OFFSETS.csv).

Other safely identifiable offsets may remain; the earlier 6858 count is a historical pass result, not proof that every possible name was recovered. Review component accesses and known object pointers outside the limited naming patterns before classifying remaining fields as unresolved.

## Readable hex values - 2026-10-13

Replaced **6858 occurrences in 364 source/header files**: **5298 named uses** and **1560 decimal sizes/counts/zero-one values**. Added **430 typed constants in 59 domains** in `ReadableValues.h`; reused existing switch/result identifiers. See [HEX_VALUES.md](HEX_VALUES.md) and [HEX_VALUE_NAME_MAP.csv](HEX_VALUE_NAME_MAP.csv) for scope, retained values and evidence confidence.

Exact source reversal against `37474b0` passes. All **307 production C++ units** pass Win32 syntax-only checks, with `/arch:IA32` for the CRT math module; **1206 distinct literal/replacement pairs** pass compiler value/type assertions. Five existing byte/bool comparison warnings remain unchanged. Project/filter parity: **307 CPP / 284 headers**. Windows ASM/Game.exe checks cover **120 instruction byte sequences**, six complete field getters and **274/459 static token pairs**; remaining names are distinguished as existing-call-site/layout inference. No build, tests or game run occurred.

This does not claim that every raw offset has been identified: **16380 baseline hex positions** remain, including existing named/SDK definitions, GUID/numeric tables and unresolved layouts. The next substantive naming block is Windows xref analysis of the remaining object layouts; do not assign speculative field or flag effects. The following checkpoints are historical.

## Numeric case labels replaced - 2026-10-12

All **2764/2764 numeric case labels** in **234 switches / 110 registered source files** now use scoped semantic constants. There are **2365 constants in 144 domains**, in `SwitchIdentifiers.h` and `XResultIdentifiers.h`. See [CASE_IDENTIFIERS.md](CASE_IDENTIFIERS.md) and [CASE_IDENTIFIER_MAP.csv](CASE_IDENTIFIER_MAP.csv).

Values and literal types are preserved. Reversing the case replacements and added includes reproduces all 110 prior source files exactly: switch selectors, branches, default handling, call order and fallthrough are unchanged. The registered source scan has **0 remaining numeric case expressions**. Project/filter parity is **307 CPP / 283 headers**; the function-name map remains **3151 unique addresses**.

Windows ASM/Game.exe checks verify all **600 DirectX + 100 DirectDraw** result-to-string mappings and **2324 instruction byte sequences**. Mission name tables and loader token evidence are recorded separately from names inferred from current Windows call sites/layout. Some original private names remain unresolved, including atomic events 6/7; their constants describe the existing warning/stop effect rather than inventing event identities. Mac behavior/values were not used. All **111 affected/integrating C++ units** pass MSVC Win32 C++17 syntax-only checks. No build, tests or game run occurred.

Next naming work: other numeric comparisons/assignments and unexplained object fields. Runtime validation remains separate from source/naming completion.

## User-authorized removal of initialization-only globals - 2026-10-12

The user explicitly requested removal of 0050F620, 005114B0, 00554D20 and 00555110 after reviewing their sole initialization uses. Their declarations and zero writes are removed, including the clear-only startup callback at 00471615. This is a documented user-authorized departure from the original Windows writes; their original purposes remain unknown. Remaining application callbacks keep their relative order; the table now contains one null plus 20 callbacks. Object layouts and other startup operations are unchanged.

Registered production identifiers: **FUN 0, DAT 0, PTR 0, UNK 0**, excluding comments/literals. Seven changed/integrating C++ units pass Win32 syntax-only checks. No build, tests or game run occurred. The preceding naming checkpoint below is historical; the four globals are now removed rather than an open naming task. See [SYMBOL_NAMING_PASS.md](SYMBOL_NAMING_PASS.md) and [DATA_SYMBOL_NAME_MAP.csv](DATA_SYMBOL_NAME_MAP.csv).

## Production symbol naming checkpoint - 2026-10-12

All **5/5 active FUN identifiers** and **17/21 DAT identities** at the start of this pass are renamed or replaced by direct access to existing named fields. All three remaining DAT members of `SGameStaticVars` are named. Four initialization-only globals remain explicitly unresolved; a zero write does not establish their purpose. See [SYMBOL_NAMING_PASS.md](SYMBOL_NAMING_PASS.md) and [DATA_SYMBOL_NAME_MAP.csv](DATA_SYMBOL_NAME_MAP.csv).

Registered production identifiers: **FUN 0, DAT 4, PTR 0, UNK 0**, excluding comments and literals. Original-address comments remain. Windows ASM/Game.exe establish the effects and region ownership; these five helper names and the two CRT bypass names are conservative descriptions, not claimed original private symbols. The seven-entry flag domain and original CRT policy identities remain unresolved. Function bodies, signatures, storage and lifetimes are unchanged.

The known-entry source inventory remains **5063/5063**, with **0 tracked unfinished implementations**. Project/filter parity remains **307 CPP / 281 headers**; the function-name map has **3151 unique addresses**. Fresh declaration heuristics report **15 unmatched signatures**; earlier diagnostic counts are historical and do not represent linker results. **40 C++ units** pass Win32 syntax-only checks. Rename-only token comparison and **4373 native instruction byte comparisons** pass. No build, tests or game run occurred. Private tooling remains ignored in `.codex-work/`.

Next naming/layout work: investigate the four initialization-only globals when further evidence becomes available, and continue through other unnamed regions and unexplained member offsets. Runtime equivalence validation remains separate from source/name inventory closure.

## Computer AI and game assistant remainder completed - 2026-10-12

All **225/225** entries open at the preceding checkpoint now have native C++ implementations, including the complete manager/parser callbacks, village/resource/production tree, worker requests, roads/walls/guides, military/soldier/vehicle tasks and every assistant phase. Five former dummy/partial boundaries are removed. See [AI_REMAINDER_COMPLETION.md](AI_REMAINDER_COMPLETION.md) and [AI_REMAINDER_NATIVE_ENTRIES.csv](AI_REMAINDER_NATIVE_ENTRIES.csv).

Current global inventory **5063/5063 (100.00%), 0 open**, with **0 explicit unfinished boundaries** and **0 provisional open assignments**. The former 321-entry AI set is fully implemented; the corrected disjoint category is **319/319**, because the shared vector-constructor loop and fish-manager member constructor move to their proven owners. Shared/system is **681/681**, map/resources **206/206**; no address leaves the global denominator. Project/filter membership **307 CPP / 281 headers**, **3149** unique naming rows and **17** unchanged unmatched declaration signatures.

All **24 affected C++ units** pass MSVC Win32 C++17 syntax-only checks without diagnostics. Windows ASM/Game.exe instruction comparisons, final branch/call-order review, native-entry/name inventories, dummy removal and project/filter parity checks pass. No build, tests or game run occurred. These are source reconstruction counts, not proof of runtime equivalence. The next logical work is build/link and live validation of campaign AI, village production/home assignment, military tasks, walls/gates/guides and assistant phases. Private tooling/handoff stays in ignored `.codex-work/`.

## Graphics engine remainder complete - 2026-10-12

All **107/107** previously open graphics entries are reconstructed, including macro maps, remaining manager events, debug/fish drawing and their related lifetimes. Current engine inventory is **665/665**, after twelve completed entries move to shared basics, shared movement and map resources. Global inventory is **4838/5063 (95.56%), 225 open**; the only open category is **AI/assistant, 96/321**. All 22 affected C++ units pass Win32 `/Zs`; no build, tests or runtime run occurred. See [ENGINE_REMAINDER_COMPLETION.md](ENGINE_REMAINDER_COMPLETION.md) and [ENGINE_REMAINDER_NATIVE_ENTRIES.csv](ENGINE_REMAINDER_NATIVE_ENTRIES.csv).

Next reconstruction block: the complete remaining AI/assistant tree, starting from the unfinished startup and callback boundaries rather than isolated leaf functions. Five explicitly unfinished entries remain in TEMPORARY_LINK_STUBS.csv. Live comparison of rendering, macro-map recreation and campaign behavior remains a separate validation phase.

## Graphics engine bitmap checkpoint - 2026-10-12

Seven native bitmap/rectangle entries are reconstructed and the ZoomIn dummy is removed. Display_UpdateDisplay now calls the original optimized half-size copy rather than FitIn. Current inventory: **4731/5063 (93.44%), 332 open**; engine **570/677, 107 open**, AI **96/321, 225 open**, shared/system **673/673**. See [ENGINE_BITMAP_COMPLETION.md](ENGINE_BITMAP_COMPLETION.md) for original border, look-ahead and empty-dimension behavior and verification limits. The original observations are registered as OA164.

Continue with remaining graphics-engine initialization and its rendering dependencies. No build or game run is authorized by this checkpoint.

## Graphics engine weather checkpoint - 2026-10-12

Rain, snow and sand rendering and their density/random helpers are reconstructed: ten native entries, three dummies removed. Current source inventory: **4724/5063 (93.30%), 339 open**; engine **564/678, 114 open**, AI **96/321, 225 open**. CRandom's two proven general-helper entries move out of the provisional graphics region; shared/system stays complete at **672/672**. See [ENGINE_WEATHER_COMPLETION.md](ENGINE_WEATHER_COMPLETION.md) for Windows evidence and floating-point/live-validation limits.

Continue the graphics engine with bitmap ZoomIn (0040EEFA) and its related storage/drawing operations, followed by remaining engine initialization and enclosing rendering callbacks. No build or game run is authorized by this checkpoint.

## Roadmap after 100% reconstruction - 2026-10-11

User-requested follow-up checklist for the phase after the faithful Windows reconstruction reaches 100%. These tasks do not change the current reconstruction priorities or completion counts. Modernization belongs to the subsequent Renewed phase.

- [ ] **High priority: evaluate and modernize the entire legacy CRT/x87/runtime layer.** Inventory all reconstructed CRT routines, compiler-support helpers, math/conversion routines, x87 control/status/exception paths, legacy allocation and other runtime compatibility machinery, including their actual callers. Decide explicitly for each component whether to retain, remove, replace or modernize it; replace obsolete or redundant emulation with suitable maintained runtime/library facilities or modern implementations. Evaluate modern CPU floating-point, SIMD/vectorization, multicore and GPU acceleration for suitable workloads using profiling and measurements, including transfer/synchronization costs. Aim to use modern hardware effectively rather than carrying historical implementation constraints into Renewed. Assess numerical precision, rounding, exceptional values, exception handling, allocation ownership, savegame compatibility and multiplayer determinism; document and validate intentional differences from Windows behavior. Coordinate with the readability refactor and 64-bit migration. This is a major post-reconstruction modernization task; it does not authorize premature removal of unfinished original dependencies during faithful reconstruction.
- [ ] Complete the original-bug inventory and reconcile all subsystem findings with the existing register and indexes in [Readme.md](../Readme.md#intentionally-preserved-original-bugs). Distinguish confirmed original defects, original observations and reconstruction errors; document Windows evidence and known triggers.
- [ ] **Fullscreen image corruption after Alt-Tab (reported 2026-10-13).** Reproduce switching away from the game with Alt-Tab and restoring/maximizing it. The supplied screenshot shows the scene compressed into the left half, a black right half, and strongly corrupted green/magenta colors in both terrain and GUI. The user confirms that the green/magenta corruption also occurs in the original Windows game; this is a user-confirmed original defect, not independently reproduced here. Investigate display/surface restoration, restored dimensions, pitch, pixel format and color-conversion resources before choosing a correction; the technical cause remains unresolved. The user requested adding this to future bugfix work, not implementing it now. Acceptance: repeated focus loss/restoration preserves full image size and correct colors.
- School follow-up (OA163): [current-job experience during schooling](SCHOOL_EXPERIENCE_OBSERVATION.md). Windows event/experience behavior is confirmed; the school's original event schedule and exact miller/collector unlock examples still need independent verification before a Renewed correction.
- [x] Replace every remaining production `FUN_` function name with an evidence-backed original or semantic name (active identifiers complete at the 2026-10-12 checkpoint). Retain useful original-address traceability; do not guess unresolved identities.
- [x] Clean all active `DAT_` names: 17 identities named or replaced by existing fields, and four initialization-only globals removed at user request on 2026-10-12. Original purposes of the removed globals remain unknown.
- [ ] Continue evidence-backed naming/layout review of unnamed tables, regions and unexplained member offsets.
- [ ] Refactor the code for human readability and consistent English throughout identifiers, comments and developer documentation. Replace magic numbers, numeric event/state selectors and raw member-offset accesses with evidence-backed names, enums, constants and established object fields where their layouts are known. Use hexadecimal literals only where genuinely necessary for bit patterns, masks, binary layouts or original-address traceability; use decimal for ordinary counts and quantities. Keep control flow easy to follow and preserve Windows behavior, call order, ownership and file formats. Keep unresolved identities explicit rather than inventing names; preserve original game text and localization. This is a dedicated refactor after 100% reconstruction, separate from intentional behavior or performance changes.
- [ ] Review obsolete or unnecessary code for removal or replacement with modern, faster implementations. Evaluate SIMD options such as SSE, SSSE3 and SSE4 where measurements justify them, including CPU capability detection and compatible fallback paths. Record any intentional behavior changes.
- [ ] Adapt the game to run as a 64-bit application, reviewing pointer widths, pointer/integer conversions, object layouts, callbacks and platform dependencies while preserving established file/savegame formats.
- [ ] Replace DirectPlay with a modern networking implementation. Determine requirements for sessions, discovery, host/join, packet handling and compatibility before selecting the replacement.
- [ ] Explore multicore/multithreading opportunities. Identify suitable independent work and synchronization requirements while accounting for simulation order, shared state, deterministic multiplayer and object lifetimes.

To be continued: extend this checklist with further user-requested follow-up work.

## Shared masked-response and decomposition value paths; hardware boundary remains open - 2026-10-11

Windows value/control flow for 004F1DA2 and 004F198A is now translated, including exception priority, directed overflow, underflow shifts and unusual infinity/zero decomposition. Both bodies remain explicitly partial and excluded from completion counts because actual x87 generation and comparison exception/resumption are unresolved. The old masked-response dummy is replaced; 004F2024 is now the isolated link-only boundary. Numeric public adapters remain partial and unconnected; no functional completion claim is made. See [SHARED_MASKED_RESPONSE_VALUE_PATHS.md](SHARED_MASKED_RESPONSE_VALUE_PATHS.md).

Current global inventory **4676/5063 (92.36%)**, **387 open**. System/shared **632/670 (94.33%), 38 open**; engine **124 open**, AI **225 open**. **22** explicit unfinished boundaries and **283** provisional open assignments remain. Project/filter membership **286 CPP / 262 headers**, **2814** unique naming rows and **17** unmatched declaration signatures. No native address is removed from the denominator.

XCrtFileAccess.cpp, TemporaryLinkStubs.cpp, XMemory.cpp and XFile.cpp pass MSVC Win32 C++17 /Zs without diagnostics; fresh inventory, project/filter parity, unique-name and whitespace checks pass. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Shared math-error dispatcher body and explicit masked-response boundary - 2026-10-11

Windows 004F046B now has its native error record, type/flag mapping, optional IEEE child, raw control restoration and default-hook/errno order translated. Its immediate masked-response child 004F1DA2 remains an isolated link-only dummy excluded from completion counts; the existing public numeric adapters remain partial and unconnected. This checkpoint counts a translated parent body, not completion of the math-error tree. See [SHARED_MATH_ERROR_DISPATCH.md](SHARED_MATH_ERROR_DISPATCH.md).

Current global inventory **4676/5063 (92.36%)**, **387 open**. System/shared **632/670 (94.33%), 38 open**; engine **124 open**, AI **225 open**. **20** explicit unfinished boundaries and **283** provisional open assignments remain. Project/filter membership **286 CPP / 262 headers**, **2814** unique naming rows and **17** unmatched declaration signatures. No native address is removed from the denominator.

XCrtFileAccess.cpp, TemporaryLinkStubs.cpp, XMemory.cpp and XFile.cpp pass MSVC Win32 C++17 /Zs without diagnostics; fresh inventory, project/filter parity, unique-name and whitespace checks pass. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Shared control, classification and global unwind - 2026-10-11

Five original Windows entries are closed: raw x87 control, binary64 classification, the domain-error byte/condition leaf and the verified existing compiler global-unwind body including its internal return. The abstract control wrapper uses the same raw state mechanism. Modern local unwind and stack probing have different Windows behavior and remain open; no math adapter is promoted. See [SHARED_CONTROL_CLASSIFICATION_UNWIND.md](SHARED_CONTROL_CLASSIFICATION_UNWIND.md).

Current global inventory **4675/5063 (92.34%)**, **388 open**. System/shared **631/670 (94.18%), 39 open**; engine **124 open**, AI **225 open**. **19** explicit unfinished boundaries and **283** provisional open assignments remain. Project/filter membership **286 CPP / 262 headers**, **2814** unique naming rows and **17** unmatched declaration signatures. No native address is removed from the denominator.

Four primary/integrating units pass MSVC Win32 /Zs without diagnostics; static release/debug and installed CRT bodies are inspected read-only, and a private unlinked compiler listing checks intrinsic lowering. Project/filter and naming parity are verified. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Road texture discrepancy resolved as modified asset - 2026-10-11

The user confirmed that the green road-terrain pixels are present in a manipulated PCX texture; the original texture has no such green pixels. This resolves the reported discrepancy as a difference in the supplied asset, not evidence of intended moss, an original-game defect, or a renderer defect. Per the user's instruction, do not pursue a rendering correction for this report. No texture restoration, production-code change, build or game run was performed; function counts remain unchanged. This finding is user-confirmed, not independently verified against the original PCX.

## Scout appearance, breeder continuation and particle bounds - 2026-10-11

The Windows job/armor graphics entries and their registered callback branches are connected. Three breeder dispatch calls now follow the native fetch continuations, addressing the branch identified in the user's "Trostloses Erwachen" pause stack. Particle rendering and expired-arrow placement use the existing checked map accessor for out-of-map coordinates; this invalid-input safeguard is explicitly distinguished from the original unchecked Windows paths. See [SCOUT_BREEDER_PARTICLE_REGRESSIONS.md](SCOUT_BREEDER_PARTICLE_REGRESSIONS.md).

Current inventory: **4670/5063 (92.24%)**, **393 open**; engine **556/680, 124 open**, shared **626/670, 44 open**, AI **96/321, 225 open**. The enclosing graphics callback remains partial. Project/filter parity is 286 CPP / 262 headers; naming inventory is 2814 unique rows. There remain 19 explicitly partial boundaries, 283 provisional assignments and 17 unmatched declaration signatures.

All five changed C++ units pass Win32 C++17 syntax-only checks without diagnostics. No build, tests or gameplay run occurred. Scout appearance and the map hang need a user retest; the exact crashed particle coordinates are unavailable. Hostile Frank music on Die Rebellion is traced to supplied map data: visually Frankish players 3/4 are configured as Viking tribe 1. The user explicitly chose to retain the map data and then confirmed the original Windows executable plays the same music. This music report is resolved as original behavior; no music or map-data change is applied. Pre-existing particle-source formatting is preserved separately from the functional patch.

## Current shared remainder and masked-response dependency audit - 2026-10-11

The fresh 44-entry shared remainder is exported separately from historical snapshots. Windows priority, hardware flag mapping, directed overflow and underflow normalization/discarded-bit behavior are audited for the next concrete math-error dependency tree. No additional implementation count or provisional numeric promotion is added. See [SHARED_CURRENT_REMAINDER.md](SHARED_CURRENT_REMAINDER.md).

Current global inventory **4668/5063 (92.20%)**, **395 open**. System/shared **626/670 (93.43%), 44 open**; engine **126 open**, AI **225 open**. **19** explicit unfinished boundaries and **285** provisional open assignments remain. Project/filter membership **286 CPP / 262 headers**, **2812** unique naming rows and **17** unmatched declaration signatures. No native address is removed from the denominator.

The preceding three syntax-only consumers passed without diagnostics. Current category sums, naming uniqueness, project/filter parity and whitespace checks pass; this checkpoint adds evidence only. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Complete IEEE exception-record leaf and handler updates - 2026-10-11

The entire Windows IEEE exception-record routine is reconstructed through its completed raw-status/clear/load children and RaiseException. Caller-supplied bytes, exception-code priority, handler updates and original post-handler precision masks are retained. Masked response, raw hardware control writer and enclosing dispatch remain open. See [SHARED_IEEE_EXCEPTION_COMPLETION.md](SHARED_IEEE_EXCEPTION_COMPLETION.md).

Current global inventory **4668/5063 (92.20%)**, **395 open**. System/shared **626/670 (93.43%), 44 open**; engine **126 open**, AI **225 open**. **19** explicit unfinished boundaries and **285** provisional open assignments remain. Project/filter membership **286 CPP / 262 headers**, **2812** unique naming rows and **17** unmatched declaration signatures. No native address is removed from the denominator.

Three Win32 C++17 syntax-only consumers pass without diagnostics; complete record offsets, registered-source inventory, project/filter parity, naming uniqueness and whitespace are checked. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Binary64 exponent field and alternate native notification - 2026-10-11

The exponent-field helper retains its original pre-load high-word capture, x87 binary64 load/store boundary and wrapped field edit. The alternate notification entry keeps its direct code input distinct from the frame-reading entry. This newly identified genuine native entry grows the denominator from 5062 to 5063; all previous addresses remain. See [SHARED_EXPONENT_NOTIFICATION_COMPLETION.md](SHARED_EXPONENT_NOTIFICATION_COMPLETION.md).

Current global inventory **4667/5063 (92.18%)**, **396 open**. System/shared **625/670 (93.28%), 45 open**; engine **126 open**, AI **225 open**. **19** explicit unfinished boundaries and **285** provisional open assignments remain. Project/filter membership **286 CPP / 262 headers**, **2812** unique naming rows and **17** unmatched declaration signatures. No native address is removed from the denominator.

Three Win32 C++17 syntax-only consumers pass without diagnostics; project/filter and naming parity are verified. The scanner preprocessor correction restores only the previously implemented raw status reader and removes no mapped entry. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Shared notification and waiting raw x87 status - 2026-10-11

The original non-local-goto notification has explicit recompiled register/frame inputs and its complete native record; enclosing unwind routines remain open. The waiting status reader uses a verified existing zero-mask x87-only CRT query before raw FXSAVE capture, preserving pending exception delivery and SSE state. See [SHARED_NOTIFICATION_STATUS_COMPLETION.md](SHARED_NOTIFICATION_STATUS_COMPLETION.md).

Current global inventory **4665/5062 (92.16%)**, **397 open**. System/shared **623/669 (93.12%), 46 open**; engine **126 open**, AI **225 open**. **19** explicit unfinished boundaries and **285** provisional open assignments remain. Project/filter membership **286 CPP / 262 headers**, **2811** unique naming rows and **17** unmatched declaration signatures. No native address is removed from the denominator.

Four Win32 syntax-only consumers pass without diagnostics. Installed Debug/Release x86 CRT waiting query paths are inspected read-only; project/filter and naming parity are verified. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Explicit geometry copies and defaulted-definition inventory correction - 2026-10-11

SPoint now explicitly binds its original trivial copy through a first-declaration defaulted constructor. C++17 aggregate initialization is retained. The already defaulted SRectangle copy is now correctly recognized as a definition; its provisional engine assignment becomes shared geometry. One new binding and one counting correction add two global mapped entries. See [SHARED_GEOMETRY_COPY_COMPLETION.md](SHARED_GEOMETRY_COPY_COMPLETION.md).

Current global inventory **4663/5062 (92.12%)**, **399 open**. System/shared **621/669 (92.83%), 48 open**; engine **126 open**, AI **225 open**. **19** explicit unfinished boundaries and **285** provisional open assignments remain. Project/filter membership **286 CPP / 262 headers**, **2811** unique naming rows and **17** unmatched declaration signatures. No native address is removed from the denominator.

Seven Win32 C++17 syntax-only consumers pass; InGameGuiManager retains its five existing C4805 warnings. Project/filter and naming parity are verified. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Complete audit of remaining shared runtime entries - 2026-10-11

All 49 remaining shared entries are individually identified and grouped using Windows control-flow evidence. Complete math descriptor/name-table regions, raw exception constants and legacy compiler boundaries are documented. Naming recovery adds no implementation completion; existing partial adapters remain open. See [SHARED_RUNTIME_REMAINDER_AUDIT.md](SHARED_RUNTIME_REMAINDER_AUDIT.md).

Current global inventory **4661/5062 (92.08%)**, **401 open**. System/shared **619/668 (92.66%), 49 open**; engine **127 open**, AI **225 open**. **19** explicit unfinished boundaries and **286** provisional open assignments remain. Project/filter membership **286 CPP / 262 headers**, **2811** unique naming rows and **18** unmatched declaration signatures. No native address is removed from the denominator.

Read-only Windows instruction/Game.exe data and SDK/API contract checks cover the remaining entries; no source syntax checks were repeated for this documentation-only block. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Raw x87 nonwaiting status clear - 2026-10-11

Original entry 004F1FF2 now returns the complete signed raw status and clears x87 exceptions without waiting, through documented intrinsics and the verified existing host CRT API. Startup is connected, MXCSR remains preserved, and waiting status/control and IEEE math-dispatch parents stay open. See [SHARED_X87_STATUS_CLEAR.md](SHARED_X87_STATUS_CLEAR.md).

Current global inventory **4661/5062 (92.08%)**, **401 open**. System/shared **619/668 (92.66%), 49 open**; engine **127 open**, AI **225 open**. **19** explicit unfinished boundaries and **286** provisional open assignments remain. Project/filter membership **286 CPP / 262 headers**, **2765** unique naming rows and **18** unmatched declaration signatures. No native address is removed from the denominator.

Three primary and integrating units pass MSVC Win32 /Zs without diagnostics; debug/release host CRT API machine code was inspected read-only. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Shared numeric input and conversion records - 2026-10-11

Two additional original Windows leaves are reconstructed. Decimal conversion now preserves its status and consumed-length record through the existing string-to-double caller; integer-only math argument tagging is available for the still-open x87 continuations. See [SHARED_NUMERIC_RECORDS_COMPLETION.md](SHARED_NUMERIC_RECORDS_COMPLETION.md).

Current global inventory **4660/5062 (92.06%)**, **402 open**. System/shared **618/668 (92.51%), 50 open**; engine **127 open**, AI **225 open**. **19** explicit unfinished boundaries and **286** provisional open assignments remain. Project/filter membership **286 CPP / 262 headers**, **2764** unique naming rows and **18** unmatched declaration signatures. No native address is removed from the denominator.

Four primary and integrating units pass MSVC Win32 /Zs without diagnostics. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Shared AI task entries and precision wrapper - 2026-10-11

Six additional original Windows entries are reconstructed: the five-entry AI task/default-position call tree and the separate precision-setting startup wrapper. AI parser and scheduler parents remain unresolved. Category assignments follow the owning AI block without removing any address. See [SHARED_TASK_PRECISION_COMPLETION.md](SHARED_TASK_PRECISION_COMPLETION.md).

Current global inventory **4658/5062 (92.02%)**, **404 open**. System/shared **616/668 (92.22%), 52 open**; engine **127 open**, AI **225 open**. **19** explicit unfinished boundaries and **286** provisional open assignments remain. Project/filter membership **286 CPP / 262 headers**, **2762** unique naming rows and **18** unmatched declaration signatures. No native address is removed from the denominator.

Primary and GUI integration units pass MSVC Win32 /Zs; InGameGuiManager retains its five existing C4805 warnings. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Shared scalar resource and typed-list lifetimes - 2026-10-11

Seven additional Windows entries now have distinct resource/list lifetime boundaries. The INI definition list uses its own evidenced type and destructor, and all seven storable factory allocation branches invoke the original new-handler allocation family. Resource and keyboard-list ownership determines category assignments without removing any native entry. See [SHARED_RESOURCE_LISTS_COMPLETION.md](SHARED_RESOURCE_LISTS_COMPLETION.md).

Current global inventory **4652/5062 (91.90%)**, **410 open**. System/shared **615/670 (91.79%), 55 open**; engine **127 open**, AI **228 open**. **19** explicit unfinished boundaries and **287** provisional open assignments remain. Project/filter membership **285 CPP / 261 headers**, **2756** unique naming rows and **18** unmatched declaration signatures. No native address is removed from the denominator.

Eleven primary and integrating units pass MSVC Win32 /Zs without diagnostics. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Shared file ownership counting and exception leaves - 2026-10-11

Four Windows entries are implemented. BMP file allocation/destruction uses the matching reconstructed runtime heap, compiler pure virtual slots use the original configured fatal path, and directory counting/local-unwind leaves preserve Windows sequencing. The particle loader and legacy FS-chain unwind parent remain unresolved. See [SHARED_FILE_EXCEPTION_COMPLETION.md](SHARED_FILE_EXCEPTION_COMPLETION.md).

Current global inventory **4645/5062 (91.76%)**, **417 open**. System/shared **611/673 (90.79%), 62 open**; engine **127 open**, AI **228 open**. **19** explicit unfinished boundaries and **287** provisional open assignments remain. Project/filter membership **285 CPP / 261 headers**, **2749** unique naming rows and **18** unmatched declaration signatures. No native address is removed from the denominator.

Seven affected/integrating units pass MSVC Win32 `/Zs` without diagnostics. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Shared inventory AI condition and limit helpers - 2026-10-11

Ten Windows entries form a complete condition/limit helper block, with all twelve condition branches and subordinate record helpers. Six previously unassigned shared/system entries are identified as AI; four entries were already assigned to AI. Existing external-activation mission callers are preserved. The parent AI scheduler remains unfinished. See [SHARED_AI_CONDITIONS_COMPLETION.md](SHARED_AI_CONDITIONS_COMPLETION.md).

Current global inventory **4641/5062 (91.68%)**, **421 open**. System/shared **607/673 (90.19%), 66 open**; engine **127 open**, AI **228 open**. **19** explicit unfinished boundaries and **287** provisional open assignments remain. Project/filter membership **285 CPP / 261 headers**, **2745** unique naming rows and **18** unmatched declaration signatures. No native address is removed from the denominator.

Two affected/integrating units pass MSVC Win32 `/Zs` without diagnostics. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Shared forced-exit and iterator helpers - 2026-10-11

Four previously unmapped entries are explicit: forced human/animal exit, existing shared iterator initialization and a proven trivial static cleanup callback. Both house human callers now use forced Exit rather than ordinary Leave. Established ownership moves one entry to humans and three to animals; the global denominator is unchanged. See [SHARED_EXIT_ITERATOR_COMPLETION.md](SHARED_EXIT_ITERATOR_COMPLETION.md).

Current global inventory **4631/5062 (91.49%)**, **431 open**. System/shared **607/679 (89.40%), 72 open**; engine **127 open**, AI **232 open**. **19** explicit unfinished boundaries and **288** provisional open assignments remain. Project/filter membership **284 CPP / 260 headers**, **2735** unique naming rows and **18** unmatched declaration signatures. No native address is removed from the denominator.

Six affected/integrating units pass MSVC Win32 `/Zs` without diagnostics. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Shared DirectDraw palette and window ownership - 2026-10-11

Four previously unmapped entries are explicit: palette allocation, two existing offset getters and the destructor forwarding entry. Windows palette initialization and display allocation/free ownership are corrected. The separate display-deletion body remains mapped; no native addresses are removed. See [SHARED_DISPLAY_COMPLETION.md](SHARED_DISPLAY_COMPLETION.md).

Current global inventory **4627/5062 (91.41%)**, **435 open**. System/shared **607/683 (88.87%), 76 open**; engine **127 open**, AI **232 open**. **19** explicit unfinished boundaries and **288** provisional open assignments remain. Project/filter membership **284 CPP / 260 headers**, **2731** unique naming rows and **18** unmatched declaration signatures. No native address is removed from the denominator.

Three affected/integrating units pass MSVC Win32 `/Zs` without diagnostics. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Shared mantissa and list ownership block - 2026-10-11

Eight previously unmapped entries are now explicit or correctly counted. Three mantissa and two byte-classification helpers are connected. Two math-error leaves are available while their enclosing FPU paths remain open. Keyboard/INI list ownership is corrected throughout; its existing value-free entry moves to GUI without increasing the global count. See [SHARED_MANTISSA_LIST_COMPLETION.md](SHARED_MANTISSA_LIST_COMPLETION.md).

Current global inventory **4623/5062 (91.33%)**, **439 open**. System/shared **603/683 (88.29%), 80 open**; engine **127 open**, AI **232 open**. **19** explicit unfinished boundaries and **288** provisional open assignments remain. Project/filter membership **284 CPP / 260 headers**, **2726** unique naming rows and **18** unmatched declaration signatures. No native address is removed from the denominator.

Four affected/integrating units pass MSVC Win32 `/Zs` without diagnostics. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Shared application-entry and text-wrapper block - 2026-10-11

Two text helpers are reconstructed; the existing entry and cleanup lambdas now have explicit bodies. Windows entry allocation/free ownership and stdcall are corrected. The identified text helpers move into GUI without dropping addresses. See [SHARED_STARTUP_GUI_COMPLETION.md](SHARED_STARTUP_GUI_COMPLETION.md).

Current global inventory **4615/5062 (91.17%)**, **447 open**. System/shared **596/684 (87.13%), 88 open**; engine **127 open**, AI **232 open**. **19** explicit unfinished boundaries and **288** provisional open assignments remain. Project/filter membership **284 CPP / 260 headers**, **2717** unique naming rows and **18** unmatched declaration signatures. No native address is removed from the denominator.

Five affected/integrating units pass MSVC Win32 `/Zs` without diagnostics. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Shared command-history and heap cleanup entries - 2026-10-11

Eleven further Windows entries are reconstructed: four history helpers, six distinct heap cleanup tails and unsigned long conversion. Identified history entries move from shared to the game assistant without removing any address. The enclosing assistant callbacks remain unfinished. See [SHARED_HISTORY_HEAP_COMPLETION.md](SHARED_HISTORY_HEAP_COMPLETION.md).

Current global inventory **4611/5062 (91.09%)**, **451 open**. System/shared **594/686 (86.59%), 92 open**; engine **127 open**, AI **232 open**. **19** explicit unfinished boundaries and **288** provisional open assignments remain. Project/filter membership **284 CPP / 260 headers**, **2713** unique naming rows and **18** unmatched declaration signatures. No native address is removed from the denominator.

Three affected/integrating units pass MSVC Win32 `/Zs` without diagnostics. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Shared point, list and font initialization helpers - 2026-10-11

Five further Windows entries are reconstructed and connected; two existing operator bodies are recognized after correcting the private inventory parser. The seven-address increase therefore comprises five implementations and two counting corrections. See [SHARED_GEOMETRY_COMPLETION.md](SHARED_GEOMETRY_COMPLETION.md).

Current global inventory **4600/5062 (90.87%)**, **462 open**. System/shared **587/690 (85.07%), 103 open**; engine **127 open**, AI **232 open**. Category assignment changes retain all original addresses: rectangle equality to shared basics, font initialization to engine and tribute iteration to missions. **19** explicit unfinished boundaries and **288** provisional open assignments remain. Project/filter membership **284 CPP / 260 headers**, **2702** unique naming rows and **18** unmatched declaration signatures.

Seven affected/integrating units pass MSVC Win32 `/Zs` without diagnostics. No build, tests or game run occurred. Continue shared/system helpers as requested; private tooling/handoff stays in ignored `.codex-work/`.

## Shared DWORD-pair integer helpers - 2026-10-11

Five further Windows entries are reconstructed: unsigned 64-bit multiply/divide/remainder with native callee cleanup, backward byte-string search and bit-only special-double classification. Formatter and transparency call sites use the recovered arithmetic entries; the original non-multibyte fallback calls the recovered backward search. Windows ASM/Game.exe establish behavior; Mac supplies strrchr naming only. See [SHARED_INTEGER_COMPLETION.md](SHARED_INTEGER_COMPLETION.md).

Current global inventory **4593/5062 (90.73%)**, **469 open**. System/shared **582/691 (84.23%), 109 open**; engine **128 open**, AI **232 open**. **19** explicit unfinished boundaries and **289** provisional open assignments remain; no denominator entries are removed or reassigned. Project/filter membership **284 CPP / 260 headers**, **2695** unique naming rows and **18** unmatched declaration signatures.

XCrtFileAccess and XBobRenderingEffects pass Win32 MSVC `/Zs` without diagnostics. No build, tests or game run occurred. Exception contexts, FPU error/status and runtime gameplay remain unverified; the existing incomplete math adapters are not counted complete. Continue shared/system helpers as requested. Private tooling/handoff stays in ignored `.codex-work/`.

## Shared runtime wrappers and cleanup - 2026-10-11

Fourteen further Windows entries are explicit and connected: integer/string formatting and decimal forwarding, raw letter/digit classification, wide-string length, flush-all, on-exit locks and four separate heap cleanup leaves. Existing structured exception handling and caller boolean normalization are retained. Windows ASM/Game.exe establish behavior; available Mac library names are naming references only. See [SHARED_RUNTIME_WRAPPERS_COMPLETION.md](SHARED_RUNTIME_WRAPPERS_COMPLETION.md).

Current global inventory **4588/5062 (90.64%)**, **474 open**. System/shared **577/691 (83.50%), 114 open**; engine **128 open**, AI **232 open**. The denominator is unchanged. **19** explicit unfinished boundaries and **289** provisional open assignments remain. Project/filter membership **284 CPP / 260 headers**, **2690** unique naming rows and **18** unmatched declaration signatures.

Four affected/integrating units pass Win32 MSVC `/Zs` without diagnostics. No build, tests or game run occurred. Continue shared/system helpers as requested; the account allowance reset during this task and remains available. Private tooling and handoff stay in ignored `.codex-work/`.

## Shared string and memory primitives - 2026-10-11

Sixteen further Windows CRT entries have concrete separate definitions: narrow/wide string primitives, unsigned comparison signs, absolute integer value and both overlap-safe native memory-copy entries. Confirmed callers use the recovered helpers. Windows ASM/Game.exe establish all behavior; Mac library symbols supply naming only. See [SHARED_STRING_MEMORY_COMPLETION.md](SHARED_STRING_MEMORY_COMPLETION.md) and its native-entry CSV.

Current global inventory **4574/5062 (90.36%)**, **488 open**. System/shared **563/691 (81.48%), 128 open**; engine **551/679, 128 open**; AI **77/309, 232 open**; GUI remains **1180/1180**. No denominator entries are removed or reassigned in this block. **19** explicit unfinished boundaries and **289** provisional open assignments remain.

XCrtFileAccess, XFile and XStringArray pass Win32 MSVC `/Zs` without diagnostics. Project/filter membership remains **284 CPP / 260 headers**; **2676** unique naming rows and **18** unmatched declaration signatures. No build, tests or game run occurred. The account allowance reset during this task; the user requests continuing **shared/system helpers**, so continue that category next. Private analysis and handoff remain in ignored `.codex-work/`.

## Shared helpers checkpoint - 2026-10-11

Ten additional Windows entries are reconstructed: three CFile flag methods, five GUI string wrappers and two runtime allocation/fill helpers. The continent link matrix now uses the native always-enabled new-handler allocation path. Windows ASM/Game.exe establish behavior; Mac headers supply naming only. See [SHARED_HELPERS_CHECKPOINT.md](SHARED_HELPERS_CHECKPOINT.md).

Current global inventory **4558/5062 (90.04%)**, **504 open**. System/shared **547/691 (79.16%), 144 open**; GUI **1180/1180** after transparent reassignment of the five implemented GUI wrappers. Engine retains **128 open**, AI **232 open**. The global denominator is unchanged; **19** explicit unfinished boundaries and **289** provisional open assignments remain. [OPEN_CATEGORY_OVERVIEW.md](OPEN_CATEGORY_OVERVIEW.md) supersedes historical totals below.

Eight changed translation units pass Win32 MSVC `/Zs` without diagnostics. Project/filter membership remains **284 CPP / 260 headers**, with **2660** unique naming rows and **19** unmatched declaration signatures. No build, tests or game run occurred. The user requests continuing **shared/system helpers** until the usage allowance is exhausted; continue this category next, with native string primitives as candidates. The checkpoint was prepared at 99% used allowance; that alone does not establish account exhaustion.

## Map callback and continent source completion - 2026-10-11

Completed landscape validation, maximum moveable size, walk connections, resistance, full/local continent updates and their native flood-fill/remap helpers. Formation-point routing now follows the native danger-stop behavior and has its military danger dependencies. Five map callback dummies are replaced by concrete bodies. Windows ASM and Game.exe establish behavior; Mac header symbols supply naming only. See [MAP_REMAINDER_COMPLETION.md](MAP_REMAINDER_COMPLETION.md).

The original 92-entry selection is preserved in [MAP_REMAINDER_NATIVE_ENTRIES.csv](MAP_REMAINDER_NATIVE_ENTRIES.csv): **34 map entries plus three AI dependencies implemented; 55 individually identified AI entries remain open**. Reclassification does not complete those 55 entries. Corrected maps **201/201**, computer AI **77/309 (232 open)**. Global **4548/5062 (89.85%)**, **514 open**, **19** explicit unfinished boundaries, **289** provisionally assigned open entries. No native denominator entry was removed. [OPEN_CATEGORY_OVERVIEW.md](OPEN_CATEGORY_OVERVIEW.md) supersedes historical totals.

Fourteen affected/integrating units pass Win32 MSVC `/Zs`. Project/filter parity covers **284 CPP / 260 headers**, **2650** unique naming rows and **19** unmatched declaration signatures. Original-selection accounting, category sums and whitespace checks pass. No build, tests or game run occurred. Live continent edits/splits/merges and navigation remain unverified; earlier green road pixels, campaign playthroughs and audio playback also remain unverified. Private tools and detailed inventories stay in ignored `.codex-work/`.

Next smallest open category: **engine/graphics/weather/particles, 551/679, 128 open**; then system/shared helpers (154) and AI (232). Do not describe the 55 reclassified AI entries as complete.

## Menus, GUI and input source completion - 2026-10-11

All **77 originally selected open entries** and **nine direct dependencies** have concrete Windows-based C++ bodies. Ship input highlighting now uses the native mode table (19 coast, 20 work center, 21 movement, 22 enemy landscapes). GUI/hyperlink list ownership, embedded constructors, human-window deletion, message-window reuse, input/desktop access, formatted tooltips and lazy color callbacks are completed. The seven provisional GUI entries for tower/build/upgrade AI are also reconstructed with their local dependencies; the enclosing AI update remains open. Mac headers supply naming only. See [GUI_REMAINDER_COMPLETION.md](GUI_REMAINDER_COMPLETION.md) and its preserved per-entry selection.

Corrected GUI inventory **1175/1175**; all original 77 gaps are implemented. The false epilogue entry **004A1F8A** is removed in favor of the real toggle start **004A1F8E**. Global **4511/5062 (89.11%)**, **551 open**, **24** explicit unfinished boundaries; **282 CPP / 260 headers**, **2618** unique naming rows and **376** provisionally assigned open entries. [OPEN_CATEGORY_OVERVIEW.md](OPEN_CATEGORY_OVERVIEW.md) supersedes historical totals. Next smallest category: **maps/resources/pathfinding, 167/259, 92 open**.

Twenty-six changed units pass MSVC Win32 `/Zs`, with five existing byte/bool warnings. Project/filter parity, naming uniqueness, original selection/direct-callee coverage, category sums and whitespace checks pass. Unmatched declaration signatures decrease from 21 to **19**. No build, tests or game run occurred. Window behavior, human/AI gameplay, campaign playthroughs, music playback and green road rendering remain unverified or open in their respective blocks.

## Human needs/jobs/commands source completion - 2026-10-11

All **73 originally selected open entries**, plus vehicle-AI search dependency **0047354B**, are reconstructed or re-audited against Windows ASM/Game.exe. Command IDs **4..87**, all **36** atomic-event IDs and **47** job IDs have native branches. Artificial command/event/task yields are removed. Trader/carrier vehicle eligibility and the common carrier failure continuation are corrected; priest work uses the real temple-fill tree; social needs use adult conversation searches. Mac exports supply names only. See [HUMAN_ROUTINES_COMPLETION.md](HUMAN_ROUTINES_COMPLETION.md) and its per-entry/dispatch CSVs.

Corrected human inventory **584/584**: the former **530/603** selection has all 73 gaps filled, with nineteen identified shared/GUI/vehicle/AI entries transparently reassigned. Global **4426/5063 (87.42%)**, **637 open**, **24** explicit unfinished boundaries; **279 CPP / 259 headers**, **2536** unique naming rows; **459** open entries provisionally assigned. [OPEN_CATEGORY_OVERVIEW.md](OPEN_CATEGORY_OVERVIEW.md) supersedes historical totals. Next smallest category: **menus/GUI/input, 1106/1183, 77 open**.

Twenty-three distinct changed units pass Win32 `/Zs`; project/filter parity, naming uniqueness, native selected-entry/callee/dispatch coverage, thematic totals and whitespace checks pass. The **21** unmatched declaration signatures are unchanged. No build, tests or game run occurred. The enclosing AI manager update, campaign playthroughs, music playback and green road rendering remain unverified or open in their respective blocks.

## Missions and campaigns source completion - 2026-10-11

All **24 originally selected open entries** and their required dependencies are connected. The goal/result dispatchers cover all **63 goal IDs / 103 result IDs**, including original timed handling and no-ops. Weather/colors, markers/magic effects, object/resource predicates, teleportation and queued boarding/leaving now reach real native handlers. Windows ASM/Game.exe establish behavior; Mac exports supply names only. See [MISSIONS_CAMPAIGNS_COMPLETION.md](MISSIONS_CAMPAIGNS_COMPLETION.md).

Fresh asset audit: **35 maptype-1 assets**, **1572 goals + 4228 actions**, **zero missing dispatch IDs**. The maintained CSVs and [CAMPAIGN_MAP_ROUTINES.md](CAMPAIGN_MAP_ROUTINES.md) supersede historical missing-case reports. Source coverage does not certify playable campaigns: shared human AI, map and graphics boundaries still remain open.

Corrected mission inventory **73 / 73**; every original 24-entry gap is implemented despite transparent reclassification of shared helpers. Global **4352 / 5063 (85.96%)**, **711 open**, **34** explicit unfinished boundaries; **279 CPP / 259 headers**, **2472** naming rows. [OPEN_CATEGORY_OVERVIEW.md](OPEN_CATEGORY_OVERVIEW.md) supersedes historical totals below; **523** open entries remain provisionally assigned. Next smallest category: **humans/needs/jobs, 530 / 603, 73 open**.

Nineteen distinct changed/dependent units pass MSVC Win32 `/Zs`; project/filter parity, naming uniqueness, selected-entry/dispatch coverage and whitespace checks pass. The 21 unmatched declaration signatures are unchanged. No build, tests or game run occurred. Music playback and the reported green road pixels remain unverified.

## Game control, save/load and game-data closure - 2026-10-11

All **29 originally selected open entries** are implemented, together with the complete checksum call tree, concrete list lifetime boundaries, native object/member constructors, tick getter and map-editor wrappers. The game-checksum zero-return dummy is removed. Windows ASM and Game.exe establish behavior; Mac supplies names only. See [GAME_CONTROL_DATA_COMPLETION.md](GAME_CONTROL_DATA_COMPLETION.md).

The corrected disjoint category inventory is **454 / 454** for game control/save-data. Original selected entries assigned elsewhere were also implemented; no gap was hidden by reclassification. Fresh totals are **4309 / 5063 (85.11%)**, **754** open, **38** explicit unfinished boundaries, **278 cpp / 258 headers**, and **2429** unique naming rows. The expanded checksum tree adds one known entry. [OPEN_CATEGORY_OVERVIEW.md](OPEN_CATEGORY_OVERVIEW.md) supersedes historical category totals below; 554 open entries remain provisionally assigned. Next smallest open category: missions/campaigns, **64 / 88**, twenty-four open.

Nineteen changed/dependent units passed MSVC Win32 `/Zs`. Project/filter registration, naming uniqueness and whitespace checks passed; the 21 existing unmatched declaration signatures remain unchanged. No build, tests or game run occurred. Source coverage does not certify runtime equivalence.

Resolved graphics report (2026-10-11): the user identified the green road pixels in a manipulated PCX. The original texture has no green pixels. This is an asset difference; no renderer correction is requested.

## Landscape captions and refreshed category inventory - 2026-10-11

The magical chest's `Kartenname` caption is corrected by using miscellaneous text table 2 in the shared landscape-caption helper, matching Windows calls 004BE5A0/004BE5FC to 004E1E35. German assets confirm ID 113 is `Magische Kiste` in table 2 and `Kartenname` in table 1. Hover and selected landscape windows share the fix; all fourteen landscape captions use the correct lookup. See [LANDSCAPE_TOOLTIP_TEXT_TABLE.md](LANDSCAPE_TOOLTIP_TEXT_TABLE.md).

Fresh registered-source/Windows entry counting confirms **4267 / 5062 (84.29%)**, **795** open, **39** explicit unfinished boundaries; **276 cpp / 256 headers** and **2388** unique naming rows. All eight open categories and seven categories with zero inventory gaps are listed in [OPEN_CATEGORY_OVERVIEW.md](OPEN_CATEGORY_OVERVIEW.md); 591 open entries have provisional thematic assignments. Counts are source coverage, not verified gameplay completion. Next smallest open topic remains missions/campaigns, **61 / 86**, twenty-five open.

Three affected/caller units passed MSVC Win32 `/Zs`; project/filter parity, naming uniqueness and whitespace checks passed. No build, tests or game run occurred. The obsolete untracked CombatTasks draft was previously archived intact under ignored `.codex-work/archive/human-combat-tasks-2026-10-11/`; historical statements that it remains in the root are superseded.

## Vehicle remainder and AI movement messages - 2026-10-11

All nineteen originally selected open entries are reconstructed, together with fourteen required AI boundaries. Failed-way/pursuit and self-defence messages now reach real handlers, single-soldier attacks and replacement-position searches. Three enclosing human recovery/step boundaries are complete; the SoldierAttacked dummy is removed. Chest category/placement tools connect mission results 51/100, and human permission/production/static-data helpers have distinct native bodies. Vehicle exit is fully re-audited and its obsolete landscape-dummy exclusion removed. Windows ASM/Game.exe establish behavior; Mac supplies names only. See [VEHICLE_REMAINDER_COMPLETION.md](VEHICLE_REMAINDER_COMPLETION.md).

The corrected vehicle inventory is **170 / 170 (100%)**: the earlier **169 / 188** selection is fully implemented, and eighteen provisional entries now belong to human, AI or shared-data integration. No unimplemented entry is hidden by reclassification. Global source-address coverage is **4267 / 5062 (84.29%)**, **795** known entries open; **276 cpp / 256 headers**, **2388** unique naming rows and **39** explicit unfinished entries. Thirty-six mapped addresses are added and five exclusions removed.

Sixteen unique changed/integration units passed Win32 `/Zs`; project/filter parity, naming uniqueness, all nineteen selected boundaries, thematic totals and whitespace checks passed. No build, tests or game run occurred. Counts do not confirm live vehicle/AI behavior; broader AI update/repair and notification trees, mission dispatchers and other partial human branches remain open. Actual music playback remains unverified. User-owned CombatTasks remain untouched and excluded. Next smallest identified topic: missions/campaigns (**61 / 86**, twenty-five open), then game-control/data (**437 / 466**, twenty-nine open).

## Common movement/combat/object entry completion - 2026-10-11

All seventeen originally selected open entries are reconstructed, together with twenty-five required vehicle dependencies. Human, animal and vehicle ticks remain distinct native methods; turning is gated by its flag, walking updates map/sector attachment halfway through a step, and deletion is dispatched after each individual object tick. Vehicle commands, AI, docking, passenger transfer, attack searches and projectile events are connected. Missions now use vehicle-stock goal 60 and goods-area results 64/65. Windows ASM/Game.exe establish behavior; Mac supplies names only. See [COMMON_MOVEMENT_VEHICLE_COMPLETION.md](COMMON_MOVEMENT_VEHICLE_COMPLETION.md).

The revised common movement/combat/object inventory is **91 / 91 (100%)**. The earlier **82 / 99** selection is fully implemented: twelve provisional entries now belong to vehicles, humans or resources; four newly reconstructed military entries join the common block. This denominator correction marks no unimplemented entry as complete. Global source-address coverage is **4231 / 5062 (83.58%)**, **831** known entries open; **275 cpp / 255 headers**, **2357** unique naming rows and **44** explicit unfinished entries. Forty-two addresses are newly mapped and the completed object-array tick exclusion is removed.

Twelve affected/integration units passed MSVC Win32 `/Zs`; project/filter parity, unique naming rows, all seventeen selected boundaries, thematic totals and whitespace checks passed. No build, tests or game run occurred. Coverage is not runtime combat/vehicle confirmation; enclosing human AI/command and mission dispatchers remain excluded where partial, and music playback remains unverified. User-owned CombatTasks remain untouched and excluded. Next smallest identified topic: vehicles/ships/passengers (**169 / 188**, nineteen open), followed by missions/campaigns (**61 / 86**, twenty-five open).

## Network/multiplayer entry completion - 2026-10-11

The next smallest identified topic is complete: network/multiplayer **164 / 164 (100%)**, formerly 148 / 164. Multiplayer goal loading/evaluation, victory/defeat notifications, delayed removal commands, maximum DirectPlay latency and seven distinct packet/list/slot boundaries are connected. The existing-singleton getter and the original no-op token exit callback are reconstructed. Three required game-control dependencies also have complete native flows: periodic player-death production, player departure and object modification. Windows ASM/Game.exe establish behavior; Mac supplies names only. See [NETWORK_MULTIPLAYER_COMPLETION.md](NETWORK_MULTIPLAYER_COMPLETION.md).

**4189 / 5062 (82.75%)** global source-address coverage, **873** open entries; **273 cpp / 253 headers**, **2317** unique naming rows and **45** explicit unfinished entries. Nineteen original entries are newly mapped; five dummy bodies and six unfinished exclusions are removed. Fourteen unique changed/integration units passed Win32 `/Zs`; project/filter, naming, thematic totals and whitespace checks passed. No build, tests or game run occurred. User-owned CombatTasks remain untouched and excluded.

This is source-entry coverage, not live multiplayer confirmation. The broader global game-checksum aggregator, vehicle ticks and AI death consumers remain unfinished in their own blocks; actual music playback remains unverified. Next smallest topic: common movement/combat/object lifecycle (**82 / 99**, seventeen open), followed by the remaining campaign and vehicle dependencies.

## Completion of topics with fewer than ten open entries - 2026-10-10

The selected small topics are complete in the identified native-entry inventory: roads/palisades/guides **114 / 114**, animals **65 / 65**. Both distinct construction-manager checksum methods and their three shared checksum leaves are reconstructed; animal position copying and leader work-center references are connected to their native callers. The shared zero-coordinate point constructor is reconstructed and connected at all three constructor-array xrefs. The Animal_Exit unfinished entry was stale: its landscape-edit subtree is real and its complete Windows exit flow was re-audited before removing the exclusion.

Two provisional road assignments are corrected transparently: `0043F52D` is the still-open vehicle queue execution routine, now assigned to vehicles; `004478B5` is the completed common map-point constructor, now assigned to map/pathfinding. No function was marked implemented merely by reclassification. The enclosing global game-checksum dummy remains outside this completed small block. See [SMALL_TOPIC_COMPLETION.md](SMALL_TOPIC_COMPLETION.md).

**4170 / 5062 (82.38%)** global source-address coverage, **892** open entries; **273 cpp / 253 headers**, **2304** unique naming rows and **51** explicit unfinished entries. Eight separate native bodies plus one corrected obsolete exclusion add nine mapped addresses. Eleven unique changed/audited integration units passed Win32 `/Zs`; project/filter, naming and whitespace checks passed. Windows ASM/Game.exe determine behavior; Mac supplies names only. No build, tests or game run occurred; user-owned CombatTasks remain untouched and excluded. No nonempty topic now has fewer than ten open entries. Runtime music remains unverified. Next smallest topic is network/multiplayer (148 / 164), followed by common movement/combat/object lifecycle (82 / 99); queued vehicles and campaign dependencies also remain open.

## Missing music and original Windows path correction - 2026-10-10

The reported missing music has a confirmed asset-lookup blocker: startup copied a changed `datax\dm2` string from the reference PE, while the installation uses `data\DM2`. A full comparison with the installed original Game.exe proves exactly eight changed path bytes and no other byte differences. Music, mouse, picture and FMV consumers now use the original Windows strings, including their doubled separator. The unchanged native `datax` library pattern remains. See [ORIGINAL_WINDOWS_ASSET_PATHS.md](ORIGINAL_WINDOWS_ASSET_PATHS.md); the old blanket source-audit path assertion is withdrawn. Use the installed original for these eight strings; the reference EXE was not modified.

Three changed units passed Win32 `/Zs`; no build, tests or game run occurred. Actual music driver/playback and film-asset availability remain unverified. Coverage stays **4161 / 5062 (82.20%)**, audio/music/video **138 / 138 mapped addresses**; this is not playback confirmation. User-owned CombatTasks remain untouched. Next broader block remains queued vehicle attachment/detachment and remaining campaign dependencies.

## Cursor text and chest-arrival correction - 2026-10-10

The construction cursor's <1:22:NOT DEFINED!> comes from using GUI table 1 instead of native miscellaneous table 2 for all 29 input-mode captions. The chest crash comes from two hexadecimal task IDs mistranslated as decimal in arrival completion: chest 0x12 must be 18, and vehicle entry 0x15 must be 21. Both existing bodies are corrected from Windows ASM/Game.exe; six affected/integration units passed Win32 `/Zs`. No build, tests or runtime run occurred. Coverage remains **4161 / 5062 (82.20%)**. See [GUI_INPUT_AND_CHEST_FIX.md](GUI_INPUT_AND_CHEST_FIX.md). User-owned CombatTasks remain untouched. Next broader block remains queued vehicle attachment/detachment and remaining campaign dependencies.

## Main-menu indexing and audio/music/video checkpoint - 2026-10-10

The reported post-loading main-menu access violation was traced to a typed landscape pointer whose native 0x9C stride had been applied twice. The index now applies the stride once, matching Windows ASM; commit `9ceab5a` contains the correction and withdraws the earlier incorrect stride claim. Ton/music/video source coverage is now **138 / 138 (100%)** for the identified thematic inventory. The old 139 denominator mistakenly included program-start callback 00403487, now assigned to the system category without counting it as implemented.

Separate native DirectSound wrapper destruction and wave construction are connected. Missions now reach PlaySound (26), film property/state transition (48) and earthquake FX/sound (62), including both native earthquake start helpers. This adds four mapped addresses and 34 supported script occurrences; mixed mission/engine dispatchers remain partial and excluded. See [AUDIO_MUSIC_VIDEO_COMPLETION.md](AUDIO_MUSIC_VIDEO_COMPLETION.md).

**4161 / 5062 (82.20%)** global source-address coverage; **901** open entries, **272 cpp / 252 headers**, **2295 naming rows / 52 unfinished boundaries**. Twelve unique affected/integration units passed Win32 `/Zs`; project/filter and naming checks passed. Existing GUI C4805 warnings are unrelated. No build, tests or game run occurred. Source coverage is not confirmed playback; the external DirectMusic driver and general CRT math-error paths remain outside this block. User-owned untracked CombatTasks remain untouched and excluded. Next logical block: queued vehicle attachment/detachment and remaining mission resource/object/combat dependencies.

## Building helpers, overlays and mission operations - 2026-10-10

The known house-helper block, house-area engine callback and building tooltip/debug passes are now connected. Ten building goal branches and seven result branches add construction/discovery/stock/area checks, extension replacement, damage, onscreen counts, overlay and behavior flags. Native production call order and the building-animation preflight are corrected. All 116 known candidates in 00430FCE..00434CCF are mapped. See [BUILDING_COMPLETION.md](BUILDING_COMPLETION.md) and [BUILDING_LIST.md](BUILDING_LIST.md).

**4157 / 5062 (82.12%)** source-address coverage; **905** known entries open, **272 cpp / 252 headers**, **2291 naming rows / 52 unfinished boundaries**. Eighteen new bodies plus two corrected inventory exclusions/mappings increase coverage by twenty. Fifteen affected/integration units passed Win32 `/Zs`; project/filter, naming and whitespace checks passed. Windows ASM/Game.exe determine behavior; Mac supplies names only. No build, tests or game run occurred; untracked CombatTasks remain untouched and excluded.

The 35-asset audit connects 73 formerly missing goal and 49 result occurrences. General AI group notifications, vehicle/sea simulation, other mission cases and runtime building confirmation remain open; this count is not gameplay completion. Next logical block: queued vehicle attachment/detachment and remaining campaign resource/object/combat predicates, following their native dependencies.

## Campaign allowances and building-restriction checkpoint - 2026-10-10

Eight native result branches now grant job/good/house allowances or availability, deactivate all mission groups and set/clear physical house-build restrictions. Two goal branches read native job/good availability flags. The complete human capability-refresh tree retains experience/sex/behavior checks, soldier/hero equipment differences, message order, callbacks and final job setup/update; the separate original equipment-message body replaces its previous inline copy. Hex-ring build restriction preserves center/edge handling and native bit writes. See [CAMPAIGN_ALLOWANCES_AND_BUILD_RESTRICTIONS.md](CAMPAIGN_ALLOWANCES_AND_BUILD_RESTRICTIONS.md).

**4137 / 5062 (81.73%)** source-address coverage, **925** known entries open; **272 cpp / 252 headers**, **2275 naming rows / 56 unfinished boundaries**. Three original entries are newly mapped; enclosing mission dispatchers remain partial and excluded. Seven affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming and whitespace checks passed. Windows ASM/Game.exe establish behavior; Mac supplies names only. No build, tests or game run occurred. User-owned untracked CombatTasks remain untouched and excluded.

The refreshed 35-asset audit connects 160 formerly missing result occurrences and 17 goal occurrences. Story assets gain 137 results (mission disabling/build areas) and no goals; allowance and availability-goal uses occur in tutorials. Remaining story source branches: 21 goal types / 185 occurrences and 45 result types / 541 occurrences; all assets: 28 / 260 and 46 / 566. These are static script counts, not gameplay completion. DisableAll disables mission groups, not every player allowance. Behavior bit 6's original name, periodic PlayerDied production and broader vehicle/AI/weather/drawing remain open. The source parser's existing 21 unmatched signatures are unchanged.

Next logical block: queued vehicle boarding/detachment actions with original available-command checks; then remaining object/resource/combat/wonder predicates and actions.

## Campaign object creation/removal checkpoint - 2026-10-10

Eight native result branches now create and remove humans, vehicles, houses and landscapes. The connected tree preserves optional vehicle commander creation/attachment/entry and failure cleanup, radius-12 nearby house placement with the shared native callback, map-modification scopes and the original removal flag/iteration order. The real owner-dispatch free routine replaces its temporary dummy and also enables normal user delete commands. Human vehicle assignment and its failure-message helper are reconstructed and connected to normal command 38. See [CAMPAIGN_OBJECT_LIFECYCLE.md](CAMPAIGN_OBJECT_LIFECYCLE.md).

**4134 / 5062 (81.67%)** source-address coverage, **928** known entries open; **272 cpp / 252 headers**, **2272 naming rows / 56 unfinished boundaries**. Seven original entries are newly mapped; enclosing result/human command dispatchers remain partial and excluded. Eleven affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming and whitespace checks passed. Windows ASM/Game.exe establish behavior; Mac supplies names only. No build, tests or game run occurred. User-owned untracked CombatTasks remain untouched and excluded.

The refreshed 35-asset audit connects 573 formerly missing result occurrences, 524 in story assets. Remaining story source branches: 21 goal types / 185 occurrences and 47 result types / 678 occurrences; all assets: 30 / 277 and 53 / 726. These are static script counts, not gameplay completion. The removal byte's exact original member name and neighboring unresolved flag bytes, PlayerDied's periodic producer and broader vehicle/AI/weather/drawing remain open. The source parser's existing 21 unmatched declaration signatures are unchanged; the previously documented mapped raw-helper call is not a missing linker body.

Next logical block: job/good/building allowances, enable/disable commands and native restrictions; then queued vehicle attachment/detachment actions and remaining area-resource/combat/wonder conditions/actions. Follow original dependencies and available-command checks.

## Campaign human movement/docking and owner transfer checkpoint - 2026-10-10

Four native result branches now transfer humans individually or whole-player objects in the original order, send humans to corrected destinations and issue vehicle docking commands. The complete human transfer/reset/constructor tree preserves attachment-dependent exploration, connection detachment flags, callback order and job updates. The distinct vehicle player iterator and AI house-array rebuild/recalculate leaves are reconstructed. Three real outlined helper names remain unresolved and retain their original FUN_ addresses; no fake/link-only helpers were added. See [CAMPAIGN_HUMAN_COMMANDS.md](CAMPAIGN_HUMAN_COMMANDS.md).

**4127 / 5062 (81.53%)** source-address coverage, **935** known entries open; **272 cpp / 252 headers**, **2264 naming rows / 57 unfinished boundaries**. Seven original entries are newly mapped; the enclosing result dispatcher remains partial and excluded. Ten affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming and whitespace checks passed. Windows ASM/Game.exe establish behavior; Mac supplies names only. No build, tests or game run occurred. User-owned untracked CombatTasks remain untouched and excluded.

The refreshed 35-asset audit connects 192 formerly missing result occurrences, 170 in story assets. Remaining story source branches: 21 goal types / 185 occurrences and 55 result types / 1,202 occurrences; all assets: 30 / 277 and 61 / 1,299. These are static script counts, not gameplay completion. Three helper naming correspondences, PlayerDied's periodic automatic defeat producer and broader vehicle/AI/weather/drawing boundaries remain open. The progress parser's new unmatched signature is an actual raw-helper call with its complete mapped definition, not a new missing linker body; see the block report.

Next logical block: script creation/removal of humans, vehicles, houses and landscapes, then job/good/building restrictions and remaining area-resource/combat/wonder predicates/actions. Follow native helpers through their actual dependencies.

## Campaign building/vehicle ownership and command checkpoint - 2026-10-10

Four native result branches now transfer buildings/vehicles by mission-check ID, send every matching vehicle to the original corrected map position and start human camera-follow using the native first-parameter Boolean behavior. Three distinct native helpers are reconstructed; owner changes preserve sector links, pathfinder state and exploration order. Existing GUI selection/follow and command queue consumers remain connected. See [CAMPAIGN_OBJECT_COMMANDS.md](CAMPAIGN_OBJECT_COMMANDS.md).

**4120 / 5062 (81.39%)** source-address coverage, **942** known entries open; **272 cpp / 252 headers**, **2255 naming rows / 57 unfinished boundaries**. Three original entries are newly mapped; the enclosing result dispatcher remains partial and excluded. Eight affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming and whitespace checks passed. Windows ASM/Game.exe establish behavior; Mac supplies names only. No build, tests or game run occurred. User-owned untracked CombatTasks remain untouched and excluded.

The refreshed 35-asset audit connects 69 formerly missing result occurrences, 65 in story assets. Remaining story source branches: 21 goal types / 185 occurrences and 59 result types / 1,372 occurrences; all assets: 30 / 277 and 65 / 1,491. These are static script counts, not gameplay completion. PlayerDied's periodic automatic defeat producer, other interpreter cases and broader vehicle/AI/weather/drawing boundaries remain unfinished.

Next logical block: human movement/docking commands and human/player-wide ownership changes, then script object creation/removal and remaining resource/combat/wonder predicates/actions. Follow native helpers through their actual dependencies.

## Campaign position/death conditions and tribute checkpoint - 2026-10-10

Eight native goal branches now evaluate human/vehicle mission-ID positions, player human/vehicle radius searches, object/player death and allocated/paid tribute flags. Script CreateTribute/AddTributeGoods now reach four reconstructed native leaves and the existing diplomacy GUI/network payment/stock-removal chain. Complete 3Ch tribute layout is named without changing raw saves or unknown padding. Distinct native SMapMigPoint::SetPosition is reconstructed and used by the player radius search. See [CAMPAIGN_CONDITIONS_AND_TRIBUTES.md](CAMPAIGN_CONDITIONS_AND_TRIBUTES.md).

**4117 / 5062 (81.33%)** source-address coverage, **945** known entries open; **272 cpp / 252 headers**, **2252 naming rows / 57 unfinished boundaries**. Five original entries are newly mapped; the enclosing goal/result dispatchers remain partial and excluded. Eight affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming and whitespace checks passed. Windows ASM/Game.exe establish behavior; Mac supplies names only. No build, tests or game run occurred. User-owned untracked CombatTasks remain untouched and excluded.

The refreshed 35-asset audit connects 818 previously missing goal occurrences and 113 result occurrences; story-only changes are 777 and 109. Remaining story source branches: 21 goal types / 185 occurrences and 63 result types / 1,437 occurrences; all assets: 30 / 277 and 69 / 1,560. These are static script counts, not gameplay completion. PlayerDied's predicate is connected; periodic automatic player defeat in 00409BCC and player-leaves/object modification, broader vehicle/AI work and other script actions remain unfinished. See [CAMPAIGN_MAP_ROUTINES.md](CAMPAIGN_MAP_ROUTINES.md).

Next logical block: campaign object creation/removal/ownership and commands, starting with first-map ChangeHousesPlayerId and cruise SendVehicle/SelectHuman; then remaining object/resource/combat predicates and wonder/resource/overlay actions. Follow native helpers through their actual dependencies; preserve still-open engine/weather/drawing boundaries.

## Campaign result/submap lifecycle checkpoint - 2026-10-10

Windows MissionWon/MissionFailed/AllowMap results now set their original flags, print strings, dispatch local-player jingles and multiplayer trigger leaves, and persist scripted map unlocks. StartSubMission/EndSubMission requests now execute the complete parent save/load/embed/restore/remove/callback orchestration. The mission scan returns false immediately after either transition without accessing a replaced manager. SetImportHumanFlag modifies the original human flag byte for matching mission-check IDs. The existing control override consumer and GUI submap callbacks were verified; no generic character-transfer routine is inferred. Maptype 00510830 is named within its unchanged save layout, and shared string-output 004A30C0 recovers its original external name. See [CAMPAIGN_LIFECYCLE.md](CAMPAIGN_LIFECYCLE.md).

**4112 / 5062 (81.23%)** source-address coverage, **950** known entries open; **272 cpp / 252 headers**, **2247 naming rows / 57 unfinished boundaries**. Five original entries are newly counted: four new bodies and the completed mission scan. Thirteen affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming and whitespace checks passed. Windows ASM/Game.exe establish behavior; Mac header supplies names only. No build, tests or game run occurred. User-owned untracked CombatTasks remain untouched and excluded.

The refreshed 35-asset campaign audit removes 154 missing result occurrences and connects 31 formerly partial submap requests. Story-only inventory: 29 missing goal types / 962 occurrences and 65 missing result types / 1,546 occurrences; all audited assets: 38 / 1,095 and 71 / 1,673 respectively. These are static script counts, not gameplay completion. Multiplayer goal loading/tick, other script actions/goals and object-import behavior remain unfinished. See [CAMPAIGN_MAP_ROUTINES.md](CAMPAIGN_MAP_ROUTINES.md) and its CSV inventories.

Next logical block: FindPosByHumans, FindPosByPlayersMapMoveable, HumansDied and PayTribute, then required object creation/ownership/commands and wonder/resource/overlay actions. The enclosing goal/result dispatchers and miscellaneous engine callback remain excluded from completion counts. Broader AI/vehicle/weather/drawing boundaries remain separate work.

## Particle data/creation/tick and callback checkpoint - 2026-10-10

The particle static-data/effect constructors and complete creation/generator/wind/game-tick tree are reconstructed and connected to the existing drawing/movement path. Engine callback branches 0x1C and 0x23..0x27 now reach building smoke generators, arrows, falling smoke, direct/directional/hit effects and the native human scream. Three link dummies are removed. The preceding assumption that 005689DC was isolated zero storage is corrected: Windows constructor/loader indirect writes establish the whole four-entry munition-ID table at 005689D0..005689DF. Human animation event 27 is also corrected and connected as native signed stock modification; other building-event contexts retain their explicit guard. See [PARTICLE_SIMULATION.md](PARTICLE_SIMULATION.md).

**4107 / 5062 (81.13%)** source-address coverage, **955** known entries open; **271 cpp / 251 headers**, **2243 naming rows / 58 unfinished boundaries**. Thirty-six original entries are newly mapped. Ten affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming and whitespace checks passed. Windows ASM/Game.exe establish behavior; Mac supplies names only. Numeric x87/double/math differences remain documented; no build, tests or game run occurred. User-owned untracked CombatTasks remain untouched and excluded. Source integration does not establish runtime completion of every engine/combat effect.

Next logical block: campaign win/fail/unlock and the start/return-submap orchestration/import chain, then frequently used missing mission predicates and actions. The concrete campaign audit remains valid: [CAMPAIGN_MAP_ROUTINES.md](CAMPAIGN_MAP_ROUTINES.md). Remaining miscellaneous engine callbacks, full enclosing draw audit, weather and broader AI/vehicle boundaries are separate unfinished work.

## Particle drawing and campaign-map audit - 2026-10-10

The Windows particle drawing/movement subtree is reconstructed: sort/bottom draw 0048688E, row drawing 004869D0, native field hook 004912E3..004912F5, drawing dispatcher 00486A40, frame/valency/death/direction updates and all seven movement modes. The former global dummy binding is replaced by the namespaced production implementation. The five helper names and comparison global 005689DC remain explicitly unresolved rather than guessed. See [PARTICLE_DRAWING.md](PARTICLE_DRAWING.md). Particle static-data constructors, creation and generator/wind/tick remain unfinished; this does not establish visible smoke/projectile effects. The enclosing field remains excluded pending its full audit.

The user-requested campaign audit concretely reconciles Windows goal/result jump tables with installed scripts and current HEAD. All 27 story-campaign assets plus seven tutorials and one additional declared campaign-type submap are inventoried: 35 assets / 1,157 mission groups. Story maps alone use 29 missing goal types and 69 missing result types; start/end-submap results only set request flags. Win/fail/unlock, import/parent restoration, tribute/wonder checks and special marker/weather/overlay actions are proven open boundaries. Main campaign 3 has eight main maps and nine submaps; no separate routine per main map was found in the inspected shared loader/interpreter tree. See [CAMPAIGN_MAP_ROUTINES.md](CAMPAIGN_MAP_ROUTINES.md), [CAMPAIGN_MAP_COVERAGE.csv](CAMPAIGN_MAP_COVERAGE.csv) and [CAMPAIGN_MAP_SCRIPT_AUDIT.csv](CAMPAIGN_MAP_SCRIPT_AUDIT.csv). No campaign behavior was invented or changed by the audit.

**4071 / 5062 (80.42%)** source-address coverage, **991** known entries open; **269 cpp / 249 headers**, **2220 naming rows / 61 unfinished boundaries**. Eleven original entries are newly mapped and one drawing dummy removed. Four affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming and whitespace checks passed. Windows ASM/Game.exe establish behavior; Mac supplies names only. Numeric x87/atan2 precision limits remain documented. No build, tests or game run occurred. User-owned untracked CombatTasks remain untouched and excluded.

Next logical block: complete particle static-data/effect loading and creation/tick before claiming particles finished; then campaign win/fail/unlock and the start/return-submap orchestration/import chain, followed by the frequently used missing mission predicates and actions. Continue the broader profession/AI/vehicle scope with its existing explicit boundaries.

## Guideposts/construction markers/repair sound checkpoint - 2026-10-10

The Windows guidepost drawing tree at 004956B7/004958FB is reconstructed and connected, including selection outlines, player/unowned palettes, back/post/front order, twenty-arm limit and the native zero/one qsort comparator. Existing angle/image/hit-test helpers are shared with selection. Road and palisade construction-marker branches now draw their original player signs, fallback boxes and debug markers. Repair sound starts or updates the original first wave from the visible damaged-house count. A confirmed wrong 0x9C-stride lookup in landscape transition-phase selection is corrected. See [GUIDE_AND_CONSTRUCTION_MARKERS.md](GUIDE_AND_CONSTRUCTION_MARKERS.md).

**4060 / 5062 (80.21%)** source-address coverage, **1002** known entries open; **268 cpp / 248 headers**, **2215 naming rows / 62 unfinished boundaries**. This block adds four mapped original entries; marker and sound branches belong to the still-excluded field entry. Ten affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only. Existing x87 math-adapter differences remain explicitly documented.

Next logical block: particle sorting/bottom drawing at 0048688E, row drawing at 004869D0 and the particle draw subtree at 00486A40. The enclosing 004911D5 remains partial because its particle-line hook at 004912E3..004912F5 is still missing; it stays excluded from coverage. Full engine preview/gameplay completion requires those dependencies and user runtime confirmation. Continue the broader open profession/AI/vehicle scope afterward. User-owned untracked CombatTasks remain untouched and excluded.

## House/construction/demolition rendering checkpoint - 2026-10-10

The complete Windows house drawing subtree at 00495B57 is reconstructed and connected to the foreground map path: completed/building/upgrading images, animated and landscape overlays, door images, selection outlines, resident/worker/soldier signs, incense/fire and separate back/front demolition smoke. Native construction/collapse/ranged masks and shadow masks now support 16/32-bit destinations. Original fire RNG order, bottom-row collapse displacement, lazy debug-color exit registration and repair/music counters are retained. Existing house hit-test/sign descriptors are shared with the drawing unit. See [HOUSE_DRAWING.md](HOUSE_DRAWING.md).

**4056 / 5062 (80.13%)** source-address coverage, **1006** known entries open; **267 cpp / 247 headers**, **2210 naming rows / 62 unfinished boundaries**. This block adds 18 mapped original entries. Twelve affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming inventory and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Correction to the prior checkpoint: 004956B7 draws guideposts; construction is inside 00495B57. Next logical block: guidepost drawing at 004956B7/004958FB, then road/wall construction markers, particles and repair-hammer sound. The enclosing 004911D5 remains partial and excluded; full preview/gameplay completion requires its remaining branches and user runtime confirmation. Continue the broader open profession/AI/vehicle scope afterward. User-owned untracked CombatTasks remain untouched and excluded.

## Building panel/upgrade/stock audit - 2026-10-10

The building panel and its upgrade, production/input/output, minimum-stock and inhabitant command trees were checked against Windows assembly/Game.exe. The extra 16-pixel gap before the home family count is corrected. Existing stock sorting already calls the native special-character-aware comparator and remains unchanged. Windows creates no extra production-counter/stop controls despite retaining their message handlers; no Mac behavior was imported. See [HOUSE_PANEL.md](HOUSE_PANEL.md).

Coverage remains **4038 / 5062 (79.77%)**, **1024** known entries open; **266 cpp / 246 headers**, **2191 naming rows / 62 unfinished boundaries**. This fixes an existing function and adds no reconstructed entry. Seven affected/integration units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred.

Next logical block: the house drawing tree at 00495B57 and guidepost tree at 004956B7 and its foreground integration into the still-partial 004911D5. Building previews and effects remain unfinished; do not report all menus or gameplay complete. Continue the broader outstanding profession/AI/vehicle scope afterward. User-owned untracked CombatTasks remain untouched and excluded.

## Construction menu/building selection checkpoint - 2026-10-10

The user clarified that "building list" means the construction menu/building selection. The empty static-engine-house loader dummy is replaced by the complete Windows constructor/parser, including all 26 tokens, 0x8BC-byte records, building/stage mappings, construction costs, images, overlays, areas and effect points. The whole singleton/token region is represented with verified offsets. Distinct record constructors and the shared coordinate constructor are connected. The existing construction window, filtering, help/cost previews and placement-mode transition were checked against Windows and already implement their native paths. The installed Windows houses.cif contains 90 engine definitions and 123 stages; it supplies configuration only. See BUILD_MENU_SELECTION.md.

**4038 / 5062 (79.77%)** source-address coverage, **1024** known entries open; **266 cpp / 246 headers**, **2191 naming rows / 62 unfinished boundaries**. Eleven affected/integration units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Next logical UI block: the building panel after placement, upgrades and production/stock controls. Runtime construction-menu appearance, availability and placement need confirmation in the next user game run. Continue the broader ALL-professions and outstanding effects/AI/vehicle scope afterward. User-owned untracked CombatTasks remain untouched and excluded; source coverage is not gameplay completion.

## Building list and defensive-arrow checkpoint - 2026-10-09

All 55 building logic IDs (54 actual types plus the None sentinel) are inventoried with installed Windows names/configuration, current source areas and explicit remaining dependencies in BUILDING_LIST.md/.csv. House_DoOneGameTick now includes the full native civilian-defense arrow branch and House_l_Military_StartArrow, with original target-search/RNG order and delayed building damage. The statistics list was checked against Windows and already matches its native behavior.

**4032 / 5062 (79.65%)** source-address coverage, **1030** known entries open; **265 cpp / 245 headers**, **2186 naming rows / 63 unfinished boundaries**. Affected units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Next: remaining building helper entry mappings, projectile/hit/smoke visuals and original AI defense-group bodies, then vehicle/sea consumers and mission-specific wonder goals. Continue the earlier ALL-professions scope; source integration and the building inventory do not establish runtime completion. User-owned untracked CombatTasks remain untouched and excluded.

## Aggressive animal combat checkpoint - 2026-10-09

The full aggressive-animal enemy search, attack-position selection, chase, leader-target propagation and melee-hit event are connected. Four held AI/path/stay/event boundaries are removed. The native shared attack-position record is reused by humans and animals. See ANIMAL_MILITARY_TASKS.md.

**4030 / 5062 (79.61%)** source-address coverage, **1032** known entries open; **265 cpp / 245 headers**, **2185 naming rows / 64 unfinished boundaries**. Affected units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Next user-requested block: building list and building behavior audit, including the outstanding defensive-arrow tick. Remaining profession dependencies include particle visuals, AI group notification bodies and vehicle/sea simulation; runtime confirmation is pending. User-owned untracked CombatTasks remain untouched and excluded.

## Weapon-hit events and delayed projectile damage - 2026-10-09

Human attack event 25 is connected for landscape, house, animal, vehicle and human targets. Native shooting scatter and moving-target prediction now schedule delayed impacts; the delayed manager tick, hit-list rules, area damage, experience and miss callbacks are reconstructed. House, vehicle and human damage responses preserve armor, direction, amulets, difficulty, flee/retaliation commands, messages, happiness and native side-effect order. The delayed tick dummy is removed; three original AI group notification dependencies are explicitly isolated and excluded from completion counts. See WEAPON_HITS_AND_DELAYED_ACTIONS.md.

**4020 / 5062 (79.42%)** source-address coverage, **1042** known entries open; **264 cpp / 243 headers**, **2179 naming rows / 68 unfinished boundaries**. Affected units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Continue ALL remaining profession dependencies: aggressive animal counterattacks and pursuit, particle visuals for arrows/hits/smoke, AI defense-group notifications, defensive arrows and vehicle/sea simulation. Event 27 remains held. No runtime completion is claimed. User-owned untracked CombatTasks remain untouched and excluded.

## Hunting and dragging-animal checkpoint - 2026-10-09

Hunter cadaver collection and prey search, the shared attack start/finish work tree, non-AI pursuit recovery, all 29 task-finish entries and dragging-animal vehicle entry are connected. Complete supporting animal damage/anger/flee routines are reconstructed; weapon-hit event 25 and projectiles remain held. See [HUMAN_HUNTING_AND_ATTACKS.md](HUMAN_HUNTING_AND_ATTACKS.md).

**4002 / 5062 (79.06%)** source-address coverage, **1060** known entries open; **261 cpp / 241 headers**, **2159 naming rows / 66 unfinished boundaries**. Affected units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Continue ALL remaining profession dependencies: weapon-hit/projectiles, animal combat, defensive arrows and remaining vehicle/sea simulation. Live hunting is not yet complete; runtime confirmation is pending. User-owned untracked CombatTasks remain untouched and excluded.

## Vehicle production checkpoint - 2026-10-09

Jobs 9/10 now share the native carpenter/armorer dispatcher and complete vehicle-builder work tree: site search and placement, material collection, delivery, construction animation and recovery. The occupied-house tick replaces its empty dummy and converts completed construction sites into vehicles; defensive-arrow work remains explicitly partial. See [HUMAN_VEHICLE_PRODUCTION.md](HUMAN_VEHICLE_PRODUCTION.md).

**3966 / 5061 (78.36%)** source-address coverage, **1095** known entries open; **257 cpp / 237 headers**, **2126 naming rows / 68 unfinished boundaries**. Affected units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Continue ALL remaining professions: hunter/combat and remaining vehicle/sea simulation, including defensive-arrow and animal combat/vehicle dependencies. Vehicle-builder source integration is complete; runtime confirmation is pending. User-owned untracked CombatTasks remain untouched and excluded.

## Trader checkpoint - 2026-10-09

Job 25 now executes domestic and foreign trade routes, its four distinct goods-selection routines, stock/minimum checks, batch/capacity accounting, pickup/delivery and vehicle approach. Commander and maximum-stock accessors are connected. See [HUMAN_TRADER_JOBS.md](HUMAN_TRADER_JOBS.md).

**3946 / 5061 (77.97%)** source-address coverage, **1115** known entries open; **256 cpp / 236 headers**, **2107 naming rows / 68 unfinished boundaries**. Affected units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Continue ALL remaining professions: hunter/combat and carpenter/armorer vehicle production, plus remaining vehicle/sea simulation dependencies. Carrier and trader work trees are connected; their vehicle gameplay is not yet confirmed. User-owned untracked CombatTasks remain untouched and excluded.

## Carrier and vehicle entry checkpoint - 2026-10-09

Carrier ground warehouse and attached-vehicle work paths, demand searches, stock callbacks, explicit-source pile selection and task 21 startup/finish are reconstructed. The actual human vehicle-entry dummy is replaced. See [HUMAN_TRANSPORT_JOBS.md](HUMAN_TRANSPORT_JOBS.md). Vehicle simulation and sea navigation still require their own remaining branches.

**3933 / 5061 (77.71%)** source-address coverage, **1128** known entries open; **255 cpp / 235 headers**, **2096 naming rows / 68 unfinished boundaries**. Affected units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Continue ALL remaining professions: trader, hunter/combat and carpenter/armorer vehicle production, with their vehicle/sea dependencies. Menu exit/fullscreen masks/32-bit selection are committed; runtime confirmation remains pending. Do not report all professions complete. User-owned untracked CombatTasks remain untouched and excluded.

## Breeder and civilian animal checkpoint - 2026-10-09

Ground breeder startup, animal stock recount/adoption, slaughter approach, output flushing and newborn production are connected. Animal conditions now run in the common map-moveable tick: growth, civilian movement/path callbacks, leader/group handling, food, idle/stay, house interaction and birth-center updates are connected. Five partial animal boundaries explicitly retain pending combat/vehicle branches and are excluded from completion counts. See [HUMAN_ANIMAL_JOBS.md](HUMAN_ANIMAL_JOBS.md).

**3916 / 5061 (77.38%)** source-address coverage, **1145** entries open; **254 cpp / 234 headers**, **2079 naming rows / 69 unfinished boundaries**. Affected units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only. All remaining professions are still the active user request: hunter, carpenter/armorer vehicle production, carrier/trader and vehicle/sea paths, plus unresolved animal combat and vehicle dependencies. Do not report all professions complete. User-owned untracked CombatTasks are untouched and excluded.

## Farmer, herb guy, fisher, jester and druid checkpoint - 2026-10-09

Five previously missing ground job branches (18/22/28/29/30) are connected with their complete native work paths: planting/cultivating/harvesting and stock relief, shore selection and fishing, entertainment, and druid production/stock/healing. Search callbacks, fish-swarm consumption and catch/retry events are included. The shared needs/task/movement machinery remains in use. The user now confirms soldiers and collectors work well; these five additional professions have no runtime confirmation yet. See [HUMAN_OUTDOOR_SOCIAL_JOBS.md](HUMAN_OUTDOOR_SOCIAL_JOBS.md).

**3874 / 5061 (76.55%)** source-address coverage, **1187** known entries open; **252 cpp / 232 headers**, **2045 naming rows / 64 unfinished boundaries**. Affected cpp units passed MSVC Win32 `/Zs`; project/filter checks passed. No Debug/Release build, tests or gameplay run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only. The known-entry inventory grew by two discovered helper entry points.

The current user request still requires ALL remaining professions: hunter, breeder, carpenter/armorer vehicle construction, carrier/trader and vehicle/sea-dependent paths with their dependencies. Continue these full trees; do not report all professions complete. Exit/fullscreen RGB masks/32-bit selection were committed as 56952be; the user localized the tint to GUI and transition pictures in fullscreen only and excluded the screenshot. See MAIN_MENU_STATE.md for the requested compatibility changes and pending runtime colour confirmation. User-owned untracked CombatTasks remain untouched and excluded.

## Collector job and map collection checkpoint - 2026-10-09

Job 8 now dispatches to the complete native collector body 0046C876, with its generic 0047603F and distinct non-bio/bio map collection paths. Search around the work center, harvesting/pickup/store selection, retries, future-stock reservations, landscape split/grow/transform/plant events and engine edit wrappers are connected. The shared needs/task/movement machinery is retained. User feedback confirms the previous soldier routine now behaves well; the new collector correction has no runtime confirmation yet. See [HUMAN_MAP_COLLECTION.md](HUMAN_MAP_COLLECTION.md).

**3839 / 5059 (75.88%)** source-address coverage, **1220** known entries open; **251 cpp / 231 headers**, **2013 naming rows / 64 unfinished boundaries**. Eight affected/new cpp units passed MSVC Win32 `/Zs`; source/project checks passed. No Debug/Release build, tests or gameplay run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Next: remaining outdoor job dispatch (farmers, hunter/breeder, fisher, carrier/trader and vehicle production) and their complete dependencies. Combat tasks, military intermediate-point callbacks and the full house tick remain separate open blocks. The untracked user-owned CombatTasks files remain untouched and excluded. A map collector source correction does not establish all professions or gameplay complete.

## Soldier movement/job continuation checkpoint - 2026-10-09

Ground soldier/hero jobs 31..47 now dispatch to the complete native 0046F41A body. The former unsupported job could leave the walking atomic active after arrival; the reconstructed path selects idle or a return walk. Its enemy/landscape search and sector cache helpers are reconstructed as a complete related family. Windows ASM/Game.exe establish behavior; the Mac header supplies names only. See [SOLDIER_MOVEMENT.md](SOLDIER_MOVEMENT.md).

**3806 / 5059 (75.23%)** source-address coverage, **1253** known entries open; **248 cpp / 228 headers**, **1989 naming rows / 64 unfinished boundaries**. This is not verified gameplay completion. Six affected/new calling cpp units passed MSVC Win32 `/Zs`; source/project checks passed. No Debug/Release build, tests or gameplay run occurred.

Next: attack/pursue task startup, finish/failure and atomic events, then arrows/delayed hits and the full house tick. Selected targets still reach unsupported combat tasks independently; the intermediate-point military callback and vehicle-dependent job dispatch remain partial. User-owned untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded. Do not infer full combat completion from the ground-job correction.

## Military target search and happiness checkpoint - 2026-10-09

The complete six-function enemy target-search family and happiness callback tree are reconstructed from Windows. Happiness now recalculates all four contributions and event values, handles attacks/deaths and updates the player total. The former callback partial and civilian-death dummy are removed. Native full-grid human iteration and the direct-coordinate sector cursor constructor are connected. Mac headers supply names only. See [MILITARY_TARGETS_HAPPINESS.md](MILITARY_TARGETS_HAPPINESS.md).

**3795 / 5059 (75.01%)** source-address coverage, **1264** known entries open; **245 cpp / 227 headers**, **1981 naming rows / 64 unfinished boundaries**. This is not verified gameplay completion. MSVC Win32 `/Zs` and source/project checks passed; no Debug/Release build, tests or gameplay run occurred.

Next: arrows and delayed weapon-hit execution/application, then the complete HouseArray tick 0040ACE5 / House_DoOneGameTick 00433694. That tick remains a link dummy, so timed demolition, vehicle-construction conversion and military house damage remain unfinished. The target searches and house-loss happiness helper are ready; 0043A9B1 searches houses, not vehicles. User-owned untracked CombatTasks files remain untouched and excluded.

## Periodic house second checkpoint - 2026-10-09

HouseArray_DoOneGameSecond 0040AD1D now invokes the complete House_DoOneGameSecond 00433BF9. Native builder recruitment, religion/hitpoint effects, production/home-quality upkeep, stock/request smoothing, overlay expiration and civilian defender reassignment are connected. Windows ASM/Game.exe establish behavior; the Mac header supplies names only. See [HOUSE_CONSTRUCTION_LIFECYCLE.md](HOUSE_CONSTRUCTION_LIFECYCLE.md).

**3772 / 5059 (74.56%)** source-address coverage, **1287** known entries open; **242 cpp / 225 headers**, **1959 naming rows / 66 unfinished boundaries**. This is not verified gameplay completion. Only MSVC Win32 `/Zs` and source/project consistency checks ran; no Debug/Release build, tests or game run.

Next: the complete HouseArray tick 0040ACE5 / House_DoOneGameTick 00433694 tree, including destruction timer, vehicle conversion, first-contact reporting, enemy searches, arrows and delayed weapon hits. The tick remains an isolated dummy, so timed demolition and military house damage are still unfinished. User-owned untracked CombatTasks files remain untouched and excluded.

## House construction and lifecycle checkpoint - 2026-10-09

House builder execution, material search/collection, build events, repairs and construction/upgrade completion are reconstructed from Windows. House placement now allocates and initializes actual houses, including terrain/resource/map registration and sector attachment. Demolition start and house release preserve inhabitants, callbacks, ruin placement and salvage. Five former partial/dummy entries are removed. `House_Repair_IsFinished` corrects the misleading predicate name at 0043339A. See [HOUSE_CONSTRUCTION_LIFECYCLE.md](HOUSE_CONSTRUCTION_LIFECYCLE.md).

**3768 / 5059 (74.48%)** source-address coverage, **1291** known entries open; **241 cpp / 225 headers**, **1956 naming rows / 67 unfinished boundaries**. This is not verified gameplay completion. The local file `.codex-work/docs/FUNCTION_PROGRESS_2026_10_09.json` records the production-only audit.

Demolition is not yet complete during gameplay: HouseArray_DoOneGameTick 0040ACE5 remains an isolated dummy; DoOneGameSecond was connected in the newer checkpoint above. Next: their complete house tick 00433694 and second 00433BF9 trees, including destruction timer, vehicle conversion, automatic builder recruitment, stock/overlay maintenance, defence and delayed hit dependencies. No new dummies or guessed military branches were added. Only MSVC Win32 `/Zs` and source/project consistency checks ran; no Debug/Release build, tests or game run. The user-owned untracked CombatTasks files remain untouched and excluded.

## Menu, keyboard and selection checkpoint - 2026-10-09

Primary keyboard commands and all four in-game cheats, network information, general menu families and the GUI logic callback are connected from Windows. All selection families now connect, including the single-house window, goods and inhabitants. House controls cover construction, upgrading/removal, defence, homes, production, stock pages/minimums, foreign trading and help. Character observer/follow/equipment command IDs, production help links and desktop window clamping are corrected. Main-menu cursor/tooltips and message-presentation help links replace unfinished entries. See [MENUS_KEYBOARD_COMPLETION.md](MENUS_KEYBOARD_COMPLETION.md).

**3726 / 5059 (73.65%)** source-address coverage, **1333** known entries open; **239 cpp / 225 headers** with filter parity, **1917 naming rows / 72 unfinished boundaries**. Compared with the pre-menu checkpoint, the union adds two previously unlisted source entries: human-list toggle 004A1F8A and overview close 004A28EC. This is not verified gameplay completion. The local file `.codex-work/docs/FUNCTION_PROGRESS_2026_10_09.json` records the production-only audit.

Single-human constructor/message/draw, manager variable initialization and the full world-display message dispatcher replace their partial/dummy entries. All 28 input modes, human/vehicle default movement, selection validation and map highlighting now replace five further stale partial entries. House placement retains ordered terrain/area/nearby-human checks, and vehicle movement retains formation targets and commander response. The structured house accessor 0048089E is correctly named GetHouseDataPtr. General menu/input audits are reconciled with current source. Both scroll paths use the separate native 300/600/1200 pixels-per-second getter; manager lists now use their real destructors. Clock/rotation and volume math adapters replace zero dummies on ordinary numeric paths but retain explicit CRT error/status partial boundaries. Global defence and house upgrade start/stop now execute their complete Windows trees, including nearby gates, construction stock/deliveries, worker refresh and surplus-material return. Only MSVC Win32 `/Zs` and source/project consistency checks ran; no Debug/Release build, tests or game run. Remaining priority: downstream house placement/removal and other command-execution boundaries. The user-owned untracked CombatTasks files remain untouched and excluded.

## Road, palisade and gate construction - 2026-10-08

Connected native start/end selection, reachable-point highlighting, complete vector previews, network flag creation/cancellation, per-second worker assignment, builder job/material/pickup/delivery/build/repair execution and road map-edit callbacks. Own flag selection now opens native camera/cancellation windows. Configured road/wall/gate shortcuts and gate eligibility/placement/state commands are connected. Windows ASM/Game.exe establish all behavior and layouts; Mac headers supply names only. Twelve link dummies were replaced. See [WALL_ROAD_CONSTRUCTION.md](WALL_ROAD_CONSTRUCTION.md).

**3352 / 5057 (66.28%)** source-address coverage, **1705** known entries open; **212 cpp / 198 headers** with filter parity, **1170 naming rows / 99 unfinished boundaries**. Seventy mappings added; the candidate denominator is unchanged. Partials/dummies and user-owned untracked CombatTasks files remain excluded. This is not verified gameplay completion.

All seventeen affected cpp units passed MSVC Win32 `/Zs`; only pre-existing manager C4805 warnings remain. No Debug/Release build, tests or gameplay run. The general construction chooser, house placement/highlighting and house-builder tree are next. Gate command execution is real; the general landscape window and its controls remain pending. House branches, other input/window/hotkey branches and vehicle work remain explicit partial boundaries.

## Tooltip, exploration redraw and double-click correction - 2026-10-08

Restored Windows tooltip string group 2 (IDs 60..69) and the native profession separator, correcting the Hardware/experience label and unrelated name prefix. Specialized the existing integer Gouraud pixel variants, removed unused color updates/runtime arithmetic calls and enabled optimized Debug compilation for that renderer alone. Exploration/fog/redraw rules remain native. The modern Windows input adapter now delivers button transitions separately and preserves event timestamps for the existing 400 ms detection and 250 ms following-click suppression; profession filters, military grouping and selection area are unchanged. Adapter timing is an intentional compatibility correction, not new native behavior. See [TOOLTIP_EXPLORATION_DOUBLECLICK_REGRESSION.md](TOOLTIP_EXPLORATION_DOUBLECLICK_REGRESSION.md).

All five changed cpp units passed MSVC Win32 `/Zs`. Project evaluation and filter parity were checked without building. No tests/gameplay run or measured performance improvement. Runtime confirmation remains pending. Source-address checkpoint unchanged: **3282 / 5057 (64.90%)**, **1775** open candidates; **208 cpp / 194 headers**, **1120 naming rows / 113 unfinished boundaries**. User-owned untracked combat files remain untouched and excluded. Next reconstruction block: multiple/non-human selection windows and their controls.

## Complete contextual default-command preparation - 2026-10-08

Connected the complete Windows hover/default preparation tree for single/group humans and vehicles: house/home/work/builder/school assignment, trader house membership, attacks, vehicle entry, job/production changes, equipment, chests, build flags, exploration and movement. Both 0x14-byte command buffers, native priorities, selected-member exclusion, modifier handling, group comparison/overwrite branches and failure reset are preserved. Vehicle command availability, parent allocation/revision validation, build-flag owners and worker-job capacity helpers are connected. Existing command queue insertion and human responding voices consume the prepared commands. Windows ASM/Game.exe establish behavior; Mac headers supply names only. See [DEFAULT_INTERACTION_COMMANDS.md](DEFAULT_INTERACTION_COMMANDS.md).

**3282 / 5057 (64.90%)** source-address coverage; **1775** known candidates remain open. The numerator increased by 16; the denominator is unchanged. Completed 004BABE0 is removed from unfinished boundaries, while moved 0042789B is counted once. This is not verified gameplay completion.

All four changed/new cpp units passed MSVC Win32 `/Zs`; no build/tests/gameplay run. Project/filter parity: **208 cpp / 194 headers**. Inventories: **1120 naming rows / 113 unfinished boundaries**. User-owned untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded.

Next logical block: multiple/non-human selection windows and their controls, then remaining vehicle/general default-action dispatch and construction modes/previews. Road/wall vector previews and downstream combat/vehicle/task boundaries remain independently unfinished.

## Native in-game cursor and hover presentation - 2026-10-08

Connected the real 0x14-byte in-game mouse-pointer object and its complete two-slot Windows table. Hardware/software pointer selection, normal/pressed/override sprites, 500 ms tooltip delay, pointer/position change detection, CRLF splitting and outlined tooltip drawing follow 004BB78A/004BB7A7/004BB8FC/004BC24C. The existing single-line tooltip helper is now shared and its bitmap-reference/background-fill call sequence is corrected. Hover text retains both control schemes, human names/jobs/products/experience, own/enemy house main-type filtering, contextual human/vehicle command descriptions and landscape/animal information. Mac headers supply names only; all behavior and tables come from Windows ASM/Game.exe.

World-cursor callback 004B3264 now draws the ten bookmarks, valid target rings, house preview, human work-center marker and road/wall source signs. Its road/wall vector preview is explicitly partial: 0042871C/00427AEA and 0043E8AA remain link dummies, so the nonterminating dummy iterator is not invoked. The prepared-command dispatcher 004BB0C1 is complete for single/group humans and vehicles, including the revision/type-38 school chooser continuation. Preparing contextual commands in 004BABE0 remains partial; human 00435605/004362E9 and vehicle 00437AAF/00437D9F are the next logical block. Multiple/non-human selection windows and construction modes follow.

**3266 / 5057 (64.58%)** source-address coverage; **1791** known candidates remain open. The numerator increased by 20 and the candidate denominator by two; partials/dummies remain excluded. This is not verified gameplay completion. See [WORLD_CURSOR_HOVER.md](WORLD_CURSOR_HOVER.md).

Win32 `/Zs` passed for all twelve changed/calling cpp units; only existing manager C4805 warnings remain. No build/tests/gameplay run. Project/filter parity: **207 cpp / 193 headers**. Inventories: **1104 naming rows / 114 unfinished boundaries**. User-owned untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded.

## Character work markers and specialized input modes - 2026-10-08

Connected work-marker placement/work-area display (production buttons 10037/10038), native portrait right-click context actions and camera/follow call order. Character input modes 1/2/3/10/13/22/23/25/26 now validate point/object targets, enqueue the Windows commands, issue responding voices and cancel in native order. Native input-mode selection checks, object highlights, logic-tick highlight refresh, cursor target rings, cancellation notifications and overview exploration restoration are complete. Map highlighting remains partial for mode 12 and modes 14..21; downstream combat execution retains independent gaps.

Windows ASM/Game.exe establish all behavior, constants and layouts. Mac headers supply corresponding names only; outlined helper names without Mac correspondence are documented semantic adapters. See [HUMAN_SELECTION_COMMAND_MODES.md](HUMAN_SELECTION_COMMAND_MODES.md).

**3246 / 5055 (64.21%)** source-address coverage; **1809** known entries remain open. Captured partials/dummies remain excluded; this is not verified gameplay completion.

Win32 `/Zs` passed for ten changed/calling cpp units; existing manager C4805 warnings only. No build/tests/gameplay run. Production/filter inventory remains **206 cpp / 192 headers**; **1086 naming rows / 116 tracked unfinished boundaries**. User-owned untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded.

Next functional block: world-cursor rendering and contextual hover/default actions, then multiple/non-human selection windows and vehicle/construction modes. Keep keyboard hotkeys/chat/cheats and help/context popups as explicit separate boundaries.

## Native character choosers and contextual actions - 2026-10-08

Following `e5b6b79`, `InGameGuiHumanJobWindows.cpp/.h` reconstruct the complete Windows job, name, produced-good and school chooser blocks: 004A2683/004A26C2/004A2723/004A2761/004A27EA -> 004C736E..004C8274 and 004C8B04..004C9009. Job changes retain single/group availability, alphabetical sorting, the six-entry farmer table, pressed current-job rows, two-second refresh with scroll restoration, command 21 followed by optional 22, response voices, automatic workplace selection and frame registration/unregistration. School rows use the human tribe, native job byte +87 in 0x88-byte job records, both player enable/unlock arrays and original farmer exclusions. Commands 34/35 preserve the optional-good behavior, including the existing desired-good fallback for single-good jobs. School target mode 6 enqueues 32, tests house type 38, responds, cancels the mode and then opens the learning chooser in native order.

Name changes retain the 12-character continuous input handler, Enter/Escape distinction, active-window check, blinking cursor, native portrait drawing, direct dynamic-human rename and multiplayer UTF-16 discriminator-2 record. The previously empty primary-handler constructor 004BC31C now creates its real 0x2C-byte element and 18-slot table; message 004BC36F connects name-key forwarding and remains partial for other hotkey/chat/cheat paths. Produced-good rows distinguish available toggle buttons from locked text rows, retain the farmer's extra good-65 row, and enqueue 22 before the responding voice and deferred close. All four complete nineteen-slot chooser tables and contiguous farmer/tooltip/graphic regions were read from Game.exe. Folded validation 004C8238 is represented once; Mac headers supply names only.

`InGameGuiHumanActionButtons.cpp/.h` replaces action-layout/checksum dummies with their Windows single/group human branches. Group intersection, removal of action 12 only after the native multiple-member branch, priorities, five groups, placement, edge bends, symbols, tooltips, overlay ownership and selection-list lifetime follow 004A10CB. Checksum 004A1BF0 deliberately repeats the first list's hash in the group loop, as Windows does. Simple-command translation 004A480E is complete. World context helpers 004BB288/004BB2F2, manager entry 004A10A4, overlay insertion 004771BE/004B84ED and tooltip selection 004E1EC1 are connected. Character-window buttons 10005/10006 open job/name choosers. Default message 004A3EDB routes character actions and retains existing miscellaneous behavior.

Remaining explicit boundaries: action layout/checksum house and vehicle branches; default-handler non-character branches; input modes 1/2/3/10/13/22/23/25/26 and related military/work-marker targets; primary keyboard hotkeys/chat/cheats; portrait context handlers; multiple/non-human selection windows and remaining trader AI. These partial original entries remain excluded from function completion counts. Displaying a context button does not establish that every specialized input mode is implemented.

MSVC Win32 `/Zs` passed for twelve new/changed/calling cpp units; only existing manager C4805 warnings remain. No Debug/Release build, tests or gameplay run. Project/filter parity: **206 cpp / 192 headers**. Inventories: **1067 naming rows / 121 unfinished boundaries**, not gameplay coverage. User-owned untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded. The progress script now reads only project-registered production sources; see the refreshed source-address inventory in [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md).

Next: work-marker/specialized character input modes and portrait context handling, followed by multiple/non-human selection windows.

## Character command availability and assignment actions - 2026-10-08

Following `843e3c7`, `C2HumanUserControl.cpp/.h` reconstruct the complete Windows command-list tree 0040B01F -> 00434F37..004355AE and its 25-entry handler 0044071B..004408A5. Normal/alternate branches retain action order, duplicate/capacity semantics, tribe/age/job/family/vehicle checks, queue exclusions and native sorting priorities. Windows 0043550B repeats opcode 15 in the alternate pregnancy check; this is preserved. The two outlined predicates 00436A68/00436A97 remain inline with unresolved separate semantic identities. Mac headers supply names only.

The character window now includes native marriage/child, home assignment/detachment, workplace/builder assignment/detachment and vehicle entry/exit controls with original graphic/tooltip IDs, spacing and collapsed-control removal. Family actions enqueue 14/15/16; detachments enqueue 27/29/31/39. Target modes 4/5/7/8/9 send 26/28/30/36/37 after native house highlighting and target rings; mode 27 uses original vehicle eligibility/highlights and sends human 38 or vehicle 113. Command insertion precedes responding voices and cancellation. Vehicle passenger-capacity leaves reuse the existing revision-validating VehicleReference_IsAllocated rather than adding another implementation.

InputMode_Changed 004A0D01 replaces its link dummy, retaining fourteen ordered desktop hide/unhide calls and both native desktop-selection notifications on mode transitions. Camera-follow toggle 10004 connects the complete human/vehicle follow-state tree 004BB388..004BB57A, including revision validation, per-frame update before mouse-object checking and the native -25 pixel vertical offset.

The input/mouse/highlight dispatchers and character constructor/message/draw/layout remain partial. School mode 6 still requires its native learning chooser 004A27EA -> 004C8B04 and is not enabled; its map-highlight branch is reconstructed. Other contextual commands/action-button bar, work-marker controls, rename popup, portrait popup, multiple/non-human windows and trader AI remain open. Highlighting_Vehicles_Update 004CB137 is tracked as a new partial boundary for its unconnected enemy/other branches. Source reconstruction does not establish gameplay equivalence.

MSVC Win32 `/Zs` passed for all six changed/new cpp units and existing InGameGuiManager/InGameGuiSupportAccess callers; only pre-existing C4805 warnings appeared in the manager. No Debug/Release build, tests or gameplay run. Project/filter parity: **204 cpp / 190 headers**. Inventories: **1033 naming rows / 121 unfinished boundaries**, not coverage. Existing untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded.

Next: native school/job/name choosers and contextual action-button bar, then portrait popup and multiple/non-human selection windows. See [HUMAN_SELECTION_INFO_WINDOW.md](HUMAN_SELECTION_INFO_WINDOW.md).

## Trader routes and configuration - 2026-10-08

Following `d503457`, trader route assignment and detachment are connected through native mode 8 and network opcodes 36/37. The cursor clips its 14x14 highlight area before the native five-ring target search. Modal desktop hiding/restoration follows the Windows list order and activates the first window in list +78 on return to normal mode. Both configured mouse modes reach the assignment action.

The native 660x360 trader popup now includes both house views, sorted import controls, food substitutions, stock overlays, foreign trade-agreement choices, reset and camera focus. Opcodes 49/50/51 preserve restart-before-mutation order; close callbacks clear the character window's owned popup. Merchant route-slot/import/validity helpers, map trade-agreement loading and diplomacy cleanup replace two link dummies. Behavior, layout, constants and tables are confirmed from Windows ASM/Game.exe; Mac headers supply names only. The complete token region 004F3898..004F38A7, popup table 004F7A10..004F7A5B and outline offsets 004F7000..004F701F were inspected.

The broader input/highlighting dispatchers and single-human constructor/message/draw/layout remain partial. Other special modes, InputMode_Changed 004A0D01, command availability, vehicle attachment, contextual controls and trader job/task execution remain open. Source reconstruction does not establish a successful gameplay run. Native merchant validity preconditions and count-changing diplomacy traversal are retained; stock tooltip precision uses the existing float-returning helper instead of original x87 intermediates.

MSVC Win32 `/Zs` passed for all eleven changed/new cpp files and the three existing map/lifetime/savegame callers. No Debug/Release build, tests or gameplay run. Project/filter parity: **203 cpp / 189 headers**. Inventories: **1013 naming rows / 121 unfinished boundaries**, not coverage. Existing untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded.

Next: native command-availability tree 0040B01F -> 00434F37..004355AE and remaining contextual character actions, then rename/follow/context and multiple/non-human selection windows. See [HUMAN_SELECTION_INFO_WINDOW.md](HUMAN_SELECTION_INFO_WINDOW.md).

## Equipment chooser, experience pages and trader references - 2026-10-08

Following `387128f`, the character's equipment arrows open the native 420x300 chooser. Single/group availability, equip/drop commands, responses, window reuse and checksum-based closing follow Windows ASM/Game.exe. The missing nearby-equipment search now uses the existing map/stock/guide paths without random selection. Eleven experience pages, the fourteen strongest entries and native sorting/refresh are connected from the complete 004F67D0..004F6A8F static region.

The trader section displays valid route houses and selects them after allocation/revision checks. Route assignment, detachment and the trader configuration popup remain explicit boundaries; their native controls are present. Remaining basic/production/military contextual actions, rename/follow/context popups and multiple/non-human selection windows also remain open. Mac headers supply names only. Window constructor/message/draw/layout boundaries remain excluded from completion counts.

MSVC Win32 `/Zs` passed for all ten affected cpp files. No Debug/Release build, tests or gameplay run. Project/filter parity: **201 cpp / 187 headers**. Inventories: **969 naming rows / 122 unfinished boundaries**, not coverage. Existing untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded.

Next: native route/configuration and command-availability trees, then remaining selection/context windows. See [HUMAN_SELECTION_INFO_WINDOW.md](HUMAN_SELECTION_INFO_WINDOW.md) and current source; runtime confirmation is still required.

## Character relationships, equipment and production - 2026-10-08

Following `95e6483`, the single-human window connects partner/child/parent selection, home/workplace/attached-vehicle references, military status, equipment/item condition displays and production counters/locked-good experience. Production buttons enqueue native opcodes 0x16/0x17 and responding voices. Equipment is currently display-only; the chooser is still open. Relationships belong to the basic section; descriptor index 4 is equipment, correcting the earlier family-tab assumption.

The complete 100-entry dynamic-button handler and shared enabled/collapsed/variable-height layout follow Windows ASM/Game.exe. Mac headers supply names only. Native weapon/armor rows apply only to soldiers/heroes. The equipment enable predicate's original name at 00436A97 remains unresolved; its Windows byte/exception checks are preserved inline. The window remains partial and excluded from completion counts.

MSVC Win32 `/Zs` passed for all three affected cpp files. No Debug/Release build, tests or gameplay run. Project/filter parity remains **200 cpp / 186 headers**. Inventories: **950 naming rows / 122 unfinished boundaries**, not functional coverage. Existing untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded.

Next: equipment chooser 004A279D/004C8275 and native availability search, command-availability tree 00434F37, experience/trader sections and remaining contextual controls. Rename/follow/context popups and multiple/non-human windows also remain open. Read [HUMAN_SELECTION_INFO_WINDOW.md](HUMAN_SELECTION_INFO_WINDOW.md) and current source; runtime confirmation remains required.

## Single-character status window and startup briefing - 2026-10-08

The single-human selection path 004A0E9D now opens a native-size basic status window with animated preview, name, job/child age, health, food, stamina, social and religion bars, tooltips, focus and persisted collapse state. The needs eligibility is the Windows adult-age check, so adult women retain those bars. The complete tooltip storage, six descriptor records and 19-slot virtual table were inspected in Windows ASM/Game.exe. Shared drawing helpers use recovered original names, including CBitmap::Draw_Box for RGB colors. Mac supplies names only.

Startup briefings had a concrete propagation blocker: the priority-50 multiplayer-goal callback returned false and stopped CCallbackManager before the mission callback. AI, assistant and happiness callback dummies had the same wrong return. All four now preserve the native unconditional true return. Their missing subsystem dispatch remains explicit, partial and excluded from completion counts. The existing mission-script/36-tick/PlayCutscene/0x1B/popup chain remains authoritative; no unconditional briefing ID was invented. Installed script inspection found 59 automatic first-mission briefings (57 True, two TimeGone).

The character constructor, destructor, message handler, layout and drawing remain partial: other tabs, relationship/equipment/home/vehicle rows, rename/follow/context popups, multiple-human and non-human selection windows are still open. See [HUMAN_SELECTION_INFO_WINDOW.md](HUMAN_SELECTION_INFO_WINDOW.md) and [MISSION_BRIEFING.md](MISSION_BRIEFING.md) for evidence and exact boundaries.

MSVC Win32 /Zs passed for all five affected cpp files; existing manager C4805 warnings remain. No Debug/Release build, tests or gameplay run. Project/filter parity: **200 cpp / 186 headers**. Inventories: **932 naming rows / 122 unfinished boundaries**; these are not coverage counts. Existing untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded.

Next: complete remaining character tabs/actions from Windows ASM, then multiple-human/non-human selection windows and contextual commands. Runtime confirmation of the status panel and scripted startup briefing is still required.

## Human selection, world input, messages and voices - 2026-10-07

The standard mouse mode now connects click/drag/Shift/Control/double-click human selection, selected graphics and right-click land movement through the Windows command queue. Current animation sprites supply hit areas; original priorities and the 20-pixel fallback are retained. Selection validation/destruction handles stale references and group compaction. Hover markers connect family, home, workplace and house occupants. Middle-button panning, zoom and message camera focus use native display paths.

Top-row messages now open the reusable native presentation window, display their text/portrait, focus/select the referenced object and dismiss on confirmation. All 63 known message text types use the original dedicated message string array, including diplomacy and special items. Human generic voices and animation sound callback event 0x17 connect oriented sound playback with native fog/distance/random/overlap rules. The remaining engine misc callbacks stay unfinished.

Windows ASM/Game.exe supplies behavior and constants; Mac supplies names only. 0048B532 is correctly named InhabitantSign_GetLandscapeId; the distinct SoldierSign_GetLandscapeId is 0048B574. No inline assembly was added. C++ guide-angle math records the remaining x87 intermediate-precision difference. See [HUMAN_WORLD_INPUT_AND_MESSAGES.md](HUMAN_WORLD_INPUT_AND_MESSAGES.md) for exact boundaries and evidence.

Selection/action windows, contextual object commands, positive input modes/highlighting/modal handling, construction selection, help context windows and automatic camera controls remain unfinished and excluded from completion counts. The alternative mouse mode is also partial. Do not claim every GUI interaction is complete.

All affected translation units pass MSVC Win32 `/Zs`; no Debug/Release build, tests or gameplay run. Project/filter parity: **199 cpp / 185 headers**. Inventories: **913 naming rows / 117 unfinished boundaries**, not functional coverage. Existing untracked C2HumanCombatTasks.cpp/.h are preceding combat work and remain excluded.

Next: selection/action windows and the contextual default-action tree, then specialized input/camera paths. Runtime confirmation is still required for the requested selection/movement/message/voice behavior.

## Vehicle lifecycle and complete chest rewards - 2026-10-07

Following `d387c05`, C2VehicleLifecycle connects Windows vehicle allocation/init/free/exit, initial idle/animation/walking selection, native placement/overlap searches, passenger reset/movement checks and human/vehicle move-out/detachment. Reward 89 now uses the real allocator, so every Windows chest content branch is connected and event 33 has no chest-reward staging guard. Vehicle AI/tick/task/boarding/combat functionality is not implied complete.

The Windows misc-landscape manager constructor (0048A4BA) now loads both owned mapping arrays and weighted groups from the original misc.ini. Exact cadaver mapping (0048A86C/00480904) and random depot mapping (0048A8A1/004808F4) retain original matching order, random-call timing and type-zero fallback. The complete 00568548..00568567 storage/token/owner region and Windows token/name strings were inspected. Existing destruction retains ownership. The landscape-edit operation 0040B0C6 remains a link dummy, so animal/vehicle exit remain partial and excluded from completion claims.

Game.exe bytes correct the disassembly at 0044CA93: sign-extended imm8 FF writes -1. The path radius at 004F3B24 is 60. Movement readiness checks the inclusive commander slot and attached vehicles. Idle sends nearby humans away over the footprint; it does not directly change terrain blockage. Failed relocation returns in the native order. All behavior/offsets/constants come from Windows ASM/Game.exe; Mac supplied names only; the Windows depot-wrapper name remains unresolved and its current name is inferred from call sites.

Eight affected cpp files passed MSVC Win32 `/Zs`; no Debug/Release build, tests or gameplay run. Project/filter parity: **195 cpp / 185 headers**. Inventories: **862 naming rows / 109 unfinished boundaries**. Counts are inventories, not functional coverage. User-owned count.bat deletion remains excluded.

Next: finish the human attack/task-14 tree reachable from civilian idle, remaining outdoor and human vehicle workflows, creature AI/tick callbacks and death-edit dependencies. All reported freezes remain unconfirmed until runtime validation.

## Chest lifecycle, goods placement and animal allocation - 2026-10-07

Following `6a86206`, command 18, task 18 startup/completion/failure and atomic event 33 now connect the Windows chest tree (0043D34B/0044613F/0044621D/0046A310). Completion uses the native entry shared with guide completion; failure follows the inlined task-18 reset and generic continuation. C2HumanChest preserves large-chest eligibility, contents 1..96, sound/item callbacks, goods placement, production unlocks and human/animal rewards. Reward 89 alone remains held at the atomic reconstruction boundary because its vehicle allocator is link-only. Other native failures may proceed through that boundary. 0044CF1C stays partial/excluded until reward 89 is real.

00462176 now distributes goods at the origin and across same-continent hexagon rings, retaining partial placement and false on exhaustion. Its canonical return is bool; the old link dummy is removed. Animal allocation/init/free, atomic selection, leader validation and aggression membership are connected (0040AD95/0041C2A9/0041C358/0042DCE9/0042DE20/0042E82C/0042E9AE/0042EBC1/0042F2AD/0042F2D1/004471CA), including array-order neutral eviction only for non-neutral requests. The array iterator at 004402CD/00440408 retains signed scan bounds and unsigned distance comparisons. Animal death landscaping still uses existing external cadaver/edit dummies and is tracked partial; animal AI is not established complete. No new dummies were added. Shared carried-good clearing retains its callback.

All behavior/layout/constants/call order derive from Windows ASM and Game.exe; Mac is a naming reference only. Native oddities are retained: chest event resets an unreserved target before validating it, task 18 restores landscape target fields after walking startup, and allocation marks objects active before moveable allocation. All ten changed/new cpp files passed MSVC Win32 `/Zs` after corrections. No Debug/Release build, tests or gameplay run. Manifests: **194 cpp / 184 headers**; inventories: **825 naming rows / 112 unfinished boundaries**. Completion/coverage is not inferred from these counts. count.bat deletion remains user-owned and excluded.

Next: vehicle allocation/initialization/exit and reward 89, then remaining combat/outdoor/vehicle task trees, original Windows default-action correspondence and external death/AI dependencies. Every human workflow and all reported freezes remain unconfirmed.

## Human job, production and school commands - 2026-10-07

Following `2caf9c9`, C2HumanWorkCommands connects work/build/learning attachment and detachment, vehicle detachment, work-center assignment, job changes, production-counter changes, learning selections and execution restart (0043CB68/0043CCE3/0043CE7F/0043CF70/0043D181/0043D297/0043D4ED/0043DB6B/0043DDC6/0043DE68/0043DED7/0043E023). Commands 17, 21..25, 28, 30..32, 34/35 and 39 now dispatch. The shared house student predicate and job/good setters retain 004322E6/00432653/0043266A slot semantics. Initialization and command callers share the same declarations. Two attachment link dummies are removed.

Windows ASM/Game.exe supplies all behavior. Native prerequisite/validation order, distinct zero/one/two returns, 36 full equipment-record resets, equipment-dependent military job selection, repeated production setup and separate road/wall branches are retained. 0045BFEC is a confirmed no-op; no notification behavior is invented. The vehicle command predicts its next attachment position at 00438F6A and rereads the passenger binding after move-out. Mac supplies names only; 00436AAD remains an inferred Windows outlined helper name. Game.exe confirms opcode 3 is rejected by the command-check selector, so it is not treated as the missing default-action implementation.

All six touched/new cpp translation units passed MSVC Win32 `/Zs`. No Debug/Release build, tests or gameplay run. Project/filter parity: **193 cpp / 183 headers**. Inventories: **806 naming rows / 112 unfinished boundaries**. These are not completion counts. User deletion of count.bat is preserved and excluded.

Next: locate the Windows default-action entry and reconstruct chest/opening tasks, then combat/outdoor/vehicle trees and external death/AI NoWayFound dependencies. Defence and unsupported passenger-command boundaries remain explicit. Do not claim all human workflows or all reported freezes complete.

## Scout exploration and guide lifecycle - 2026-10-07

Following `e997683`/`13b3b2e`, C2HumanScout connects commands 19/20, task 19 guide building, task 20 exploration, native requeue/completion/failure and the eight-candidate unexplored-point search. Command 20 also retains the dispatcher's parameter-2 work-center update. Atomic event 20 now executes the scout-only animation-43 branch of 00468F5D; other construction branches remain held. C2GuideAccess implements 0041B061..0041B305, 0041B936, 00426885/004268DD and 00447283..004477CB: placement, reciprocal links, replacement/disconnection, invalidation/recalculation, insertion/removal, map-change passes and periodic goods scanning. Three guide link dummies are removed. Callers share the canonical bool fastcall insertion declaration.

Windows ASM/Game.exe supplies behavior; Mac supplies names only. Native goods recalculation and periodic scans deliberately differ in reset, distance and stock filtering. The native zero-point spacing exception and unconditional reciprocal count decrement are retained. 0045D56F/0045D5D1 names are inferred from Windows; separate Mac identities are unresolved. The generic build event remains explicitly partial.

All nine touched/new cpp files passed MSVC Win32 `/Zs` after corrections. No Debug/Release build, tests or gameplay run. Project/filter parity: **192 cpp / 182 headers**. Inventories: **789 naming rows / 114 unfinished boundaries**. These are not gameplay completion counts; historical 2584/5054 coverage remains historical. User deletion of count.bat is preserved and excluded from this block.

Next: default-action/job-change/work/learning assignment commands, then remaining combat/outdoor/vehicle/chest trees and external death/AI NoWayFound dependencies. Do not claim every human workflow or all reported freezes complete. Read current source and [HUMAN_FREEZE_RECOVERY.md](HUMAN_FREEZE_RECOVERY.md).

## Sector and guide routing follow-up - 2026-10-07

`68dd870` commits family, initial house attachment, training/exercise and idle opponent targets. This follow-up connects Windows sector/guide fallback at 00447CF1, sector selection/base points/WayPossible at 00450014..004507F1, real reciprocal connections at 0044FD3E and the complete special sector pathfinder at 004703AE..00471417 with its preferred-direction helper 0043EC04. Guide routing at 0041B30C..0041B3F3 and 0041B515..0041B935 preserves nearby-guide filtering, same-system compaction, forty search buckets, cached id/revision updates and the adjacent move point at 00447677. Sector routing includes the native civilian danger costs at 0045437A/00454405. Game.exe's sector-cursor jump table confirms direction 7 increments X as it decrements Y; the previous missing X increment is corrected.

Eight touched/new cpp files passed MSVC Win32 `/Zs` after the final corrections. No Debug/Release build, tests or gameplay run. Project/filter parity: **190 cpp / 180 headers**. Naming inventory: **761 rows**; unfinished inventory: **116 entries**. The two obsolete 00447CF1/0044FD3E unfinished entries and the connection link dummy are removed. Naming entries are not a completion count; historical 2584/5054 coverage remains historical. SSectorGridCursor::IsInsideMap is inferred from its confirmed Windows predicate, without a separately established Mac symbol.

Next: scout guide-building task/commands and guide lifecycle/recalculation, then default-action/job-change/work/learning assignment commands and remaining combat/outdoor/vehicle/chest trees. Guide insertion/removal/callback dummies still remain; this routing block alone does not complete guide creation. External death and AI NoWayFound dependencies remain. Do not claim all human workflows or all reported freezes complete. Read current source and [HUMAN_FREEZE_RECOVERY.md](HUMAN_FREEZE_RECOVERY.md); Windows ASM/Game.exe exclusively determines behavior, Mac supplies names only.

## Family, learning and idle target follow-up - 2026-10-07

`f5f12d4` commits the household/goods/equipment block. Current source additionally connects marriage/love/family home choice, child wishes/waits/birth and requeue, real initial house attachment and all four attachment-capacity predicates, family grouping, training/exercise and gold/wealth/counter events, home attach/detach commands and opcode 33. Distinct STargetData helpers restore the native idle validity/distance branch; nearby valid opponents now enter the still-open combat task 14. Human sector iteration preserves the marriage search order. Read the follow-up in [HUMAN_FREEZE_RECOVERY.md](HUMAN_FREEZE_RECOVERY.md) and C2HumanFamily/C2HouseAccess/C2HumanProduction/C2InteractionTargetAccess. Windows ASM/Game.exe supplies behavior; Mac headers supply names only.

Still open: sector/guide path fallback; default-action/job-change/work/learning assignment commands; combat/outdoor/vehicle/chest/guide trees and external death dependencies. Do not claim every human workflow or all freezes complete. All 13 touched/new cpp files passed Win32 /Zs; no Debug/Release build or runtime run. Project/filter parity: 187 cpp / 178 headers. Inventories: 734 naming rows / 118 unfinished entries; the 2584/5054 address count is historical. Earlier entries below describe previous checkpoints.

## Human activity and freeze recovery - 2026-10-07

The user reports freezes among women, children and scouts. The new recovery block connects household collection/delivery, equipment replacement and full equip/drop leaves, civilian goto/go-home/listening commands, goods/food/prayer searches and storage, ordinary indoor production, active civilian path callbacks, landscape/regrow ticks and real house detachment. Read [HUMAN_FREEZE_RECOVERY.md](HUMAN_FREEZE_RECOVERY.md) and current C2HumanGoods/C2HumanProduction/C2HumanEquipment/C2HumanWalking before relying on earlier entries below. All behavior derives from Windows ASM/Game.exe; Mac supplies names only.

Remaining family/birth/learning/outdoor/vehicle/combat/default-action and sector/guide route branches are explicitly unfinished; opponent-target idle and unknown event sets can still hold humans. Do not claim all freezes fixed or all human workflows complete. Verification: all 21 touched cpp files passed MSVC Win32 /Zs syntax-only checks; project/filter parity 186 cpp / 177 headers; no inline asm. No Debug/Release build, tests or new runtime run. Inventories: 670 names / 121 unfinished entries. The 2584/5054 coverage figure remains historical. Earlier chapters describe older checkpoints.

## Human events and needs follow-up - 2026-10-07

Read [HUMAN_ATOMIC_EVENTS_AND_NEEDS.md](HUMAN_ATOMIC_EVENTS_AND_NEEDS.md), C2HumanAtomicEvents and C2HumanNeeds before continuing. Urgent needs/items and supported task-7 starts/finishes now exist; do not recreate the older staging guard. Next close Food_Collect/Food_TryToCollect and cargo storage, House_Religion_GetNextPrayPoint/FindTempleForPraying, Target_RestoreSecondaryTarget and task failure 0047024A/CurrentTask_DoFailure. Then connect production/collection/building/training/combat event trees and active path-step/result callbacks. Secondary-target terminal finishes and untranslated whole event sets are held as explicit reconstruction boundaries. Human condition/tick ownership also depends on the existing house-detach/death-cleanup dummies. General passenger exit and child animal-play remain open. Mac supplies names only; behavior must be established from Windows ASM/Game.exe. 364 names / 135 unfinished; no build/tests/runtime. Earlier entries are historical.

## Human goodie follow-up - 2026-10-06

The new game chat has connected supported goodie startup, conversation/listening/waiting, sleep-place/work-area searches, non-passenger walk starts and finish leaves. Close child task 29's animal-play/search-center tree, `CVehicle::Passengers_MoveOut_h`, vehicle target-arrival validation and cargo-bearing waiting; then reconstruct atomic event dispatcher 00468678, conditions/need updates and task failure. New path-step consumption with native position/result callbacks and sector/guide fallback remains subsequent work. The current naming/unfinished inventories contain 318/127 rows; no runtime confirmation or refreshed address-coverage count. See [HUMAN_GOODIE_TASKS.md](HUMAN_GOODIE_TASKS.md) and README OA152/FP194..FP196. Earlier entries below describe historical checkpoints.

## Next chat: human behavior continuation - 2026-10-06

The user moved game reconstruction to a new chat; resume from committed `5e2c4db` / `5cc139f`. First complete task 24 goodie startup and talk/sleep/search/wait helpers, then event-bearing atomics and need/task dispatch; active path-step consumption and sector/guide fallback remain open. No new runtime confirmation. C# BMD converter work in this chat is a separate project. Read local file `.codex-work/docs/NEXT_CHAT_HANDOFF.md` before continuing.

## Human idle task lifecycle subset - 2026-10-05

The human tick now calls the native AI wrapper (perform atomic, check interruptibility, task machine). Supported idle finish/job-selection/start transitions run in the same tick. Ordinary land idle selection uses native random/counter/action rules; the complete idle finish retains its counter and resets both targets. Existing vehicle/social/religion predicates are shared. Other commands/tasks, urgent requests/items, conditions, production, house/crowding/escape/walk and goodie starts remain explicit partial boundaries. Autonomous civilians can still reach unfinished task 24 after counter 10; completed standing/talking/sleeping/eating or movement is not claimed. Ten new Mac naming rows and seven new partial inventory entries (121 total); project/filters include C2HumanTaskAccess.cpp/.h. No build/tests/runtime or new visual confirmation. README OA151/FP191..FP193; see [HUMAN_IDLE_TASKS.md](HUMAN_IDLE_TASKS.md). Next: goodie/talk/sleep helpers, atomic events and need-task starts/conditions.


## Mission tick/briefing execution and human arrival - 2026-10-05

The native mission callback now evaluates goals on callback 1 every 36 ticks and preserves true for all other calls. Timed/random-timed goals, four aggregate modes, activation timestamps, briefing result 9 (PlayCutscene), current briefing ID, callback 0x1B, scan-stop flag and popup sound are connected. Six additional general goal cases and control/exploration/external-AI-flag/diplomacy/visibility/request-flag actions are translated. The aggregate scan's submap calls, remaining general goals and remaining action cases are explicitly partial and excluded; scripts using those cases are not complete. Human destination arrival and its target-landscape/reset/weapon accessors are complete, retaining need validation, other-human scan, target reset and weapon-range/task-status order. The new-position side-effect tree, conditions, task/atomic execution and sector/guide fallback still block active new walking steps; no autonomous movement is claimed. Eleven naming rows (one existing callback row refreshed) bring the naming CSV to 275 rows. One callback dummy removed and three newly exposed partial entries give 114 tracked unfinished boundaries. Project/filters: 177 cpp / 169 headers. Fresh coverage: 2,584 / 5,054 source-associated entry candidates (51.13%), 2,470 open, including those 114 boundaries. Six selector-table addresses were removed from the previous audit and three genuine entry candidates added; coverage is not verified gameplay completeness. Windows/PE/Mac/source review only, no build/tests/runtime. README OA150/FP187..FP190. See [MISSION_BRIEFING.md](MISSION_BRIEFING.md), [LOCAL_PATHFINDER_ROUTING.md](LOCAL_PATHFINDER_ROUTING.md) and [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md).


## Human pathfinder failure/stop and priority-command queue - 2026-10-05

Six full Windows bodies now provide the separate priority/interrupting command predicates, ordered 87-slot queue query/index and human AI path failure/stop callbacks. Failure marks task status 3, sets the retry flag, detaches paired-human waits for tasks 5/13 and clears carrier circulation fields. Stop marks task 2 as failed and interrupts only when the atomic is interruptable and a priority command is queued. These callbacks are available for the pending human tick reconstruction; that partial tick still does not consume new path steps or invoke arrival/failure/stopped dispatch. Sector/guide fallback and the condition/task/arrival tree remain open. Six new mappings bring the current inventory to 265 mappings / 112 unfinished boundaries. Existing files remain included in project/filters. PE jump table and Windows control/call order reviewed; no build, tests or runtime run. README OA149/FP185/FP186. See [LOCAL_PATHFINDER_ROUTING.md](LOCAL_PATHFINDER_ROUTING.md). Next: human new-position/arrival callbacks, then native atomic walking and task-machine dispatch.


## Mission briefing window and GUI savegame IO - 2026-10-05

The 0x3B8 CMissionInfoWindow popup now has its Windows virtual table, construction/deletion, three pages, HLT loading, scroll/history controls, goal rendering, camera focus and language-aware voice/preview callbacks. The mission button (0xF42) and callback event 0x1B connect the existing-window update/new-window path. Shared preview resources and window pause/resume follow original ownership/order. GUI save/load preserves the XGUI wrapper and all nine child chunks, including briefing history, camera, statistics, human list, diplomacy, messages, groups, dead-human data and message priority. These bodies do not complete mission evaluation, the persistent onscreen mission overlay or the remaining partial GUI callback/default-handler branches. Movement still awaits sector/guide fallback and human task/step events. 36 new original-address mappings; 112 unfinished boundaries remain excluded. Project/filters contain 176 cpp / 168 headers. Windows ASM/PE/source review only; no build, tests or runtime run. README OA148/FP181..FP184. See [MISSION_BRIEFING.md](MISSION_BRIEFING.md). Next: human path/atomic step callbacks and task-machine execution; mission goal/result evaluation remains separate.


## Local pathfinder routing - 2026-10-05

The point/following starts, stop, next-direction state machine, predicted target position/revision lookup, walk/vehicle line searches, four independent directed flood searches, blocked-area escape marking, preferred geometry/weights and twenty-run vector construction are reconstructed from Windows assembly. Search budgets retain the two-search limit, exempt player IDs -1/20 and per-tick reset; the former GameControl tick dummy now implements that reset, while defeat detection remains partial. The route builder is explicitly partial at sector/guide fallback, and the existing partial creature tick still waits at active paths: task dispatch and arrival/failure callbacks must be reconstructed before activating gameplay movement. No movement or autonomous wandering is claimed working. 22 newly documented original addresses; 112 tracked unfinished boundaries, excluded from completion. Existing production files remain in project/filters. ASM/source/project/index checks only; no builds, tests or runtime launch. README OA147/FP176..FP180. See [LOCAL_PATHFINDER_ROUTING.md](LOCAL_PATHFINDER_ROUTING.md). Next: sector/guide fallback, human atomic walk/task dispatch and step-result callbacks.


## Viking graphics callback-chain correction - 2026-10-05

A bounded run of the user-built Debug executable confirms all 94 COOP humans are created/attached/explored, but the input callback's false return prevents engine body/head initialization. Windows 004B77E8..004B790C always returns true; the partial callback now retains that contract. Its selection/highlighting/cursor side effects remain pending and excluded from completion. A process-only return comparison gives body/head counts 22/22/50 and selects frames for all 22 local humans; no build, tests or disk binary patch. The briefing constructor returns null and is not the demonstrated blocker. Egyptians animating on the spot still need the human task/path tree. 111 unfinished boundaries / 201 mappings remain. README OA146/FP175. See [CREATURE_TICK_PROGRESS.md](CREATURE_TICK_PROGRESS.md).

## Creature tick progress and COOP startup diagnosis - 2026-10-05

The formerly empty 0040BD33 tick entry now advances original turning/active walking and event-free human atomics. Attachment_PerformTurn is fully reconstructed; the tick and CurrentTask_DoPerform remain partial and excluded from completion. Condition updates, task selection/events, new paths, animal/vehicle atomics and deletion remain pending: autonomous movement and queued movement commands are not claimed complete. COOP contains explicit 22/22/50 Viking start objects for players 0/1/4; Debug clean-map startup now emits per-player allocation/attachment/graphics/exploration counters to distinguish missing creation from drawing. The Viking runtime cause remains unresolved. Two new Mac mappings; 111 unfinished boundaries. README OA145/FP173/FP174. Existing production files only; no build, tests or runtime launch. See [CREATURE_TICK_PROGRESS.md](CREATURE_TICK_PROGRESS.md). Next: complete human task/pathfinder execution and inspect COOP startup counters.

## Moveable draw entries, foreground ordering and mouse wheel - 2026-10-05

The 00493656 draw entry now dispatches humans, animals, vehicles and markers through the shared renderer, including cart commanders, walking brightness/position interpolation, palettes, selection, status symbols and group labels. The typed 004951A4 bob descriptor and 004972DE landscape descriptor connect to stable per-field foreground depth ordering. Human/animal seen counters are connected; houses, construction markers, particles and generic visible-entity sound playback remain unfinished in 004911D5, which stays excluded from completion. One dummy is removed: 110 unfinished boundaries remain. 24 new function-name mappings use original Mac names and Windows behavior. The desktop now sends wheel delta in GUI parameter 3; world wheel zoom follows 004BAB2C with original DE_Zoom_In/Out helpers. README OA143/OA144 and FP168..FP172 index assumptions and failures. Existing production files remain synchronized with project/filters. Static ASM/PE/source verification only; no build, tests or runtime run. Next: house/construction foreground rendering and generic sound playback. See [MAP_MOVEABLE_DRAWING.md](MAP_MOVEABLE_DRAWING.md) and [MOUSE_INPUT_MODERNIZATION.md](MOUSE_INPUT_MODERNIZATION.md).

## Shared moveable drawing and direct effects - 2026-10-05

The complete shared renderer at 0049445E now connects frame selection to marking outlines, behind/front house overlays, shadows, restricted/full glow, body/head shading and explicit-strength transparency. Its house-bob helper, marking-color accessor and three missing bob effects are reconstructed from full Windows ranges; Mac supplies original names. The type-specific draw/descriptor entries and foreground traversal/depth ordering remain unfinished and unconnected, so visible creatures are not established. Six new mappings; 111 unfinished boundaries remain. Existing production files are used (174 cpp / 167 headers). README OA142 and FP164..FP167 index assumptions and failure paths. Windows ASM/source verification only; no build, tests or runtime run. Next: DrawAtom_MapMoveable_DoDraw and its bob descriptor, then stable foreground ordering. See [MAP_MOVEABLE_DRAWING.md](MAP_MOVEABLE_DRAWING.md).

## Human initialization and moveable sprite selection - 2026-10-05

The human creation callback at 004803D5 now selects body/head/shadow variants, base palettes, player/job random patches and localized names, retaining savegame palettes/names and native random-call order. The complete 00494AE6 frame selector implements walking, turning, atomic actions and in-house parts/overlay ordering; the unfinished draw/descriptor entries are not connected to it yet. Palette execution, player-patch/name getters and both directional turn helpers are reconstructed. Six new mappings and one removed dummy leave 111 unfinished boundaries; project/filters include the new production pair (174 cpp / 167 headers). README OA140/OA141 and FP159..FP163 index original assumptions and failures. Windows ASM/source/layout verification only; no build, tests or runtime run. Next: actual sprite drawing and its direct effects/house-overlay dependencies, followed by draw/descriptor entries and foreground traversal/depth ordering. See [MAP_MOVEABLE_SPRITE_SELECTION.md](MAP_MOVEABLE_SPRITE_SELECTION.md).

## Moveable and human graphic data - 2026-10-05

The two startup constructors at 0048BD66 and 0048ACFB now load shared atomic/walk/turn sequences, animal/vehicle graphics, human body/head job variants, random-patch metadata, localized human names and ten player-color/sign records. Fast animation/walk lookup, independent animation randomness, patch lookup and parent-job base/change lookup are reconstructed. Thirteen address mappings are added and two dummies removed: 112 unfinished boundaries remain. Project/filters include the two production pairs (173 cpp / 166 headers). README OA138/OA139 and FP154..FP158 index original assumptions and failures. Windows ASM/PE/source verification only; no build, tests or runtime run. The moveable draw entry and its shared landscape-field branch remain unfinished. Next: frame-cache/sprite selection, human dynamic graphics initialization and foreground moveable traversal/depth ordering. See [MAP_MOVEABLE_GRAPHIC_DATA.md](MAP_MOVEABLE_GRAPHIC_DATA.md).

## Vegetation drawing - 2026-10-05

The landscape branches of the former 004911D5 dummy now draw loaded vegetation through the existing graphic renderer in static/background/foreground passes. Seven complete helpers reconstruct macro-interpolated brightness, highlight/exploration, auto-transition progress, inclusive rectangle checks and wall segments. Static attachments retain off-rectangle origins and original visibility/brightness behavior. The shared field remains partial because creature, construction-marker, particle and visible-entity sound branches are unfinished; its inventory row remains excluded from completion. Regrowth and auto-transition ticks remain pending. The new cpp/header pair is in project/filters (171 cpp / 164 headers); 114 unfinished inventory entries remain. README OA137/FP150..FP153 records original assumptions and rejection paths. Windows ASM/PE/source checks only; no build, tests or runtime run. See [VEGETATION_DRAWING.md](VEGETATION_DRAWING.md). Next drawing block: moveables/creatures and their shared-field integration.

**Priority snapshot:** 2026-10-04

This queue reflects the latest known project direction. Check `HEAD`/Git first in case a later local commit already resolved an item.

## Human creation and initial exploration - 2026-10-04

Human creation now connects Add_Human -> AllocateHuman -> Human_Init -> real moveable/pathfinder placement -> job-dependent sight radius -> GameMap_Exploration_DoExplore. Job/permission/production setup, merchant/work markers, initial animation and human cleanup are reconstructed at their original control/call boundaries. Two dummies are replaced; five deeper death/edit/work-house operations remain isolated, and existing house attachment, other creators, mission execution and dynamic-object drawing remain unfinished. Source mapping is 2,441/5,057 (48.27%), with 2,616 unmapped candidates and 120 tracked unfinished entries. Project/filters match at 168 cpp / 161 headers. Windows ASM/PE/source verification only; no build, tests or runtime confirmation of the previously black loaded map. See [HUMAN_INITIALIZATION_AND_EXPLORATION.md](HUMAN_INITIALIZATION_AND_EXPLORATION.md). Next: house inhabitants and home/work/build attachment commands, then other creators and dynamic-object drawing.

## Active build checkpoint - 2026-10-04

HEAD is 1ef245b (Restore menu and guide initialization), with the in-game default handler, complete Settings, static ground drawing and engine map IO/startup blocks pending. The prior naming/output checkpoint is 4220d37. The committed naming pass replaced all 143 active FUN identifiers; see [FUNCTION_NAMES.md](FUNCTION_NAMES.md) and [FUNCTION_NAME_MAP.csv](FUNCTION_NAME_MAP.csv). The current terrain startup block replaces the Pattern and Landscapes constructor dummies with real Windows loaders and their texture/area dependencies; see [ENGINE_TERRAIN_STATIC_DATA.md](ENGINE_TERRAIN_STATIC_DATA.md). The subsequent GameControl startup block replaces its constructor, diplomacy setter and logic callback and reconstructs their direct helpers; see [GAME_CONTROL_STARTUP.md](GAME_CONTROL_STARTUP.md). TemporaryLinkStubs.cpp now isolates 117 unfinished original implementations (including partial entries); [TEMPORARY_LINK_STUBS.csv](TEMPORARY_LINK_STUBS.csv) tracks every pending address. Dummies are excluded from completion counts.

The earlier a77fd47 checkpoint passed Debug|Win32 and Release|Win32 compilation/linking. No further builds are run unless explicitly requested by the user. Terrain/GameControl and progress-bar corrections have agent assembly/PE/static verification. The user rebuilt in Visual Studio and reached the main menu, confirming progression beyond the reported startup/free crashes; correct rendering and gameplay remain unverified. The subsequent font correction has static verification only. Project and filters contain the same 165 cpp files (164 production units plus the temporary stub unit) and 158 headers. Executables use OpenVikings/Debug and OpenVikings/Release, PDBs use symbols/ and intermediate products use obj/. The last tested unresolved linker inventory is empty; this is a historical result, not a new build claim.

Pattern/landscape groups, full records, animation frames, four area families, resolved transitions, texture loading, palette ID lookup and the complete cached-ID regions now follow Windows startup. The observed null Pattern group-data pointer came from the dummy constructor and is corrected at its owner initialization. The SoundFX loader shares the established group helpers. The palette constructor and its direct color/remapping helpers now load the full Windows palette records and complete static/token regions; see [ENGINE_STATIC_PALETTES.md](ENGINE_STATIC_PALETTES.md).

The subsequent user screenshot reaches player statistics and shows a null GameControl owner. Its dummy constructor omitted that owner initialization. The real constructor now publishes/clears the object, loads players/miscellaneous/allowed/special-item groups, applies startup state, initializes tribe availability and registers the real logic callback. Diplomacy changes preserve message/callback order, and statistics allocations now use the original explicit new-handler thunk. The subsequent user run progresses through this startup to the main menu. The next reported heap breakpoint exposed a host/runtime allocation mismatch in the progress-bar bitmap. Its object allocation now uses the original explicit new-handler runtime path; see [PROGRESS_BAR_HEAP_OWNERSHIP.md](PROGRESS_BAR_HEAP_OWNERSHIP.md). The subsequent user run reaches the menu without the reported progress-bar free breakpoint.

The existing message-manager callback/history/priority tree, mini-button family, Human filtering and transport reservation rollback remain reconstructed. CMessageManager::PresentationWindow_Open, Tool_CreateMessageString and DrawAtom_MapMoveable_DoDraw remain tracked dummies. New calls use recovered Mac names directly, with Windows behavior and ABI.

Actual provisional source-address coverage is 2,346/5,057 (46.39%), with 2,711 unmapped candidates; the 117 unfinished dummy/partial entries do not increase the numerator. Eight startup constructor boundaries remain unfinished. The current menu-font audit corrects two transparency descriptor shifts from 16 to 22 decimal bits, confirmed by all twelve corresponding PE instructions; see [MENU_RENDERING_AUDIT.md](MENU_RENDERING_AUDIT.md). The menu-world element now has real draw/message/destruction/remapping routines and camera/zoom/exploration/minimap-copy helpers; seventeen mappings replace six dummies. See [MAIN_MENU_WORLD_ELEMENT.md](MAIN_MENU_WORLD_ELEMENT.md). The binary map loader, unpacking, map constructor and saved owner tree now add 28 further mappings and replace three dummies; fish/transition ownership is corrected across startup, tick and destruction. See [BINARY_MAP_LOADING.md](BINARY_MAP_LOADING.md). Next: human/object initialization and mission execution, then Houses/static-data and landscape drawing. Current world/map/palette changes have static verification only. Remove each matching stub/inventory row as its real body is connected. See [NATIVE_LINK_SUPPORT.md](NATIVE_LINK_SUPPORT.md) and [LINK_BUILD_AUDIT.md](LINK_BUILD_AUDIT.md).


The palette block adds nine mapped addresses and replaces the 00480A20 dummy, retaining Windows allocation/cookie ownership, 31 preshades, remapping, INI parameter consumption and auxiliary palettes. README OA105/OA106 and FP088..FP090 index reviewed failures; OB073 remains the original index-31 defect. No build or runtime rerun. See [ENGINE_STATIC_PALETTES.md](ENGINE_STATIC_PALETTES.md).

The 2026-10-04 button report exposed the default command-handler dummy at 004D6BBB. Its full 46-command Windows dispatch and direct network/lobby helpers are now reconstructed, adding eighteen mappings and removing one dummy. Sound/animation were independent of the no-op command path. README OA107/OA108 and FP091..FP093 index retained failure paths; see [MAIN_MENU_COMMAND_DISPATCH.md](MAIN_MENU_COMMAND_DISPATCH.md). Static verification only. The subsequent credits/keyboard block reconstructs both entries and their connected tree, adding 42 mappings and removing three dummies. Project/filter inclusion and README OB096/OB097, OA109/OA110 and FP094..FP096 are synchronized; see [MAIN_MENU_CREDITS_AND_KEYBOARD.md](MAIN_MENU_CREDITS_AND_KEYBOARD.md). Runtime confirmation remains pending.

## Mission data and native exploration - 2026-10-04

Mission data and native map exploration are now reconstructed: clean-map loading reads all mission definitions, complete goal/result metadata and named parameters; exploration updates the original player WORD mask, house/landscape footprints and redraw callbacks. Static-object startup at 0041D24B and mission execution remain unfinished. The 0041D09B wrapper is explicitly partial and excluded from coverage. Nineteen mappings bring source coverage to 2,325/5,057 (45.98%), with 2,732 unmapped candidates and 105 tracked unfinished boundaries. Project/filters contain 164 cpp and 157 headers. Windows ASM/PE/source review only; no build, tests or runtime run. See [MISSION_DATA_AND_EXPLORATION.md](MISSION_DATA_AND_EXPLORATION.md). Next: the StaticObjects creation tree, then mission tick/goal evaluation and result execution.

## In-game terrain visibility and player centering - 2026-10-04

The user confirms main-menu terrain, but reports a black loaded game map. Menu drawing disables exploration; game terrain uses the player's logical +8 WORD mask. The two startup dummies for player assignment and camera centering, plus the object/terrain center fallback, are now reconstructed with their connected position/follow helpers. The GUI logic callback's native unconditional true is restored; its actual dispatch remains incomplete and excluded from coverage. Five mappings replace three dummies: 2,306/5,057 (45.60%), 2,751 unmapped and 104 unfinished entries. No builds, tests or new runtime run. See [IN_GAME_TERRAIN_VISIBILITY.md](IN_GAME_TERRAIN_VISIBILITY.md). Next priority: mission/static-object startup and genuine exploration initialization; centering alone does not establish that a new map has visible fields.

## Main-menu ambient crash correction - 2026-10-04

The user-requested debugger run reproduced a filename-derived wave ID and an ambient record indexed before its table. Both collectors now compare against FFFFFFFF: native 83 /7 immediate FF is sign-extended, despite the export text showing FFFFFF. The earlier original-mismatch claim is retracted. A process-only comparison correction avoids the reproduced exception for sixty seconds; a separate twenty-second probe confirms program state 8 (MainMenu) and GUI screen 1. No build or disk EXE modification was performed; gameplay/rendering remain separate. See [AMBIENT_SOUND_SENTINEL_FIX.md](AMBIENT_SOUND_SENTINEL_FIX.md), README OA092 and FP120. Function coverage is unchanged.

## Engine map IO and startup loading - 2026-10-04

The black-terrain investigation identifies two real omissions: the MapIO callback returned false without loading engine terrain, and the startup-record executor did nothing. Both now have Windows-equivalent bodies. Clean-map name remapping/planes, engine savegame dispatch and direct entity/weather/music/particle serializers are connected. Human and particle constructors establish their original serializer ownership. Player-aware dirty rectangles, load completion and road-overlay updates are reconstructed. Exploration still uses the original logical +8 WORD mask; unseen terrain is not forcibly revealed.

Windows ASM/PE/source review adds 39 mappings and replaces six dummies: 2,301/5,057 (45.50%), 2,756 unmapped candidates and 107 unfinished entries. Project/filters match at 162 cpp / 156 headers; eight startup constructor names remain directly referenced by C2GameEngineStartup.cpp. README OA121..OA123 and FP116..FP119 index original failures. No builds, tests or runtime run; the latest screenshot proves GUI progression, not terrain rendering after this change. See [ENGINE_MAP_IO.md](ENGINE_MAP_IO.md). Next: separate 004911D5 landscape/creature drawing, static-house/object dependencies and remaining gameplay exploration initialization.

## Static ground drawing - 2026-10-04

The former static-update dummy now connects all buffer-splitting stages, textured macro triangles, exploration/highlight and vertex shading, transition layers and road/house overlays. Engine-map initialization/height/transition callbacks, transition INI storage and scalar/MMX scanline shaders have real Windows-equivalent bodies. Five dummies are replaced and 67 mappings added: 2,262/5,057 (44.73%), 2,795 unmapped candidates, 113 unfinished entries and eight startup constructor boundaries. Project/filter inventories match at 161 cpp / 155 headers. Windows ASM/PE/source verification only; no builds, tests or game runtime. See [ENGINE_GROUND_DRAWING.md](ENGINE_GROUND_DRAWING.md), README OA118..OA120 and FP112..FP115. The separate 004911D5 landscape/creature body remains unfinished. Next related block: that drawing tree and its Houses/dynamic-object dependencies.

## P0 - Replace temporary dummies with genuine definitions

The click-command, credits constructor/scroll setter and primary keyboard dummies are replaced with their full connected Windows trees. See [MAIN_MENU_COMMAND_DISPATCH.md](MAIN_MENU_COMMAND_DISPATCH.md) and [MAIN_MENU_CREDITS_AND_KEYBOARD.md](MAIN_MENU_CREDITS_AND_KEYBOARD.md). The load-menu screenshot exposed the null tool-listbox dummy. The listbox, controller, child lifetime/draw/input tree and savegame filling are now reconstructed; see [MAIN_MENU_LISTBOX_AND_SAVEGAMES.md](MAIN_MENU_LISTBOX_AND_SAVEGAMES.md). Houses and deeper loading dependencies remain queued afterward.

The menu-world callbacks, remapping, lifetime, camera and zoom helpers are reconstructed; see [MAIN_MENU_WORLD_ELEMENT.md](MAIN_MENU_WORLD_ELEMENT.md). Binary loading (0042FBF4), unpacking and its immediate owner tree are reconstructed; see [BINARY_MAP_LOADING.md](BINARY_MAP_LOADING.md). Palette-manager startup (00480A20), the complete palette region and direct helpers are reconstructed; see [ENGINE_STATIC_PALETTES.md](ENGINE_STATIC_PALETTES.md). Prioritize Houses (0048E651) and its required helpers. The two erroneous font/transparency descriptor shifts are corrected; runtime confirmation remains pending. See [MENU_RENDERING_AUDIT.md](MENU_RENDERING_AUDIT.md). Pattern/Landscapes startup and their direct texture/area helpers are reconstructed. The subsequent user run reached GameControl/statistics startup; GameControl and its direct initialization/callback helpers are now reconstructed as well. The user has since reached the main menu; gameplay remains unverified. See [ENGINE_TERRAIN_STATIC_DATA.md](ENGINE_TERRAIN_STATIC_DATA.md) and [GAME_CONTROL_STARTUP.md](GAME_CONTROL_STARTUP.md).

Fish/landscape-transition bindings are corrected throughout the owning lifecycle; the fish callback is real, while transition tick 00426F84 remains pending. See [BINARY_MAP_LOADING.md](BINARY_MAP_LOADING.md). Resolve pending dependencies by the semantic names in [TEMPORARY_LINK_STUBS.csv](TEMPORARY_LINK_STUBS.csv); zero active FUN identifiers does not mean zero unfinished functions.

The current inventory above supersedes the earlier 133-unit / 133-symbol and 124-unit / 213-symbol snapshots. See [LINK_BUILD_AUDIT.md](LINK_BUILD_AUDIT.md), [TEMPORARY_LINK_STUBS.csv](TEMPORARY_LINK_STUBS.csv) and [GUI_LINK_CLOSURE.md](GUI_LINK_CLOSURE.md).

- Follow the GameTime_DoOneGameTick tree from 00407812, including the user-reported 00428736, 00410F00, 0041ACD6 and 0040BD33, then the directly related command/tick helpers. Preserve documented Windows timing and unresolved dependencies until their bodies are reconstructed.
- The former 00410F00 ABI/namespace mismatch is resolved through UserNetworkCommandManager_AddCommand. Continue genuine dependencies exposed by Execute at 00410FCC, especially object removal, road/wall/guide commands and good-circulation interruption.
- The title/pre-ingame FMV tree and both remaining data bindings are reconstructed; continue the remaining function trees and retain unresolved particle-byte naming/writer evidence. The complete animal/SoundFX token regions and assertion callback are reconstructed. Do not define guessed globals. Only the explicitly authorized temporary dummies may substitute unfinished functions; keep them tracked separately.
- Validate connected blocks against Windows ASM and keep project/filter inclusion synchronized. Run compiler/link checks only when the user explicitly requests them; preserve historical build results separately from current static verification.

## P0 - Menu message handlers, shared widgets and remaining GUI callbacks

- Committed the prior in-game/string-output work as d64247c (Reconstruct ingame GUI and networking), then reconstructed all thirty-two distinct original call targets in MainMenuGuiManager.cpp or connected existing native entries. The full 23-entry Windows screen switch includes all three lobby modes; native list/handler lifecycles, Human iteration, camera following, desktop work and automatic host/join are connected. See [MAIN_MENU_GUI_MANAGER.md](MAIN_MENU_GUI_MANAGER.md).
- Followed provider/session/player enumeration, host/join/search, snapshot copying, dynamic attachment and search-timer construction/pruning through the DirectPlay/Windows API boundaries. Restored three missing bitmap-tool virtual slots and corrected menu graphics allocation/free families. README OB093, OA078-OA082 and FP054-FP059 index the reviewed failure paths. Genuine shared widget, inherited virtual, menu message/world/mouse and deeper desktop bodies remain original dependencies.
- Fourteen affected units generate Win32 /Od and /O2 objects; only established NetworkRuntime deprecated-API warnings remain. All 123 production implementations pass Win32 syntax compilation with zero errors. Project/filters cover all 123 implementations and 125 headers; main platform/output settings are unchanged. No tests, full link or game-runtime verification.
- Provisional address coverage is 1,499/5,045 (29.71%), with 3,546 unmapped candidates and 57 mapped entries retaining direct original calls. Forty-three startup subconstructors remain. Next connected block: default/primary menu message handlers and shared text-button/list/hypertext families, then world/mouse drawing and desktop dispatch. Current main-menu work is uncommitted on HEAD d64247c.

Next connected work:

- reconstruct menu default/primary message callbacks 004D6BBB/004DB0D1 and follow the command branches through their real helper tree,
- reconstruct text-button/list/hypertext constructors and callbacks retained in Screen_UpdateGui, including 004B8DA2, 004B8E2C, 004B8F1C, 004CFF78, 004CFFF5 and 004CBCD6; preserve original allocation and field-write order,
- follow menu world draw/message/delete 004DA615/004DAAC1/004DA5BF, mouse drawing 004DAE69 and world exploration/darkening/interpolation 0047E669/004DAB77/0047C9DA,
- complete desktop input/drag/draw 00477A82/00477D80/0047796A and inherited base-element virtual entries without inventing substitutes,
- analyze the complete player-color table and remaining 005694D8..005694DB region through their Windows xrefs; retain unresolved names until supported,
- continue in-game auxiliary callbacks and class-static initialization as indexed in [IN_GAME_STATE.md](IN_GAME_STATE.md).

The completed string-output/desktop block is recorded in [STRING_OUTPUT_WINDOW.md](STRING_OUTPUT_WINDOW.md); its 116/117 syntax result is a historical pre-main-menu snapshot. The current production result is 123/123. No tests/test projects are requested.

## P1 â€” Main-menu loading and directly related logic follow-up

The menu-state file's three unknown targets now use reconstructed reload, ambient-stop and clock entry bodies. Loader/centering/network-clock helpers, complete clock/token regions and shared callers are connected; previous atomic/tribe work is committed as `acfcdb7`. See [MAIN_MENU_STATE.md](MAIN_MENU_STATE.md). This main-menu follow-up was subsequently committed as fb69f88 (Main Menu Code).

Next connected work:

- binary dispatcher `0042FBF4`, unpacking and its immediate constructor/owner tree are reconstructed; continue the tracked map-validation callbacks and deeper map mutation bodies,
- follow its mission/AI/merchant/goal script loaders `0041D09B`, `0040822E`, `0041CB1E`, `0041910B`,
- close player-center fallback `00439E61`, its iterators and terrain/spiral branches,
- command queue/send/receive/dispatch `00410F00`, `00411829`, `004118F6` and `00410FCC` are reconstructed; continue their remaining real callees and latency collector `0040B183`; retain the direct Windows PE decoding evidence for the misdecoded send entry,
- continue original per-tick manager and autosave bodies; preserve OB088/OB089, OA071-OA073 and FP044-FP047,
- audit the remaining loader metadata and hardware FPU-environment behavior without inventing resource invariants or helper bodies.

Historical menu-state snapshot: production syntax was 107/109 with 52 diagnostics in MainMenuGuiManager.cpp (40) and MasterControlProgram_PS_InGame.cpp (12). Thirteen complete affected units generate Win32 `/Od` and `/O2` objects. Project/filters cover 109 implementations and 117 headers. Provisional mapping is 1,377/5,044, including 58 mapped entries with original calls. No tests/test projects or full link/game runtime verification.

## P1 â€” Special-sector pathfinder theme block

### `0044FD3E` and special-sector pathfinder follow-up

The `004507F2` callback, two-pass recalculation, sector update, flood-fill and load-flag paths are now reconstructed locally. The complete connection/pathfinder block remains open. Evidence and isolated x86 verification are recorded in [MAP_WALK_SECTOR_CALLBACK.md](MAP_WALK_SECTOR_CALLBACK.md).

Required work:

- translate `0044FD3E` (`l_Sectors_CheckAndSetConnection`), currently retained as an explicit original external call,
- reconstruct the directly required `CMapPathfinderSpecialSectorSearchElement` search chain around `004703CA` and `0047041B`,
- resolve the existing `0x60`-byte workspace beginning at `00550E84` from all Windows accesses and Mac correspondence,
- the complete shared pathfinder queue region `00512620..00550E5F` is now named `sMapPathfinderStaticVariables`; deeper pathfinder operations remain separate targets,
- preserve the documented Windows water-list scan behavior and remaining unknown record bytes,
- verify the connected path against Windows ASM and production call sites after implementing the pathfinder code; the user discontinued test projects.

`00498128` and the other four unknown calls in `C2EngineDrawingAccess.cpp` are now reconstructed. See [ENGINE_DRAWING_BOB_EFFECTS.md](ENGINE_DRAWING_BOB_EFFECTS.md). Graphics follow-up: review the already implemented normal transparency routines and their packed-row descriptor handling.

The seven unknown call sites in `WorldDisplayElementAccess.cpp` are now reconstructed, including their immediate display-control, tooltip and ambient-sound helpers. See [WORLD_DISPLAY_ELEMENT_DRAW.md](WORLD_DISPLAY_ELEMENT_DRAW.md). Next connected graphics block: `00481AE2` with the static split/rasterization chain, then `004911D5` foreground object traversal. Its retained original dependencies and music-region ownership work are listed in that document.

## P1 - Static-job construction and loading

Startup, campaign/string, map/savegame, terrain, good, house, vehicle, atomic-animation and tribe blocks are reconstructed and connected. See [STATIC_TRIBE_TYPE_DATA.md](STATIC_TRIBE_TYPE_DATA.md) and preceding subsystem documents. Startup retains 43 original subordinate constructors, four registered engine callbacks, executor `0040743A` and deeper map/tick/command boundaries. Map loader `0040A68B` and synchronization initialization `004079CF` now have native bodies in the main-menu follow-up. Next connected block: static-job constructor `0041300A`, its complete resource/static/token regions and directly required helpers; reuse existing destruction `004134B6` and parser/runtime infrastructure. Then continue remaining static managers. Current production syntax coverage is 107/109 with 52 diagnostics; provisional function-address coverage is 1,377/5,044. See [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md). No tests/test projects. Existing unknown fields/regions remain explicitly unresolved; preserve indexed original defects OB082-OB087 during faithful reconstruction.

Separately review existing object/buffer allocation sites against Windows thunk `004E5DDD`, which always calls `Runtime_AllocateWithNewHandler(size, 1)`. Mode-dependent `Runtime_Allocate` has the same allocation/free family but can differ in handler invocation when global mode is zero. The completed system-manager, converter-shadow-table and property-manager paths use the explicit-handler interface.

## P1 - Display startup/palette/presentation follow-up

`00479052` and the complete bitmap-to-screen copy/convert/lock/diagnostic tree are reconstructed and integrated. See [BITMAP_TO_SCREEN.md](BITMAP_TO_SCREEN.md). Continue with the existing display startup/palette/presentation paths and their original failure handling; then the separate static-window/base-tool lifetime and world-input blocks. The current production syntax inventory is 82/86 with 110 diagnostics in the same four incomplete units. No tests/test projects are requested.

## P1 â€” Further game/engine constructor and load ownership

The shutdown/destruction tree and startup orchestration are reconstructed; see [ENGINE_SHUTDOWN.md](ENGINE_SHUTDOWN.md) and [GAME_ENGINE_STARTUP.md](GAME_ENGINE_STARTUP.md). Complete the subordinate constructor/loading bodies after the completed static-string/campaign, map/savegame and terrain-type blocks. Bind storage to established owners, native lists, pattern/texture/dynamic-object arrays and dimension mirrors. Do not reintroduce copied process addresses or duplicate regions. The Windows-only music owner at `00568418`, extra job-experience table, AI embedded owner and unidentified record fields still require semantic/layout analysis. Full link/runtime remains open.

## P1 â€” Continue global/static symbol cleanup

The repository-wide audit found 47 remaining live `DAT_` symbols and six retained hardware/conversion `__asm` blocks after its supported replacements. The full per-file inventory, static-analysis findings and six incomplete production compilation units are recorded in [SOURCE_AUDIT_2026_09_30.md](SOURCE_AUDIT_2026_09_30.md). Next integration block: the main-menu/master-control owning dependencies and their established interfaces; retain original error paths while resolving them.

The property-options and directory block is reconstructed and connected, including global options load/save/defaults and `CFile::WriteString`; see [PROPERTY_OPTIONS_IO.md](PROPERTY_OPTIONS_IO.md). The complete Windows CD-audio manager and game-disc search are also reconstructed and connected; see [CD_AUDIO_MANAGER.md](CD_AUDIO_MANAGER.md). Desktop startup/shutdown and display-position preservation include their immediate cleanup, bitmap ownership and player-notification paths; see [GUI_DESKTOP_RESOLUTION.md](GUI_DESKTOP_RESOLUTION.md). World-display lifetime and overview-handler callback/static-state work is recorded in [WORLD_DISPLAY_LIFECYCLE_OVERVIEW.md](WORLD_DISPLAY_LIFECYCLE_OVERVIEW.md). Overview bitmap lifetime, rendering control, zoom/scroll/timed markers and the static overview-window entry path are reconstructed; see [OVERVIEW_BITMAP_RENDERING.md](OVERVIEW_BITMAP_RENDERING.md). Terrain sampling/rasterization `004CEF61 -> 004CF4E9/004CF5C9`, brightness/cache helpers and the complete overview-color region are reconstructed; see [OVERVIEW_TERRAIN_SAMPLING.md](OVERVIEW_TERRAIN_SAMPLING.md). All five object/chest/sight overlays and their direct iterator chains are now reconstructed and connected; see [OVERVIEW_OBJECT_OVERLAYS.md](OVERVIEW_OBJECT_OVERLAYS.md). Overview bitmap input and immediate coordinate/selection/vehicle-command helpers are now connected; see [OVERVIEW_INPUT.md](OVERVIEW_INPUT.md). All 14 game-side DirectMusic manager entries are reconstructed and integrated; see [DIRECT_MUSIC_MANAGER.md](DIRECT_MUSIC_MANAGER.md). The current 84-unit syntax pass has 80 passing units and four incomplete units with 123 diagnostics; 41 live `DAT_` symbols remain. Engine-map/bitmap primitives and data-owner implementations remain original dependencies. Next connected GUI block: static-window/base-tool lifetime, followed by world-input dispatcher `004BA3BE`. Audio follow-up concerns the separate driver DLL and original CRT logarithm; no game-side DirectMusic manager methods remain missing.

Systematically scan all C++ sources for remaining:

- `DAT_...`,
- `PTR_...`,
- `UNK_...`,
- unnamed callback pointers,
- raw static storage blocks,
- unexplained member offsets,
- stale duplicate names for the same address.

For every hit:

1. inspect current C++ usage,
2. inspect all Windows xrefs,
3. compare adjacent globals as a region,
4. search Mac C/H for a matching semantic block,
5. inspect Git history,
6. rename only when supported.

### Known areas still worth review

- integration of the five identified object arrays into all higher-level logic paths,
- unresolved token contents/tables,
- remaining constants/GUID-like static regions,
- remaining static blocks not yet assigned to owning classes/managers.

## P1 â€” CGuiBaseStringsManager closure

Known resolved pieces:

- constructor `004E1D39`,
- destructor `004E1DA1`,
- singleton `00569888`,
- 13 string arrays beginning around `005697F0`,
- fallback/static block around `00569824`,
- known string-access wrapper call sites.

Still important:

- finish/verify getter `004E1DCB`,
- recover the exact filename/table semantics around `0050BBBCâ€“0050BBEF` if enough reference data exists,
- do not fabricate table entries if the required reference bytes/strings remain unavailable.

## P1 â€” Verify older unresolved raw helpers against current HEAD

These were historically unresolved or only partially named. Search current code first; remove from the active list if already solved:

- `0040BD76`,
- `0040BE7D`,
- `0040C5A6`,
- `00405ADB`,
- `00424F6C`,
- `0040C778`,
- `0040C931`,
- `004E75FA`,
- `004E9772`.

Do not translate these merely because they appear in this list; handle them when their owning subsystem/theme block is active.

## P2 â€” Network leftovers, only if still present

Historical unresolved network references:

- `FUN_004DBD68`,
- `FUN_004E3252`,
- original StartNetworkForMenu callers around `004D3623`, `004D7B0F`, `004D7BAB`.

A large network shutdown/runtime cleanup has already been completed. Verify with `git grep` before reopening this subsystem.

## P2 â€” Application/display leftovers, only if still present

Historical X2DApplication follow-ups included:

- `StartUp`,
- `GetInstance`,
- `GetWindowHandle`,
- later lifecycle/message work,
- XMKeyManager follow-up work.

These items come from an earlier project stage and may already be superseded. Inspect current source and Git history before acting on them.

## P2 â€” Build/runtime verification

Where source reconstruction is complete but not runtime-proven:

- run/confirm Win32/x86 MSVC build,
- fix real compiler issues without changing reverse-engineered behavior,
- distinguish compiler cleanup from semantic reverse engineering,
- validate ownership/lifetime-sensitive paths in runtime where practical.

Do not use build errors as justification to invent unresolved original functions.

## Related DirectMusic research â€” dormant unless explicitly resumed

If work returns to the managed/native `gedx8musicdrv` reimplementation, historical deep-open helpers included:

- `10003890`,
- `10004120`,
- `10004670`,
- follow-ups around `10004250`, `100042C0`, `10004490`,
- additional helpers around `10003F00`, `10003F50`, `10003FA0`, `10004020`, `10004070`, `100040D0`.

Key unresolved semantics historically included composite object construction and a second DMIME descriptor-query result associated with output offset `+0x2C`.

Do not mix this dormant driver work into the main game DAT-cleanup pass unless the user explicitly switches focus.

## Definition of done for a theme block

A block is considered properly closed when:

- primary functions are translated,
- directly related helpers are translated or explicitly documented as unresolved,
- related globals/static arrays are named where evidence supports it,
- ownership/lifetime is understood,
- Windows call/side-effect order is preserved,
- Mac naming has been checked,
- stale DAT/PTR/UNK names in that block are removed where justified,
- remaining uncertainties are written down instead of guessed,
- the next logical theme block is stated clearly.

## Main-menu listbox and savegames - 2026-10-04

The user reached the load screen and reported a null element at Desktop::Element_Add (+0x18). The tool-listbox constructor dummy returned null; its controller, child insertion and savegame filler were also dummies. Their full connected Windows block is now reconstructed, with 40 added mappings and four dummies removed. Current coverage is 2,065/5,056 (40.84%), excluding 123 dummies; project/filters contain 152 cpp files and 149 headers. README OA111 and FP097..FP099 record the retained allocation, child lifetime, clipping/input and savegame payload paths. ASM/PE/static verification only; no build or runtime rerun. See [MAIN_MENU_LISTBOX_AND_SAVEGAMES.md](MAIN_MENU_LISTBOX_AND_SAVEGAMES.md). Next: remaining deeper load/game dependencies and Houses startup.

## Guide-manager startup correction - 2026-10-04

The user reached the in-game overview, where the guide iterator dereferenced a null CGuideArray owner. Constructor dummy 0041AE53 omitted that owned array. Its genuine startup initialization, callback registration and scan-buffer allocation are now connected; the loading constructor and saved manager chunk are reconstructed and the existing destruction path is checked. The complete 00511540..0051154B static region is named and layout-asserted. Three mappings replace one dummy: 2,068/5,056 (40.90%), with 2,988 unmapped candidates and 122 dummies; twelve startup constructors remain. No production files were added, so project/filter inventory stays 152 cpp / 149 headers. README OA112 and FP100/FP101 index retained failure paths. ASM/static verification only; no build/runtime rerun. See [GUIDE_MANAGER_STARTUP.md](GUIDE_MANAGER_STARTUP.md). Next guide block: connection/goods scanning callback 0041B936 and insertion/removal 0041B061/0041B0A5, which remain tracked dummies.

The remaining guide callback dummy now returns the original unconditional true result, so registering it does not abort the callback chain. Its guide work remains pending and excluded from mapping counts.

## In-game default handler and Settings window - 2026-10-04

Historical opening-only checkpoint, superseded by the complete Settings block below. The preceding menu/listbox/guide work is committed as 1ef245b (Restore menu and guide initialization). The user confirms that the null virtual-table screenshot occurred on the Settings button. The default handler now has genuine construction/deletion and its full table. The 0xF3C branch connects the original Options toggle/open path, centered window construction/destruction, frame registration, queued single-player pause/restore and mover widget. Common window controls, six tabs, clock and initial page are reconstructed; the five other page bodies and option actions remain explicit partial dependencies. Overview/speed/priority branches are also connected. Fifteen genuine mappings replace two constructor/destructor dummies, while three newly exposed partial entries remain tracked: 2,083/5,056 (41.20%), 2,973 unmapped candidates and 123 unfinished entries. Project/filter inventory is 154 cpp / 151 headers; twelve engine-startup constructors remain. README OA113/OA114 and FP102..FP105 index ownership, deferred commands and partial-page limits. ASM/PE/source verification only; no build, tests or runtime run. See [IN_GAME_DEFAULT_HANDLER_STARTUP.md](IN_GAME_DEFAULT_HANDLER_STARTUP.md). Next: Options page bodies/actions 004C00E1/004BFA55, followed by remaining 004A3EDB dispatch.

## Complete Settings pages and actions - 2026-10-04

All six Settings pages and their native commands are connected, including scroll/display/audio properties, CD controls, savegame sorting/loading/saving, autosave and quit/restart confirmation. The native IO tree now includes header writing, all manager serializers/load constructors, map-plane writers, byte/word RLE and embedded restart ownership. AI and Mission startup constructor dummies are replaced. The unrelated 004A3EDB dispatcher remains partial; shared engine IO and mission/AI/gameplay callbacks remain unfinished and can still limit restoration/gameplay.

Static ASM/PE/source audits confirm all 74 related virtual slots, the 21-manager save order, matching project/filter inventory and 118 stub/inventory entries. Coverage is 2,195/5,057 (43.41%), with 2,862 unmapped candidates; 157 cpp / 153 headers and ten unfinished startup constructor boundaries. No build, tests or runtime were invoked. README OA114..OA117 and FP104..FP111 index the original error/unchecked paths. See [IN_GAME_OPTIONS_WINDOW.md](IN_GAME_OPTIONS_WINDOW.md). Next: remaining default-handler commands at 004A3EDB.

## Mission start-object commands - 2026-10-04

The complete StaticObjects dispatcher, family/home assignment branches and miscellaneous human-name/palette postpasses are connected in C2MissionStartObjects.cpp. Array slot allocation, positional moveable iteration, participation/mission-check predicates, house engine-name lookup and the complete vehicle passenger attachment tree follow Windows assembly. Core object creation remains unfinished; no initial exploration is synthesized. The root/parser dummies are removed, and fourteen newly exposed dependency boundaries remain isolated and excluded from coverage. Current source audit: 2,346/5,057 (46.39%), 2,711 unmapped candidates, 117 unfinished entries, 165 cpp and 158 headers. ASM/PE/source checks only; no build or runtime run. See [MISSION_START_OBJECTS.md](MISSION_START_OBJECTS.md). Next: CHumanArray::AllocateHuman / Human_Init, placement and initial exploration, then other creators and mission execution.

## Map moveable placement and pathfinder ownership - 2026-10-04

The moveable allocation/init/exit tree, pathfinder allocation/init/free and logical micro/sector attachment are reconstructed. Human, animal and vehicle sector lists preserve their distinct native categories, neighbour updates and counter order. Twenty new mappings and two relocated accessors bring the audit to 2,366/5,057 (46.79%), with 2,691 unmapped candidates and 117 unchanged unfinished boundaries. Project/filters match at 166 cpp and 159 headers. No builds, tests or runtime runs were invoked. README OA128/OA129 and FP131..FP134 index retained failure/unchecked paths. See [MAP_MOVEABLE_PLACEMENT.md](MAP_MOVEABLE_PLACEMENT.md). This prerequisite does not replace the human creator; next are the correctly named CHumanArray::AllocateHuman and Human_Init, their job/cleanup trees and the native initial exploration call.


## Overview pattern-query correction - 2026-10-04

Completed: replace 0048114E/00481188 global dummies with real NC2E2 pattern queries and connect the overview. The six-output query now initializes its outputs through the existing native helper. Pending: user runtime confirmation of map loading and overview sampling; house/home/work initialization and separate dynamic-object drawing remain the next reconstruction blocks. No build or tests performed. See [ENGINE_GROUND_DRAWING.md](ENGINE_GROUND_DRAWING.md).


## Intentional mouse modernization - 2026-10-04

The user requested debugger-friendly mouse behavior on `codex/modernize-mouse-input`, based on commit `885cc57`. Active mouse input now uses foreground Raw Input instead of constructing DirectInput; debugger-attached and remote absolute-input paths do not warp or clip the Windows cursor. Buttons/wheel keep existing messages, focus loss clears state, and the bounded ordered message budget is 1024. Original object offsets remain unchanged. This is an explicit modernization exception to ASM equivalence, not a reconstruction coverage increase. Registration/read failure handling and pending runtime checks are documented in [MOUSE_INPUT_MODERNIZATION.md](MOUSE_INPUT_MODERNIZATION.md). Static review only; no build or tests.


## Window-close shutdown correction and mouse confirmation - 2026-10-04

The user confirms debugger mouse lag is gone on codex/modernize-mouse-input. A separate X/close exception exposes virtual dispatch through the texture list's intentionally cleared vptr. Four engine list adapters now use qualified direct destruction, preserving Windows cleanup order and original constructor zeroing. README OA134/FP143 indexes the reconstruction defect. Static review only; closing after the fix still awaits user runtime confirmation. See [ENGINE_SHUTDOWN.md](ENGINE_SHUTDOWN.md).


## In-game minimap controls - 2026-10-05

Small-overview deletion/drawing and manager reopen-button handling are reconstructed. Large-overview toggle/open, complete window construction/destruction/drawing, thirteen filters, marked-state feedback, two zoom controls, shared pause/resume and the complete saved fifteen-byte flag region follow Windows ASM. Overview camera click/drag command 2004 is now connected; the rest of the world-input dispatcher remains a tracked partial implementation. Three complete dummies are removed (115 unfinished entries remain). Human right-click movement, general selection/world input and input-mode reset remain separate missing blocks. Existing production units/headers are already in project and filters. README OA135/FP144/FP145 records assumptions and failure branches. Static review only; no build, tests or runtime verification. See [INGAME_MINIMAP_CONTROLS.md](INGAME_MINIMAP_CONTROLS.md).


## Human minimap movement commands - 2026-10-05

The overview now calls the real Misc_Tool_SendSelectedHuman for single/group human selections. Target-point filling/callbacks, complete weighted/unweighted SearchOverMap, AddHumanCommand and both responding-sound overloads plus display-oriented playback follow Windows ASM. The whole sixteen-queue region 00512620..00550E5F is named sMapPathfinderStaticVariables and shared with startup/sector search. The human command dummy is removed (114 unfinished inventory entries remain). Existing command execution/storage is reused; this does not complete general selection/world input or simulation. Both new cpp/header pairs are included in project and filters. README OA136/FP146..FP149 indexes the rejection paths and retained assumptions. Static review only; no build, tests or runtime run. See [HUMAN_MINIMAP_MOVEMENT.md](HUMAN_MINIMAP_MOVEMENT.md). Next: remaining WorldDisplayElement_MessageHandle selection/mouse branches and InputMode_Set/selection controls.
