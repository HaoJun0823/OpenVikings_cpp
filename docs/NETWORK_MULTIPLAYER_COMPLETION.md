# Network and multiplayer entry completion - 2026-10-11

## Scope and counts

The next smallest nonempty thematic inventory was network/multiplayer: **148 / 164**, sixteen open native entries. All sixteen now have real implementations or distinct constructor/destructor boundaries connected to their existing native callers. Network/multiplayer is **164 / 164 (100%)** in this identified source-address inventory. Three necessary game-control dependencies are also completed, without changing their thematic classification.

Global source-address coverage increases by nineteen to **4189 / 5062 (82.75%)**, with **873** open entries. Naming inventory: **2317** unique addresses; explicit unfinished inventory: **45**. Project/filter parity remains **273 cpp / 253 headers**. Five dummy bodies and the partial game-control exclusion were removed; no replacement dummy was introduced.

These figures measure reconstructed entry points, not successful live multiplayer sessions. The global game-checksum aggregator, vehicle tick simulation and AI player-death consumers belong to other unfinished blocks. The multiplayer manager's own checksum is real; its caller in the broader global checksum remains unfinished. No full-game or network-desynchronization guarantee follows from the entry count.

## Multiplayer goal tree

Windows ASM/Game.exe establish every field and transition. Mac header names identify the corresponding manager methods only. Existing constructors and raw version-1 savegame serialization retain the native **740h** buffer.

| Entry | Reconstructed boundary |
| --- | --- |
| `004190FD` | Hash the complete 740h manager with the existing native checksum helper. |
| `0041910B` | Load `misc_multiplayer_goals`: goods, inhabitants, mission victory and mission defeat. |
| `00419303` | Original bare-RET token exit callback, represented by an exit-registration adapter. |
| `00419344` | Check all twenty players and all twenty goal slots, update result order and dispatch callback 30h. |
| `0041950B` | Unchecked player win/loss predicate. |
| `0041952C` | Clear pending departure flags, check active players and enqueue system command 89h. |
| `004195A4` | Type-1 callback every 120 ticks: pending departures first, goal checks second; always return true. |

The complete five-entry token region at `004F3820..004F3847` and lazy pointer/flag storage at `005114F8..005114FC` are represented. The loader sets the default player-death goal only for map type 4. It preserves the original exact-equality capacity checks, retained payload storage, eight goods pairs, parameter consumption and nonclearing behavior. Unknown commands are ignored by the original dispatch.

The manager layout is preserved: active byte at +0, twenty 14h-byte player results at +4, winner/loser counters at +194h/+198h, goal count at +19Ch and twenty 48h-byte goals at +1A0h. The evaluator deliberately scans all goal slots rather than using the goal count. Goods use signed thresholds and signed pair-count comparisons: zero pairs win, a negative count does not. Inhabitants use unsigned comparison and count WORD 0 or WORD 9. Mission triggers retain their existing native per-player bytes.

Result state and order counters are written before callbacks. Defeat's pending-removal byte is written after its callback, and the next 120-tick pass processes it. There is no invented active-manager guard in the callback. Player bounds, padding and raw saves retain the Windows behavior.

## Player-death and departure dependencies

`GameControl_System_DoOneGameTick` (`00409BCC`) now includes the native periodic defeat scan in addition to resetting pathfinder budgets. Starting at tick 720, every 125 ticks it checks each active, undefeated player unless `playerneverdies` is set. A surviving adult male human prevents defeat. Otherwise it writes the defeated byte, issues callback 2Ah and constructs/finishes/sends user message 3Eh in the original order. It does not itself manufacture a network departure command.

`GameControl_Player_LeavesGame` (`0040A295`) changes a connected network player with a nonzero peer ID from type 1 to type 2 and clears that ID before delegating to the separate object-modification routine.

`GameControl_Player_LeftGameModifyObjects` (`0040A2D4`) retains the unused middle parameter and uses the last Boolean to control removal. The native order is houses, humans, animals, vehicles and guides. Attached humans on continent type 1 create the original neutral animal after human destruction (player 20, tribe 1Eh, job 31h). Existing owned animals become player 20. Capacities and backing arrays are read through the existing native managers, preserving callback-sensitive iteration and allocation/free families. The neighboring game-static bytes at +1B0h/+1B1h are set/cleared exactly as Windows does, without guessing names for the surrounding unknown region. Both native network commands 88h and 89h now reach real implementations.

## Latency and native packet boundaries

`GameTime_Network_GetHighestPlayerLatency` (`0040B183`) snapshots the existing player-array pointer, obtains the DirectPlay singleton and scans the current player count using the native 98h stride. It skips zero and local IDs, queries existing `GetConnectionData` and returns the largest successful round-trip latency. Native automatic frequency adjustment now receives real samples.

`GetExistingNetworkInstance` (`004DC432`) returns the existing pointer without allocation; the native in-session predicate calls it before its ordinary singleton getter.

| Entry | Connected constructor/destructor |
| --- | --- |
| `004DD864` | Command packet: type 5, zero tick/length and 200 payload bytes. All three original call sites are connected, including command sending and re-send conversion. |
| `004DEE2D` | Chat packet: type 3, zero character count and 100h text bytes; both normal and special chat callers. |
| `004DEE4E` | Payload packet: type 4, zero size/checksum and 400h payload bytes. |
| `004DFFDF` | Four-byte file-transfer list constructor; implicit member construction precedes manager initialization. Windows folds this entry with sound-device pointer initialization; that equivalent sound initialization is retained. |
| `004E0A2C` | File announcement: type 11, zero GUID from 004F7FB0, zero sender and 100h filename bytes. |
| `004E0FFD` | Dynamic slot-array constructor: increment the original counter, then clear revision, +0 DWORD, tick, count and pointer in native order. |
| `004E1016` | Distinct destructor tail-jump entry to the existing slot release routine, connected in reverse three-array teardown order. |

Packet padding remains untouched. Constructors preserve field write order and the original memset lengths; subsequent callers overwrite only their original fields. Raw Game.exe bytes correct the disassembly export's malformed first two bytes at `004DD864` (`56 8B F1`, push ESI / mov ESI,ECX). No inline assembly is introduced. DirectPlay-specific packet names are descriptive names established from Windows accesses; no corresponding Mac symbols were found or used to infer behavior.

## Validation and next block

Fourteen unique changed/integration units passed MSVC Win32 `/Zs`: multiplayer goal access, game time, game-control network/startup, network runtime/shutdown, temporary stubs, map loading, logic lifetime, savegame state, user network commands, main-menu networking, network-info GUI and DirectSound manager. Project/filter parity, naming uniqueness, complete thematic totals and whitespace checks passed. No Debug/Release build, tests or game execution occurred. User-owned untracked CombatTasks sources remain untouched and excluded.

The next smallest nonempty inventory is common movement/combat/object lifecycle: **82 / 99**, seventeen open entries. Queued vehicles, other campaign predicates/actions, global checksum aggregation and AI remain explicit follow-up work.
