# Overview bitmap input — 2026-10-01

## Reconstructed entries

The overview virtual message slot now reaches `WorldOverviewElement_MessageHandle` (`004CF81A`) through a native ECX/stack adapter. Related entries include `l_BE_GlobalToLocalCoordinates` (`00478A79`), `SMapMigPoint::GetLong` (`0042ADFF`), vehicle-selection getters (`004B74CB/004B74DC/004B74EC`) and `CUserNetworkCommandManager::AddVehicleCommand` (`00410EB8`). Windows assembly determines their behavior. Mac declarations establish original class/method names and parameter identities only.

| Message | Windows action |
| --- | --- |
| `1` | Convert global mouse coordinates to a clamped map point and send desktop message `0x1000/2004` |
| `0x20` | Convert/clamp first, then send selected humans or issue vehicle command `0x65` when input mode is zero |
| `0x80` | Perform the same desktop-position action only while the left mouse button is down |
| `0x100` | When enabled, call zoom modification with signed wheel direction -1/0/+1 |
| `0x800` | For parameter 12000, update the complete bitmap area |

All five recognized messages return true even when their action is skipped. Unknown messages return false. Wheel zero still invokes zoom modification and its original center transform. No extra selection, mouse or map validity guards are added.

Global-to-local conversion subtracts one immediate parent's position, then the element position, retaining wrapped DWORD arithmetic and the original read/write order. Overview transforms narrow to signed WORDs and the established map clamp runs before input-mode checks. `GetLong` shifts sign-extended Y and ORs sign-extended X; it deliberately retains the original negative-X behavior rather than substituting a normalized packed representation.

Selection modes 1/2 call the original selected-human dispatch. Mode 4 constructs one zeroed `0x14`-byte vehicle command; mode 5 constructs one per selected vehicle and re-reads the count after each command. Each packet sets byte zero to `0x65`, the object ID at +4 and the map position at +0x0C. The vehicle-command helper accepts types `0x64..0x73`, indexes the existing `0x54C`-byte vehicle array without range guards, checks allocation, fills the low player byte at +1 and the original record DWORD at +8, then calls the genuine command queue/execute routine with both flags false. Unknown packet fields remain zero/opaque.

Desktop position dispatch reads the manager's desktop and virtual slot +8 before computing the packed argument. The original five stack parameters and receiver are retained; the overview adapter emits `ret 20` in the optimized Win32 listing.

## Singleton ownership boundaries

All memory xrefs were indexed for `00569410` (257) and `00510A20` (88). The former is the input-manager singleton, published by `004B4697` and cleared by `004B484B`. The latter is the user-network-command manager singleton, published by `00410D2A` and cleared by `00410D79`. Mac names match these Windows owning lifecycles and call sequences. The four-byte pointer regions are explicit external bindings, without fabricated owning constructors or duplicate object storage. Neighboring initialization flags and subsystem singletons remain separate unresolved owners.

The existing mouse manager supplies the left-button state at +8. The in-game GUI manager and its desktop are already reconstructed. The selected-human dispatch `004B7B62`, command queue/execute `00410F00` and both singleton owning lifecycles remain genuine original boundaries. Their broader input/network subtrees are not replaced with guessed movement or queue behavior.

## Verification and next block

Seven affected implementations generate Win32 C++17 `/Od` and `/O2` objects without warnings. Static analysis adds no findings in this input implementation. All 84 production units were syntax checked: 80 pass; four incomplete units retain 123 diagnostics after the concurrent DirectMusic integration. Both new source/header files are in `OpenVikings.vcxproj` and its existing GUI filters. XML, file inventory, duplicate entries and filter references are checked. No tests were added. Full linking and game input execution remain unverified.

Confirmed from Windows: message IDs, return behavior, mouse/selection filters, arithmetic, packet writes, receiver and call order. Supported by Mac: method/owner names and parameter identity. Next connected GUI block: static-window destruction/title drawing `004C9777/004C9824` and base-tool construction `004B8D11`, followed by world input `004BA3BE`. Input-manager and network-command owning paths can later close the documented external boundaries.
