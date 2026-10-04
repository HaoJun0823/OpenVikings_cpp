# Main-menu ambient sound sentinel correction

Snapshot: 2026-10-04. The user reports a read access violation in `CManager::PreloadWave_Play` while the main menu loads. At the user's request, the existing Debug executable was launched under a temporary Win32 console debugger with the configured game working directory. No build was run.

## Reproduced failure

The recorded caller chain is `MainMenuWorldDisplayElement_XGui_BE_Element_Draw` -> `AmbientSoundManager_UpdateAmbientSounds` -> `AmbientSoundManager_PlayLandscapeAmbientSounds` -> `PreloadWave_Play`. The incoming wave identifier is `6E65475C`, whose bytes spell `\Gen`. The selected ambient record lies exactly 0x1570 bytes before the real fifteen-record table: ambient ID FFFFFFFF was treated as a valid index. Filename bytes from preceding memory became the wave identifier and volume. The DirectSound entry calculation matches Windows; it is the downstream failure location, not the source of the wrong identifier.

Both collectors incorrectly compared mappings with `00FFFFFF`. The native SoundFX constructor initializes unassigned mappings to `FFFFFFFF`. The instruction export displays `FFFFFF` at 0047F610, 0047F659 and 0047F80C, but the raw instruction is `83 /7 ... FF`, which sign-extends its immediate byte to FFFFFFFF for the DWORD comparison. Both the supplied reference Game.exe and the installed Windows game contain those same bytes:

| Address | Native bytes | Effective comparison |
| --- | --- | --- |
| `0047F610` | `83 BC 8B F8 01 00 00 FF` | Pattern mapping != FFFFFFFF |
| `0047F659` | `83 BC 8B F8 01 00 00 FF` | Pattern mapping != FFFFFFFF |
| `0047F80C` | `83 BC 8A 88 03 00 00 FF` | Landscape mapping != FFFFFFFF |

`C2E2AmbientSoundManagerAccess.cpp` now uses FFFFFFFF in both collectors. The source pattern loop represents the two distinct native pattern comparisons. Unassigned groups no longer contribute candidates; group ranking, valid sounds, volume calculation and native unchecked DirectSound indexing are unchanged. No artificial wave IDs or arbitrary range-check fallback replaces the original comparison.

The earlier README OA092 and NATIVE_LINK_SUPPORT claim of an original sentinel mismatch was incorrect and is retracted. This is a reconstruction error caused by reading the instruction text without accounting for immediate sign extension. Original malformed group-index concerns in OB071 remain separate.

## Runtime verification

The unmodified executable reproduces the access violation twice. For validation without rebuilding, only the two comparison immediates in the test process were changed to FFFFFFFF after checking their exact compiled instruction bytes. The EXE on disk was not changed. The corrected process ran for sixty seconds without that exception; a separate twenty-second probe also completed without an access violation and confirmed program state 8 (MainMenu) and GUI screen 1. Each debugger-owned process is terminated after capture/verification so no suspended hook remains running. This validates the comparison correction in the current executable, not compilation of the edited C++ or complete terrain/gameplay behavior.

The source mapping count remains 2,301/5,057; no function or project file was added to production. README OA092, FP120 and the reconstruction-error table record this correction. Runtime debugger captures are temporary artifacts outside the repository.
