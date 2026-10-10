# Original Windows asset paths - 2026-10-10

## Missing music report and evidence

The user reports no music after the source-address completion checkpoint. Startup passed an absolute `datax\dm2` directory to the original DirectMusic driver. The configured debugger working directory is `E:\Steam\steamapps\common\Cultures 8th Wonder`; it contains `gedx8musicdrv.dll`, the `data\DM2` directory and the expected SGT music assets, but no `datax\DM2` directory. Installed options select music mode 2 and DirectMusic volume 70.

The disassembly's path comments and the reference PE initially disagreed. A full byte comparison now establishes why: `D:\Projekte\C++\cultures\OpenVikings_ASM\Game.exe` and the installed original `E:\Steam\steamapps\common\Cultures 8th Wonder\Game.exe` have identical length (1,146,880 bytes) and differ at exactly eight bytes. Every difference changes an original backslash (`0x5C`) into `x` (`0x78`) inside an asset path. All other bytes, including the corresponding startup and playback code, match. The reference copy's modified path bytes must not override the original Windows installation for these strings. Neither executable was modified during this correction; no Mac behavior or path was used.

| String address | Original Windows string | Changed reference byte, file offset |
| --- | --- | --- |
| `004FE0A4` | `\data\\dm2\` | `000FE0A9` |
| `004FE100` | `data\\mouse\mouseright.cur` | `000FE104` |
| `004FE11C` | `data\\mouse\mousepressed.cur` | `000FE120` |
| `004FE13C` | `data\\mouse\mousenormal.cur` | `000FE140` |
| `004FE33C` | `data\\pictures\post00.bmp` | `000FE340` |
| `004FE3AC` | `data\\pictures\pre00.bmp` | `000FE3B0` |
| `004FE3E8` | `data\\FMV\%%s\%s.mpg` | `000FE3EC` |
| `004FE410` | `data\\pictures\%s%2.2d.bmp` | `000FE414` |

The table shows actual string characters, not C++ escape notation. The doubled directory separator is retained exactly in the corrected literals. Music startup `00401509..0040152F` appends `004FE0A4` to the current directory and passes it to `NDMManager::Init` (`004035B6`). The original cached-object driver call subsequently consumes that physical directory; it does not use the game's archive lookup to redirect a nonexistent directory.

## Source correction and boundaries

`MasterControlProgram.cpp` now restores the original music and three mouse-cursor paths. `MasterControlProgram_PS_TitleScreen.cpp` restores the native film pattern, picture-sequence pattern and pre00 existence probe. `MasterControlProgram_PS_PreIngameFmv.cpp` restores the same native film pattern. These changes also address the same established asset-resolution problem in the previously completed video block.

The library pattern at `004FE1CC`, `datax\libs\data%4.4d.lib`, is identical in both EXEs and remains unchanged. The reference's altered eight strings are not a basis for rewriting other occurrences of `datax`. The currently requested missing-post-picture exit behavior is retained; no absent media is synthesized. This installation has no loose `data\FMV` directory, so film asset availability remains a separate runtime boundary.

The earlier blanket statement in [SOURCE_AUDIT_2026_09_30.md](SOURCE_AUDIT_2026_09_30.md) that all `datax` startup paths were original is withdrawn for these eight strings. That older inspection used only the modified reference copy.

All three changed translation units passed MSVC Win32 `/Zs`; whitespace checks passed. No build, tests or game execution occurred. The corrected path removes a confirmed music-resource lookup blocker; actual driver initialization and playback still require runtime confirmation. Existing driver errors are already emitted as `DMManager reports error: ...` through `OutputDebugStringA`. Coverage remains **4161 / 5062 (82.20%)**; **138 / 138** audio/music/video addresses are mapped, which is source coverage rather than verified playback.
