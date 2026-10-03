# Main-menu button command dispatch

Snapshot: 2026-10-04, command block after d22ddbd (Palette and Colors). The later credits/keyboard follow-up is tracked separately.

## Reported failure and confirmed cause

The user reports correct button sound and click animation but no navigation. TextButton_DoPressedAction (004B936E) dispatches message 0x800, command from button +3C, sender pointer, payload from +4C and extra data from +50 through the handler's virtual slot +8, then plays confirmation sound. Screen_UpdateGui installs the manager's embedded default handler at +216C as the button owner. That handler's slot pointed to the isolated 004D6BBB dummy, which returned false without acting on any command. The animation and sound therefore worked independently of navigation. This is a missing reconstruction, not an original game defect.

The default handler now has a genuine Windows implementation. No input thresholds, animation or sound behavior were changed. The subsequent credits/keyboard follow-up reconstructs the primary handler and hypertext widget; see [MAIN_MENU_CREDITS_AND_KEYBOARD.md](MAIN_MENU_CREDITS_AND_KEYBOARD.md). The counts below retain this command block's historical snapshot.

## Complete Windows dispatch

The complete PE jump table 004D6FDA..004D7091 contains 46 entries for 0x1388..0x13B5. Mac supplies the class/method name, but Windows command IDs and destinations are decoded independently.

| Commands | Windows action |
| --- | --- |
| 1388 / 138A / 138B / 138C / 138F | Screens 2 / 1 / 3 / 9 / 22: quit confirmation, main, single-player, multiplayer and credits |
| 1389 / 138D | Add post-picture-sequence property or remove intro_skip; defer TitleScreen state 5 |
| 138E | Explicit no-op table entry |
| 1390 / 1391 | Return to screen 1 only with game CD, or set the original bypass byte then return |
| 1392..1395 | Screens 4, 5, 8, 6 for map/campaign/load selections |
| 1396 / 1397 | Reset startup, mode 1, copy 16-byte GUID from payload; defer Game state 11 or open difficulty screen 7 |
| 1398 | Reset startup, mode 2, store savegame ID at +14; defer Game state 11 |
| 1399..139B | Savegame sort types 2, 1, 3; rebuild screen |
| 139C..139E / 139F | Difficulty 0, 1, 2 at master +148; rebuild, or defer PreIngameFmv state 15 |
| 13A0..13A6 | Network return with shutdown, callsign/edit gate, provider selection, host/search/join and original error screens |
| 13A7..13AE | Lobby selection, selected map/savegame data and difficulty |
| 13AF / 13B0 | Toggle connection policy, or validate game start; send original kick messages to unassigned players and enter screen 15 |
| 13B1..13B5 | Select player; host-only kick, slot assignment, type change and color change |

Every message other than 0x800 and every unknown command returns true without an action. The original handler ignores its element, sender and extra-data arguments. In particular, payload is the third message parameter; it is not the button/sender pointer. Screen rebuilding removes the old buttons during dispatch, so no new sender access was introduced afterward.

## Direct dependency block

Eighteen distinct Windows entries are added: default handler 004D6BBB; CD predicate 004D32F4; provider setter/getter 004D71BA/004D71DC; connection policy 004D787B and CDPNet setter 004DD5BD; selected-player kick and unassigned-player kick 004D78A2/004D7922; selection/assignment/type/color 004D7D19/004D7E5D/004D7F7E/004D804F; next-color search 004D80A7; lobby selection/difficulty 004D80F6/004D814C; map selection 004D81C7; empty token exit 004D8454; savegame selection 004D8455. One dummy/inventory row is removed. Existing network, screen, startup-reset, property, archive and file/runtime methods are reused. Network-only source method names describe established Windows roles; no matching Mac network-menu symbols were found.

Map selection clears the 0x160 lobby region, copies its GUID, temporarily mounts a user-map archive and $maproot$ path, parses map.ini's multiplayer group, then assigns computer slots/colors, restores difficulty, rebuilds and broadcasts. Complete token array 004F7E58..004F7E77 contains playeroption, playerhideinmenu and playerfixcolors; complete state 005694E8..005694EF contains the token pointer and initialization flags. The path is cleared immediately after reader construction; user-map library removal precedes reader destruction, matching Windows. Unknown commands are ignored and option values above two are consumed without setting bits.

Savegame selection preserves the extra map-info lookup, initializes all twenty slots and copies stored player types/colors. Human names match the current player list case-insensitively, with first match winning. The saved player field +30 is now colorId throughout loading and lobby restoration; its spelling describes a Windows-confirmed role rather than a recovered Mac member name. Serialized layout remains 0x34 bytes.

## Retained failure paths and arithmetic

Provider selection rejects index >= count and logs the original text. Provider lookup checks only -1. Missing callsign starts editing. Host/search/join failures change to screen 18 with shutdown before recording their specific error. Invalid game-start data is added to chat as type two. CD absence leaves the gated command unchanged. Host-only player commands retain their gates; callbacks and rebuild/broadcast order remain as in Windows.

GUID pointers, slots, map/savegame lookup results and library allocations remain unchecked on their original paths. Map player indices use signed upper-only bounds, so negative indices retain DWORD-wrapped offsets. Initial fixed-color state is cleared with the lobby before INI parsing; changing it does not retroactively refill every earlier zeroed color. Hidden-color bits are preserved in the high byte of player flags. Color search increments with DWORD wrap, wraps exactly ten to zero, excludes hidden slots and returns zero on a complete cycle. Type transitions keep the original dual-flag precedence, and saved-game IDs inhibit color replacement. Slot assignment removes previous bindings before selecting replacement computer colors.

The two-byte CDPNet connection-policy packet is type two. An unchanged stored byte returns true immediately; a change outside host mode fails. The host stores its byte before SendPacket and retains it even if sending fails. GUI policy changes ignore that result and rebuild. The start command passes true to SetAllowConnections, as the Windows push does; it is not silently inverted. Type-three dynamic messages sent for kicks are 0x168 bytes and clients process them by changing to screen 21 with shutdown. README OA107/OA108 and FP091..FP093 index the retained paths.

## Verification and next work

ASM/PE/static verification checks all 46 jump-table destinations, command payload offsets, helper control flow, strings, token/state regions, archive lifetime, packet data, and unique definitions. Project/filters contain 150 cpp files and 147 headers; the new unit is under GUI. Provisional coverage is 1,983/5,056 (39.22%), with 3,073 unmapped entries and 130 dummies excluded. No builds, tests or game execution were performed; runtime confirmation of button navigation remains pending.

The primary keyboard/credits block is now reconstructed in the linked follow-up; next connected menu work is the tool-listbox owner/controller family. The queued Houses constructor 0048E651 and deeper drawing/gameplay work remain separate unfinished tasks.
