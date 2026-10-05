# Human movement from the minimap

Date: 2026-10-05. Windows ASM/source review only; no build, tests or runtime run.

## Completed command path

The overview's existing right-click handler (`004CF81A`) validates the map point and requires input mode zero. Human selection modes one and two now enter the real `CInputManager::Misc_Tool_SendSelectedHuman` implementation (`004B7B62`). Other selection modes are unchanged.

A single human receives the clicked packed micro-grid point. A group first calls `l_Selection_FillTargetPointArray` (`004B7DED`) with radius zero and the selected count. `slInputManagerSearchForTargetPoints` (`004B7E3E`) counts all candidates, including the initial point and rejected points. It preserves the micro-point low-three-bit radius check and the unsigned `outputCount * spacing <= visited` condition. Spacing comes from twice `VE_MicroPoint_GetMicroPointCount` (`0042B40F`).

Group command submission skips human flag `+0xB84 & 0x20`. The original selected index still advances for skipped humans; target selection uses that index modulo the found target count. There is no invented zero-count fallback. Both branches form a zeroed twenty-byte command with opcode four, human ID at `+4` and packed target at `+0x0C`.

`CUserNetworkCommandManager::AddHumanCommand` (`00410E2E`) accepts opcodes 2..87, checks the human's occupied byte, fills the player byte and allocation revision, and submits through the existing `AddCommand`. The existing `Execute` validates allocation/revision and passes the command to `Human_DoAddNewUserCommand`; its opcode-four handling and command storage already exist. This block does not replace the unfinished general selection/world-input or simulation callbacks.

## Map search and shared static storage

The complete `CMapPathfinderElement::SearchOverMap` (`00449706..00449AB7`) now implements both native branches. It rejects radii above 511 before invoking callbacks or statistics. The initial callback runs before clearing calculation/scratch state. Callback success returns immediately; exhaustion increments the original failure counter only for player IDs 0..19.

Weighted search uses twelve cyclic queues, the six source walk-direction bits, neighbor blocking/cost bits, the original `radius * 2 + 12` budget and pending-point count. Unweighted search alternates two queues, ignores costs and retains its unsigned radius decrement. Both call the callback before capacity checking and calculation-map marking. A queue with exactly 4000 points rejects another insertion without marking it. Neither branch adds wrapping, guessed nearest-point behavior or a separate allocation.

The whole Windows region `00512620..00550E5F` is now named `sMapPathfinderStaticVariables`, matching the Mac symbol. It consists of sixteen `0x3E84` blocks, each holding a DWORD count and 4000 packed DWORD points. Windows xrefs include the startup constructor, weighted pathfinder searches, two-queue searches and walk-sector flood fill. Startup still clears only point payloads; the existing sector path uses the same first two queues. Searches remain non-reentrant because both queues and calculation-map state are shared.

## Responding sounds

Command submission precedes the human-ID `PlayRespondingSound` overload (`00485A4E`), which reads the active display. The human/display overload (`00485931`) retains the DirectSound initialization gate, hero selector zero, tribe/job exclusions, adult/sex selection and `selector % responseCount` lookup (`004856F9`, `00485A73`). It suppresses a response if any wave variant for that response is already running and then repeats the native response lookup.

`Tool_Sound_PlaySoundOrientedOnDisplay` (`00490EEA`) preserves optional display/visibility/distance rejection, signed display-relative pan calculation, runtime-random variant selection, the running-wave check, unsigned volume arithmetic and pan clamping only when attenuation is disabled. Human responses request repeat count one, skip-running enabled and attenuation disabled. Existing preloaded-wave ownership and playback remain in `DSManager`.

Mac confirms public class/method names, the target-search callback and shared static symbol. Its compiler inlines the two small sex/age and response-index helpers; their local C++ names describe Windows-confirmed operations, rather than claiming recovered Mac symbols. Callback-data type name `SMapPathfinderManagerSearchOverMapCallbackData` is recovered from the Mac header; Windows establishes its eight-byte layout. The distance field and target-search local field names are inferred from reads/writes.

## Retained failure paths and verification limits

- Invalid selection modes do nothing; group-locked humans are skipped, while single selection retains the original lack of that group-only check.
- Invalid command opcodes and unoccupied humans are ignored by command submission. Playback still follows the caller's original order.
- Search exhaustion returns false; oversized radii return false before statistics/callback work. Full queues are not expanded. Shared state is not made reentrant.
- Unavailable/uninitialized DirectSound, missing response IDs, zero variants and already running variants suppress playback. The optional spatial path rejects outside, unseen or distant points.
- Owners/IDs/display dimensions retain original validity assumptions. Pan calculation divides by width/height; zero dimensions are not silently treated as a valid sound position. The group path retains its native nonzero-target-count requirement.

README OA136 and FP146..FP149 index these findings. The `004B7B62` dummy and inventory row are removed, leaving 114 unfinished inventory entries. Both new production units and headers are included in the Visual Studio project and filters. Static review covers selection branches, command layout, queue ordering/budgets/capacity, callback layout, sound gates and symbol uniqueness. Runtime behavior is still unverified.

Next logical block: the remaining `WorldDisplayElement_MessageHandle` mouse/selection branches and `CInputManager::InputMode_Set`/selection controls. User runtime follow-up for this block: issue minimap right-click commands for a selected human and group, including group-locked units; verify distinct destinations, sound suppression and successful deferred execution.
