# Camera and tooltip movement consumers

Camera following `0047C9DA`, moveable tooltip drawing `0049883D` and moveable debug drawing `004990E4` now use the established actor and movement field names. Fifty-six scalar field accesses plus carrier moveable-ID accesses and coordinate/stride expressions were renamed. No movement algorithm or command behavior was changed.

## Confirmed Windows behavior

Camera following first checks whether a human is inside a vehicle, then separately checks whether the resulting moveable is a vehicle inside another vehicle. The checks remain sequential; no generalized carrier-chain traversal or generation check was added. The native attached-vehicle byte remains the deciding value in that path.

The ordinary camera and tooltip paths interpolate only when walking and not turning. They read the walking direction, step progress and map-position-advanced flag. After the midpoint has moved the logical position, they subtract progress from the full-step value and reverse direction before computing the remaining displacement. The original wrapped products, signed divisions and access/call order remain unchanged.

The debug movement pass intentionally has its own walking check and eight-entry interpolation tables; it does not gain the ordinary tooltip's turning check. Its actor-specific debug branches use vehicle, animal and human layouts separately. The local variable called `object` is named only inside those proven kind-specific scopes.

The signed divisor constant remains 10000, while progress inversion uses unsigned 10000u. `FullStepSigned` documents this distinction alongside the existing unsigned `FullStep`. All original array strides are unchanged. Mac evidence supplies the already established routine names only; carrier/movement fields and control flow come from Windows.

## Validation and next block

Exact baseline/source comparisons passed for three files. All 33 replacement literal/value/type pairs passed compiler assertions. Both production units and the private assertion unit passed Win32 syntax-only `/Zs`; 602 native instruction byte sequences match Game.exe. No build, tests or game execution were performed.

This completes these movement consumers, not every tooltip field or the whole source review. The next related block is the input manager's selection storage and group counters.
