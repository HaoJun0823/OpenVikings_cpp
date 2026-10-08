# Windows contextual default commands

Behavior, constants, offsets and branch order are confirmed from Windows assembly and Game.exe. Mac headers supply corresponding names only. No Mac implementation behavior is used. `C2DefaultInteraction.cpp/.h` hold this functional block; `WorldDisplayElementAccess.cpp` connects it to the existing hover/cursor and prepared-command dispatcher.

| Windows entry | Reconstructed behavior |
| --- | --- |
| 004BABE0 | Hit testing, family/home/workplace/inhabitant markers, both command-buffer resets, selected-object exclusion and complete single/group human/vehicle preparation |
| 00435605 | Complete nine-way single-human target tree, including priority and secondary commands |
| 004362E9 | Complete group-human target tree and common-command comparison |
| 00437919 | Normal/group vehicle command availability, attachment, military mode and passenger exit controls |
| 00437AAF / 00437CB1 | Complete single-vehicle target tree and visibility/continent/modified point-command checks |
| 00437D9F | Complete group-vehicle target tree and common-command comparison |
| 004408CA / 004408DA | Native ten-entry vehicle command handler initialization and capacity check |
| 00412248 / 0041225D | Chest landscape classification and allowed jobs |
| 0042789B / 0042853B | Build-flag point membership and first matching owner; Windows shares these entries between road and wall managers |
| 0042F68A | Animal micro-point lookup through its map moveable |
| 00434C41 / 00434C62 | Primary worker job and remaining capacity for that job |
| 0044DA64 | Parent vehicle allocation/revision validation; aligned instructions read directly from Game.exe |

Single humans retain the original priority among enemy attacks, vehicle entry, builder/home/work/school assignments, trader house addition/removal, job changes, production changes, equipment, chests, construction flags, exploration and movement. Job change queues command 21 followed by house assignment 28. Production change queues 22 and, without a work house, a following point command 17. A chest queues 18 followed by movement 4; its destination tries native unwrapped directions 2 then 0 and falls back to the original point when both micro-points carry bit 0x80 at +0xC. Existing real queue insertion and human responding sounds in 004BB0C1 consume these prepared commands.

The command buffers remain exactly 0x14 bytes: opcode byte +0, actor ID +4, actor revision +8 and parameters +0xC/+0x10. Action identifiers are distinct from network opcodes. Group attacks/house assignments initialize from member zero and compare action, opcode and target ID for later members, without adding a target-revision comparison. Group equipment/chest/landscape-attack/vehicle-entry paths overwrite the prepared command for each eligible member, as Windows does. Any failed member clears the common action and both command buffers. Hovering an already selected member prepares no command.

Native differences between single and group behavior are preserved. Group humans do not run the single-human moveable/continent gate. Group vehicle point attacks do not run single-vehicle map/visibility/continent checks. Single-vehicle modified point attacks follow map/visibility checks but precede continent comparison. A failed single-vehicle house attack returns zero; other supported targets can fall through to movement. Single-human non-enemy human targets still query the active target's continent and return zero. Single-human unsupported/already equipped item kinds return zero; the corresponding group branch continues to chest/landscape attack checks.

Game.exe's entire nine-entry single-human selector at 00436216..00436239 was inspected. Equipment opcodes 81/82/80/79/83 are translations of the five Windows branches, not a recovered static pointer table. The ten-entry vehicle handler retains duplicate insertion and checks equality with capacity 10. Its `IsCommandAllowed` uses the same native 0044073F search as the human handler; Mac header line 9300 supplies the vehicle method name, and this shared address is counted once.

Build-flag manager records retain the observed count, player table and six-byte point stride. Existing private 0042789B and inline 0042853B reconstructions now call the shared named implementation. Unused record bytes remain unexplained. The existing house worker record's `unknown04` is used by the confirmed signed capacity comparison; its broader field naming remains unchanged. The outlined merchant helper 00436B16 has confirmed Windows behavior, but its separate original semantic name is unresolved and it is not newly claimed as an address mapping.

Remaining independent boundaries: vehicle/general default-action dispatch in 004B5CFF, road/wall vector previews and construction modes, multiple/non-human selection windows, and downstream combat/vehicle/task execution where the inventory still marks partial bodies. Completing command preparation does not establish complete execution of every resulting action.

Validation: all four changed/new cpp units passed MSVC Win32 `/Zs`; project/filter parity is 208 cpp / 194 headers. No build, tests or gameplay run. The refreshed source-address audit is **3282 / 5057 (64.90%)**, 1775 open candidates, 113 tracked unfinished boundaries and 1120 naming rows. Fifteen previously unmapped entries plus completed 004BABE0 add 16 mappings; moved 0042789B remains counted once, and the known-candidate denominator is unchanged. User-owned untracked CombatTasks files were untouched, unregistered and excluded.
