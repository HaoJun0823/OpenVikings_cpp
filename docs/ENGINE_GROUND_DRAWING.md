# Static ground drawing reconstruction

Snapshot: 2026-10-04. Windows ASM/PE and source review only; no compilation, tests or game execution.

## Connected drawing path

The former `00481AE2` no-op now follows all four static-buffer splitting stages, preserving world-boundary splits, buffer wrapping, sub-bitmap lifetimes and the original outside-world test. `00491C12` converts the world origin to macro-grid coordinates and draws the requested field into the static landscape bitmap. Each quadrangle produces two height-adjusted textured triangles, with exploration/highlight brightness and optional vertex colors. Transition layers retain descending layer order. Road and house overlays retain both original passes, six micro-grid samples and four sub-triangles per macro triangle.

Production implementation: `C2EngineGroundDrawing.cpp`. Display's existing scrolling/copying path calls the same fastcall adapter with the original 24-byte rectangle. The downstream landscape/creature call is retained with its original flags; its existing `004911D5` dummy remains an explicit separate dependency. Ground drawing itself no longer depends on that body producing pixels.

## Required data initialization

`00482241` now publishes the engine-map owner, clears its complete 28-byte static storage, allocates twelve-byte macro and four-byte micro records, initializes micro IDs to `FFFF`, calculates macro brightness and sets both transition layers to `FF`. Height and transition callbacks register in original order at priority ten. Dirty bounds are accumulated in micro coordinates, expanded by three macro points and cut inside the map before recalculation. The existing destructor unregisters these same callbacks and frees these same runtime-owned buffers.

The real transition manager loads `pointtype` and `transition` groups from `data\engine2d\inis\patterntransitions\transitions.ini`. Point records contain five pattern-group IDs; transitions contain base/alpha texture IDs and six coordinate sets for each orientation. Both token tables and all four special overlay IDs are recovered. The complete `005689F0..00568A0F` static region has typed storage; the existing shutdown byte-array view aliases that same storage rather than duplicating ownership.

The two around-way routines keep distinct Windows entry points while using a shared source-level implementation of their matching geometric branches. Logic descriptions and engine pattern descriptions have distinct getters, including the engine's zero-logic-type rejection. Transition assignment retains the original group matching, shape lookup, encoded-byte comparison and map-modified notification.

## Shader

`NToolGouraudShader::CGouraudShader` preserves the 0x94-byte Windows layout and the complete shared scanline region `00555118..00568357`: 1,400 rows of 0x38 bytes. Setup keeps stable vertex sorting, signed 16.16 edge interpolation, truncating integer division, two edge phases and clipping adjustments. Thirty-two-bit wrap arithmetic is explicit.

Both scalar destination depths, uniform and vertex-color shading, and both alpha variants have production bodies. CPU selection still uses the existing `CPU_HasMMX`. The MMX paths are expressed as scalar C++ lane operations preserving low-word multiplication, signed saturated light increments, byte packing, minimum brightness five when any vertex is nonzero, DWORD texture indexing and RGB555/RGB565 masks. MMX alpha uses `256 - alpha`; scalar alpha uses `255 - alpha`. The vertex-alpha high-color path retains its original three-bit expansion for every channel, including RGB565 green. No Mac shader arithmetic or class layout is substituted.

The constructor is called through placement construction on the existing reconstructed-runtime allocation; display teardown retains its existing runtime free. No host/runtime allocation families are mixed.

## Address groups

| Windows entries / region | Meaning |
| --- | --- |
| `00481AE2`, `00481BAF`, `00481C10`, `00481CDD`, `00481E20` | Static-buffer stages and world rejection |
| `00491C12`, `00491DE2`, `00491EA8`, `00492F26` | Ground field, quadrangle and overlays |
| `0047CCDD`, `0047CDA2`, `0049341C`, `00493447` | Brightness and Windows height/color adapters |
| `00482241`, `00482614`, `0048274A`, `004829DA`, `00482C00`, `00482D30` | Engine-map startup, height updates and transition assignment/checking |
| `00483007`, `0048320E`, `00483243`, `0048327A` | Engine pattern descriptions around ways/points |
| `00424108`, `0042430F`, `00424339`, `00424364`, `00424862` | Logic triangle descriptions and exploration lookup |
| `00411C48`, `00411C84` | Micro/macro map-modified notifications |
| `0048D714`, `0048DB87`, `0048DB88`, `0048DBFB`, `0048DC39` | Transition loading, empty exit callbacks and ID lookup |
| `00481E51`, `0049A12F..0049B089`, `0049EC09`, `0049F5D5..0049FBDB` | Shader construction, setup, scalar and MMX paths; individual addresses remain in source |
| `0042A632`, `0042A9B0`, `0042AA54`, `0042AFCB`, `0042B128..0042B2FD` | Grid offsets, conversion, wrapped/unwrapped movement and validation |
| `004F49D0..004F4A1F`, `004F4628..004F465F`, `004F4B70..004F4BA7` | Complete token tables and two distinct fourteen-entry shape tables |

Mac reference confirms class/function names and parameter identity. Windows-only height/color and depth/MMX adapters use semantic names describing their verified Windows role; the Mac build inlines or combines those routines. Shared scanline capacity follows the complete Windows static-region boundary.

## Verification and remaining work

Static review checks both token tables and both shape tables against `Game.exe`, texture/alpha/coordinate layout offsets, shader layout and declaration/body correspondence, callback/overlay order, runtime ownership, unique source address mappings and the absence of new raw `FUN_` calls. Project and filters contain the same 161 cpp files and 155 headers. Five dummies and their CSV rows are replaced, leaving 113 unfinished entries and eight unfinished startup constructors. The source-address audit reports 2,262/5,057 mappings (44.73%), 2,795 unmapped candidates and 2,623 parsed definitions; the block adds 67 distinct mappings.

README OA118..OA120 and FP112..FP115 index original unchecked/error paths. Builds and runtime rendering remain unverified. Next related block: `004911D5` landscape/creature drawing and its static-house/dynamic-object dependencies; remaining default-handler commands, engine IO and overlay updates also remain separately tracked. This block does not claim complete terrain animation, entities, gameplay or savegame restoration.


## Overview pattern-query crash correction - 2026-10-04

The map-loading screenshot reports an access violation inside OverviewDisplay_Read, with a debug-fill address. Source inspection identifies a concrete uninitialized-output path: the global link-only 00481188 dummy did nothing while UpdateArea consumed its six stack pointers. The 0048114E dummy also returned null for every texture-scan query.

Both entry points now live in NC2E2 in C2EngineMapTerrainAccess.cpp and are declared in its shared header. Windows 0048114E calls IsBorderPoint, returns null for borders, otherwise reads engine macro WORD +0 for triangle A or +2 for B and adds index * 0x70 to static pattern storage 00568A34. Windows 00481188 forwards to 0048327A, already reconstructed as EngineMap_Misc_GetTrianglePatternDescriptions_AroundPoint: six null outputs, followed by short-circuit direction 1/3/5 pair queries. The existing overview null-to-black sample handling is retained. Mac identifies the original entry-point names.

All four overview call sites use the shared declarations; both global dummies and their inventory rows are removed (118 unfinished entries remain). No project files were added. Windows ASM, declaration/call integration and static-pattern stride were checked; no build, tests or runtime run was performed. The screenshot does not include a call stack, so the exact failing caller remains unconfirmed. README OA133 and FP142 index the original border/partial-result handling.
