# Production allowance naming correction — 2026-10-15

During the ongoing 500-access readability continuation, the numeric access `human + 0x195 + good` in the default interaction selector was incorrectly named `HumanOffset::AllowedEquipmentFlagsSigned`. That name already exists for the equipment allowance array at +0x1E0. The in-progress generator produced a duplicate declaration; the user commented out the +0x195 candidate while formatting the repository in commit `4fa0790`. With that duplicate disabled, the changed call resolved to +0x1E0 and read the wrong array.

The access now uses **`AllowedProductionGoodFlagsSigned = 0x195`**, the signed-literal counterpart of the existing `AllowedProductionGoodFlags = 0x195u`. The real equipment constant remains +0x1E0. The temporary commented duplicate was replaced by the correct declaration. No unrelated formatting, imports or removals from the user's commit were reverted.

Windows selector 00435605, specifically **00435D50**, compares a BYTE at `[esi + ebx + 0x195]`, followed by the produced-good check and the ability-to-produce call. This matches the source before the in-progress rename in `8e2bcbf`. The disassembly bytes were read directly from the original Game.exe and agree with the Windows assembly export. The correction restores that original production-array access; it is not a change to original gameplay.

`C2DefaultInteraction.cpp` passes incremental Win32 `/Zs`. Private checks confirm the +0x195 value and original `int` literal type, and that the equipment constant remains +0x1E0. A scan of the active constants in `C2ObjectLayout.h` finds no duplicate names within a namespace. No build, tests or game run was performed. Temporary analysis and logs are kept under ignored `.codex-work/`.
