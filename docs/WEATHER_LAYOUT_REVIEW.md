# Weather sectors and screen overlays

The Windows weather manager and the three separate rain/snow/sand rendering paths now use named fields, density units, sector dimensions and owned color tables. The original routines remain separate. This completes the readability review of this block, not the entire engine or a proof of all runtime behavior.

## Confirmed from Windows assembly and Game.exe

- Manager `00484BD1`: twelve bytes, signed sector width at 0, signed height at 4, owned sector array at 8. Creation `00484C4F` computes `(map dimension + 9) / 10`. Freeing `00484CA5` releases the array, clears its pointer, then height and width. Allocation families and callback order are retained.
- Each twelve-byte sector contains rain/snow/sand densities at 0/4/8. The coordinates use ten map points per sector and a five-point center offset. The three density readers validate the packed map point, clamp sector coordinates and interpolate four neighboring sectors. Thirty-two-bit intermediate wrap, signed divisions and clamp to 0..10000 are unchanged. Windows ignores the `ignoreDark` and `playerId` parameters in these readers; no behavior was added for them.
- Rectangle setter `0048520F` sorts the coordinate bounds, clamps density and writes an inclusive sector rectangle. Weather types 1/2/3 mean rain/snow/sand. Invalid types retain their original no-write behavior and zero-dimension edge behavior remains unchanged.
- External setter `004809AB` multiplies the fourth argument by 100 before passing it as density to the rectangle setter. Its misleading `duration` parameter is now `densityPercent`. This is a name change, with the original wrapping multiplication preserved. The map INI commands directly supply internal density units.
- Load/save `00484CC3` / `00484D34` serialize only the first eight manager bytes, followed by width*height*12 sector bytes. The pointer is never serialized. Version handling and clear/read ordering are retained.
- Display weather state contains rain flag/table at 928/92C, snow flag/table at 930/934 and sand flag/table at 938/93C. Construction `0047C685` enables the flags. Exit `0047D335` releases and clears each table in that order. Each lazily allocated table contains 65536 sixteen-bit entries (131072 bytes).
- Draw routines `0047D37B`, `0047D64A`, `0047D918` sample weather at the display center. World-pixel origin is at 954/958, buffer extent at 964/968. Their bitmap accesses use width/height, row pixel count, depth and sixteen-bit memory fields already present in CBitmap. The original sixteen-bit-only path remains unchanged.
- Pixel masks at C/E/10 and adjustment fields at 2C/30/34 are confirmed by the overlay native loads. Windows uses the adjustments as masked x86 shift counts here, rather than the converter's packing right shifts. Names reflect the existing converter layout. Original random seeds, random call order, time scaling, floating multiplication order, streak/flake/sand pixel writes and table color formulas are preserved.

## Names and limitations

The Mac reference supplies corresponding manager and draw routine names only. Offsets, constants, ordering and parameter meaning above come from Windows evidence. The two small weather structs document the layout; production allocation and mechanical access remain unchanged. Existing bitmap and pixel-converter field names are supported by their C++ layouts and the Windows overlay uses; this block does not claim a complete review of those classes or the whole display object.

## Validation

Private staged-source comparison checks the exact committed baseline and final files. Win32 compiler assertions check literal values and types and weather member offsets. Native instruction bytes are verified against Game.exe. Syntax-only `/Zs` checks cover the affected units and representative bitmap/pixel-converter consumers. No build, tests or game execution were requested or performed.

The reference inventory is `WEATHER_LAYOUT_FIELDS.csv`; private scripts and extraction artifacts remain in the ignored `.codex-work/` directory. The next block is display viewport, scrolling and bitmap buffer state.
