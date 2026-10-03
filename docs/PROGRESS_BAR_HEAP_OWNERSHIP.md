# Progress-bar bitmap heap ownership

Snapshot: 2026-10-03. Windows x86 instructions are authoritative.

The user reports a debugger breakpoint in `ntdll` reached through `HeapFree`, `NXBasics::Runtime_Free`, `CBitmap::operator delete` and `CMasterControlProgram::ProgressBar_Exit`. The screenshot alone does not supply the heap diagnostic text, but inspection establishes an allocation-family mismatch on this exact object path.

`ProgressBar_Init` formerly allocated its 0x54-byte bitmap object with global `::operator new(..., std::nothrow)`. The later `delete CBitmap*` calls the class's deleting function, which uses `Runtime_Free`. That function frees from the project's heap created by `Runtime_HeapInit`; it cannot free a host-CRT allocation. The inherited `CStorable::operator new` does not affect an explicitly qualified global allocation.

Windows `004025A9/004025AB` allocates 0x54 bytes through `004E5DDD`. That thunk calls the runtime allocator with new-handler argument one. `00402620` invokes the bitmap's deleting destructor with flag one and then clears the progress-bar member. The reconstruction now allocates with `Runtime_AllocateWithNewHandler(0x54, 1)`, preserving the null branch, placement construction, temporary bitmap/picture destruction order, display update and existing bitmap deletion.

No original game bug is asserted. This is a reconstruction ownership error. The debugger breakpoint is the point where the invalid free is detected, rather than evidence of an explicit breakpoint in the progress-bar source. No workaround is added to suppress heap checks or ignore the original error.

Verification: compared both original progress-bar functions and the allocation thunk, inspected the bitmap delete and runtime heap creation/free paths, and reviewed the other production bitmap allocation sites for the same explicitly global 0x54-byte allocation. They use runtime allocation or inherited Storable allocation; the stack placement constructions retain their explicit lifetimes. No build, unit tests or runtime rerun were performed. Source-address coverage and the temporary inventory are unchanged.
