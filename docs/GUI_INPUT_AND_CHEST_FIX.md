# Cursor text and chest task corrections - 2026-10-10

Windows ASM/Game.exe establish both corrections. Existing Mac declarations provide names only; no behavior is taken from Mac code.

## Undefined construction cursor text

The screenshot shows `<1:22:NOT DEFINED!>` above the selected building name. This is the GUI string manager's native missing-text diagnostic: table 1, ID 22. `InGameGuiMousePointer.cpp` displays the current input-mode name and then the selected house name while mode 12 is active.

Windows entry `004B7E92` and the complete 29-DWORD table at `004B7F23..004B7F96` select string IDs 10..38 for modes 0..28. Every branch calls `004E1E35`, the miscellaneous GUI lookup for **table 2**, not main GUI lookup `004E1E27` for table 1. The C++ mode-plus-ten mapping was correct, but its table was wrong. `StringTool_GetInputModeNamePtr` now uses `StringTool_GetMiscMenuStringPtr` for all 29 modes and retains the native `<UNKNOWN>` fallback.

The installed German `ingameguimisc.cif` contains ID 22, `Wähle Hausbauplatz`; `ingameguimain.cif` skips IDs 17..29. This confirms the diagnostic without hardcoding a replacement caption or changing the language assets. The original function name is already mapped to Mac header declaration line 5964.

## Access violation after approaching a chest

The second screenshot's call stack enters `Human_l_StartTask_GetChild` from task startup and faults in `House_Stock_RequestAmountOfGoods` at house offset `0x14`. The chest approach completion in `Human_l_FinishTask_Goto` (`00445300`) had copied two native hexadecimal task values as decimal values:

| Native write | Meaning | Previous C++ value | Correct value |
| --- | --- | --- | --- |
| `004453A1`: task `0x15` for target type 6 | Enter vehicle / `Human_l_StartTask_MoveInsideVehicle` | 15 (pursuit) | 21 |
| `004453BB`: task `0x12` for landscape target with wanted good 64 | Open chest / `Human_l_StartTask_OpenChest` | 12 (get girl) | 18 |

The corrected arrival branches now reach the existing native task trees. Their shared status reset to zero and other cargo/equipment branches retain the Windows behavior. A human without a home wrongly entering the child task explains the invalid house access; the screenshot does not independently expose the home ID. No guessed null guard or change to the house-stock routine was introduced. The text-table error and task-ID error are distinct causes.

## Verification

Six affected/integration units passed Win32 MSVC `/Zs`: `InGameGuiInputAccess.cpp`, `C2HumanCommandAccess.cpp`, `C2HumanTaskAccess.cpp`, `C2HumanChest.cpp`, `C2HumanVehicleJobs.cpp`, and `C2HumanTransportJobs.cpp`. Windows assembly, the original PE jump table, current callers, dispatchers and installed CIF strings were inspected. No build, tests or game run occurred; actual chest opening and cursor captions still require runtime confirmation. The function inventory remains **4161 / 5062 (82.20%)** because these correct existing bodies. User-owned untracked CombatTasks remain untouched.
