# Main-menu GUI manager reconstruction

Snapshot: 2026-10-02, HEAD d64247c (Reconstruct ingame GUI and networking) plus this uncommitted main-menu block.

## Scope and authority

All thirty-two distinct original call targets previously used directly in MainMenuGuiManager.cpp now use reconstructed entries or existing native implementations. This includes the complete Screen_UpdateGui switch, menu list ownership, provider/session/player enumeration, lobby validation, automatic hosting/joining, Human iteration, camera following and desktop work. The connected host/join/search tree is reconstructed through the DirectPlay COM and Windows thread/timer APIs. No replacement bodies were invented for deeper widgets or virtual callbacks.

Windows instructions establish control flow, widths, layouts, return values, allocation/free families and call order. Windows Game.exe supplies the complete tables, strings and instruction bytes at boundaries where the text export overlaps data. Mac references supply class/function names and parameter identities only. The prior in-game/string-output work was committed as d64247c before this block began.

## Entries and integration

| Windows entries | Native implementation / purpose |
| --- | --- |
| 004D2B7C, 004D2BDD | Embedded GUI object-variable list construction/destruction |
| 004D32FF, 004D3A6D | Desktop bitmap query and complete screen rebuilding |
| 004D6AA5, 004D6AEA, 004D6AF9, 004D6B08 | Element removal, frame registration/unregistration and frame dispatch |
| 004D6B8F, 004D33A4 | Default-message-handler construction and deleting destruction |
| 004D70B6, 004D71F5, 004D7286, 004D73E5 | Provider enumeration, default hosting, hosting and session search |
| 004D74A4, 004D757E, 004D75EB, 004D76CC | Session summaries and vector deletion, joining and player refresh |
| 004D782F, 004D7849, 004D7899, 004D79A7 | Change detection, TCP/IP selection, connection policy and start validation |
| 004D7AA7, 004D7B06, 004D7B0B, 004D7BA7 | Lobby broadcast, callsign forwarding and automatic host/join |
| 004D7C95, 004D7D56, 004D7D8D, 004D7DB7, 004D8169 | Chat clear, player lookup/index/description and lobby reset |
| 004DA53A, 004DAD1B, 004DAD5C, 004DADB4 | World-display construction and following/validation/update |
| 004DAE2F, 004DAE4C, 004DB0A0, 004DB407 | Menu mouse-pointer and primary-message-handler lifecycles |
| 004788C6, 004788E6, 00478917, 00478936 | Shared base-element construction, deleting destruction and state initialization |
| 004B8FD0, 004B9042, 004B9052 | Text-button caption, four-byte background color and tooltip replacement |
| 0042C1D4, 0042C392, 0042C3AE | Human iteration; constructor shares existing 004402A3 layout/body |
| 00477104, 00477859, 004103E8, 0040AEBD | Desktop work, deferred close-list processing, rectangle inflation and float speed forwarding |
| 004DC8E3, 004DCABA, 004DCB30, 004DCCEE, 004DD462 | DirectPlay provider/connection queries, hosting, searching and joining |
| 004DDC75, 004DDD28, 004DDE94, 004DDED2, 004DE100 | Session-list initialization/invalidation/pruning, summaries and deep snapshot copy |
| 004DC2C1, 004DE7D1, 004DE848 | Session-search timer callback, timer thread and timer construction |
| 004DEA48, 004DEB32 | Dynamic-network reset and attachment |

Existing native event formatting 004DB635, display construction, lists, GUIDs, string/runtime helpers, network-player queries, dynamic-network updating and shutdown are shared. The existing dynamic-memory release, dynamic shutdown, timer shutdown and session-list destruction bodies are exposed for reuse without changing their behavior. Desktop work is connected to both menu and in-game callers. Distinct Windows entries remain distinct native functions.

## Complete screen switch

The Windows table at 004D6A49..004D6AA4 contains twenty-three entries. It is data, not an extra function. All branches are reconstructed, including the large lobby case and its internal branch at 004D5CEC.

| Screen IDs | Reconstructed branch |
| --- | --- |
| 1-3 | Main menu and initial selection menus, with original captions/tooltips and command IDs |
| 4, 8 | Grouped clean-map/campaign lists, original group counts and row filters |
| 5 | Campaign-map graphics buttons using all nine position records, including the zero terminator |
| 6 | Savegame sorting, list filling and list controller |
| 7 | Difficulty and option selection |
| 9-13 | Callsign, provider, host/search and session selection |
| 14 | Lobby overview, clean-map/difficulty selection and savegame selection for lobbyValue 0/1/2 |
| 15 | Player list during transfer/preparation |
| 16-18, 21 | Status/error headings and the original back-button branches |
| 19, 20 | Original default/unimplemented GUI branch; automatic networking remains in screen initialization |
| 22 | Localized credits hypertext, rectangle inflation and timed scrolling |
| 23 | Two-choice menu with null tooltips |
| Other IDs | Original unknown/unimplemented strings and back button |

Preserved details include allocation before most caption queries, tooltip-before-caption order, unconditional title/button field writes after nullable construction, selected/disabled bytes, payload fields, list/desktop insertion order, twenty lobby slots, repeated host/local-player queries and singleton-based start validation. Savegame list filling, widget construction and inherited virtual behavior remain real original dependencies.

## Layout, tables and ownership

- CGuiManager remains 0x21B4 bytes. Its original constructor still constructs embedded lists and handlers before clearing the range ending at +216C. Destruction retains graphics teardown, singleton clearing, text-input exit, derived/base handler table transitions and list destruction order.
- The complete static region 005694B0..005694DB remains represented by SStaticVars. The final four-byte region 005694D8..005694DB stays unresolved: its first byte is written in default-message handling and copied to disabled buttons, but a stronger semantic identity is not established.
- The complete campaign button region 00508960..005089CB contains eight mission positions and a zero terminator. No adjacent data was folded into it.
- The PE supplies eighteen entries each for base element 004F43F4, bitmap tool element 004F6D58, default handler 004F7DD0, menu world element 004F7EC8 and primary handler 004F7F38. The mouse-pointer table at 004F7F20 has two entries. The bitmap tool table previously omitted its last three slots; they are restored. These are different families from the nineteen-entry base-tool/string-output tables.
- Original serial DWORD 00504E58 is initialized to one; all exported accesses assign the current value and then increment it. Other unexplained member offsets and inherited callback identities remain explicit.
- Desktop (13C), primary handler (2C), mouse pointer (14), world element (58), world display (994) and the one-byte GUI resource managers use Runtime_AllocateWithNewHandler and Runtime_Free. Placement construction and qualified destruction preserve the original allocation family.
- Session snapshots deep-copy the description/name and duplicate address +10 before +0C. Both success and failure cleanup preserve ownership. Summary vectors retain the four-byte element-count cookie and reverse name destruction. Provider/session/player GUI arrays retain their ten-row limits and original string-copy rules.

## Reviewed failure behavior

README OB093, OA078-OA082 and FP054-FP059 index this block's allocation, HRESULT, cleanup, polling and lobby paths. They distinguish confirmed Windows behavior from unverified runtime consequences.

- Provider enumeration expects the first BufferTooSmall result, allocates the returned byte size, but clears only the provider count in bytes. A failed second call leaves the allocation owned by its caller; the menu frees it.
- Empty/null-address search tests result != 0; explicit-address search tests signed result >= 0. Zero-return initial-state or missing-address paths therefore have different outcomes. These comparisons are preserved rather than normalized.
- Search failure deletes its critical section before stopping its timer. Timer creation does not handle failed CreateThread locally; arguments are freed by the thread only. The original timer and automatic-join loops contain no sleep. Pruning an expired session advances the traversal twice.
- Host failures report their actual stage; playerChangeCount is reset after Host even when it fails. Failed Connect stops searching before reporting; failed stop-search after successful Connect is logged but joining still succeeds. Dynamic attachment failure releases its own memory without adding a new disconnect.
- Lobby reset clears the complete 160-byte region before reading its mode at +1068 for the color loop. Start validation preserves the original first-match scans, empty/missing-map/player rejection and optional error output.

## Remaining genuine boundaries and next block

The target manager file has no live original FUN call expressions. This does not claim that the whole GUI subsystem is complete. Main-menu default/primary virtual message bodies 004D6BBB/004DB0D1, world draw/message/delete callbacks 004DA615/004DAAC1/004DA5BF, mouse drawing 004DAE69 and inherited base methods remain original callback entries. World construction retains exploration/darkening 0047E669/004DAB77; follow interpolation retains 0047C9DA, and filtered Human iteration retains position access 0042CE24.

Screen dependencies remain widget constructors 004B8B51, 004B8DA2, 004B8E2C, 004B8EAC, 004B8F1C, 004B9520, 004CFF78, 004CFFF5 and 004CBCD6; list attachment 004D01A3, savegame filling 004C1A3F, player-color lookup 0048B442 and hypertext scrolling 004CBF6C. Desktop work retains input/drag/draw dispatch 00477A82/00477D80/0047796A. Their declarations preserve original boundaries; none are empty substitutes.

Next coherent GUI block: menu default/primary message handlers and the shared text-button/list/hypertext families, then world/mouse drawing and the remaining desktop dispatchers. Static region 005694D8 and the full player-color table require their complete xref/theme analysis before further naming.

## Verification and certainty

- Fourteen affected translation units generate Win32 MSVC objects with /Od and /O2. Only established deprecated-network-API warnings remain in NetworkRuntime.cpp.
- All 123 production translation units pass Win32 /Zs syntax compilation with zero errors. This replaces the previous 116/117 result with 39 menu diagnostics.
- Project and filters cover all 123 implementations and 125 headers exactly once. Six implementations and three headers were added; platform and Debug/Release output settings are unchanged.
- Windows graph/branch review includes the complete screen switch, network call tree, allocation/free order, full PE tables and overlap boundaries. Mac naming was cross-checked independently of behavior.
- Provisional address coverage is 1,499/5,045 (29.71%): 3,546 candidates remain unmapped and 57 mapped addresses retain direct original calls. The corrected source scan excludes the screen table from function coverage. See FUNCTION_PROGRESS.md for method and limits.
- No tests/test projects were added. The subsequent full Win32 Debug/Release link audit still reports 267 unresolved symbols after integration corrections; game-runtime behavior remains unverified. See [LINK_BUILD_AUDIT.md](LINK_BUILD_AUDIT.md). Object/syntax success is not a gameplay-completion claim.

Audit artifacts are outside the repository under C:\Users\ravo\AppData\Local\Temp\OpenVikings-engine-shutdown-20261001 (mainmenu-complete, mainmenu-build and production-syntax.log) and OpenVikings-function-progress-20261001.
