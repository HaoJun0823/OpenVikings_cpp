# Input manager selection storage

The input manager's selection prefix now has named fields and an explicit 2740-byte layout record. Two input/selection units use the shared names for 210 scalar and base accesses; the final inventory includes 229 references including reset lists and group-offset choices. The rest of the manager's cursor/highlighting storage remains a separate review target.

## Confirmed from Windows assembly and Game.exe

Input mode is at 0 and selection type at 4. A single-human ID/unique-ID pair is at 8/C, followed by the human group count at 10 and 300 interleaved eight-byte ID/unique-ID rows at 14. The next fields are single animal at 974/978, single vehicle at 97C/980, vehicle group count at 984 and thirty interleaved rows at 988. The vehicle group ends at A78, where the selected house ID/unique-ID pair begins.

House-type selection uses four DWORDs at A80/A84/A88/A8C: type, development stage, special item type and special item. All four still participate in selection identity. The selected guide pair is at A90/A94. Landscape selection stores type at A98, variant at A9C, packed map point at AA0 and player ID at AA4. Road/wall build points remain at AA8/AAC and the selection-change count at AB0. The complete prefix ends at AB4; it does not include the rest of the input object.

The native selection functions `004B6115`..`004B6EAF` and getter/notification functions `004B7303`..`004B767D` establish these offsets and group capacities. Single-to-group promotion retains the previous selection as the first row, then adds the new row. Duplicate and exact-capacity checks remain in their original order. Vehicle grouping still requires both relevant vehicles to have native type 5. Original group validation, compaction, callback order and fallback to a single selected actor are unchanged.

Unselecting keeps the original payload retention and exact sentinel/zero field lists. Getters for an inactive landscape/road/wall selection still write the original (-1,-1) packed point where Windows does so. The selection-change count and GUI update ordering are unchanged. No broader actor validation, new bounds checking or eager payload clearing was added.

## Naming and representation

The record types document the Windows layout while production reads/writes keep their existing widths and raw storage. Each group is interleaved, not a separate ID array and unique-ID array; the column-offset names describe the existing strided accesses. The Mac reference is a routine-name reference only. Names and layout above are supported by Windows setters, getters, call sites and native bytes.

`INPUT_SELECTION_FIELDS.csv` records final references. Helper call operands and other input-mode command constants are not claimed as fully reviewed by this block. The whole source review remains incomplete.

## Validation and next block

Exact baseline/stage/source checks passed for five files. All 55 replacement literal/value/type pairs and physical/overlapping member offsets passed Win32 compiler assertions. Both affected units plus lifecycle and world-display consumers passed syntax-only `/Zs`; 1341 native instruction byte sequences match Game.exe. No build, tests or game execution were performed.

The next related blocks are display draw counters and the remaining cursor/highlighting state and command helper operands.
