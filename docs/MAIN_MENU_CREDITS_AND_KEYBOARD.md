# Main-menu credits and keyboard input

Snapshot: 2026-10-04, working changes after d22ddbd (Palette and Colors), alongside the preceding menu-command block.

## Connected scope

The primary keyboard handler 004DB0D1, credits widget constructor 004CBCD6 and auto-scroll setter 004CBF6C now have real bodies. Their three temporary definitions and inventory rows are removed. The credits screen still uses its original rectangle, directory, flags, 20-pixel/second speed and 1000-ms delay. It now reaches a real resource container, entity parser and display manager, rather than writing fields through the constructor dummy's null return.

The block adds 42 distinct Windows mappings: 39 hypertext/widget entries, keyboard handler and clipboard helper, and palette copy construction 0040F9B7. Original list/runtime/font/bitmap/picture/storable methods are reused. Source helpers represent Windows inline allocation/stores; they are not additional claimed original functions. List-specialization deleting wrappers are supplied by native list lifetimes and are not counted as independently reconstructed addresses.

Mac confirms the NHyperLinkTextSystem class/method names, SMicroEntity, display-line records, sTokenArray/sTokenList, lastKeysBuffer and the palette copy constructor. Clipboard copying has a Windows-only role name. Windows assembly supplies all layouts, parser decisions, ownership and call order. The instruction export and Game.exe PE data were both checked; the controller jump table is data and excluded from function coverage.

## Keyboard and clipboard

Only message 0x200 processes input; every path returns true. Ctrl is bit two of the key manager's first DWORD, and V is key 0x56. The Windows helper 004DB3A5 rejects null output/capacity <= 1, opens the clipboard with no window, requests CF_TEXT, checks nonempty text, copies at most capacity minus one and terminates the output. CloseClipboard runs after every successful open. It treats the returned handle directly as a string pointer, without GlobalLock/GlobalUnlock, exactly as Windows does. That unsafe handle assumption remains an original observation; no clipboard runtime result is claimed.

Pasting uses a 100-byte stack buffer and feeds each byte to StringInput_HandleKeys with key zero, re-reading strlen for each iteration. Its return is ignored. Failed paste is consumed without entering cheat recognition. Ordinary key/character messages first reach the existing StringInput_HandleKeys, preserving callsign/session-name/chat completion. Only an unconsumed nonzero character updates cheat history.

The complete 21-byte region 005694F0..00569504 is lastKeysBuffer. Windows moves nineteen bytes one position upward, writes byte twenty to zero, and inserts the newest character at zero. Only campaign screen five and a nonnull gCampaignStaticDataManager enter code recognition. Case-sensitive prefix comparisons against PE strings esao, lekaro, lesniztahcs, lepmet, aipmylo, oarahp and sitnalta unlock campaign three's missions 20..80 respectively; sursemag unlocks all seven. Comparisons are independent. Each allow operation retains save=true and reloads the owning singleton. A match plays FXTool_Chat, rebuilds the screen and clears only history byte zero.

## Hypertext ownership and parsing

The 0x48-byte raw GUI widget owns CDataContainer (+2C, 0x50 bytes), CEntityManager (+30, eight bytes) and CDisplayManager (+34, 0x60 bytes). It installs all nineteen virtual slots from Windows table 004F795C..004F79A7, including pressed action and background drawing. Destruction clears $local$ first, then destroys display lines, parsed entities, cached resources and the GUI base. All dynamic owners, nodes, resource records and text buffers use the explicit new-handler mode-one runtime allocation family and Runtime_Free. Native non-owning list vtables are retained as a source-level implementation detail; count/link/ownership state matches Windows, whose resource-container constructor clears its initialized list storage.

Four resource lists cache case-insensitive filenames. Font/palette entries are 0x108 bytes, bitmap entries 0x10C with separate bitmap/palette ownership, and text-file names 0x104 bytes. Names copy at most 259 bytes after clearing storage. Missing files return null; missing text-file IDs return zero, which also identifies the first valid entry. Cached Storable owners are destroyed before their record allocation. CPicture extraction transfers bitmap/palette ownership as in Windows.

The complete token table 004F7D08..004F7D87 has fifteen named rows plus its terminator; the complete eight-byte state 005694A4..005694AB contains the pointer and initialization flag. Tokens are font, color, picture, userpicture, icon, usericon, block, textxstart, textxend, anchor, localjump, globaljump, include, rem and onscreencallback. The first parse initializes the existing token machinery and registers the original empty exit callback.

AddString skips literal space/tab/CR/LF, recognizes explicit backslash-n line breaks, splits words and temporarily terminates angle-bracket tags. Unknown tags are skipped. Missing closing '>' returns false. Include recursively loads a selected block and optional CR line conversion. Bitmap user callbacks retain their original four integer arguments and borrowed return value. SMicroEntity is 0x1C bytes; only type-two entities own their text allocation.

AddTextFile uses file-size plus one, closes the file before block parsing, and ignores AddString's return. Named blocks use two unchecked 100-byte formatted marker buffers. Missing start/end markers return false without releasing the loaded text allocation (OB096). Include and on-screen callback arguments retain unchecked comma assumptions. Globaljump restores its comma before passing that comma itself to atoi, so ordinary anchors become zero (OB097). These are retained Windows behaviors, not substitutes for missing reconstruction.

## Display and interaction

Line handlers are twelve bytes and own non-owning lists of 0x2C-byte line entities. The display manager zeroes only its first 0x30 bytes before building layout. Font/palette controllers, alignment, margins, explicit breaks, anchors, one-shot jumps, centered pictures, inline icons and screen callbacks follow all ten entries of controller table 004CBBFC..004CBC23.

Words wrap at the right margin, insert the font's 'i' width as spacing and derive line height from font +8 times 3/2. Center/right alignment retains unsigned distance shifts; justification retains signed fixed-point division and the 0xC00 threshold. Line completion removes trailing word spacing, defaults empty height to twenty, bottom-aligns entities and appends a new line handler. The final empty line and maximum-scroll calculation are retained. Missing font/palette and certain malformed resource/controller states remain unchecked, including uninitialized marker rectangle fields on the original paths.

Drawing visits only vertically intersecting lines. Cached images use color-key zero; user images use ordinary copy. Highlighting copies/modifies a palette by 1.5 and restores the cached image's original palette. Text color comes from palette +0x400; screen-callback controllers write their three values and disable themselves after drawing. Native palette copy construction now matches 0040F9B7's cleared cache state and 0x400-byte color copy.

Scrolling clamps through l_SetDisplayY, retains the aligned double at +18 and converts to the integer +10 position with the existing Windows truncation wrapper. Auto-scroll uses timeGetTime, DWORD deadline subtraction, signed elapsed time and the PE single-precision factor 0.001. Wheel messages stop auto-scroll and divide signed param3 by -4. Hit testing translates mouse coordinates through the existing parent/local helper; clicks rebuild external-file entities/layout before jumping to the captured anchor. All widget virtuals now point to production bodies.

## Verification and remaining scope

Windows ASM/PE/static review covers layouts, token/virtual/controller tables, parser branches, keyword strings, owner order, runtime calls and unique definitions. Project and GUI filters are synchronized at 151 cpp files and 148 headers. Provisional coverage is 2,025/5,056 (40.05%), with 3,031 unmapped candidates and 127 dummies excluded. No builds, tests or game runtime were run, as requested. Runtime confirmation of credits, typing and navigation remains pending.

README OB096/OB097, OA109/OA110 and FP094..FP096 index the reviewed failures. Other main-menu listbox/tool-list constructors remain tracked dummies; this block does not establish complete menu/gameplay coverage. Next connected GUI block: CToolListBoxElement/CToolListBoxControllerElement and Element_Add, followed by the separately queued Houses startup and deeper drawing/gameplay dependencies.
