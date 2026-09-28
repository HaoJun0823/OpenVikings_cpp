#include "XCrtFileAccess.h"
#include "OriginalStrings.h"

#include <cstring>

// Original data: 005699e0; initially empty name buffer.
char gRuntimeTemporaryFileName[16]{};

// Original data: 005699f0; initially empty temporary-stream name buffer.
// Reconstruction capacity covers the complete generated filename.
char gRuntimeTemporaryStreamFileName[16]{};

namespace NXBasics
{
    // Global data (original order and initializers preserved)

    // Original range: 004fe070..004fe07c, end exclusive.
    // The adapter discards the original temporary-stream cleanup return value.
    RuntimeVoidCallback gRuntimePreTerminators[3] =
    {
        nullptr, &Runtime_PreTerminateStreams, +[] ()
            {
                Runtime_RemoveTemporaryStreams();
            }
    };

    // Original range: 004fe080..004fe084 (exclusive), one null slot in Game.exe.
    RuntimeVoidCallback gRuntimeTerminators[1] = { nullptr };

    // Original data: 0050c230, initially FUN_004e6efc.
    RuntimeVoidCallback gRuntimeFloatingPointInitializer = &Runtime_InitializeFloatingPoint;

    // Original data: 0050c240, initially FUN_004ea147.
    void(__cdecl* gRuntimeFatalExitCallback)(int) = &Runtime_QuickExit;

    // Additional CRT data (original-address order).
    // Original data: 0050c244.
    std::uint32_t gRuntimeApplicationType = 0x00000002u;

    // Original data: 0050c3f0; original target: 004fba44.
    const char* gRuntimeNullString = "(null)";

    // Original data: 0050c3f4; original target: 004fba34.
    const wchar_t* gRuntimeNullWideString = L"(null)";

    // Original data: 0050c400 through 0050c601; element zero represents EOF.
    std::uint16_t gRuntimeCharacterTypeStorage[257] = {
        0x0000u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0028u,
        0x0028u,
        0x0028u,
        0x0028u,
        0x0028u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0020u,
        0x0048u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0084u,
        0x0084u,
        0x0084u,
        0x0084u,
        0x0084u,
        0x0084u,
        0x0084u,
        0x0084u,
        0x0084u,
        0x0084u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0081u,
        0x0081u,
        0x0081u,
        0x0081u,
        0x0081u,
        0x0081u,
        0x0001u,
        0x0001u,
        0x0001u,
        0x0001u,
        0x0001u,
        0x0001u,
        0x0001u,
        0x0001u,
        0x0001u,
        0x0001u,
        0x0001u,
        0x0001u,
        0x0001u,
        0x0001u,
        0x0001u,
        0x0001u,
        0x0001u,
        0x0001u,
        0x0001u,
        0x0001u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0082u,
        0x0082u,
        0x0082u,
        0x0082u,
        0x0082u,
        0x0082u,
        0x0002u,
        0x0002u,
        0x0002u,
        0x0002u,
        0x0002u,
        0x0002u,
        0x0002u,
        0x0002u,
        0x0002u,
        0x0002u,
        0x0002u,
        0x0002u,
        0x0002u,
        0x0002u,
        0x0002u,
        0x0002u,
        0x0002u,
        0x0002u,
        0x0002u,
        0x0002u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0010u,
        0x0020u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
        0x0000u,
    };

    // Original data: 0050c3f8; original target: 0050c402.
    std::uint16_t* gRuntimeCharacterTypes = gRuntimeCharacterTypeStorage + 1;

    // Original data: 0050c604.
    std::int32_t gRuntimeMaxMultibyteLength = 0x00000001u;

    // Original data: 0050c608.
    char gRuntimeDecimalSeparator = '.';

    // Original data: 0050c610 through 0050c633.
    RuntimeDescriptorRecord gRuntimeInvalidDescriptorRecord = {
        0xFFFFFFFFu, 0x00u, 0x0Au, {}, 0u, {} };

    // Original data: 0050c634.
    std::int32_t gRuntimeTimeZoneBiasSeconds = 28800;

    // Original data: 0050c638.
    std::int32_t gRuntimeDaylightEnabled = 1;

    // Original data: 0050c63c.
    std::int32_t gRuntimeDaylightBiasSeconds = -3600;

    // Original address: 0050c640.
    char gRuntimeStandardTimeZoneName[64] = "PST";

    // Original address: 0050c680.
    char gRuntimeDaylightTimeZoneName[64] = "PDT";

    // Original addresses: 0050c6c0 and 0050c6c4.
    char* gRuntimeTimeZoneNames[2] = { gRuntimeStandardTimeZoneName, gRuntimeDaylightTimeZoneName };

    // Original address: 0050c6c8.
    std::int32_t gRuntimeDaylightStartYear = -1;

    // Original address: 0050c6cc.
    std::int32_t gRuntimeDaylightStartYearDay = 0;

    // Original address: 0050c6d0.
    std::int32_t gRuntimeDaylightStartMilliseconds = 0;

    // Original address: 0050c6d8.
    std::int32_t gRuntimeDaylightEndYear = -1;

    // Original address: 0050c6dc.
    std::int32_t gRuntimeDaylightEndYearDay = 0;

    // Original address: 0050c6e0.
    std::int32_t gRuntimeDaylightEndMilliseconds = 0;

    // Original data: 0050c6e4.
    DWORD gRuntimeThreadDataTlsIndex = 0xFFFFFFFFu;

    // Float dispatch slots initially point to the missing-float-support handler.
    // Original data: 0050c780.
    decltype(&Runtime_ConvertFloat) gRuntimeFloatConvertCallback = reinterpret_cast<decltype(&Runtime_ConvertFloat)>(&Runtime_FloatSupportUnavailable);

    // Original data: 0050c784.
    decltype(&Runtime_FloatTrimTrailingZeros) gRuntimeFloatTrimTrailingZerosCallback = reinterpret_cast<decltype(&Runtime_FloatTrimTrailingZeros)>(&Runtime_FloatSupportUnavailable);

    // Original data: 0050c788.
    decltype(&Runtime_AssignFloat) gRuntimeFloatAssignCallback = reinterpret_cast<decltype(&Runtime_AssignFloat)>(&Runtime_FloatSupportUnavailable);

    // Original data: 0050c78c.
    decltype(&Runtime_FloatInsertDecimalPoint) gRuntimeFloatInsertDecimalPointCallback = reinterpret_cast<decltype(&Runtime_FloatInsertDecimalPoint)>(&Runtime_FloatSupportUnavailable);

    // Original data: 0050c790.
    decltype(&Runtime_FloatAtLeastReference) gRuntimeFloatReferenceCheckCallback = reinterpret_cast<decltype(&Runtime_FloatAtLeastReference)>(&Runtime_FloatSupportUnavailable);

    // Original data: 0050c794.
    decltype(&Runtime_ConvertFloat) gRuntimeFloatConvertAlternateCallback = reinterpret_cast<decltype(&Runtime_ConvertFloat)>(&Runtime_FloatSupportUnavailable);

    // Original data: 0050c968 through 0050c9df.
    RuntimeExceptionAction gRuntimeDefaultExceptionActions[10] = {
        {0xC0000005u, 11, 0u}, {0xC000001Du, 4, 0u}, {0xC0000096u, 4, 0u}, {0xC000008Du, 8, 0u}, {0xC000008Eu, 8, 0u}, {0xC000008Fu, 8, 0u}, {0xC0000090u, 8, 0u}, {0xC0000091u, 8, 0u}, {0xC0000092u, 8, 0u}, {0xC0000093u, 8, 0u} };

    // Original data: 0050c9e0.
    std::int32_t gRuntimeFloatingPointActionBegin = 3;

    // Original data: 0050c9e4.
    std::int32_t gRuntimeFloatingPointActionCount = 7;

    // Original data: 0050c9ec.
    std::int32_t gRuntimeExceptionActionCount = 10;

    // Original table: 0050ca08..0050ca98 (exclusive).
    const RuntimeFatalMessage gRuntimeFatalMessages[] = {
        {2, "R6002\r\n- floating point not loaded\r\n"},				  // 0050ca08; text 004fbde8.
        {8, "R6008\r\n- not enough space for arguments\r\n"},			  // 0050ca10; text 004fbdbc.
        {9, "R6009\r\n- not enough space for environment\r\n"},			  // 0050ca18; text 004fbd90.
        {10, "\r\nabnormal program termination\r\n"},					  // 0050ca20; text 004fbd6c.
        {16, "R6016\r\n- not enough space for thread data\r\n"},		  // 0050ca28; text 004fbd40.
        {17, "R6017\r\n- unexpected multithread lock error\r\n"},		  // 0050ca30; text 004fbd10.
        {18, "R6018\r\n- unexpected heap error\r\n"},					  // 0050ca38; text 004fbcec.
        {19, "R6019\r\n- unable to open console device\r\n"},			  // 0050ca40; text 004fbcc0.
        {24, "R6024\r\n- not enough space for _onexit/atexit table\r\n"}, // 0050ca48; text 004fbc88.
        {25, "R6025\r\n- pure virtual function call\r\n"},				  // 0050ca50; text 004fbc60.
        {26, "R6026\r\n- not enough space for stdio initialization\r\n"}, // 0050ca58; text 004fbc28.
        {27, "R6027\r\n- not enough space for lowio initialization\r\n"}, // 0050ca60; text 004fbbf0.
        {28, "R6028\r\n- unable to initialize heap\r\n"},				  // 0050ca68; text 004fbbc8.
        {120, "DOMAIN error\r\n"},										  // 0050ca70; text 004fbbb8.
        {121, "SING error\r\n"},										  // 0050ca78; text 004fbba8.
        {122, "TLOSS error\r\n"},										  // 0050ca80; text 004fbb98.
        {252, "\r\n"},													  // 0050ca88; text 004fe480.
        {255, "runtime error "},										  // 0050ca90; text 004fbb88.
    };

    // Original data: 0050ca98 through 0050cb47.
    // Exact strings recovered from Game.exe, including original wording.
    const char* gRuntimeErrorTextTable[] =
    {
        "No error",							   // 0
        "Operation not permitted",			   // 1
        "No such file or directory",		   // 2
        "No such process",					   // 3
        "Interrupted function call",		   // 4
        "Input/output error",				   // 5
        "No such device or address",		   // 6
        "Arg list too long",				   // 7
        "Exec format error",				   // 8
        "Bad file descriptor",				   // 9
        "No child processes",				   // 10
        "Resource temporarily unavailable",	   // 11
        "Not enough space",					   // 12
        "Permission denied",				   // 13
        "Bad address",						   // 14
        "Unknown error",					   // 15
        "Resource device",					   // 16
        "File exists",						   // 17
        "Improper link",					   // 18
        "No such device",					   // 19
        "Not a directory",					   // 20
        "Is a directory",					   // 21
        "Invalid argument",					   // 22
        "Too many open files in system",	   // 23
        "Too many open files",				   // 24
        "Inappropriate I/O control operation", // 25
        "Unknown error",					   // 26
        "File too large",					   // 27
        "No space left on device",			   // 28
        "Invalid seek",						   // 29
        "Read-only file system",			   // 30
        "Too many links",					   // 31
        "Broken pipe",						   // 32
        "Domain error",						   // 33
        "Result too large",					   // 34
        "Unknown error",					   // 35
        "Resource deadlock avoided",		   // 36
        "Unknown error",					   // 37
        "Filename too long",				   // 38
        "No locks available",				   // 39
        "Function not implemented",			   // 40
        "Directory not empty",				   // 41
        "Illegal byte sequence",			   // 42
        "Unknown error"						   // 43: fallback
    };

    // Original data: 0050cb48.
    std::int32_t gRuntimeErrorTextCount = 43;

    // Original data: 0050cca0 through 0050ecbf.
    RuntimeSmallBlockRegionMode2 gRuntimeSmallBlockInitialRegionMode2 = {
        &gRuntimeSmallBlockInitialRegionMode2,
        &gRuntimeSmallBlockInitialRegionMode2,
        &gRuntimeSmallBlockInitialRegionMode2.pages[0],
        &gRuntimeSmallBlockInitialRegionMode2.pages[0],
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFFFFFFFFu)),
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFFFFFFFFu)),
        {{0xF0u, 0xF1u}},
        {} };

    // Original data: 0050ecc0.
    RuntimeSmallBlockRegionMode2* gRuntimeSmallBlockSearchRegionMode2 = &gRuntimeSmallBlockInitialRegionMode2;

    // Original data: 0050ecc4.
    std::uint32_t gRuntimeSmallBlockThresholdMode2 = 0x1E0u;

    // Original data: 0050f120.
    const RuntimeBinaryFormat gRuntimeDoubleFormat = { 1024, -1023, 53, 11, 64, 1023 };

    // Original data: 0050f138.
    const RuntimeBinaryFormat gRuntimeFloatFormat = { 128, -127, 24, 8, 32, 127 };

    // Original data: 00569990.
    int gRuntimePentiumFdivAdjustment = 0;

    // Original address: 00569994.
    char* gRuntimeStartupEnvironmentBlock = nullptr;

    // Original data: 0056999c.
    int gRuntimeErrorOutputMode = 0;

    // Original storage starts at 005699a0.
    // The next known global starts at 005699cc; this span holds every table string.
    char gRuntimeFallbackErrorText[0x2C] = {};

    // Original data: 005699c8.
    RuntimeNewHandler gRuntimeNewHandler = nullptr;

    // Original data: 005699cc.
    std::uint32_t gRuntimeInterruptSignalAction = 0;

    // Original data: 005699d0.
    std::uint32_t gRuntimeBreakSignalAction = 0;

    // Original data: 005699d4.
    std::uint32_t gRuntimeAbortSignalAction = 0;

    // Original data: 005699d8.
    std::uint32_t gRuntimeTerminateSignalAction = 0;

    // Original data: 00569a00.
    std::uint32_t gRuntimeFileCreationMask = 0x00000000u;

    // Original address: 00569a20.
    char** gRuntimeEnvironment = nullptr;

    // Original address: 00569a24.
    // Shared environment table checked before making a writable copy.
    char** gRuntimeInitialEnvironment = nullptr;

    // Original address: 00569a28.
    wchar_t** gRuntimeWideEnvironment = nullptr;

    // Original data: 00569a38.
    unsigned char gRuntimeCleanupReturnsToCaller = 0;

    // Original data: 00569a3c.
    int gRuntimeCleanupStarted = 0;

    // Original data: 00569a40.
    int gRuntimeProcessExitStarted = 0;

    // Original address: 00569a48.
    std::int32_t gRuntimeUsesWindowsTimeZone = 0;

    // Original address: 00569a50.
    TIME_ZONE_INFORMATION gRuntimeTimeZoneInformation{};

    // Original address: 00569afc.
    char* gRuntimePreviousTimeZoneEnvironment = nullptr;

    // Original data: 00569b00.
    std::uint32_t gRuntimeTimeZoneInitialized = 0;

    // Original data: 00569b2c.
    std::int32_t gRuntimeDefaultFileMode = 0x00000000u;

    // Original data: 00569b30 through 00569b8f.
    CRITICAL_SECTION gRuntimeStaticLocks[4] = {};

    // Original data: 0050c798 through 0050c857.
    CRITICAL_SECTION* gRuntimeLockTable[48] = {
        nullptr, &gRuntimeStaticLocks[0], nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr,
        nullptr, &gRuntimeStaticLocks[2], nullptr, nullptr,
        nullptr, &gRuntimeStaticLocks[3], nullptr, nullptr,
        nullptr, &gRuntimeStaticLocks[1] };

    // Original data: 00569b98.
    LCID gRuntimeCTypeLocale = 0x00000000u;

    // Original data: 00569ba8.
    UINT gRuntimeCodePage = 0x00000000u;

    // Original address: 00569cc0.
    // 0 = undetected, 1 = Unicode API, 2 = ANSI API.
    std::int32_t gRuntimeEnvironmentStringsBackend = 0;

    // Original data: 00569cc4.
    RuntimeVoidCallback gRuntimeErrorBannerCallback = nullptr;

    // Original data: 00569cc8.
    std::uint32_t gRuntimeMapStringBackend = 0x00000000u;

    // Original data: 00569ccc.
    std::int32_t gRuntimeNewHandlerMode = 0x00000000u;

    // Original data: 00569cd0.
    int gRuntimeSystemCodePageRequested = 0;

    // Original data: 00569cd4.
    std::uint32_t gRuntimeSmallBlockEmptyPageCountMode2 = 0;

    // Original data: 00569cd8.
    std::uint32_t gRuntimeStreamBufferRequestCount = 0x00000000u;

    // Original data: 00569cdc and 00569ce0.
    char* gRuntimeTemporaryStreamBuffers[2] = {};

    // Original data: 00569ce4.
    std::uint32_t gRuntimeDefaultStreamFlags = 0;

    // Original data: 00569cec.
    std::uint32_t gRuntimeGetStringTypeBackend = 0x00000000u;

    // Original data: 00569d54.
    RuntimeMessageBoxCallback gRuntimeMessageBox = nullptr;

    // Original data: 00569d58.
    RuntimeActiveWindowCallback gRuntimeGetActiveWindow = nullptr;

    // Original data: 00569d5c.
    RuntimeLastPopupCallback gRuntimeGetLastActivePopup = nullptr;

    // Original address: 00569d60.
    // 0 = undetected, 1 = Unicode API, 2 = ANSI API.
    std::int32_t gRuntimeCompareStringBackend = 0;

    // Original data: 0056a384.
    RuntimeFormatStream** gRuntimeStreamTable = nullptr;

    // Original data: 0056a3a0 through 0056b39f.
    char gRuntimeStdinBuffer[0x1000] = {};

    // Original data: 0050ecc8 through 0050ef47.
    RuntimeFormatStream gRuntimeStaticStreams[20] = {
        {gRuntimeStdinBuffer, 0u, gRuntimeStdinBuffer, 0x101u, 0u, {}, 0x1000u, {}},
        {nullptr, 0u, nullptr, 0x02u, 1u, {}, 0u, {}},
        {nullptr, 0u, nullptr, 0x02u, 2u, {}, 0u, {}} };

    // Original data: 0050ecc8.
    RuntimeFormatStream& gRuntimeStdinStream = gRuntimeStaticStreams[0];

    // Original data: 0050ece8.
    RuntimeFormatStream& gRuntimeStdoutStream = gRuntimeStaticStreams[1];

    // Original data: 0050ed08.
    RuntimeFormatStream& gRuntimeStderrStream = gRuntimeStaticStreams[2];

    // Original data: 0056b3a0.
    std::int32_t gRuntimeStreamCapacity = 0;

    // Original data: 0056b3a4.
    std::uint32_t gRuntimeSmallBlockRegionCapacity = 0;

    // Original data: 0056b3a8.
    std::uint32_t gRuntimeSmallBlockPendingGroupIndex = 0;

    // Original data: 0056b3ac.
    // Stores a 32-bit address in the current x86 representation.
    std::uint32_t gRuntimeSmallBlockSearchRegion = 0;

    // Original data: 0056b3b0.
    // Stores a 32-bit address in the current x86 representation.
    std::uint32_t gRuntimeSmallBlockPendingRegion = 0;

    // Original data: 0056b3b4.
    std::uint32_t gRuntimeSmallBlockRegionCount = 0;

    // Original data: 0056b3b8.
    void* gRuntimeSmallBlockRegionTable = nullptr;

    // Original data: 0056b3bc.
    std::uint32_t gRuntimeSmallBlockThreshold = 0;

    // Original data: 0056b3c0.
    UINT gRuntimeMultibyteCodePage = 0;

    // Original data: 0056b3d0 through 0056b3db.
    // Preserve the original packed representation.
    std::uint32_t gRuntimeMultibyteFullWidthCase[3] = {};

    // Original data: 0056b3dc.
    int gRuntimeIsMultibyteCodePage = 0;

    // Original data: 0056b3e0 through 0056b4df.
    std::uint8_t gRuntimeMultibyteCaseMap[256] = {};

    // Original data: 0056b4e0 through 0056b5e0.
    // Byte values use indices 1 through 256.
    std::uint8_t gRuntimeMultibyteTypes[257] = {};

    // Original data: 0056b5e4.
    LCID gRuntimeMultibyteLocale = 0;

    // Original data: 0056b5e8.
    HANDLE gRuntimeHeap = nullptr;

    // Original data: 0056b5ec.
    std::uint32_t gRuntimeHeapMode = 0;

    // Original data: 0056b5f0.
    LONG gRuntimeLocaleChangeState = 0x00000000u;

    // Original data: 0056b5f4.
    LONG gRuntimeLocaleReaderCount = 0x00000000u;

    // Original data: 0056b600 through 0056b6ff.
    // Each allocated block contains 32 descriptor records.
    std::uint8_t* gRuntimeDescriptorBlocks[64] = {};

    // Original data: 0056b700.
    // Runtime descriptor initialization sets this to 32 after allocating block zero.
    std::uint32_t gRuntimeDescriptorCapacity = 0;

    // Original address: 0056b704.
    std::int32_t gRuntimeEnvironmentInitialized = 0;

    // Original data: 0056b708.
    int gRuntimeMultibyteInitialized = 0;

    // Original data: 0056b70c.
    RuntimeOnExitCallback* gRuntimeOnExitEnd = nullptr;

    // Original data: 0056b710.
    RuntimeOnExitCallback* gRuntimeOnExitBegin = nullptr;

    // Original functions: FUN_004e5f2c; forwarding wrapper: FUN_004e5fb7.
    std::int32_t Runtime_StringToInteger(const char* text)
    {
        const auto* cursor = reinterpret_cast<const std::uint8_t*>(text);

        const auto hasCharacterType = [] (std::uint32_t character, std::uint32_t mask) -> bool
        {
            if (gRuntimeMaxMultibyteLength > 1)
            {
                return Runtime_ClassifyCharacter(static_cast<std::int32_t>(character), mask) != 0;
            }

            const auto* characterTypes = reinterpret_cast<const std::uint8_t*>(gRuntimeCharacterTypes);
            return (characterTypes[character * 2u] & mask) != 0;
        };

        while (hasCharacterType(*cursor, 0x08u))
        {
            ++cursor;
        }

        std::uint32_t character = *cursor++;
        const bool negative = character == 0x2Du;

        if (negative || character == 0x2Bu)
        {
            character = *cursor++;
        }

        std::uint32_t value = 0;

        while (hasCharacterType(character, 0x04u))
        {
            // Preserve 32-bit wraparound without signed-overflow undefined behavior.
            value = value * 10u + character - 0x30u;
            character = *cursor++;
        }

        if (negative)
        {
            value = 0u - value;
        }

        std::int32_t result;
        std::memcpy(&result, &value, sizeof(result));
        return result;
    }

    // Original function: LAB_004eb9f6
    void Runtime_FloatTrimTrailingZeros(char* text)
    {
        char decimalSeparator;
        std::memcpy(&decimalSeparator, &gRuntimeDecimalSeparator, sizeof(decimalSeparator));

        char* position = text;

        while (*position != '\0' && *position != decimalSeparator)
        {
            ++position;
        }

        if (*position == '\0')
        {
            return;
        }

        ++position;

        while (*position != '\0' && *position != 'e' && *position != 'E')
        {
            ++position;
        }

        const char* suffix = position;
        char* destination = position;

        while (destination[-1] == '0')
        {
            --destination;
        }

        if (destination[-1] == decimalSeparator)
        {
            --destination;
        }

        for (;;)
        {
            const char character = *suffix++;
            *destination++ = character;

            if (character == '\0')
            {
                break;
            }
        }
    }

    // Original function: LAB_004eba44
    std::int32_t Runtime_FloatAtLeastReference(const double* value)
    {
        // Original reference value at DAT_004f3348: +0.0.
        return *value >= 0.0 ? 1 : 0;
    }

    // Original function: __cfltcvt at 004ebd4c
    void Runtime_ConvertFloat(const double* value, char* buffer, std::int32_t format, std::int32_t precision, std::int32_t uppercase)
    {
        if (format == 'e' || format == 'E')
        {
            Runtime_ConvertFloatScientific(value, buffer, precision, uppercase);
            return;
        }

        if (format == 'f')
        {
            Runtime_ConvertFloatFixed(value, buffer, precision);
            return;
        }

        Runtime_ConvertFloatGeneral(value, buffer, precision, uppercase);
    }

    // Original function: ___add_12
    void Runtime_Add96(std::uint32_t* destination, const std::uint32_t* source)
    {
        if (Runtime_Add32WithCarry(destination[0], source[0], &destination[0]) != 0)
        {
            if (Runtime_Add32WithCarry(destination[1], 1u, &destination[1]) != 0)
                ++destination[2];
        }

        if (Runtime_Add32WithCarry(destination[1], source[1], &destination[1]) != 0)
            ++destination[2];

        Runtime_Add32WithCarry(destination[2], source[2], &destination[2]);
    }

    // Reconstruction helper; no separate original function.
    std::int32_t Runtime_TimeSigned32(std::uint32_t bits)
    {
        std::int32_t result;
        std::memcpy(&result, &bits, sizeof(result));
        return result;
    }

    // Function definitions (original-address order; unaddressed helpers last)

    // Original function: FUN_00410bca.
    // Includes the direct classification path of FUN_004e7cbc.
    bool Runtime_IsLetter(char character)
    {
        const std::uint8_t byteValue = static_cast<std::uint8_t>(character);
        const std::int32_t signedCharacter = byteValue < 0x80 ? static_cast<std::int32_t>(byteValue) : static_cast<std::int32_t>(byteValue) - 0x100;

        if (gRuntimeMaxMultibyteLength > 1)
        {
            return Runtime_ClassifyCharacter(signedCharacter, 0x0103u) != 0;
        }

        const auto* classificationTable = reinterpret_cast<const std::uint8_t*>(gRuntimeCharacterTypes);

        std::uint16_t classification;
        std::memcpy(&classification, classificationTable + signedCharacter * 2, sizeof(classification));

        return (classification & 0x0103) != 0;
    }

    // Original function: FUN_00410bdd.
    // Includes the direct classification path of FUN_004e7cea.
    bool Runtime_IsDigit(char character)
    {
        const std::uint8_t byteValue = static_cast<std::uint8_t>(character);
        const std::int32_t signedCharacter = byteValue < 0x80 ? static_cast<std::int32_t>(byteValue) : static_cast<std::int32_t>(byteValue) - 0x100;

        if (gRuntimeMaxMultibyteLength > 1)
        {
            return Runtime_ClassifyCharacter(signedCharacter, 0x04u) != 0;
        }

        const auto* classificationTable = reinterpret_cast<const std::uint8_t*>(gRuntimeCharacterTypes);

        return (classificationTable[signedCharacter * 2] & 0x04) != 0;
    }

    // Original function: FUN_00410bf0
    bool Runtime_IsLetterOrNumber(char character)
    {
        const auto byteValue = static_cast<std::uint8_t>(character);
        const std::int32_t signedCharacter = byteValue < 0x80u ? static_cast<std::int32_t>(byteValue) : static_cast<std::int32_t>(byteValue) - 0x100;

        return Runtime_ClassifyAlphanumeric(signedCharacter) != 0;
    }

    // Original function: FUN_00410c03
    bool Runtime_IsPropertyNameCharacter(char character)
    {
        const auto byteValue = static_cast<std::uint8_t>(character);
        const std::int32_t signedCharacter = byteValue < 0x80u ? static_cast<std::int32_t>(byteValue) : static_cast<std::int32_t>(byteValue) - 0x100;

        if (Runtime_ClassifyAlphanumeric(signedCharacter) != 0)
        {
            return true;
        }

        return byteValue == 0x5Fu;
    }

    // Original function: FUN_004e5d8b
    std::int32_t Runtime_PrintString(char* destination, const char* format, ...)
    {
        va_list arguments;
        va_start(arguments, format);

        // The existing formatter implements the same string-stream setup
        // and terminating write as the original unbounded wrapper.
        const std::int32_t result = Runtime_FormatString(destination, 0x7FFFFFFF, format, arguments);

        va_end(arguments);
        return result;
    }

    // Original function: FUN_004e81c4
    std::int32_t Runtime_PrintStringN(char* destination, std::uint32_t capacity, const char* format, ...)
    {
        va_list arguments;
        va_start(arguments, format);

        const std::int32_t result = Runtime_FormatString(destination, Runtime_TimeSigned32(capacity), format, arguments);

        va_end(arguments);
        return result;
    }

    // Original function: FUN_004e5fc2
    RuntimeOnExitCallback Runtime_RegisterOnExit(RuntimeOnExitCallback callback)
    {
        Runtime_Lock(0x0D);

        const std::uint32_t capacity = Runtime_GetAllocationSize(gRuntimeOnExitBegin);
        const std::uint32_t usedBytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(gRuntimeOnExitEnd) - reinterpret_cast<std::uintptr_t>(gRuntimeOnExitBegin));

        if (capacity < usedBytes + sizeof(RuntimeOnExitCallback))
        {
            const std::uint32_t newCapacity = Runtime_GetAllocationSize(gRuntimeOnExitBegin) + 0x10u;
            auto* replacement = static_cast<RuntimeOnExitCallback*>(Runtime_Reallocate(gRuntimeOnExitBegin, newCapacity));

            if (replacement == nullptr)
            {
                Runtime_Unlock(0x0D);
                return nullptr;
            }

            // Reload the globals after realloc, as the original does.
            const std::uint32_t endOffset = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(gRuntimeOnExitEnd) - reinterpret_cast<std::uintptr_t>(gRuntimeOnExitBegin));

            gRuntimeOnExitBegin = replacement;
            gRuntimeOnExitEnd = reinterpret_cast<RuntimeOnExitCallback*>(reinterpret_cast<std::uintptr_t>(replacement) + endOffset);
        }

        *gRuntimeOnExitEnd++ = callback;

        Runtime_Unlock(0x0D);
        return callback;
    }

    // Original function: FUN_004e6040
    int Runtime_AtExit(RuntimeOnExitCallback callback)
    {
        return Runtime_RegisterOnExit(callback) != nullptr ? 0 : -1;
    }

    // Original function: FUN_004e6052
    void Runtime_InitializeOnExitTable()
    {
        gRuntimeOnExitBegin = static_cast<RuntimeOnExitCallback*>(Runtime_Allocate(0x80u));

        if (gRuntimeOnExitBegin == nullptr)
            Runtime_FatalExit(0x18);

        // The initial null entry is outside the populated callback range.
        gRuntimeOnExitBegin[0] = nullptr;
        gRuntimeOnExitEnd = gRuntimeOnExitBegin;
    }

    // Original function: FUN_004e6081
    std::int32_t Runtime_Close(std::uint32_t descriptor)
    {
        if (descriptor < static_cast<std::uint32_t>(gRuntimeDescriptorCapacity))
        {
            const auto* block = reinterpret_cast<const std::uint8_t*>(gRuntimeDescriptorBlocks[descriptor >> 5]);
            const std::uint32_t entryOffset = (descriptor & 31u) * 36u;

            if ((block[entryOffset + 4u] & 0x01u) != 0)
            {
                Runtime_LockDescriptor(descriptor);

                const std::int32_t result = Runtime_CloseNoLock(descriptor);

                Runtime_UnlockDescriptor(descriptor);
                return result;
            }
        }

        // Invalid or closed descriptor.
        *Runtime_GetErrnoPointer() = 9;
        *Runtime_GetDosErrnoPointer() = 0;
        return -1;
    }

    // Original function: FUN_004e60de
    std::int32_t Runtime_CloseNoLock(std::uint32_t descriptor)
    {
        DWORD closeError = 0;

        const auto initialHandle = Runtime_GetOsHandle(descriptor);

        if (static_cast<std::uint32_t>(initialHandle) != 0xFFFFFFFFu)
        {
            bool closeHandle = true;

            if (descriptor == 1u || descriptor == 2u)
            {
                // Preserve the original lookup order.
                const auto stderrHandle = Runtime_GetOsHandle(2u);
                const auto stdoutHandle = Runtime_GetOsHandle(1u);

                closeHandle = stdoutHandle != stderrHandle;
            }

            if (closeHandle)
            {
                // The original retrieves the handle again before closing it.
                const auto handleValue = Runtime_GetOsHandle(descriptor);
                const HANDLE handle = reinterpret_cast<HANDLE>(static_cast<std::uintptr_t>(handleValue));

                if (!::CloseHandle(handle))
                {
                    closeError = ::GetLastError();
                }
            }
        }

        // Perform descriptor cleanup even when CloseHandle failed.
        Runtime_ClearOsHandle(descriptor);

        auto* block = reinterpret_cast<std::uint8_t*>(gRuntimeDescriptorBlocks[descriptor >> 5]);
        const std::uint32_t entryOffset = (descriptor & 31u) * 36u;

        // Clear the entire flags byte, not only the open bit.
        block[entryOffset + 4u] = 0;

        if (closeError != 0)
        {
            Runtime_MapOsError(closeError);
            return -1;
        }

        return 0;
    }

    // Original function: FUN_004e64a5
    std::int32_t Runtime_Write(std::uint32_t descriptor, const void* data, std::uint32_t byteCount)
    {
        if (descriptor < static_cast<std::uint32_t>(gRuntimeDescriptorCapacity))
        {
            const auto* block = reinterpret_cast<const std::uint8_t*>(gRuntimeDescriptorBlocks[descriptor >> 5]);
            const std::uint32_t entryOffset = (descriptor & 31u) * 36u;

            if ((block[entryOffset + 4u] & 0x01u) != 0)
            {
                Runtime_LockDescriptor(descriptor);

                const std::int32_t result = Runtime_WriteNoLock(descriptor, data, byteCount);

                Runtime_UnlockDescriptor(descriptor);
                return result;
            }
        }

        *Runtime_GetErrnoPointer() = 9;
        *Runtime_GetDosErrnoPointer() = 0;
        return -1;
    }

    // Original function: FUN_004e650a
    std::int32_t Runtime_WriteNoLock(std::uint32_t descriptor, const void* data, std::uint32_t byteCount)
    {
        if (byteCount == 0)
        {
            return 0;
        }

        const std::uint32_t blockIndex = descriptor >> 5;
        const std::uint32_t entryOffset = (descriptor & 31u) * 36u;

        // gRuntimeDescriptorBlocks remains the original, unresolved descriptor-block table.
        const auto getEntry = [blockIndex, entryOffset] () -> const std::uint8_t*
        {
            return reinterpret_cast<const std::uint8_t*>(gRuntimeDescriptorBlocks[blockIndex]) + entryOffset;
        };

        const auto getHandle = [&getEntry] () -> HANDLE
        {
            HANDLE handle;
            std::memcpy(&handle, getEntry(), sizeof(handle));
            return handle;
        };

        if ((getEntry()[4] & 0x20u) != 0)
        {
            // The original ignores the result of seeking to the end.
            Runtime_SeekNoLock(descriptor, 0, 2);
        }

        const auto* input = static_cast<const std::uint8_t*>(data);
        std::uint32_t writtenTotal = 0;
        std::uint32_t insertedCarriageReturns = 0;
        DWORD writeError = 0;

        if ((getEntry()[4] & 0x80u) != 0)
        {
            std::uint8_t buffer[0x401];
            std::uint32_t consumed = 0;

            while (consumed < byteCount)
            {
                std::uint32_t buffered = 0;

                while (consumed < byteCount)
                {
                    const std::uint8_t value = input[consumed];
                    ++consumed;

                    if (value == 0x0A)
                    {
                        ++insertedCarriageReturns;
                        buffer[buffered++] = 0x0D;
                    }

                    buffer[buffered++] = value;

                    if (buffered >= 0x400u)
                    {
                        break;
                    }
                }

                DWORD written;

                if (!::WriteFile(getHandle(), buffer, buffered, &written, nullptr))
                {
                    writeError = ::GetLastError();
                    break;
                }

                writtenTotal += written;

                // Preserve the signed comparison performed by the original.
                std::int32_t signedWritten;
                std::memcpy(&signedWritten, &written, sizeof(signedWritten));

                if (signedWritten < static_cast<std::int32_t>(buffered))
                {
                    break;
                }
            }
        }
        else
        {
            DWORD written;

            if (::WriteFile(getHandle(), data, byteCount, &written, nullptr))
            {
                writtenTotal = written;
            }
            else
            {
                writeError = ::GetLastError();
            }
        }

        if (writtenTotal != 0)
        {
            // Count all inserted CR bytes, including those in a partially written
            // or failed final text chunk, as the original does.
            const std::uint32_t resultBits = writtenTotal - insertedCarriageReturns;
            std::int32_t result;
            std::memcpy(&result, &resultBits, sizeof(result));
            return result;
        }

        if (writeError != 0)
        {
            if (writeError == 5)
            {
                *Runtime_GetErrnoPointer() = 9;
                *Runtime_GetDosErrnoPointer() = 5;
            }
            else
            {
                Runtime_MapOsError(writeError);
            }

            return -1;
        }

        if ((getEntry()[4] & 0x40u) != 0 && input[0] == 0x1A)
        {
            return 0;
        }

        *Runtime_GetErrnoPointer() = 28;
        *Runtime_GetDosErrnoPointer() = 0;
        return -1;
    }

    // Original function: FUN_004e679e
    std::int32_t Runtime_Open(const char* fileName, std::uint32_t flags, std::uint32_t permissions)
    {
        return Runtime_OpenShared(fileName, flags, 0x40u, permissions);
    }

    // Original function: FUN_004e67b5
    std::int32_t Runtime_OpenShared(const char* fileName, std::uint32_t flags, std::uint32_t shareMode, std::uint32_t permissions)
    {
        SECURITY_ATTRIBUTES security{};
        security.nLength = sizeof(security);
        security.lpSecurityDescriptor = nullptr;
        security.bInheritHandle = (flags & 0x80u) == 0;

        std::uint8_t descriptorFlags = (flags & 0x80u) != 0 ? 0x10u : 0u;

        if ((flags & 0x8000u) == 0)
        {
            if ((flags & 0x4000u) != 0 || gRuntimeDefaultFileMode != 0x8000)
            {
                descriptorFlags |= 0x80u;
            }
        }

        const auto failWithError = [] (std::int32_t error) -> std::int32_t
        {
            *Runtime_GetErrnoPointer() = error;
            *Runtime_GetDosErrnoPointer() = 0;
            return -1;
        };

        DWORD desiredAccess;

        switch (flags & 3u)
        {
            case 0:
                desiredAccess = GENERIC_READ;
                break;

            case 1:
                desiredAccess = GENERIC_WRITE;
                break;

            case 2:
                desiredAccess = GENERIC_READ | GENERIC_WRITE;
                break;

            default:
                return failWithError(22);
        }

        DWORD sharing;

        switch (shareMode)
        {
            case 0x10:
                sharing = 0;
                break;

            case 0x20:
                sharing = FILE_SHARE_READ;
                break;

            case 0x30:
                sharing = FILE_SHARE_WRITE;
                break;

            case 0x40:
                sharing = FILE_SHARE_READ | FILE_SHARE_WRITE;
                break;

            default:
                return failWithError(22);
        }

        DWORD creationDisposition;

        switch (flags & 0x700u)
        {
            case 0x000:
            case 0x400:
                creationDisposition = OPEN_EXISTING;
                break;

            case 0x100:
                creationDisposition = OPEN_ALWAYS;
                break;

            case 0x200:
            case 0x600:
                creationDisposition = TRUNCATE_EXISTING;
                break;

            case 0x300:
                creationDisposition = CREATE_ALWAYS;
                break;

            case 0x500:
            case 0x700:
                creationDisposition = CREATE_NEW;
                break;

            default:
                return failWithError(22);
        }

        DWORD attributes = FILE_ATTRIBUTE_NORMAL;

        if ((flags & 0x100u) != 0)
        {
            const std::uint32_t effectivePermissions = permissions & ~static_cast<std::uint32_t>(gRuntimeFileCreationMask);

            if ((effectivePermissions & 0x80u) == 0)
            {
                attributes = FILE_ATTRIBUTE_READONLY;
            }
        }

        if ((flags & 0x40u) != 0)
        {
            attributes |= FILE_FLAG_DELETE_ON_CLOSE;
            desiredAccess |= DELETE;
        }

        if ((flags & 0x1000u) != 0)
        {
            attributes |= FILE_ATTRIBUTE_TEMPORARY;
        }

        if ((flags & 0x20u) != 0)
        {
            attributes |= FILE_FLAG_SEQUENTIAL_SCAN;
        }
        else if ((flags & 0x10u) != 0)
        {
            attributes |= FILE_FLAG_RANDOM_ACCESS;
        }

        const std::uint32_t descriptorBits = Runtime_AllocateDescriptor();

        if (descriptorBits == 0xFFFFFFFFu)
        {
            return failWithError(24);
        }

        std::int32_t descriptor;
        std::memcpy(&descriptor, &descriptorBits, sizeof(descriptor));

        const HANDLE handle = ::CreateFileA(fileName, desiredAccess, sharing, &security, creationDisposition, attributes, nullptr);

        const auto failWithLastError = [descriptor] () -> std::int32_t
        {
            const DWORD error = ::GetLastError();
            Runtime_MapOsError(error);
            Runtime_UnlockDescriptor(descriptor);
            return -1;
        };

        if (handle == INVALID_HANDLE_VALUE)
        {
            return failWithLastError();
        }

        const DWORD fileType = ::GetFileType(handle);

        if (fileType == FILE_TYPE_UNKNOWN)
        {
            // Preserve the original order: close first, then obtain the error.
            ::CloseHandle(handle);
            return failWithLastError();
        }

        if (fileType == FILE_TYPE_CHAR)
        {
            descriptorFlags |= 0x40u;
        }
        else if (fileType == FILE_TYPE_PIPE)
        {
            descriptorFlags |= 0x08u;
        }

        Runtime_SetOsHandle(descriptor, handle);

        const std::uint32_t blockIndex = descriptorBits >> 5;
        const std::uint32_t entryOffset = (descriptorBits & 31u) * 36u;

        const auto getEntry = [blockIndex, entryOffset] () -> std::uint8_t*
        {
            return reinterpret_cast<std::uint8_t*>(gRuntimeDescriptorBlocks[blockIndex]) + entryOffset;
        };

        descriptorFlags |= 0x01u;
        getEntry()[4] = descriptorFlags;

        const bool isDeviceOrPipe = (descriptorFlags & 0x48u) != 0;

        const auto closeAndFail = [descriptor] () -> std::int32_t
        {
            Runtime_Close(descriptor);
            Runtime_UnlockDescriptor(descriptor);
            return -1;
        };

        if (!isDeviceOrPipe && (descriptorFlags & 0x80u) != 0 && (flags & 2u) != 0)
        {
            const std::uint32_t endPosition = Runtime_SeekNoLock(descriptor, -1, 2);

            if (endPosition == 0xFFFFFFFFu)
            {
                if (*Runtime_GetDosErrnoPointer() != 0x83u)
                {
                    return closeAndFail();
                }
            }
            else
            {
                std::uint8_t finalByte = 0;
                const std::int32_t readResult = Runtime_ReadNoLock(descriptor, &finalByte, 1);

                // Preserve the original zero-result test.
                if (readResult == 0 && finalByte == 0x1A)
                {
                    const std::int32_t resizeResult = Runtime_ResizeNoLock(descriptor, endPosition);

                    if (resizeResult == -1)
                    {
                        return closeAndFail();
                    }
                }

                const std::uint32_t resetPosition = Runtime_SeekNoLock(descriptor, 0, 0);

                if (resetPosition == 0xFFFFFFFFu)
                {
                    return closeAndFail();
                }
            }
        }

        if (!isDeviceOrPipe && (flags & 0x08u) != 0)
        {
            getEntry()[4] |= 0x20u;
        }

        Runtime_UnlockDescriptor(descriptor);
        return descriptor;
    }

    // Original function: FUN_004e6a84
    std::int32_t Runtime_Read(std::uint32_t descriptor, void* destination, std::uint32_t byteCount)
    {
        if (descriptor < static_cast<std::uint32_t>(gRuntimeDescriptorCapacity))
        {
            const auto* descriptorBlocks = reinterpret_cast<const std::uint8_t*>(&gRuntimeDescriptorBlocks);

            const std::uint8_t* block;
            std::memcpy(&block, descriptorBlocks + (descriptor >> 5) * 4u, sizeof(block));

            const std::uint32_t entryOffset = (descriptor & 0x1Fu) * 0x24u;

            if ((block[entryOffset + 0x04] & 0x01) != 0)
            {
                Runtime_LockDescriptor(descriptor);

                const std::int32_t result = Runtime_ReadNoLock(descriptor, destination, byteCount);

                Runtime_UnlockDescriptor(descriptor);

                return result;
            }
        }

        *Runtime_GetErrnoPointer() = 9;
        *Runtime_GetDosErrnoPointer() = 0;

        return -1;
    }

    // Original function: FUN_004e6ae9
    std::int32_t Runtime_ReadNoLock(std::uint32_t descriptor, void* destination, std::uint32_t byteCount)
    {
        if (byteCount == 0)
        {
            return 0;
        }

        const auto* descriptorBlocks = reinterpret_cast<const std::uint8_t*>(&gRuntimeDescriptorBlocks);
        const std::uint32_t blockOffset = (descriptor >> 5) * 4u;
        const std::uint32_t entryOffset = (descriptor & 0x1Fu) * 0x24u;

        const auto getEntry = [descriptorBlocks, blockOffset, entryOffset] () -> std::uint8_t*
        {
            std::uint8_t* block;
            std::memcpy(&block, descriptorBlocks + blockOffset, sizeof(block));

            return block + entryOffset;
        };

        const auto getHandle = [&getEntry] () -> HANDLE
        {
            HANDLE handle;
            std::memcpy(&handle, getEntry(), sizeof(handle));

            return handle;
        };

        std::uint8_t* entry = getEntry();
        const std::uint8_t initialFlags = entry[0x04];

        if ((initialFlags & 0x02) != 0)
        {
            return 0;
        }

        auto* buffer = static_cast<std::uint8_t*>(destination);
        std::uint8_t* readDestination = buffer;
        std::uint32_t totalBytes = 0;

        if ((initialFlags & 0x48) != 0)
        {
            const std::uint8_t bufferedCharacter = entry[0x05];

            if (bufferedCharacter != '\n')
            {
                --byteCount;
                *readDestination++ = bufferedCharacter;
                totalBytes = 1;

                getEntry()[0x05] = '\n';
            }
        }

        DWORD bytesRead;
        const BOOL readSucceeded = ReadFile(getHandle(), readDestination, byteCount, &bytesRead, nullptr);

        if (readSucceeded == FALSE)
        {
            const DWORD error = GetLastError();

            if (error == ERROR_ACCESS_DENIED)
            {
                *Runtime_GetErrnoPointer() = 9;
                *Runtime_GetDosErrnoPointer() = ERROR_ACCESS_DENIED;
                return -1;
            }

            if (error == ERROR_BROKEN_PIPE)
            {
                return 0;
            }

            Runtime_MapOsError(error);
            return -1;
        }

        totalBytes += bytesRead;

        entry = getEntry();
        std::uint8_t flags = entry[0x04];

        if ((flags & 0x80) == 0)
        {
            std::int32_t result;
            std::memcpy(&result, &totalBytes, sizeof(result));
            return result;
        }

        if (bytesRead != 0 && buffer[0] == '\n')
        {
            flags |= 0x04;
        }
        else
        {
            flags &= 0xFB;
        }

        entry[0x04] = flags;

        if (totalBytes == 0)
        {
            return 0;
        }

        std::uint8_t* input = buffer;
        std::uint8_t* output = buffer;
        std::uint8_t* end = buffer + totalBytes;

        while (input < end)
        {
            const std::uint8_t character = *input;

            if (character == 0x1A)
            {
                entry = getEntry();

                if ((entry[0x04] & 0x40) == 0)
                {
                    entry[0x04] |= 0x02;
                }

                break;
            }

            if (character != '\r')
            {
                *output++ = character;
                ++input;
                continue;
            }

            if (input < end - 1)
            {
                if (input[1] == '\n')
                {
                    input += 2;
                    *output++ = '\n';
                }
                else
                {
                    ++input;
                    *output++ = '\r';
                }

                continue;
            }

            ++input;

            std::uint8_t lookAhead;
            const BOOL lookAheadSucceeded = ReadFile(getHandle(), &lookAhead, 1, &bytesRead, nullptr);

            if (lookAheadSucceeded == FALSE)
            {
                const DWORD error = GetLastError();

                if (error != 0)
                {
                    *output++ = '\r';
                    continue;
                }
            }

            if (bytesRead == 0)
            {
                *output++ = '\r';
                continue;
            }

            entry = getEntry();

            if ((entry[0x04] & 0x48) != 0)
            {
                if (lookAhead == '\n')
                {
                    *output++ = '\n';
                }
                else
                {
                    *output++ = '\r';
                    getEntry()[0x05] = lookAhead;
                }

                continue;
            }

            if (output == buffer && lookAhead == '\n')
            {
                *output++ = '\n';
                continue;
            }

            Runtime_SeekNoLock(descriptor, -1, 1);

            if (lookAhead != '\n')
            {
                *output++ = '\r';
            }
        }

        const std::uint32_t resultBits = static_cast<std::uint32_t>(output - buffer);

        std::int32_t result;
        std::memcpy(&result, &resultBits, sizeof(result));

        return result;
    }

    // Original function: FUN_004e6cc2
    std::uint32_t Runtime_Seek(std::uint32_t descriptor, std::int32_t offset, std::uint32_t origin)
    {
        if (descriptor < static_cast<std::uint32_t>(gRuntimeDescriptorCapacity))
        {
            const auto* descriptorBlocks = reinterpret_cast<const std::uint8_t*>(&gRuntimeDescriptorBlocks);

            const std::uint8_t* block;
            std::memcpy(&block, descriptorBlocks + (descriptor >> 5) * 4u, sizeof(block));

            const std::uint32_t entryOffset = (descriptor & 0x1Fu) * 0x24u;

            if ((block[entryOffset + 0x04] & 0x01) != 0)
            {
                Runtime_LockDescriptor(descriptor);

                const std::uint32_t position = Runtime_SeekNoLock(descriptor, offset, origin);

                Runtime_UnlockDescriptor(descriptor);

                return position;
            }
        }

        *Runtime_GetErrnoPointer() = 9;
        *Runtime_GetDosErrnoPointer() = 0;

        return UINT32_C(0xFFFFFFFF);
    }

    // Original function: FUN_004e6d27
    std::uint32_t Runtime_SeekNoLock(std::uint32_t descriptor, std::int32_t offset, std::uint32_t origin)
    {
        const std::uint32_t handleValue = Runtime_GetOsHandle(descriptor);

        if (handleValue == UINT32_C(0xFFFFFFFF))
        {
            *Runtime_GetErrnoPointer() = 9;
            return UINT32_C(0xFFFFFFFF);
        }

        HANDLE handle = reinterpret_cast<HANDLE>(static_cast<std::uintptr_t>(handleValue));
        const DWORD position = SetFilePointer(handle, static_cast<LONG>(offset), nullptr, static_cast<DWORD>(origin));

        DWORD error = 0;

        if (position == INVALID_SET_FILE_POINTER)
        {
            error = GetLastError();
        }

        if (error != 0)
        {
            Runtime_MapOsError(error);
            return UINT32_C(0xFFFFFFFF);
        }

        const auto* descriptorBlocks = reinterpret_cast<const std::uint8_t*>(&gRuntimeDescriptorBlocks);

        std::uint8_t* block;
        std::memcpy(&block, descriptorBlocks + (descriptor >> 5) * 4u, sizeof(block));

        const std::uint32_t entryOffset = (descriptor & 0x1Fu) * 0x24u;
        block[entryOffset + 0x04] &= 0xFD;

        return position;
    }

    // Original function: FUN_004e6d9a
    std::uint32_t Runtime_GetFileSize(std::uint32_t descriptor)
    {
        if (descriptor < static_cast<std::uint32_t>(gRuntimeDescriptorCapacity))
        {
            const auto* descriptorBlocks = reinterpret_cast<const std::uint8_t*>(&gRuntimeDescriptorBlocks);

            const std::uint8_t* block;
            std::memcpy(&block, descriptorBlocks + (descriptor >> 5) * 4u, sizeof(block));

            const std::uint32_t entryOffset = (descriptor & 0x1Fu) * 0x24u;

            if ((block[entryOffset + 0x04] & 0x01) != 0)
            {
                Runtime_LockDescriptor(descriptor);

                const std::uint32_t originalPosition = Runtime_SeekNoLock(descriptor, 0, 1);
                std::uint32_t size = UINT32_C(0xFFFFFFFF);

                if (originalPosition != UINT32_C(0xFFFFFFFF))
                {
                    size = Runtime_SeekNoLock(descriptor, 0, 2);

                    if (originalPosition != size)
                    {
                        std::int32_t restoreOffset;
                        std::memcpy(&restoreOffset, &originalPosition, sizeof(restoreOffset));

                        Runtime_SeekNoLock(descriptor, restoreOffset, 0);
                    }
                }

                Runtime_UnlockDescriptor(descriptor);

                return size;
            }
        }

        *Runtime_GetErrnoPointer() = 9;
        *Runtime_GetDosErrnoPointer() = 0;

        return UINT32_C(0xFFFFFFFF);
    }

    // Original function: FUN_004e6efc
    // Includes the precision-setting wrapper FUN_004eb923.
    void Runtime_InitializeFloatingPoint()
    {
        Runtime_InitializeFloatCallbacks();

        gRuntimePentiumFdivAdjustment = Runtime_GetPentiumDivisionAdjustment();
        Runtime_ControlFloatingPoint(0x00010000u, 0x00030000u);

        __asm
        {
            fnclex
        }
    }

    // Original function: FUN_004e6f13
    void Runtime_InitializeFloatCallbacks()
    {
        gRuntimeFloatTrimTrailingZerosCallback = &Runtime_FloatTrimTrailingZeros;
        gRuntimeFloatConvertCallback = &Runtime_ConvertFloat;
        gRuntimeFloatAssignCallback = &Runtime_AssignFloat;
        gRuntimeFloatInsertDecimalPointCallback = &Runtime_FloatInsertDecimalPoint;
        gRuntimeFloatReferenceCheckCallback = &Runtime_FloatAtLeastReference;
        gRuntimeFloatConvertAlternateCallback = &Runtime_ConvertFloat;
    }

    // Original function: FUN_004e6f80
    std::int32_t Runtime_CompareStringsIgnoreCaseN(const char* first, const char* second, std::uint32_t count)
    {
        if (count == 0)
        {
            return 0;
        }

        const auto* left = reinterpret_cast<const std::uint8_t*>(first);
        const auto* right = reinterpret_cast<const std::uint8_t*>(second);

        if (gRuntimeCTypeLocale == 0)
        {
            do
            {
                std::uint32_t leftValue = *left;
                std::uint32_t rightValue = *right;

                // The original checks terminators before case conversion.
                if (leftValue == 0 || rightValue == 0)
                {
                    if (leftValue == rightValue)
                    {
                        return 0;
                    }

                    return leftValue < rightValue ? -1 : 1;
                }

                ++left;
                ++right;

                if (leftValue >= 0x41u && leftValue <= 0x5Au)
                {
                    leftValue += 0x20u;
                }

                if (rightValue >= 0x41u && rightValue <= 0x5Au)
                {
                    rightValue += 0x20u;
                }

                if (leftValue != rightValue)
                {
                    return leftValue < rightValue ? -1 : 1;
                }
            } while (--count != 0);

            return 0;
        }

        auto* activeReaders = reinterpret_cast<volatile LONG*>(&gRuntimeLocaleReaderCount);
        const auto* localeChangeState = reinterpret_cast<const volatile LONG*>(&gRuntimeLocaleChangeState);

        ::InterlockedIncrement(activeReaders);

        bool locked = false;

        if (*localeChangeState > 0)
        {
            ::InterlockedDecrement(activeReaders);
            Runtime_Lock(0x13);
            locked = true;
        }

        std::uint32_t leftValue = 0;
        std::uint32_t rightValue = 0;
        std::int32_t result = 0;

        do
        {
            // MOV AL / MOV BL preserve the upper register bytes.
            leftValue = (leftValue & 0xFFFFFF00u) | *left;
            rightValue = (rightValue & 0xFFFFFF00u) | *right;

            // The original tests the full register values.
            if (leftValue == 0 || rightValue == 0)
            {
                if (leftValue != rightValue)
                {
                    result = leftValue < rightValue ? -1 : 1;
                }

                break;
            }

            ++left;
            ++right;

            // Preserve the original conversion order and full-width results.
            rightValue = Runtime_ToLowerNoLock(rightValue);
            leftValue = Runtime_ToLowerNoLock(leftValue);

            if (leftValue != rightValue)
            {
                result = leftValue < rightValue ? -1 : 1;
                break;
            }
        } while (--count != 0);

        if (locked)
        {
            Runtime_Unlock(0x13);
        }
        else
        {
            ::InterlockedDecrement(activeReaders);
        }

        return result;
    }

    // Original function: FUN_004e71a6 (__amsg_exit).
    // The default callback terminates; a replacement callback may return, as in the ASM.
    void Runtime_FatalExit(int errorCode)
    {
        if (gRuntimeErrorOutputMode == 1)
            Runtime_WriteErrorBanner();
        Runtime_WriteErrorMessage(errorCode);
        gRuntimeFatalExitCallback(255);
    }

    // Original function: FUN_004e71cb.
    // Used before locks/TLS are usable; must not dispatch through Runtime_QuickExit.
    void Runtime_StartupFatalExit(int errorCode)
    {
        if (gRuntimeErrorOutputMode == 1)
            Runtime_WriteErrorBanner();
        Runtime_WriteErrorMessage(errorCode);
        ExitProcess(255);
    }

    // Original function: FUN_004e7525
    char* Runtime_GetErrorText(std::int32_t errorCode)
    {
        auto* threadBytes = reinterpret_cast<std::uint8_t*>(Runtime_GetThreadData());

        char* buffer;
        std::memcpy(&buffer, threadBytes + 0x24, sizeof(buffer));

        if (buffer == nullptr)
        {
            buffer = static_cast<char*>(Runtime_Allocate(0x86u));

            // Preserve the null slot after allocation failure.
            // A later call will attempt allocation again.
            std::memcpy(threadBytes + 0x24, &buffer, sizeof(buffer));

            if (buffer == nullptr)
            {
                buffer = gRuntimeFallbackErrorText;
            }
        }

        std::int32_t index = gRuntimeErrorTextCount;

        if (errorCode >= 0 && errorCode < gRuntimeErrorTextCount)
        {
            index = errorCode;
        }

        std::strcpy(buffer, gRuntimeErrorTextTable[index]);
        return buffer;
    }

    // Original function: FUN_004e7575
    void Runtime_MapOsError(std::uint32_t osError)
    {
        *Runtime_GetDosErrnoPointer() = osError;

        for (const auto& entry : gRuntimeErrorMappings)
        {
            if (entry.osError == osError)
            {
                DWORD* errorPointer = Runtime_GetErrnoPointer();
                *errorPointer = entry.crtError;
                return;
            }
        }

        if (osError >= 0x13u && osError <= 0x24u)
        {
            *Runtime_GetErrnoPointer() = 0x0D;
            return;
        }

        if (osError >= 0xBCu && osError <= 0xCAu)
        {
            *Runtime_GetErrnoPointer() = 0x08;
            return;
        }

        *Runtime_GetErrnoPointer() = 0x16;
    }

    // Original function: FUN_004e75e8
    DWORD* Runtime_GetErrnoPointer()
    {
        auto* threadBytes = reinterpret_cast<std::uint8_t*>(Runtime_GetThreadData());
        return reinterpret_cast<DWORD*>(threadBytes + 0x08);
    }

    // Original function: FUN_004e75f1
    DWORD* Runtime_GetDosErrnoPointer()
    {
        auto* threadBytes = reinterpret_cast<std::uint8_t*>(Runtime_GetThreadData());
        return reinterpret_cast<DWORD*>(threadBytes + 0x0C);
    }

    // Original function: FUN_004e75fa
    std::int32_t Runtime_FormatString(char* destination, std::int32_t capacity, const char* format, va_list arguments)
    {
        RuntimeFormatStream stream;

        stream.buffer = destination;
        stream.position = destination;
        stream.remainingBytes = static_cast<std::uint32_t>(capacity);
        stream.flags = 0x42;

        const std::int32_t result = Runtime_FormatToStream(&stream, format, arguments);

        // Preserve the 32-bit decrement and subsequent sign-bit test.
        stream.remainingBytes -= std::uint32_t{ 1 };

        if ((stream.remainingBytes & UINT32_C(0x80000000)) == 0)
        {
            *stream.position = '\0';
        }
        else
        {
            Runtime_FlushAndWriteCharacter(0, &stream);
        }

        return result;
    }

    // Original function: FUN_004e764a
    std::int32_t Runtime_CreateDirectory(const char* path)
    {
        std::uint32_t error = 0;

        if (::CreateDirectoryA(path, nullptr) == FALSE)
        {
            error = ::GetLastError();
        }

        if (error != 0)
        {
            Runtime_MapOsError(error);
            return -1;
        }

        return 0;
    }

    // Original function: FUN_004e7676
    std::int32_t Runtime_FindFirst(const char* pattern, void* result)
    {
        WIN32_FIND_DATAA nativeData;
        const HANDLE handle = ::FindFirstFileA(pattern, &nativeData);

        if (handle == INVALID_HANDLE_VALUE)
        {
            const DWORD error = ::GetLastError();
            DWORD runtimeError = 0x16u;

            if (error == 2u || error == 3u || error == 18u)
            {
                runtimeError = 2u;
            }
            else if (error == 8u)
            {
                runtimeError = 0x0Cu;
            }

            *Runtime_GetErrnoPointer() = runtimeError;
            return -1;
        }

        auto* output = ::new (result) RuntimeFindData;

        // Only the exact FILE_ATTRIBUTE_NORMAL value is converted to zero.
        output->attributes = nativeData.dwFileAttributes == 0x80u ? 0u : nativeData.dwFileAttributes;
        output->creationTime = Runtime_FileTimeToTime32(&nativeData.ftCreationTime);
        output->accessTime = Runtime_FileTimeToTime32(&nativeData.ftLastAccessTime);
        output->writeTime = Runtime_FileTimeToTime32(&nativeData.ftLastWriteTime);
        output->fileSize = nativeData.nFileSizeLow;
        std::strcpy(output->name, nativeData.cFileName);

        return static_cast<std::int32_t>(reinterpret_cast<std::intptr_t>(handle));
    }

    // Original function: FUN_004e7743
    std::int32_t Runtime_FindNext(std::int32_t searchHandle, void* result)
    {
        WIN32_FIND_DATAA nativeData;
        const HANDLE handle = reinterpret_cast<HANDLE>(static_cast<std::intptr_t>(searchHandle));

        if (::FindNextFileA(handle, &nativeData) == 0)
        {
            const DWORD error = ::GetLastError();
            DWORD runtimeError = 0x16u;

            if (error == 2u || error == 3u || error == 18u)
            {
                runtimeError = 2u;
            }
            else if (error == 8u)
            {
                runtimeError = 0x0Cu;
            }

            *Runtime_GetErrnoPointer() = runtimeError;
            return -1;
        }

        auto* output = ::new (result) RuntimeFindData;

        output->attributes = nativeData.dwFileAttributes == 0x80u ? 0u : nativeData.dwFileAttributes;
        output->creationTime = Runtime_FileTimeToTime32(&nativeData.ftCreationTime);
        output->accessTime = Runtime_FileTimeToTime32(&nativeData.ftLastAccessTime);
        output->writeTime = Runtime_FileTimeToTime32(&nativeData.ftLastWriteTime);
        output->fileSize = nativeData.nFileSizeLow;
        std::strcpy(output->name, nativeData.cFileName);

        return 0;
    }

    // Original function: FUN_004e780b
    std::int32_t Runtime_FindClose(std::int32_t searchHandle)
    {
        const HANDLE handle = reinterpret_cast<HANDLE>(static_cast<std::intptr_t>(searchHandle));

        if (::FindClose(handle) == 0)
        {
            *Runtime_GetErrnoPointer() = 0x16u;
            return -1;
        }

        return 0;
    }

    // Original function: FUN_004e782b
    std::int32_t Runtime_FileTimeToTime32(const FILETIME* fileTime)
    {
        if (fileTime->dwLowDateTime == 0 && fileTime->dwHighDateTime == 0)
        {
            return -1;
        }

        FILETIME localFileTime;

        if (::FileTimeToLocalFileTime(fileTime, &localFileTime) == 0)
        {
            return -1;
        }

        SYSTEMTIME localSystemTime;

        if (::FileTimeToSystemTime(&localFileTime, &localSystemTime) == 0)
        {
            return -1;
        }

        // Convert using the reconstructed CRT calendar and automatic DST selection.
        return Runtime_MakeTime32(localSystemTime.wYear, localSystemTime.wMonth, localSystemTime.wDay, localSystemTime.wHour, localSystemTime.wMinute, localSystemTime.wSecond, -1);
    }

    // Original function: FUN_004e788f
    std::int32_t Runtime_RemoveFile(const char* fileName)
    {
        const std::uint32_t error = DeleteFileA(fileName) != FALSE ? 0u : GetLastError();

        if (error != 0)
        {
            Runtime_MapOsError(error);
            return -1;
        }

        return 0;
    }

    // Original function: FUN_004e78b9
    std::int32_t Runtime_ToLower(std::int32_t character)
    {
        if (gRuntimeCTypeLocale == 0)
        {
            if (character >= 0x41 && character <= 0x5A)
            {
                return character + 0x20;
            }

            return character;
        }

        auto* localeUsers = reinterpret_cast<volatile LONG*>(&gRuntimeLocaleReaderCount);
        bool localeLocked = false;

        InterlockedIncrement(localeUsers);

        if (gRuntimeLocaleChangeState != 0)
        {
            InterlockedDecrement(localeUsers);
            Runtime_Lock(0x13);
            localeLocked = true;
        }

        const std::uint32_t resultBits = Runtime_ToLowerNoLock(static_cast<std::uint32_t>(character));

        if (localeLocked)
        {
            Runtime_Unlock(0x13);
        }
        else
        {
            InterlockedDecrement(localeUsers);
        }

        std::int32_t result;
        std::memcpy(&result, &resultBits, sizeof(result));

        return result;
    }

    // Original function: FUN_004e7928
    std::uint32_t Runtime_ToLowerNoLock(std::uint32_t character)
    {
        std::int32_t signedCharacter;
        std::memcpy(&signedCharacter, &character, sizeof(signedCharacter));

        if (gRuntimeCTypeLocale == 0)
        {
            if (signedCharacter >= 0x41 && signedCharacter <= 0x5A)
            {
                return character + 0x20u;
            }

            return character;
        }

        if (signedCharacter < 0x100)
        {
            bool isUpper;

            if (gRuntimeMaxMultibyteLength > 1)
            {
                isUpper = Runtime_ClassifyCharacter(signedCharacter, 1u) != 0;
            }
            else
            {
                const auto* characterTypes = reinterpret_cast<const std::uint8_t*>(gRuntimeCharacterTypes);

                // Preserve signed indexing, including the original EOF case.
                isUpper = (characterTypes[static_cast<std::ptrdiff_t>(signedCharacter) * 2] & 0x01u) != 0;
            }

            if (!isUpper)
            {
                return character;
            }
        }

        const auto* characterTypes = reinterpret_cast<const std::uint8_t*>(gRuntimeCharacterTypes);
        const std::uint32_t highByte = (character >> 8) & 0xFFu;

        char source[3];
        int sourceLength;

        if ((characterTypes[highByte * 2u + 1u] & 0x80u) != 0)
        {
            // Emit the lead byte before the low byte.
            source[0] = static_cast<char>(highByte);
            source[1] = static_cast<char>(character & 0xFFu);
            source[2] = '\0';
            sourceLength = 2;
        }
        else
        {
            source[0] = static_cast<char>(character & 0xFFu);
            source[1] = '\0';
            sourceLength = 1;
        }

        char destination[3];

        const int mappedLength = Runtime_MapString(gRuntimeCTypeLocale, 0x100u, source, sourceLength, destination, 3, 0, 1);

        if (mappedLength == 0)
        {
            return character;
        }

        const auto firstByte = static_cast<std::uint8_t>(destination[0]);

        if (mappedLength == 1)
        {
            return firstByte;
        }

        // Preserve the original output-byte packing.
        const auto secondByte = static_cast<std::uint8_t>(destination[1]);
        return (static_cast<std::uint32_t>(secondByte) << 8) | firstByte;
    }

    // Original function: FUN_004e79f3
    void Runtime_Abort()
    {
        Runtime_WriteErrorMessage(10);
        Runtime_RaiseSignal(22);
        Runtime_QuickExit(3);
    }

    // Original function: FUN_004e7a4b
    int Runtime_CallNewHandler(std::uint32_t allocationSize)
    {
        using NewHandler = int(__cdecl*)(std::uint32_t);

        NewHandler handler;
        std::memcpy(&handler, &gRuntimeNewHandler, sizeof(handler));

        if (handler == nullptr)
        {
            return 0;
        }

        return handler(allocationSize) != 0 ? 1 : 0;
    }

    // Original function: FUN_004e7aa8
    void Runtime_SplitPath(const char* path, char* drive, char* directory, char* baseName, char* extension)
    {
        const char* pathStart = path;

        // The original recognizes a drive solely by the colon at index one.
        if (std::strlen(pathStart) >= 1 && pathStart[1] == ':')
        {
            if (drive != nullptr)
            {
                Runtime_CopyMultibyteBytes(drive, pathStart, 2u);
                drive[2] = '\0';
            }

            pathStart += 2;
        }
        else if (drive != nullptr)
        {
            drive[0] = '\0';
        }

        const char* fileStart = nullptr;
        const char* lastDot = nullptr;
        const char* cursor = pathStart;

        while (*cursor != '\0')
        {
            const std::uint8_t character = static_cast<std::uint8_t>(*cursor);

            if ((gRuntimeMultibyteTypes[character + 1u] & 0x04u) != 0)
            {
                // Preserve the original unconditional skip of the trailing byte.
                ++cursor;
            }
            else if (character == '/' || character == '\\')
            {
                fileStart = cursor + 1;
            }
            else if (character == '.')
            {
                lastDot = cursor;
            }

            ++cursor;
        }

        const char* pathEnd = cursor;

        if (fileStart != nullptr)
        {
            if (directory != nullptr)
            {
                std::uint32_t count = static_cast<std::uint32_t>(fileStart - pathStart);

                if (count >= 0xFFu)
                {
                    count = 0xFFu;
                }

                Runtime_CopyMultibyteBytes(directory, pathStart, count);
                directory[count] = '\0';
            }

            pathStart = fileStart;
        }
        else if (directory != nullptr)
        {
            directory[0] = '\0';
        }

        if (lastDot != nullptr && lastDot >= pathStart)
        {
            if (baseName != nullptr)
            {
                std::uint32_t count = static_cast<std::uint32_t>(lastDot - pathStart);

                if (count >= 0xFFu)
                {
                    count = 0xFFu;
                }

                Runtime_CopyMultibyteBytes(baseName, pathStart, count);
                baseName[count] = '\0';
            }

            if (extension != nullptr)
            {
                std::uint32_t count = static_cast<std::uint32_t>(pathEnd - lastDot);

                if (count >= 0xFFu)
                {
                    count = 0xFFu;
                }

                Runtime_CopyMultibyteBytes(extension, lastDot, count);
                extension[count] = '\0';
            }
        }
        else
        {
            if (baseName != nullptr)
            {
                std::uint32_t count = static_cast<std::uint32_t>(pathEnd - pathStart);

                if (count >= 0xFFu)
                {
                    count = 0xFFu;
                }

                Runtime_CopyMultibyteBytes(baseName, pathStart, count);
                baseName[count] = '\0';
            }

            if (extension != nullptr)
            {
                extension[0] = '\0';
            }
        }
    }

    // Original function: FUN_004e7d12
    std::uint32_t Runtime_ClassifyAlphanumeric(std::int32_t character)
    {
        if (gRuntimeMaxMultibyteLength > 1)
        {
            return Runtime_ClassifyCharacter(character, 0x107u);
        }

        const auto* characterTypes = reinterpret_cast<const std::uint8_t*>(gRuntimeCharacterTypes);

        std::uint16_t flags;
        std::memcpy(&flags, characterTypes + static_cast<std::ptrdiff_t>(character) * 2, sizeof(flags));

        return flags & 0x107u;
    }

    // Original function: FUN_004e7d70
    std::int32_t Runtime_CompareStringsIgnoreCase(const char* first, const char* second)
    {
        const auto* left = reinterpret_cast<const std::uint8_t*>(first);
        const auto* right = reinterpret_cast<const std::uint8_t*>(second);

        if (gRuntimeCTypeLocale == 0)
        {
            for (;;)
            {
                std::uint32_t rightCharacter = *right++;
                std::uint32_t leftCharacter = *left++;

                if (leftCharacter != rightCharacter)
                {
                    if (rightCharacter >= 0x41u && rightCharacter <= 0x5Au)
                    {
                        rightCharacter += 0x20u;
                    }

                    if (leftCharacter >= 0x41u && leftCharacter <= 0x5Au)
                    {
                        leftCharacter += 0x20u;
                    }

                    if (leftCharacter != rightCharacter)
                    {
                        return leftCharacter < rightCharacter ? -1 : 1;
                    }
                }

                if (rightCharacter == 0)
                {
                    return 0;
                }
            }
        }

        auto* activeReaders = reinterpret_cast<volatile LONG*>(&gRuntimeLocaleReaderCount);
        const auto* localeChangeState = reinterpret_cast<const volatile LONG*>(&gRuntimeLocaleChangeState);

        ::InterlockedIncrement(activeReaders);

        bool locked = false;

        if (*localeChangeState > 0)
        {
            ::InterlockedDecrement(activeReaders);
            Runtime_Lock(0x13);
            locked = true;
        }

        // Preserve the register values across byte loads and conversion calls.
        std::uint32_t rightValue = 0xFFu;
        std::uint32_t leftValue = 0;

        while ((rightValue & 0xFFu) != 0)
        {
            rightValue = (rightValue & 0xFFFFFF00u) | *right++;
            leftValue = (leftValue & 0xFFFFFF00u) | *left++;

            if ((rightValue & 0xFFu) == (leftValue & 0xFFu))
            {
                continue;
            }

            // The original converts the first string's character first.
            leftValue = Runtime_ToLowerNoLock(leftValue);
            rightValue = Runtime_ToLowerNoLock(rightValue);

            const std::uint32_t leftByte = leftValue & 0xFFu;
            const std::uint32_t rightByte = rightValue & 0xFFu;

            if (leftByte != rightByte)
            {
                rightValue = leftByte < rightByte ? 0xFFFFFFFFu : 1u;
                break;
            }
        }

        const std::uint32_t resultBits = rightValue;

        if (locked)
        {
            Runtime_Unlock(0x13);
        }
        else
        {
            ::InterlockedDecrement(activeReaders);
        }

        std::int32_t result;
        std::memcpy(&result, &resultBits, sizeof(result));
        return result;
    }

    // Original function: FUN_004e8090
    char* Runtime_LowercaseString(char* text)
    {
        if (gRuntimeCTypeLocale != 0)
        {
            auto* localeUsers = reinterpret_cast<volatile LONG*>(&gRuntimeLocaleReaderCount);
            bool localeLocked = false;

            ::InterlockedIncrement(localeUsers);

            if (gRuntimeLocaleChangeState != 0)
            {
                ::InterlockedDecrement(localeUsers);
                Runtime_Lock(0x13);
                localeLocked = true;
            }

            // The locale may have changed while obtaining access.
            if (gRuntimeCTypeLocale != 0)
            {
                char* converted = nullptr;
                const int requiredBytes = Runtime_MapString(gRuntimeCTypeLocale, 0x100u, text, -1, nullptr, 0, 0, 1);

                if (requiredBytes != 0)
                {
                    converted = static_cast<char*>(Runtime_Allocate(static_cast<std::uint32_t>(requiredBytes)));

                    if (converted != nullptr)
                    {
                        if (Runtime_MapString(gRuntimeCTypeLocale, 0x100u, text, -1, converted, requiredBytes, 0, 1) != 0)
                            std::strcpy(text, converted);
                    }
                }

                if (localeLocked)
                    Runtime_Unlock(0x13);
                else
                    ::InterlockedDecrement(localeUsers);

                Runtime_Free(converted);
                return text;
            }

            if (localeLocked)
                Runtime_Unlock(0x13);
            else
                ::InterlockedDecrement(localeUsers);
        }

        for (char* cursor = text; *cursor != '\0'; ++cursor)
        {
            if (*cursor >= 'A' && *cursor <= 'Z')
                *cursor = static_cast<char>(*cursor + 0x20);
        }

        return text;
    }

    // Original function: FUN_004e8215
    void Runtime_MakePath(char* path, const char* drive, const char* directory, const char* baseName, const char* extension)
    {
        char* output = path;

        if (drive != nullptr && *drive != '\0')
        {
            // The original uses only the first drive character.
            *output++ = *drive;
            *output++ = ':';
        }

        if (directory != nullptr && *directory != '\0')
        {
            const char* cursor = directory;

            while (*cursor != '\0')
            {
                *output++ = *cursor++;
            }

            const char* lastCharacter = Runtime_PreviousMultibyteCharacter(directory, cursor);

            if (*lastCharacter != '/' && *lastCharacter != '\\')
            {
                *output++ = '\\';
            }
        }

        if (baseName != nullptr)
        {
            while (*baseName != '\0')
            {
                *output++ = *baseName++;
            }
        }

        if (extension != nullptr)
        {
            if (*extension != '\0' && *extension != '.')
            {
                *output++ = '.';
            }

            do
            {
                *output++ = *extension;
            } while (*extension++ != '\0');
        }
        else
        {
            *output = '\0';
        }
    }

    // Original function: FUN_004e82a6
    double Runtime_StringToDouble(const char* text)
    {
        for (;;)
        {
            const std::uint8_t character = static_cast<std::uint8_t>(*text);

            if (Runtime_ClassifyCharacter(character, 8u) == 0)
            {
                break;
            }

            ++text;
        }

        // Original helper: FUN_004ed0b7.
        // Only its binary64 result is consumed by FUN_004e82a6.
        std::uint32_t extended[3]{};
        const char* endPointer = nullptr;

        const int parseFlags = Runtime_ParseExtended12(extended, &endPointer, text, 0, 0, 0, 0);

        if ((parseFlags & 4) != 0)
        {
            return 0.0;
        }

        double value;
        Runtime_Extended12ToDouble(extended, &value);

        return value;
    }

    // Original function: FUN_004e8354
    int Runtime_RaiseSignal(std::int32_t signalNumber)
    {
        std::uint32_t* actionSlot = nullptr;
        std::uint8_t* threadBytes = nullptr;
        bool usesGlobalLock = false;

        switch (signalNumber)
        {
            case 2:
                actionSlot = &gRuntimeInterruptSignalAction;
                usesGlobalLock = true;
                break;

            case 4:
            case 8:
            case 11:
                {
                    threadBytes = reinterpret_cast<std::uint8_t*>(Runtime_GetThreadData());

                    RuntimeExceptionAction* actions;
                    std::memcpy(&actions, threadBytes + 0x50, sizeof(actions));

                    actionSlot = &Runtime_FindSignalAction(signalNumber, actions)->action;
                    break;
                }

            case 15:
                actionSlot = &gRuntimeTerminateSignalAction;
                usesGlobalLock = true;
                break;

            case 21:
                actionSlot = &gRuntimeBreakSignalAction;
                usesGlobalLock = true;
                break;

            case 22:
                actionSlot = &gRuntimeAbortSignalAction;
                usesGlobalLock = true;
                break;

            default:
                return -1;
        }

        // Preserve the original action snapshot before acquiring the lock.
        const std::uint32_t action = *actionSlot;

        if (usesGlobalLock)
        {
            Runtime_Lock(1);
        }

        // Action 1 means ignore.
        if (action == 1)
        {
            if (usesGlobalLock)
            {
                Runtime_Unlock(1);
            }

            return 0;
        }

        // Action 0 means the default termination action.
        if (action == 0)
        {
            if (usesGlobalLock)
            {
                Runtime_Unlock(1);
            }

            Runtime_QuickExit(3);
        }

        std::uint32_t savedExceptionInformation = 0;
        std::int32_t savedFloatingPointCode = 0;

        if (signalNumber == 4 || signalNumber == 8 || signalNumber == 11)
        {
            std::memcpy(&savedExceptionInformation, threadBytes + 0x54, sizeof(savedExceptionInformation));

            const std::uint32_t clearedExceptionInformation = 0;
            std::memcpy(threadBytes + 0x54, &clearedExceptionInformation, sizeof(clearedExceptionInformation));

            if (signalNumber == 8)
            {
                std::memcpy(&savedFloatingPointCode, threadBytes + 0x58, sizeof(savedFloatingPointCode));

                const std::int32_t floatingPointCode = 0x8C;
                std::memcpy(threadBytes + 0x58, &floatingPointCode, sizeof(floatingPointCode));
            }
        }

        if (signalNumber == 8)
        {
            for (std::int32_t index = gRuntimeFloatingPointActionBegin; index < gRuntimeFloatingPointActionBegin + gRuntimeFloatingPointActionCount; ++index)
            {
                RuntimeExceptionAction* actions;
                std::memcpy(&actions, threadBytes + 0x50, sizeof(actions));

                actions[index].action = 0;
            }
        }
        else
        {
            *actionSlot = 0;
        }

        if (usesGlobalLock)
        {
            Runtime_Unlock(1);
        }

        if (signalNumber == 8)
        {
            using FloatingPointSignalCallback = void(__cdecl*)(std::int32_t, std::int32_t);

            const auto callback = reinterpret_cast<FloatingPointSignalCallback>(static_cast<std::uintptr_t>(action));

            std::int32_t floatingPointCode;
            std::memcpy(&floatingPointCode, threadBytes + 0x58, sizeof(floatingPointCode));

            callback(8, floatingPointCode);
        }
        else
        {
            using SignalCallback = void(__cdecl*)(std::int32_t);

            const auto callback = reinterpret_cast<SignalCallback>(static_cast<std::uintptr_t>(action));

            callback(signalNumber);
        }

        if (signalNumber == 4 || signalNumber == 8 || signalNumber == 11)
        {
            std::memcpy(threadBytes + 0x54, &savedExceptionInformation, sizeof(savedExceptionInformation));

            if (signalNumber == 8)
            {
                std::memcpy(threadBytes + 0x58, &savedFloatingPointCode, sizeof(savedFloatingPointCode));
            }
        }

        return 0;
    }

    // Original function: FUN_004e84d6
    RuntimeExceptionAction* Runtime_FindSignalAction(std::int32_t signalNumber, RuntimeExceptionAction* actions)
    {
        RuntimeExceptionAction* current = actions;

        if (current->signalNumber != signalNumber)
        {
            RuntimeExceptionAction* end = actions + gRuntimeExceptionActionCount;

            do
            {
                ++current;
            } while (current < end && current->signalNumber != signalNumber);
        }

        if (current >= actions + gRuntimeExceptionActionCount || current->signalNumber != signalNumber)
        {
            return nullptr;
        }

        return current;
    }

    // Original function: FUN_004e8513
    void Runtime_Free(void* memory)
    {
        if (memory == nullptr)
            return;

        const std::uint32_t heapMode = static_cast<std::uint32_t>(gRuntimeHeapMode);

        if (heapMode == 3)
        {
            std::uint32_t heapRecord = 0;

            Runtime_Lock(0x09);

            __try
            {
                heapRecord = Runtime_FindSmallBlockRegion(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(memory)));

                if (heapRecord != 0)
                {
                    Runtime_FreeSmallBlock(heapRecord, memory);
                }
            }
            __finally
            {
                Runtime_Unlock(0x09);
            }

            if (heapRecord != 0)
                return;
        }
        else if (heapMode == 2)
        {
            // Preserve the three DWORD-sized values used by the original.
            std::uint32_t firstHeapValue;
            std::uint32_t secondHeapValue;
            std::uint32_t allocationRecord = 0;

            Runtime_Lock(0x09);

            __try
            {
                allocationRecord = Runtime_FindSmallBlockEntryMode2(memory, &firstHeapValue, &secondHeapValue);

                if (allocationRecord != 0)
                {
                    Runtime_FreeSmallBlockMode2(firstHeapValue, secondHeapValue, allocationRecord);
                }
            }
            __finally
            {
                Runtime_Unlock(0x09);
            }

            if (allocationRecord != 0)
                return;
        }

        // The original ignores HeapFree's return value.
        ::HeapFree(gRuntimeHeap, 0, memory);
    }

    // Original function: FUN_004e85fc
    void* Runtime_Allocate(std::uint32_t requestedBytes)
    {
        return Runtime_AllocateWithNewHandler(requestedBytes, gRuntimeNewHandlerMode);
    }

    // Original function: FUN_004e860e
    void* Runtime_AllocateWithNewHandler(std::uint32_t requestedBytes, std::int32_t useNewHandler)
    {
        if (requestedBytes > 0xFFFFFFE0u)
            return nullptr;
        for (;;)
        {
            void* allocation = Runtime_AllocateHeapBlock(requestedBytes);
            if (allocation != nullptr || useNewHandler == 0)
                return allocation;
            if (Runtime_CallNewHandler(requestedBytes) == 0)
                return nullptr;
        }
    }

    // Original function: FUN_004e863a
    void* Runtime_AllocateHeapBlock(std::uint32_t requestedBytes)
    {
        void* allocation = nullptr;
        if (gRuntimeHeapMode == 3)
        {
            if (requestedBytes <= gRuntimeSmallBlockThreshold)
            {
                Runtime_Lock(0x09);
                __try
                {
                    allocation = Runtime_AllocateSmallBlock(requestedBytes);
                }
                __finally
                {
                    // Original cleanup funclet: 004e86a1.
                    Runtime_Unlock(0x09);
                }
                if (allocation != nullptr)
                    return allocation;
            }
        }
        else if (gRuntimeHeapMode == 2)
        {
            requestedBytes = requestedBytes == 0 ? 16u : (requestedBytes + 15u) & 0xFFFFFFF0u;
            if (requestedBytes <= gRuntimeSmallBlockThresholdMode2)
            {
                Runtime_Lock(0x09);
                __try
                {
                    allocation = Runtime_AllocateSmallBlockMode2(requestedBytes >> 4);
                }
                __finally
                {
                    // Original cleanup funclet: 004e8700.
                    Runtime_Unlock(0x09);
                }
                if (allocation != nullptr)
                    return allocation;
            }
            return HeapAlloc(gRuntimeHeap, 0, requestedBytes);
        }
        const std::uint32_t allocationBytes = ((requestedBytes == 0 ? 1u : requestedBytes) + 15u) & 0xFFFFFFF0u;
        return HeapAlloc(gRuntimeHeap, 0, allocationBytes);
    }

    // Original function: FUN_004e8736
    std::int32_t Runtime_Rand()
    {
        auto* threadBytes = reinterpret_cast<std::uint8_t*>(Runtime_GetThreadData());

        std::uint32_t state;
        std::memcpy(&state, threadBytes + 0x14, sizeof(state));

        state = state * 0x343FDu + 0x269EC3u;
        std::memcpy(threadBytes + 0x14, &state, sizeof(state));

        return static_cast<std::int32_t>((state >> 16) & 0x7FFFu);
    }

    // Original function: FUN_004e89e4
    std::int32_t Runtime_RenameFile(const char* oldFileName, const char* newFileName)
    {
        const std::uint32_t error = MoveFileA(oldFileName, newFileName) != FALSE ? 0u : GetLastError();

        if (error != 0)
        {
            Runtime_MapOsError(error);
            return -1;
        }

        return 0;
    }

    // Original function: FUN_004e8c4a
    std::int32_t Runtime_CloseStream(RuntimeFormatStream* stream)
    {
        if ((stream->flags & 0x40u) != 0)
        {
            stream->flags = 0;
            return -1;
        }

        Runtime_LockStream(stream);
        const std::int32_t result = Runtime_CloseStreamNoLock(stream);
        Runtime_UnlockStream(stream);

        return result;
    }

    // Original function: FUN_004e8c7b
    std::int32_t Runtime_CloseStreamNoLock(RuntimeFormatStream* stream)
    {
        std::int32_t result = -1;

        if ((stream->flags & 0x83u) != 0)
        {
            result = Runtime_FlushStreamBuffer(stream);
            Runtime_FreeStreamBuffer(stream);

            if (Runtime_Close(stream->descriptor) < 0)
            {
                result = -1;
            }
            else if (stream->temporaryFileName != nullptr)
            {
                Runtime_Free(stream->temporaryFileName);
                stream->temporaryFileName = nullptr;
            }
        }

        stream->flags = 0;
        return result;
    }

    // Original function: FUN_004e8cc7
    std::int32_t Runtime_FlushStream(RuntimeFormatStream* stream)
    {
        if (stream == nullptr)
            return Runtime_FlushStreams(0);

        Runtime_LockStream(stream);
        const std::int32_t result = Runtime_FlushStreamNoLock(stream);
        Runtime_UnlockStream(stream);

        return result;
    }

    // Original function: FUN_004e8cf6
    std::int32_t Runtime_FlushStreamNoLock(RuntimeFormatStream* stream)
    {
        if (Runtime_FlushStreamBuffer(stream) != 0)
            return -1;

        if ((stream->flags & 0x4000u) != 0)
            return Runtime_CommitDescriptor(stream->descriptor) != 0 ? -1 : 0;

        return 0;
    }

    // Original function: FUN_004e8d24
    std::int32_t Runtime_FlushStreamBuffer(RuntimeFormatStream* stream)
    {
        std::int32_t result = 0;

        if ((stream->flags & 3u) == 2u && (stream->flags & 0x108u) != 0)
        {
            const std::uint32_t position = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(stream->position));
            const std::uint32_t buffer = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(stream->buffer));
            const std::int32_t byteCount = static_cast<std::int32_t>(position - buffer);

            if (byteCount > 0)
            {
                if (Runtime_Write(stream->descriptor, stream->buffer, static_cast<std::uint32_t>(byteCount)) == byteCount)
                {
                    if ((stream->flags & 0x80u) != 0)
                        stream->flags &= ~0x02u;
                }
                else
                {
                    stream->flags |= 0x20u;
                    result = -1;
                }
            }
        }

        stream->remainingBytes = 0;
        stream->position = stream->buffer;
        return result;
    }

    // Original function: FUN_004e8d89
    std::int32_t Runtime_FlushStreams(std::int32_t mode)
    {
        std::int32_t flushedCount = 0;
        std::int32_t result = 0;

        Runtime_Lock(2);

        for (std::int32_t index = 0; index < gRuntimeStreamCapacity; ++index)
        {
            RuntimeFormatStream* stream = gRuntimeStreamTable[index];

            if (stream == nullptr || (stream->flags & 0x83u) == 0)
                continue;

            Runtime_LockStreamIndex(index, stream);

            stream = gRuntimeStreamTable[index];

            if ((stream->flags & 0x83u) != 0)
            {
                if (mode == 1)
                {
                    if (Runtime_FlushStreamNoLock(stream) != -1)
                        ++flushedCount;
                }
                else if (mode == 0 && (stream->flags & 0x02u) != 0)
                {
                    if (Runtime_FlushStreamNoLock(stream) == -1)
                        result = -1;
                }
            }

            Runtime_UnlockStreamIndex(index, gRuntimeStreamTable[index]);
        }

        Runtime_Unlock(2);
        return mode == 1 ? flushedCount : result;
    }

    // Original function: FUN_004e8e2d
    RuntimeFormatStream* Runtime_OpenStreamShared(const char* fileName, const char* mode, std::uint32_t shareMode)
    {
        RuntimeFormatStream* stream = Runtime_AcquireUnusedStream();

        if (stream == nullptr)
            return nullptr;

        RuntimeFormatStream* result = Runtime_OpenStreamNoLock(fileName, mode, shareMode, stream);

        // Unlock the acquired stream even when opening the descriptor failed.
        Runtime_UnlockStream(stream);

        return result;
    }

    // Original function: FUN_004e8e5e
    RuntimeFormatStream* Runtime_OpenStream(const char* fileName, const char* mode)
    {
        return Runtime_OpenStreamShared(fileName, mode, 0x40u);
    }

    // Original function: FUN_004e8e71
    char* Runtime_GetEnvironmentVariableLocked(const char* name)
    {
        Runtime_Lock(0x0C);
        char* result = Runtime_GetEnvironmentVariable(name);
        Runtime_Unlock(0x0C);

        return result;
    }

    // Original function: FUN_004e8e92.
    char* Runtime_GetEnvironmentVariable(const char* name)
    {
        char** environment = gRuntimeEnvironment;

        if (gRuntimeEnvironmentInitialized == 0)
        {
            return nullptr;
        }

        if (environment == nullptr)
        {
            if (gRuntimeWideEnvironment == nullptr)
            {
                return nullptr;
            }

            if (Runtime_ConvertWideEnvironment() != 0)
            {
                return nullptr;
            }

            environment = gRuntimeEnvironment;

            if (environment == nullptr)
            {
                return nullptr;
            }
        }

        if (name == nullptr)
        {
            return nullptr;
        }

        const std::uint32_t nameLength = static_cast<std::uint32_t>(std::strlen(name));

        for (; *environment != nullptr; ++environment)
        {
            char* entry = *environment;

            if (std::strlen(entry) <= nameLength)
            {
                continue;
            }

            if (entry[nameLength] != '=')
            {
                continue;
            }

            if (Runtime_CompareEnvironmentNames(entry, name, Runtime_TimeSigned32(nameLength)) == 0)
            {
                return entry + nameLength + 1u;
            }
        }

        return nullptr;
    }

    // Original function: FUN_004e8f0f
    char* Runtime_GetLocalTimeString(char* destination)
    {
        SYSTEMTIME time;
        ::GetLocalTime(&time);

        destination[0] = static_cast<char>('0' + time.wHour / 10);
        destination[1] = static_cast<char>('0' + time.wHour % 10);
        destination[2] = ':';
        destination[3] = static_cast<char>('0' + time.wMinute / 10);
        destination[4] = static_cast<char>('0' + time.wMinute % 10);
        destination[5] = ':';
        destination[6] = static_cast<char>('0' + time.wSecond / 10);
        destination[7] = static_cast<char>('0' + time.wSecond % 10);
        destination[8] = '\0';

        return destination;
    }

    // Original function: FUN_004e8f88
    std::int32_t Runtime_PrintStream(RuntimeFormatStream* stream, const char* format, ...)
    {
        va_list arguments;
        va_start(arguments, format);

        Runtime_LockStream(stream);

        const std::int32_t temporaryBufferInstalled = Runtime_BeginTemporaryStreamBuffer(stream);
        const std::int32_t result = Runtime_FormatToStream(stream, format, arguments);

        Runtime_EndTemporaryStreamBuffer(temporaryBufferInstalled, stream);
        Runtime_UnlockStream(stream);

        va_end(arguments);
        return result;
    }

    // Original function: FUN_004e8fc4
    wchar_t* Runtime_DuplicateWideString(const wchar_t* source)
    {
        if (source == nullptr)
        {
            return nullptr;
        }

        std::uint32_t length = 0;

        while (source[length] != L'\0')
        {
            ++length;
        }

        const std::uint32_t allocationSize = length * 2u + 2u;
        auto* duplicate = static_cast<wchar_t*>(Runtime_Allocate(allocationSize));

        if (duplicate == nullptr)
        {
            return nullptr;
        }

        wchar_t* destination = duplicate;

        do
        {
            *destination++ = *source;
        } while (*source++ != L'\0');

        return duplicate;
    }

    // Original function: FUN_004e907b
    char* Runtime_LongToString(std::int32_t value, char* destination, std::int32_t radix)
    {
        char* output = destination;
        std::uint32_t magnitude = static_cast<std::uint32_t>(value);

        if (radix == 10 && value < 0)
        {
            *output++ = '-';

            // Unsigned subtraction preserves the original NEG behavior,
            // including the magnitude of INT32_MIN.
            magnitude = std::uint32_t{ 0 } - magnitude;
        }

        // Original conversion helper: FUN_004e901f.
        // The original does not validate the radix.
        char* firstDigit = output;
        const std::uint32_t base = static_cast<std::uint32_t>(radix);

        do
        {
            const std::uint32_t digit = magnitude % base;
            magnitude /= base;

            *output++ = static_cast<char>(digit + (digit > 9u ? 0x57u : 0x30u));
        } while (magnitude != 0);

        *output = '\0';

        char* lastDigit = output - 1;

        while (firstDigit < lastDigit)
        {
            const char saved = *firstDigit;
            *firstDigit++ = *lastDigit;
            *lastDigit-- = saved;
        }

        return destination;
    }

    // Original function: FUN_004e90fd
    std::uint32_t Runtime_ReadStream(void* destination, std::uint32_t elementSize, std::uint32_t elementCount, RuntimeFormatStream* stream)
    {
        Runtime_LockStream(stream);

        const std::uint32_t result = Runtime_ReadStreamNoLock(destination, elementSize, elementCount, stream);

        Runtime_UnlockStream(stream);
        return result;
    }

    // Original function: FUN_004e912c
    std::uint32_t Runtime_ReadStreamNoLock(void* destination, std::uint32_t elementSize, std::uint32_t elementCount, RuntimeFormatStream* stream)
    {
        const std::uint32_t totalBytes = elementSize * elementCount;

        if (totalBytes == 0)
            return 0;

        auto* output = static_cast<unsigned char*>(destination);
        std::uint32_t remaining = totalBytes;
        std::uint32_t blockSize = (stream->flags & 0x10Cu) != 0 ? stream->bufferSize : 0x1000u;

        while (remaining != 0)
        {
            if ((stream->flags & 0x10Cu) != 0 && stream->remainingBytes != 0)
            {
                const std::uint32_t copiedBytes = remaining < stream->remainingBytes ? remaining : stream->remainingBytes;

                std::memcpy(output, stream->position, copiedBytes);

                remaining -= copiedBytes;
                stream->remainingBytes -= copiedBytes;
                stream->position += copiedBytes;
                output += copiedBytes;
            }
            else if (remaining >= blockSize)
            {
                const std::uint32_t requestedBytes = blockSize != 0 ? remaining - remaining % blockSize : remaining;
                const std::int32_t readResult = Runtime_Read(stream->descriptor, output, requestedBytes);

                if (readResult == 0)
                {
                    stream->flags |= 0x10u;
                    return (totalBytes - remaining) / elementSize;
                }

                if (readResult == -1)
                {
                    stream->flags |= 0x20u;
                    return (totalBytes - remaining) / elementSize;
                }

                const std::uint32_t readBytes = static_cast<std::uint32_t>(readResult);

                remaining -= readBytes;
                output += readBytes;
            }
            else
            {
                const std::int32_t character = Runtime_FillStreamBufferAndReadCharacter(stream);

                if (character == -1)
                    return (totalBytes - remaining) / elementSize;

                *output = static_cast<unsigned char>(character);

                ++output;
                --remaining;

                blockSize = stream->bufferSize;
            }
        }

        return elementCount;
    }

    // Original function: FUN_004e9214
    std::int32_t Runtime_SeekStream(RuntimeFormatStream* stream, std::int32_t offset, std::uint32_t origin)
    {
        Runtime_LockStream(stream);

        const std::int32_t result = Runtime_SeekStreamNoLock(stream, offset, origin);

        Runtime_UnlockStream(stream);
        return result;
    }

    // Original function: FUN_004e9240
    std::int32_t Runtime_SeekStreamNoLock(RuntimeFormatStream* stream, std::int32_t offset, std::uint32_t origin)
    {
        if ((stream->flags & 0x83u) == 0 || origin > 2u)
        {
            *Runtime_GetErrnoPointer() = 22;
            return -1;
        }

        stream->flags &= ~0x10u;

        if (origin == 1u)
        {
            const std::uint32_t position = static_cast<std::uint32_t>(Runtime_TellStreamNoLock(stream));

            // Preserve the original 32-bit addition, including an error result.
            offset = Runtime_TimeSigned32(static_cast<std::uint32_t>(offset) + position);
            origin = 0;
        }

        // The original implementation ignores the flush result.
        Runtime_FlushStreamBuffer(stream);

        if ((stream->flags & 0x80u) != 0)
        {
            stream->flags &= ~3u;
        }
        else if ((stream->flags & 1u) != 0 && (stream->flags & 8u) != 0 && (stream->flags & 0x400u) == 0)
        {
            stream->bufferSize = 0x200u;
        }

        return Runtime_Seek(stream->descriptor, offset, origin) == 0xFFFFFFFFu ? -1 : 0;
    }

    // Original function: FUN_004e92cd
    std::int32_t Runtime_ReadStreamCharacter(RuntimeFormatStream* stream)
    {
        Runtime_LockStream(stream);

        --stream->remainingBytes;

        std::int32_t result;

        if (Runtime_TimeSigned32(stream->remainingBytes) >= 0)
        {
            result = static_cast<unsigned char>(*stream->position);
            ++stream->position;
        }
        else
        {
            result = Runtime_FillStreamBufferAndReadCharacter(stream);
        }

        Runtime_UnlockStream(stream);
        return result;
    }

    // Original function: FUN_004e92fe
    std::uint32_t Runtime_WriteStream(const void* data, std::uint32_t elementSize, std::uint32_t elementCount, RuntimeFormatStream* stream)
    {
        Runtime_LockStream(stream);

        const std::uint32_t result = Runtime_WriteStreamNoLock(data, elementSize, elementCount, stream);

        Runtime_UnlockStream(stream);
        return result;
    }

    // Original function: FUN_004e932d
    std::uint32_t Runtime_WriteStreamNoLock(const void* data, std::uint32_t elementSize, std::uint32_t elementCount, RuntimeFormatStream* stream)
    {
        // Preserve the original unsigned 32-bit multiplication.
        const std::uint32_t totalBytes = elementSize * elementCount;

        if (totalBytes == 0)
            return 0;

        const auto* source = static_cast<const unsigned char*>(data);
        std::uint32_t remaining = totalBytes;
        std::uint32_t blockSize = (stream->flags & 0x10Cu) != 0 ? stream->bufferSize : 0x1000u;

        while (remaining != 0)
        {
            const bool hasBuffer = (stream->flags & 0x108u) != 0;

            if (hasBuffer && stream->remainingBytes != 0)
            {
                const std::uint32_t copiedBytes = remaining < stream->remainingBytes ? remaining : stream->remainingBytes;

                std::memcpy(stream->position, source, copiedBytes);

                stream->remainingBytes -= copiedBytes;
                stream->position += copiedBytes;
                source += copiedBytes;
                remaining -= copiedBytes;
            }
            else if (remaining >= blockSize)
            {
                if (hasBuffer && Runtime_FlushStreamBuffer(stream) != 0)
                    return (totalBytes - remaining) / elementSize;

                const std::uint32_t requestedBytes = blockSize != 0 ? remaining - remaining % blockSize : remaining;
                const std::int32_t writeResult = Runtime_Write(stream->descriptor, source, requestedBytes);

                if (writeResult == -1)
                {
                    stream->flags |= 0x20u;
                    return (totalBytes - remaining) / elementSize;
                }

                const std::uint32_t writtenBytes = static_cast<std::uint32_t>(writeResult);

                source += writtenBytes;
                remaining -= writtenBytes;

                if (writtenBytes < requestedBytes)
                {
                    stream->flags |= 0x20u;
                    return (totalBytes - remaining) / elementSize;
                }
            }
            else
            {
                // The original call sign-extends the source byte.
                const std::int32_t character = *source < 0x80u ? static_cast<std::int32_t>(*source) : static_cast<std::int32_t>(*source) - 0x100;

                if (Runtime_FlushAndWriteCharacter(static_cast<std::uint32_t>(character), stream) == 0xFFFFFFFFu)
                    return (totalBytes - remaining) / elementSize;

                ++source;
                --remaining;

                blockSize = stream->bufferSize;

                if (Runtime_TimeSigned32(blockSize) <= 0)
                    blockSize = 1;
            }
        }

        return elementCount;
    }

    // Original function: FUN_004e9437
    std::int32_t Runtime_WriteStreamCharacter(std::int32_t character, RuntimeFormatStream* stream)
    {
        Runtime_LockStream(stream);

        --stream->remainingBytes;

        std::int32_t result;

        if (Runtime_TimeSigned32(stream->remainingBytes) >= 0)
        {
            const auto value = static_cast<unsigned char>(character);

            *reinterpret_cast<unsigned char*>(stream->position) = value;
            ++stream->position;

            result = value;
        }
        else
        {
            result = Runtime_TimeSigned32(Runtime_FlushAndWriteCharacter(static_cast<std::uint32_t>(character), stream));
        }

        Runtime_UnlockStream(stream);
        return result;
    }

    // Original function: FUN_004e9472
    std::int32_t Runtime_TellStream(RuntimeFormatStream* stream)
    {
        Runtime_LockStream(stream);

        const std::int32_t result = Runtime_TellStreamNoLock(stream);

        Runtime_UnlockStream(stream);
        return result;
    }

    // Original function: FUN_004e9494
    std::int32_t Runtime_TellStreamNoLock(RuntimeFormatStream* stream)
    {
        if (Runtime_TimeSigned32(stream->remainingBytes) < 0)
            stream->remainingBytes = 0;

        const std::uint32_t descriptor = stream->descriptor;
        std::uint32_t filePosition = Runtime_Seek(descriptor, 0, 1);

        if (Runtime_TimeSigned32(filePosition) < 0)
            return -1;

        if ((stream->flags & 0x108u) == 0)
            return Runtime_TimeSigned32(filePosition - stream->remainingBytes);

        const std::uint32_t bufferOffset = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(stream->position) - reinterpret_cast<std::uintptr_t>(stream->buffer));
        std::uint32_t logicalOffset = bufferOffset;

        if ((stream->flags & 3u) != 0)
        {
            const auto* block = reinterpret_cast<const RuntimeDescriptorRecord*>(gRuntimeDescriptorBlocks[descriptor >> 5]);
            const RuntimeDescriptorRecord& record = block[descriptor & 0x1Fu];

            if ((record.flags & 0x80u) != 0)
            {
                for (const char* current = stream->buffer; current < stream->position; ++current)
                {
                    if (*current == '\n')
                        ++logicalOffset;
                }
            }
        }
        else if ((stream->flags & 0x80u) == 0)
        {
            *Runtime_GetErrnoPointer() = 22;
            return -1;
        }

        if (filePosition == 0)
            return Runtime_TimeSigned32(logicalOffset);

        if ((stream->flags & 1u) != 0)
        {
            if (stream->remainingBytes == 0)
            {
                logicalOffset = 0;
            }
            else
            {
                std::uint32_t bufferedBytes = bufferOffset + stream->remainingBytes;
                const auto* block = reinterpret_cast<const RuntimeDescriptorRecord*>(gRuntimeDescriptorBlocks[descriptor >> 5]);
                const RuntimeDescriptorRecord& record = block[descriptor & 0x1Fu];

                if ((record.flags & 0x80u) != 0)
                {
                    const std::uint32_t endPosition = Runtime_Seek(descriptor, 0, 2);

                    if (endPosition == filePosition)
                    {
                        const char* end = stream->buffer + bufferedBytes;

                        for (const char* current = stream->buffer; current < end; ++current)
                        {
                            if (*current == '\n')
                                ++bufferedBytes;
                        }

                        if ((stream->flags & 0x2000u) != 0)
                            ++bufferedBytes;
                    }
                    else
                    {
                        Runtime_Seek(descriptor, Runtime_TimeSigned32(filePosition), 0);

                        if (bufferedBytes <= 0x200u && (stream->flags & 8u) != 0 && (stream->flags & 0x400u) == 0)
                            bufferedBytes = 0x200u;
                        else
                            bufferedBytes = stream->bufferSize;

                        const auto* currentBlock = reinterpret_cast<const RuntimeDescriptorRecord*>(gRuntimeDescriptorBlocks[descriptor >> 5]);

                        if ((currentBlock[descriptor & 0x1Fu].flags & 4u) != 0)
                            ++bufferedBytes;
                    }
                }

                filePosition -= bufferedBytes;
            }
        }

        return Runtime_TimeSigned32(filePosition + logicalOffset);
    }

    // Original function: FUN_004e95f5
    RuntimeFormatStream* Runtime_CreateTemporaryStream()
    {
        Runtime_Lock(3);

        char* fileName = gRuntimeTemporaryStreamFileName;

        if (fileName[0] == '\0')
        {
            Runtime_InitializeTemporaryFileName(1);
        }
        else if (Runtime_AdvanceTemporaryFileName(fileName) != 0)
        {
            Runtime_Unlock(3);
            return nullptr;
        }

        RuntimeFormatStream* stream = Runtime_AcquireUnusedStream();

        if (stream == nullptr)
        {
            Runtime_Unlock(3);
            return nullptr;
        }

        std::int32_t descriptor = Runtime_OpenShared(fileName, 0x8542u, 0x40u, 0x180u);

        while (descriptor == -1)
        {
            if (*Runtime_GetErrnoPointer() != 0x11)
            {
                break;
            }

            if (Runtime_AdvanceTemporaryFileName(fileName) != 0)
            {
                break;
            }

            descriptor = Runtime_OpenShared(fileName, 0x8542u, 0x40u, 0x180u);
        }

        if (descriptor == -1)
        {
            Runtime_UnlockStream(stream);
            Runtime_Unlock(3);
            return nullptr;
        }

        stream->temporaryFileName = Runtime_DuplicateString(fileName);

        if (stream->temporaryFileName == nullptr)
        {
            Runtime_Close(static_cast<std::uint32_t>(descriptor));
            Runtime_UnlockStream(stream);
            Runtime_Unlock(3);
            return nullptr;
        }

        stream->remainingBytes = 0;
        stream->position = nullptr;
        stream->buffer = nullptr;
        stream->flags = gRuntimeDefaultStreamFlags | 0x80u;
        stream->descriptor = static_cast<std::uint32_t>(descriptor);

        Runtime_UnlockStream(stream);
        Runtime_Unlock(3);

        return stream;
    }

    // Original function: FUN_004e96cb
    void Runtime_InitializeTemporaryFileName(std::int32_t temporaryStream)
    {
        char* fileName = temporaryStream != 0 ? gRuntimeTemporaryStreamFileName : gRuntimeTemporaryFileName;

        std::strcpy(fileName, OriginalStrings::WindowsRootPath);

        // Preserve the original fixed offset instead of searching for the terminator.
        char* destination = fileName + 1;

        if (destination[-1] != '\\' && destination[-1] != '/')
        {
            *destination++ = '\\';
        }

        *destination++ = temporaryStream != 0 ? 't' : 's';

        // At radix 32, the existing converter preserves all unsigned input bits.
        Runtime_LongToString(Runtime_TimeSigned32(static_cast<std::uint32_t>(GetCurrentProcessId())), destination, 32);
        std::strcat(fileName, OriginalStrings::Dot);
    }

    // Original function: FUN_004e972c
    std::int32_t Runtime_AdvanceTemporaryFileName(char* fileName)
    {
        const char* separator = Runtime_FindLastMultibyteCharacter(fileName, '.');
        char* suffix = const_cast<char*>(separator) + 1;

        const std::uint32_t nextValue = Runtime_ParseInteger32(suffix, nullptr, 32, 1u) + 1u;

        if (nextValue >= 0x7FFFu)
        {
            return -1;
        }

        char convertedValue[4];

        Runtime_LongToString(static_cast<std::int32_t>(nextValue), convertedValue, 32);
        std::strcpy(suffix, convertedValue);

        return 0;
    }

    // Original function: FUN_004e9772
    std::uint32_t Runtime_FlushAndWriteCharacter(std::uint32_t character, RuntimeFormatStream* stream)
    {
        const std::uint32_t initialFlags = stream->flags;

        if ((initialFlags & 0x82u) == 0 || (initialFlags & 0x40u) != 0)
        {
            stream->flags = initialFlags | 0x20u;
            return UINT32_C(0xFFFFFFFF);
        }

        // Read only after rejecting string streams, whose descriptor
        // is not initialized by Runtime_FormatString.
        const std::uint32_t descriptor = stream->descriptor;

        if ((initialFlags & 0x01u) != 0)
        {
            stream->remainingBytes = 0;

            if ((initialFlags & 0x10u) == 0)
            {
                stream->flags = initialFlags | 0x20u;
                return UINT32_C(0xFFFFFFFF);
            }

            stream->position = stream->buffer;
            stream->flags = initialFlags & ~UINT32_C(0x01);
        }

        stream->remainingBytes = 0;
        std::uint32_t writtenBytes = 0;

        stream->flags = (stream->flags & ~UINT32_C(0x10)) | 0x02u;

        if ((stream->flags & 0x10Cu) == 0)
        {
            bool allocateBuffer = true;

            if (static_cast<const void*>(stream) == static_cast<const void*>(&gRuntimeStdoutStream) || static_cast<const void*>(stream) == static_cast<const void*>(&gRuntimeStderrStream))
            {
                if (Runtime_IsCharacterDevice(descriptor) != 0)
                {
                    allocateBuffer = false;
                }
            }

            if (allocateBuffer)
            {
                Runtime_AllocateStreamBuffer(stream);
            }
        }

        std::uint32_t expectedBytes;

        if ((stream->flags & 0x108u) != 0)
        {
            char* buffer = stream->buffer;

            const std::uint32_t bufferedBytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(stream->position) - reinterpret_cast<std::uintptr_t>(buffer));

            expectedBytes = bufferedBytes;
            stream->position = buffer + 1;
            stream->remainingBytes = stream->bufferSize - std::uint32_t{ 1 };

            // Preserve the signed comparison performed by TEST/JLE.
            if (bufferedBytes != 0 && (bufferedBytes & UINT32_C(0x80000000)) == 0)
            {
                writtenBytes = static_cast<std::uint32_t>(Runtime_Write(descriptor, buffer, bufferedBytes));
            }
            else
            {
                const std::uint8_t* descriptorRecord;

                if (descriptor == UINT32_C(0xFFFFFFFF))
                {
                    descriptorRecord = reinterpret_cast<const std::uint8_t*>(&gRuntimeInvalidDescriptorRecord);
                }
                else
                {
                    // Reproduce the arithmetic right shift used for the block index.
                    std::int32_t signedDescriptor;
                    std::memcpy(&signedDescriptor, &descriptor, sizeof(signedDescriptor));

                    const std::int32_t blockIndex = signedDescriptor >> 5;
                    const auto* descriptorBlocks = reinterpret_cast<const std::byte*>(&gRuntimeDescriptorBlocks);

                    const std::uint8_t* block;
                    std::memcpy(&block, descriptorBlocks + blockIndex * 4, sizeof(block));

                    descriptorRecord = block + (descriptor & 0x1Fu) * 0x24u;
                }

                if ((descriptorRecord[0x04] & 0x20u) != 0)
                {
                    Runtime_Seek(descriptor, 0, 2);
                }
            }

            // Reload the buffer pointer after the write or seek.
            *reinterpret_cast<std::uint8_t*>(stream->buffer) = static_cast<std::uint8_t>(character);
        }
        else
        {
            expectedBytes = 1;
            writtenBytes = static_cast<std::uint32_t>(Runtime_Write(descriptor, &character, 1));
        }

        if (writtenBytes != expectedBytes)
        {
            stream->flags |= 0x20u;
            return UINT32_C(0xFFFFFFFF);
        }

        return character & 0xFFu;
    }

    // Original function: FUN_004e988a
    std::int32_t Runtime_FormatToStream(RuntimeFormatStream* stream, const char* format, va_list arguments)
    {
        const auto asSigned = [] (std::uint32_t bits) -> std::int32_t
        {
            std::int32_t value;
            std::memcpy(&value, &bits, sizeof(value));
            return value;
        };

        const auto signedCharacter = [] (std::uint8_t value) -> std::int32_t
        {
            return value < 0x80 ? static_cast<std::int32_t>(value) : static_cast<std::int32_t>(value) - 0x100;
        };

        // MSVC x86 passes a va_list as a pointer into the argument area.
        va_list argumentCursor = arguments;
        const char* formatPosition = format;

        std::uint32_t state = 0;
        std::int32_t writtenCharacters = 0;

        std::uint32_t flags = 0;
        std::uint32_t width = 0;
        std::uint32_t precision = UINT32_C(0xFFFFFFFF);
        std::uint32_t prefixLength = 0;
        std::uint32_t textLength = 0;

        bool suppressOutput = false;
        bool wideText = false;
        std::int32_t uppercaseFloat = 0;

        char scratch[512];
        char prefix[2];

        // Conversion cases assign this before producing defined output.
        const char* text = nullptr;

        std::uint8_t character = static_cast<std::uint8_t>(*formatPosition++);

        const auto writeLiteral = [&] ()
        {
            wideText = false;

            const auto* classification = reinterpret_cast<const std::uint8_t*>(gRuntimeCharacterTypes);

            if ((classification[static_cast<std::uint32_t>(character) * 2u + 1u] & 0x80u) != 0)
            {
                Runtime_FormatWriteCharacter(static_cast<std::uint32_t>(signedCharacter(character)), stream, &writtenCharacters);
                character = static_cast<std::uint8_t>(*formatPosition++);
            }

            Runtime_FormatWriteCharacter(static_cast<std::uint32_t>(signedCharacter(character)), stream, &writtenCharacters);
        };

        while (character != 0)
        {
            if (writtenCharacters < 0)
            {
                break;
            }

            std::uint32_t characterClass = 0;

            if (character >= 0x20 && character <= 0x78)
            {
                characterClass = gRuntimeFormatTable[character - 0x20] & 0x0Fu;
            }

            state = gRuntimeFormatTable[characterClass * 8u + state] >> 4;

            switch (state)
            {
                case 0:
                    writeLiteral();
                    break;

                case 1:
                    precision = UINT32_C(0xFFFFFFFF);
                    uppercaseFloat = 0;
                    suppressOutput = false;
                    width = 0;
                    prefixLength = 0;
                    flags = 0;
                    wideText = false;
                    break;

                case 2:
                    switch (character)
                    {
                        case ' ':
                            flags |= 0x02u;
                            break;

                        case '#':
                            flags |= 0x80u;
                            break;

                        case '+':
                            flags |= 0x01u;
                            break;

                        case '-':
                            flags |= 0x04u;
                            break;

                        case '0':
                            flags |= 0x08u;
                            break;
                    }
                    break;

                case 3:
                    if (character == '*')
                    {
                        width = Runtime_FormatReadArgument32(&argumentCursor);

                        if (asSigned(width) < 0)
                        {
                            flags |= 0x04u;
                            width = std::uint32_t{ 0 } - width;
                        }
                    }
                    else
                    {
                        width = width * 10u + static_cast<std::uint32_t>(character) - 0x30u;
                    }
                    break;

                case 4:
                    precision = 0;
                    break;

                case 5:
                    if (character == '*')
                    {
                        precision = Runtime_FormatReadArgument32(&argumentCursor);

                        if (asSigned(precision) < 0)
                        {
                            precision = UINT32_C(0xFFFFFFFF);
                        }
                    }
                    else
                    {
                        precision = precision * 10u + static_cast<std::uint32_t>(character) - 0x30u;
                    }
                    break;

                case 6:
                    switch (character)
                    {
                        case 'h':
                            flags |= 0x20u;
                            break;

                        case 'l':
                            flags |= 0x10u;
                            break;

                        case 'w':
                            flags |= 0x800u;
                            break;

                        case 'I':
                            if (formatPosition[0] == '6' && formatPosition[1] == '4')
                            {
                                formatPosition += 2;
                                flags |= 0x8000u;
                            }
                            else
                            {
                                state = 0;
                                writeLiteral();
                            }
                            break;
                    }
                    break;

                case 7:
                    {
                        switch (character)
                        {
                            case 'S':
                            case 's':
                                {
                                    if (character == 'S' && (flags & 0x830u) == 0)
                                    {
                                        flags |= 0x800u;
                                    }

                                    std::uint32_t limit = precision;

                                    if (limit == UINT32_C(0xFFFFFFFF))
                                    {
                                        limit = 0x7FFFFFFFu;
                                    }

                                    const std::uint32_t pointerBits = Runtime_FormatReadArgument32(&argumentCursor);
                                    text = reinterpret_cast<const char*>(static_cast<std::uintptr_t>(pointerBits));

                                    if ((flags & 0x810u) != 0)
                                    {
                                        if (text == nullptr)
                                        {
                                            text = reinterpret_cast<const char*>(gRuntimeNullWideString);
                                        }

                                        wideText = true;
                                        const char* position = text;

                                        while (limit != 0)
                                        {
                                            --limit;

                                            std::uint16_t codeUnit;
                                            std::memcpy(&codeUnit, position, sizeof(codeUnit));

                                            if (codeUnit == 0)
                                            {
                                                break;
                                            }

                                            position += 2;
                                        }

                                        textLength = static_cast<std::uint32_t>((position - text) / 2);
                                    }
                                    else
                                    {
                                        if (text == nullptr)
                                        {
                                            text = reinterpret_cast<const char*>(gRuntimeNullString);
                                        }

                                        const char* position = text;

                                        while (limit != 0)
                                        {
                                            --limit;

                                            if (*position == '\0')
                                            {
                                                break;
                                            }

                                            ++position;
                                        }

                                        textLength = static_cast<std::uint32_t>(position - text);
                                    }
                                    break;
                                }

                            case 'C':
                            case 'c':
                                {
                                    if (character == 'C' && (flags & 0x830u) == 0)
                                    {
                                        flags |= 0x800u;
                                    }

                                    if ((flags & 0x810u) != 0)
                                    {
                                        const std::uint32_t value = Runtime_FormatReadArgument16(&argumentCursor);
                                        const std::int32_t convertedLength = Runtime_WideCharacterToMultibyte(scratch, value);

                                        textLength = static_cast<std::uint32_t>(convertedLength);

                                        if (convertedLength < 0)
                                        {
                                            suppressOutput = true;
                                        }
                                    }
                                    else
                                    {
                                        const std::uint32_t value = Runtime_FormatReadArgument32(&argumentCursor);
                                        const std::uint8_t byteValue = static_cast<std::uint8_t>(value);

                                        std::memcpy(scratch, &byteValue, 1);
                                        textLength = 1;
                                    }

                                    text = scratch;
                                    break;
                                }

                            case 'Z':
                                {
                                    const std::uint32_t pointerBits = Runtime_FormatReadArgument32(&argumentCursor);
                                    const auto* countedString = reinterpret_cast<const std::byte*>(static_cast<std::uintptr_t>(pointerBits));

                                    const char* stringBuffer = nullptr;

                                    if (countedString != nullptr)
                                    {
                                        std::memcpy(&stringBuffer, countedString + 0x04, sizeof(stringBuffer));
                                    }

                                    if (countedString == nullptr || stringBuffer == nullptr)
                                    {
                                        text = reinterpret_cast<const char*>(gRuntimeNullString);
                                        textLength = static_cast<std::uint32_t>(std::strlen(text));
                                    }
                                    else
                                    {
                                        std::int16_t storedLength;
                                        std::memcpy(&storedLength, countedString, sizeof(storedLength));

                                        const std::uint32_t lengthBits = static_cast<std::uint32_t>(static_cast<std::int32_t>(storedLength));

                                        text = stringBuffer;

                                        if ((flags & 0x800u) != 0)
                                        {
                                            textLength = lengthBits >> 1;
                                            wideText = true;
                                        }
                                        else
                                        {
                                            textLength = lengthBits;
                                            wideText = false;
                                        }
                                    }
                                    break;
                                }

                            case 'E':
                            case 'G':
                            case 'e':
                            case 'f':
                            case 'g':
                                {
                                    if (character == 'E' || character == 'G')
                                    {
                                        uppercaseFloat = 1;
                                        character += 0x20;
                                    }

                                    flags |= 0x40u;
                                    text = scratch;

                                    if (asSigned(precision) < 0)
                                    {
                                        precision = 6;
                                    }
                                    else if (precision == 0 && character == 'g')
                                    {
                                        precision = 1;
                                    }

                                    double value;
                                    std::memcpy(&value, argumentCursor, sizeof(value));
                                    argumentCursor += sizeof(value);

                                    using FloatConversion = void(__cdecl*)(const double*, char*, std::int32_t, std::int32_t, std::int32_t);
                                    using FloatAdjustment = void(__cdecl*)(char*);

                                    FloatConversion convertFloat;
                                    std::memcpy(&convertFloat, &gRuntimeFloatConvertCallback, sizeof(convertFloat));

                                    convertFloat(&value, scratch, signedCharacter(character), asSigned(precision), uppercaseFloat);

                                    if ((flags & 0x80u) != 0 && precision == 0)
                                    {
                                        FloatAdjustment adjustDecimalPoint;
                                        std::memcpy(&adjustDecimalPoint, &gRuntimeFloatInsertDecimalPointCallback, sizeof(adjustDecimalPoint));

                                        adjustDecimalPoint(scratch);
                                    }

                                    if (character == 'g' && (flags & 0x80u) == 0)
                                    {
                                        FloatAdjustment adjustGeneralFormat;
                                        std::memcpy(&adjustGeneralFormat, &gRuntimeFloatTrimTrailingZerosCallback, sizeof(adjustGeneralFormat));

                                        adjustGeneralFormat(scratch);
                                    }

                                    if (scratch[0] == '-')
                                    {
                                        flags |= 0x100u;
                                        text = scratch + 1;
                                    }

                                    textLength = static_cast<std::uint32_t>(std::strlen(text));
                                    break;
                                }

                            case 'n':
                                {
                                    const std::uint32_t pointerBits = Runtime_FormatReadArgument32(&argumentCursor);
                                    void* destination = reinterpret_cast<void*>(static_cast<std::uintptr_t>(pointerBits));

                                    if ((flags & 0x20u) != 0)
                                    {
                                        const std::uint16_t count = static_cast<std::uint16_t>(writtenCharacters);
                                        std::memcpy(destination, &count, sizeof(count));
                                    }
                                    else
                                    {
                                        std::memcpy(destination, &writtenCharacters, sizeof(writtenCharacters));
                                    }

                                    suppressOutput = true;
                                    break;
                                }

                            case 'd':
                            case 'i':
                            case 'u':
                            case 'o':
                            case 'p':
                            case 'x':
                            case 'X':
                                {
                                    std::uint32_t radix = 10;
                                    std::uint32_t digitOffset = 0;

                                    if (character == 'd' || character == 'i')
                                    {
                                        flags |= 0x40u;
                                    }
                                    else if (character == 'o')
                                    {
                                        radix = 8;

                                        if ((flags & 0x80u) != 0)
                                        {
                                            flags |= 0x200u;
                                        }
                                    }
                                    else if (character == 'p' || character == 'x' || character == 'X')
                                    {
                                        if (character == 'p')
                                        {
                                            precision = 8;
                                        }

                                        radix = 16;
                                        digitOffset = character == 'x' ? 0x27u : 0x07u;

                                        if ((flags & 0x80u) != 0)
                                        {
                                            prefix[0] = '0';
                                            prefix[1] = static_cast<char>(digitOffset + 0x51u);
                                            prefixLength = 2;
                                        }
                                    }

                                    std::uint64_t value;

                                    if ((flags & 0x8000u) != 0)
                                    {
                                        value = Runtime_FormatReadArgument64(&argumentCursor);
                                    }
                                    else
                                    {
                                        const std::uint32_t argumentBits = Runtime_FormatReadArgument32(&argumentCursor);

                                        if ((flags & 0x20u) != 0)
                                        {
                                            const std::uint16_t lowWord = static_cast<std::uint16_t>(argumentBits);

                                            if ((flags & 0x40u) != 0)
                                            {
                                                std::int16_t signedWord;
                                                std::memcpy(&signedWord, &lowWord, sizeof(signedWord));

                                                value = static_cast<std::uint64_t>(static_cast<std::int64_t>(signedWord));
                                            }
                                            else
                                            {
                                                value = lowWord;
                                            }
                                        }
                                        else if ((flags & 0x40u) != 0)
                                        {
                                            value = static_cast<std::uint64_t>(static_cast<std::int64_t>(asSigned(argumentBits)));
                                        }
                                        else
                                        {
                                            value = argumentBits;
                                        }
                                    }

                                    if ((flags & 0x40u) != 0 && (value & UINT64_C(0x8000000000000000)) != 0)
                                    {
                                        value = std::uint64_t{ 0 } - value;
                                        flags |= 0x100u;
                                    }

                                    if ((flags & 0x8000u) == 0)
                                    {
                                        value &= UINT64_C(0xFFFFFFFF);
                                    }

                                    if (asSigned(precision) < 0)
                                    {
                                        precision = 1;
                                    }
                                    else
                                    {
                                        flags &= ~UINT32_C(0x08);
                                    }

                                    if (value == 0)
                                    {
                                        prefixLength = 0;
                                    }

                                    char* digitPosition = scratch + sizeof(scratch) - 1;

                                    for (;;)
                                    {
                                        const std::uint32_t previousPrecision = precision;
                                        --precision;

                                        if (asSigned(previousPrecision) <= 0 && value == 0)
                                        {
                                            break;
                                        }

                                        std::uint32_t digit = static_cast<std::uint32_t>(value % radix) + 0x30u;
                                        value /= radix;

                                        if (digit > 0x39u)
                                        {
                                            digit += digitOffset;
                                        }

                                        *digitPosition-- = static_cast<char>(digit);
                                    }

                                    textLength = static_cast<std::uint32_t>((scratch + sizeof(scratch) - 1) - digitPosition);
                                    text = digitPosition + 1;

                                    if ((flags & 0x200u) != 0 && (textLength == 0 || *text != '0'))
                                    {
                                        *digitPosition = '0';
                                        text = digitPosition;
                                        ++textLength;
                                    }

                                    break;
                                }

                            default:
                                // Preserve the common output path for unhandled conversion codes.
                                break;
                        }

                        if (suppressOutput)
                        {
                            break;
                        }

                        if ((flags & 0x40u) != 0)
                        {
                            if ((flags & 0x100u) != 0)
                            {
                                prefix[0] = '-';
                                prefixLength = 1;
                            }
                            else if ((flags & 0x01u) != 0)
                            {
                                prefix[0] = '+';
                                prefixLength = 1;
                            }
                            else if ((flags & 0x02u) != 0)
                            {
                                prefix[0] = ' ';
                                prefixLength = 1;
                            }
                        }

                        const std::uint32_t paddingBits = width - prefixLength - textLength;
                        const std::int32_t padding = asSigned(paddingBits);

                        if ((flags & 0x0Cu) == 0)
                        {
                            Runtime_FormatWriteRepeated(' ', padding, stream, &writtenCharacters);
                        }

                        Runtime_FormatWriteText(prefix, asSigned(prefixLength), stream, &writtenCharacters);

                        if ((flags & 0x08u) != 0 && (flags & 0x04u) == 0)
                        {
                            Runtime_FormatWriteRepeated('0', padding, stream, &writtenCharacters);
                        }

                        if (wideText && asSigned(textLength) > 0)
                        {
                            const char* position = text;
                            std::uint32_t remaining = textLength;

                            do
                            {
                                std::uint16_t codeUnit;
                                std::memcpy(&codeUnit, position, sizeof(codeUnit));
                                position += 2;

                                char converted[4];
                                const std::int32_t convertedLength = Runtime_WideCharacterToMultibyte(converted, codeUnit);

                                if (convertedLength <= 0)
                                {
                                    break;
                                }

                                Runtime_FormatWriteText(converted, convertedLength, stream, &writtenCharacters);
                                --remaining;
                            } while (remaining != 0);
                        }
                        else
                        {
                            Runtime_FormatWriteText(text, asSigned(textLength), stream, &writtenCharacters);
                        }

                        if ((flags & 0x04u) != 0)
                        {
                            Runtime_FormatWriteRepeated(' ', padding, stream, &writtenCharacters);
                        }

                        break;
                    }
            }

            character = static_cast<std::uint8_t>(*formatPosition++);
        }

        return writtenCharacters;
    }

    // Original function: FUN_004e9fcb
    void Runtime_FormatWriteCharacter(std::uint32_t character, RuntimeFormatStream* stream, std::int32_t* writtenCharacters)
    {
        stream->remainingBytes -= std::uint32_t{ 1 };

        std::uint32_t result;

        if ((stream->remainingBytes & UINT32_C(0x80000000)) == 0)
        {
            const std::uint8_t byteValue = static_cast<std::uint8_t>(character);

            std::memcpy(stream->position, &byteValue, sizeof(byteValue));
            ++stream->position;

            result = byteValue;
        }
        else
        {
            result = Runtime_FlushAndWriteCharacter(character, stream);
        }

        if (result == UINT32_C(0xFFFFFFFF))
        {
            *writtenCharacters = -1;
            return;
        }

        // Preserve the 32-bit INC, including wraparound.
        std::uint32_t countBits;
        std::memcpy(&countBits, writtenCharacters, sizeof(countBits));

        ++countBits;

        std::memcpy(writtenCharacters, &countBits, sizeof(countBits));
    }

    // Original function: FUN_004ea000
    void Runtime_FormatWriteRepeated(std::uint32_t character, std::int32_t count, RuntimeFormatStream* stream, std::int32_t* writtenCharacters)
    {
        while (count > 0)
        {
            Runtime_FormatWriteCharacter(character, stream, writtenCharacters);

            if (*writtenCharacters == -1)
            {
                return;
            }

            --count;
        }
    }

    // Original function: FUN_004ea031
    void Runtime_FormatWriteText(const char* text, std::int32_t length, RuntimeFormatStream* stream, std::int32_t* writtenCharacters)
    {
        while (length > 0)
        {
            const std::uint8_t byteValue = static_cast<std::uint8_t>(*text++);

            // Original MOVSX sign-extends the source byte.
            const std::int32_t character = byteValue < 0x80 ? static_cast<std::int32_t>(byteValue) : static_cast<std::int32_t>(byteValue) - 0x100;

            Runtime_FormatWriteCharacter(static_cast<std::uint32_t>(character), stream, writtenCharacters);

            if (*writtenCharacters == -1)
            {
                return;
            }

            --length;
        }
    }

    // Original function: FUN_004ea069
    std::uint32_t Runtime_FormatReadArgument32(va_list* arguments)
    {
        *arguments += 4;

        std::uint32_t value;
        std::memcpy(&value, *arguments - 4, sizeof(value));

        return value;
    }

    // Original function: FUN_004ea076
    std::uint64_t Runtime_FormatReadArgument64(va_list* arguments)
    {
        *arguments += 8;

        std::uint64_t value;
        std::memcpy(&value, *arguments - 8, sizeof(value));

        return value;
    }

    // Original function: FUN_004ea086
    std::uint32_t Runtime_FormatReadArgument16(va_list* arguments)
    {
        *arguments += 4;

        const std::uint32_t advancedAddress = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(*arguments));

        std::uint16_t value;
        std::memcpy(&value, *arguments - 4, sizeof(value));

        // MOV AX preserves the upper half of the advanced argument address.
        return (advancedAddress & UINT32_C(0xFFFF0000)) | static_cast<std::uint32_t>(value);
    }

    // Original function: FUN_004ea094
    std::uint32_t Runtime_ClassifyCharacter(std::int32_t character, std::uint32_t mask)
    {
        const std::uint32_t characterBits = static_cast<std::uint32_t>(character);
        const auto* characterTypes = reinterpret_cast<const std::uint8_t*>(gRuntimeCharacterTypes);

        // The unsigned comparison includes EOF (-1) and byte values 0 through 255.
        if (characterBits + 1u <= 0x100u)
        {
            std::uint16_t classification;
            std::memcpy(&classification, characterTypes + static_cast<std::ptrdiff_t>(character) * 2, sizeof(classification));

            return static_cast<std::uint32_t>(classification) & mask;
        }

        const std::uint8_t highByte = static_cast<std::uint8_t>(characterBits >> 8);
        const std::uint8_t lowByte = static_cast<std::uint8_t>(characterBits);

        char source[3];
        std::int32_t sourceLength;

        if ((characterTypes[static_cast<std::uint32_t>(highByte) * 2u + 1u] & 0x80u) != 0)
        {
            source[0] = static_cast<char>(highByte);
            source[1] = static_cast<char>(lowByte);
            source[2] = '\0';
            sourceLength = 2;
        }
        else
        {
            source[0] = static_cast<char>(lowByte);
            source[1] = '\0';
            sourceLength = 1;
        }

        // The original uses the upper word of its character argument as output storage.
        std::uint16_t classification = static_cast<std::uint16_t>(characterBits >> 16);

        if (Runtime_GetStringType(1, source, sourceLength, &classification, 0, 0, 1) == FALSE)
            return 0;

        return static_cast<std::uint32_t>(classification) & mask;
    }

    // Original function: FUN_004ea109.
    // The original application initializer range is 004fe000..004fe058.
    // Supply translated application callbacks, never original executable addresses.
    void Runtime_InitializeCrt(RuntimeVoidCallback* applicationBegin, RuntimeVoidCallback* applicationEnd)
    {
        if (gRuntimeFloatingPointInitializer != nullptr)
            gRuntimeFloatingPointInitializer();

        // Original C initializer range: 004fe05c..004fe06c.
        Runtime_InitializeOnExitTable();
        Runtime_InitializeMultibyte();
        Runtime_InitializeStreams();

        Runtime_CallInitializers(applicationBegin, applicationEnd);
    }

    // Original function: FUN_004ea136
    void Runtime_Exit(std::int32_t exitCode)
    {
        Runtime_PerformExit(exitCode, 0, 0);
    }

    // Original function: FUN_004ea147
    void Runtime_QuickExit(int exitCode)
    {
        Runtime_PerformExit(exitCode, 1, 0);
    }

    // Original function: FUN_004ea158
    void Runtime_PerformExit(std::int32_t exitCode, std::int32_t quickExit, std::int32_t returnToCaller)
    {
        // Original lock/unlock helpers: FUN_004ea1fd and FUN_004ea206.
        Runtime_Lock(13);

        if (gRuntimeProcessExitStarted == 1)
            ::TerminateProcess(::GetCurrentProcess(), static_cast<UINT>(exitCode));

        gRuntimeCleanupStarted = 1;
        gRuntimeCleanupReturnsToCaller = static_cast<unsigned char>(returnToCaller);

        if (quickExit == 0)
        {
            if (gRuntimeOnExitBegin != nullptr)
            {
                // Preserve reverse traversal and reloading of the lower bound.
                std::uintptr_t cursor = reinterpret_cast<std::uintptr_t>(gRuntimeOnExitEnd);
                cursor -= sizeof(RuntimeOnExitCallback);

                while (cursor >= reinterpret_cast<std::uintptr_t>(gRuntimeOnExitBegin))
                {
                    RuntimeOnExitCallback callback;
                    std::memcpy(&callback, reinterpret_cast<const void*>(cursor), sizeof(callback));

                    if (callback != nullptr)
                        callback();

                    cursor -= sizeof(RuntimeOnExitCallback);
                }
            }

            Runtime_CallInitializers(gRuntimePreTerminators, gRuntimePreTerminators + 3);
        }

        Runtime_CallInitializers(gRuntimeTerminators, gRuntimeTerminators + 1);

        if (returnToCaller != 0)
        {
            Runtime_Unlock(13);
            return;
        }

        gRuntimeProcessExitStarted = 1;
        ::ExitProcess(static_cast<UINT>(exitCode));
    }

    // Original function: FUN_004ea20f.
    void Runtime_CallInitializers(RuntimeVoidCallback* begin, RuntimeVoidCallback* end)
    {
        // Preserve the original unsigned address comparison.
        for (; reinterpret_cast<std::uintptr_t>(begin) < reinterpret_cast<std::uintptr_t>(end); ++begin)
        {
            if (*begin != nullptr)
                (*begin)();
        }
    }

    // Original function: FUN_004ea229
    void* Runtime_Reallocate(void* memory, std::uint32_t requestedBytes)
    {
        if (memory == nullptr)
            return Runtime_Allocate(requestedBytes);

        if (requestedBytes == 0)
        {
            Runtime_Free(memory);
            return nullptr;
        }

        const std::uint32_t heapMode = gRuntimeHeapMode;

        if (heapMode == 2 && requestedBytes <= 0xFFFFFFE0u)
            requestedBytes = (requestedBytes + 0x0Fu) & ~0x0Fu;

        for (;;)
        {
            void* replacement = nullptr;

            if (requestedBytes <= 0xFFFFFFE0u)
            {
                if (heapMode == 3)
                {
                    std::uint32_t regionAddress = 0;

                    Runtime_Lock(9);
                    __try
                    {
                        regionAddress = Runtime_FindSmallBlockRegion(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(memory)));

                        if (regionAddress != 0)
                        {
                            if (requestedBytes <= gRuntimeSmallBlockThreshold)
                            {
                                if (Runtime_ResizeSmallBlock(regionAddress, memory, requestedBytes))
                                {
                                    replacement = memory;
                                }
                                else
                                {
                                    replacement = Runtime_AllocateSmallBlock(requestedBytes);

                                    if (replacement != nullptr)
                                    {
                                        // Preserve the original header-minus-one copy bound.
                                        const std::uint32_t oldSize = *(static_cast<std::uint32_t*>(memory) - 1) - 1u;
                                        const std::uint32_t copySize = oldSize < requestedBytes ? oldSize : requestedBytes;

                                        std::memcpy(replacement, memory, copySize);

                                        // Allocation may have moved the region table.
                                        regionAddress = Runtime_FindSmallBlockRegion(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(memory)));

                                        Runtime_FreeSmallBlock(regionAddress, memory);
                                    }
                                }
                            }

                            if (replacement == nullptr)
                            {
                                requestedBytes = (requestedBytes + 0x0Fu) & ~0x0Fu;
                                replacement = HeapAlloc(gRuntimeHeap, 0, requestedBytes);

                                if (replacement != nullptr)
                                {
                                    const std::uint32_t oldSize = *(static_cast<std::uint32_t*>(memory) - 1) - 1u;
                                    const std::uint32_t copySize = oldSize < requestedBytes ? oldSize : requestedBytes;

                                    std::memcpy(replacement, memory, copySize);
                                    Runtime_FreeSmallBlock(regionAddress, memory);
                                }
                            }
                        }
                    }
                    __finally
                    {
                        Runtime_Unlock(9);
                    }

                    if (regionAddress == 0)
                    {
                        requestedBytes = (requestedBytes + 0x0Fu) & ~0x0Fu;
                        replacement = HeapReAlloc(gRuntimeHeap, 0, memory, requestedBytes);
                    }
                }
                else if (heapMode == 2)
                {
                    Runtime_Lock(9);
                    __try
                    {
                        std::uint32_t regionAddress = 0;
                        std::uint32_t pageAddress = 0;

                        const std::uint32_t entryAddress = Runtime_FindSmallBlockEntryMode2(memory, &regionAddress, &pageAddress);

                        if (entryAddress != 0)
                        {
                            // The original comparison is strictly less than.
                            if (requestedBytes < gRuntimeSmallBlockThresholdMode2)
                            {
                                const std::uint32_t requestedUnits = requestedBytes >> 4;

                                if (Runtime_ResizeSmallBlockMode2(regionAddress, pageAddress, entryAddress, requestedUnits))
                                {
                                    replacement = memory;
                                }
                                else
                                {
                                    replacement = Runtime_AllocateSmallBlockMode2(requestedUnits);

                                    if (replacement != nullptr)
                                    {
                                        const std::uint32_t oldSize = static_cast<std::uint32_t>(*reinterpret_cast<std::uint8_t*>(entryAddress)) << 4;
                                        const std::uint32_t copySize = oldSize < requestedBytes ? oldSize : requestedBytes;

                                        std::memcpy(replacement, memory, copySize);
                                        Runtime_FreeSmallBlockMode2(regionAddress, pageAddress, entryAddress);
                                    }
                                }
                            }

                            if (replacement == nullptr)
                            {
                                replacement = HeapAlloc(gRuntimeHeap, 0, requestedBytes);

                                if (replacement != nullptr)
                                {
                                    const std::uint32_t oldSize = static_cast<std::uint32_t>(*reinterpret_cast<std::uint8_t*>(entryAddress)) << 4;
                                    const std::uint32_t copySize = oldSize < requestedBytes ? oldSize : requestedBytes;

                                    std::memcpy(replacement, memory, copySize);
                                    Runtime_FreeSmallBlockMode2(regionAddress, pageAddress, entryAddress);
                                }
                            }
                        }
                        else
                        {
                            replacement = HeapReAlloc(gRuntimeHeap, 0, memory, requestedBytes);
                        }
                    }
                    __finally
                    {
                        Runtime_Unlock(9);
                    }
                }
                else
                {
                    requestedBytes = (requestedBytes + 0x0Fu) & ~0x0Fu;
                    replacement = HeapReAlloc(gRuntimeHeap, 0, memory, requestedBytes);
                }
            }

            if (replacement != nullptr)
                return replacement;

            if (gRuntimeNewHandlerMode == 0 || Runtime_CallNewHandler(requestedBytes) == 0)
                return nullptr;
        }
    }

    // Original function: FUN_004ea558
    std::uint32_t Runtime_GetAllocationSize(void* memory)
    {
        if (gRuntimeHeapMode == 3u)
        {
            std::uint32_t regionAddress = 0;
            std::uint32_t allocationSize = 0;

            Runtime_Lock(9);

            __try
            {
                const auto memoryAddress = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(memory));
                regionAddress = Runtime_FindSmallBlockRegion(memoryAddress);

                if (regionAddress != 0)
                {
                    std::uint32_t blockHeader;
                    std::memcpy(&blockHeader, static_cast<const std::uint8_t*>(memory) - 4, sizeof(blockHeader));

                    // Remove the allocation bit and both four-byte boundary tags.
                    allocationSize = blockHeader - 9u;
                }
            }
            __finally
            {
                // Original cleanup helper: FUN_004ea5c2.
                Runtime_Unlock(9);
            }

            if (regionAddress != 0)
                return allocationSize;
        }
        else if (gRuntimeHeapMode == 2u)
        {
            std::uint32_t entryAddress = 0;
            std::uint32_t allocationSize = 0;

            Runtime_Lock(9);

            __try
            {
                std::uint32_t regionAddress;
                std::uint32_t pageAddress;

                entryAddress = Runtime_FindSmallBlockEntryMode2(memory, &regionAddress, &pageAddress);

                if (entryAddress != 0)
                {
                    const auto* entry = reinterpret_cast<const std::uint8_t*>(static_cast<std::uintptr_t>(entryAddress));
                    allocationSize = static_cast<std::uint32_t>(*entry) << 4;
                }
            }
            __finally
            {
                // Original cleanup helper: FUN_004ea63d.
                Runtime_Unlock(9);
            }

            if (entryAddress != 0)
                return allocationSize;
        }

        return static_cast<std::uint32_t>(::HeapSize(gRuntimeHeap, 0, memory));
    }

    // Original function: FUN_004ea646
    std::uint32_t Runtime_AllocateDescriptor()
    {
        Runtime_Lock(0x12);

        auto* descriptorBlocks = reinterpret_cast<std::uint8_t*>(&gRuntimeDescriptorBlocks);
        std::uint32_t result = 0xFFFFFFFFu;

        const auto readBlock = [descriptorBlocks] (std::uint32_t index) -> std::uint8_t*
        {
            std::uint8_t* block;
            std::memcpy(&block, descriptorBlocks + index * 4u, sizeof(block));
            return block;
        };

        for (std::uint32_t blockIndex = 0; blockIndex < 64u; ++blockIndex)
        {
            std::uint8_t* block = readBlock(blockIndex);

            if (block == nullptr)
            {
                block = static_cast<std::uint8_t*>(Runtime_Allocate(0x480));

                if (block == nullptr)
                    break;

                std::uint32_t descriptorCount;
                std::memcpy(&descriptorCount, &gRuntimeDescriptorCapacity, sizeof(descriptorCount));
                descriptorCount += 32u;
                std::memcpy(&gRuntimeDescriptorCapacity, &descriptorCount, sizeof(descriptorCount));

                std::memcpy(descriptorBlocks + blockIndex * 4u, &block, sizeof(block));

                const std::uint32_t invalidHandle = 0xFFFFFFFFu;
                const std::uint32_t uninitialized = 0;

                for (std::uint32_t index = 0; index < 32u; ++index)
                {
                    std::uint8_t* entry = block + index * 0x24u;

                    entry[0x04] = 0;
                    std::memcpy(entry, &invalidHandle, sizeof(invalidHandle));
                    std::memcpy(entry + 0x08, &uninitialized, sizeof(uninitialized));
                    entry[0x05] = '\n';
                }

                result = blockIndex * 32u;
                Runtime_LockDescriptor(result);
                break;
            }

            std::uint8_t* entry = block;

            while (entry < readBlock(blockIndex) + 0x480u)
            {
                if ((entry[0x04] & 0x01u) == 0)
                {
                    auto* initialized = reinterpret_cast<volatile std::uint32_t*>(entry + 0x08);
                    auto* criticalSection = reinterpret_cast<CRITICAL_SECTION*>(entry + 0x0C);

                    if (*initialized == 0)
                    {
                        Runtime_Lock(0x11);

                        if (*initialized == 0)
                        {
                            ::InitializeCriticalSection(criticalSection);
                            *initialized = *initialized + 1u;
                        }

                        Runtime_Unlock(0x11);
                    }

                    ::EnterCriticalSection(criticalSection);

                    // Another thread may have opened the descriptor before this lock was acquired.
                    if ((entry[0x04] & 0x01u) == 0)
                    {
                        const std::uint32_t invalidHandle = 0xFFFFFFFFu;
                        std::memcpy(entry, &invalidHandle, sizeof(invalidHandle));

                        const std::ptrdiff_t entryIndex = (entry - readBlock(blockIndex)) / 0x24;
                        result = blockIndex * 32u + static_cast<std::uint32_t>(entryIndex);
                        break;
                    }

                    ::LeaveCriticalSection(criticalSection);
                }

                entry += 0x24;
            }

            if (result != 0xFFFFFFFFu)
                break;
        }

        // A successfully allocated descriptor remains locked for the caller.
        Runtime_Unlock(0x12);
        return result;
    }

    // Original function: FUN_004ea769
    std::int32_t Runtime_SetOsHandle(std::uint32_t descriptor, HANDLE handle)
    {
        if (descriptor < static_cast<std::uint32_t>(gRuntimeDescriptorCapacity))
        {
            std::int32_t signedDescriptor;
            std::memcpy(&signedDescriptor, &descriptor, sizeof(signedDescriptor));

            const std::int32_t blockIndex = signedDescriptor >> 5;
            const auto* descriptorBlocks = reinterpret_cast<const std::uint8_t*>(&gRuntimeDescriptorBlocks);
            const std::uint32_t entryOffset = (descriptor & 0x1Fu) * 0x24u;

            std::uint8_t* block;
            std::memcpy(&block, descriptorBlocks + static_cast<std::ptrdiff_t>(blockIndex) * 4, sizeof(block));

            std::uint32_t previousHandle;
            std::memcpy(&previousHandle, block + entryOffset, sizeof(previousHandle));

            if (previousHandle == 0xFFFFFFFFu)
            {
                if (gRuntimeApplicationType == 1)
                {
                    switch (descriptor)
                    {
                        case 0:
                            ::SetStdHandle(STD_INPUT_HANDLE, handle);
                            break;

                        case 1:
                            ::SetStdHandle(STD_OUTPUT_HANDLE, handle);
                            break;

                        case 2:
                            ::SetStdHandle(STD_ERROR_HANDLE, handle);
                            break;
                    }
                }

                // Reload the block pointer after the possible Windows call.
                std::memcpy(&block, descriptorBlocks + static_cast<std::ptrdiff_t>(blockIndex) * 4, sizeof(block));
                std::memcpy(block + entryOffset, &handle, sizeof(handle));

                return 0;
            }
        }

        *Runtime_GetErrnoPointer() = 9;
        *Runtime_GetDosErrnoPointer() = 0;
        return -1;
    }

    // Original function: FUN_004ea7e5
    std::int32_t Runtime_ClearOsHandle(std::uint32_t descriptor)
    {
        if (descriptor < static_cast<std::uint32_t>(gRuntimeDescriptorCapacity))
        {
            const std::uint32_t blockIndex = descriptor >> 5;
            const std::uint32_t entryOffset = (descriptor & 31u) * 36u;
            auto* block = reinterpret_cast<std::uint8_t*>(gRuntimeDescriptorBlocks[blockIndex]);

            if ((block[entryOffset + 4u] & 0x01u) != 0)
            {
                std::uint32_t handleValue;
                std::memcpy(&handleValue, block + entryOffset, sizeof(handleValue));

                if (handleValue != 0xFFFFFFFFu)
                {
                    if (gRuntimeApplicationType == 1)
                    {
                        // The original ignores the return value of SetStdHandle.
                        switch (descriptor)
                        {
                            case 0u:
                                ::SetStdHandle(STD_INPUT_HANDLE, nullptr);
                                break;

                            case 1u:
                                ::SetStdHandle(STD_OUTPUT_HANDLE, nullptr);
                                break;

                            case 2u:
                                ::SetStdHandle(STD_ERROR_HANDLE, nullptr);
                                break;
                        }
                    }

                    // Reload the block pointer after the possible Windows call.
                    block = reinterpret_cast<std::uint8_t*>(gRuntimeDescriptorBlocks[blockIndex]);

                    const std::uint32_t invalidHandle = 0xFFFFFFFFu;
                    std::memcpy(block + entryOffset, &invalidHandle, sizeof(invalidHandle));
                    return 0;
                }
            }
        }

        *Runtime_GetErrnoPointer() = 9;
        *Runtime_GetDosErrnoPointer() = 0;
        return -1;
    }

    // Original function: FUN_004ea864
    std::uint32_t Runtime_GetOsHandle(std::uint32_t descriptor)
    {
        if (descriptor < static_cast<std::uint32_t>(gRuntimeDescriptorCapacity))
        {
            const auto* block = reinterpret_cast<const std::uint8_t*>(gRuntimeDescriptorBlocks[descriptor >> 5]);
            const std::uint32_t entryOffset = (descriptor & 31u) * 36u;

            if ((block[entryOffset + 4u] & 0x01u) != 0)
            {
                std::uint32_t handleValue;
                std::memcpy(&handleValue, block + entryOffset, sizeof(handleValue));
                return handleValue;
            }
        }

        *Runtime_GetErrnoPointer() = 9;
        *Runtime_GetDosErrnoPointer() = 0;
        return 0xFFFFFFFFu;
    }

    // Original function: FUN_004ea8a6
    void Runtime_LockDescriptor(std::uint32_t descriptor)
    {
        const std::uint32_t blockIndex = descriptor >> 5;
        const std::uint32_t entryOffset = (descriptor & 31u) * 36u;

        auto* block = reinterpret_cast<std::uint8_t*>(gRuntimeDescriptorBlocks[blockIndex]);
        auto* entry = block + entryOffset;
        auto* initialized = reinterpret_cast<volatile std::uint32_t*>(entry + 0x08);

        if (*initialized == 0)
        {
            Runtime_Lock(0x11);

            // Recheck the original entry after acquiring the global CRT lock.
            if (*initialized == 0)
            {
                auto* criticalSection = reinterpret_cast<CRITICAL_SECTION*>(entry + 0x0C);
                ::InitializeCriticalSection(criticalSection);

                *initialized = *initialized + 1u;
            }

            Runtime_Unlock(0x11);
        }

        // The original reloads the block pointer before entering the section.
        block = reinterpret_cast<std::uint8_t*>(gRuntimeDescriptorBlocks[blockIndex]);
        auto* criticalSection = reinterpret_cast<CRITICAL_SECTION*>(block + entryOffset + 0x0C);

        ::EnterCriticalSection(criticalSection);
    }

    // Original function: FUN_004ea905
    void Runtime_UnlockDescriptor(std::uint32_t descriptor)
    {
        const std::uint32_t blockIndex = descriptor >> 5;
        const std::uint32_t entryOffset = (descriptor & 31u) * 36u;

        auto* block = reinterpret_cast<std::uint8_t*>(gRuntimeDescriptorBlocks[blockIndex]);
        auto* criticalSection = reinterpret_cast<CRITICAL_SECTION*>(block + entryOffset + 0x0C);

        ::LeaveCriticalSection(criticalSection);
    }

    // Original function: FUN_004ea927
    void Runtime_InitializeDescriptors()
    {
        auto* firstBlock = static_cast<RuntimeDescriptorRecord*>(Runtime_Allocate(0x480u));

        if (firstBlock == nullptr)
            Runtime_FatalExit(0x1B);

        gRuntimeDescriptorBlocks[0] = reinterpret_cast<std::uint8_t*>(firstBlock);
        gRuntimeDescriptorCapacity = 32u;

        for (std::uint32_t index = 0; index < 32u; ++index)
        {
            firstBlock[index].osHandle = 0xFFFFFFFFu;
            firstBlock[index].flags = 0;
            firstBlock[index].lookaheadCharacter = '\n';
            firstBlock[index].lockInitialized = 0;
        }

        STARTUPINFOA startupInfo;
        ::GetStartupInfoA(&startupInfo);

        if (startupInfo.cbReserved2 != 0 && startupInfo.lpReserved2 != nullptr)
        {
            std::int32_t inheritedCount;
            std::memcpy(&inheritedCount, startupInfo.lpReserved2, sizeof(inheritedCount));

            const std::uint8_t* inheritedFlags = startupInfo.lpReserved2 + sizeof(inheritedCount);

            // The handle array follows the original number of flag bytes,
            // before the descriptor count is limited to this runtime's capacity.
            const std::uint8_t* inheritedHandles = inheritedFlags + inheritedCount;

            if (inheritedCount >= 2048)
                inheritedCount = 2048;

            std::uint32_t blockIndex = 1;

            while (static_cast<std::int32_t>(gRuntimeDescriptorCapacity) < inheritedCount)
            {
                auto* block = static_cast<RuntimeDescriptorRecord*>(Runtime_Allocate(0x480u));

                if (block == nullptr)
                {
                    inheritedCount = static_cast<std::int32_t>(gRuntimeDescriptorCapacity);
                    break;
                }

                gRuntimeDescriptorCapacity += 32u;
                gRuntimeDescriptorBlocks[blockIndex] = reinterpret_cast<std::uint8_t*>(block);

                for (std::uint32_t index = 0; index < 32u; ++index)
                {
                    block[index].osHandle = 0xFFFFFFFFu;
                    block[index].flags = 0;
                    block[index].lookaheadCharacter = '\n';
                    block[index].lockInitialized = 0;
                }

                ++blockIndex;
            }

            for (std::int32_t descriptor = 0; descriptor < inheritedCount; ++descriptor)
            {
                std::uint32_t handleBits;
                std::memcpy(&handleBits, inheritedHandles + static_cast<std::size_t>(descriptor) * sizeof(handleBits), sizeof(handleBits));

                const std::uint8_t flags = inheritedFlags[descriptor];

                if (handleBits == 0xFFFFFFFFu || (flags & 0x01u) == 0)
                    continue;

                // Inherited pipe handles bypass GetFileType in the original.
                if ((flags & 0x08u) == 0)
                {
                    HANDLE handle = reinterpret_cast<HANDLE>(static_cast<std::uintptr_t>(handleBits));

                    if (::GetFileType(handle) == FILE_TYPE_UNKNOWN)
                        continue;
                }

                const std::uint32_t index = static_cast<std::uint32_t>(descriptor);
                auto* block = reinterpret_cast<RuntimeDescriptorRecord*>(gRuntimeDescriptorBlocks[index >> 5]);
                RuntimeDescriptorRecord& entry = block[index & 31u];

                entry.osHandle = handleBits;
                entry.flags = flags;
            }
        }

        for (std::uint32_t descriptor = 0; descriptor < 3u; ++descriptor)
        {
            auto* block = reinterpret_cast<RuntimeDescriptorRecord*>(gRuntimeDescriptorBlocks[0]);
            RuntimeDescriptorRecord& entry = block[descriptor];

            if (entry.osHandle != 0xFFFFFFFFu)
            {
                // Standard descriptors always use text mode after startup.
                entry.flags |= 0x80u;
                continue;
            }

            entry.flags = 0x81u;

            DWORD standardHandle;

            if (descriptor == 0)
                standardHandle = STD_INPUT_HANDLE;
            else if (descriptor == 1)
                standardHandle = STD_OUTPUT_HANDLE;
            else
                standardHandle = STD_ERROR_HANDLE;

            HANDLE handle = ::GetStdHandle(standardHandle);

            if (handle == INVALID_HANDLE_VALUE)
            {
                entry.flags |= 0x40u;
                continue;
            }

            const DWORD fileType = ::GetFileType(handle);

            if (fileType == FILE_TYPE_UNKNOWN)
            {
                // Preserve the invalid stored handle and mark it as a device.
                entry.flags |= 0x40u;
                continue;
            }

            entry.osHandle = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(handle));

            switch (fileType & 0xFFu)
            {
                case FILE_TYPE_CHAR:
                    entry.flags |= 0x40u;
                    break;

                case FILE_TYPE_PIPE:
                    entry.flags |= 0x08u;
                    break;
            }
        }

        ::SetHandleCount(gRuntimeDescriptorCapacity);
    }

    // Original function: FUN_004eaae3
    void Runtime_InitializeTimeZone()
    {
        const auto* initialized = reinterpret_cast<const volatile std::uint32_t*>(&gRuntimeTimeZoneInitialized);

        if (*initialized != 0)
        {
            return;
        }

        Runtime_Lock(0x0B);

        if (*initialized == 0)
        {
            // Refresh the original CRT environment and Windows time-zone settings.
            Runtime_RefreshTimeZone();

            // The original marks initialization complete even when setup returns early.
            ++gRuntimeTimeZoneInitialized;
        }

        Runtime_Unlock(0x0B);
    }

    // Original function: FUN_004eab11.
    void Runtime_RefreshTimeZone()
    {
        Runtime_Lock(0x0C);

        gRuntimeDaylightStartYear = -1;
        gRuntimeDaylightEndYear = -1;
        gRuntimeUsesWindowsTimeZone = 0;

        // This must access the original CRT environment, not the live Win32 environment.
        const char* environmentValue = Runtime_GetEnvironmentVariable("TZ");

        if (environmentValue == nullptr)
        {
            Runtime_Unlock(0x0C);

            if (GetTimeZoneInformation(&gRuntimeTimeZoneInformation) == TIME_ZONE_ID_INVALID)
            {
                return;
            }

            const std::int32_t standardBias = gRuntimeTimeZoneInformation.StandardBias;
            std::uint32_t bias = static_cast<std::uint32_t>(gRuntimeTimeZoneInformation.Bias) * 60u;

            gRuntimeUsesWindowsTimeZone = 1;

            if (gRuntimeTimeZoneInformation.StandardDate.wMonth != 0)
            {
                bias += static_cast<std::uint32_t>(standardBias) * 60u;
            }

            gRuntimeTimeZoneBiasSeconds = Runtime_TimeSigned32(bias);

            if (gRuntimeTimeZoneInformation.DaylightDate.wMonth != 0 && gRuntimeTimeZoneInformation.DaylightBias != 0)
            {
                gRuntimeDaylightEnabled = 1;

                const std::uint32_t daylightBias = static_cast<std::uint32_t>(gRuntimeTimeZoneInformation.DaylightBias) - static_cast<std::uint32_t>(standardBias);
                gRuntimeDaylightBiasSeconds = Runtime_TimeSigned32(daylightBias * 60u);
            }
            else
            {
                gRuntimeDaylightEnabled = 0;
                gRuntimeDaylightBiasSeconds = 0;
            }

            BOOL usedDefaultCharacter = FALSE;

            const int standardNameLength = WideCharToMultiByte(gRuntimeCodePage, 0x220u, gRuntimeTimeZoneInformation.StandardName, -1, gRuntimeTimeZoneNames[0], 63, nullptr, &usedDefaultCharacter);

            if (standardNameLength != 0 && usedDefaultCharacter == FALSE)
            {
                gRuntimeTimeZoneNames[0][63] = '\0';
            }
            else
            {
                gRuntimeTimeZoneNames[0][0] = '\0';
            }

            const int daylightNameLength = WideCharToMultiByte(gRuntimeCodePage, 0x220u, gRuntimeTimeZoneInformation.DaylightName, -1, gRuntimeTimeZoneNames[1], 63, nullptr, &usedDefaultCharacter);

            if (daylightNameLength != 0 && usedDefaultCharacter == FALSE)
            {
                gRuntimeTimeZoneNames[1][63] = '\0';
            }
            else
            {
                gRuntimeTimeZoneNames[1][0] = '\0';
            }

            return;
        }

        if (*environmentValue == '\0')
        {
            Runtime_Unlock(0x0C);
            return;
        }

        if (gRuntimePreviousTimeZoneEnvironment != nullptr && std::strcmp(environmentValue, gRuntimePreviousTimeZoneEnvironment) == 0)
        {
            Runtime_Unlock(0x0C);
            return;
        }

        Runtime_Free(gRuntimePreviousTimeZoneEnvironment);

        const std::uint32_t requiredBytes = static_cast<std::uint32_t>(std::strlen(environmentValue)) + 1u;
        gRuntimePreviousTimeZoneEnvironment = static_cast<char*>(Runtime_Allocate(requiredBytes));

        if (gRuntimePreviousTimeZoneEnvironment == nullptr)
        {
            Runtime_Unlock(0x0C);
            return;
        }

        std::strcpy(gRuntimePreviousTimeZoneEnvironment, environmentValue);

        // The original releases the environment lock before parsing the original pointer.
        Runtime_Unlock(0x0C);

        std::strncpy(gRuntimeTimeZoneNames[0], environmentValue, 3u);
        gRuntimeTimeZoneNames[0][3] = '\0';

        // The original assumes that the standard name occupies at least three bytes.
        const char* cursor = environmentValue + 3;
        bool negativeBias = false;

        if (*cursor == '-')
        {
            negativeBias = true;
            ++cursor;
        }

        std::uint32_t bias = static_cast<std::uint32_t>(Runtime_StringToInteger(cursor)) * 3600u;
        gRuntimeTimeZoneBiasSeconds = Runtime_TimeSigned32(bias);

        while (*cursor == '+' || (*cursor >= '0' && *cursor <= '9'))
        {
            ++cursor;
        }

        if (*cursor == ':')
        {
            ++cursor;

            bias += static_cast<std::uint32_t>(Runtime_StringToInteger(cursor)) * 60u;
            gRuntimeTimeZoneBiasSeconds = Runtime_TimeSigned32(bias);

            while (*cursor >= '0' && *cursor <= '9')
            {
                ++cursor;
            }

            if (*cursor == ':')
            {
                ++cursor;

                bias += static_cast<std::uint32_t>(Runtime_StringToInteger(cursor));
                gRuntimeTimeZoneBiasSeconds = Runtime_TimeSigned32(bias);

                while (*cursor >= '0' && *cursor <= '9')
                {
                    ++cursor;
                }
            }
        }

        if (negativeBias)
        {
            bias = 0u - bias;
            gRuntimeTimeZoneBiasSeconds = Runtime_TimeSigned32(bias);
        }

        const std::uint32_t daylightByte = static_cast<unsigned char>(*cursor);
        gRuntimeDaylightEnabled = daylightByte < 0x80u ? static_cast<std::int32_t>(daylightByte) : static_cast<std::int32_t>(daylightByte) - 256;

        if (gRuntimeDaylightEnabled != 0)
        {
            std::strncpy(gRuntimeTimeZoneNames[1], cursor, 3u);
            gRuntimeTimeZoneNames[1][3] = '\0';
        }
        else
        {
            gRuntimeTimeZoneNames[1][0] = '\0';
        }

        // Preserve the existing daylight bias: the original TZ branch does not update it.
    }

    // Original function: FUN_004ead98
    std::int32_t Runtime_IsDaylightTime(const RuntimeCalendarTime* calendar)
    {
        Runtime_Lock(0x0B);

        // Evaluate the reconstructed daylight transitions while holding the time-zone lock.
        const std::int32_t result = Runtime_IsDaylightTimeNoLock(calendar);

        Runtime_Unlock(0x0B);
        return result;
    }

    // Original function: FUN_004eadb9.
    std::int32_t Runtime_IsDaylightTimeNoLock(const RuntimeCalendarTime* calendar)
    {
        if (gRuntimeDaylightEnabled == 0)
        {
            return 0;
        }

        const std::int32_t year = calendar->year;

        if (year != gRuntimeDaylightStartYear || year != gRuntimeDaylightEndYear)
        {
            if (gRuntimeUsesWindowsTimeZone != 0)
            {
                const SYSTEMTIME& daylightDate = gRuntimeTimeZoneInformation.DaylightDate;

                if (daylightDate.wYear == 0)
                {
                    Runtime_SetDaylightTransition(1, 1, year, daylightDate.wMonth, daylightDate.wDay, daylightDate.wDayOfWeek, 0, daylightDate.wHour, daylightDate.wMinute, daylightDate.wSecond, daylightDate.wMilliseconds);
                }
                else
                {
                    Runtime_SetDaylightTransition(1, 0, year, daylightDate.wMonth, 0, 0, daylightDate.wDay, daylightDate.wHour, daylightDate.wMinute, daylightDate.wSecond, daylightDate.wMilliseconds);
                }

                const SYSTEMTIME& standardDate = gRuntimeTimeZoneInformation.StandardDate;

                if (standardDate.wYear == 0)
                {
                    Runtime_SetDaylightTransition(0, 1, year, standardDate.wMonth, standardDate.wDay, standardDate.wDayOfWeek, 0, standardDate.wHour, standardDate.wMinute, standardDate.wSecond, standardDate.wMilliseconds);
                }
                else
                {
                    Runtime_SetDaylightTransition(0, 0, year, standardDate.wMonth, 0, 0, standardDate.wDay, standardDate.wHour, standardDate.wMinute, standardDate.wSecond, standardDate.wMilliseconds);
                }
            }
            else
            {
                Runtime_SetDaylightTransition(1, 1, year, 4, 1, 0, 0, 2, 0, 0, 0);
                Runtime_SetDaylightTransition(0, 1, year, 10, 5, 0, 0, 2, 0, 0, 0);
            }
        }

        const std::int32_t yearDay = calendar->yearDay;
        const std::int32_t startDay = gRuntimeDaylightStartYearDay;
        const std::int32_t endDay = gRuntimeDaylightEndYearDay;

        if (startDay < endDay)
        {
            if (yearDay < startDay || yearDay > endDay)
            {
                return 0;
            }

            if (yearDay > startDay && yearDay < endDay)
            {
                return 1;
            }
        }
        else
        {
            if (yearDay < endDay || yearDay > startDay)
            {
                return 1;
            }

            if (yearDay > endDay && yearDay < startDay)
            {
                return 0;
            }
        }

        std::uint32_t milliseconds = static_cast<std::uint32_t>(calendar->hour);
        milliseconds = milliseconds * 60u + static_cast<std::uint32_t>(calendar->minute);
        milliseconds = milliseconds * 60u + static_cast<std::uint32_t>(calendar->second);
        milliseconds *= 1000u;

        const std::int32_t signedMilliseconds = Runtime_TimeSigned32(milliseconds);

        if (yearDay == startDay)
        {
            return signedMilliseconds >= gRuntimeDaylightStartMilliseconds ? 1 : 0;
        }

        return signedMilliseconds < gRuntimeDaylightEndMilliseconds ? 1 : 0;
    }

    // Original function: FUN_004eaf65.
    void Runtime_SetDaylightTransition(std::int32_t isStart, std::int32_t ruleType, std::int32_t year, std::int32_t month, std::int32_t week, std::int32_t weekDay, std::int32_t monthDay, std::int32_t hour, std::int32_t minute, std::int32_t second, std::int32_t millisecond)
    {
        const std::uint32_t yearBits = static_cast<std::uint32_t>(year);
        const std::int32_t* monthOffsets = (yearBits & 3u) == 0u ? gRuntimeLeapMonthDayOffsets : gRuntimeMonthDayOffsets;

        std::uint32_t transitionDay;

        if (ruleType == 1)
        {
            const std::uint32_t firstDay = static_cast<std::uint32_t>(monthOffsets[month]) + 1u;
            const std::uint32_t previousYear = yearBits - 1u;

            // Reproduce SAR by two without relying on signed right-shift behavior.
            const std::uint32_t leapYears = (previousYear >> 2u) | ((previousYear & 0x80000000u) != 0u ? 0xC0000000u : 0u);
            const std::uint32_t weekdayValue = yearBits * 365u + firstDay + leapYears - 0x63DBu;
            const std::int32_t firstWeekDay = Runtime_TimeSigned32(weekdayValue) % 7;

            transitionDay = firstDay + static_cast<std::uint32_t>(week) * 7u;
            transitionDay -= static_cast<std::uint32_t>(firstWeekDay);
            transitionDay += static_cast<std::uint32_t>(weekDay);

            if (firstWeekDay <= weekDay)
            {
                transitionDay -= 7u;
            }

            if (week == 5 && Runtime_TimeSigned32(transitionDay) > monthOffsets[month + 1])
            {
                transitionDay -= 7u;
            }
        }
        else
        {
            transitionDay = static_cast<std::uint32_t>(monthOffsets[month]) + static_cast<std::uint32_t>(monthDay);
        }

        std::uint32_t milliseconds = static_cast<std::uint32_t>(hour) * 60u;
        milliseconds += static_cast<std::uint32_t>(minute);
        milliseconds *= 60u;

        if (isStart == 1)
        {
            milliseconds += static_cast<std::uint32_t>(second);
            milliseconds *= 1000u;
            milliseconds += static_cast<std::uint32_t>(millisecond);

            gRuntimeDaylightStartYearDay = Runtime_TimeSigned32(transitionDay);
            gRuntimeDaylightStartYear = year;
            gRuntimeDaylightStartMilliseconds = Runtime_TimeSigned32(milliseconds);
            return;
        }

        milliseconds += static_cast<std::uint32_t>(gRuntimeDaylightBiasSeconds);
        milliseconds += static_cast<std::uint32_t>(second);
        milliseconds *= 1000u;
        milliseconds += static_cast<std::uint32_t>(millisecond);

        // The original adjusts by at most one day.
        if (Runtime_TimeSigned32(milliseconds) < 0)
        {
            milliseconds += 86400000u;
            --transitionDay;
        }
        else if (Runtime_TimeSigned32(milliseconds) >= 86400000)
        {
            milliseconds -= 86400000u;
            ++transitionDay;
        }

        gRuntimeDaylightEndYearDay = Runtime_TimeSigned32(transitionDay);
        gRuntimeDaylightEndMilliseconds = Runtime_TimeSigned32(milliseconds);
        gRuntimeDaylightEndYear = year;
    }

    // Original function: FUN_004eb0a5
    std::int32_t Runtime_MakeTime32(std::int32_t year, std::int32_t month, std::int32_t day, std::int32_t hour, std::int32_t minute, std::int32_t second, std::int32_t daylight)
    {
        const std::uint32_t yearBits = static_cast<std::uint32_t>(year) - 1900u;

        std::int32_t relativeYear;
        std::memcpy(&relativeYear, &yearBits, sizeof(relativeYear));

        if (relativeYear < 70 || relativeYear > 138)
        {
            return -1;
        }

        // Preserve the original unchecked month index and 32-bit arithmetic.
        std::uint32_t yearDayBits = static_cast<std::uint32_t>(gRuntimeMonthDayOffsets[month]) + static_cast<std::uint32_t>(day);

        if ((yearBits & 3u) == 0 && month > 2)
        {
            ++yearDayBits;
        }

        Runtime_InitializeTimeZone();

        std::uint32_t resultBits = yearBits * 365u;
        resultBits += (yearBits - 1u) >> 2;
        resultBits += yearDayBits;
        resultBits = resultBits * 24u + static_cast<std::uint32_t>(hour);
        resultBits = resultBits * 60u + static_cast<std::uint32_t>(minute);
        resultBits = resultBits * 60u + static_cast<std::uint32_t>(second);
        resultBits += static_cast<std::uint32_t>(gRuntimeTimeZoneBiasSeconds);
        resultBits += 0x7C558180u;

        bool applyDaylight = daylight == 1;

        if (daylight == -1 && gRuntimeDaylightEnabled != 0)
        {
            // Intentionally not value-initialized: preserve the original partial setup.
            RuntimeCalendarTime calendar;

            calendar.hour = hour;
            calendar.year = relativeYear;

            const std::uint32_t monthBits = static_cast<std::uint32_t>(month) - 1u;
            std::memcpy(&calendar.month, &monthBits, sizeof(calendar.month));
            std::memcpy(&calendar.yearDay, &yearDayBits, sizeof(calendar.yearDay));

            applyDaylight = Runtime_IsDaylightTime(&calendar) != 0;
        }

        if (applyDaylight)
        {
            resultBits += static_cast<std::uint32_t>(gRuntimeDaylightBiasSeconds);
        }

        std::int32_t result;
        std::memcpy(&result, &resultBits, sizeof(result));
        return result;
    }

    // Original function: FUN_004eb167
    std::int32_t Runtime_InitializeThreading()
    {
        Runtime_InitializeLocks();

        const DWORD tlsIndex = ::TlsAlloc();
        gRuntimeThreadDataTlsIndex = tlsIndex;

        if (tlsIndex == TLS_OUT_OF_INDEXES)
        {
            return 0;
        }

        auto* threadData = Runtime_AllocateZeroed(1u, 0x74u);

        if (threadData == nullptr)
        {
            return 0;
        }

        if (!::TlsSetValue(gRuntimeThreadDataTlsIndex, threadData))
        {
            return 0;
        }

        Runtime_InitializeThreadData(threadData);

        const DWORD threadId = ::GetCurrentThreadId();
        auto* threadBytes = reinterpret_cast<std::uint8_t*>(threadData);

        const std::uint32_t invalidHandle = 0xFFFFFFFFu;
        std::memcpy(threadBytes + 0x04, &invalidHandle, sizeof(invalidHandle));
        std::memcpy(threadBytes, &threadId, sizeof(threadId));

        return 1;
    }

    // Original function: FUN_004eb1bb
    void Runtime_InitializeThreadData(void* threadData)
    {
        auto* threadBytes = static_cast<std::uint8_t*>(threadData);

        const void* initialTable = &gRuntimeDefaultExceptionActions;
        std::memcpy(threadBytes + 0x50, &initialTable, sizeof(initialTable));

        const std::uint32_t initialValue = 1;
        std::memcpy(threadBytes + 0x14, &initialValue, sizeof(initialValue));
    }

    // Original function: FUN_004eb1ce
    DWORD* Runtime_GetThreadData()
    {
        const DWORD savedLastError = ::GetLastError();
        void* threadData = ::TlsGetValue(gRuntimeThreadDataTlsIndex);

        if (threadData == nullptr)
        {
            threadData = Runtime_AllocateZeroed(1u, 0x74u);

            if (threadData == nullptr || !::TlsSetValue(gRuntimeThreadDataTlsIndex, threadData))
            {
                // Fatal CRT error; the original performs no local cleanup.
                Runtime_FatalExit(0x10);
            }
            else
            {
                Runtime_InitializeThreadData(threadData);

                const DWORD threadId = ::GetCurrentThreadId();
                auto* threadBytes = static_cast<std::uint8_t*>(threadData);

                const std::uint32_t invalidHandle = 0xFFFFFFFFu;
                std::memcpy(threadBytes + 0x04, &invalidHandle, sizeof(invalidHandle));
                std::memcpy(threadBytes, &threadId, sizeof(threadId));
            }
        }

        ::SetLastError(savedLastError);
        return static_cast<DWORD*>(threadData);
    }

    // Original function: FUN_004eb235
    std::int32_t Runtime_ResizeNoLock(std::uint32_t descriptor, std::uint32_t newSize)
    {
        const auto asSigned = [] (std::uint32_t bits) -> std::int32_t
        {
            std::int32_t value;
            std::memcpy(&value, &bits, sizeof(value));
            return value;
        };

        const std::uint32_t originalPosition = Runtime_SeekNoLock(descriptor, 0, 1);

        if (originalPosition == 0xFFFFFFFFu)
            return -1;

        const std::uint32_t currentSize = Runtime_SeekNoLock(descriptor, 0, 2);

        if (currentSize == 0xFFFFFFFFu)
            return -1;

        std::uint32_t remainingBits = newSize - currentSize;
        std::int32_t result = 0;

        if (asSigned(remainingBits) > 0)
        {
            std::uint8_t zeros[0x1000];
            std::memset(zeros, 0, sizeof(zeros));

            const std::int32_t previousMode = Runtime_SetModeNoLock(descriptor, 0x8000);

            do
            {
                const std::int32_t remaining = asSigned(remainingBits);
                const std::uint32_t chunkSize = remaining < 0x1000 ? static_cast<std::uint32_t>(remaining) : 0x1000u;
                const std::int32_t written = Runtime_WriteNoLock(descriptor, zeros, chunkSize);

                if (written == -1)
                {
                    if (*Runtime_GetDosErrnoPointer() == 5)
                        *Runtime_GetErrnoPointer() = 0x0D;

                    result = -1;
                    break;
                }

                // Preserve the original subtraction and signed continuation test.
                remainingBits -= static_cast<std::uint32_t>(written);
            } while (asSigned(remainingBits) > 0);

            Runtime_SetModeNoLock(descriptor, previousMode);
        }
        else if (asSigned(remainingBits) < 0)
        {
            // The original ignores this seek's return value.
            Runtime_SeekNoLock(descriptor, asSigned(newSize), 0);

            const std::uint32_t handleValue = Runtime_GetOsHandle(descriptor);
            const HANDLE handle = reinterpret_cast<HANDLE>(static_cast<std::uintptr_t>(handleValue));

            if (::SetEndOfFile(handle) == FALSE)
            {
                result = -1;

                // Preserve the original order of errno access and GetLastError.
                *Runtime_GetErrnoPointer() = 0x0D;
                const DWORD error = ::GetLastError();
                *Runtime_GetDosErrnoPointer() = error;
            }
        }

        // The restore result does not replace the resize result.
        Runtime_SeekNoLock(descriptor, asSigned(originalPosition), 0);
        return result;
    }

    // Original function: FUN_004eb935
    int Runtime_ProbePentiumDivision()
    {
        // Original constants: 004fbb18, 004fbb20 and 004f3350.
        const double numerator = 4195835.0;
        const double denominator = 3145727.0;
        const double reference = 1.0;
        double residual;
        std::uint16_t status;

        // Preserve x87 evaluation and the explicit double-precision store.
        __asm
        {
            fld qword ptr numerator
            fdiv qword ptr denominator
            fmul qword ptr denominator
            fsubr qword ptr numerator
            fstp qword ptr residual
            fld qword ptr residual
            fcomp qword ptr reference
            fnstsw ax
            mov status, ax
        }

        return (status & 0x4100u) == 0 ? 1 : 0;
    }

    // Original function: FUN_004eb973
    int Runtime_GetPentiumDivisionAdjustment()
    {
        const HMODULE kernel = GetModuleHandleA("KERNEL32");

        if (kernel != nullptr)
        {
            using ProcessorFeatureQuery = BOOL(WINAPI*)(DWORD);

            const auto query = reinterpret_cast<ProcessorFeatureQuery>(GetProcAddress(kernel, "IsProcessorFeaturePresent"));

            if (query != nullptr)
                return query(0);
        }

        return Runtime_ProbePentiumDivision();
    }

    // Original function: FUN_004eb99c
    void Runtime_FloatInsertDecimalPoint(char* text)
    {
        char* position = text;

        const std::uint8_t firstByte = static_cast<std::uint8_t>(*position);
        const std::int32_t firstCharacter = firstByte < 0x80 ? static_cast<std::int32_t>(firstByte) : static_cast<std::int32_t>(firstByte) - 0x100;

        if (Runtime_ToLower(firstCharacter) != 'e')
        {
            // The original advances before testing the next character.
            do
            {
                ++position;
            } while (Runtime_IsDigit(*position));
        }

        char decimalSeparator;
        std::memcpy(&decimalSeparator, &gRuntimeDecimalSeparator, sizeof(decimalSeparator));

        char carriedCharacter = *position;
        *position++ = decimalSeparator;

        for (;;)
        {
            const char nextCharacter = *position;
            *position = carriedCharacter;

            const char writtenCharacter = *position;
            carriedCharacter = nextCharacter;
            ++position;

            if (writtenCharacter == '\0')
            {
                break;
            }
        }
    }

    // Original function: FUN_004eba5c (__fassign).
    void Runtime_AssignFloat(int storeDouble, void* destination, const char* text)
    {
        std::uint32_t result[2];
        if (storeDouble != 0)
        {
            Runtime_ParseDouble(result, text);
            std::memcpy(destination, result, 8);
        }
        else
        {
            Runtime_ParseFloat(result, text);
            std::memcpy(destination, result, 4);
        }
    }

    // Original function: FUN_004eba9a
    char* Runtime_ConvertFloatScientific(const double* value, char* buffer, std::int32_t precision, std::int32_t uppercase)
    {
        RuntimeFloatConversionState state;
        char workspace[24];

        Runtime_PrepareFloatConversion(*value, &state, workspace);

        // Preserve the 32-bit addition used for the requested digit count.
        const std::uint32_t digitCountBits = static_cast<std::uint32_t>(precision) + std::uint32_t{ 1 };

        std::int32_t digitCount;
        std::memcpy(&digitCount, &digitCountBits, sizeof(digitCount));

        char* digits = buffer + (state.sign == '-' ? 1 : 0) + (precision > 0 ? 1 : 0);

        Runtime_GenerateFloatDigits(digits, digitCount, &state);
        Runtime_FormatExponentialDigits(buffer, precision, uppercase, &state, 0);

        return buffer;
    }

    // Original function: FUN_004ebafb
    char* Runtime_FormatExponentialDigits(char* buffer, std::int32_t precision, std::int32_t uppercase, const RuntimeFloatConversionState* state, char generalFormat)
    {
        if (generalFormat != 0)
        {
            Runtime_ShiftString(buffer + (state->sign == '-' ? 1 : 0), precision > 0 ? 1 : 0);
        }

        char* position = buffer;

        if (state->sign == '-')
            *position++ = '-';

        if (precision > 0)
        {
            position[0] = position[1];
            ++position;
            *position = static_cast<char>(gRuntimeDecimalSeparator);
        }

        char* exponentText = position + precision + (generalFormat == 0 ? 1 : 0);
        std::memcpy(exponentText, "e+000", 6);

        if (uppercase != 0)
            exponentText[0] = 'E';

        // The original checks the source digits when deciding whether the value is zero.
        if (state->digits[0] != '0')
        {
            std::int32_t exponent = state->decimalPosition - 1;

            if (exponent < 0)
            {
                exponent = -exponent;
                exponentText[1] = '-';
            }

            if (exponent >= 100)
            {
                exponentText[2] = static_cast<char>('0' + exponent / 100);
                exponent %= 100;
            }

            if (exponent >= 10)
            {
                exponentText[3] = static_cast<char>('0' + exponent / 10);
                exponent %= 10;
            }

            exponentText[4] = static_cast<char>('0' + exponent);
        }

        return buffer;
    }

    // Original function: FUN_004ebbbd
    char* Runtime_ConvertFloatFixed(const double* value, char* buffer, std::int32_t precision)
    {
        RuntimeFloatConversionState state;
        char workspace[24];

        Runtime_PrepareFloatConversion(*value, &state, workspace);

        const std::uint32_t digitCountBits = static_cast<std::uint32_t>(state.decimalPosition) + static_cast<std::uint32_t>(precision);

        std::int32_t digitCount;
        std::memcpy(&digitCount, &digitCountBits, sizeof(digitCount));

        char* digits = buffer + (state.sign == '-' ? 1 : 0);

        Runtime_GenerateFloatDigits(digits, digitCount, &state);
        Runtime_FormatFixedDigits(buffer, precision, &state, 0);

        return buffer;
    }

    // Original function: FUN_004ebc12
    char* Runtime_FormatFixedDigits(char* buffer, std::int32_t precision, const RuntimeFloatConversionState* state, char generalFormat)
    {
        const std::int32_t lastIntegerDigit = state->decimalPosition - 1;

        if (generalFormat != 0 && lastIntegerDigit == precision)
        {
            char* position = buffer + (state->sign == '-' ? 1 : 0) + lastIntegerDigit;
            position[0] = '0';
            position[1] = '\0';
        }

        char* position = buffer;

        if (state->sign == '-')
            *position++ = '-';

        if (state->decimalPosition <= 0)
        {
            Runtime_ShiftString(position, 1);
            *position++ = '0';
        }
        else
        {
            position += state->decimalPosition;
        }

        if (precision > 0)
        {
            Runtime_ShiftString(position, 1);
            *position++ = static_cast<char>(gRuntimeDecimalSeparator);

            if (state->decimalPosition < 0)
            {
                const std::int32_t leadingZeros = -state->decimalPosition;

                if (generalFormat != 0 || precision >= leadingZeros)
                    precision = leadingZeros;

                Runtime_ShiftString(position, precision);
                std::memset(position, '0', static_cast<std::size_t>(precision));
            }
        }

        return buffer;
    }

    // Original function: FUN_004ebcb9
    void Runtime_ConvertFloatGeneral(const double* value, char* buffer, std::int32_t precision, std::int32_t uppercase)
    {
        RuntimeFloatConversionState state;
        char workspace[24];

        Runtime_PrepareFloatConversion(*value, &state, workspace);

        const std::uint32_t initialExponentBits = static_cast<std::uint32_t>(state.decimalPosition) - std::uint32_t{ 1 };

        std::int32_t initialExponent;
        std::memcpy(&initialExponent, &initialExponentBits, sizeof(initialExponent));

        char* digits = buffer + (state.sign == '-' ? 1 : 0);

        Runtime_GenerateFloatDigits(digits, precision, &state);

        // The digit-generation step may change the decimal position.
        const std::uint32_t roundedExponentBits = static_cast<std::uint32_t>(state.decimalPosition) - std::uint32_t{ 1 };

        std::int32_t roundedExponent;
        std::memcpy(&roundedExponent, &roundedExponentBits, sizeof(roundedExponent));

        const bool exponentIncreased = initialExponent < roundedExponent;

        if (roundedExponent < -4 || roundedExponent >= precision)
        {
            Runtime_FormatExponentialDigits(buffer, precision, uppercase, &state, 1);
            return;
        }

        if (exponentIncreased)
        {
            char* position = digits;
            char character;

            do
            {
                character = *position++;
            } while (character != '\0');

            // Remove the final generated digit after an exponent increase.
            position[-2] = '\0';
        }

        Runtime_FormatFixedDigits(buffer, precision, &state, 1);
    }

    // Original function: FUN_004ebd9d
    void Runtime_ShiftString(char* text, std::int32_t offset)
    {
        if (offset == 0)
            return;

        const std::size_t byteCount = std::strlen(text) + 1;
        std::memmove(text + offset, text, byteCount);
    }

    // Original function: FUN_004ebdc2
    void Runtime_InitializeLocks()
    {
        auto* slots = reinterpret_cast<CRITICAL_SECTION* volatile*>(&gRuntimeLockTable);

        ::InitializeCriticalSection(slots[0x11]);
        ::InitializeCriticalSection(slots[0x0D]);
        ::InitializeCriticalSection(slots[0x09]);
        ::InitializeCriticalSection(slots[0x01]);
    }

    // Original function: FUN_004ebdeb
    void Runtime_Lock(std::int32_t lockIndex)
    {
        auto* slots = reinterpret_cast<CRITICAL_SECTION* volatile*>(&gRuntimeLockTable);
        auto* slot = slots + lockIndex;

        if (*slot == nullptr)
        {
            // Keep the original CRT allocation and deallocation pair.
            auto* candidate = static_cast<CRITICAL_SECTION*>(Runtime_Allocate(0x18));

            if (candidate == nullptr)
            {
                Runtime_FatalExit(0x11);
            }

            Runtime_Lock(0x11);

            // Another thread may have initialized this slot in the meantime.
            if (*slot == nullptr)
            {
                ::InitializeCriticalSection(candidate);
                *slot = candidate;
            }
            else
            {
                Runtime_Free(candidate);
            }

            Runtime_Unlock(0x11);
        }

        ::EnterCriticalSection(*slot);
    }

    // Original function: FUN_004ebe4c
    void Runtime_Unlock(std::int32_t lockIndex)
    {
        auto* slots = reinterpret_cast<CRITICAL_SECTION* volatile*>(&gRuntimeLockTable);

        ::LeaveCriticalSection(slots[lockIndex]);
    }

    // Original function: FUN_004ec031.
    void Runtime_InitializeEnvironment()
    {
        if (gRuntimeMultibyteInitialized == 0)
        {
            Runtime_InitializeMultibyte();
        }

        char* cursor = gRuntimeStartupEnvironmentBlock;
        std::uint32_t count = 0;

        while (*cursor != '\0')
        {
            if (*cursor != '=')
            {
                ++count;
            }

            cursor += std::strlen(cursor) + 1u;
        }

        gRuntimeEnvironment = static_cast<char**>(Runtime_Allocate(count * 4u + 4u));

        if (gRuntimeEnvironment == nullptr)
        {
            Runtime_FatalExit(9);
        }

        char** destination = gRuntimeEnvironment;
        cursor = gRuntimeStartupEnvironmentBlock;

        while (*cursor != '\0')
        {
            const std::uint32_t length = static_cast<std::uint32_t>(std::strlen(cursor)) + 1u;

            if (*cursor != '=')
            {
                *destination = static_cast<char*>(Runtime_Allocate(length));

                if (*destination == nullptr)
                {
                    Runtime_FatalExit(9);
                }

                std::strcpy(*destination, cursor);
                ++destination;
            }

            cursor += length;
        }

        Runtime_Free(gRuntimeStartupEnvironmentBlock);
        gRuntimeStartupEnvironmentBlock = nullptr;

        *destination = nullptr;
        gRuntimeEnvironmentInitialized = 1;
    }

    // Original function: FUN_004ec337.
    char* Runtime_GetEnvironmentBlock()
    {
        wchar_t* wideBlock = nullptr;
        char* ansiBlock = nullptr;

        if (gRuntimeEnvironmentStringsBackend == 0)
        {
            wideBlock = GetEnvironmentStringsW();

            if (wideBlock != nullptr)
            {
                gRuntimeEnvironmentStringsBackend = 1;
            }
            else
            {
                ansiBlock = GetEnvironmentStringsA();

                if (ansiBlock == nullptr)
                {
                    return nullptr;
                }

                gRuntimeEnvironmentStringsBackend = 2;
            }
        }

        if (gRuntimeEnvironmentStringsBackend == 1)
        {
            if (wideBlock == nullptr)
            {
                wideBlock = GetEnvironmentStringsW();

                if (wideBlock == nullptr)
                {
                    return nullptr;
                }
            }

            const wchar_t* end = wideBlock;

            while (*end != L'\0')
            {
                while (*end != L'\0')
                {
                    ++end;
                }

                ++end;
            }

            const int characterCount = static_cast<int>(end - wideBlock + 1);
            const int requiredBytes = WideCharToMultiByte(0, 0, wideBlock, characterCount, nullptr, 0, nullptr, nullptr);
            char* result = nullptr;

            if (requiredBytes != 0)
            {
                result = static_cast<char*>(Runtime_Allocate(static_cast<std::uint32_t>(requiredBytes)));

                if (result != nullptr)
                {
                    if (WideCharToMultiByte(0, 0, wideBlock, characterCount, result, requiredBytes, nullptr, nullptr) == 0)
                    {
                        Runtime_Free(result);
                        result = nullptr;
                    }
                }
            }

            FreeEnvironmentStringsW(wideBlock);
            return result;
        }

        if (gRuntimeEnvironmentStringsBackend != 2)
        {
            return nullptr;
        }

        if (ansiBlock == nullptr)
        {
            ansiBlock = GetEnvironmentStringsA();

            if (ansiBlock == nullptr)
            {
                return nullptr;
            }
        }

        const char* end = ansiBlock;

        while (*end != '\0')
        {
            while (*end != '\0')
            {
                ++end;
            }

            ++end;
        }

        const std::uint32_t requiredBytes = static_cast<std::uint32_t>(end - ansiBlock) + 1u;
        char* result = static_cast<char*>(Runtime_Allocate(requiredBytes));

        if (result != nullptr)
        {
            std::memcpy(result, ansiBlock, requiredBytes);
        }

        FreeEnvironmentStringsA(ansiBlock);
        return result;
    }

    // Original function: FUN_004ec469
    void Runtime_GetExecutableLinkerVersion(std::uint32_t* version)
    {
        *version = 0;

        const auto* module = reinterpret_cast<const std::uint8_t*>(GetModuleHandleA(nullptr));

        std::uint16_t signature;
        std::memcpy(&signature, module, sizeof(signature));

        if (signature != 0x5A4Du)
        {
            return;
        }

        std::uint32_t peOffset;
        std::memcpy(&peOffset, module + 0x3C, sizeof(peOffset));

        if (peOffset == 0)
        {
            return;
        }

        // Preserve the original check: no additional PE signature validation.
        const auto* peHeader = module + peOffset;
        *version = static_cast<std::uint32_t>(peHeader[0x1A]) | (static_cast<std::uint32_t>(peHeader[0x1B]) << 8);
    }

    // Original function: FUN_004ec496
    std::int32_t Runtime_SelectHeapMode()
    {
        OSVERSIONINFOA version;
        version.dwOSVersionInfoSize = sizeof(version);

        if (GetVersionExA(&version) != FALSE)
        {
            if (version.dwPlatformId == 2u && version.dwMajorVersion >= 5u)
            {
                return 1;
            }
        }

        char selection[0x1090];

        if (GetEnvironmentVariableA("__MSVCRT_HEAP_SELECT", selection, sizeof(selection)) != 0)
        {
            const auto uppercaseAscii = [] (char* text)
            {
                while (*text != '\0')
                {
                    if (*text >= 'a' && *text <= 'z')
                    {
                        *text = static_cast<char>(*text - 0x20);
                    }

                    ++text;
                }
            };

            uppercaseAscii(selection);

            char* selectedEntry;

            // Original function: FUN_004e7a70
            // Preserve the original comparison length, including the literal's NUL.
            if (std::strncmp("__GLOBAL_HEAP_SELECTED", selection, 0x16u) == 0)
            {
                selectedEntry = selection;
            }
            else
            {
                char executablePath[0x104];

                GetModuleFileNameA(nullptr, executablePath, sizeof(executablePath));
                uppercaseAscii(executablePath);

                // Original function: FUN_004e8010
                selectedEntry = std::strstr(selection, executablePath);
            }

            if (selectedEntry != nullptr)
            {
                // Original function: FUN_004e5e70
                char* comma = std::strchr(selectedEntry, ',');

                if (comma != nullptr)
                {
                    char* number = comma + 1;

                    for (char* cursor = number; *cursor != '\0'; ++cursor)
                    {
                        if (*cursor == ';')
                        {
                            *cursor = '\0';
                            break;
                        }
                    }

                    const std::int32_t selectedMode = Runtime_StringToLong(number, nullptr, 10);

                    if (selectedMode == 1 || selectedMode == 2 || selectedMode == 3)
                    {
                        return selectedMode;
                    }
                }
            }
        }

        std::uint32_t linkerVersion;
        Runtime_GetExecutableLinkerVersion(&linkerVersion);
        return (linkerVersion & 0xFFu) >= 6u ? 3 : 2;
    }

    // Original function: FUN_004ec5de
    int Runtime_InitializeHeap(std::int32_t enableSerialization)
    {
        const DWORD options = enableSerialization == 0 ? HEAP_NO_SERIALIZE : 0;
        gRuntimeHeap = HeapCreate(options, 0x1000, 0);

        if (gRuntimeHeap == nullptr)
        {
            return 0;
        }

        gRuntimeHeapMode = static_cast<std::uint32_t>(Runtime_SelectHeapMode());

        int initialized = 1;

        if (gRuntimeHeapMode == 3)
        {
            initialized = Runtime_InitializeSmallBlockHeap(0x3F8);
        }
        else if (gRuntimeHeapMode == 2)
        {
            initialized = Runtime_CreateSmallBlockRegionMode2() != nullptr ? 1 : 0;
        }

        if (initialized == 0)
        {
            // The original leaves the stored handle unchanged after destruction.
            HeapDestroy(gRuntimeHeap);
            return 0;
        }

        return 1;
    }

    // Original function: FUN_004ec80c.
    void Runtime_WriteErrorBanner()
    {
        if (gRuntimeErrorOutputMode == 1 || (gRuntimeErrorOutputMode == 0 && gRuntimeApplicationType == 1))
        {
            Runtime_WriteErrorMessage(252);
            if (gRuntimeErrorBannerCallback != nullptr)
                gRuntimeErrorBannerCallback();
            Runtime_WriteErrorMessage(255);
        }
    }

    // Original function: FUN_004ec845.
    void Runtime_WriteErrorMessage(int errorCode)
    {
        const char* message = nullptr;
        for (const auto& entry : gRuntimeFatalMessages)
            if (entry.code == errorCode)
            {
                message = entry.text;
                break;
            }
        if (message == nullptr)
            return;
        if (gRuntimeErrorOutputMode == 1 || (gRuntimeErrorOutputMode == 0 && gRuntimeApplicationType == 1))
        {
            DWORD written;
            WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, static_cast<DWORD>(std::strlen(message)), &written, nullptr);
        }
        else if (errorCode != 252)
        {
            char path[261] = {};
            if (GetModuleFileNameA(nullptr, path, 260) == 0)
                std::strcpy(path, "<program name unknown>");
            // One extra zero byte makes legacy Win32 truncation safe for strlen.
            char* shortPath = path;
            const std::size_t length = std::strlen(path);
            if (length + 1 > 60)
            {
                shortPath += length - 59;
                std::memcpy(shortPath, "...", 3);
            }
            char buffer[160];
            std::strcpy(buffer, "Runtime Error!\n\nProgram: ");
            std::strcat(buffer, shortPath);
            std::strcat(buffer, "\n\n");
            std::strcat(buffer, message);
            Runtime_ShowErrorDialog(buffer, "Microsoft Visual C++ Runtime Library", 0x12010);
        }
    }

    // Original function: FUN_004ec998
    int Runtime_MapString(LCID locale, DWORD mapFlags, const char* source, int sourceLength, char* destination, int destinationCapacity, UINT codePage, int rejectInvalidCharacters)
    {
        std::uint32_t backend = static_cast<std::uint32_t>(gRuntimeMapStringBackend);

        if (backend == 0)
        {
            if (::LCMapStringW(0, 0x100u, L"", 1, nullptr, 0) != 0)
            {
                backend = 1;
            }
            else
            {
                if (::LCMapStringA(0, 0x100u, "", 1, nullptr, 0) == 0)
                    return 0;

                backend = 2;
            }

            gRuntimeMapStringBackend = backend;
        }

        if (sourceLength > 0)
            sourceLength = Runtime_BoundedStringLength(source, sourceLength);

        if (backend == 2)
            return ::LCMapStringA(locale, mapFlags, source, sourceLength, destination, destinationCapacity);

        if (backend != 1)
            return 0;

        if (codePage == 0)
            codePage = static_cast<UINT>(gRuntimeCodePage);

        const DWORD queryFlags = rejectInvalidCharacters != 0 ? 0x09u : 0x01u;
        const int wideSourceLength = ::MultiByteToWideChar(codePage, queryFlags, source, sourceLength, nullptr, 0);

        if (wideSourceLength == 0)
            return 0;

        WCHAR* wideSource = nullptr;

        __try
        {
            const std::uint32_t byteCount = static_cast<std::uint32_t>(wideSourceLength) * 2u;
            const std::uint32_t allocationSize = (byteCount + 3u) & 0xFFFFFFFCu;

            wideSource = static_cast<WCHAR*>(_alloca(allocationSize));
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            wideSource = nullptr;
        }

        if (wideSource == nullptr)
            return 0;

        // The actual conversion always uses 0x01.
        if (::MultiByteToWideChar(codePage, 0x01u, source, sourceLength, wideSource, wideSourceLength) == 0)
            return 0;

        // Preserve the queried source length rather than using the conversion result.
        const int mappedLength = ::LCMapStringW(locale, mapFlags, wideSource, wideSourceLength, nullptr, 0);

        if (mappedLength == 0)
            return 0;

        if ((mapFlags & 0x400u) != 0)
        {
            // Sort keys are byte sequences written directly to the caller's buffer.
            if (destinationCapacity == 0)
                return mappedLength;

            if (mappedLength > destinationCapacity)
                return 0;

            if (::LCMapStringW(locale, mapFlags, wideSource, wideSourceLength, reinterpret_cast<LPWSTR>(destination), destinationCapacity) == 0)
                return 0;

            return mappedLength;
        }

        WCHAR* wideDestination = nullptr;

        __try
        {
            const std::uint32_t byteCount = static_cast<std::uint32_t>(mappedLength) * 2u;
            const std::uint32_t allocationSize = (byteCount + 3u) & 0xFFFFFFFCu;

            wideDestination = static_cast<WCHAR*>(_alloca(allocationSize));
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            wideDestination = nullptr;
        }

        if (wideDestination == nullptr)
            return 0;

        if (::LCMapStringW(locale, mapFlags, wideSource, wideSourceLength, wideDestination, mappedLength) == 0)
            return 0;

        // The original uses the queried mapping length for conversion back to bytes.
        if (destinationCapacity == 0)
            return ::WideCharToMultiByte(codePage, 0x220u, wideDestination, mappedLength, nullptr, 0, nullptr, nullptr);

        return ::WideCharToMultiByte(codePage, 0x220u, wideDestination, mappedLength, destination, destinationCapacity, nullptr, nullptr);
    }

    // Original function: FUN_004ecbbc
    int Runtime_SetMultibyteCodePage(std::int32_t requestedCodePage)
    {
        Runtime_Lock(0x19);

        const UINT codePage = Runtime_ResolveMultibyteCodePage(requestedCodePage);

        if (codePage == gRuntimeMultibyteCodePage)
        {
            Runtime_Unlock(0x19);
            return 0;
        }

        if (codePage == 0)
        {
            Runtime_ResetMultibyteCodePage();
            Runtime_UpdateMultibyteCaseTables();
            Runtime_Unlock(0x19);
            return 0;
        }

        const RuntimeMultibyteProfile* profile = nullptr;

        for (const auto& candidate : gRuntimeMultibyteProfiles)
        {
            if (candidate.codePage == codePage)
            {
                profile = &candidate;
                break;
            }
        }

        if (profile != nullptr)
        {
            std::memset(gRuntimeMultibyteTypes, 0, sizeof(gRuntimeMultibyteTypes));

            for (unsigned category = 0; category < 4; ++category)
            {
                const auto* ranges = profile->ranges[category];

                for (unsigned pair = 0; pair < 8 && ranges[pair] != 0; pair += 2)
                {
                    if (ranges[pair + 1] == 0)
                        break;

                    for (unsigned character = ranges[pair]; character <= ranges[pair + 1]; ++character)
                        gRuntimeMultibyteTypes[character + 1] |= gRuntimeMultibyteRangeMasks[category];
                }
            }

            gRuntimeMultibyteCodePage = codePage;
            gRuntimeIsMultibyteCodePage = 1;
            gRuntimeMultibyteLocale = Runtime_GetMultibyteLocale(codePage);

            std::memcpy(gRuntimeMultibyteFullWidthCase, profile->fullWidthCase, sizeof(gRuntimeMultibyteFullWidthCase));
        }
        else
        {
            CPINFO information;

            if (GetCPInfo(codePage, &information) != 1)
            {
                if (gRuntimeSystemCodePageRequested == 0)
                {
                    Runtime_Unlock(0x19);
                    return -1;
                }

                Runtime_ResetMultibyteCodePage();
                Runtime_UpdateMultibyteCaseTables();
                Runtime_Unlock(0x19);
                return 0;
            }

            std::memset(gRuntimeMultibyteTypes, 0, sizeof(gRuntimeMultibyteTypes));

            gRuntimeMultibyteCodePage = codePage;
            gRuntimeMultibyteLocale = 0;
            gRuntimeIsMultibyteCodePage = 0;

            if (information.MaxCharSize > 1)
            {
                for (unsigned pair = 0; pair < MAX_LEADBYTES && information.LeadByte[pair] != 0; pair += 2)
                {
                    if (information.LeadByte[pair + 1] == 0)
                        break;

                    for (unsigned character = information.LeadByte[pair]; character <= information.LeadByte[pair + 1]; ++character)
                        gRuntimeMultibyteTypes[character + 1] |= 0x04u;
                }

                // The original excludes byte values 0 and 255.
                for (unsigned character = 1; character < 255; ++character)
                    gRuntimeMultibyteTypes[character + 1] |= 0x08u;

                gRuntimeMultibyteLocale = Runtime_GetMultibyteLocale(codePage);
                gRuntimeIsMultibyteCodePage = 1;
            }

            std::memset(gRuntimeMultibyteFullWidthCase, 0, sizeof(gRuntimeMultibyteFullWidthCase));
        }

        Runtime_UpdateMultibyteCaseTables();

        Runtime_Unlock(0x19);
        return 0;
    }

    // Original function: FUN_004ecd69
    UINT Runtime_ResolveMultibyteCodePage(std::int32_t requestedCodePage)
    {
        gRuntimeSystemCodePageRequested = 0;

        switch (requestedCodePage)
        {
            case -2:
                gRuntimeSystemCodePageRequested = 1;
                return GetOEMCP();

            case -3:
                gRuntimeSystemCodePageRequested = 1;
                return GetACP();

            case -4:
                gRuntimeSystemCodePageRequested = 1;
                return gRuntimeCodePage;

            default:
                return static_cast<UINT>(requestedCodePage);
        }
    }

    // Original function: FUN_004ecdb3
    LCID Runtime_GetMultibyteLocale(UINT codePage)
    {
        switch (codePage)
        {
            case 932:
                return 0x0411;
            case 936:
                return 0x0804;
            case 949:
                return 0x0412;
            case 950:
                return 0x0404;
            default:
                return 0;
        }
    }

    // Original function: FUN_004ecde6
    void Runtime_ResetMultibyteCodePage()
    {
        std::memset(gRuntimeMultibyteTypes, 0, sizeof(gRuntimeMultibyteTypes));
        std::memset(gRuntimeMultibyteFullWidthCase, 0, sizeof(gRuntimeMultibyteFullWidthCase));

        gRuntimeMultibyteCodePage = 0;
        gRuntimeIsMultibyteCodePage = 0;
        gRuntimeMultibyteLocale = 0;
    }

    // Original function: FUN_004ece0f
    void Runtime_UpdateMultibyteCaseTables()
    {
        CPINFO information;

        if (GetCPInfo(gRuntimeMultibyteCodePage, &information) == 1)
        {
            char characters[256];
            char lowercase[256];
            char uppercase[256];
            WORD characterTypes[256];

            for (unsigned character = 0; character < 256; ++character)
                characters[character] = static_cast<char>(character);

            characters[0] = ' ';

            for (unsigned pair = 0; pair < MAX_LEADBYTES && information.LeadByte[pair] != 0; pair += 2)
            {
                for (unsigned character = information.LeadByte[pair]; character <= information.LeadByte[pair + 1]; ++character)
                    characters[character] = ' ';
            }

            // The original does not check these three return values.
            Runtime_GetStringType(1, characters, 256, characterTypes, gRuntimeMultibyteCodePage, gRuntimeMultibyteLocale, 0);
            Runtime_MapString(gRuntimeMultibyteLocale, 0x100, characters, 256, lowercase, 256, gRuntimeMultibyteCodePage, 0);
            Runtime_MapString(gRuntimeMultibyteLocale, 0x200, characters, 256, uppercase, 256, gRuntimeMultibyteCodePage, 0);

            for (unsigned character = 0; character < 256; ++character)
            {
                if ((characterTypes[character] & 0x01u) != 0)
                {
                    gRuntimeMultibyteTypes[character + 1] |= 0x10u;
                    gRuntimeMultibyteCaseMap[character] = static_cast<std::uint8_t>(lowercase[character]);
                }
                else if ((characterTypes[character] & 0x02u) != 0)
                {
                    gRuntimeMultibyteTypes[character + 1] |= 0x20u;
                    gRuntimeMultibyteCaseMap[character] = static_cast<std::uint8_t>(uppercase[character]);
                }
                else
                {
                    gRuntimeMultibyteCaseMap[character] = 0;
                }
            }
        }
        else
        {
            for (unsigned character = 0; character < 256; ++character)
            {
                if (character >= 'A' && character <= 'Z')
                {
                    gRuntimeMultibyteTypes[character + 1] |= 0x10u;
                    gRuntimeMultibyteCaseMap[character] = static_cast<std::uint8_t>(character + 0x20u);
                }
                else if (character >= 'a' && character <= 'z')
                {
                    gRuntimeMultibyteTypes[character + 1] |= 0x20u;
                    gRuntimeMultibyteCaseMap[character] = static_cast<std::uint8_t>(character - 0x20u);
                }
                else
                {
                    gRuntimeMultibyteCaseMap[character] = 0;
                }
            }
        }
    }

    // Original function: FUN_004ecf94
    void Runtime_InitializeMultibyte()
    {
        if (gRuntimeMultibyteInitialized == 0)
        {
            Runtime_SetMultibyteCodePage(-3);
            gRuntimeMultibyteInitialized = 1;
        }
    }

    // Original function: FUN_004ecfb0
    char* Runtime_CopyMultibyteBytes(char* destination, const char* source, std::uint32_t byteCount)
    {
        if (gRuntimeIsMultibyteCodePage == 0)
        {
            return std::strncpy(destination, source, byteCount);
        }

        Runtime_Lock(0x19);

        char* output = destination;
        const auto* input = reinterpret_cast<const std::uint8_t*>(source);
        std::uint32_t remaining = byteCount;

        while (remaining != 0)
        {
            const std::uint8_t character = *input++;
            --remaining;
            *output++ = static_cast<char>(character);

            if ((gRuntimeMultibyteTypes[character + 1u] & 0x04u) != 0)
            {
                if (remaining == 0)
                {
                    // Remove a lead byte that would be truncated by the byte limit.
                    output[-1] = '\0';
                    break;
                }

                const std::uint8_t trailingCharacter = *input++;
                --remaining;
                *output++ = static_cast<char>(trailingCharacter);

                if (trailingCharacter == 0)
                {
                    // Remove an incomplete multibyte character at the source terminator.
                    output[-2] = '\0';
                    break;
                }
            }
            else if (character == 0)
            {
                break;
            }
        }

        if (remaining != 0)
        {
            std::memset(output, 0, remaining);
        }

        Runtime_Unlock(0x19);
        return destination;
    }

    // Original function: FUN_004ed04a
    const char* Runtime_PreviousMultibyteCharacter(const char* start, const char* current)
    {
        const std::uintptr_t begin = reinterpret_cast<std::uintptr_t>(start);
        const std::uintptr_t position = reinterpret_cast<std::uintptr_t>(current);

        if (begin >= position)
        {
            return nullptr;
        }

        if (gRuntimeIsMultibyteCodePage == 0)
        {
            return current - 1;
        }

        Runtime_Lock(0x19);

        std::uintptr_t scan = position - 1;

        if ((gRuntimeMultibyteTypes[*reinterpret_cast<const std::uint8_t*>(scan) + 1u] & 4u) != 0)
        {
            Runtime_Unlock(0x19);

            // Preserve the original immediate two-byte step.
            return reinterpret_cast<const char*>(scan - 1);
        }

        --scan;

        // Integer addresses preserve the original scan below the lower bound
        // without constructing an out-of-range pointer for the comparison.
        while (scan >= begin && (gRuntimeMultibyteTypes[*reinterpret_cast<const std::uint8_t*>(scan) + 1u] & 4u) != 0)
        {
            --scan;
        }

        Runtime_Unlock(0x19);

        const std::uintptr_t adjustment = (position - scan) & 1u;

        return reinterpret_cast<const char*>(position - adjustment - 1u);
    }

    // Original function: FUN_004ed45a
    int Runtime_InitializeSmallBlockHeap(std::uint32_t maximumRequest)
    {
        void* table = HeapAlloc(gRuntimeHeap, 0, 0x140);
        gRuntimeSmallBlockRegionTable = table;

        if (table == nullptr)
        {
            return 0;
        }

        gRuntimeSmallBlockPendingRegion = 0;
        gRuntimeSmallBlockRegionCount = 0;
        gRuntimeSmallBlockSearchRegion = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(table));
        gRuntimeSmallBlockThreshold = maximumRequest;
        gRuntimeSmallBlockRegionCapacity = 0x10;

        return 1;
    }

    // Original function: FUN_004ed4a2
    std::uint32_t Runtime_FindSmallBlockRegion(std::uint32_t memoryAddress)
    {
        const auto read32 = [] (std::uintptr_t address) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(value));
            return value;
        };

        const std::uint32_t count = read32(reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockRegionCount));
        const std::uint32_t table = read32(reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockRegionTable));
        const std::uint32_t end = table + count * 0x14u;

        for (std::uint32_t region = table; region < end; region += 0x14u)
        {
            const std::uint32_t offset = memoryAddress - read32(region + 0x0C);

            if (offset < 0x100000u)
            {
                return region;
            }
        }

        return 0;
    }

    // Original function: FUN_004ed4cd
    void Runtime_FreeSmallBlock(std::uint32_t regionAddress, void* memory)
    {
        const auto read32 = [] (std::uintptr_t address) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(value));
            return value;
        };

        const auto write32 = [] (std::uintptr_t address, std::uint32_t value)
        {
            std::memcpy(reinterpret_cast<void*>(address), &value, sizeof(value));
        };

        const auto read8 = [] (std::uintptr_t address) -> std::uint8_t
        {
            return *reinterpret_cast<const std::uint8_t*>(address);
        };

        const auto write8 = [] (std::uintptr_t address, std::uint8_t value)
        {
            *reinterpret_cast<std::uint8_t*>(address) = value;
        };

        const auto asSigned = [] (std::uint32_t value) -> std::int32_t
        {
            std::int32_t result;
            std::memcpy(&result, &value, sizeof(result));
            return result;
        };

        const auto sizeClass = [&] (std::uint32_t size) -> std::uint32_t
        {
            const std::uint32_t index = static_cast<std::uint32_t>(asSigned(size) >> 4) - 1u;
            return index > 0x3Fu ? 0x3Fu : index;
        };

        std::uintptr_t region = regionAddress;
        const std::uint32_t memoryAddress = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(memory));
        const std::uintptr_t metadata = read32(region + 0x10);
        const std::uint32_t groupIndex = (memoryAddress - read32(region + 0x0C)) >> 15;
        const std::uintptr_t group = metadata + 0x144 + groupIndex * 0x204u;

        std::uintptr_t block = static_cast<std::uint32_t>(memoryAddress - 4u);
        std::uint32_t blockSize = read32(block) - 1u;

        if ((blockSize & 1u) != 0)
        {
            return;
        }

        const std::uintptr_t nextBlock = block + blockSize;
        const std::uint32_t nextSize = read32(nextBlock);
        const std::uint32_t previousSize = read32(block - 4);

        const auto removeFreeBlock = [&] (std::uintptr_t freeBlock, std::uint32_t index)
        {
            if (read32(freeBlock + 0x04) == read32(freeBlock + 0x08))
            {
                const std::uint32_t mask = ~(0x80000000u >> (index & 31u));
                const std::uintptr_t groupMap = metadata + (index < 32u ? 0x44u : 0xC4u) + groupIndex * 4u;
                const std::uintptr_t regionMap = region + (index < 32u ? 0u : 4u);
                const std::uintptr_t counter = metadata + 0x04 + index;

                write32(groupMap, read32(groupMap) & mask);
                write8(counter, static_cast<std::uint8_t>(read8(counter) - 1u));

                if (read8(counter) == 0)
                {
                    write32(regionMap, read32(regionMap) & mask);
                }
            }

            write32(read32(freeBlock + 0x08) + 0x04, read32(freeBlock + 0x04));
            write32(read32(freeBlock + 0x04) + 0x08, read32(freeBlock + 0x08));
        };

        // Merge with the following block if its allocation bit is clear.
        if ((nextSize & 1u) == 0)
        {
            removeFreeBlock(nextBlock, sizeClass(nextSize));
            blockSize += nextSize;
        }

        std::uint32_t mergedClass = sizeClass(blockSize);
        bool insertBlock = true;

        // A preceding free block can stay in its list if its size class is unchanged.
        if ((previousSize & 1u) == 0)
        {
            block -= previousSize;

            const std::uint32_t previousClass = sizeClass(previousSize);
            blockSize += previousSize;
            mergedClass = sizeClass(blockSize);

            if (previousClass == mergedClass)
            {
                insertBlock = false;
            }
            else
            {
                removeFreeBlock(block, previousClass);
            }
        }

        if (insertBlock)
        {
            const std::uintptr_t sentinel = group + mergedClass * 8u;

            write32(block + 0x04, read32(sentinel + 0x04));
            write32(block + 0x08, static_cast<std::uint32_t>(sentinel));
            write32(sentinel + 0x04, static_cast<std::uint32_t>(block));
            write32(read32(block + 0x04) + 0x08, static_cast<std::uint32_t>(block));

            if (read32(block + 0x04) == read32(block + 0x08))
            {
                const std::uintptr_t counter = metadata + 0x04 + mergedClass;
                const std::uint8_t previousCount = read8(counter);
                const std::uint32_t mask = 0x80000000u >> (mergedClass & 31u);
                const std::uintptr_t regionMap = region + (mergedClass < 32u ? 0u : 4u);
                const std::uintptr_t groupMap = metadata + (mergedClass < 32u ? 0x44u : 0xC4u) + groupIndex * 4u;

                write8(counter, static_cast<std::uint8_t>(previousCount + 1u));

                if (previousCount == 0)
                {
                    write32(regionMap, read32(regionMap) | mask);
                }

                write32(groupMap, read32(groupMap) | mask);
            }
        }

        write32(block, blockSize);
        write32(block + blockSize - 4, blockSize);

        write32(group, read32(group) - 1u);

        if (read32(group) != 0)
        {
            return;
        }

        const std::uintptr_t pendingRegionAddress = reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockPendingRegion);
        const std::uintptr_t pendingGroupAddress = reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockPendingGroupIndex);
        const std::uintptr_t regionCountAddress = reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockRegionCount);
        const std::uintptr_t tableAddress = reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockRegionTable);
        const std::uintptr_t searchCursorAddress = reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockSearchRegion);

        if (read32(pendingRegionAddress) != 0)
        {
            std::uintptr_t pendingRegion = read32(pendingRegionAddress);
            std::uint32_t pendingGroup = read32(pendingGroupAddress);
            const std::uint32_t groupMemory = read32(pendingRegion + 0x0C) + (pendingGroup << 15);

            // The original updates its bookkeeping even if VirtualFree fails.
            VirtualFree(reinterpret_cast<void*>(static_cast<std::uintptr_t>(groupMemory)), 0x8000, MEM_DECOMMIT);

            pendingRegion = read32(pendingRegionAddress);
            pendingGroup = read32(pendingGroupAddress);

            write32(pendingRegion + 0x08, read32(pendingRegion + 0x08) | (0x80000000u >> (pendingGroup & 31u)));

            pendingRegion = read32(pendingRegionAddress);
            pendingGroup = read32(pendingGroupAddress);
            std::uintptr_t pendingMetadata = read32(pendingRegion + 0x10);

            write32(pendingMetadata + 0xC4 + pendingGroup * 4u, 0);

            pendingRegion = read32(pendingRegionAddress);
            pendingMetadata = read32(pendingRegion + 0x10);
            write8(pendingMetadata + 0x43, static_cast<std::uint8_t>(read8(pendingMetadata + 0x43) - 1u));

            pendingRegion = read32(pendingRegionAddress);
            pendingMetadata = read32(pendingRegion + 0x10);

            if (read8(pendingMetadata + 0x43) == 0)
            {
                write32(pendingRegion + 0x04, read32(pendingRegion + 0x04) & 0xFFFFFFFEu);
                pendingRegion = read32(pendingRegionAddress);
            }

            if (read32(pendingRegion + 0x08) == 0xFFFFFFFFu)
            {
                VirtualFree(reinterpret_cast<void*>(static_cast<std::uintptr_t>(read32(pendingRegion + 0x0C))), 0, MEM_RELEASE);

                pendingRegion = read32(pendingRegionAddress);
                HeapFree(gRuntimeHeap, 0, reinterpret_cast<void*>(static_cast<std::uintptr_t>(read32(pendingRegion + 0x10))));

                const std::uint32_t table = read32(tableAddress);
                const std::uint32_t count = read32(regionCountAddress);
                const std::uintptr_t removedRegion = read32(pendingRegionAddress);
                const std::uint32_t bytesToMove = table + count * 0x14u - static_cast<std::uint32_t>(removedRegion) - 0x14u;

                std::memmove(reinterpret_cast<void*>(removedRegion), reinterpret_cast<const void*>(removedRegion + 0x14), bytesToMove);

                write32(regionCountAddress, read32(regionCountAddress) - 1u);

                if (region > read32(pendingRegionAddress))
                {
                    region -= 0x14;
                }

                write32(searchCursorAddress, read32(tableAddress));
            }
        }

        // Keep the newly emptied group until another group becomes empty.
        write32(pendingGroupAddress, groupIndex);
        write32(pendingRegionAddress, static_cast<std::uint32_t>(region));
    }

    // Original function: FUN_004ed7f6
    void* Runtime_AllocateSmallBlock(std::uint32_t requestedBytes)
    {
        const auto read32 = [] (std::uintptr_t address) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(value));
            return value;
        };

        const auto write32 = [] (std::uintptr_t address, std::uint32_t value)
        {
            std::memcpy(reinterpret_cast<void*>(address), &value, sizeof(value));
        };

        const auto read8 = [] (std::uintptr_t address) -> std::uint8_t
        {
            std::uint8_t value;
            std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(value));
            return value;
        };

        const auto write8 = [] (std::uintptr_t address, std::uint8_t value)
        {
            std::memcpy(reinterpret_cast<void*>(address), &value, sizeof(value));
        };

        const auto asSigned = [] (std::uint32_t bits) -> std::int32_t
        {
            std::int32_t value;
            std::memcpy(&value, &bits, sizeof(value));
            return value;
        };

        const std::uintptr_t regionTable = read32(reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockRegionTable));
        const std::uint32_t regionCount = read32(reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockRegionCount));
        const std::uintptr_t regionEnd = regionTable + regionCount * 0x14u;
        const std::uintptr_t searchStart = read32(reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockSearchRegion));

        // Include the two size tags and round the block size to sixteen bytes.
        const std::uint32_t blockSize = (requestedBytes + 0x17u) & 0xFFFFFFF0u;
        const std::int32_t requestedClass = (asSigned(blockSize) >> 4) - 1;

        std::uint32_t lowMask;
        std::uint32_t highMask;

        if (requestedClass < 32)
        {
            lowMask = 0xFFFFFFFFu >> (static_cast<std::uint32_t>(requestedClass) & 31u);
            highMask = 0xFFFFFFFFu;
        }
        else
        {
            lowMask = 0;
            highMask = 0xFFFFFFFFu >> ((static_cast<std::uint32_t>(requestedClass) - 32u) & 31u);
        }

        const auto hasSuitableBlock = [&] (std::uintptr_t region) -> bool
        {
            return ((read32(region) & lowMask) | (read32(region + 0x04) & highMask)) != 0;
        };

        std::uintptr_t region = searchStart;

        while (region < regionEnd && !hasSuitableBlock(region))
            region += 0x14;

        if (region == regionEnd)
        {
            region = regionTable;

            while (region < searchStart && !hasSuitableBlock(region))
                region += 0x14;

            if (region == searchStart)
            {
                // Search for a region that can supply another group.
                while (region < regionEnd && read32(region + 0x08) == 0)
                    region += 0x14;

                if (region == regionEnd)
                {
                    region = regionTable;

                    while (region < searchStart && read32(region + 0x08) == 0)
                        region += 0x14;

                    if (region == searchStart)
                    {
                        region = reinterpret_cast<std::uintptr_t>(Runtime_CreateSmallBlockRegion());

                        if (region == 0)
                            return nullptr;
                    }
                }

                const std::int32_t group = Runtime_CommitSmallBlockGroup(static_cast<std::uint32_t>(region));

                // Reload the metadata pointer after creating the group.
                write32(read32(region + 0x10), static_cast<std::uint32_t>(group));

                if (read32(read32(region + 0x10)) == 0xFFFFFFFFu)
                    return nullptr;
            }
        }

        write32(reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockSearchRegion), static_cast<std::uint32_t>(region));

        const std::uintptr_t metadata = read32(region + 0x10);
        std::uint32_t groupIndex = read32(metadata);

        const auto groupHasSuitableBlock = [&] (std::uint32_t index) -> bool
        {
            return ((read32(metadata + 0x44u + index * 4u) & lowMask) | (read32(metadata + 0xC4u + index * 4u) & highMask)) != 0;
        };

        if (groupIndex == 0xFFFFFFFFu || !groupHasSuitableBlock(groupIndex))
        {
            groupIndex = 0;

            while (!groupHasSuitableBlock(groupIndex))
                ++groupIndex;
        }

        const std::uintptr_t group = metadata + 0x144u + groupIndex * 0x204u;

        std::uint32_t availableBits = read32(metadata + 0x44u + groupIndex * 4u) & lowMask;
        std::uint32_t sizeClass = 0;

        if (availableBits == 0)
        {
            availableBits = read32(metadata + 0xC4u + groupIndex * 4u) & highMask;
            sizeClass = 32;
        }

        // Size classes are ordered from the most significant bit downwards.
        while ((availableBits & 0x80000000u) == 0)
        {
            availableBits <<= 1;
            ++sizeClass;
        }

        std::uintptr_t block = read32(group + sizeClass * 8u + 0x04);
        const std::uint32_t remainingSize = read32(block) - blockSize;

        std::int32_t remainingClass = (asSigned(remainingSize) >> 4) - 1;

        if (remainingClass > 63)
            remainingClass = 63;

        if (remainingClass != static_cast<std::int32_t>(sizeClass))
        {
            if (read32(block + 0x04) == read32(block + 0x08))
            {
                // Removing the only block empties this group's size-class list.
                const std::uint32_t bit = 0x80000000u >> (sizeClass & 31u);
                const std::uintptr_t groupMask = metadata + (sizeClass < 32 ? 0x44u : 0xC4u) + groupIndex * 4u;

                write32(groupMask, read32(groupMask) & ~bit);

                const std::uintptr_t counterAddress = metadata + 0x04u + sizeClass;
                const std::uint8_t count = static_cast<std::uint8_t>(read8(counterAddress) - 1u);
                write8(counterAddress, count);

                if (count == 0)
                {
                    const std::uintptr_t regionMask = region + (sizeClass < 32 ? 0u : 4u);
                    write32(regionMask, read32(regionMask) & ~bit);
                }
            }

            // Unlink the block, preserving the original write order.
            write32(read32(block + 0x08) + 0x04u, read32(block + 0x04));
            write32(read32(block + 0x04) + 0x08u, read32(block + 0x08));

            if (remainingSize != 0)
            {
                const std::uint32_t newClass = static_cast<std::uint32_t>(remainingClass);
                const std::uintptr_t sentinel = group + newClass * 8u;

                // Insert the remainder at the front of its new size-class list.
                write32(block + 0x04, read32(sentinel + 0x04));
                write32(block + 0x08, static_cast<std::uint32_t>(sentinel));
                write32(sentinel + 0x04, static_cast<std::uint32_t>(block));
                write32(read32(block + 0x04) + 0x08u, static_cast<std::uint32_t>(block));

                if (read32(block + 0x04) == read32(block + 0x08))
                {
                    const std::uintptr_t counterAddress = metadata + 0x04u + newClass;
                    const std::uint8_t previousCount = read8(counterAddress);
                    write8(counterAddress, static_cast<std::uint8_t>(previousCount + 1u));

                    const std::uint32_t bit = 0x80000000u >> (newClass & 31u);

                    if (previousCount == 0)
                    {
                        const std::uintptr_t regionMask = region + (newClass < 32 ? 0u : 4u);
                        write32(regionMask, read32(regionMask) | bit);
                    }

                    const std::uintptr_t groupMask = metadata + (newClass < 32 ? 0x44u : 0xC4u) + groupIndex * 4u;
                    write32(groupMask, read32(groupMask) | bit);
                }
            }
        }

        if (remainingSize != 0)
        {
            write32(block, remainingSize);
            write32(block + remainingSize - 4u, remainingSize);
        }

        // Allocate from the high end of the original free block.
        block += remainingSize;

        const std::uint32_t allocatedTag = blockSize + 1u;
        write32(block, allocatedTag);
        write32(block + blockSize - 4u, allocatedTag);

        const std::uint32_t previousAllocationCount = read32(group);
        write32(group, previousAllocationCount + 1u);

        if (previousAllocationCount == 0 && region == read32(reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockPendingRegion)) && groupIndex == read32(reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockPendingGroupIndex)))
        {
            write32(reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockPendingRegion), 0);
        }

        write32(metadata, groupIndex);
        return reinterpret_cast<void*>(block + 4u);
    }

    // Original function: FUN_004edaff
    void* Runtime_CreateSmallBlockRegion()
    {
        const auto read32 = [] (std::uintptr_t address) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(value));
            return value;
        };

        const auto write32 = [] (std::uintptr_t address, std::uint32_t value)
        {
            std::memcpy(reinterpret_cast<void*>(address), &value, sizeof(value));
        };

        const std::uintptr_t countAddress = reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockRegionCount);
        const std::uintptr_t capacityAddress = reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockRegionCapacity);
        const std::uintptr_t tableAddress = reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockRegionTable);

        std::uint32_t regionCount = read32(countAddress);
        const std::uint32_t regionCapacity = read32(capacityAddress);

        if (regionCount == regionCapacity)
        {
            // Grow the table by sixteen entries of twenty bytes each.
            const std::uint32_t tableBytes = (regionCapacity + 16u) * 0x14u;
            void* previousTable = reinterpret_cast<void*>(static_cast<std::uintptr_t>(read32(tableAddress)));

            void* resizedTable = ::HeapReAlloc(gRuntimeHeap, 0, previousTable, tableBytes);

            if (resizedTable == nullptr)
                return nullptr;

            write32(capacityAddress, read32(capacityAddress) + 16u);
            write32(tableAddress, static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(resizedTable)));

            regionCount = read32(countAddress);
        }

        const std::uintptr_t region = read32(tableAddress) + regionCount * 0x14u;

        void* metadata = ::HeapAlloc(gRuntimeHeap, HEAP_ZERO_MEMORY, 0x41C4);
        write32(region + 0x10, static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(metadata)));

        if (metadata == nullptr)
            return nullptr;

        void* reservedMemory = ::VirtualAlloc(nullptr, 0x100000, MEM_RESERVE, PAGE_READWRITE);
        write32(region + 0x0C, static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(reservedMemory)));

        if (reservedMemory == nullptr)
        {
            // The original frees the metadata without clearing its stored pointer.
            ::HeapFree(gRuntimeHeap, 0, reinterpret_cast<void*>(static_cast<std::uintptr_t>(read32(region + 0x10))));
            return nullptr;
        }

        write32(region + 0x08, 0xFFFFFFFFu);
        write32(region, 0);
        write32(region + 0x04, 0);

        write32(countAddress, read32(countAddress) + 1u);
        write32(read32(region + 0x10), 0xFFFFFFFFu);

        return reinterpret_cast<void*>(region);
    }

    // Original function: FUN_004edbb0
    std::int32_t Runtime_CommitSmallBlockGroup(std::uint32_t regionAddress)
    {
        const auto read32 = [] (std::uintptr_t address) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(value));
            return value;
        };

        const auto write32 = [] (std::uintptr_t address, std::uint32_t value)
        {
            std::memcpy(reinterpret_cast<void*>(address), &value, sizeof(value));
        };

        const std::uintptr_t region = regionAddress;
        const std::uintptr_t metadata = read32(region + 0x10);

        std::uint32_t availableGroups = read32(region + 0x08);
        std::uint32_t groupIndex = 0;

        // The caller supplies a region with at least one available group.
        while ((availableGroups & 0x80000000u) == 0)
        {
            availableGroups <<= 1;
            ++groupIndex;
        }

        const std::uintptr_t group = metadata + 0x144u + groupIndex * 0x204u;

        // Initialize the first 63 size-class sentinels before committing memory.
        for (std::uint32_t sizeClass = 0; sizeClass < 63u; ++sizeClass)
        {
            const std::uintptr_t sentinel = group + sizeClass * 8u;

            write32(sentinel + 0x08, static_cast<std::uint32_t>(sentinel));
            write32(sentinel + 0x04, static_cast<std::uint32_t>(sentinel));
        }

        const std::uintptr_t groupMemory = read32(region + 0x0C) + (groupIndex << 15);

        if (::VirtualAlloc(reinterpret_cast<void*>(groupMemory), 0x8000, MEM_COMMIT, PAGE_READWRITE) == nullptr)
            return -1;

        // Each page contributes one free block with two boundary tags.
        for (std::uint32_t pageIndex = 0; pageIndex < 8u; ++pageIndex)
        {
            const std::uintptr_t page = groupMemory + pageIndex * 0x1000u;
            const std::uintptr_t block = page + 0x0C;

            write32(page + 0x08, 0xFFFFFFFFu);
            write32(page + 0xFFC, 0xFFFFFFFFu);

            write32(block, 0xFF0u);
            write32(block + 0x04, static_cast<std::uint32_t>(block + 0x1000u));
            write32(block + 0x08, static_cast<std::uint32_t>(block - 0x1000u));
            write32(page + 0xFF8, 0xFF0u);
        }

        // Connect the eight blocks to the final size-class sentinel.
        const std::uintptr_t sentinel = group + 0x1F8u;
        const std::uintptr_t firstBlock = groupMemory + 0x0C;
        const std::uintptr_t lastBlock = groupMemory + 0x700Cu;

        write32(sentinel + 0x04, static_cast<std::uint32_t>(firstBlock));
        write32(firstBlock + 0x08, static_cast<std::uint32_t>(sentinel));
        write32(sentinel + 0x08, static_cast<std::uint32_t>(lastBlock));
        write32(lastBlock + 0x04, static_cast<std::uint32_t>(sentinel));

        write32(metadata + 0x44u + groupIndex * 4u, 0);
        write32(metadata + 0xC4u + groupIndex * 4u, 1);

        const std::uintptr_t counterAddress = metadata + 0x43u;

        std::uint8_t previousCount;
        std::memcpy(&previousCount, reinterpret_cast<const void*>(counterAddress), sizeof(previousCount));

        const std::uint8_t updatedCount = static_cast<std::uint8_t>(previousCount + 1u);
        std::memcpy(reinterpret_cast<void*>(counterAddress), &updatedCount, sizeof(updatedCount));

        if (previousCount == 0)
            write32(region + 0x04, read32(region + 0x04) | 1u);

        const std::uint32_t groupBit = 0x80000000u >> (groupIndex & 31u);
        write32(region + 0x08, read32(region + 0x08) & ~groupBit);

        return static_cast<std::int32_t>(groupIndex);
    }

    // Original function: FUN_004edcab
    int Runtime_ResizeSmallBlock(std::uint32_t regionAddress, void* memory, std::uint32_t requestedBytes)
    {
        auto* region = reinterpret_cast<RuntimeSmallBlockRegion*>(regionAddress);

        const auto address = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(memory));
        const auto reservation = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(region->reservation));
        const auto metadata = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(region->metadata));

        const std::uint32_t groupIndex = (address - reservation) >> 15;
        const std::uint32_t groupAddress = metadata + 0x144u + groupIndex * 0x204u;

        const auto word = [] (std::uint32_t location) -> std::uint32_t&
        {
            return *reinterpret_cast<std::uint32_t*>(location);
        };

        const auto sizeClass = [] (std::uint32_t bytes) -> std::uint32_t
        {
            const std::uint32_t index = (bytes >> 4) - 1u;
            return index < 63u ? index : 63u;
        };

        const auto removeFreeBlock = [&] (std::uint32_t block, std::uint32_t bytes)
        {
            const std::uint32_t index = sizeClass(bytes);
            const std::uint32_t next = word(block + 4u);
            const std::uint32_t previous = word(block + 8u);

            if (next == previous)
            {
                const std::uint32_t mask = 0x80000000u >> (index & 31u);
                const std::uint32_t bitmap = metadata + (index < 32u ? 0x44u : 0xC4u) + groupIndex * 4u;

                word(bitmap) &= ~mask;

                auto& count = *reinterpret_cast<std::uint8_t*>(metadata + 4u + index);

                --count;

                if (count == 0)
                {
                    if (index < 32u)
                        region->availableClassesLow &= ~mask;
                    else
                        region->availableClassesHigh &= ~mask;
                }
            }

            word(previous + 4u) = next;
            word(next + 8u) = previous;
        };

        const auto insertFreeBlock = [&] (std::uint32_t block, std::uint32_t bytes)
        {
            const std::uint32_t index = sizeClass(bytes);
            const std::uint32_t sentinel = groupAddress + index * 8u;
            const std::uint32_t next = word(sentinel + 4u);

            word(block + 4u) = next;
            word(block + 8u) = sentinel;
            word(sentinel + 4u) = block;
            word(next + 8u) = block;

            if (next == sentinel)
            {
                const std::uint32_t mask = 0x80000000u >> (index & 31u);
                const std::uint32_t bitmap = metadata + (index < 32u ? 0x44u : 0xC4u) + groupIndex * 4u;

                auto& count = *reinterpret_cast<std::uint8_t*>(metadata + 4u + index);
                const std::uint8_t previousCount = count++;

                if (previousCount == 0)
                {
                    if (index < 32u)
                        region->availableClassesLow |= mask;
                    else
                        region->availableClassesHigh |= mask;
                }

                word(bitmap) |= mask;
            }

            word(block) = bytes;
            word(block + bytes - 4u) = bytes;
        };

        const std::uint32_t oldTotal = word(address - 4u) - 1u;
        const std::uint32_t newTotal = (requestedBytes + 0x17u) & 0xFFFFFFF0u;

        if (newTotal == oldTotal)
            return 1;

        const std::uint32_t followingBlock = address + oldTotal - 4u;
        const std::uint32_t followingSize = word(followingBlock);

        if (newTotal > oldTotal)
        {
            if ((followingSize & 1u) != 0 || newTotal > oldTotal + followingSize)
                return 0;

            removeFreeBlock(followingBlock, followingSize);

            const std::uint32_t remainder = oldTotal + followingSize - newTotal;

            if (remainder != 0)
                insertFreeBlock(address + newTotal - 4u, remainder);

            word(address - 4u) = newTotal + 1u;
            word(address + newTotal - 8u) = newTotal + 1u;
        }
        else
        {
            std::uint32_t remainder = oldTotal - newTotal;

            word(address - 4u) = newTotal + 1u;
            word(address + newTotal - 8u) = newTotal + 1u;

            if ((followingSize & 1u) == 0)
            {
                removeFreeBlock(followingBlock, followingSize);
                remainder += followingSize;
            }

            insertFreeBlock(address + newTotal - 4u, remainder);
        }

        return 1;
    }

    // Original function: FUN_004edfa1
    void* Runtime_CreateSmallBlockRegionMode2()
    {
        const auto read32 = [] (std::uintptr_t address) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(value));
            return value;
        };

        const auto write32 = [] (std::uintptr_t address, std::uint32_t value)
        {
            std::memcpy(reinterpret_cast<void*>(address), &value, sizeof(value));
        };

        const std::uintptr_t firstRegion = reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockInitialRegionMode2);
        std::uintptr_t region = firstRegion;

        if (read32(firstRegion + 0x10) != 0xFFFFFFFFu)
        {
            void* metadata = HeapAlloc(gRuntimeHeap, 0, 0x2020);

            if (metadata == nullptr)
            {
                return nullptr;
            }

            region = reinterpret_cast<std::uintptr_t>(metadata);
        }

        void* reserved = VirtualAlloc(nullptr, 0x400000, MEM_RESERVE, PAGE_READWRITE);

        if (reserved == nullptr)
        {
            if (region != firstRegion)
            {
                HeapFree(gRuntimeHeap, 0, reinterpret_cast<void*>(region));
            }

            return nullptr;
        }

        void* committed = VirtualAlloc(reserved, 0x10000, MEM_COMMIT, PAGE_READWRITE);

        if (committed == nullptr)
        {
            VirtualFree(reserved, 0, MEM_RELEASE);

            if (region != firstRegion)
            {
                HeapFree(gRuntimeHeap, 0, reinterpret_cast<void*>(region));
            }

            return nullptr;
        }

        if (region == firstRegion)
        {
            if (read32(firstRegion) == 0)
            {
                write32(firstRegion, static_cast<std::uint32_t>(firstRegion));
            }

            if (read32(firstRegion + 0x04) == 0)
            {
                write32(firstRegion + 0x04, static_cast<std::uint32_t>(firstRegion));
            }
        }
        else
        {
            // Append the new region immediately before the static first region.
            write32(region, static_cast<std::uint32_t>(firstRegion));
            write32(region + 0x04, read32(firstRegion + 0x04));
            write32(firstRegion + 0x04, static_cast<std::uint32_t>(region));
            write32(read32(region + 0x04), static_cast<std::uint32_t>(region));
        }

        const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(reserved);

        write32(region + 0x14, static_cast<std::uint32_t>(base + 0x400000));
        write32(region + 0x0C, static_cast<std::uint32_t>(region + 0x98));
        write32(region + 0x10, static_cast<std::uint32_t>(base));
        write32(region + 0x08, static_cast<std::uint32_t>(region + 0x18));

        for (std::uint32_t index = 0; index < 0x400; ++index)
        {
            const std::uintptr_t entry = region + 0x18 + index * 8u;

            write32(entry, index < 0x10 ? 0xF0u : 0xFFFFFFFFu);
            write32(entry + 0x04, 0xF1);
        }

        std::memset(reserved, 0, 0x10000);

        for (std::uint32_t index = 0; index < 0x10; ++index)
        {
            const std::uintptr_t page = base + (index << 12);

            *reinterpret_cast<std::uint8_t*>(page + 0xF8) = 0xFF;
            write32(page, static_cast<std::uint32_t>(page + 0x08));
            write32(page + 0x04, 0xF0);
        }

        return reinterpret_cast<void*>(region);
    }

    // Original function: FUN_004ee0e5
    void Runtime_ReleaseSmallBlockRegionMode2(std::uint32_t regionAddress)
    {
        const auto read32 = [] (std::uintptr_t address) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(value));
            return value;
        };

        const auto write32 = [] (std::uintptr_t address, std::uint32_t value)
        {
            std::memcpy(reinterpret_cast<void*>(address), &value, sizeof(value));
        };

        const std::uintptr_t region = regionAddress;
        const std::uintptr_t firstRegion = reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockInitialRegionMode2);
        const std::uintptr_t cursorAddress = reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockSearchRegionMode2);

        VirtualFree(reinterpret_cast<void*>(static_cast<std::uintptr_t>(read32(region + 0x10))), 0, MEM_RELEASE);

        if (read32(cursorAddress) == regionAddress)
        {
            write32(cursorAddress, read32(region + 0x04));
        }

        if (region == firstRegion)
        {
            // The static region remains linked and only loses its reservation.
            write32(firstRegion + 0x10, 0xFFFFFFFFu);
            return;
        }

        write32(read32(region + 0x04), read32(region));
        write32(read32(region) + 0x04, read32(region + 0x04));

        HeapFree(gRuntimeHeap, 0, reinterpret_cast<void*>(region));
    }

    // Original function: FUN_004ee13b
    void Runtime_DecommitSmallBlockPagesMode2(std::uint32_t pagesToRelease)
    {
        const auto read32 = [] (std::uintptr_t address) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(value));
            return value;
        };

        const auto write32 = [] (std::uintptr_t address, std::uint32_t value)
        {
            std::memcpy(reinterpret_cast<void*>(address), &value, sizeof(value));
        };

        const auto asSigned = [] (std::uint32_t value) -> std::int32_t
        {
            std::int32_t result;
            std::memcpy(&result, &value, sizeof(result));
            return result;
        };

        const std::uintptr_t firstRegion = reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockInitialRegionMode2);
        const std::uintptr_t emptyPageCountAddress = reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockEmptyPageCountMode2);

        std::uintptr_t region = read32(firstRegion + 0x04);

        for (;;)
        {
            if (read32(region + 0x10) != 0xFFFFFFFFu)
            {
                std::uint32_t releasedPages = 0;

                // Visit pages from the end of the region toward its beginning.
                for (std::int32_t index = 0x3FF; index >= 0; --index)
                {
                    const std::uintptr_t entry = region + 0x18 + static_cast<std::uint32_t>(index) * 8u;

                    if (read32(entry) != 0xF0u)
                    {
                        continue;
                    }

                    const std::uint32_t pageAddress = read32(region + 0x10) + (static_cast<std::uint32_t>(index) << 12);

                    if (VirtualFree(reinterpret_cast<void*>(static_cast<std::uintptr_t>(pageAddress)), 0x1000, MEM_DECOMMIT) == FALSE)
                    {
                        continue;
                    }

                    write32(entry, 0xFFFFFFFFu);
                    write32(emptyPageCountAddress, read32(emptyPageCountAddress) - 1u);

                    const std::uintptr_t firstUncommittedEntry = read32(region + 0x0C);

                    if (firstUncommittedEntry == 0 || firstUncommittedEntry > entry)
                    {
                        write32(region + 0x0C, static_cast<std::uint32_t>(entry));
                    }

                    ++releasedPages;
                    --pagesToRelease;

                    if (pagesToRelease == 0)
                    {
                        break;
                    }
                }

                const std::uintptr_t processedRegion = region;
                region = read32(processedRegion + 0x04);

                if (releasedPages != 0)
                {
                    std::uint32_t index = 0;

                    while (index < 0x400u && read32(processedRegion + 0x18 + index * 8u) == 0xFFFFFFFFu)
                    {
                        ++index;
                    }

                    if (index == 0x400u)
                    {
                        Runtime_ReleaseSmallBlockRegionMode2(static_cast<std::uint32_t>(processedRegion));
                    }
                }
            }

            // Reload the tail because releasing a region can change it.
            if (region == read32(firstRegion + 0x04) || asSigned(pagesToRelease) <= 0)
            {
                return;
            }
        }
    }

    // Original function: FUN_004ee1fd
    std::uint32_t Runtime_FindSmallBlockEntryMode2(void* memory, std::uint32_t* regionOutput, std::uint32_t* pageOutput)
    {
        const auto read32 = [] (std::uintptr_t address) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(value));
            return value;
        };

        const std::uint32_t address = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(memory));
        const std::uintptr_t firstRegion = reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockInitialRegionMode2);
        std::uintptr_t region = firstRegion;

        do
        {
            if (address > read32(region + 0x10) && address < read32(region + 0x14))
            {
                if ((address & 0x0Fu) != 0)
                {
                    return 0;
                }

                const std::uint32_t pageOffset = address & 0xFFFu;

                if (pageOffset < 0x100u)
                {
                    return 0;
                }

                const std::uint32_t page = address & 0xFFFFF000u;

                *regionOutput = static_cast<std::uint32_t>(region);
                *pageOutput = page;

                return page + 0x08u + ((pageOffset - 0x100u) >> 4);
            }

            region = read32(region);
        } while (region != firstRegion);

        return 0;
    }

    // Original function: FUN_004ee254
    void Runtime_FreeSmallBlockMode2(std::uint32_t regionAddress, std::uint32_t pageAddress, std::uint32_t entryAddress)
    {
        const auto read32 = [] (std::uintptr_t address) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(value));
            return value;
        };

        const auto write32 = [] (std::uintptr_t address, std::uint32_t value)
        {
            std::memcpy(reinterpret_cast<void*>(address), &value, sizeof(value));
        };

        const auto asSigned = [] (std::uint32_t value) -> std::int32_t
        {
            std::int32_t result;
            std::memcpy(&result, &value, sizeof(result));
            return result;
        };

        const std::uintptr_t region = regionAddress;
        const std::uint32_t pageOffset = pageAddress - read32(region + 0x10);
        const std::uint32_t pageIndex = static_cast<std::uint32_t>(asSigned(pageOffset) >> 12);
        const std::uintptr_t pageEntry = region + 0x18 + pageIndex * 8u;

        auto* allocationEntry = reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(entryAddress));
        const std::uint32_t releasedUnits = *allocationEntry;

        write32(pageEntry, read32(pageEntry) + releasedUnits);
        *allocationEntry = 0;

        const bool pageIsEmpty = read32(pageEntry) == 0xF0u;
        write32(pageEntry + 0x04, 0xF1u);

        if (pageIsEmpty)
        {
            const std::uintptr_t countAddress = reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockEmptyPageCountMode2);
            write32(countAddress, read32(countAddress) + 1u);

            if (read32(countAddress) == 0x20u)
            {
                Runtime_DecommitSmallBlockPagesMode2(0x10u);
            }
        }
    }

    // Original function: FUN_004ee299
    void* Runtime_AllocateSmallBlockMode2(std::uint32_t unitCount)
    {
        const auto read32 = [] (std::uintptr_t address) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(value));
            return value;
        };

        const auto write32 = [] (std::uintptr_t address, std::uint32_t value)
        {
            std::memcpy(reinterpret_cast<void*>(address), &value, sizeof(value));
        };

        const auto write8 = [] (std::uintptr_t address, std::uint8_t value)
        {
            std::memcpy(reinterpret_cast<void*>(address), &value, sizeof(value));
        };

        const auto asSigned = [] (std::uint32_t value) -> std::int32_t
        {
            std::int32_t result;
            std::memcpy(&result, &value, sizeof(result));
            return result;
        };

        const auto cursorAddress = reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockSearchRegionMode2);
        const auto firstRegion = reinterpret_cast<std::uintptr_t>(&gRuntimeSmallBlockInitialRegionMode2);

        std::uintptr_t region = read32(cursorAddress);

        // Search committed pages, starting at the cached page of each region.
        do
        {
            const std::uint32_t base = read32(region + 0x10);

            if (base != 0xFFFFFFFFu)
            {
                std::uintptr_t entry = read32(region + 0x08);
                const std::uintptr_t entriesEnd = region + 0x2018;

                const std::uint32_t entryOffset = static_cast<std::uint32_t>(entry - region - 0x18);
                const std::uint32_t pageIndex = static_cast<std::uint32_t>(asSigned(entryOffset) >> 3);
                std::uint32_t page = base + (pageIndex << 12);

                for (; entry < entriesEnd; entry += 8, page += 0x1000u)
                {
                    const std::uint32_t freeUnits = read32(entry);

                    if (asSigned(freeUnits) >= asSigned(unitCount) && read32(entry + 4) > unitCount)
                    {
                        const std::uintptr_t allocation = reinterpret_cast<std::uintptr_t>(Runtime_AllocateFromSmallBlockPageMode2(reinterpret_cast<void*>(static_cast<std::uintptr_t>(page)), freeUnits, unitCount));

                        if (allocation != 0)
                        {
                            write32(cursorAddress, static_cast<std::uint32_t>(region));
                            write32(entry, read32(entry) - unitCount);
                            write32(region + 0x08, static_cast<std::uint32_t>(entry));
                            return reinterpret_cast<void*>(allocation);
                        }

                        write32(entry + 4, unitCount);
                    }
                }

                // Reload the cached entry and base after the helper calls.
                const std::uintptr_t cachedEntry = read32(region + 0x08);
                entry = region + 0x18;
                page = read32(region + 0x10);

                for (; entry < cachedEntry; entry += 8, page += 0x1000u)
                {
                    const std::uint32_t freeUnits = read32(entry);

                    if (asSigned(freeUnits) >= asSigned(unitCount) && read32(entry + 4) > unitCount)
                    {
                        const std::uintptr_t allocation = reinterpret_cast<std::uintptr_t>(Runtime_AllocateFromSmallBlockPageMode2(reinterpret_cast<void*>(static_cast<std::uintptr_t>(page)), freeUnits, unitCount));

                        if (allocation != 0)
                        {
                            write32(cursorAddress, static_cast<std::uint32_t>(region));
                            write32(entry, read32(entry) - unitCount);
                            write32(region + 0x08, static_cast<std::uint32_t>(entry));
                            return reinterpret_cast<void*>(allocation);
                        }

                        write32(entry + 4, unitCount);
                    }
                }
            }

            region = read32(region);
        } while (region != read32(cursorAddress));

        // Find a region with pages that have not been committed yet.
        region = firstRegion;

        for (;;)
        {
            if (read32(region + 0x10) != 0xFFFFFFFFu && read32(region + 0x0C) != 0)
            {
                break;
            }

            region = read32(region);

            if (region == firstRegion)
            {
                const std::uintptr_t newRegion = reinterpret_cast<std::uintptr_t>(Runtime_CreateSmallBlockRegionMode2());

                if (newRegion == 0)
                {
                    return nullptr;
                }

                const std::uintptr_t page = read32(newRegion + 0x10);

                write8(page + 0x08, static_cast<std::uint8_t>(unitCount));
                write32(cursorAddress, static_cast<std::uint32_t>(newRegion));
                write32(page, static_cast<std::uint32_t>(page + unitCount + 0x08));
                write32(page + 0x04, 0xF0u - unitCount);

                // This path subtracts only the low byte of the requested count.
                write32(newRegion + 0x18, read32(newRegion + 0x18) - static_cast<std::uint8_t>(unitCount));

                return reinterpret_cast<void*>(page + 0x100);
            }
        }

        const std::uintptr_t firstEntry = read32(region + 0x0C);
        const std::uint32_t entryOffset = static_cast<std::uint32_t>(firstEntry - region - 0x18);
        const std::uint32_t pageIndex = static_cast<std::uint32_t>(asSigned(entryOffset) >> 3);
        const std::uint32_t pageAddress = read32(region + 0x10) + (pageIndex << 12);
        const std::uintptr_t firstPage = pageAddress;

        std::uintptr_t entry = firstEntry;
        std::uint32_t pageCount = 0;

        // The original also reads the next entry after counting the sixteenth page.
        while (read32(entry) == 0xFFFFFFFFu)
        {
            if (pageCount >= 0x10u)
            {
                break;
            }

            entry += 8;
            ++pageCount;
        }

        const std::uint32_t commitBytes = pageCount << 12;

        void* committed = VirtualAlloc(reinterpret_cast<void*>(firstPage), commitBytes, MEM_COMMIT, PAGE_READWRITE);

        if (reinterpret_cast<std::uintptr_t>(committed) != firstPage)
        {
            return nullptr;
        }

        // The original pushes a zero byte count, so this call writes no bytes.
        std::memset(reinterpret_cast<void*>(firstPage), static_cast<int>(commitBytes), 0);

        entry = firstEntry;

        for (std::uint32_t index = 0; index < pageCount; ++index)
        {
            const std::uintptr_t page = firstPage + (index << 12);

            write8(page + 0xF8, 0xFF);
            write32(page, static_cast<std::uint32_t>(page + 0x08));
            write32(page + 0x04, 0xF0);
            write32(entry, 0xF0);
            write32(entry + 0x04, 0xF1);

            entry += 8;
        }

        write32(cursorAddress, static_cast<std::uint32_t>(region));

        const std::uintptr_t entriesEnd = region + 0x2018;

        while (entry < entriesEnd && read32(entry) != 0xFFFFFFFFu)
        {
            entry += 8;
        }

        write32(region + 0x0C, entry < entriesEnd ? static_cast<std::uint32_t>(entry) : 0u);

        // Allocate directly from the first newly committed page.
        write8(firstPage + 0x08, static_cast<std::uint8_t>(unitCount));
        write32(region + 0x08, static_cast<std::uint32_t>(firstEntry));
        write32(firstEntry, read32(firstEntry) - unitCount);
        write32(firstPage + 0x04, read32(firstPage + 0x04) - unitCount);
        write32(firstPage, static_cast<std::uint32_t>(firstPage + unitCount + 0x08));

        return reinterpret_cast<void*>(firstPage + 0x100);
    }

    // Original function: FUN_004ee4a1
    void* Runtime_AllocateFromSmallBlockPageMode2(void* pageMemory, std::uint32_t freeUnits, std::uint32_t unitCount)
    {
        const auto read32 = [] (std::uintptr_t address) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(value));
            return value;
        };

        const auto write32 = [] (std::uintptr_t address, std::uint32_t value)
        {
            std::memcpy(reinterpret_cast<void*>(address), &value, sizeof(value));
        };

        const auto read8 = [] (std::uintptr_t address) -> std::uint8_t
        {
            return *reinterpret_cast<const std::uint8_t*>(address);
        };

        const auto write8 = [] (std::uintptr_t address, std::uint8_t value)
        {
            *reinterpret_cast<std::uint8_t*>(address) = value;
        };

        const std::uintptr_t page = reinterpret_cast<std::uintptr_t>(pageMemory);
        const std::uintptr_t mapBegin = page + 0x08;
        const std::uintptr_t mapEnd = page + 0xF8;
        const std::uint32_t cachedFreeUnits = read32(page + 0x04);
        const std::uintptr_t cachedPosition = read32(page);

        const auto allocationAddress = [page] (std::uintptr_t entry) -> void*
        {
            const std::uint32_t address = (static_cast<std::uint32_t>(entry + 0x08) << 4) - static_cast<std::uint32_t>(page) * 15u;
            return reinterpret_cast<void*>(static_cast<std::uintptr_t>(address));
        };

        const auto updateCursor = [&] (std::uintptr_t entry, std::uint32_t runLength)
        {
            const std::uintptr_t next = entry + unitCount;

            if (next < mapEnd)
            {
                write32(page, static_cast<std::uint32_t>(next));
                write32(page + 0x04, runLength - unitCount);
            }
            else
            {
                write32(page + 0x04, 0);
                write32(page, static_cast<std::uint32_t>(mapBegin));
            }
        };

        if (cachedFreeUnits >= unitCount)
        {
            // The fast path writes the allocation marker before updating the cursor.
            write8(cachedPosition, static_cast<std::uint8_t>(unitCount));
            updateCursor(cachedPosition, cachedFreeUnits);
            return allocationAddress(cachedPosition);
        }

        std::uintptr_t position = cachedPosition;
        const std::uintptr_t cachedRunEnd = cachedPosition + cachedFreeUnits;

        if (read8(cachedRunEnd) != 0)
        {
            position = cachedRunEnd;
        }

        // Search from the cached position toward the end of the map.
        while (position + unitCount < mapEnd)
        {
            const std::uint8_t marker = read8(position);

            if (marker != 0)
            {
                position += marker;
                continue;
            }

            std::uintptr_t runEnd = position + 1;
            std::uint32_t runLength = 1;

            while (read8(runEnd) == 0)
            {
                ++runEnd;
                ++runLength;
            }

            if (runLength >= unitCount)
            {
                updateCursor(position, runLength);
                write8(position, static_cast<std::uint8_t>(unitCount));
                return allocationAddress(position);
            }

            if (position == cachedPosition)
            {
                write32(page + 0x04, runLength);
            }
            else
            {
                freeUnits -= runLength;

                if (freeUnits < unitCount)
                {
                    return nullptr;
                }
            }

            position = runEnd;
        }

        // Wrap around and search up to the original cached position.
        position = mapBegin;

        while (position < cachedPosition)
        {
            if (position + unitCount >= mapEnd)
            {
                return nullptr;
            }

            const std::uint8_t marker = read8(position);

            if (marker != 0)
            {
                position += marker;
                continue;
            }

            std::uintptr_t runEnd = position + 1;
            std::uint32_t runLength = 1;

            while (read8(runEnd) == 0)
            {
                ++runEnd;
                ++runLength;
            }

            if (runLength >= unitCount)
            {
                updateCursor(position, runLength);
                write8(position, static_cast<std::uint8_t>(unitCount));
                return allocationAddress(position);
            }

            freeUnits -= runLength;

            if (freeUnits < unitCount)
            {
                return nullptr;
            }

            position = runEnd;
        }

        return nullptr;
    }

    // Original function: FUN_004ee5c5
    int Runtime_ResizeSmallBlockMode2(std::uint32_t regionAddress, std::uint32_t pageAddress, std::uint32_t entryAddress, std::uint32_t requestedUnits)
    {
        auto* region = reinterpret_cast<RuntimeSmallBlockRegionMode2*>(regionAddress);
        auto* entry = reinterpret_cast<std::uint8_t*>(entryAddress);

        const std::uint32_t reservationAddress =
            static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(region->reservation));
        const std::uint32_t pageIndex = (pageAddress - reservationAddress) >> 12;

        auto& pageEntry = region->pages[pageIndex];
        const std::uint32_t oldUnits = *entry;

        if (oldUnits > requestedUnits)
        {
            *entry = static_cast<std::uint8_t>(requestedUnits);
            pageEntry.freeUnits += oldUnits - requestedUnits;
            pageEntry.failedRequestLimit = 0xF1u;
            return 1;
        }

        // Equal sizes return zero in this allocator.
        if (oldUnits == requestedUnits)
            return 0;

        const std::uint32_t endAddress = entryAddress + requestedUnits;
        const std::uint32_t mapEnd = pageAddress + 0xF8u;

        if (endAddress > mapEnd)
            return 0;

        std::uint32_t scanAddress = entryAddress + oldUnits;

        while (scanAddress < endAddress && *reinterpret_cast<std::uint8_t*>(scanAddress) == 0)
            ++scanAddress;

        if (scanAddress != endAddress)
            return 0;

        *entry = static_cast<std::uint8_t>(requestedUnits);

        auto* pageHeader = reinterpret_cast<std::uint32_t*>(pageAddress);
        const std::uint32_t searchAddress = pageHeader[0];

        if (entryAddress <= searchAddress && endAddress > searchAddress)
        {
            if (endAddress < mapEnd)
            {
                pageHeader[0] = endAddress;

                std::uint32_t remainingUnits = 0;

                while (*reinterpret_cast<std::uint8_t*>(endAddress + remainingUnits) == 0)
                    ++remainingUnits;

                pageHeader[1] = remainingUnits;
            }
            else
            {
                pageHeader[0] = pageAddress + 8u;
                pageHeader[1] = 0;
            }
        }

        pageEntry.freeUnits += oldUnits - requestedUnits;
        return 1;
    }

    // Original function: FUN_004ee66e
    void Runtime_InitializeStreams()
    {
        if (gRuntimeStreamCapacity == 0)
            gRuntimeStreamCapacity = 512;
        else if (gRuntimeStreamCapacity < 20)
            gRuntimeStreamCapacity = 20;

        gRuntimeStreamTable = static_cast<RuntimeFormatStream**>(Runtime_AllocateZeroed(static_cast<std::uint32_t>(gRuntimeStreamCapacity), sizeof(RuntimeFormatStream*)));

        if (gRuntimeStreamTable == nullptr)
        {
            gRuntimeStreamCapacity = 20;
            gRuntimeStreamTable = static_cast<RuntimeFormatStream**>(Runtime_AllocateZeroed(20u, sizeof(RuntimeFormatStream*)));

            if (gRuntimeStreamTable == nullptr)
                Runtime_FatalExit(0x1A);
        }

        for (std::int32_t index = 0; index < 20; ++index)
            gRuntimeStreamTable[index] = &gRuntimeStaticStreams[index];

        auto* descriptors = reinterpret_cast<RuntimeDescriptorRecord*>(gRuntimeDescriptorBlocks[0]);

        for (std::uint32_t index = 0; index < 3u; ++index)
        {
            const std::uint32_t handle = descriptors[index].osHandle;

            if (handle == 0xFFFFFFFFu || handle == 0)
                gRuntimeStaticStreams[index].descriptor = 0xFFFFFFFFu;
        }
    }

    // Original function: FUN_004ee716
    void Runtime_PreTerminateStreams()
    {
        // Original forwarding wrapper FUN_004e8d80 calls the flush helper with 1.
        Runtime_FlushStreams(1);

        if (gRuntimeCleanupReturnsToCaller != 0)
            Runtime_CloseAllStreams();
    }

    // Original function: FUN_004ee72a
    void Runtime_LockStream(RuntimeFormatStream* stream)
    {
        const std::uintptr_t address = reinterpret_cast<std::uintptr_t>(stream);
        const std::uintptr_t first = reinterpret_cast<std::uintptr_t>(&gRuntimeStaticStreams[0]);
        const std::uintptr_t last = reinterpret_cast<std::uintptr_t>(&gRuntimeStaticStreams[19]);

        if (address >= first && address <= last)
        {
            const std::int32_t index = static_cast<std::int32_t>((address - first) >> 5);
            Runtime_Lock(index + 0x1C);
        }
        else
        {
            auto* lock = reinterpret_cast<CRITICAL_SECTION*>(reinterpret_cast<std::uint8_t*>(stream) + 0x20);
            ::EnterCriticalSection(lock);
        }
    }

    // Original function: FUN_004ee759
    void Runtime_LockStreamIndex(std::int32_t index, RuntimeFormatStream* stream)
    {
        if (index < 20)
        {
            Runtime_Lock(index + 0x1C);
        }
        else
        {
            auto* lock = reinterpret_cast<CRITICAL_SECTION*>(reinterpret_cast<std::uint8_t*>(stream) + 0x20);
            ::EnterCriticalSection(lock);
        }
    }

    // Original function: FUN_004ee77c
    void Runtime_UnlockStream(RuntimeFormatStream* stream)
    {
        const std::uintptr_t address = reinterpret_cast<std::uintptr_t>(stream);
        const std::uintptr_t first = reinterpret_cast<std::uintptr_t>(&gRuntimeStaticStreams[0]);
        const std::uintptr_t last = reinterpret_cast<std::uintptr_t>(&gRuntimeStaticStreams[19]);

        if (address >= first && address <= last)
        {
            const std::int32_t index = static_cast<std::int32_t>((address - first) >> 5);
            Runtime_Unlock(index + 0x1C);
        }
        else
        {
            auto* lock = reinterpret_cast<CRITICAL_SECTION*>(reinterpret_cast<std::uint8_t*>(stream) + 0x20);
            ::LeaveCriticalSection(lock);
        }
    }

    // Original function: FUN_004ee7ab
    void Runtime_UnlockStreamIndex(std::int32_t index, RuntimeFormatStream* stream)
    {
        if (index < 20)
        {
            Runtime_Unlock(index + 0x1C);
        }
        else
        {
            auto* lock = reinterpret_cast<CRITICAL_SECTION*>(reinterpret_cast<std::uint8_t*>(stream) + 0x20);
            ::LeaveCriticalSection(lock);
        }
    }

    // Original function: FUN_004ee7ce
    void Runtime_FreeStreamBuffer(RuntimeFormatStream* stream)
    {
        if ((stream->flags & 0x83u) != 0 && (stream->flags & 0x08u) != 0)
        {
            Runtime_Free(stream->buffer);
            stream->flags &= ~0x408u;
            stream->position = nullptr;
            stream->buffer = nullptr;
            stream->remainingBytes = 0;
        }
    }

    // Original function: FUN_004ee7f9
    std::int32_t Runtime_CommitDescriptor(std::uint32_t descriptor)
    {
        if (descriptor >= gRuntimeDescriptorCapacity)
        {
            *Runtime_GetErrnoPointer() = 9;
            return -1;
        }

        auto* block = reinterpret_cast<RuntimeDescriptorRecord*>(gRuntimeDescriptorBlocks[descriptor >> 5]);
        const std::uint32_t index = descriptor & 31u;

        if ((block[index].flags & 0x01u) == 0)
        {
            *Runtime_GetErrnoPointer() = 9;
            return -1;
        }

        Runtime_LockDescriptor(descriptor);

        block = reinterpret_cast<RuntimeDescriptorRecord*>(gRuntimeDescriptorBlocks[descriptor >> 5]);
        std::int32_t result = 0;

        if ((block[index].flags & 0x01u) == 0)
        {
            *Runtime_GetErrnoPointer() = 9;
            result = -1;
        }
        else
        {
            HANDLE handle = reinterpret_cast<HANDLE>(static_cast<std::uintptr_t>(Runtime_GetOsHandle(descriptor)));
            DWORD error = 0;

            if (::FlushFileBuffers(handle) == FALSE)
                error = ::GetLastError();

            // The original treats a zero last-error value as success.
            if (error != 0)
            {
                *Runtime_GetDosErrnoPointer() = error;
                *Runtime_GetErrnoPointer() = 9;
                result = -1;
            }
        }

        Runtime_UnlockDescriptor(descriptor);
        return result;
    }

    // Original function: FUN_004ee88c
    RuntimeFormatStream* Runtime_OpenStreamNoLock(const char* fileName, const char* mode, std::uint32_t shareMode, RuntimeFormatStream* stream)
    {
        std::uint32_t streamFlags = gRuntimeDefaultStreamFlags;
        std::uint32_t openFlags;

        switch (*mode)
        {
            case 'r':
                openFlags = 0;
                streamFlags |= 0x01u;
                break;

            case 'w':
                openFlags = 0x301u;
                streamFlags |= 0x02u;
                break;

            case 'a':
                openFlags = 0x109u;
                streamFlags |= 0x02u;
                break;

            default:
                return nullptr;
        }

        bool commitModeSeen = false;
        bool accessHintSeen = false;
        bool parsing = true;

        while (parsing)
        {
            const char character = *++mode;

            if (character == '\0')
                break;

            switch (character)
            {
                case '+':
                    if ((openFlags & 0x02u) != 0)
                    {
                        parsing = false;
                        break;
                    }

                    openFlags = (openFlags & ~0x01u) | 0x02u;
                    streamFlags = (streamFlags & ~0x03u) | 0x80u;
                    break;

                case 'b':
                case 't':
                    if ((openFlags & 0xC000u) != 0)
                    {
                        parsing = false;
                        break;
                    }

                    openFlags |= character == 'b' ? 0x8000u : 0x4000u;
                    break;

                case 'c':
                case 'n':
                    if (commitModeSeen)
                    {
                        parsing = false;
                        break;
                    }

                    commitModeSeen = true;

                    if (character == 'c')
                        streamFlags |= 0x4000u;
                    else
                        streamFlags &= ~0x4000u;

                    break;

                case 'S':
                case 'R':
                    if (accessHintSeen)
                    {
                        parsing = false;
                        break;
                    }

                    accessHintSeen = true;
                    openFlags |= character == 'S' ? 0x20u : 0x10u;
                    break;

                case 'D':
                    if ((openFlags & 0x40u) != 0)
                    {
                        parsing = false;
                        break;
                    }

                    openFlags |= 0x40u;
                    break;

                case 'T':
                    if ((openFlags & 0x1000u) != 0)
                    {
                        parsing = false;
                        break;
                    }

                    openFlags |= 0x1000u;
                    break;

                default:
                    parsing = false;
                    break;
            }
        }

        const std::int32_t descriptor = Runtime_OpenShared(fileName, openFlags, shareMode, 0x1A4u);

        if (descriptor < 0)
            return nullptr;

        ++gRuntimeStreamBufferRequestCount;

        stream->flags = streamFlags;
        stream->remainingBytes = 0;
        stream->position = nullptr;
        stream->buffer = nullptr;
        stream->temporaryFileName = nullptr;
        stream->descriptor = static_cast<std::uint32_t>(descriptor);

        return stream;
    }

    // Original function: FUN_004ee9fc
    RuntimeFormatStream* Runtime_AcquireUnusedStream()
    {
        RuntimeFormatStream* result = nullptr;

        Runtime_Lock(2);

        for (std::int32_t index = 0; index < gRuntimeStreamCapacity; ++index)
        {
            RuntimeFormatStream* stream = gRuntimeStreamTable[index];

            if (stream == nullptr)
            {
                // A dynamic stream includes its critical section at offset 0x20.
                stream = static_cast<RuntimeFormatStream*>(Runtime_Allocate(0x38u));
                gRuntimeStreamTable[index] = stream;

                // The original stops at the first allocation failure.
                if (stream == nullptr)
                    break;

                auto* lock = reinterpret_cast<CRITICAL_SECTION*>(reinterpret_cast<std::uint8_t*>(stream) + 0x20);
                ::InitializeCriticalSection(lock);
                ::EnterCriticalSection(lock);

                result = stream;
                break;
            }

            if ((stream->flags & 0x83u) != 0)
                continue;

            Runtime_LockStreamIndex(index, stream);

            // Recheck after obtaining the stream lock.
            stream = gRuntimeStreamTable[index];

            if ((stream->flags & 0x83u) == 0)
            {
                result = stream;
                break;
            }

            Runtime_UnlockStreamIndex(index, stream);
        }

        if (result != nullptr)
        {
            result->descriptor = 0xFFFFFFFFu;
            result->remainingBytes = 0;
            result->flags = 0;
            result->buffer = nullptr;
            result->position = nullptr;
            result->temporaryFileName = nullptr;

            // bufferSize and unknown14 remain untouched.
        }

        Runtime_Unlock(2);

        // A successful result remains locked for the caller.
        return result;
    }

    // Original function: FUN_004eeac4.
    std::int32_t Runtime_CompareEnvironmentNames(const char* first, const char* second, std::int32_t length)
    {
        if (length == 0)
        {
            return 0;
        }

        const int result = Runtime_CompareLocaleStrings(gRuntimeMultibyteLocale, 1u, first, length, second, length, gRuntimeMultibyteCodePage);

        if (result == 0)
        {
            return 0x7FFFFFFF;
        }

        return result - 2;
    }

    // Original function: FUN_004eeb03.
    std::int32_t Runtime_ConvertWideEnvironment()
    {
        wchar_t** source = gRuntimeWideEnvironment;

        while (*source != nullptr)
        {
            const int requiredBytes = WideCharToMultiByte(1, 0, *source, -1, nullptr, 0, nullptr, nullptr);

            if (requiredBytes == 0)
            {
                return -1;
            }

            char* entry = static_cast<char*>(Runtime_Allocate(static_cast<std::uint32_t>(requiredBytes)));

            if (entry == nullptr)
            {
                return -1;
            }

            if (WideCharToMultiByte(1, 0, *source, -1, entry, requiredBytes, nullptr, nullptr) == 0)
            {
                // Preserve the original allocation leak on conversion failure.
                return -1;
            }

            // Preserve the original ignored insertion result.
            Runtime_SetEnvironmentEntry(entry, 0);
            ++source;
        }

        return 0;
    }

    // Original function: FUN_004eeb71
    std::int32_t Runtime_BeginTemporaryStreamBuffer(RuntimeFormatStream* stream)
    {
        if (Runtime_IsCharacterDevice(stream->descriptor) == 0)
            return 0;

        std::uint32_t index;

        if (stream == &gRuntimeStdoutStream)
            index = 0;
        else if (stream == &gRuntimeStderrStream)
            index = 1;
        else
            return 0;

        // The original increments this counter before testing existing buffers.
        ++gRuntimeStreamBufferRequestCount;

        if ((stream->flags & 0x10Cu) != 0)
            return 0;

        char*& cachedBuffer = gRuntimeTemporaryStreamBuffers[index];

        if (cachedBuffer == nullptr)
            cachedBuffer = static_cast<char*>(Runtime_Allocate(0x1000u));

        if (cachedBuffer == nullptr)
        {
            stream->buffer = reinterpret_cast<char*>(stream->unknown14);
            stream->position = stream->buffer;
            stream->bufferSize = 2;
            stream->remainingBytes = 2;
        }
        else
        {
            stream->buffer = cachedBuffer;
            stream->position = cachedBuffer;
            stream->bufferSize = 0x1000u;
            stream->remainingBytes = 0x1000u;
        }

        stream->flags |= 0x1102u;
        return 1;
    }

    // Original function: FUN_004eebfe
    void Runtime_EndTemporaryStreamBuffer(std::int32_t temporaryBufferInstalled, RuntimeFormatStream* stream)
    {
        if (temporaryBufferInstalled == 0)
            return;

        if ((stream->flags & 0x1000u) == 0)
            return;

        // The original ignores the flush result and retains the cached allocation.
        Runtime_FlushStreamBuffer(stream);

        stream->flags &= ~0x1100u;
        stream->bufferSize = 0;
        stream->position = nullptr;
        stream->buffer = nullptr;
    }

    // Original function: FUN_004eed15
    std::int32_t Runtime_FillStreamBufferAndReadCharacter(RuntimeFormatStream* stream)
    {
        if ((stream->flags & 0x83u) == 0 || (stream->flags & 0x40u) != 0)
            return -1;

        if ((stream->flags & 2u) != 0)
        {
            stream->flags |= 0x20u;
            return -1;
        }

        stream->flags |= 1u;

        if ((stream->flags & 0x10Cu) == 0)
            Runtime_AllocateStreamBuffer(stream);
        else
            stream->position = stream->buffer;

        const std::int32_t readResult = Runtime_Read(stream->descriptor, stream->buffer, stream->bufferSize);
        stream->remainingBytes = static_cast<std::uint32_t>(readResult);

        if (readResult == 0 || readResult == -1)
        {
            stream->flags |= readResult == 0 ? 0x10u : 0x20u;
            stream->remainingBytes = 0;
            return -1;
        }

        if ((stream->flags & 0x82u) == 0)
        {
            const RuntimeDescriptorRecord* record;

            if (stream->descriptor == 0xFFFFFFFFu)
            {
                record = &gRuntimeInvalidDescriptorRecord;
            }
            else
            {
                const auto* block = reinterpret_cast<const RuntimeDescriptorRecord*>(gRuntimeDescriptorBlocks[stream->descriptor >> 5]);
                record = &block[stream->descriptor & 0x1Fu];
            }

            if ((record->flags & 0x82u) == 0x82u)
                stream->flags |= 0x2000u;
        }

        if (stream->bufferSize == 0x200u && (stream->flags & 8u) != 0 && (stream->flags & 0x400u) == 0)
            stream->bufferSize = 0x1000u;

        --stream->remainingBytes;

        const std::int32_t result = static_cast<unsigned char>(*stream->position);
        ++stream->position;
        return result;
    }

    // Original function: FUN_004eedf1.
    char* Runtime_DuplicateString(const char* source)
    {
        if (source == nullptr)
        {
            return nullptr;
        }

        const std::uint32_t requiredBytes = static_cast<std::uint32_t>(std::strlen(source)) + 1u;
        char* result = static_cast<char*>(Runtime_Allocate(requiredBytes));

        if (result != nullptr)
        {
            std::strcpy(result, source);
        }

        return result;
    }

    // Original function: FUN_004eee22
    std::int32_t Runtime_StringToLong(const char* text, char** endPointer, std::int32_t base)
    {
        const std::uint32_t resultBits = Runtime_ParseInteger32(text, endPointer, base, 0u);
        std::int32_t result;
        std::memcpy(&result, &resultBits, sizeof(result));
        return result;
    }

    // Original function: FUN_004eee39
    std::uint32_t Runtime_ParseInteger32(const char* text, char** endPointer, std::int32_t base, std::uint32_t flags)
    {
        const char* cursor = text;
        std::uint32_t value = 0;

        const auto classify = [] (std::uint8_t character, std::uint32_t mask) -> bool
        {
            if (gRuntimeMaxMultibyteLength > 1)
            {
                return Runtime_ClassifyCharacter(character, mask) != 0;
            }

            const auto* types = reinterpret_cast<const std::uint8_t*>(gRuntimeCharacterTypes);
            std::uint16_t type;
            std::memcpy(&type, types + static_cast<std::size_t>(character) * 2u, sizeof(type));
            return (type & mask) != 0;
        };

        while (classify(static_cast<std::uint8_t>(*cursor), 0x08u))
        {
            ++cursor;
        }

        if (*cursor == '-')
        {
            flags |= 0x02u;
            ++cursor;
        }
        else if (*cursor == '+')
        {
            ++cursor;
        }

        if (base < 0 || base == 1 || base > 36)
        {
            if (endPointer != nullptr)
            {
                *endPointer = const_cast<char*>(text);
            }

            // The original invalid-base path does not set errno.
            return 0;
        }

        if (base == 0)
        {
            if (*cursor != '0')
            {
                base = 10;
            }
            else if (cursor[1] == 'x' || cursor[1] == 'X')
            {
                base = 16;
            }
            else
            {
                base = 8;
            }
        }

        if (base == 16 && cursor[0] == '0' && (cursor[1] == 'x' || cursor[1] == 'X'))
        {
            cursor += 2;
        }

        const auto radix = static_cast<std::uint32_t>(base);
        const std::uint32_t cutoff = 0xFFFFFFFFu / radix;
        const std::uint32_t remainder = 0xFFFFFFFFu % radix;

        for (;;)
        {
            const auto character = static_cast<std::uint8_t>(*cursor);
            const std::int32_t signedCharacter = character < 0x80u ? character : static_cast<std::int32_t>(character) - 0x100;
            std::uint32_t digit;

            if (classify(character, 0x04u))
            {
                digit = static_cast<std::uint32_t>(signedCharacter) - 0x30u;
            }
            else
            {
                if (!classify(character, 0x103u))
                {
                    break;
                }

                digit = static_cast<std::uint32_t>(Runtime_ToUpper(signedCharacter)) - 0x37u;
            }

            if (digit >= radix)
            {
                break;
            }

            flags |= 0x08u;

            if (value > cutoff || (value == cutoff && digit > remainder))
            {
                flags |= 0x04u;
            }
            else
            {
                value = value * radix + digit;
            }

            // Continue consuming valid digits after overflow.
            ++cursor;
        }

        if ((flags & 0x08u) == 0)
        {
            cursor = text;
            value = 0;
        }
        else
        {
            const bool unsignedConversion = (flags & 0x01u) != 0;
            const bool negative = (flags & 0x02u) != 0;
            const std::uint32_t signedLimit = negative ? 0x80000000u : 0x7FFFFFFFu;

            if ((flags & 0x04u) != 0 || (!unsignedConversion && value > signedLimit))
            {
                *Runtime_GetErrnoPointer() = 0x22u;
                value = unsignedConversion ? 0xFFFFFFFFu : signedLimit;
            }
        }

        if (endPointer != nullptr)
        {
            *endPointer = const_cast<char*>(cursor);
        }

        if ((flags & 0x02u) != 0)
        {
            value = 0u - value;
        }

        return value;
    }

    // Original function: FUN_004ef055
    const char* Runtime_FindLastMultibyteCharacter(const char* text, std::uint32_t character)
    {
        const char* result = nullptr;

        if (gRuntimeIsMultibyteCodePage == 0)
        {
            const auto target = static_cast<std::uint8_t>(character);

            for (const char* current = text; ; ++current)
            {
                const auto value = static_cast<std::uint8_t>(*current);

                if (value == target)
                {
                    result = current;
                }

                if (value == 0)
                {
                    return result;
                }
            }
        }

        Runtime_Lock(0x19);

        const char* current = text;

        for (;;)
        {
            const auto firstByte = static_cast<std::uint8_t>(*current);
            std::uint8_t lastByte = firstByte;
            std::uint32_t value = firstByte;

            if ((gRuntimeMultibyteTypes[static_cast<std::uint32_t>(firstByte) + 1u] & 0x04u) != 0)
            {
                ++current;
                lastByte = static_cast<std::uint8_t>(*current);

                if (lastByte != 0)
                {
                    value = (value << 8) | lastByte;

                    if (value == character)
                    {
                        result = current - 1;
                    }
                }
                else if (result == nullptr)
                {
                    // Preserve the original handling of an incomplete final character.
                    result = current;
                }
            }
            else if (value == character)
            {
                result = current;
            }

            ++current;

            if (lastByte == 0)
            {
                break;
            }
        }

        Runtime_Unlock(0x19);
        return result;
    }

    // Original function: FUN_004ef0c7
    std::int32_t Runtime_RemoveTemporaryStreams()
    {
        std::int32_t count = 0;

        Runtime_Lock(2);

        for (std::int32_t index = 0; index < gRuntimeStreamCapacity; ++index)
        {
            RuntimeFormatStream* stream = gRuntimeStreamTable[index];

            if (stream == nullptr)
                continue;

            Runtime_LockStreamIndex(index, stream);

            stream = gRuntimeStreamTable[index];

            if ((stream->flags & 0x83u) != 0 && stream->temporaryFileName != nullptr)
            {
                Runtime_CloseStreamNoLock(stream);
                ++count;
            }

            Runtime_UnlockStreamIndex(index, gRuntimeStreamTable[index]);
        }

        Runtime_Unlock(2);
        return count;
    }

    // Original function: FUN_004ef134
    void Runtime_AllocateStreamBuffer(RuntimeFormatStream* stream)
    {
        // Count allocation attempts, including unsuccessful ones.
        std::uint32_t allocationCount;
        std::memcpy(&allocationCount, &gRuntimeStreamBufferRequestCount, sizeof(allocationCount));
        ++allocationCount;
        std::memcpy(&gRuntimeStreamBufferRequestCount, &allocationCount, sizeof(allocationCount));

        stream->buffer = static_cast<char*>(Runtime_Allocate(0x1000));

        if (stream->buffer != nullptr)
        {
            stream->flags |= 0x08u;
            stream->bufferSize = 0x1000u;
        }
        else
        {
            stream->flags |= 0x04u;
            stream->buffer = reinterpret_cast<char*>(stream->unknown14);
            stream->bufferSize = 2;
        }

        stream->remainingBytes = 0;
        stream->position = stream->buffer;
    }

    // Original function: FUN_004ef178
    std::uint32_t Runtime_IsCharacterDevice(std::uint32_t descriptor)
    {
        if (descriptor >= static_cast<std::uint32_t>(gRuntimeDescriptorCapacity))
            return 0;

        std::int32_t signedDescriptor;
        std::memcpy(&signedDescriptor, &descriptor, sizeof(signedDescriptor));

        const std::int32_t blockIndex = signedDescriptor >> 5;
        const auto* descriptorBlocks = reinterpret_cast<const std::uint8_t*>(&gRuntimeDescriptorBlocks);

        const std::uint8_t* block;
        std::memcpy(&block, descriptorBlocks + static_cast<std::ptrdiff_t>(blockIndex) * 4, sizeof(block));

        const std::uint32_t entryOffset = (descriptor & 0x1Fu) * 0x24u;
        return block[entryOffset + 0x04] & 0x40u;
    }

    // Original function: FUN_004ef1a1
    std::int32_t Runtime_WideCharacterToMultibyte(char* destination, std::uint32_t characterBits)
    {
        auto* localeUsers = reinterpret_cast<volatile LONG*>(&gRuntimeLocaleReaderCount);

        InterlockedIncrement(localeUsers);

        bool localeLocked = false;

        if (gRuntimeLocaleChangeState != 0)
        {
            InterlockedDecrement(localeUsers);
            Runtime_Lock(0x13);
            localeLocked = true;
        }

        const std::int32_t result = Runtime_WideCharacterToMultibyteNoLock(destination, characterBits);

        if (localeLocked)
        {
            Runtime_Unlock(0x13);
        }
        else
        {
            InterlockedDecrement(localeUsers);
        }

        return result;
    }

    // Original function: FUN_004ef1fa
    std::int32_t Runtime_WideCharacterToMultibyteNoLock(char* destination, std::uint32_t characterBits)
    {
        if (destination == nullptr)
        {
            return 0;
        }

        const WCHAR character = static_cast<WCHAR>(characterBits);

        if (gRuntimeCTypeLocale == 0)
        {
            if (character > 0xFF)
            {
                *Runtime_GetErrnoPointer() = 0x2A;
                return -1;
            }

            const std::uint8_t byteValue = static_cast<std::uint8_t>(character);
            std::memcpy(destination, &byteValue, sizeof(byteValue));

            return 1;
        }

        BOOL usedDefaultCharacter = FALSE;

        const int result = WideCharToMultiByte(gRuntimeCodePage, 0x220, &character, 1, destination, gRuntimeMaxMultibyteLength, nullptr, &usedDefaultCharacter);

        if (result == 0 || usedDefaultCharacter != FALSE)
        {
            *Runtime_GetErrnoPointer() = 0x2A;
            return -1;
        }

        return result;
    }

    // Original function: FUN_004ef263
    BOOL Runtime_GetStringType(DWORD informationType, LPCSTR source, int sourceLength, LPWORD characterTypes, UINT codePage, LCID locale, int rejectInvalidCharacters)
    {
        std::uint32_t backend = static_cast<std::uint32_t>(gRuntimeGetStringTypeBackend);

        if (backend == 0)
        {
            WORD probeType;

            if (::GetStringTypeW(1, L"", 1, &probeType) != FALSE)
            {
                backend = 1;
            }
            else
            {
                if (::GetStringTypeA(0, 1, "", 1, &probeType) == FALSE)
                    return FALSE;

                backend = 2;
            }

            gRuntimeGetStringTypeBackend = backend;
        }

        if (backend == 2)
        {
            if (locale == 0)
                locale = static_cast<LCID>(gRuntimeCTypeLocale);

            return ::GetStringTypeA(locale, informationType, source, sourceLength, characterTypes);
        }

        if (backend != 1)
            return FALSE;

        if (codePage == 0)
            codePage = static_cast<UINT>(gRuntimeCodePage);

        const DWORD queryFlags = rejectInvalidCharacters != 0 ? 0x09u : 0x01u;
        const int requiredCharacters = ::MultiByteToWideChar(codePage, queryFlags, source, sourceLength, nullptr, 0);

        if (requiredCharacters == 0)
            return FALSE;

        WCHAR* wideBuffer = nullptr;

        __try
        {
            // Preserve the original 32-bit size calculation and four-byte alignment.
            const std::uint32_t byteCount = static_cast<std::uint32_t>(requiredCharacters) * 2u;
            const std::uint32_t allocationSize = (byteCount + 3u) & 0xFFFFFFFCu;

            wideBuffer = static_cast<WCHAR*>(_alloca(allocationSize));
            std::memset(wideBuffer, 0, byteCount);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            wideBuffer = nullptr;
        }

        if (wideBuffer == nullptr)
            return FALSE;

        // The conversion call always uses 0x01, even when the size query used 0x09.
        const int convertedCharacters = ::MultiByteToWideChar(codePage, 0x01u, source, sourceLength, wideBuffer, requiredCharacters);

        if (convertedCharacters == 0)
            return FALSE;

        return ::GetStringTypeW(informationType, wideBuffer, convertedCharacters, characterTypes);
    }

    // Original function: FUN_004ef3ac
    void* Runtime_AllocateZeroed(std::uint32_t count, std::uint32_t elementSize)
    {
        // Preserve the original multiplication modulo 2^32.
        const std::uint32_t requestedBytes = count * elementSize;
        std::uint32_t allocationBytes = requestedBytes;

        if (allocationBytes <= 0xFFFFFFE0u)
        {
            if (allocationBytes == 0)
                allocationBytes = 1;

            allocationBytes = (allocationBytes + 15u) & 0xFFFFFFF0u;
        }

        for (;;)
        {
            void* allocation = nullptr;

            if (allocationBytes <= 0xFFFFFFE0u)
            {
                const std::uint32_t heapMode = static_cast<std::uint32_t>(gRuntimeHeapMode);

                if (heapMode == 3)
                {
                    if (requestedBytes <= static_cast<std::uint32_t>(gRuntimeSmallBlockThreshold))
                    {
                        Runtime_Lock(0x09);

                        __try
                        {
                            allocation = Runtime_AllocateSmallBlock(requestedBytes);
                        }
                        __finally
                        {
                            Runtime_Unlock(0x09);
                        }

                        if (allocation != nullptr)
                            std::memset(allocation, 0, requestedBytes);
                    }
                }
                else if (heapMode == 2)
                {
                    if (allocationBytes <= static_cast<std::uint32_t>(gRuntimeSmallBlockThresholdMode2))
                    {
                        Runtime_Lock(0x09);

                        __try
                        {
                            allocation = Runtime_AllocateSmallBlockMode2(allocationBytes >> 4);
                        }
                        __finally
                        {
                            Runtime_Unlock(0x09);
                        }

                        if (allocation != nullptr)
                            std::memset(allocation, 0, allocationBytes);
                    }
                }

                if (allocation != nullptr)
                    return allocation;

                allocation = ::HeapAlloc(gRuntimeHeap, HEAP_ZERO_MEMORY, allocationBytes);
            }

            if (allocation != nullptr)
                return allocation;

            if (gRuntimeNewHandlerMode == 0)
                return nullptr;

            if (Runtime_CallNewHandler(allocationBytes) == 0)
            {
                return nullptr;
            }
        }
    }

    // Original function: FUN_004ef4e9
    std::int32_t Runtime_SetModeNoLock(std::uint32_t descriptor, std::int32_t mode)
    {
        std::int32_t signedDescriptor;
        std::memcpy(&signedDescriptor, &descriptor, sizeof(signedDescriptor));

        const std::int32_t blockIndex = signedDescriptor >> 5;
        const auto* descriptorBlocks = reinterpret_cast<const std::uint8_t*>(&gRuntimeDescriptorBlocks);

        std::uint8_t* block;
        std::memcpy(&block, descriptorBlocks + static_cast<std::ptrdiff_t>(blockIndex) * 4, sizeof(block));

        const std::uint32_t entryOffset = (descriptor & 0x1Fu) * 0x24u;
        std::uint8_t* flags = block + entryOffset + 0x04;
        const std::uint8_t previousFlags = *flags;

        if (mode == 0x8000)
        {
            *flags = static_cast<std::uint8_t>(previousFlags & 0x7Fu);
        }
        else if (mode == 0x4000)
        {
            *flags = static_cast<std::uint8_t>(previousFlags | 0x80u);
        }
        else
        {
            *Runtime_GetErrnoPointer() = 0x16;
            return -1;
        }

        return (previousFlags & 0x80u) != 0 ? 0x4000 : 0x8000;
    }

    // Original function: FUN_004f053b
    std::uint32_t Runtime_ControlX87(std::uint32_t newControl, std::uint32_t mask)
    {
        std::uint16_t controlWord;

        __asm
        {
            fwait
            fnstcw controlWord
        }

        const std::uint32_t previousControl = Runtime_DecodeX87ControlWord(controlWord);
        const std::uint32_t updatedControl = (previousControl & ~mask) | (newControl & mask);

        controlWord = Runtime_EncodeX87ControlWord(updatedControl);

        __asm
        {
            fldcw controlWord
        }

        return updatedControl;
    }

    // Original function: FUN_004f0570
    std::uint32_t Runtime_ControlFloatingPoint(std::uint32_t newControl, std::uint32_t mask)
    {
        return Runtime_ControlX87(newControl, mask & ~0x00080000u);
    }

    // Original function: FUN_004f0586
    std::uint32_t Runtime_DecodeX87ControlWord(std::uint16_t controlWord)
    {
        std::uint32_t control = 0;

        if ((controlWord & 0x0001u) != 0)
            control |= 0x00000010u;
        if ((controlWord & 0x0004u) != 0)
            control |= 0x00000008u;
        if ((controlWord & 0x0008u) != 0)
            control |= 0x00000004u;
        if ((controlWord & 0x0010u) != 0)
            control |= 0x00000002u;
        if ((controlWord & 0x0020u) != 0)
            control |= 0x00000001u;
        if ((controlWord & 0x0002u) != 0)
            control |= 0x00080000u;

        control |= static_cast<std::uint32_t>(controlWord & 0x0C00u) >> 2;

        switch (controlWord & 0x0300u)
        {
            case 0x0000u:
                control |= 0x00020000u;
                break;

            case 0x0200u:
                control |= 0x00010000u;
                break;

            default:
                break;
        }

        if ((controlWord & 0x1000u) != 0)
            control |= 0x00040000u;

        return control;
    }

    // Original function: FUN_004f0618
    std::uint16_t Runtime_EncodeX87ControlWord(std::uint32_t control)
    {
        std::uint32_t controlWord = 0;

        if ((control & 0x00000010u) != 0)
            controlWord |= 0x0001u;
        if ((control & 0x00000008u) != 0)
            controlWord |= 0x0004u;
        if ((control & 0x00000004u) != 0)
            controlWord |= 0x0008u;
        if ((control & 0x00000002u) != 0)
            controlWord |= 0x0010u;
        if ((control & 0x00000001u) != 0)
            controlWord |= 0x0020u;
        if ((control & 0x00080000u) != 0)
            controlWord |= 0x0002u;

        controlWord |= (control & 0x00000300u) << 2;

        switch (control & 0x00030000u)
        {
            case 0x00000000u:
                controlWord |= 0x0300u;
                break;

            case 0x00010000u:
                controlWord |= 0x0200u;
                break;

            default:
                break;
        }

        if ((control & 0x00040000u) != 0)
            controlWord |= 0x1000u;

        return static_cast<std::uint16_t>(controlWord);
    }

    // Original function: FUN_004f06a1.
    // Big-endian word order; tests bits strictly AFTER bitIndex (MSB is bit 0).
    bool Runtime_MantissaTailIsZero(const std::uint32_t* value, int bitIndex)
    {
        const int word = bitIndex / 32;
        if ((value[word] & ~(0xffffffffu << (31 - bitIndex % 32))) != 0)
            return false;
        for (int i = word + 1; i < 3; ++i)
            if (value[i] != 0)
                return false;
        return true;
    }

    // Original function: FUN_004f06ea.
    std::uint32_t Runtime_IncrementMantissa(std::uint32_t* value, int bitIndex)
    {
        int word = bitIndex / 32;
        std::uint64_t sum = std::uint64_t(value[word]) + (1u << (31 - bitIndex % 32));
        value[word] = static_cast<std::uint32_t>(sum);
        std::uint32_t carry = static_cast<std::uint32_t>(sum >> 32);
        while (--word >= 0 && carry != 0)
        {
            sum = std::uint64_t(value[word]) + carry;
            value[word] = static_cast<std::uint32_t>(sum);
            carry = static_cast<std::uint32_t>(sum >> 32);
        }
        return carry;
    }

    // Original function: FUN_004f0740.
    // Preserve the original rule: the tail test skips bit precision + 1.
    // This is deliberately not replaced with host floating-point rounding.
    std::uint32_t Runtime_RoundMantissa(std::uint32_t* value, int precision)
    {
        const int word = precision / 32;
        const int shift = 31 - precision % 32;
        std::uint32_t carry = 0;
        if ((value[word] & (1u << shift)) != 0 && !Runtime_MantissaTailIsZero(value, precision + 1))
            carry = Runtime_IncrementMantissa(value, precision - 1);
        value[word] &= 0xffffffffu << shift;
        for (int i = word + 1; i < 3; ++i)
            value[i] = 0;
        return carry;
    }

    // Original function: FUN_004f080e.
    // Big-endian word order; distinct from Runtime_ShiftRight96's little-endian order.
    void Runtime_ShiftMantissaRight(std::uint32_t* value, int count)
    {
        const std::uint32_t original[3] = { value[0], value[1], value[2] };
        const int words = count / 32;
        const int bits = count % 32;
        for (int i = 0; i < 3; ++i)
        {
            const int source = i - words;
            value[i] = source >= 0 ? original[source] >> bits : 0;
            if (bits != 0 && source > 0)
                value[i] |= original[source - 1] << (32 - bits);
        }
    }

    // Original function: FUN_004f089b.
    // FUN_004f07cc, FUN_004f07e7 and FUN_004f07f3 are folded into local copy/zero checks.
    int Runtime_Extended12ToBinary(const std::uint32_t* source, void* destination, const RuntimeBinaryFormat& format)
    {
        const auto* bytes = reinterpret_cast<const unsigned char*>(source);
        std::uint16_t signExponent, low;
        std::uint32_t mantissa[3];
        std::memcpy(&signExponent, bytes + 10, 2);
        std::memcpy(&mantissa[0], bytes + 6, 4);
        std::memcpy(&mantissa[1], bytes + 2, 4);
        std::memcpy(&low, bytes, 2);
        mantissa[2] = std::uint32_t(low) << 16;
        int exponent = (signExponent & 0x7fff) - 0x3fff;
        int field = 0;
        int status = 0;
        if (exponent == -0x3fff)
        {
            if ((mantissa[0] | mantissa[1] | mantissa[2]) != 0)
                status = 2;
            mantissa[0] = mantissa[1] = mantissa[2] = 0;
        }
        else
        {
            const std::uint32_t original[3] = { mantissa[0], mantissa[1], mantissa[2] };
            if (Runtime_RoundMantissa(mantissa, format.precision) != 0)
                ++exponent;
            if (exponent < format.minimumExponent - format.precision)
            {
                mantissa[0] = mantissa[1] = mantissa[2] = 0;
                status = 2;
            }
            else if (exponent <= format.minimumExponent)
            {
                std::memcpy(mantissa, original, sizeof(mantissa));
                Runtime_ShiftMantissaRight(mantissa, format.minimumExponent - exponent);
                Runtime_RoundMantissa(mantissa, format.precision);
                Runtime_ShiftMantissaRight(mantissa, format.exponentBits + 1);
                status = 2;
            }
            else if (exponent >= format.maximumExponent)
            {
                mantissa[0] = 0x80000000u;
                mantissa[1] = mantissa[2] = 0;
                Runtime_ShiftMantissaRight(mantissa, format.exponentBits);
                field = format.bias + format.maximumExponent;
                status = 1;
            }
            else
            {
                mantissa[0] &= 0x7fffffffu;
                Runtime_ShiftMantissaRight(mantissa, format.exponentBits);
                field = format.bias + exponent;
            }
        }
        const std::uint32_t high = (std::uint32_t(field) << (31 - format.exponentBits)) | ((signExponent & 0x8000u) << 16) | mantissa[0];
        if (format.totalBits == 64)
        {
            std::memcpy(destination, &mantissa[1], 4);
            std::memcpy(static_cast<unsigned char*>(destination) + 4, &high, 4);
        }
        else if (format.totalBits == 32)
            std::memcpy(destination, &high, 4);
        return status;
    }

    // Original function: FUN_004f0a07.
    int Runtime_Extended12ToDouble(const std::uint32_t* source, void* destination)
    {
        return Runtime_Extended12ToBinary(source, destination, gRuntimeDoubleFormat);
    }

    // Original function: FUN_004f0a1d.
    int Runtime_Extended12ToFloat(const std::uint32_t* source, void* destination)
    {
        return Runtime_Extended12ToBinary(source, destination, gRuntimeFloatFormat);
    }

    // Original function: FUN_004f0a33.
    int Runtime_ParseDouble(void* destination, const char* text)
    {
        std::uint32_t extended[3];
        const char* end;
        Runtime_ParseExtended12(extended, &end, text, 0, 0, 0, 0);
        return Runtime_Extended12ToDouble(extended, destination);
    }

    // Original function: FUN_004f0a60.
    int Runtime_ParseFloat(void* destination, const char* text)
    {
        std::uint32_t extended[3];
        const char* end;
        Runtime_ParseExtended12(extended, &end, text, 0, 0, 0, 0);
        return Runtime_Extended12ToFloat(extended, destination);
    }

    // Original function: FUN_004f0a8d
    void Runtime_GenerateFloatDigits(char* buffer, std::int32_t digitCount, RuntimeFloatConversionState* state)
    {
        const char* source = state->digits;
        char* output = buffer + 1;

        buffer[0] = '0';

        if (digitCount > 0)
        {
            std::int32_t remaining = digitCount;

            do
            {
                const char character = *source;

                if (character != '\0')
                {
                    *output++ = character;
                    ++source;
                }
                else
                {
                    *output++ = '0';
                }

                --remaining;
            } while (remaining != 0);
        }

        *output = '\0';

        if (digitCount >= 0)
        {
            const std::uint8_t nextByte = static_cast<std::uint8_t>(*source);
            const std::int32_t nextCharacter = nextByte < 0x80 ? static_cast<std::int32_t>(nextByte) : static_cast<std::int32_t>(nextByte) - 0x100;

            // The original uses a signed byte comparison against '5'.
            if (nextCharacter >= '5')
            {
                for (;;)
                {
                    --output;

                    if (*output != '9')
                    {
                        break;
                    }

                    *output = '0';
                }

                const std::uint8_t incrementedByte = static_cast<std::uint8_t>(static_cast<std::uint8_t>(*output) + 1u);
                std::memcpy(output, &incrementedByte, sizeof(incrementedByte));
            }
        }

        if (buffer[0] == '1')
        {
            // Preserve the 32-bit increment of the decimal position.
            std::uint32_t positionBits;
            std::memcpy(&positionBits, &state->decimalPosition, sizeof(positionBits));

            ++positionBits;

            std::memcpy(&state->decimalPosition, &positionBits, sizeof(positionBits));
        }
        else
        {
            const std::size_t byteCount = std::strlen(buffer + 1) + 1;
            std::memmove(buffer, buffer + 1, byteCount);
        }
    }

    // Original function: FUN_004f0b04
    RuntimeFloatConversionState* Runtime_PrepareFloatConversion(double value, RuntimeFloatConversionState* state, char* workspace)
    {
        RuntimeFloatIntermediate intermediate;
        RuntimeFloatDecimalResult decimal;

        Runtime_DoubleToIntermediate(&intermediate, &value);

        // The intermediate value occupies twelve stack bytes by value.
        state->conversionResult = Runtime_IntermediateToDecimal(intermediate, 17, 0, &decimal);

        state->sign = static_cast<std::int32_t>(decimal.sign);
        state->decimalPosition = static_cast<std::int32_t>(decimal.decimalPosition);

        std::strcpy(workspace, decimal.digits);

        state->digits = workspace;

        return state;
    }

    // Original function: FUN_004f0b60
    void Runtime_DoubleToIntermediate(RuntimeFloatIntermediate* destination, const double* source)
    {
        std::uint32_t sourceWords[2];
        std::memcpy(sourceWords, source, sizeof(sourceWords));

        const std::uint32_t sourceLow = sourceWords[0];
        const std::uint32_t sourceHigh = sourceWords[1];

        const std::uint32_t sign = (sourceHigh >> 16) & 0x8000u;
        const std::uint32_t exponent = (sourceHigh >> 20) & 0x7FFu;
        const std::uint32_t fractionHigh = sourceHigh & 0xFFFFFu;

        std::uint32_t explicitIntegerBit = 0x80000000u;
        std::uint32_t extendedExponent;

        if (exponent != 0)
        {
            if (exponent == 0x7FFu)
            {
                extendedExponent = 0x7FFFu;
            }
            else
            {
                extendedExponent = exponent + 0x3C00u;
            }
        }
        else
        {
            if (fractionHigh == 0 && sourceLow == 0)
            {
                // The original also clears the sign of negative zero.
                destination->word1 = 0;
                destination->word0 = 0;
                destination->word2 = 0;
                return;
            }

            extendedExponent = 0x3C01u;
            explicitIntegerBit = 0;
        }

        std::uint32_t mantissaHigh = (sourceLow >> 21) | (fractionHigh << 11) | explicitIntegerBit;
        std::uint32_t mantissaLow = sourceLow << 11;

        destination->word1 = mantissaHigh;
        destination->word0 = mantissaLow;

        while ((mantissaHigh & 0x80000000u) == 0)
        {
            mantissaLow = destination->word0;

            mantissaHigh = (mantissaHigh << 1) | (mantissaLow >> 31);
            mantissaLow <<= 1;

            destination->word0 = mantissaLow;
            destination->word1 = mantissaHigh;

            // Preserve the original addition; only the low word is stored.
            extendedExponent += 0xFFFFu;
        }

        destination->word2 = static_cast<std::uint16_t>(sign | extendedExponent);
    }

    // Original function: FUN_004f0c16
    void Runtime_FloatSupportUnavailable()
    {
        Runtime_FatalExit(2);
    }

    // Original function: FUN_004f0cdc.
    int Runtime_ShowErrorDialog(const char* text, const char* caption, UINT flags)
    {
        if (gRuntimeMessageBox == nullptr)
        {
            HMODULE user32 = LoadLibraryA("user32.dll");
            if (user32 == nullptr)
                return 0;
            gRuntimeMessageBox = reinterpret_cast<RuntimeMessageBoxCallback>(GetProcAddress(user32, "MessageBoxA"));
            if (gRuntimeMessageBox == nullptr)
                return 0;
            gRuntimeGetActiveWindow = reinterpret_cast<RuntimeActiveWindowCallback>(GetProcAddress(user32, "GetActiveWindow"));
            gRuntimeGetLastActivePopup = reinterpret_cast<RuntimeLastPopupCallback>(GetProcAddress(user32, "GetLastActivePopup"));
        }
        HWND owner = nullptr;
        if (gRuntimeGetActiveWindow != nullptr)
        {
            owner = gRuntimeGetActiveWindow();
            if (owner != nullptr && gRuntimeGetLastActivePopup != nullptr)
                owner = gRuntimeGetLastActivePopup(owner);
        }
        return gRuntimeMessageBox(owner, text, caption, flags);
    }

    // Original function: FUN_004f0d65.
    int Runtime_ParseExtended12(std::uint32_t* destination, const char** endPointer, const char* text, int preserveLowWord, int defaultExponent, int defaultFractionDigits, int allowBareExponent)
    {
        const char* cursor = text;
        const char* backup = text;
        signed char digits[25] = {};
        int count = 0, state = 0, exponentSign = 1;
        std::uint32_t adjustment = 0, parsedExponent = 0;
        bool sawDigits = false, sawDecimal = false, sawExponent = false, done = false;
        std::uint16_t sign = 0;
        const auto isDigit = [] (unsigned char c)
        {
            return Runtime_ClassifyCharacter(c, 4) != 0;
        };
        const auto isNonzeroDigit = [] (unsigned char c)
        {
            return c >= '1' && c <= '9';
        };
        while (*cursor == ' ' || *cursor == '\t' || *cursor == '\n' || *cursor == '\r')
            ++cursor;
        while (!done)
        {
            unsigned char c = static_cast<unsigned char>(*cursor++);
            switch (state)
            {
                case 0:
                case 2:
                    if (isNonzeroDigit(c))
                    {
                        state = 3;
                        --cursor;
                    }
                    else if (c == static_cast<unsigned char>(gRuntimeDecimalSeparator))
                        state = 5;
                    else if (c == '0')
                        state = 1;
                    else if (state == 0 && (c == '+' || c == '-'))
                    {
                        sign = c == '-' ? 0x8000 : 0;
                        state = 2;
                    }
                    else
                    {
                        cursor = state == 2 ? backup : cursor - 1;
                        done = true;
                    }
                    break;
                case 1:
                    sawDigits = true;
                    if (c == '0')
                        break;
                    if (isNonzeroDigit(c))
                    {
                        state = 3;
                        --cursor;
                        break;
                    }
                    if (c == static_cast<unsigned char>(gRuntimeDecimalSeparator))
                    {
                        state = 4;
                        break;
                    }
                    goto exponentMarker;
                case 3:
                    sawDigits = true;
                    while (isDigit(c))
                    {
                        if (count < 25)
                            digits[count++] = static_cast<signed char>(c - '0');
                        else
                            ++adjustment;
                        c = static_cast<unsigned char>(*cursor++);
                    }
                    if (c == static_cast<unsigned char>(gRuntimeDecimalSeparator))
                    {
                        state = 4;
                        break;
                    }
                    goto exponentMarker;
                case 4:
                    sawDigits = sawDecimal = true;
                    if (count == 0)
                        while (c == '0')
                        {
                            --adjustment;
                            c = static_cast<unsigned char>(*cursor++);
                        }
                    while (isDigit(c))
                    {
                        if (count < 25)
                        {
                            digits[count++] = static_cast<signed char>(c - '0');
                            --adjustment;
                        }
                        c = static_cast<unsigned char>(*cursor++);
                    }
                    goto exponentMarker;
                case 5:
                    sawDecimal = true;
                    if (isDigit(c))
                    {
                        state = 4;
                        --cursor;
                    }
                    else
                    {
                        cursor = backup;
                        done = true;
                    }
                    break;
                case 6:
                    backup = cursor - 2;
                    if (c == '+' || c == '-')
                    {
                        exponentSign = c == '-' ? -1 : 1;
                        state = 7;
                        break;
                    }
                    [[fallthrough]];
                case 7:
                    if (isNonzeroDigit(c))
                    {
                        state = 9;
                        --cursor;
                    }
                    else if (c == '0')
                        state = 8;
                    else
                    {
                        cursor = backup;
                        done = true;
                    }
                    break;
                case 8:
                    sawExponent = true;
                    while (c == '0')
                        c = static_cast<unsigned char>(*cursor++);
                    if (isNonzeroDigit(c))
                    {
                        state = 9;
                        --cursor;
                    }
                    else
                    {
                        --cursor;
                        done = true;
                    }
                    break;
                case 9:
                    sawExponent = true;
                    while (isDigit(c))
                    {
                        parsedExponent = parsedExponent * 10 + static_cast<std::int32_t>(static_cast<signed char>(c)) - '0';
                        if (static_cast<std::int32_t>(parsedExponent) > 5200)
                        {
                            parsedExponent = 5201;
                            break;
                        }
                        c = static_cast<unsigned char>(*cursor++);
                    }
                    while (isDigit(c))
                        c = static_cast<unsigned char>(*cursor++);
                    --cursor;
                    done = true;
                    break;
                case 11:
                    if (allowBareExponent == 0)
                    {
                        --cursor;
                        done = true;
                        break;
                    }
                    backup = cursor - 1;
                    if (c == '+' || c == '-')
                    {
                        exponentSign = c == '-' ? -1 : 1;
                        state = 7;
                    }
                    else
                    {
                        cursor = backup;
                        done = true;
                    }
                    break;
            }
            continue;
        exponentMarker:
            if (c == '+' || c == '-')
            {
                --cursor;
                state = 11;
            }
            else if (c == 'd' || c == 'D' || c == 'e' || c == 'E')
                state = 6;
            else
            {
                --cursor;
                done = true;
            }
        }
        *endPointer = cursor;
        destination[0] = destination[1] = destination[2] = 0;
        int status = sawDigits ? 0 : 4;
        if (sawDigits && count != 0)
        {
            if (count > 24)
            {
                // Original 004f10f4 reads digits[23], not digits[24].
                if (digits[23] >= 5)
                    ++digits[23];
                count = 24;
                ++adjustment;
            }
            while (count > 0 && digits[count - 1] == 0)
            {
                --count;
                ++adjustment;
            }
            if (count != 0)
            {
                Runtime_DigitsToExtended12(digits, count, destination);
                std::uint32_t exponentBits = (exponentSign < 0 ? 0u - parsedExponent : parsedExponent) + adjustment;
                if (!sawExponent)
                    exponentBits += static_cast<std::uint32_t>(defaultExponent);
                if (!sawDecimal)
                    exponentBits -= static_cast<std::uint32_t>(defaultFractionDigits);
                const auto exponent = static_cast<std::int32_t>(exponentBits);
                if (exponent > 5200)
                {
                    destination[0] = 0;
                    destination[1] = 0;
                    destination[2] = 0x7fff8000u;
                    status = 2;
                }
                else if (exponent < -5200)
                {
                    destination[0] = destination[1] = destination[2] = 0;
                    status = 1;
                }
                else
                    Runtime_ScaleExtended12(destination, exponent, preserveLowWord);
            }
        }
        destination[2] |= std::uint32_t(sign) << 16;
        return status;
    }

    // Original function: FUN_004f1236
    std::int32_t Runtime_CloseAllStreams()
    {
        std::int32_t closedCount = 0;

        Runtime_Lock(2);

        // Standard streams 0, 1 and 2 are excluded.
        for (std::int32_t index = 3; index < gRuntimeStreamCapacity; ++index)
        {
            RuntimeFormatStream* stream = gRuntimeStreamTable[index];

            if (stream == nullptr)
                continue;

            if ((stream->flags & 0x83u) != 0)
            {
                // Original close wrapper: FUN_004e8c4a.
                if ((stream->flags & 0x40u) != 0)
                {
                    stream->flags = 0;
                }
                else
                {
                    // Original pointer-based lock wrappers: FUN_004ee72a / FUN_004ee77c.
                    // Table indices select the same static or embedded locks here.
                    Runtime_LockStreamIndex(index, stream);
                    const std::int32_t result = Runtime_CloseStreamNoLock(stream);
                    Runtime_UnlockStreamIndex(index, stream);

                    if (result != -1)
                        ++closedCount;
                }
            }

            if (index >= 20)
            {
                stream = gRuntimeStreamTable[index];
                auto* lock = reinterpret_cast<CRITICAL_SECTION*>(reinterpret_cast<std::uint8_t*>(stream) + 0x20);
                ::DeleteCriticalSection(lock);

                Runtime_Free(gRuntimeStreamTable[index]);
                gRuntimeStreamTable[index] = nullptr;
            }
        }

        Runtime_Unlock(2);
        return closedCount;
    }

    // Original function: FUN_004f12b7.
    int Runtime_CompareLocaleStrings(LCID locale, DWORD flags, const char* first, int firstLength, const char* second, int secondLength, UINT codePage)
    {
        if (gRuntimeCompareStringBackend == 0)
        {
            if (CompareStringW(0, 0, L"", 1, L"", 1) != 0)
            {
                gRuntimeCompareStringBackend = 1;
            }
            else if (CompareStringA(0, 0, "", 1, "", 1) != 0)
            {
                gRuntimeCompareStringBackend = 2;
            }
            else
            {
                return 0;
            }
        }

        if (firstLength > 0)
        {
            firstLength = Runtime_BoundedStringLength(first, firstLength);
        }

        if (secondLength > 0)
        {
            secondLength = Runtime_BoundedStringLength(second, secondLength);
        }

        if (gRuntimeCompareStringBackend == 2)
        {
            return CompareStringA(locale, flags, first, firstLength, second, secondLength);
        }

        if (gRuntimeCompareStringBackend != 1)
        {
            return 0;
        }

        if (codePage == 0)
        {
            codePage = gRuntimeCodePage;
        }

        if (firstLength == 0 || secondLength == 0)
        {
            if (firstLength == secondLength)
            {
                return 2;
            }

            if (secondLength > 1)
            {
                return 1;
            }

            if (firstLength > 1)
            {
                return 3;
            }

            CPINFO information;

            if (GetCPInfo(codePage, &information) == FALSE)
            {
                return 0;
            }

            if (firstLength > 0)
            {
                if (information.MaxCharSize >= 2)
                {
                    const BYTE character = static_cast<BYTE>(*first);

                    for (int index = 0; information.LeadByte[index] != 0; index += 2)
                    {
                        const BYTE last = information.LeadByte[index + 1];

                        if (last == 0)
                        {
                            break;
                        }

                        if (character >= information.LeadByte[index] && character <= last)
                        {
                            return 2;
                        }
                    }
                }

                return 3;
            }

            if (secondLength > 0)
            {
                if (information.MaxCharSize >= 2)
                {
                    const BYTE character = static_cast<BYTE>(*second);

                    for (int index = 0; information.LeadByte[index] != 0; index += 2)
                    {
                        const BYTE last = information.LeadByte[index + 1];

                        if (last == 0)
                        {
                            break;
                        }

                        if (character >= information.LeadByte[index] && character <= last)
                        {
                            return 2;
                        }
                    }
                }

                return 1;
            }
        }

        const int firstWideLength = MultiByteToWideChar(codePage, 9u, first, firstLength, nullptr, 0);

        if (firstWideLength == 0)
        {
            return 0;
        }

        WCHAR* firstWide = nullptr;

        __try
        {
            const std::uint32_t allocationSize = (static_cast<std::uint32_t>(firstWideLength) * 2u + 3u) & ~3u;
            firstWide = static_cast<WCHAR*>(_alloca(allocationSize));
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            firstWide = nullptr;
        }

        if (firstWide == nullptr)
        {
            return 0;
        }

        if (MultiByteToWideChar(codePage, 1u, first, firstLength, firstWide, firstWideLength) == 0)
        {
            return 0;
        }

        const int secondWideLength = MultiByteToWideChar(codePage, 9u, second, secondLength, nullptr, 0);

        if (secondWideLength == 0)
        {
            return 0;
        }

        WCHAR* secondWide = nullptr;

        __try
        {
            const std::uint32_t allocationSize = (static_cast<std::uint32_t>(secondWideLength) * 2u + 3u) & ~3u;
            secondWide = static_cast<WCHAR*>(_alloca(allocationSize));
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            secondWide = nullptr;
        }

        if (secondWide == nullptr)
        {
            return 0;
        }

        if (MultiByteToWideChar(codePage, 1u, second, secondLength, secondWide, secondWideLength) == 0)
        {
            return 0;
        }

        return CompareStringW(locale, flags, firstWide, firstWideLength, secondWide, secondWideLength);
    }

    // Original function: FUN_004f1534
    std::int32_t Runtime_BoundedStringLength(const char* text, std::int32_t maximumLength)
    {
        const char* position = text;
        std::uint32_t remaining = static_cast<std::uint32_t>(maximumLength);

        while (remaining != 0)
        {
            if (*position == '\0')
                break;

            ++position;
            --remaining;
        }

        // Preserve the original final byte check, including at the limit.
        if (*position == '\0')
        {
            const std::uint32_t lengthBits = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(position) - reinterpret_cast<std::uintptr_t>(text));

            std::int32_t length;
            std::memcpy(&length, &lengthBits, sizeof(length));
            return length;
        }

        return maximumLength;
    }

    // Original function: FUN_004f155f.
    std::int32_t Runtime_SetEnvironmentEntry(char* entry, std::int32_t updateSystemEnvironment)
    {
        if (entry == nullptr)
        {
            return -1;
        }

        const char* separator = Runtime_FindMultibyteCharacter(entry, '=');

        if (separator == nullptr || separator == entry)
        {
            return -1;
        }

        const bool removeEntry = separator[1] == '\0';

        if (gRuntimeEnvironment == gRuntimeInitialEnvironment)
        {
            gRuntimeEnvironment = Runtime_CopyEnvironment(gRuntimeEnvironment);
        }

        if (gRuntimeEnvironment == nullptr)
        {
            if (updateSystemEnvironment != 0 && gRuntimeWideEnvironment != nullptr)
            {
                if (Runtime_ConvertWideEnvironment() != 0)
                {
                    return -1;
                }
            }
            else
            {
                if (removeEntry)
                {
                    return 0;
                }

                gRuntimeEnvironment = static_cast<char**>(Runtime_Allocate(4u));

                if (gRuntimeEnvironment == nullptr)
                {
                    return -1;
                }

                gRuntimeEnvironment[0] = nullptr;

                if (gRuntimeWideEnvironment == nullptr)
                {
                    gRuntimeWideEnvironment = static_cast<wchar_t**>(Runtime_Allocate(4u));

                    if (gRuntimeWideEnvironment == nullptr)
                    {
                        return -1;
                    }

                    gRuntimeWideEnvironment[0] = nullptr;
                }
            }
        }

        const std::int32_t nameLength = static_cast<std::int32_t>(separator - entry);
        char** environment = gRuntimeEnvironment;
        std::int32_t index = Runtime_FindEnvironmentEntry(entry, nameLength);

        if (index >= 0 && environment[0] != nullptr)
        {
            if (removeEntry)
            {
                Runtime_Free(environment[index]);

                do
                {
                    environment[index] = environment[index + 1];
                    ++index;
                } while (environment[index - 1] != nullptr);

                char** resized = static_cast<char**>(Runtime_Reallocate(environment, static_cast<std::uint32_t>(index) * 4u));

                if (resized != nullptr)
                {
                    gRuntimeEnvironment = resized;
                }
            }
            else
            {
                // Preserve the original missing release of the replaced string.
                environment[index] = entry;
            }
        }
        else
        {
            if (removeEntry)
            {
                return 0;
            }

            if (index < 0)
            {
                index = -index;
            }

            const std::uint32_t requiredBytes = static_cast<std::uint32_t>(index) * 4u + 8u;
            char** resized = static_cast<char**>(Runtime_Reallocate(environment, requiredBytes));

            if (resized == nullptr)
            {
                return -1;
            }

            resized[index] = entry;
            resized[index + 1] = nullptr;
            gRuntimeEnvironment = resized;
        }

        if (updateSystemEnvironment != 0)
        {
            const std::uint32_t requiredBytes = static_cast<std::uint32_t>(std::strlen(entry)) + 2u;
            char* temporary = static_cast<char*>(Runtime_Allocate(requiredBytes));

            if (temporary != nullptr)
            {
                std::strcpy(temporary, entry);

                char* temporarySeparator = temporary + (separator - entry);
                *temporarySeparator = '\0';

                const char* value = removeEntry ? nullptr : temporarySeparator + 1;
                SetEnvironmentVariableA(temporary, value);

                Runtime_Free(temporary);
            }
        }

        return 0;
    }

    // Original function: FUN_004f16e6.
    std::int32_t Runtime_FindEnvironmentEntry(const char* name, std::int32_t nameLength)
    {
        char** entry = gRuntimeEnvironment;
        std::int32_t index = 0;

        while (*entry != nullptr)
        {
            if (Runtime_CompareEnvironmentNames(name, *entry, nameLength) == 0)
            {
                const char delimiter = (*entry)[nameLength];

                if (delimiter == '=' || delimiter == '\0')
                {
                    return index;
                }
            }

            ++entry;
            ++index;
        }

        // An empty table returns zero; the caller also checks its first entry.
        return -index;
    }

    // Original function: FUN_004f173e.
    char** Runtime_CopyEnvironment(char** source)
    {
        if (source == nullptr)
        {
            return nullptr;
        }

        std::uint32_t count = 0;

        while (source[count] != nullptr)
        {
            ++count;
        }

        char** result = static_cast<char**>(Runtime_Allocate(count * 4u + 4u));

        if (result == nullptr)
        {
            Runtime_FatalExit(9);
        }

        for (std::uint32_t index = 0; index < count; ++index)
        {
            // Preserve the original unchecked per-string allocation result.
            result[index] = Runtime_DuplicateString(source[index]);
        }

        result[count] = nullptr;
        return result;
    }

    // Original function: FUN_004f17a5
    std::int32_t Runtime_ToUpper(std::int32_t character)
    {
        if (gRuntimeCTypeLocale == 0)
        {
            if (character >= 0x61 && character <= 0x7A)
            {
                return character - 0x20;
            }

            return character;
        }

        auto* localeUsers = reinterpret_cast<volatile LONG*>(&gRuntimeLocaleReaderCount);
        bool localeLocked = false;

        InterlockedIncrement(localeUsers);

        if (gRuntimeLocaleChangeState != 0)
        {
            InterlockedDecrement(localeUsers);
            Runtime_Lock(0x13);
            localeLocked = true;
        }

        const std::uint32_t resultBits = Runtime_ToUpperNoLock(static_cast<std::uint32_t>(character));

        if (localeLocked)
        {
            Runtime_Unlock(0x13);
        }
        else
        {
            InterlockedDecrement(localeUsers);
        }

        std::int32_t result;
        std::memcpy(&result, &resultBits, sizeof(result));
        return result;
    }

    // Original function: FUN_004f1814
    std::uint32_t Runtime_ToUpperNoLock(std::uint32_t character)
    {
        std::int32_t signedCharacter;
        std::memcpy(&signedCharacter, &character, sizeof(signedCharacter));

        if (gRuntimeCTypeLocale == 0)
        {
            if (signedCharacter >= 0x61 && signedCharacter <= 0x7A)
            {
                return character - 0x20u;
            }

            return character;
        }

        if (signedCharacter < 0x100)
        {
            bool isLower;

            if (gRuntimeMaxMultibyteLength > 1)
            {
                isLower = Runtime_ClassifyCharacter(signedCharacter, 2u);
            }
            else
            {
                const auto* characterTypes = reinterpret_cast<const std::uint8_t*>(gRuntimeCharacterTypes);

                // Preserve signed indexing, including the original EOF case.
                isLower = (characterTypes[static_cast<std::ptrdiff_t>(signedCharacter) * 2] & 0x02u) != 0;
            }

            if (!isLower)
            {
                return character;
            }
        }

        const auto* characterTypes = reinterpret_cast<const std::uint8_t*>(gRuntimeCharacterTypes);
        const std::uint32_t highByte = (character >> 8) & 0xFFu;

        char source[3];
        int sourceLength;

        if ((characterTypes[highByte * 2u + 1u] & 0x80u) != 0)
        {
            // Emit the lead byte before the low byte.
            source[0] = static_cast<char>(highByte);
            source[1] = static_cast<char>(character & 0xFFu);
            source[2] = '\0';
            sourceLength = 2;
        }
        else
        {
            source[0] = static_cast<char>(character & 0xFFu);
            source[1] = '\0';
            sourceLength = 1;
        }

        char destination[3];

        const int mappedLength = Runtime_MapString(gRuntimeCTypeLocale, 0x200u, source, sourceLength, destination, 3, 0, 1);

        if (mappedLength == 0)
        {
            return character;
        }

        const auto firstByte = static_cast<std::uint8_t>(destination[0]);

        if (mappedLength == 1)
        {
            return firstByte;
        }

        // Preserve the original output-byte packing.
        const auto secondByte = static_cast<std::uint8_t>(destination[1]);
        return (static_cast<std::uint32_t>(secondByte) << 8) | firstByte;
    }

    // Original function: FUN_004f207a
    std::uint32_t Runtime_Add32WithCarry(std::uint32_t first, std::uint32_t second, std::uint32_t* destination)
    {
        const std::uint32_t sum = first + second;
        const std::uint32_t carry = (sum < first || sum < second) ? 1u : 0u;
        *destination = sum;
        return carry;
    }

    // Original function: FUN_004f20f9
    void Runtime_ShiftLeft96(std::uint32_t* value)
    {
        const std::uint32_t low = value[0];
        const std::uint32_t middle = value[1];

        value[0] = low << 1;

        const std::uint32_t shiftedMiddle = (middle << 1) | (low >> 31);
        const std::uint32_t high = value[2];

        value[1] = shiftedMiddle;
        value[2] = (high << 1) | (middle >> 31);
    }

    // Original function: FUN_004f2127
    void Runtime_ShiftRight96(std::uint32_t* value)
    {
        const std::uint32_t high = value[2];
        const std::uint32_t middle = value[1];

        value[1] = (middle >> 1) | (high << 31);

        const std::uint32_t low = value[0];

        value[2] = high >> 1;
        value[0] = (low >> 1) | (middle << 31);
    }

    // Original function: FUN_004f2154.
    // Called with at least one nonzero decimal digit; words are little-endian.
    void Runtime_DigitsToExtended12(const signed char* digits, int count, std::uint32_t* destination)
    {
        destination[0] = destination[1] = destination[2] = 0;
        std::uint16_t exponent = 0x404e;
        for (int i = 0; i < count; ++i)
        {
            const std::uint32_t original[3] = { destination[0], destination[1], destination[2] };
            Runtime_ShiftLeft96(destination);
            Runtime_ShiftLeft96(destination);
            Runtime_Add96(destination, original);
            Runtime_ShiftLeft96(destination);
            const std::uint32_t digit[3] = { static_cast<std::uint32_t>(static_cast<std::int32_t>(digits[i])), 0, 0 };
            Runtime_Add96(destination, digit);
        }
        while (destination[2] == 0)
        {
            destination[2] = destination[1] >> 16;
            destination[1] = (destination[1] << 16) | (destination[0] >> 16);
            destination[0] <<= 16;
            exponent = static_cast<std::uint16_t>(exponent - 16);
        }
        while ((destination[2] & 0x8000u) == 0)
        {
            Runtime_ShiftLeft96(destination);
            --exponent;
        }
        destination[2] = (destination[2] & 0xffffu) | (std::uint32_t(exponent) << 16);
    }

    // Original function: FUN_004f221b
    std::uint32_t Runtime_IntermediateToDecimal(RuntimeFloatIntermediate value, std::int32_t requestedDigits, std::uint32_t options, RuntimeFloatDecimalResult* result)
    {
        const auto asSigned = [] (std::uint32_t bits) -> std::int32_t
        {
            std::int32_t value;
            std::memcpy(&value, &bits, sizeof(value));
            return value;
        };

        const auto writeZero = [result] () -> std::uint32_t
        {
            result->decimalPosition = 0;
            result->sign = ' ';
            result->digitCount = 1;
            result->digits[0] = '0';
            result->digits[1] = '\0';

            return 1;
        };

        const std::uint32_t sign = value.word2 & 0x8000u;
        const std::uint32_t exponent = value.word2 & 0x7FFFu;
        const std::uint32_t mantissaHigh = value.word1;
        const std::uint32_t mantissaLow = value.word0;

        result->sign = sign != 0 ? '-' : ' ';

        if (exponent == 0 && mantissaHigh == 0 && mantissaLow == 0)
        {
            return writeZero();
        }

        if (exponent == 0x7FFFu)
        {
            result->decimalPosition = 1;

            const bool isInfinity = mantissaHigh == 0x80000000u && mantissaLow == 0;

            if (!isInfinity && (mantissaHigh & 0x40000000u) == 0)
            {
                std::strcpy(result->digits, "1#SNAN");
                result->digitCount = 6;
            }
            else if (sign != 0 && mantissaHigh == 0xC0000000u && mantissaLow == 0)
            {
                std::strcpy(result->digits, "1#IND");
                result->digitCount = 5;
            }
            else if (isInfinity)
            {
                std::strcpy(result->digits, "1#INF");
                result->digitCount = 5;
            }
            else
            {
                std::strcpy(result->digits, "1#QNAN");
                result->digitCount = 6;
            }

            return 0;
        }

        // Original twelve-byte constant at local_20.
        std::uint32_t scaleFactor[3] =
        {
            0xCCCCCCCCu,
            0xCCCCCCCCu,
            0x3FFBCCCCu };

        // The working representation has a two-byte low extension.
        std::uint32_t working[3] =
        {
            mantissaLow << 16,
            (mantissaLow >> 16) | (mantissaHigh << 16),
            (mantissaHigh >> 16) | (exponent << 16) };

        const std::uint32_t estimateBits = exponent * 0x4D10u + ((exponent >> 8) + (mantissaHigh >> 24) * 2u) * 0x4Du + 0xECBced0Cu;

        std::int32_t decimalEstimate = asSigned(estimateBits) >> 16;

        Runtime_ScaleExtended12(working, -decimalEstimate, 1);

        if ((working[2] >> 16) >= 0x3FFFu)
        {
            ++decimalEstimate;
            Runtime_MultiplyExtended12(working, scaleFactor);
        }

        result->decimalPosition = static_cast<std::int16_t>(decimalEstimate);

        std::int32_t digitLimit = requestedDigits;

        if ((options & 0x01u) != 0)
        {
            const std::uint32_t limitBits = static_cast<std::uint32_t>(requestedDigits) + static_cast<std::uint32_t>(static_cast<std::int32_t>(result->decimalPosition));
            digitLimit = asSigned(limitBits);

            if (digitLimit <= 0)
            {
                return writeZero();
            }
        }

        if (digitLimit > 21)
        {
            digitLimit = 21;
        }

        const std::int32_t binaryShift = static_cast<std::int32_t>(working[2] >> 16) - 0x3FFE;

        working[2] &= 0x0000FFFFu;

        for (std::int32_t index = 0; index < 8; ++index)
        {
            Runtime_ShiftLeft96(working);
        }

        if (binaryShift < 0)
        {
            std::uint32_t remainingShifts = static_cast<std::uint32_t>(-binaryShift) & 0xFFu;

            while (remainingShifts != 0)
            {
                Runtime_ShiftRight96(working);
                --remainingShifts;
            }
        }

        auto* resultBytes = reinterpret_cast<std::uint8_t*>(result);
        std::ptrdiff_t outputOffset = 4;

        const std::uint32_t generatedCountBits = static_cast<std::uint32_t>(digitLimit) + std::uint32_t{ 1 };
        std::int32_t remainingDigits = asSigned(generatedCountBits);

        while (remainingDigits > 0)
        {
            std::uint32_t previous[3];
            std::memcpy(previous, working, sizeof(previous));

            Runtime_ShiftLeft96(working);
            Runtime_ShiftLeft96(working);
            Runtime_Add96(working, previous);
            Runtime_ShiftLeft96(working);

            const std::uint8_t digit = static_cast<std::uint8_t>(working[2] >> 24);
            working[2] &= 0x00FFFFFFu;

            resultBytes[outputOffset++] = static_cast<std::uint8_t>(digit + 0x30u);
            --remainingDigits;
        }

        const std::uint8_t roundingByte = resultBytes[outputOffset - 1];
        const std::int32_t roundingCharacter = roundingByte < 0x80 ? static_cast<std::int32_t>(roundingByte) : static_cast<std::int32_t>(roundingByte) - 0x100;

        std::ptrdiff_t lastOffset = outputOffset - 2;

        if (roundingCharacter >= '5')
        {
            while (lastOffset >= 4 && resultBytes[lastOffset] == '9')
            {
                resultBytes[lastOffset] = '0';
                --lastOffset;
            }

            if (lastOffset < 4)
            {
                ++lastOffset;

                std::uint16_t positionBits;
                std::memcpy(&positionBits, &result->decimalPosition, sizeof(positionBits));

                ++positionBits;

                std::memcpy(&result->decimalPosition, &positionBits, sizeof(positionBits));
            }

            resultBytes[lastOffset] = static_cast<std::uint8_t>(resultBytes[lastOffset] + 1u);
        }
        else
        {
            while (lastOffset >= 4 && resultBytes[lastOffset] == '0')
            {
                --lastOffset;
            }

            if (lastOffset < 4)
            {
                return writeZero();
            }
        }

        // Preserve the byte-sized length and its signed extension.
        const std::uint8_t lengthByte = static_cast<std::uint8_t>(lastOffset - 3);
        result->digitCount = lengthByte;

        const std::int32_t signedLength = lengthByte < 0x80 ? static_cast<std::int32_t>(lengthByte) : static_cast<std::int32_t>(lengthByte) - 0x100;
        resultBytes[4 + signedLength] = 0;

        return 1;
    }

    // Original function: FUN_004f24ae
    void Runtime_MultiplyExtended12(std::uint32_t* destination, const std::uint32_t* source)
    {
        const auto read16 = [] (const void* data, std::size_t offset) -> std::uint16_t
        {
            std::uint16_t value;
            std::memcpy(&value, static_cast<const unsigned char*>(data) + offset, sizeof(value));
            return value;
        };

        const auto read32 = [] (const void* data, std::size_t offset) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, static_cast<const unsigned char*>(data) + offset, sizeof(value));
            return value;
        };

        const auto write16 = [] (void* data, std::size_t offset, std::uint16_t value)
        {
            std::memcpy(static_cast<unsigned char*>(data) + offset, &value, sizeof(value));
        };

        const auto write32 = [] (void* data, std::size_t offset, std::uint32_t value)
        {
            std::memcpy(static_cast<unsigned char*>(data) + offset, &value, sizeof(value));
        };

        const auto signedExponent = [] (std::uint32_t bits) -> std::int32_t
        {
            const std::uint32_t low = bits & 0xFFFFu;
            return (low & 0x8000u) != 0
                ? static_cast<std::int32_t>(low) - 0x10000
                : static_cast<std::int32_t>(low);
        };

        const std::uint16_t leftSignExponent = read16(destination, 10);
        const std::uint16_t rightSignExponent = read16(source, 10);
        const std::uint32_t sign = (leftSignExponent ^ rightSignExponent) & 0x8000u;
        const std::uint32_t leftExponent = leftSignExponent & 0x7FFFu;
        const std::uint32_t rightExponent = rightSignExponent & 0x7FFFu;

        std::uint32_t exponentBits = leftExponent + rightExponent;
        std::uint32_t product[3] = {};

        const auto setOverflow = [&] ()
        {
            destination[1] = 0;
            destination[0] = 0;
            destination[2] = 0x7FFF8000u + (sign != 0 ? 0x80000000u : 0u);
        };

        const auto setZero = [&] ()
        {
            destination[2] = 0;
            destination[1] = 0;
            destination[0] = 0;
        };

        if (leftExponent >= 0x7FFFu || rightExponent >= 0x7FFFu || (exponentBits & 0xFFFFu) > 0xBFFDu)
        {
            setOverflow();
            return;
        }

        if ((exponentBits & 0xFFFFu) <= 0x3FBFu)
        {
            setZero();
            return;
        }

        if (leftExponent == 0)
        {
            ++exponentBits;

            if ((destination[2] & 0x7FFFFFFFu) == 0 && destination[1] == 0 && destination[0] == 0)
            {
                write16(destination, 10, 0);
                return;
            }
        }

        if (rightExponent == 0)
        {
            ++exponentBits;

            if ((source[2] & 0x7FFFFFFFu) == 0 && source[1] == 0 && source[0] == 0)
            {
                setZero();
                return;
            }
        }

        // Accumulate only the partial products retained by the original routine.
        for (std::size_t row = 0; row < 5; ++row)
        {
            const std::size_t offset = row * 2;

            for (std::size_t column = 0; column < 5 - row; ++column)
            {
                const std::uint32_t left = read16(destination, (row + column) * 2);
                const std::uint32_t right = read16(source, (4 - column) * 2);
                const std::uint32_t term = left * right;

                std::uint32_t sum;
                const std::uint32_t carry = Runtime_Add32WithCarry(read32(product, offset), term, &sum);
                write32(product, offset, sum);

                if (carry != 0)
                {
                    const std::uint16_t next = read16(product, offset + 4);
                    write16(product, offset + 4, static_cast<std::uint16_t>(next + 1u));
                }
            }
        }

        exponentBits += 0xC002u;

        if (signedExponent(exponentBits) > 0)
        {
            while ((product[2] & 0x80000000u) == 0)
            {
                Runtime_ShiftLeft96(product);
                exponentBits += 0xFFFFu;

                if (signedExponent(exponentBits) <= 0)
                    break;
            }
        }

        if (signedExponent(exponentBits) <= 0)
        {
            exponentBits += 0xFFFFu;

            if (signedExponent(exponentBits) < 0)
            {
                const std::uint32_t shiftCount = static_cast<std::uint32_t>(-signedExponent(exponentBits));
                exponentBits += shiftCount;

                bool discardedBit = false;

                for (std::uint32_t index = 0; index < shiftCount; ++index)
                {
                    if ((product[0] & 1u) != 0)
                        discardedBit = true;

                    Runtime_ShiftRight96(product);
                }

                if (discardedBit)
                    product[0] |= 1u;
            }
        }

        // Round to nearest, choosing an even retained significand on a tie.
        if ((product[0] & 0xFFFFu) > 0x8000u || (product[0] & 0x1FFFFu) == 0x18000u)
        {
            const std::uint32_t low = read32(product, 2);

            if (low != 0xFFFFFFFFu)
            {
                write32(product, 2, low + 1u);
            }
            else
            {
                write32(product, 2, 0);

                const std::uint32_t middle = read32(product, 6);

                if (middle != 0xFFFFFFFFu)
                {
                    write32(product, 6, middle + 1u);
                }
                else
                {
                    write32(product, 6, 0);

                    const std::uint16_t high = read16(product, 10);

                    if (high != 0xFFFFu)
                    {
                        write16(product, 10, static_cast<std::uint16_t>(high + 1u));
                    }
                    else
                    {
                        ++exponentBits;
                        write16(product, 10, 0x8000u);
                    }
                }
            }
        }

        if ((exponentBits & 0xFFFFu) >= 0x7FFFu)
        {
            setOverflow();
            return;
        }

        write16(destination, 0, read16(product, 2));
        write32(destination, 2, product[1]);
        write32(destination, 6, product[2]);
        write16(destination, 10, static_cast<std::uint16_t>(exponentBits | sign));
    }

    // Original function: FUN_004f26ce
    void Runtime_ScaleExtended12(std::uint32_t* value, std::int32_t exponent, std::int32_t preserveLowWord)
    {
        if (exponent == 0)
            return;

        const std::uint32_t(*table)[3] = Runtime_PositivePowerFactors;
        std::uint32_t remaining = static_cast<std::uint32_t>(exponent);

        if (exponent < 0)
        {
            table = Runtime_NegativePowerFactors;
            remaining = 0u - remaining;
        }

        if (preserveLowWord == 0)
            value[0] &= 0xFFFF0000u;

        std::size_t groupOffset = 0;

        do
        {
            const std::uint32_t digit = remaining & 7u;

            // Preserve the original arithmetic right shift.
            remaining = (remaining >> 3) | ((remaining & 0x80000000u) != 0 ? 0xE0000000u : 0u);

            if (digit != 0)
            {
                const std::uint32_t* factor = table[groupOffset + digit - 1u];
                std::uint32_t adjusted[3];

                if ((factor[0] & 0xFFFFu) >= 0x8000u)
                {
                    std::memcpy(adjusted, factor, sizeof(adjusted));

                    // Decrement only the DWORD at byte offset 2.
                    std::uint32_t part;
                    auto* bytes = reinterpret_cast<unsigned char*>(adjusted);
                    std::memcpy(&part, bytes + 2, sizeof(part));
                    --part;
                    std::memcpy(bytes + 2, &part, sizeof(part));

                    factor = adjusted;
                }

                Runtime_MultiplyExtended12(value, factor);
            }

            groupOffset += 7;
        } while (remaining != 0);
    }

    // Original function: FUN_004f274a.
    const char* Runtime_FindMultibyteCharacter(const char* text, std::uint32_t character)
    {
        if (gRuntimeIsMultibyteCodePage == 0)
        {
            return std::strchr(text, static_cast<int>(character & 0xFFu));
        }

        Runtime_Lock(0x19);

        const auto* cursor = reinterpret_cast<const unsigned char*>(text);

        while (*cursor != 0)
        {
            const unsigned char first = *cursor;

            if ((gRuntimeMultibyteTypes[static_cast<std::uint32_t>(first) + 1u] & 4u) != 0)
            {
                const unsigned char second = cursor[1];

                if (second == 0)
                {
                    Runtime_Unlock(0x19);
                    return nullptr;
                }

                const std::uint32_t combined = (static_cast<std::uint32_t>(first) << 8u) | second;

                if (combined == character)
                {
                    Runtime_Unlock(0x19);
                    return reinterpret_cast<const char*>(cursor);
                }

                cursor += 2;
            }
            else
            {
                if (static_cast<std::uint32_t>(first) == character)
                {
                    Runtime_Unlock(0x19);
                    return reinterpret_cast<const char*>(cursor);
                }

                ++cursor;
            }
        }

        Runtime_Unlock(0x19);
        return character == 0 ? reinterpret_cast<const char*>(cursor) : nullptr;
    }
}
