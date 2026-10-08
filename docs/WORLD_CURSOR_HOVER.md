# Windows world cursor and hover presentation

Behavior is confirmed from Windows assembly and Game.exe. Mac headers only supply corresponding names. Current C++ provides integration context; no Mac offsets, tables or branch behavior are used.

| Entry | Connected behavior | Status |
| --- | --- | --- |
| 004BB78A / 004BB7A7 | 0x14-byte object, two-slot table 004F7020 and scalar deleting wrapper | Complete |
| 004BB8FC | Hardware/software cursor, desktop tooltip timing, both world control schemes, command/human/house/landscape/animal text | Complete; contextual command preparation is now connected |
| 004BB7C4 / 004BC24C | Centered tooltip, PlaceInside, darkening, temporary bitmap reference, fill mode 2, four black outline offsets, white/dim text, CRLF splitting with 400-byte buffer | Complete; existing single-line helper moved and corrected |
| 004A2EA7 / 004A2EC0 | Ten manager bookmarks in +E8..+10F; getter zeroes output for an out-of-range index | Complete |
| 004B7585 / 004B7596 / 004B7E92 | Engine house type/extension selection getters and 29 input-mode string IDs | Complete |
| 00499019 / 004990A1 | Unzoomed map marker outlines/fills and second-to-first micro-grid line | Complete |
| 0040E943 / 0040EA1D / 0040EAF7 / 0040EBD1 | Separate indexed/high-color/true-color filled circles and RGB dispatch | Complete |
| 0048B5C1 / 0048B5FB | Player-color road/wall construction sign landscape IDs | Complete |
| 004BE4FB / 004E2154 | Landscape tooltip selector and vehicle command string table | Complete |
| 004B3264 | Bookmarks, target ring, house preview, human work marker, road/wall source signs | Partial; vector previews pending |
| 004BB0C1 | Prepared command insertion for single/group humans/vehicles and school chooser continuation | Complete; full hover preparation connected in 004BABE0 |

The software cursor deliberately invokes the override callback twice and reloads its pointer between invocations, matching Windows. Hardware mode invokes it once. Tooltip expiry uses the original unsigned comparison; no alternative wrap policy was invented. Hovered human information suppresses the fallback name/landscape/animal block. House tooltips retain native main-type filtering in control scheme 1; this is not a construction-completion check. Human responding voices follow queue insertion; vehicle dispatch adds no human voice. School window opening follows response playback and checks command 32, house revision and engine house type 38 without adding an allocation check.

Game.exe regions inspected in full: 004F7000..004F701F (two four-element outline arrays), 004F7020..004F7027 (pointer table), 004B7F23..004B7F96 (29 mode branches), 004E21DC..004E2217 (15 vehicle strings), 004BE610..004BE648 (landscape selector). Literals: CRLF 004FE480, space 0050071C/005083AC, parentheses 0050781C/00507818, experience format 005076C4 and amount/good format 005076EC. Cursor static colors have only empty native exit callbacks; C++ static RGB values preserve their observable lifetime.

Road/wall vector preview crosses 0042871C/00427AEA -> 0043E8AA -> 0043E992 and its route search branches. The current direction dummy returns zero repeatedly. Calling that loop would never terminate, so it remains an explicit unfinished branch and 004B3264 remains excluded from completion counts. No substitute path algorithm was invented.

The complete default-interaction preparation tree 004BABE0 -> human 00435605/004362E9 and vehicle 00437AAF/00437D9F is now connected, preserving native priorities, self-selection exclusions, modifiers, group intersection and command/secondary-command layouts. See [DEFAULT_INTERACTION_COMMANDS.md](DEFAULT_INTERACTION_COMMANDS.md). Next: multiple/non-human selection windows, remaining default-action dispatch and construction previews. Downstream combat and trader task gaps remain independent.

Validation: Win32 `/Zs` for all twelve affected/calling units, project/filter parity and source-address inventory. No Debug/Release build, tests or gameplay validation. User-owned untracked C2HumanCombatTasks.cpp/.h were not edited or registered.

Inventory delta: 20 mapped entries added, while source-only pointer entries 004BB7A7/004BB8FC add two known candidates. The remaining 18 new mappings resolve existing candidates. The moved 004BB7C4 helper is counted once; partial 004B3264 remains excluded.
