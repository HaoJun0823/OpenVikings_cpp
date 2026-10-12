# Master-control initialization and property ownership

Reviewed on 2026-10-01 against Windows x86 assembly, Mac naming references, current sources and Git history.

## Startup record

`00406B33` is `NC2Logic::SGameControlStartupData::SGameControlStartupData`. Windows clears exactly `0x120` bytes and returns the object. `00406B4A` is the separate `Reset` method and clears the same range. Both identities are confirmed by the matching Mac symbols; initialization behavior and size come from Windows.

`C2ControlStartup.h/.cpp` now hold the named record and its constructor/reset. The opaque bytes preserve unresolved field identities instead of guessing a richer layout. Size is `0x120` and alignment is 4. The embedded MasterControlProgram record begins at `+0x2C`, ends at `+0x14B`, and leaves the hardware-mouse flag at `+0x14C`; the complete owner remains `0x168` bytes.

The member constructor executes before the MasterControlProgram body, matching the original call at `00401011`. The body then publishes its singleton and clears the entire `0x168`-byte owner, preserving the second clear and call order. The global record at `005106F8` uses the same type and canonical storage. Its original initializer callback `00406B1E` still clears all `0x120` bytes. The existing raw-record reset bridge delegates to the named `Reset`, and game teardown uses the global method directly. Existing byte-offset users continue to address the same record.

One previously missing original entry, the record constructor, is added. The already reconstructed reset body has moved to its named class interface; this is not another newly solved function.

## `0050F620` is not the property manager

Subsequent user-authorized cleanup (2026-10-12): this initialization-only byte and its constructor clear are removed. Its original purpose remains unknown; the following text records the prior reconstruction evidence. See [SYMBOL_NAMING_PASS.md](SYMBOL_NAMING_PASS.md).

The Windows export contains only the one-byte clear addressed by `0040102D`. No further reads, writes or matching Mac symbol establish this slot's purpose. It now has explicit one-byte storage under the original `DAT_0050f620` name. It is unresolved and must not receive a guessed semantic name. The following `0050F621..0050F623` bytes have no established accesses; the next known owner is the MasterControlProgram singleton at `0050F624`.

The actual property-manager singleton is `005697EC`, already represented by `CPropertyManager::sTheObjectPtr`. The earlier handoff description of a property-manager region around `0050F620` was an incorrect inference and has been corrected.

## Property-manager allocation and ownership

The existing constructor (`004E1408`), destructor (`004E1436`), add (`004E167D`), lookup (`004E171D`), remove/delete (`004E1765`), string duplication (`00410B59`) and list-specialization teardown (`004E1D00`) were checked along the real initialization/cleanup path. Their Mac class and method identities were retained.

All directly related ownership now uses the original runtime family:

| Allocation | Windows / current behavior |
| --- | --- |
| Manager object, `0x18` bytes | Class allocation invokes `Runtime_AllocateWithNewHandler(size, 1)`; class deletion calls runtime free |
| Property record, `0x10` bytes | Explicit-handler allocation, unchecked complete zeroing, then fields filled in original order |
| Property name and string | Null source returns null; otherwise allocate length + 1 with the handler enabled and copy without checking success |
| Property-list node, `0x08` bytes | Existing canonical `NXBasics::CListBase` allocation/free methods |
| Replacement string | Free old string before updating type/integer fields and duplicating the supplied string |
| Removed record | Remove list node, free name, free string, free record |

Allocation thunk `004E5DDD` always passes handler flag 1 to `004E860E`; free thunk `004E5E48` forwards to `004E8513`. Host `operator new/delete` in the prior manager/string/record paths were reconstruction differences.

The property-list specialization now uses the existing runtime-backed `CListBase` rather than the key-manager copy of the same original list routines. This retains its `0x14`-byte embedded layout and original virtual deletion slots while reusing established Windows node handling, iterator order and tail-removal behavior. Its raw value deletion frees only the record, as original `00479E17` does; ordinary property-manager teardown explicitly frees both strings first. The list is constructed with value ownership disabled. No keyboard-owner changes or duplicate list implementations were introduced.

## Preserved failure paths

- Failed manager allocation is still followed by original singleton calls without an added guard.
- Failed property-record allocation is still followed by unchecked zeroing.
- Failed string allocation is still followed by unchecked copying; the original null-source branch remains distinct. README OB009 already records this defect.
- Failed list-node allocation preserves the existing original list bookkeeping and later unchecked accesses, as reviewed in the shared list implementation.
- Replacing a property frees its old string before duplicating the caller's source. A source alias into the old string is not protected; practical call-site reachability remains unresolved. README OA045 records this original observation.
- Removal and teardown retain original ordering and list null-payload termination. See README OA036 and FP017 for cross-references.

## Validation and remaining work

- Eight related units pass Win32 C++17 syntax compilation, including MasterControlProgram, property manager, startup record, existing list, logic lifetime and runtime-startup callers.
- Six directly checked units generate both `/Od` and `/O2` objects: C2ControlStartup, MasterControlProgram, MasterPropertyManager, C2LogicLifetimeAccess, XList and WinMain.
- Complete production syntax inventory is 85/88 passing units, with 107 diagnostics in three incomplete menu/game-state units. MasterControlProgram's two previous initialization diagnostics are resolved.
- New production source/header entries are registered in the project and filters under `Logic`. Full link/runtime validation remains open; no tests/test projects were added.
- Next connected block: `00401985` / `004019DA`, the MasterControlProgram engine/game startup paths and their CGame/CE2Manager construction dependencies. The startup record's field meanings and `0050F620` identity remain separate unresolved data targets.
