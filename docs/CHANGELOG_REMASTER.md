# OpenVikings Remaster – Player Changelog

## Upcoming Changes – Unreleased

The following fixes are implemented in the source code. In-game verification is still pending.

### Maintenance

- **Fewer legacy dependencies:** Removed the unused DirectInput mouse backend and its build dependencies. Mouse handling continues to use Windows Raw Input, as it already did in the remaster.

### Gameplay and Stability

- **School training:** Starting an education no longer accidentally unlocks unrelated jobs or goods. Completing the selected training still grants its intended unlock. Unlocks already stored in saved games are preserved.
- **Vehicle construction:** When several workers build the same vehicle, a worker arriving after construction has finished can now leave that task correctly instead of getting stuck.
- **Combat behavior:** An overly restrictive distance check has been extended. Soldiers in the affected military mode can now consider targets between 19 and 30 map points away instead of getting stuck on that check. Their weapons' minimum range still applies.
- **Map-edge markers:** Valid markers are now fully initialized even at the edge of the map. Invalid placements outside the map no longer consume marker slots. This addresses a possible crash involving edge markers and AI guides.
- **Projectiles and other particles:** Particles that leave the map are now removed properly. They no longer remain active but invisible, and their slots can be reused.

### Audio

- **No endless wait for an audio device:** If restoring a lost sound buffer fails, the call now returns to the game instead of potentially hanging forever.
- **Correct fades:** Volume fades start from the current volume, reach their target and release their fade slot when finished. Fading to zero stops the sound.
- **Instant volume changes:** A fade with a duration of zero now applies immediately, avoiding an invalid calculation.
- **Volume controls preserve the mix:** Changing the overall sound-effects volume preserves each sound's relative volume and left/right position, including during fades. Muting sets the affected sounds to silence.

## Previously Implemented Remaster Improvements

These features were implemented in earlier updates. The current visuals and weather effects, along with AI takeover on one affected map, have been confirmed in gameplay. Broader map coverage and additional edge cases remain under review.

- **Modern fullscreen window:** Borderless fullscreen with mouse confinement and DirectX 11 presentation. Switching between startup images, videos, loading screens and menus no longer repeatedly changes the desktop resolution.
- **Cleaner resolution list:** Resolutions are sorted by size, use 32-bit color exclusively and omit the redundant color-depth label.
- **Correct aspect ratio:** Resolutions such as 4:3 retain their proportions, with black bars filling unused screen space.
- **Readable interface:** The HUD, main menu, mouse pointer and loading bar scale with the resolution, using their appearance at 1024 × 768 as the reference.
- **Larger minimap:** A larger map area with an improved frame, transparency and shadow. Excess spacing has been reduced; the narrow inner border is retained.
- **Larger videos:** Cutscenes fill the window and use smoothing when enlarged.
- **Stable mouse pointer:** Fixed flickering between the game cursor and the Windows arrow.
- **Weather in 32-bit color:** Rain, snow and sandstorms now render with the new presentation system.
- **AI takeover when playing multiplayer maps solo:** Starting a multiplayer map in single-player mode assigns the other active human starting positions to the AI, so those tribes no longer remain uncontrolled. This applies to multiplayer maps generally, including cooperative maps. Existing saved games retain their previous player assignments; the change takes effect when starting a new map.

Other known issues will be addressed in subsequent updates. Multiplayer and networking are reserved for a separate modernization project. The original branch remains an unchanged legacy version.
