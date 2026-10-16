# OpenVikings Remaster Changelog

News and changes from the `remaster` branch. Times are local to Vienna, Austria; the entries below use CEST (UTC+02:00).

## 2026-10-16 · 18:15 CEST — A modern foundation for the game

The remaster now uses the current Microsoft C++ runtime and Windows services for its shared system functions. This removes a large compatibility layer inherited from the original executable and simplifies future maintenance.

- Removed the reconstructed legacy runtime, custom small-block heaps and private compiler/math backends.
- Removed dependency on discovering internal functions in old Windows runtime DLLs.
- Moved file access, memory management, text handling and shutdown callbacks to supported runtime services.
- Replaced custom x87 math bridges with standard math functions and moved the remaining MMX blending instructions to SSE2.
- Kept the existing game-data formats, 32-bit Windows target and application initialization order.
- Kept networking modernization reserved for its separate project.

## 2026-10-16 · 16:45 CEST — Gameplay, audio and loading fixes

This update addresses inherited problems in education, equipment, transport, file loading and the interface. It also makes custom game data safer to load, while keeping the original game's data formats and the 32-bit Windows target.

### Villagers, transport and navigation

- School training no longer awards unrelated experience for the pupil's previous profession. The selected training still grants its intended skill.
- Equipment unlocks use the equipment list's own length instead of the number of production goods. Soldier and hero equipment checks now use equipment permissions rather than unrelated goods permissions.
- Military profession changes also update villagers who are temporarily detached from the map.
- Water-sector navigation selects its own flood-fill group instead of accidentally reusing the land-sector counter.
- Vehicle reservations start with clean counters when a slot is reused and remain active until their reserved amount is exhausted.
- Vehicle stock changes stay within capacity and the storage limit of each counter. Reservation records track the amount actually applied, and a full reservation list rejects a new reservation without changing future stock.
- Failed walking and vehicle-door searches release reservations rather than leaving goods permanently promised. Food pickup and cargo delivery handle reservation failures before consuming or discarding items.

### Graphics and interface

- Brightness calculations stay within the allocated palette range.
- Empty minimap texture samples use a black fallback instead of dividing by zero.
- Button-caption queries handle missing closing braces, empty output buffers and missing captions safely.
- Links between help-text files now use the requested destination anchor instead of always jumping to anchor zero.
- Missing named text blocks release their loaded buffers. Oversized block names and incomplete text reads are rejected cleanly.

### Audio and loading

- WAV loading finds the actual RIFF format and sample-data chunks, including optional chunks and padding, instead of assuming a fixed sample offset. Invalid or truncated chunks are rejected.
- Sound creation releases temporary resources if buffer creation, allocation or filling fails. Locked sound buffers are unlocked on read/reset failures, and memory-backed audio reads advance their cursor.
- Reloading a sound finds an existing entry even when an earlier preload slot has been freed, avoiding duplicate sound resources.
- Unknown jingle events no longer play the first unrelated jingle.
- Archive fallback keeps using the selected archive across search paths. Disabling search paths now also disables that fallback, and oversized expanded paths are skipped.
- Completed or inactive directory searches return normally instead of entering an endless loop.
- House, vehicle and tribe definitions reject invalid goods, passenger, job and animation indices. Oversized experience prerequisite lists cannot overwrite neighboring records.

## 2026-10-16 · 16:16:52 CEST — Keep generated editor files out of the repository

The repository now excludes the Visual Studio resource editor's generated cache. The resource script remains the maintained source for Windows resources.

- Removed `OpenVikings.aps` from version control.
- Added `*.aps` to `.gitignore` so regenerated caches stay local.

## 2026-10-16 · 16:16:28 CEST — Retire the old DirectInput mouse backend

The remaster already uses Windows Raw Input for its active mouse handling. This cleanup removes the unused backend and its remaining build dependencies.

- Removed the legacy DirectInput mouse implementation and export-definition file.
- Removed DirectInput import-library generation, the symbol alias and the unused linker dependency.
- Kept the active mouse handling and cursor behavior unchanged.

## 2026-10-16 · 15:50 CEST — First inherited gameplay and audio fixes

The first correction batch tackles stuck workers, unintended unlocks and several longstanding audio problems.

- Partial school training no longer unlocks unrelated jobs or goods. Completing the selected training still grants its intended unlock; existing saved unlocks are preserved.
- Workers arriving after another builder has finished a vehicle can leave the completed construction task correctly.
- An overly restrictive combat distance check now allows the affected military mode to consider targets between 19 and 30 map points away. Weapon minimum ranges still apply.
- Valid map-edge markers receive full initialization. Invalid placements outside the map no longer consume marker slots, addressing a possible crash involving markers and AI guides.
- Projectiles and other particles that leave the map are retired so their slots can be reused.
- Failed sound-buffer restoration returns to the game instead of retrying forever.
- Volume fades start from the current level, reach their target and release their transition slot. Fading to zero stops playback.
- Zero-duration volume transitions apply immediately without dividing by zero.
- Sound-effects volume changes preserve each sound's relative level and stereo position, including during fades. Muting sets the affected sounds to silence.

## 2026-10-16 · 12:33 CEST — AI takeover for multiplayer maps played solo

Multiplayer maps can define several human starting positions. When one of these maps is launched through single-player mode, the other active human positions now receive AI control rather than remaining idle. This applies generally, including to cooperative maps.

- Kept the local player's starting position under human control.
- Assigned the other active human starting positions to the AI when starting a new multiplayer map in single-player mode.
- Preserved existing player assignments when loading saved games, along with the map's diplomacy and scripted AI controls.

## 2026-10-16 · 12:22 CEST — Weather in 32-bit color

Weather effects now work with the remaster's 32-bit graphics path. Obsolete 16-bit output code has also been removed while retaining support for legacy source assets.

- Ported rain, snow and sandstorm overlays to 32-bit color.
- Preserved weather simulation, effect ordering and the existing detail and zoom controls.
- Removed obsolete 16-bit output fallbacks, shader variants and darkening tables.

## 2026-10-16 · 11:52 CEST — Modern Windows presentation

The remaster replaces the old window-presentation system with DirectX 11 and a modern Windows window. The goal is a comfortable experience on current displays while preserving the original game's appearance.

- Added borderless fullscreen, mouse confinement and Alt+Enter switching to a resizable window.
- Stopped repeated desktop-resolution changes between startup images, videos, loading screens and menus.
- Sorted the resolution list by size and retained only 32-bit modes, without a redundant color-depth label.
- Preserved the aspect ratio of game resolutions such as 4:3, using black bars for unused screen space.
- Scaled the HUD, main menu, cursor and loading bar with the resolution, using the interface at 1024 × 768 as the reference.
- Enlarged the minimap and improved its frame, transparency, shadow and spacing. The narrow inner edge is retained.
- Stretched videos to fill the window and added smoothing when enlarging them.
- Corrected flickering between the game cursor and the Windows arrow.

---

Networking modernization is reserved for a separate project. The `reversed-game-authentic-behaviour` branch remains the unchanged legacy reference.
