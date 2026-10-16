# Build and game assets

Reviewed October 16, 2026 from `OpenVikings.vcxproj` and current path consumers. No build or playback run was performed for this documentation review.

## Build

Open [OpenVikings.slnx](../OpenVikings.slnx) with Visual Studio C++ tools, **MSVC v145**, targeting **x86/Win32** and **C++17**. Select Debug or Release. Outputs are `OpenVikings/<Configuration>/`, PDBs are beneath `symbols/`, and intermediates beneath `obj/`.

Production sources and headers are in [`src/`](../src/). The project and its semantic Visual Studio filters reference those paths. The resource script, icon and legacy import definition remain in the repository root; the resource script includes `src/resource.h`.

Release uses `/O2`, `/Gy`, `/MD`, `/GS`, `/fp:precise`, `/Zi`, `/DEBUG:FULL`, `/OPT:REF`, `/OPT:NOICF` and nonincremental linking. Identical folding stays disabled for function-pointer identity. `XCrtMath.cpp` retains its IA32/strict floating-point overrides; `XCrtCompiler.cpp` has special raw-entry code-generation settings. Do not generalize those files' ABI to ordinary C++ calls.

The project links Windows/DirectX imports and generates the legacy DirectInput import library from [LegacyDirectInput.def](../LegacyDirectInput.def) before linking. Do not add library pragmas or redirect the legacy ABI merely to suppress an import failure. The main target is not x64.

Runtime also depends on the [verified legacy compiler/math backends](RUNTIME_COMPATIBILITY.md). A historical successful build does not certify the current working tree or backend availability on another installation.

The `remaster` frontend targets Windows 10 version 1703 or later with Per Monitor V2 awareness and links `d3d11.lib`, `dxgi.lib` and `d3dcompiler.lib` for borderless flip-model presentation. `ddraw.lib` remains solely for the isolated legacy FMV decoder. Output and intermediate directories are unchanged. See [remaster presentation](REMASTER_WINDOW_PRESENTATION.md).

## Original data and current DataX adaptation

Run from a game-data root containing the original assets and required Windows media components. The current source intentionally uses this user-requested loose-file layout:

| Consumer | Current physical path |
| --- | --- |
| DirectMusic | `datax\dm2\` beneath the working directory |
| Cursor files | `datax\mouse\mousenormal.cur`, `mousepressed.cur`, `mouseright.cur` |
| Pre/post picture sequence | `datax\pictures\%s%2.2d.bmp`; pre-picture probe `pre00.bmp` |
| Title/mission films | `datax\FMV\%%s\%s.mpg`, followed by native language substitution |
| Library mounting | `datax\libs\data%4.4d.lib` |

Archive-internal paths such as `data\gui\...` remain virtual `data` paths. A physical DataX folder does not justify rewriting archive members. The original music driver consumes a real directory; archive lookup does not redirect it.

The path review established that the assembly-reference Game.exe differs from the installed original at exactly eight path bytes (backslash changed to `x`). They occur in strings at `004FE0A4`, `004FE100`, `004FE11C`, `004FE13C`, `004FE33C`, `004FE3AC`, `004FE3E8` and `004FE410`. All other bytes matched in that comparison. Therefore the original `data` paths and the later user-authorized DataX adaptation must be distinguished. The reference executable's patched strings do not establish original behavior.

Music uses the original external `gedx8musicdrv.dll` with SGT/DLS assets. FMV uses Windows COM multimedia streams and available codecs; on `remaster` their offscreen samples are presented through D3D11 instead of a DirectDraw window surface. Those external decoder implementations were not reconstructed. Asset presence alone does not establish successful playback. Inspect actual runtime diagnostics before changing routing or substituting media behavior.

Source consumers: [startup/music/cursors](../src/MasterControlProgram.cpp), [title/pictures](../src/MasterControlProgram_PS_TitleScreen.cpp), [pre-game FMV](../src/MasterControlProgram_PS_PreIngameFmv.cpp).
