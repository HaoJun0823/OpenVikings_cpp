# Native support for the remaining Win32 link tree

Snapshot: 2026-10-03, checkpoint reconstruction committed after d64247c at the user's request. Windows instructions and executable bytes establish behavior; the Mac build supplies names and parameter identities only.

## Title, FMV and native Windows video

The title-screen entries 00402773/0040279C/004027C7, 004027D2/00402A15/00402A58, 00402AF2/00402C08/00402C90/00402E88/00402FCB and pre-ingame-FMV entries 004223F0/00422319/00422344, 00403029/0040313F/00403182/004031F7/0040330D/0040339B/00403406/00403475 now have native bodies. Their separate original routines remain separate. The complete eight-byte singleton/state regions are 0050F630..0050F637 and 0050F638..0050F63F. Title objects are E8 bytes; pre-ingame objects are D0 bytes.

Property consumption, language-specific video lookup, fullscreen mode probing, bitmap/font/palette lifetimes, cursor restoration, five-frame transition updates, pre/post picture sequences and input gating retain Windows order. Title picture replacement calls the deleting destructor with flag one: the bitmap object allocation is freed as well as its owned pixels. The 20-second picture deadline retains the original unsigned comparison and wrapped addition.

WindowsVideoPlayer reconstructs 00404452/0040446C/00404484/004045CB/00404635/0040465C/0040467C/004046DA through the actual COM and DirectDraw interfaces. The original Windows video class name remains unresolved. The complete seven-GUID region 004FC47C..004FC4EB is represented numerically. The 2C-byte object's pointer fields, source rectangle and availability/playing bytes have x86 layout assertions.

Probe accepts exact S_OK; media setup accepts nonnegative HRESULT; sample Update accepts exact S_OK. Audio-stream addition, Seek, state transitions, conversion and Blt results are ignored where Windows ignores them. Close releases surface, sample, draw-media stream, media stream, multimedia stream and DirectDraw in that order; it clears availability but retains playing state. Failure before publication of the local multimedia stream leaks that acquired interface, as in Windows (README OB095). CoInitialize is called on each probe without a matching CoUninitialize in this tree. No invented COM rollback was added.

Fullscreen probe 00478F52 and callback 00478FE7 load DDRAW.DLL, create IDirectDraw7, enumerate exact width/height/depth and release/unload. Complete query storage is 00554D64..00554D73. Mode enumeration and cooperative display changes retain their original result handling. Shared mouse query 00403014 is CMouseManager::IsAnyButtonDown.

## House, vehicle, strings and tooltip support

C2StringAccess reconstructs 0042F6E1/0042F703/0042F725/0042F747/0042F769/0042F792/0042F7A9/0042F7C0/0042F7D7/0042F7EE through the existing static CStringArray owners. Good/job/house singular/plural selection uses amount > 1; missing arrays return null. Development-name lookup 0048089E shares the existing house-entry owner. The overload preserves its upper development-stage check.

House access covers 004316CC/004316DC/00431A88/00431F3C/00433064/00434345. Stock slots preserve the five substitute-good mappings. Stock amount loads future stock unsigned and adds current stock signed; the 0.1 coefficient is the exact Windows float constant. The future-stock term is included even when the Boolean argument is false. That argument instead controls special direct counters and the main-type-three stock-entry byte gate. The stock-entry byte remains unnamed. Construction percentage uses wrapped multiplication and unchecked signed division.

Vehicle access covers 004375A6/00437912/004412F3/0044BA1B/0044BDA9/0044BDBB/0044BFD2/0044C2B5/0044CB4B plus Human map-position entry 0042CE24. Mount/carrier validity checks include allocation and generation, then slot state, nested carriers and map-moveable fallback. Missing-slot indices are deliberately not guarded where the original performs wrapped reads. The text export loses 0044DA5D; executable bytes confirm its one-byte receiver query. Vehicle stock amount is a byte and is exactly representable as float, so the old tooltip inline assembly conversion is unnecessary.

Opposite-direction entry 0042A83E uses the complete eight-DWORD table 004F3A18..004F3A37. Packed-position entry 0042ADEC receives a DWORD value; its former pointer declaration/call in the ambient callback was a reconstruction error and is corrected. House walk-block area lookup preserves type and development-stage offsets.

Tooltip comparator 004CD2BB compares plural good names through reconstructed 004E2BF0. The latter retains the complete seven-byte umlaut string 0050BF48..0050BF4E, case-insensitive runtime comparison and conditional 99-character truncation/first-byte normalization. Player-color entries 0048B442, 0048B3A1 and forwarding entry 00490EDF share existing human-misc records; inactive/invalid players use magenta, neutral player 20 uses grey, invalid color IDs select record nine. Signed upper-only player bounds remain original. GUI redraw entry 004986AB uses existing callback dispatch 00411E1D.

## Manager construction and group callbacks

Eleven native constructors are connected to their existing destruction/owner paths: marker array 0040B94A, moveable array 0040BB10, mission-onscreen manager 0041714B, game assistant 004175EC, multiplayer goals 00419032, happiness 004196CD, embedded savegame 00419BC5, tribute 00419DDB, delayed actions 00419F41, Human merchant array 0041C3AA and map pathfinder 0041CD2F.

They preserve exact clear/allocation sizes, embedded member initialization, canonical owner publication and callback registration order. Allocation failures retain the original unconditional clearing rather than inventing recovery. All array owners share already reconstructed allocation predicate 0040AD6C and CArrayHandler initialization. Embedded moveable/pathfinder constructors 0040BBAA/0041CDCC retain their writes before the subsequent full clear.

Object-group lookup entries 004A388D/004A38CB/004A3909 retain ten groups, twelve-byte entries and first-match behavior. Callback 004A37B1 and helpers 004A32C2/004A34C8/004A3508/004A3603/004A36B0/004A3741/004A3778 verify allocation/generation/local ownership, compact invalid records, remove only the first matching ID in each group, and clear group storage on the original nonzero load event. The count/index and memmove length remain unchecked.

## Keyboard assignment and particle storage

InGameGuiKeyAssignmentManager reconstructs 004A3D47/004A3D9A/004A3DA2/004A3E03/004A3E5B under the original CKeyAssignmentManager name. The complete default table 004F4E38..004F524B, including its terminator, was decoded from Windows PE bytes. The A08-byte object retains 36-byte assignment records with at most four eight-byte key entries. It preserves signed upper-only assignment bounds, equality-only capacity rejection, success for an empty requested key, current Shift/Control state checks, active-application gating and virtual-key-only direct checks. All six singleton xrefs use the same owner at 005690DC; the previous guessed GUI-definition identity is superseded.

Complete particle-generator storage 00568B44..00568B4B binds its byte/padding and already established owner at +4. PE section bounds prove zero-filled virtual data. All three direct byte reads choose the game-tick or real-time animation clock; no direct writer or original byte name is recovered. It remains explicit as unresolved00. Adjacent 00568B3C/00568B40 belong to a separate lazy color and are not folded into the generator region.

## Dynamic objects, map, weather and world music

Shared Random_Get 00410CCD/00410CD2 and forwarding entries 00407570/0040AED5 retain both original reductions. Display shaking uses its separate canonical seed through this recurrence. Dynamic house/vehicle/animal constructors 0049073B/004908B4/00490AA5 and creation resets 0049088C/00490A2C/00490C1D retain allocation sizes, clear order, type-key lookup, variant selection and sentinel writes. Complete job-graphics lookups 0048D093/0048D0CE use the existing static map-moveable records.

Map conversion/wrap/offset entries 0042A5F1/0042AC91/0042B1EA/0042AF98/0042A9E7 preserve signed arithmetic and unchecked dimensions. Existing 0042AD18 clamps inside the map; it is distinct from wrapping. Weather sector construction/allocation/query/rectangle/INI entries 00484BD1/00484C4F/004853D6/0048520F/0048508E retain ten-point sector division, inclusive updates, packed by-value points, clamping order and all four token rows. Texture construction 0049039E clears its inherited list header as Windows does.

World-music construction/update/CD update/INI/event variant entries 00485AA1/00485C3A/00485ED7/00485F57/00485BA6 preserve callback order, mode gates, stop-before-start behavior, pending-time handling, unchecked music indices and the unusual nonzero gameTime%120 condition. Complete indexed CD-track region 00505FF4..0050608F is retained; semantic subdivision of its trailing values remains unresolved. Diplomacy query 0040A12B keeps signed upper-only player bounds and inactive-player checks.

## GUI statics, clock and dead-Human history

Class-static entries 004A59AB/004A5FF5/004A7142/004A9946/004ABA22/004AF057/004B00DA/004B1093/004B1DDE have canonical complete byte regions in InGameGuiWindowState.h. Unidentified fields remain explicit bytes.

Clock entries 004B99B8/004B99F7/004B9C63/004B9C71 use complete 18-slot clock/base-tool tables. Shared deleting entry 004D0823 now publishes the base-tool table, correcting the former text-table reconstruction. Local time formatting, font positioning, exact float angle constants, pointer-coordinate truncation and four lazy grayscale colors are retained. The known-empty exit routines remain separate. Original CRT sine/cosine boundaries remain unresolved real dependencies.

Dead-Human addition/callback 004B389E/004B3957 preserve local-player filtering, 50-record ring indexing, bounded 40-byte names, timestamps and packed positions. Its Human-array reset has no standalone Windows function address. Dependency-map constructor 004121A9 fills all 176 slots in 00510A80..00510D3F from goods and the eleven original overrides.

## Static actor types and Bob resource cache

C2StaticActorTypeDataAccess reconstructs animal 00412299, armor 0041277C, weapon 00412A5D, job 0041300A and experience 00411E81 constructors with typed Windows record layouts and complete writable token regions. Allocation sizes/families, owner publication, repeated-group behavior, lazy token initialization and empty cleanup callbacks retain Windows order. Weapon tables allocate 42 arrays of 22 records. Job base-atomic inheritance copies exactly 5C bytes once; atomic/weapon/experience indices remain unchecked, and experience insertion rejects count exactly ten. Missing job definitions invoke the existing warning dialog only after the INI reader is destroyed; an unloaded file skips that validation. Names alone come from Mac correspondence.

Bob manager construction/cache lookup 004905B9/0049063F preserve the cleared inherited header, case-insensitive runtime filename comparison, 54-byte entries, 79-byte bounded names, file-existence rejection, original storable factory and caching of null load results. The two startup resources are loaded in the original order.

## Player statistics

C2PlayerStatisticAccess reconstructs 0041AA59/0041ACD6/0041AD6F/0041ADAF and count tools 004408EE/004409C1/00440A53. Active-player type one allocates 600 history samples; other active types allocate a current sample. The 138-byte sample keeps unknown fields explicit. Update increments the index, wraps only at exactly 600, fills history and current buffers in player order, then stores the timestamp. Human/house counters wrap as WORDs; goods retain wrapped DWORD totals and unsigned saturation at FFFF for per-good counts.

Player-filtered iterator entries 0042C1EE/0042B788 share existing scanning implementations. Human age/sex/home/work predicates 0042C597/0042C522/0042C652/0042C65F and integer stock query 00431944 are native. Stock lookup retains substitute IDs, special direct counters and upper-only bounds; the integer query is distinct from the existing floating-point query.

## Ambient sound, Human sounds and GUI buttons

Ambient entries 0047F3C7/0047F4D6/0047F533/0047F56E/0047F760/0047F90B retain callback order, signed WORD dimensions, unchecked allocation/index arithmetic and the complete ten-byte change region at 00554DA8. Pattern and landscape scans use distinct complete 100-DWORD counters and rank three groups, retaining ascending-group ties. The collector compares ambient IDs against 00FFFFFF while the existing loader uses FFFFFFFF; this original mismatch is preserved. Map-border and engine map-point access helpers are native.

Human-sound construction/load 00485453/004854D9 retain the first-line tribe command, unchecked tribe/sex-age/response indexing, sound-ID lookup before the unsigned fifteen-entry cap, and the complete five-token region. Speed/priority updates 004A299B/004A2A55 preserve graphics-before-cache ordering, unsigned speed division, tooltip copying and the complete initialized cache pair at 00507378.

## Network and actor command processing

C2UserNetworkCommandAccess reconstructs 00410F00/00410FCC/00411829/004118F6. Windows PE bytes establish the misdecoded send entry and both dispatcher jump tables. Offline commands execute immediately; network queues retain system/deferred/ordinary priority, ordinary-only 20-byte deduplication, original malloc/free ownership and unconditional copies after allocation failure. Send packs at most nine commands. Receive executes unchecked slot counts before freeing the copied slots, then checks ten checksum records per participating player. Duplicate matching records count independently; mismatches warn, confirmed/mismatched ticks clear records, and stale records print before clearing.

Dynamic-network send/take entries 004DED59/004DF16F retain the 200-byte size limit, original assertions, packet fields, buffer-completeness check and copied-slot allocation. The original command buffers remain owned by their network manager. Real dispatcher callees remain explicit originals where their own bodies are unfinished.

House stock/production/enhancer/defence controls, special-item consumption, player network-ID lookup, game-assistant input, Human equipment/reference predicates and Human command insertion/interruption are native. Human interruption now follows the native good-circulation tree at 00441CB4 described below. The empty house command routine 0045BFEC is confirmed by Windows RET 4, not a fabricated substitute. Vehicle command insertion/immediate execution, wanted-stock clamping and passenger/commander checks preserve inclusive slot scanning and unchecked original references.

SToUserMessage construction 0042BF7E is shared with existing GUI allocation. FinishMessage 0042BFCC stamps referenced object revisions and the ready flag. External message adapter 004A307C retains local-player filtering; GUI insertion at 004B3DAA now follows the native message-manager tree described below.

## GUI message management and Human validity

CMessageManager reconstructs update, Human-array teardown, insertion/removal/all-removal, lookup, Human pruning, mini-button layout, history insertion/equality/similarity, priority and the complete callback switch (004B3C29..004B450D). Active records retain their 310-byte layout; 200 history records use 30C bytes each. Callback registration/unregistration uses the native callback. Order of Human notification, desktop detachment/deleting virtual calls, presentation-window closing and active-record compaction remains Windows behavior. Message similarity compares hex distance after validating the other Human, preserving its asymmetric rules; equality compares all 304 bytes, including padding. Copy construction skips bytes 5..7 as Windows does.

Human_UserMessage_IsInvalid (0045C37F) follows the complete Windows 59-selector table and fourteen branch targets. Job/task/atomic-state, marriage/vehicle and need/health conditions are native; the Mac reference supplies names only. Windows xrefs and PE bytes establish the read-only 004F3B38..004F3B77 threshold region: four 10000 maxima, four 8000 values, four 2000 fulfillment thresholds and four 1000 critical thresholds. Food/religion/social/stamina fulfillment order differs from the stored Human-member order. The exact meaning of all unconsumed thresholds, particularly 004F3B74, remains unresolved; consumed constants retain their original-address trace comments.

The complete 18-slot mini-button table 004F7CBC..004F7D03 and 41C-byte lifecycle/draw/input/tooltip family are native. Shared base-tool construction, X positioning and the proven return-true virtual entry are connected. Ctrl-removal, icon selection and confirm sound retain their order. Text formatting 004D21DE, presentation opening 004B4247 and moveable portrait rendering 00493656 use confirmed Mac names and remain unfinished Windows implementations, temporarily supplied by the authorized link dummies.

## Good circulation and reservation rollback

Human_StopCurrentGoodCirculationMode (00441CB4) follows all five modes and clears the mode after its side effects. Target validity, attached home/work/vehicle checks and goods routing preserve their original validation boundaries. House storage and future-amount entries 004316EF/00431D2D/00432D14 retain production-specific updates, signed underflow rejection, 1000 saturation and sign-extended low-byte production adjustments. Vehicle future entries 0044BB37/0044BB63/0044BBA6 retain byte counters, signed capacity/underflow comparisons and requested rather than clamped ledger deltas.

The whole 21-entry vehicle Human ledger at stock-relative C8..1C3 is covered by 0044BCE5/0044BD0D/0044BD6C. Removal clears only its used flag; reuse adds to the retained counter. A full list silently leaves the ledger unchanged. InteractionTargetData_IsValid (0045CCFF) follows all nine Windows branches, including unchecked allocated/revision lookup and the vehicle-door check that ignores its return value. Vehicle door calculation/search 0043779D/004378A1 retains direction mapping, signed distance halving, continent/space checks, six neighboring positions and mutation on failure. Unwrapped repeated cursor motion at 0042AC72 shares the established single-step movement.

## Verification and remaining work

The user authorized temporary dummy implementations on 2026-10-03 so a compilable checkpoint could be reached before resuming functional reconstruction. Both Debug|Win32 and Release|Win32 build and link with zero unresolved symbols. Project/filters contain identical 141 cpp files and 140 headers; one cpp is TemporaryLinkStubs.cpp, containing 146 explicitly marked dummies. These are tracked in TEMPORARY_LINK_STUBS.csv and excluded from the actual 1,890/5,056 reconstruction coverage. Eighteen startup constructor boundaries remain unfinished. No tests or gameplay/runtime verification are claimed.

README OA094/OA095 and FP074/FP075 index the reviewed real-message/transport assumptions and failure paths. The temporary dummies provide no original behavior. Continue the message text/presentation/portrait and command/tick, script/static-manager, world and widget trees, removing each matching dummy when a real implementation is connected.

The subsequent naming pass replaces all 143 legacy active FUN identifiers while retaining these temporary bodies and all original ABIs. See [FUNCTION_NAMES.md](FUNCTION_NAMES.md) and [FUNCTION_NAME_MAP.csv](FUNCTION_NAME_MAP.csv). No automatic Debug/Release builds are run under the current user instruction.
