# OpenVikings Remaster

An open-source engine replacement for **Cultures: 8th Wonder of the World**, bringing a beloved village-building strategy game to modern Windows PCs.

Build a settlement, help your villagers learn new trades, explore the world and lead your tribe through its adventures. OpenVikings aims to preserve the game's character while improving its presentation, controls and reliability.

[Getting started](#getting-started) · [Build from source](#build-from-source) · [Changelog](#remaster-changelog) · [Contributing](#contributing) · [Disclaimer](#disclaimer)

## What's new in the remaster?

- **Comfortable fullscreen:** DirectX 11 presentation, borderless fullscreen and mouse confinement, without repeatedly changing your desktop resolution.
- **An interface you can read:** Menus, HUD elements, the cursor and loading bar scale with your resolution.
- **A clearer view:** Correct aspect ratios, a larger minimap with an improved frame, and smoother fullscreen videos.
- **Weather restored:** Rain, snow and sandstorms render in the 32-bit graphics path.
- **Better solo play:** AI takes over the other active human starting positions when you launch a multiplayer map in single-player mode.

## Project status

**In active development.** The remaster can be played, and its current presentation, weather and solo AI takeover have been checked in gameplay. Recent gameplay and audio corrections still need in-game verification; bugs and compatibility issues may remain.

The `remaster` branch is where improvements happen. The `reversed-game-authentic-behaviour` branch preserves the legacy reconstruction, including the original game's known defects.

Next steps include further bug fixes, performance improvements and support for larger settlements. Networking modernization is planned as a separate later project. The current target remains **32-bit Windows**.

## Getting started

You will need:

- Windows 10 version 1703 or later and a graphics device supporting DirectX 11.
- Your own legally obtained copy of **Cultures: 8th Wonder of the World** and its original game data.
- A build of OpenVikings from the `remaster` branch.

Build the game using the instructions below, then set up the original game data following the [game-data setup guide](docs/BUILD_AND_ASSETS.md#original-data-and-current-datax-adaptation). The current setup expects a `datax` directory and the original music driver and media components. Run the executable with the game-data root as its working directory.

## Build from source

1. Check out the `remaster` branch.
2. Open `OpenVikings.slnx` in Visual Studio with the C++ desktop development tools, **MSVC v145** and a Windows SDK installed.
3. Select **x86 (Win32)** and **Debug** or **Release**, then build the solution.
4. Find the executable in `OpenVikings/Debug/` or `OpenVikings/Release/` and use it with your game-data setup.

The project uses **C++17**. It does not currently target x64.

## Contributing

Bug reports, code improvements and help checking the game are welcome.

For a useful bug report, include the revision you played, the map or savegame, steps to reproduce the issue and what you expected to happen. Screenshots help with visual problems. If you can compare the behavior with the original game, include what you found.

Keep pull requests focused and explain what changes for players. Please do not upload original game assets. Contributor notes and technical references are available in [the documentation](docs/README.md).

<!-- remaster-changelog:start -->
## Remaster changelog

### Upcoming Changes – Unreleased

The following fixes are implemented in the source code. In-game verification is still pending.

#### Maintenance

- **Fewer legacy dependencies:** Removed the unused DirectInput mouse backend and its build dependencies. Mouse handling continues to use Windows Raw Input, as it already did in the remaster.

#### Gameplay and Stability

- **School training:** Starting an education no longer accidentally unlocks unrelated jobs or goods. Completing the selected training still grants its intended unlock. Unlocks already stored in saved games are preserved.
- **Vehicle construction:** When several workers build the same vehicle, a worker arriving after construction has finished can now leave that task correctly instead of getting stuck.
- **Combat behavior:** An overly restrictive distance check has been extended. Soldiers in the affected military mode can now consider targets between 19 and 30 map points away instead of getting stuck on that check. Their weapons' minimum range still applies.
- **Map-edge markers:** Valid markers are now fully initialized even at the edge of the map. Invalid placements outside the map no longer consume marker slots. This addresses a possible crash involving edge markers and AI guides.
- **Projectiles and other particles:** Particles that leave the map are now removed properly. They no longer remain active but invisible, and their slots can be reused.

#### Audio

- **No endless wait for an audio device:** If restoring a lost sound buffer fails, the call now returns to the game instead of potentially hanging forever.
- **Correct fades:** Volume fades start from the current volume, reach their target and release their fade slot when finished. Fading to zero stops the sound.
- **Instant volume changes:** A fade with a duration of zero now applies immediately, avoiding an invalid calculation.
- **Volume controls preserve the mix:** Changing the overall sound-effects volume preserves each sound's relative volume and left/right position, including during fades. Muting sets the affected sounds to silence.

### Previously Implemented Remaster Improvements

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
<!-- remaster-changelog:end -->

## Credits

Cultures was created by **Funatics Software**. OpenVikings is a community project built out of appreciation for these games.

Related projects include the [Open Northland](https://github.com/s-nems/open-northland) and [Cultures Saga Patches](https://github.com/Basssiiie/cultures-saga-patches), whose community fixes help inform our bug investigations.

## License

The source code is licensed under the [GNU General Public License v3.0](LICENSE). This license does not cover the original game's proprietary assets.

## Disclaimer

OpenVikings is an independent, unofficial fan project developed through reverse engineering for education, research and personal enjoyment. It is not affiliated with, endorsed by or supported by Funatics Software or Daedalic Entertainment.

The original game, its trademarks and its copyrighted content remain the property of their respective rights holders. The original game's data files are not distributed with this engine replacement. You must provide your own legally obtained game data; this project grants no rights to those assets and does not endorse copyright infringement.

Rights holders with concerns about this project are welcome to [contact the maintainer](https://github.com/Ravo92).
