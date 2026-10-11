# Shared helpers checkpoint - 2026-10-11

Ten original entries are reconstructed from Windows assembly and Game.exe. Mac headers supply the eight file/GUI names only. This is a partial shared-helper block, not completion of the remaining category.

| Windows entry | Function | Established behavior |
|---|---|---|
| 00406208 | CFile::ReadFlag | Read exactly one byte through CFile::Read. |
| 0040625C | CFile::WriteTrueFlag | Write exactly one byte with value 1. |
| 00406271 | CFile::WriteFalseFlag | Write exactly one byte with value 0. |
| 004E1E09 | StringTool_GetStringPtr | Delegate array/string IDs to the existing string manager. |
| 004E1E6D | StringTool_GetGameGuiHumanSelectedWindowStringPtr | String table 7. |
| 004E1E7B | StringTool_GetGameGuiHouseSelectedWindowStringPtr | String table 8. |
| 004E1E89 | StringTool_GetGameGuiVehicleSelectedWindowStringPtr | String table 9. |
| 004E1EB3 | StringTool_GetWonders001StringPtr | String table 12. |
| 004E5DDD | Runtime_AllocateWithNewHandler(bytes) | Delegate to the existing two-argument allocator with handler argument 1. |
| 004E5DF0 | Runtime_FillMemory | Return the original destination; fill the low byte, initial alignment bytes, DWORDs and final bytes. A zero count performs no access. |

The ReadFlag destination remains a fixed-width byte: the native entry does not normalize noncanonical flag bytes. XStringArray load/save now calls these original entries. Human/house/vehicle GUI helpers call the recovered wrappers; existing missing-string formatting and buffer ownership remain in the manager.

The native continent link-matrix allocation at 00451A5E calls 004E5DDD. Its former Runtime_Allocate call followed the configurable handler mode. It now always enables the handler as the Windows call requires, and its following native zero-fill uses Runtime_FillMemory. Allocation/free ownership is unchanged.

All ten entries previously belonged to the provisional system/shared inventory. Five GUI-owned entries move to GUI after reconstruction; this changes category denominators transparently and removes no global entries. Global coverage is 4558/5062 (90.04%), 504 open. System/shared is 547/691 (79.16%), 144 open; GUI is 1180/1180. Engine retains 128 open and computer AI 232 open. The 19 explicit unfinished boundaries and 289 broad-region assignments remain unresolved.

Eight affected translation units pass MSVC Win32 /Zs: XFile, XStringArray, GuiToolsBase, XCrtFileAccess, C2GameMapContinents, InGameGuiHouseWindow, InGameGuiSingleVehicleWindow and InGameGuiSelectionWindowAccess. No build, tests or game run occurred. Source coverage does not establish live functional equivalence. No production files were added; project/filter membership remains unchanged.

Continue the remaining shared/system helpers next. Candidate native string primitives 004E5C20, 004E5C30 and 004E5D10 require full Windows branch/call analysis before replacement. Existing logarithm/sine/cosine adapters remain unfinished where CRT error/status behavior is unresolved; do not count them complete.
