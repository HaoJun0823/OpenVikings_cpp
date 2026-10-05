# In-game minimap controls

Date: 2026-10-05. Windows ASM/PE/source review only; no build, tests or runtime run.

## Completed control paths

The small overview now has real deletion and root-bitmap decoration. Closing it clears the manager's window pointer and creates the original bottom-left reopen button. The existing default message handler's command `0xF45` opens the minimap again and deletes the reopen button. Mode cycling and both zoom buttons retain their existing Windows implementations.

The formerly empty large-overview toggle now searches for command `0x7E9`, queues an existing overview for closure, or opens the real 500 by 420 window. Opening removes selection controls, closes other large windows and retains the existing input-mode reset boundary. The window has the original title mover, close button, thirteen filter buttons, two zoom buttons, overview bitmap and frame. Opening/closing uses the existing single-player pause/resume helpers.

| Command | Action | Overview flag |
| --- | --- | --- |
| `0x2711` | Civilians | `+0x806C` |
| `0x2712` | Soldiers; clear civilians first | `+0x806D` |
| `0x2713` | Animals | `+0x806E` |
| `0x2714` | Vehicles | `+0x806F` |
| `0x2715` | Houses | `+0x8070` |
| `0x2716` | Guides | `+0x8072` |
| `0x2717` | Borders | `+0x8074` |
| `0x2718` | Pattern display; clear gray mode first | `+0x8075` |
| `0x2719` | Gray display; clear pattern mode first | `+0x8076` |
| `0x271A` | Roads | `+0x8077` |
| `0x271B` | Animal type 19 | `+0x8078` |
| `0x271C` | Animal type 10 | `+0x8079` |
| `0x271D` | Chest markers | `+0x807A` |
| `0x2774`, `0x2775` | Zoom out / in | Existing `ModifyZoom` |

Each filter updates the original text-button marked byte at `+0x55`. Borders, roads, pattern and gray changes invalidate the cached terrain. The complete fifteen-byte flag region `00569478..00569486` and validity byte `00569487` are now owned together; flags survive closing and reopening the large overview. Windows has only the constructor/destructor xrefs to this region. The Mac reference supplies the names `sDisplayFlagsStructure` and `sDisplayFlagsValidFlag`.

Overview left-click/left-drag already emits world-element message `0x1000`, command `2004`. Its previously empty receiver now checks the active display and hidden-element flag, converts map coordinates through the original local/global pixel helpers, sets the wanted camera center and ends follow mode. Windows still returns false after this command's side effects.

## Address boundaries

- Manager: `004A2322` toggle, `004A2358` open, existing `004A28A6` small open, `004A291B` small closed.
- Large overview: `004C4856` constructor, `004C4E9E` deleting destructor, `004C4EBA` destructor, `004C4EFF` message handler, `004C50C5` draw, `004C5107` marked status. Complete PE virtual table: `004F7504..004F754F`.
- Small overview: `004C9777` deletion and `004C9824` drawing. Complete PE virtual table: `004F78FC..004F7947`.
- World input: only the `004BA915..004BA978` overview-camera branch of `004BA3BE` is reconstructed. Keep the whole dispatcher excluded from completion counts.

## Remaining input dependencies

Human right-click command submission and its target-search/responding-sound tree are now reconstructed in [HUMAN_MINIMAP_MOVEMENT.md](HUMAN_MINIMAP_MOVEMENT.md). The overview's existing vehicle command submission remains unchanged. General selection, world input and `InputManager_InputMode_Set` remain separate unfinished entries.

## Original assumptions and runtime follow-up

Constructors, marked-status updates and the reopen-button path retain the original unchecked allocation/owner assumptions; a failed allocation is not silently converted into a successful UI action. The fifteen flag bytes belong to a live child until large-window destruction copies them. Zoom retains the original minimum of one and no added upper limit. Camera commands are ignored for inactive/hidden world elements.

README OA135 and FP144/FP145 index these paths. Three complete dummies are removed; the world dispatcher moves to a tracked partial production implementation. This minimap-control checkpoint left 115 unfinished inventory entries; the subsequent human right-click block reduces that to 114. All changed production units/headers already belong to both Visual Studio project and filters; no new production file is added.

User runtime follow-up: open/close/reopen the small minimap; cycle all four modes; use both zoom buttons and the wheel; click and drag the map; open/drag/close/reopen the large overview; toggle every filter and verify marked states, mutual exclusions, persistence and pause/resume behavior. Human minimap right-click command submission is implemented but runtime-unverified; general world-input and selection remain unfinished.
