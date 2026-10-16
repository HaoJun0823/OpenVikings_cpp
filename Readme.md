# OpenVikings Remaster

An open-source engine replacement for **Cultures: 8th Wonder of the World**, bringing a beloved village-building strategy game to modern Windows PCs.

Build a settlement, help your villagers learn new trades, explore the world and lead your tribe through its adventures. OpenVikings aims to preserve the game's character while improving its presentation, controls and reliability.

[Getting started](#getting-started) · [Build from source](#build-from-source) · [Changelog](CHANGELOG.md) · [Contributing](#contributing) · [Disclaimer](#disclaimer)

## What's new in the remaster?

- **Comfortable fullscreen:** DirectX 11 presentation, borderless fullscreen and mouse confinement, without repeatedly changing your desktop resolution.
- **An interface you can read:** Menus, HUD elements, the cursor and loading bar scale with your resolution.
- **A clearer view:** Correct aspect ratios, a larger minimap with an improved frame, and smoother fullscreen videos.
- **Weather restored:** Rain, snow and sandstorms render in the 32-bit graphics path.
- **Better solo play:** AI takes over the other active human starting positions when you launch a multiplayer map in single-player mode.

## Project status

**In active development.** OpenVikings Remaster combines modern Windows presentation with improvements to village life, transport, navigation, loading and audio. See the [changelog](CHANGELOG.md) for the latest updates.

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

## Changelog

Read the [dated development updates and full changelog](CHANGELOG.md) for gameplay fixes, graphics and audio improvements, and project maintenance.

## Credits

Cultures was created by **Funatics Software**. OpenVikings is a community project built out of appreciation for these games.

Related projects include the [Open Northland](https://github.com/s-nems/open-northland) and [Cultures Saga Patches](https://github.com/Basssiiie/cultures-saga-patches), whose community fixes help inform our bug investigations.

## License

The source code is licensed under the [GNU General Public License v3.0](LICENSE). This license does not cover the original game's proprietary assets.

## Disclaimer

OpenVikings is an independent, unofficial fan project developed through reverse engineering for education, research and personal enjoyment. It is not affiliated with, endorsed by or supported by Funatics Software or Daedalic Entertainment.

The original game, its trademarks and its copyrighted content remain the property of their respective rights holders. The original game's data files are not distributed with this engine replacement. You must provide your own legally obtained game data; this project grants no rights to those assets and does not endorse copyright infringement.

Rights holders with concerns about this project are welcome to [contact the maintainer](https://github.com/Ravo92).
