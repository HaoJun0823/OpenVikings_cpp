# Character selection window records review — 2026-10-14

Baseline `30d3e9f`. Windows assembly and `Game.exe` establish all offsets, widths, strides, capacities, allocations and parameter stores. Mac declarations are naming references only. Existing working C++ branches and calls are retained; this is a naming/layout review, not a complete gameplay equivalence claim.

The existing 15,736-byte physical window view is checked against its fields: 112-byte base; six enable/collapse bytes each; checksum; six rectangles; computed size; six height overrides; layout flag and three unresolved bytes; eleven child-control pointers; current experience page and eleven fourteen-DWORD job pages; 12,004-byte tooltip owner; 2,804-byte dynamic button owner; owned trader popup reference. The 44-byte clear ends after the trader-config button, before the experience page. No live objects are reinterpreted as replacement classes.

Named access domains:

- Graphic-button command data at 0x48/0x4C, selected byte at 0x36, rectangular style at 0x35, immediate-tooltip flag at 0x44 and disabled byte at 0x50. Text-button disabled is a different byte at 0x56; caption is its owned pointer at 0x30. Text-element small-font byte is 0xFB. Constructor provenance identifies each local's class.
- Two compact trader route rows each hold select/assign/action/label pointers with sixteen-byte row stride. Merchant house IDs refer to validated route records. Route buttons carry house ID and revision in their graphic-button parameters.
- Experience amounts and displayed job IDs each use four-byte fields but now have distinct semantic stride names. Eleven pages contain fourteen job IDs each; the page stride is 56 bytes. Sorting, exclusions, zero entries and name tie-breaking remain unchanged.
- Twenty-byte user-network commands retain byte opcode and DWORD object/parameter fields. Family selection retains spouse/mother/father/child order; home/work selection uses their distinct house IDs. The manager reference at 4 is the world display. Keyboard modifiers remain at 0; mouse position retains separate X/Y DWORDs.
- Static engine landscapes retain 140-byte stride and logical type at 0x38. Palette accesses identify the last entry, color-array stride and the entry used for missing production experience. The latter names its display role, not a guessed RGB color.

Tooltip limits deliberately distinguish 104 stored text bytes, 100 copied bytes with terminator at 100, and a separate 100-byte formatting buffer with terminator at 99. Native backwards lookup gives later rectangles precedence and retains inclusive right/bottom endpoints with DWORD wrap. Dynamic button updates keep an existing control when its coordinates match, even if other arguments differ. Removal clears only the active byte; bulk clear retains the owning window. Graphic/text command parameters stay distinct. No cleanup or bounds checks were invented.

The window's layout checksum, collapsed-tab persistence, preview-follow updates, owned popup shutdown, borrowed child references, call order, frame refresh, production wrapping and response sounds are unchanged. The descriptor's final zero DWORD and the window's unresolved bytes remain unresolved. Existing drawing helpers are not newly merged.

Evidence: `004AB7F1..004AEF69` (window construction/destruction/commands/drawing/layout/trader/experiences; includes the complete final three-byte return), `004B9D0C..004BA041` (tooltip and dynamic controls), `004BE4FB..004BE60F` (landscape tooltip), `004CBF94..004CC044` (fill/centered print). Exports are checked byte-for-byte against `Game.exe`; **5,142 instruction instances** provide provenance, not proof that every instruction in the export was separately reviewed.

Frozen stage comparisons retain identical C++ operation tokens after resolving constants and formatting. **60 value/type comparisons, 44 physical assertions and 87 named layout references** pass. The production unit and private assertions pass Win32 `/Zs` without diagnostics. This file's **52 raw numeric accesses become zero**, reducing the repository scan from 3,313 to **3,261**. Builds, test projects and runtime playthroughs were not performed.
