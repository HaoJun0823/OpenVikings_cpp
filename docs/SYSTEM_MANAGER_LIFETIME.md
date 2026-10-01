# System manager lifetime

Reviewed on 2026-10-01 against Windows x86 assembly and current C++ sources.

## Reconstructed entries

| Windows entry | Current implementation |
| --- | --- |
| `004069F3` | `CXBSystemManager::CXBSystemManager` |
| `00406AA2` | `SystemManagerNewHandler` |
| `00406AB5` | `CXBSystemManager::~CXBSystemManager` |
| `004E7A0A` | `Runtime_SetNewHandlerMode` |
| `004E7A27` | `Runtime_SetNewHandler` |

The first three entries form the system-manager block. The two runtime setters complete its directly required handler-registration path. Existing file-system initialization, warning formatting, palette destruction and converter methods are reused. No guessed helpers or tests were added.

## Confirmed Windows behavior

Construction returns immediately if a singleton already exists. Otherwise it publishes `this` before any resource allocation, installs the warning-only allocation handler, saves the previous handler, enables handler mode and clears the complete file-system static region through `CFile::FileSystem_Initialize` (`004063E4`).

It then allocates and constructs the palette (`0x40C`), 16-bit converter (`0x3C`) and 32-bit converter (`0x28`), in that order. Each allocation is conditional construction. The final call sets masks `0xFF0000`, `0xFF00`, `0xFF` without checking whether the 32-bit converter exists. A failed palette or 16-bit-converter allocation does not stop later allocations.

The allocation thunk `004E5DDD` calls `Runtime_AllocateWithNewHandler(size, 1)` (`004E860E`). It always requests handler processing, including allocation of the one-byte system-manager object before its constructor enables the global mode. It is distinct from mode-dependent `Runtime_Allocate` (`004E85FC`). System-manager object allocation, its three resources and the converter shadow table use the established explicit-handler interface. Deallocation thunk `004E5E48` forwards to `Runtime_Free` (`004E8513`).

The new handler reports exactly `new_handler called - out of memory - %d bytes missing`, preserving the signed interpretation of the 32-bit size, then returns zero. Warning callback/debugger output is the existing `ReportWarning` path; there is no retry request or exception added here.

Destruction frees the 32-bit converter directly, destroys and frees the 16-bit converter, then performs virtual palette destruction/deallocation (original slot `+0x0C`, flag 1). Each global is cleared after its associated cleanup returns. The previous new handler is restored before the singleton is cleared. Destruction does not reset handler mode, the saved-handler slot, diagnostic callbacks or file-system state. The caller frees the system-manager object.

`Runtime_SetNewHandlerMode` accepts only 0 and 1, returns the previous mode on success and returns -1 without modifying state otherwise. `Runtime_SetNewHandler` swaps the callback under runtime lock 9 and returns the previous callback.

The MasterControlProgram caller now uses the named destructor followed by runtime free. CApplication's existing conditional ownership uses matching class allocation/deallocation. Its destructor clears warning and fatal callbacks, in that order, before optional system-manager destruction; the original assert callback remains installed. The earlier warning/assert clearing was a reconstruction error.

## Complete system-manager static region

| Windows address | Storage / role |
| --- | --- |
| `005106D8` | `CXBSystemManager::sTheObjectPtr` |
| `005106DC` | `CXBSystemManager::sPalettePtr` |
| `005106E0` | Existing `g16BitPixelConverter` |
| `005106E4` | Existing `g32BitPixelConverter` |
| `005106E8` | `sFatalErrorFunctionPtr` |
| `005106EC` | `sWarningErrorFunctionPtr` |
| `005106F0` | `sAssertFunctionPtr` |
| `005106F4` | `gPreviousNewHandler`, retained after teardown |

The Mac reference confirms the class, singleton, palette and diagnostic-callback identities. Its `sHighColorCreatorPtr` / `CHighColorCreator` and `sTrueColorCreatorPtr` / `CTrueColorCreator` correspond to the existing converter interfaces; those stable repository names are retained. The previous-handler name is inferred from Windows read/write semantics, not an original Mac symbol. Runtime slots `005699C8` and `00569CCC` remain the existing handler and mode globals; no duplicated storage was introduced.

## Directly related converter corrections

`0040FE27` initializes only the initialized byte and two owned table pointers. `0041022C` initializes only the initialized byte. Constructors now leave the other fields untouched until masks are configured, as Windows does.

`0040FE34` conditionally frees both converter tables through the runtime, clearing each pointer afterward. `0040FE5D` frees old tables, allocates the `0x20000`-byte shadow table through `004E5DDD`, and fills all 65536 entries without a null check. Host `new[]`/`delete[]` and the added allocation-failure return were reconstruction differences and have been removed. Existing palette construction clears all `0x408` bytes after its vtable, consistent with `0040F95E`; its native virtual destructor and inherited runtime free are reused.

## Failure observations and boundaries

README OA043 records unchecked converter/table allocations. OA044 records singleton/global teardown and retained handler/callback state. FP016 indexes allocation-handler registration, warning and teardown handling. These are preserved original observations; practical invalid-state or reentrant reachability remains unresolved.

The new-handler thunk distinction also warrants a later review of other existing object/buffer allocation interfaces currently using mode-dependent `Runtime_Allocate`. Their allocator-family matching alone does not establish identical handler invocation when global mode is zero. This review is separate from the completed system-manager tree.

## Validation

- Nine relevant production units checked with MSVC Win32 C++17 syntax compilation. Eight pass; MasterControlProgram retains two existing initialization dependencies (`00406B33`, `0050F620`).
- System manager, pixel converters, runtime and application generate `/Od` and `/O2` Win32 objects.
- Production inventory: 83/87 syntax-passing units; 109 diagnostics remain in the same four incomplete units.
- `XSystemManager.cpp` is registered in both project files under `Platform\System`; the existing header entry uses the same filter.
- Full linking and game runtime validation remain unverified. No tests/test projects were introduced.

The subsequent [master-control initialization follow-up](MASTER_CONTROL_INITIALIZATION.md) resolves the two MasterControlProgram syntax dependencies listed in this snapshot and corrects the earlier attribution of `0050F620`; it remains an unresolved one-byte clear target. The current production inventory is 85/88 with 107 diagnostics in three incomplete units.
