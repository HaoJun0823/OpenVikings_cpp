# House, construction and demolition rendering - 2026-10-10

Windows assembly and the reference Windows Game.exe establish behavior. Mac symbols supply matching names only. Current source and Git history were checked before moving existing house hit-test/sign descriptors into the shared drawing unit. This checkpoint completes the house drawing subtree; the surrounding field routine remains partial and excluded from completion coverage.

## Connected Windows tree

| Windows entry | Reconstructed behavior |
| --- | --- |
| 00495B57 | House drawing: brightness, marking, completed images, construction/upgrade layers, demolition and ordered effects |
| 00496594 | Shared image helper, now accessible to house rendering; final Boolean remains unused as in Windows |
| 0049660E | Empty debug-color exit callback, registered on the first debug draw |
| 0049660F / 00496737 | Existing image/animated-overlay hit-test descriptors and resident/worker/soldier signs, moved to the shared house unit |
| 00496A95 | Add or remove one fire point per draw, preserving unsigned arithmetic, health clamping and RNG order |
| 00496B79 / 00496C02 | Separate foreground/background demolition smoke loops |
| 004341C0 / 0043423E | Action and landscape overlay predicates with the distinct original bounds |
| 00434C1A | House damage predicate using the engine type's stage-specific repair threshold |
| 0047D0EF | House marking lookup; only the primary reference validates generation |
| 0049BBA4 | Vertical run-end outline, distinct from the existing filled-run outline at 0049BDC8 |
| 0049DA62 / 0049DDF8 / 0049E1B7 | Construction time mask, collapse time mask and ranged collapse time mask |
| 0049E5A3 / 0049E618 | Bottom-up expired-row scan and construction shadow time mask |
| 004396DC / 0040EC57 / 0040ED30 | Font-palette character printing and bitmap text overloads used by native house debug labels |

The exact separate Mac name for 0049BBA4 remains unresolved; `PrintBob_FX_DrawOutline_Lines` describes its confirmed Windows implementation. It is not merged with 0049BDC8. The exit callback's semantic reconstruction name is also explicitly recorded without an invented Mac correspondence.

## Observable details retained

Completed houses draw shadow, base image, action overlays, landscape overlays and the door image in native order. The door iterator ignores hidden people and chooses the open/closed image from the first visible person within the original radius. Animated overlays use the game tick and configured frame interval.

Construction and upgrades use the stage's five-entry construction-image array, matching the upgrade flag and configured start/end percentages. The two integer divisions used to produce the 0..255 construction threshold are preserved. An upgrade retains the previous completed image beneath the new layers. During demolition, expired bottom rows shift the remaining image downward; the final returned row count also shifts fire points, with the original last-point cutoff. Ranged masks preserve strict start/end comparisons, while the normal construction mask includes its threshold. The native early return for missing demolition image managers is retained.

Back smoke precedes the house image. Incense, damage fire and inhabitant signs follow the image; front smoke follows those effects. The full existing 0xD0-byte dynamic-house record is consumed: fire count, 200 flags and selection byte at +0xCC. The already reconstructed special-landscape arrays at 00568A80..00568ABB supply house fire, smoke and incense IDs. The complete lazy debug-color region at 00568B3C..00568B40 and its exit registration are represented. No additional effect allocation or ownership family is introduced.

All mask functions support the original 16-bit and 32-bit destinations, run decoding, clipping and palette checks. The 16-bit construction shadow uses the original darkened lookup table. The unusual 32-bit packed-color arithmetic is translated literally instead of replaced with a generic blend. Collapse return-value differences on clipping, invalid types and unavailable palette tables are retained.

The foreground house branch 0049180A..0049189E is connected in `Display_l_DE_DrawLandscapeAndCreaturesField`. It requires the house anchor flag, follows the original visibility and display suppression flags, and updates house/damaged-house counters even when house images are suppressed. Active foreground draws clear and update the complete 20-entry visible-player music count region. The existing generic human sound dispatch now follows the native active-foreground/sound-manager gate, so previews do not consume its tick/RNG state.

## Remaining boundaries and verification

Correction to the previous panel checkpoint: 004956B7 draws guideposts, not construction sites. House construction is part of 00495B57. Guidepost rendering at 004956B7 and its parts at 004958FB are the next connected foreground subtree, followed by road/wall construction markers, the generic particle drawing branch and repair-hammer sound dispatch. `004911D5` stays in the unfinished inventory and is excluded from completion counts. The house path also improves the connected display-engine previews, but full preview/gameplay completion is not established without the remaining field branches and a user runtime check.

MSVC Win32 `/Zs` passed for the affected drawing, selection, display, house, bitmap and font units, plus house-window and world-preview integration units. Project/filter parity, source existence, unique naming entries and whitespace checks passed. No build, tests or game run were performed. Coverage numbers are recorded in the current project-state and progress checkpoints; they measure source-address mapping, not verified gameplay completion. User-owned untracked `C2HumanCombatTasks.cpp/.h` remain untouched and excluded.
