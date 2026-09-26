#pragma once

#include <Windows.h>

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <malloc.h>

#include <new>

// Reconstructed Win32/x86 CRT subset; requires C++17 or later.
// Static initializers in XCrtFileAccess.cpp were verified against Game.exe (image base 00400000).
// Function implementations and mutable runtime data are in XCrtFileAccess.cpp.
// Allocation and freeing use this reconstructed runtime heap throughout.
// Heap, thread, descriptor and stream startup must run in the original order.
// Historical validation results are recorded in earlier project notes.
// This organization pass does not constitute a build or runtime verification.
// Startup audit (entry 004e709e): heap 004ec5de -> threading 004eb167 ->
// descriptor startup 004ea927 -> argument/environment setup -> CRT initializers 004ea109.
// Descriptor startup, CRT initialization and normal/quick exit are reconstructed.
// CRT initialization includes floating-point setup, the onexit table,
// multibyte tables and stream initialization.
// Application initializer callbacks are supplied by the owning application.
// Heap allocation, zeroed allocation, reallocation, size queries and freeing
// use the reconstructed runtime heap, including both small-block heap modes.
// This is the application's reconstructed CRT subset, not a complete CRT library.
// The new semantic names are derived from Windows instructions/data; no matching
// private Microsoft CRT implementation was identified in the supplied Mac sources.

namespace NXBasics
{
    // Types and callback signatures (dependency order; original field layout preserved)

    struct RuntimeErrorMapping
    {
        std::uint32_t osError;
        std::uint32_t crtError;
    };

    struct RuntimeFindData
    {
        std::uint32_t attributes;  // +0x000
        std::int32_t creationTime; // +0x004
        std::int32_t accessTime;   // +0x008
        std::int32_t writeTime;	   // +0x00C
        std::uint32_t fileSize;	   // +0x010
        char name[0x104];		   // +0x014
    };

    struct RuntimeSmallBlockRegion
    {
        std::uint32_t availableClassesLow;	// +0x00
        std::uint32_t availableClassesHigh; // +0x04
        std::uint32_t uncommittedGroups;	// +0x08
        void* reservation;					// +0x0C
        void* metadata;						// +0x10
    };

    struct RuntimeSmallBlockPageEntryMode2
    {
        std::uint32_t freeUnits;		  // +0x00
        std::uint32_t failedRequestLimit; // +0x04
    };

    struct RuntimeSmallBlockRegionMode2
    {
        RuntimeSmallBlockRegionMode2* next;				   // +0x0000
        RuntimeSmallBlockRegionMode2* previous;			   // +0x0004
        RuntimeSmallBlockPageEntryMode2* searchStart;	   // +0x0008
        RuntimeSmallBlockPageEntryMode2* firstUncommitted; // +0x000C
        void* reservation;								   // +0x0010
        void* reservationEnd;							   // +0x0014
        RuntimeSmallBlockPageEntryMode2 pages[0x400];	   // +0x0018
        std::byte trailingStorage[8];					   // +0x2018
    };

    struct RuntimeDescriptorRecord
    {
        std::uint32_t osHandle;			 // +0x00
        std::uint8_t flags;				 // +0x04
        std::uint8_t lookaheadCharacter; // +0x05
        std::byte reserved06[2];		 // +0x06
        std::uint32_t lockInitialized;	 // +0x08
        CRITICAL_SECTION lock;			 // +0x0C
    };

    struct RuntimeExceptionAction
    {
        std::uint32_t exceptionCode;
        std::int32_t signalNumber;
        std::uint32_t action;
    };

    struct RuntimeFormatStream
    {
        char* position;				  // +0x00
        std::uint32_t remainingBytes; // +0x04
        char* buffer;				  // +0x08
        std::uint32_t flags;		  // +0x0C
        std::uint32_t descriptor;	  // +0x10
        std::byte unknown14[4];		  // +0x14
        std::uint32_t bufferSize;	  // +0x18
        char* temporaryFileName;	  // +0x1C
    };

    struct RuntimeFloatConversionState
    {
        std::int32_t sign;				// +0x00
        std::int32_t decimalPosition;	// +0x04
        std::uint32_t conversionResult; // +0x08
        char* digits;					// +0x0C
    };

    struct RuntimeFloatIntermediate
    {
        std::uint32_t word0; // +0x00
        std::uint32_t word1; // +0x04
        std::uint16_t word2; // +0x08
    };

    struct RuntimeFloatDecimalResult
    {
        std::int16_t decimalPosition; // +0x00
        std::int8_t sign;			  // +0x02
        std::uint8_t digitCount;	  // +0x03
        char digits[24];			  // +0x04
    };

    using RuntimeNewHandler = int(__cdecl*)(std::uint32_t);

    struct RuntimeMultibyteProfile
    {
        UINT codePage;
        std::uint32_t fullWidthCase[3];
        std::uint8_t ranges[4][8];
    };

    struct RuntimeBinaryFormat
    {
        int maximumExponent, minimumExponent, precision, exponentBits, totalBits, bias;
    };

    using RuntimeVoidCallback = void(__cdecl*)();

    using RuntimeOnExitCallback = int(__cdecl*)();

    struct RuntimeCalendarTime
    {
        std::int32_t second;   // +0x00
        std::int32_t minute;   // +0x04
        std::int32_t hour;	   // +0x08
        std::int32_t monthDay; // +0x0C
        std::int32_t month;	   // +0x10; January is zero
        std::int32_t year;	   // +0x14; years since 1900
        std::int32_t weekDay;  // +0x18
        std::int32_t yearDay;  // +0x1C; January 1 is zero
        std::int32_t daylight; // +0x20
    };

    using RuntimeMessageBoxCallback = int(WINAPI*)(HWND, LPCSTR, LPCSTR, UINT);

    using RuntimeActiveWindowCallback = HWND(WINAPI*)();

    using RuntimeLastPopupCallback = HWND(WINAPI*)(HWND);

    struct RuntimeFatalMessage
    {
        int code;
        const char* text;
    };

    // Function declarations (original-address order; unaddressed helpers last)

    bool Runtime_IsLetter(char character);
    bool Runtime_IsDigit(char character);
    bool Runtime_IsLetterOrNumber(char character);

    bool Runtime_IsPropertyNameCharacter(char character);

    std::int32_t Runtime_StringToInteger(const char* text);

    RuntimeOnExitCallback Runtime_RegisterOnExit(RuntimeOnExitCallback callback);

    int Runtime_AtExit(RuntimeOnExitCallback callback);

    void Runtime_InitializeOnExitTable();

    std::int32_t Runtime_Close(std::uint32_t descriptor);

    std::int32_t Runtime_CloseNoLock(std::uint32_t descriptor);

    std::int32_t Runtime_Write(std::uint32_t descriptor, const void* data, std::uint32_t byteCount);

    std::int32_t Runtime_WriteNoLock(std::uint32_t descriptor, const void* data, std::uint32_t byteCount);

    std::int32_t Runtime_Open(const char* fileName, std::uint32_t flags, std::uint32_t permissions);

    std::int32_t Runtime_OpenShared(const char* fileName, std::uint32_t flags, std::uint32_t shareMode, std::uint32_t permissions);

    std::int32_t Runtime_Read(std::uint32_t descriptor, void* destination, std::uint32_t byteCount);

    std::int32_t Runtime_ReadNoLock(std::uint32_t descriptor, void* destination, std::uint32_t byteCount);

    std::uint32_t Runtime_Seek(std::uint32_t descriptor, std::int32_t offset, std::uint32_t origin);

    std::uint32_t Runtime_SeekNoLock(std::uint32_t descriptor, std::int32_t offset, std::uint32_t origin);

    std::uint32_t Runtime_GetFileSize(std::uint32_t descriptor);

    void Runtime_InitializeFloatingPoint();

    void Runtime_InitializeFloatCallbacks();

    std::int32_t Runtime_CompareStringsIgnoreCaseN(const char* first, const char* second, std::uint32_t count);

    void Runtime_FatalExit(int errorCode);

    void Runtime_StartupFatalExit(int errorCode);

    char* Runtime_GetErrorText(std::int32_t errorCode);

    void Runtime_MapOsError(std::uint32_t osError);

    DWORD* Runtime_GetErrnoPointer();

    DWORD* Runtime_GetDosErrnoPointer();

    std::int32_t Runtime_FormatString(char* destination, std::int32_t capacity, const char* format, va_list arguments);

    std::int32_t Runtime_FindFirst(const char* pattern, void* result);

    std::int32_t Runtime_FindNext(std::int32_t searchHandle, void* result);

    std::int32_t Runtime_FindClose(std::int32_t searchHandle);

    std::int32_t Runtime_FileTimeToTime32(const FILETIME* fileTime);

    std::int32_t Runtime_ToLower(std::int32_t character);

    std::uint32_t Runtime_ToLowerNoLock(std::uint32_t character);

    void Runtime_Abort();

    int Runtime_CallNewHandler(std::uint32_t allocationSize);

    void Runtime_SplitPath(const char* path, char* drive, char* directory, char* baseName, char* extension);

    std::uint32_t Runtime_ClassifyAlphanumeric(std::int32_t character);

    std::int32_t Runtime_CompareStringsIgnoreCase(const char* first, const char* second);

    void Runtime_MakePath(char* path, const char* drive, const char* directory, const char* baseName, const char* extension);

    double Runtime_StringToDouble(const char* text);

    int Runtime_RaiseSignal(std::int32_t signalNumber);

    RuntimeExceptionAction* Runtime_FindSignalAction(std::int32_t signalNumber, RuntimeExceptionAction* actions);

    void Runtime_Free(void* memory);

    void* Runtime_Allocate(std::uint32_t requestedBytes);

    void* Runtime_AllocateWithNewHandler(std::uint32_t requestedBytes, std::int32_t useNewHandler);

    void* Runtime_AllocateHeapBlock(std::uint32_t requestedBytes);

    std::int32_t Runtime_CloseStreamNoLock(RuntimeFormatStream* stream);

    std::int32_t Runtime_FlushStreamNoLock(RuntimeFormatStream* stream);

    std::int32_t Runtime_FlushStreamBuffer(RuntimeFormatStream* stream);

    std::int32_t Runtime_FlushStreams(std::int32_t mode);

    char* Runtime_GetEnvironmentVariable(const char* name);

    std::uint32_t Runtime_FlushAndWriteCharacter(std::uint32_t character, RuntimeFormatStream* stream);

    std::int32_t Runtime_FormatToStream(RuntimeFormatStream* stream, const char* format, va_list arguments);

    void Runtime_FormatWriteCharacter(std::uint32_t character, RuntimeFormatStream* stream, std::int32_t* writtenCharacters);

    void Runtime_FormatWriteRepeated(std::uint32_t character, std::int32_t count, RuntimeFormatStream* stream, std::int32_t* writtenCharacters);

    void Runtime_FormatWriteText(const char* text, std::int32_t length, RuntimeFormatStream* stream, std::int32_t* writtenCharacters);

    std::uint32_t Runtime_FormatReadArgument32(va_list* arguments);

    std::uint64_t Runtime_FormatReadArgument64(va_list* arguments);

    std::uint32_t Runtime_FormatReadArgument16(va_list* arguments);

    std::uint32_t Runtime_ClassifyCharacter(std::int32_t character, std::uint32_t mask);

    void Runtime_InitializeCrt(RuntimeVoidCallback* applicationBegin, RuntimeVoidCallback* applicationEnd);

    void Runtime_Exit(std::int32_t exitCode);

    void Runtime_QuickExit(int exitCode);

    void Runtime_PerformExit(std::int32_t exitCode, std::int32_t quickExit, std::int32_t returnToCaller);

    void Runtime_CallInitializers(RuntimeVoidCallback* begin, RuntimeVoidCallback* end);

    void* Runtime_Reallocate(void* memory, std::uint32_t requestedBytes);

    std::uint32_t Runtime_GetAllocationSize(void* memory);

    std::uint32_t Runtime_AllocateDescriptor();

    std::int32_t Runtime_SetOsHandle(std::uint32_t descriptor, HANDLE handle);

    std::int32_t Runtime_ClearOsHandle(std::uint32_t descriptor);

    std::uint32_t Runtime_GetOsHandle(std::uint32_t descriptor);

    void Runtime_LockDescriptor(std::uint32_t descriptor);

    void Runtime_UnlockDescriptor(std::uint32_t descriptor);

    void Runtime_InitializeDescriptors();

    void Runtime_InitializeTimeZone();

    void Runtime_RefreshTimeZone();

    std::int32_t Runtime_IsDaylightTime(const RuntimeCalendarTime* calendar);

    std::int32_t Runtime_IsDaylightTimeNoLock(const RuntimeCalendarTime* calendar);

    void Runtime_SetDaylightTransition(std::int32_t isStart, std::int32_t ruleType, std::int32_t year, std::int32_t month, std::int32_t week, std::int32_t weekDay, std::int32_t monthDay, std::int32_t hour, std::int32_t minute, std::int32_t second, std::int32_t millisecond);

    std::int32_t Runtime_MakeTime32(std::int32_t year, std::int32_t month, std::int32_t day, std::int32_t hour, std::int32_t minute, std::int32_t second, std::int32_t daylight);

    std::int32_t Runtime_InitializeThreading();

    void Runtime_InitializeThreadData(void* threadData);

    DWORD* Runtime_GetThreadData();

    std::int32_t Runtime_ResizeNoLock(std::uint32_t descriptor, std::uint32_t newSize);

    int Runtime_ProbePentiumDivision();

    int Runtime_GetPentiumDivisionAdjustment();

    void Runtime_FloatInsertDecimalPoint(char* text);

    void Runtime_FloatTrimTrailingZeros(char* text);

    std::int32_t Runtime_FloatAtLeastReference(const double* value);

    void Runtime_AssignFloat(int storeDouble, void* destination, const char* text);

    char* Runtime_ConvertFloatScientific(const double* value, char* buffer, std::int32_t precision, std::int32_t uppercase);

    char* Runtime_FormatExponentialDigits(char* buffer, std::int32_t precision, std::int32_t uppercase, const RuntimeFloatConversionState* state, char generalFormat);

    char* Runtime_ConvertFloatFixed(const double* value, char* buffer, std::int32_t precision);

    char* Runtime_FormatFixedDigits(char* buffer, std::int32_t precision, const RuntimeFloatConversionState* state, char generalFormat);

    void Runtime_ConvertFloatGeneral(const double* value, char* buffer, std::int32_t precision, std::int32_t uppercase);

    void Runtime_ConvertFloat(const double* value, char* buffer, std::int32_t format, std::int32_t precision, std::int32_t uppercase);

    void Runtime_ShiftString(char* text, std::int32_t offset);

    void Runtime_InitializeLocks();

    void Runtime_Lock(std::int32_t lockIndex);

    void Runtime_Unlock(std::int32_t lockIndex);

    void Runtime_InitializeEnvironment();

    char* Runtime_GetEnvironmentBlock();

    void Runtime_GetExecutableLinkerVersion(std::uint32_t* version);

    std::int32_t Runtime_SelectHeapMode();

    int Runtime_InitializeHeap(std::int32_t enableSerialization);

    void Runtime_WriteErrorBanner();

    void Runtime_WriteErrorMessage(int errorCode);

    int Runtime_MapString(LCID locale, DWORD mapFlags, const char* source, int sourceLength, char* destination, int destinationCapacity, UINT codePage, int rejectInvalidCharacters);

    int Runtime_SetMultibyteCodePage(std::int32_t requestedCodePage);

    UINT Runtime_ResolveMultibyteCodePage(std::int32_t requestedCodePage);

    LCID Runtime_GetMultibyteLocale(UINT codePage);

    void Runtime_ResetMultibyteCodePage();

    void Runtime_UpdateMultibyteCaseTables();

    void Runtime_InitializeMultibyte();

    char* Runtime_CopyMultibyteBytes(char* destination, const char* source, std::uint32_t byteCount);

    const char* Runtime_PreviousMultibyteCharacter(const char* start, const char* current);

    int Runtime_InitializeSmallBlockHeap(std::uint32_t maximumRequest);

    std::uint32_t Runtime_FindSmallBlockRegion(std::uint32_t memoryAddress);

    void Runtime_FreeSmallBlock(std::uint32_t regionAddress, void* memory);

    void* Runtime_AllocateSmallBlock(std::uint32_t requestedBytes);

    void* Runtime_CreateSmallBlockRegion();

    std::int32_t Runtime_CommitSmallBlockGroup(std::uint32_t regionAddress);

    int Runtime_ResizeSmallBlock(std::uint32_t regionAddress, void* memory, std::uint32_t requestedBytes);

    void* Runtime_CreateSmallBlockRegionMode2();

    void Runtime_ReleaseSmallBlockRegionMode2(std::uint32_t regionAddress);

    void Runtime_DecommitSmallBlockPagesMode2(std::uint32_t pagesToRelease);

    std::uint32_t Runtime_FindSmallBlockEntryMode2(void* memory, std::uint32_t* regionOutput, std::uint32_t* pageOutput);

    void Runtime_FreeSmallBlockMode2(std::uint32_t regionAddress, std::uint32_t pageAddress, std::uint32_t entryAddress);

    void* Runtime_AllocateSmallBlockMode2(std::uint32_t unitCount);

    void* Runtime_AllocateFromSmallBlockPageMode2(void* pageMemory, std::uint32_t freeUnits, std::uint32_t unitCount);

    int Runtime_ResizeSmallBlockMode2(std::uint32_t regionAddress, std::uint32_t pageAddress, std::uint32_t entryAddress, std::uint32_t requestedUnits);

    void Runtime_InitializeStreams();

    void Runtime_PreTerminateStreams();

    void Runtime_LockStreamIndex(std::int32_t index, RuntimeFormatStream* stream);

    void Runtime_UnlockStreamIndex(std::int32_t index, RuntimeFormatStream* stream);

    void Runtime_FreeStreamBuffer(RuntimeFormatStream* stream);

    std::int32_t Runtime_CommitDescriptor(std::uint32_t descriptor);

    std::int32_t Runtime_CompareEnvironmentNames(const char* first, const char* second, std::int32_t length);

    std::int32_t Runtime_ConvertWideEnvironment();

    wchar_t* Runtime_DuplicateWideString(const wchar_t* source);

    char* Runtime_DuplicateString(const char* source);

    std::int32_t Runtime_StringToLong(const char* text, char** endPointer, std::int32_t base);

    std::uint32_t Runtime_ParseInteger32(const char* text, char** endPointer, std::int32_t base, std::uint32_t flags);

    std::int32_t Runtime_RemoveTemporaryStreams();

    void Runtime_AllocateStreamBuffer(RuntimeFormatStream* stream);

    std::uint32_t Runtime_IsCharacterDevice(std::uint32_t descriptor);

    std::int32_t Runtime_WideCharacterToMultibyte(char* destination, std::uint32_t characterBits);

    std::int32_t Runtime_WideCharacterToMultibyteNoLock(char* destination, std::uint32_t characterBits);

    BOOL Runtime_GetStringType(DWORD informationType, LPCSTR source, int sourceLength, LPWORD characterTypes, UINT codePage, LCID locale, int rejectInvalidCharacters);

    void* Runtime_AllocateZeroed(std::uint32_t count, std::uint32_t elementSize);

    std::int32_t Runtime_SetModeNoLock(std::uint32_t descriptor, std::int32_t mode);

    std::uint32_t Runtime_ControlX87(std::uint32_t newControl, std::uint32_t mask);

    std::uint32_t Runtime_ControlFloatingPoint(std::uint32_t newControl, std::uint32_t mask);

    std::uint32_t Runtime_DecodeX87ControlWord(std::uint16_t controlWord);

    std::uint16_t Runtime_EncodeX87ControlWord(std::uint32_t control);

    bool Runtime_MantissaTailIsZero(const std::uint32_t* value, int bitIndex);

    std::uint32_t Runtime_IncrementMantissa(std::uint32_t* value, int bitIndex);

    std::uint32_t Runtime_RoundMantissa(std::uint32_t* value, int precision);

    void Runtime_ShiftMantissaRight(std::uint32_t* value, int count);

    int Runtime_Extended12ToBinary(const std::uint32_t* source, void* destination, const RuntimeBinaryFormat& format);

    int Runtime_Extended12ToDouble(const std::uint32_t* source, void* destination);

    int Runtime_Extended12ToFloat(const std::uint32_t* source, void* destination);

    int Runtime_ParseDouble(void* destination, const char* text);

    int Runtime_ParseFloat(void* destination, const char* text);

    void Runtime_GenerateFloatDigits(char* buffer, std::int32_t digitCount, RuntimeFloatConversionState* state);

    RuntimeFloatConversionState* Runtime_PrepareFloatConversion(double value, RuntimeFloatConversionState* state, char* workspace);

    void Runtime_DoubleToIntermediate(RuntimeFloatIntermediate* destination, const double* source);

    void Runtime_FloatSupportUnavailable();

    int Runtime_ShowErrorDialog(const char* text, const char* caption, UINT flags);

    int Runtime_ParseExtended12(std::uint32_t* destination, const char** endPointer, const char* text, int preserveLowWord, int defaultExponent, int defaultFractionDigits, int allowBareExponent);

    std::int32_t Runtime_CloseAllStreams();

    int Runtime_CompareLocaleStrings(LCID locale, DWORD flags, const char* first, int firstLength, const char* second, int secondLength, UINT codePage);

    std::int32_t Runtime_BoundedStringLength(const char* text, std::int32_t maximumLength);

    std::int32_t Runtime_SetEnvironmentEntry(char* entry, std::int32_t updateSystemEnvironment);

    std::int32_t Runtime_FindEnvironmentEntry(const char* name, std::int32_t nameLength);

    char** Runtime_CopyEnvironment(char** source);

    std::int32_t Runtime_ToUpper(std::int32_t character);

    std::uint32_t Runtime_ToUpperNoLock(std::uint32_t character);

    std::uint32_t Runtime_Add32WithCarry(std::uint32_t first, std::uint32_t second, std::uint32_t* destination);

    void Runtime_ShiftLeft96(std::uint32_t* value);

    void Runtime_ShiftRight96(std::uint32_t* value);

    void Runtime_DigitsToExtended12(const signed char* digits, int count, std::uint32_t* destination);

    std::uint32_t Runtime_IntermediateToDecimal(RuntimeFloatIntermediate value, std::int32_t requestedDigits, std::uint32_t options, RuntimeFloatDecimalResult* result);

    void Runtime_MultiplyExtended12(std::uint32_t* destination, const std::uint32_t* source);

    void Runtime_ScaleExtended12(std::uint32_t* value, std::int32_t exponent, std::int32_t preserveLowWord);

    const char* Runtime_FindLastMultibyteCharacter(const char* text, std::uint32_t character);
    const char* Runtime_FindMultibyteCharacter(const char* text, std::uint32_t character);

    void Runtime_Add96(std::uint32_t* destination, const std::uint32_t* source);

    std::int32_t Runtime_TimeSigned32(std::uint32_t bits);

    char* Runtime_LowercaseString(char* text);
    void Runtime_LockStream(RuntimeFormatStream* stream);
    void Runtime_UnlockStream(RuntimeFormatStream* stream);
    std::int32_t Runtime_CloseStream(RuntimeFormatStream* stream);
    std::int32_t Runtime_FlushStream(RuntimeFormatStream* stream);
    std::int32_t Runtime_PrintString(char* destination, const char* format, ...);
    std::int32_t Runtime_PrintStringN(char* destination, std::uint32_t capacity, const char* format, ...);
    char* Runtime_GetLocalTimeString(char* destination);
    char* Runtime_LongToString(std::int32_t value, char* destination, std::int32_t radix);
    std::int32_t Runtime_BeginTemporaryStreamBuffer(RuntimeFormatStream* stream);
    void Runtime_EndTemporaryStreamBuffer(std::int32_t temporaryBufferInstalled, RuntimeFormatStream* stream);
    std::int32_t Runtime_PrintStream(RuntimeFormatStream* stream, const char* format, ...);
    RuntimeFormatStream* Runtime_AcquireUnusedStream();
    RuntimeFormatStream* Runtime_OpenStreamNoLock(const char* fileName, const char* mode, std::uint32_t shareMode, RuntimeFormatStream* stream);
    RuntimeFormatStream* Runtime_OpenStreamShared(const char* fileName, const char* mode, std::uint32_t shareMode);
    RuntimeFormatStream* Runtime_OpenStream(const char* fileName, const char* mode);
    char* Runtime_GetEnvironmentVariableLocked(const char* name);

    std::int32_t Runtime_RemoveFile(const char* fileName);
    std::int32_t Runtime_RenameFile(const char* oldFileName, const char* newFileName);
    std::int32_t Runtime_SeekStream(RuntimeFormatStream* stream, std::int32_t offset, std::uint32_t origin);
    std::int32_t Runtime_SeekStreamNoLock(RuntimeFormatStream* stream, std::int32_t offset, std::uint32_t origin);
    std::int32_t Runtime_WriteStreamCharacter(std::int32_t character, RuntimeFormatStream* stream);
    std::int32_t Runtime_TellStream(RuntimeFormatStream* stream);
    std::int32_t Runtime_TellStreamNoLock(RuntimeFormatStream* stream);
    RuntimeFormatStream* Runtime_CreateTemporaryStream();
    void Runtime_InitializeTemporaryFileName(std::int32_t temporaryStream);
    std::int32_t Runtime_AdvanceTemporaryFileName(char* fileName);
    std::int32_t Runtime_ReadStreamCharacter(RuntimeFormatStream* stream);
    std::int32_t Runtime_FillStreamBufferAndReadCharacter(RuntimeFormatStream* stream);
    std::uint32_t Runtime_WriteStream(const void* data, std::uint32_t elementSize, std::uint32_t elementCount, RuntimeFormatStream* stream);
    std::uint32_t Runtime_WriteStreamNoLock(const void* data, std::uint32_t elementSize, std::uint32_t elementCount, RuntimeFormatStream* stream);

    std::uint32_t Runtime_ReadStream(void* destination, std::uint32_t elementSize, std::uint32_t elementCount, RuntimeFormatStream* stream);
    std::uint32_t Runtime_ReadStreamNoLock(void* destination, std::uint32_t elementSize, std::uint32_t elementCount, RuntimeFormatStream* stream);

    // Shared runtime data and compile-time tables

    // Original data: DAT_004fb9d8 through 004fba33.
    inline constexpr std::uint8_t gRuntimeFormatTable[] =
        {
            0x06, 0x00, 0x00, 0x06, 0x00, 0x01, 0x00, 0x00,
            0x10, 0x00, 0x03, 0x06, 0x00, 0x06, 0x02, 0x10,
            0x04, 0x45, 0x45, 0x45, 0x05, 0x05, 0x05, 0x05,
            0x05, 0x35, 0x30, 0x00, 0x50, 0x00, 0x00, 0x00,
            0x00, 0x20, 0x28, 0x38, 0x50, 0x58, 0x07, 0x08,
            0x00, 0x37, 0x30, 0x30, 0x57, 0x50, 0x07, 0x00,
            0x00, 0x20, 0x20, 0x08, 0x00, 0x00, 0x00, 0x00,
            0x08, 0x60, 0x68, 0x60, 0x60, 0x60, 0x60, 0x00,
            0x00, 0x70, 0x70, 0x78, 0x78, 0x78, 0x78, 0x08,
            0x07, 0x08, 0x00, 0x00, 0x07, 0x00, 0x08, 0x08,
            0x08, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 0x07,
            0x08, 0x00, 0x00, 0x00};

    // Original range: 004fe070..004fe07c, end exclusive.
    // The adapter discards the original temporary-stream cleanup return value.
    extern RuntimeVoidCallback gRuntimePreTerminators[3];

    // Original range: 004fe080..004fe084 (exclusive), one null slot in Game.exe.
    extern RuntimeVoidCallback gRuntimeTerminators[1];

    // Original data: 0050c230, initially FUN_004e6efc.
    extern RuntimeVoidCallback gRuntimeFloatingPointInitializer;

    // Original data: 0050c240, initially FUN_004ea147.
    extern void(__cdecl* gRuntimeFatalExitCallback)(int);

    // Additional CRT data (original-address order).
    // Original data: 0050c244.
    extern std::uint32_t gRuntimeApplicationType;

    // Original data: 0050c250 through 0050c3b7.
    inline constexpr RuntimeErrorMapping gRuntimeErrorMappings[] =
        {
            {0x0001u, 0x16u},
            {0x0002u, 0x02u},
            {0x0003u, 0x02u},
            {0x0004u, 0x18u},
            {0x0005u, 0x0Du},
            {0x0006u, 0x09u},
            {0x0007u, 0x0Cu},
            {0x0008u, 0x0Cu},
            {0x0009u, 0x0Cu},
            {0x000Au, 0x07u},
            {0x000Bu, 0x08u},
            {0x000Cu, 0x16u},
            {0x000Du, 0x16u},
            {0x000Fu, 0x02u},
            {0x0010u, 0x0Du},
            {0x0011u, 0x12u},
            {0x0012u, 0x02u},
            {0x0021u, 0x0Du},
            {0x0035u, 0x02u},
            {0x0041u, 0x0Du},
            {0x0043u, 0x02u},
            {0x0050u, 0x11u},
            {0x0052u, 0x0Du},
            {0x0053u, 0x0Du},
            {0x0057u, 0x16u},
            {0x0059u, 0x0Bu},
            {0x006Cu, 0x0Du},
            {0x006Du, 0x20u},
            {0x0070u, 0x1Cu},
            {0x0072u, 0x09u},
            {0x0006u, 0x16u}, // Preserve the duplicate and original order.
            {0x0080u, 0x0Au},
            {0x0081u, 0x0Au},
            {0x0082u, 0x09u},
            {0x0083u, 0x16u},
            {0x0084u, 0x0Du},
            {0x0091u, 0x29u},
            {0x009Eu, 0x0Du},
            {0x00A1u, 0x02u},
            {0x00A4u, 0x0Bu},
            {0x00A7u, 0x0Du},
            {0x00B7u, 0x11u},
            {0x00CEu, 0x02u},
            {0x00D7u, 0x0Bu},
            {0x0718u, 0x0Cu}};

    // Original data: 0050c3f0; original target: 004fba44.
    extern const char* gRuntimeNullString;

    // Original data: 0050c3f4; original target: 004fba34.
    extern const wchar_t* gRuntimeNullWideString;

    // Original data: 0050c400 through 0050c601; element zero represents EOF.
    extern std::uint16_t gRuntimeCharacterTypeStorage[257];

    // Original data: 0050c3f8; original target: 0050c402.
    extern std::uint16_t* gRuntimeCharacterTypes;

    // Original data: 0050c604.
    extern std::int32_t gRuntimeMaxMultibyteLength;

    // Original data: 0050c608.
    extern char gRuntimeDecimalSeparator;

    // Original data: 0050c610 through 0050c633.
    extern RuntimeDescriptorRecord gRuntimeInvalidDescriptorRecord;

    // Original data: 0050c634.
    extern std::int32_t gRuntimeTimeZoneBiasSeconds;

    // Original data: 0050c638.
    extern std::int32_t gRuntimeDaylightEnabled;

    // Original data: 0050c63c.
    extern std::int32_t gRuntimeDaylightBiasSeconds;

    // Original address: 0050c640.
    extern char gRuntimeStandardTimeZoneName[64];

    // Original address: 0050c680.
    extern char gRuntimeDaylightTimeZoneName[64];

    // Original addresses: 0050c6c0 and 0050c6c4.
    extern char* gRuntimeTimeZoneNames[2];

    // Original address: 0050c6c8.
    extern std::int32_t gRuntimeDaylightStartYear;

    // Original address: 0050c6cc.
    extern std::int32_t gRuntimeDaylightStartYearDay;

    // Original address: 0050c6d0.
    extern std::int32_t gRuntimeDaylightStartMilliseconds;

    // Original address: 0050c6d8.
    extern std::int32_t gRuntimeDaylightEndYear;

    // Original address: 0050c6dc.
    extern std::int32_t gRuntimeDaylightEndYearDay;

    // Original address: 0050c6e0.
    extern std::int32_t gRuntimeDaylightEndMilliseconds;

    // Original data: 0050c6e4.
    extern DWORD gRuntimeThreadDataTlsIndex;

    // Float dispatch slots initially point to the missing-float-support handler.
    // Original data: 0050c780.
    extern decltype(&Runtime_ConvertFloat) gRuntimeFloatConvertCallback;

    // Original data: 0050c784.
    extern decltype(&Runtime_FloatTrimTrailingZeros) gRuntimeFloatTrimTrailingZerosCallback;

    // Original data: 0050c788.
    extern decltype(&Runtime_AssignFloat) gRuntimeFloatAssignCallback;

    // Original data: 0050c78c.
    extern decltype(&Runtime_FloatInsertDecimalPoint) gRuntimeFloatInsertDecimalPointCallback;

    // Original data: 0050c790.
    extern decltype(&Runtime_FloatAtLeastReference) gRuntimeFloatReferenceCheckCallback;

    // Original data: 0050c794.
    extern decltype(&Runtime_ConvertFloat) gRuntimeFloatConvertAlternateCallback;

    // Original data: 0050c968 through 0050c9df.
    extern RuntimeExceptionAction gRuntimeDefaultExceptionActions[10];

    // Original data: 0050c9e0.
    extern std::int32_t gRuntimeFloatingPointActionBegin;

    // Original data: 0050c9e4.
    extern std::int32_t gRuntimeFloatingPointActionCount;

    // Original data: 0050c9ec.
    extern std::int32_t gRuntimeExceptionActionCount;

    // Original table: 0050ca08..0050ca98 (exclusive).
    extern const RuntimeFatalMessage gRuntimeFatalMessages[];

    // Original data: 0050ca98 through 0050cb47.
    // Exact strings recovered from Game.exe, including original wording.
    extern const char* gRuntimeErrorTextTable[];

    // Original data: 0050cb48.
    extern std::int32_t gRuntimeErrorTextCount;

    // Original data: 0050cb50.
    inline constexpr std::uint8_t gRuntimeMultibyteRangeMasks[4] = {
        0x01, 0x02, 0x04, 0x08};

    // Original data: 0050cb58 through 0050cc47.
    inline constexpr RuntimeMultibyteProfile gRuntimeMultibyteProfiles[5] = {
        {932,
         {0x82798260u, 0x00000021u, 0x00000000u},
         {{0xA6, 0xDF, 0, 0, 0, 0, 0, 0},
          {0xA1, 0xA5, 0, 0, 0, 0, 0, 0},
          {0x81, 0x9F, 0xE0, 0xFC, 0, 0, 0, 0},
          {0x40, 0x7E, 0x80, 0xFC, 0, 0, 0, 0}}},
        {936,
         {0xA3DAA3C1u, 0x00000020u, 0x00000000u},
         {{},
          {},
          {0x81, 0xFE, 0, 0, 0, 0, 0, 0},
          {0x40, 0xFE, 0, 0, 0, 0, 0, 0}}},
        {949,
         {0xA3DAA3C1u, 0x00000020u, 0x00000000u},
         {{},
          {},
          {0x81, 0xFE, 0, 0, 0, 0, 0, 0},
          {0x41, 0xFE, 0, 0, 0, 0, 0, 0}}},
        {950,
         {0xA2E4A2CFu, 0xA2E5001Au, 0x005BA2E8u},
         {{},
          {},
          {0x81, 0xFE, 0, 0, 0, 0, 0, 0},
          {0x40, 0x7E, 0xA1, 0xFE, 0, 0, 0, 0}}},
        {1361,
         {0xDA5EDA51u, 0xDA5F0020u, 0x0032DA6Au},
         {{},
          {},
          {0x81, 0xD3, 0xD8, 0xDE, 0xE0, 0xF9, 0, 0},
          {0x31, 0x7E, 0x81, 0xFE, 0, 0, 0, 0}}}};

    // Original data: 0050cca0 through 0050ecbf.
    extern RuntimeSmallBlockRegionMode2 gRuntimeSmallBlockInitialRegionMode2;

    // Original data: 0050ecc0.
    extern RuntimeSmallBlockRegionMode2* gRuntimeSmallBlockSearchRegionMode2;

    // Original data: 0050ecc4.
    extern std::uint32_t gRuntimeSmallBlockThresholdMode2;

    // Original indexed memory base: 0050ef54.
    // Index zero belongs to the preceding data; valid months use indices 1 through 12.
    // Index 13 supplies the final day of December.
    inline constexpr std::int32_t gRuntimeLeapMonthDayOffsets[14] = {1, -1, 30, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

    // Original indexed memory base: 0050ef88.
    // Index zero is the final entry of the preceding leap-year table.
    inline constexpr std::int32_t gRuntimeMonthDayOffsets[14] = {365, -1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364};

    // Original data: 0050f120.
    extern const RuntimeBinaryFormat gRuntimeDoubleFormat;

    // Original data: 0050f138.
    extern const RuntimeBinaryFormat gRuntimeFloatFormat;

    // Original data: DAT_0050f360
    inline constexpr std::uint32_t Runtime_PositivePowerFactors[29][3] = {
        {0x00000000u, 0x00000000u, 0x4002A000u}, // 10^1
        {0x00000000u, 0x00000000u, 0x4005C800u}, // 10^2
        {0x00000000u, 0x00000000u, 0x4008FA00u}, // 10^3
        {0x00000000u, 0x00000000u, 0x400C9C40u}, // 10^4
        {0x00000000u, 0x00000000u, 0x400FC350u}, // 10^5
        {0x00000000u, 0x00000000u, 0x4012F424u}, // 10^6
        {0x00000000u, 0x80000000u, 0x40169896u}, // 10^7
        {0x00000000u, 0x20000000u, 0x4019BEBCu}, // 10^8
        {0x00000000u, 0xC9BF0400u, 0x40348E1Bu}, // 10^16
        {0xA1000000u, 0x1BCECCEDu, 0x404ED3C2u}, // 10^24
        {0xB59EF020u, 0xADA82B70u, 0x40699DC5u}, // 10^32
        {0x25FD5DD0u, 0x4F8E1AE5u, 0x4083EB19u}, // 10^40
        {0x95D79671u, 0x8D050E43u, 0x409EAF29u}, // 10^48
        {0x44A0BFF9u, 0x8F1281EDu, 0x40B98281u}, // 10^56
        {0xA6D53CBFu, 0x1F49FFCFu, 0x40D3C278u}, // 10^64
        {0x8CE0C66Fu, 0x47C980E9u, 0x41A893BAu}, // 10^128
        {0x556B85BCu, 0xF78D3927u, 0x427CE070u}, // 10^192
        {0xDE8EDDBCu, 0xEBFB9DF9u, 0x4351AA7Eu}, // 10^256
        {0xE376E6A1u, 0x2F29F2CCu, 0x44268184u}, // 10^320
        {0xAA171028u, 0xE310AEF8u, 0x44FAC4C5u}, // 10^384
        {0xF3D4A7EBu, 0x4AE1EBF7u, 0x45CF957Au}, // 10^448
        {0x91C7CC65u, 0xA0AEA60Eu, 0x46A3E319u}, // 10^512
        {0x0C17650Du, 0x75868175u, 0x4D48C976u}, // 10^1024
        {0xA7E44258u, 0x353B3993u, 0x53EDB2B8u}, // 10^1536
        {0x5DE5A74Du, 0x3B5DC53Du, 0x5A929E8Bu}, // 10^2048
        {0xF0A65DFFu, 0x54C020A1u, 0x61378CA5u}, // 10^2560
        {0x5A8BFDD1u, 0x5D25D88Bu, 0x67DBF989u}, // 10^3072
        {0xF3F895AAu, 0xC8A2BF27u, 0x6E80DD5Du}, // 10^3584
        {0x979BC94Cu, 0x52028A20u, 0x7525C460u}, // 10^4096
    };

    // Original data: DAT_0050f4c0
    inline constexpr std::uint32_t Runtime_NegativePowerFactors[29][3] = {
        {0xCCCDCCCDu, 0xCCCCCCCCu, 0x3FFBCCCCu}, // 10^-1
        {0xD70A3D71u, 0x0A3D70A3u, 0x3FF8A3D7u}, // 10^-2
        {0xDF3B645Au, 0x6E978D4Fu, 0x3FF58312u}, // 10^-3
        {0x652CD3C3u, 0x1758E219u, 0x3FF1D1B7u}, // 10^-4
        {0x84230FD0u, 0xAC471B47u, 0x3FEEA7C5u}, // 10^-5
        {0x69B6A640u, 0xBD05AF6Cu, 0x3FEB8637u}, // 10^-6
        {0x42BC3D33u, 0x94D5E57Au, 0x3FE7D6BFu}, // 10^-7
        {0xCEFDFDC2u, 0x77118461u, 0x3FE4ABCCu}, // 10^-8
        {0xE15B4C2Fu, 0x94BEC44Du, 0x3FC9E695u}, // 10^-16
        {0x3B53C492u, 0x14CD4475u, 0x3FAF9ABEu}, // 10^-24
        {0x94BA67DEu, 0x1EAD4539u, 0x3F94CFB1u}, // 10^-32
        {0xE2C62324u, 0x313BBABCu, 0x3F7A8B61u}, // 10^-40
        {0xC1595561u, 0x7C53B17Eu, 0x3F5FBB12u}, // 10^-48
        {0x8D2FEED7u, 0x8592BE06u, 0x3F44FB15u}, // 10^-56
        {0xE9A53F24u, 0xEA27A539u, 0x3F2AA87Fu}, // 10^-64
        {0xE4A1AC7Du, 0x467C64BCu, 0x3E55DDD0u}, // 10^-128
        {0xCC067B63u, 0x83775423u, 0x3D8191FFu}, // 10^-192
        {0x193AFA91u, 0x4325637Au, 0x3CACC031u}, // 10^-256
        {0x38D18921u, 0xB8974782u, 0x3BD7FD00u}, // 10^-320
        {0x085888DCu, 0xE3E8B11Bu, 0x3B03A686u}, // 10^-384
        {0x424584C6u, 0x7599B607u, 0x3A2EDB37u}, // 10^-448
        {0xD21C7133u, 0xEE32DB23u, 0x395A9049u}, // 10^-512
        {0xC0BE87A6u, 0x82A5DA57u, 0x32B5A2A6u}, // 10^-1024
        {0x11B268E2u, 0x449F52A7u, 0x2C10B759u}, // 10^-1536
        {0x2DE44925u, 0x534F3436u, 0x256BCEAEu}, // 10^-2048
        {0xA404598Fu, 0x7DC2DEC0u, 0x1EC6E8FBu}, // 10^-2560
        {0x5A88E79Eu, 0xBF3C9157u, 0x18228350u}, // 10^-3072
        {0x62654B4Eu, 0xAF8F83FDu, 0x117D9406u}, // 10^-3584
        {0x9FDE2DE4u, 0x04C8D2CEu, 0x0AD8A6DDu}, // 10^-4096
    };

    // Original data: 00569990.
    extern int gRuntimePentiumFdivAdjustment;

    // Original address: 00569994.
    extern char* gRuntimeStartupEnvironmentBlock;

    // Original data: 0056999c.
    extern int gRuntimeErrorOutputMode;

    // Original storage starts at 005699a0.
    // The next known global starts at 005699cc; this span holds every table string.
    extern char gRuntimeFallbackErrorText[0x2C];

    // Original data: 005699c8.
    extern RuntimeNewHandler gRuntimeNewHandler;

    // Original data: 005699cc.
    extern std::uint32_t gRuntimeInterruptSignalAction;

    // Original data: 005699d0.
    extern std::uint32_t gRuntimeBreakSignalAction;

    // Original data: 005699d4.
    extern std::uint32_t gRuntimeAbortSignalAction;

    // Original data: 005699d8.
    extern std::uint32_t gRuntimeTerminateSignalAction;

    // Original data: 00569a00.
    extern std::uint32_t gRuntimeFileCreationMask;

    // Original address: 00569a20.
    extern char** gRuntimeEnvironment;

    // Original address: 00569a24.
    // Shared environment table checked before making a writable copy.
    extern char** gRuntimeInitialEnvironment;

    // Original address: 00569a28.
    extern wchar_t** gRuntimeWideEnvironment;

    // Original data: 00569a38.
    extern unsigned char gRuntimeCleanupReturnsToCaller;

    // Original data: 00569a3c.
    extern int gRuntimeCleanupStarted;

    // Original data: 00569a40.
    extern int gRuntimeProcessExitStarted;

    // Original address: 00569a48.
    extern std::int32_t gRuntimeUsesWindowsTimeZone;

    // Original address: 00569a50.
    extern TIME_ZONE_INFORMATION gRuntimeTimeZoneInformation;

    // Original address: 00569afc.
    extern char* gRuntimePreviousTimeZoneEnvironment;

    // Original data: 00569b00.
    extern std::uint32_t gRuntimeTimeZoneInitialized;

    // Original data: 00569b2c.
    extern std::int32_t gRuntimeDefaultFileMode;

    // Original data: 00569b30 through 00569b8f.
    extern CRITICAL_SECTION gRuntimeStaticLocks[4];

    // Original data: 0050c798 through 0050c857.
    extern CRITICAL_SECTION* gRuntimeLockTable[48];

    // Original data: 00569b98.
    extern LCID gRuntimeCTypeLocale;

    // Original data: 00569ba8.
    extern UINT gRuntimeCodePage;

    // Original address: 00569cc0.
    // 0 = undetected, 1 = Unicode API, 2 = ANSI API.
    extern std::int32_t gRuntimeEnvironmentStringsBackend;

    // Original data: 00569cc4.
    extern RuntimeVoidCallback gRuntimeErrorBannerCallback;

    // Original data: 00569cc8.
    extern std::uint32_t gRuntimeMapStringBackend;

    // Original data: 00569ccc.
    extern std::int32_t gRuntimeNewHandlerMode;

    // Original data: 00569cd0.
    extern int gRuntimeSystemCodePageRequested;

    // Original data: 00569cd4.
    extern std::uint32_t gRuntimeSmallBlockEmptyPageCountMode2;

    // Original data: 00569cd8.
    extern std::uint32_t gRuntimeStreamBufferRequestCount;

    // Original data: 00569cdc and 00569ce0.
    extern char* gRuntimeTemporaryStreamBuffers[2];

    // Original data: 00569ce4.
    extern std::uint32_t gRuntimeDefaultStreamFlags;

    // Original data: 00569cec.
    extern std::uint32_t gRuntimeGetStringTypeBackend;

    // Original data: 00569d54.
    extern RuntimeMessageBoxCallback gRuntimeMessageBox;

    // Original data: 00569d58.
    extern RuntimeActiveWindowCallback gRuntimeGetActiveWindow;

    // Original data: 00569d5c.
    extern RuntimeLastPopupCallback gRuntimeGetLastActivePopup;

    // Original address: 00569d60.
    // 0 = undetected, 1 = Unicode API, 2 = ANSI API.
    extern std::int32_t gRuntimeCompareStringBackend;

    // Original data: 0056a384.
    extern RuntimeFormatStream** gRuntimeStreamTable;

    // Original data: 0056a3a0 through 0056b39f.
    extern char gRuntimeStdinBuffer[0x1000];

    // Original data: 0050ecc8 through 0050ef47.
    extern RuntimeFormatStream gRuntimeStaticStreams[20];

    // Original data: 0050ecc8.
    extern RuntimeFormatStream& gRuntimeStdinStream;

    // Original data: 0050ece8.
    extern RuntimeFormatStream& gRuntimeStdoutStream;

    // Original data: 0050ed08.
    extern RuntimeFormatStream& gRuntimeStderrStream;

    // Original data: 0056b3a0.
    extern std::int32_t gRuntimeStreamCapacity;

    // Original data: 0056b3a4.
    extern std::uint32_t gRuntimeSmallBlockRegionCapacity;

    // Original data: 0056b3a8.
    extern std::uint32_t gRuntimeSmallBlockPendingGroupIndex;

    // Original data: 0056b3ac.
    // Stores a 32-bit address in the current x86 representation.
    extern std::uint32_t gRuntimeSmallBlockSearchRegion;

    // Original data: 0056b3b0.
    // Stores a 32-bit address in the current x86 representation.
    extern std::uint32_t gRuntimeSmallBlockPendingRegion;

    // Original data: 0056b3b4.
    extern std::uint32_t gRuntimeSmallBlockRegionCount;

    // Original data: 0056b3b8.
    extern void* gRuntimeSmallBlockRegionTable;

    // Original data: 0056b3bc.
    extern std::uint32_t gRuntimeSmallBlockThreshold;

    // Original data: 0056b3c0.
    extern UINT gRuntimeMultibyteCodePage;

    // Original data: 0056b3d0 through 0056b3db.
    // Preserve the original packed representation.
    extern std::uint32_t gRuntimeMultibyteFullWidthCase[3];

    // Original data: 0056b3dc.
    extern int gRuntimeIsMultibyteCodePage;

    // Original data: 0056b3e0 through 0056b4df.
    extern std::uint8_t gRuntimeMultibyteCaseMap[256];

    // Original data: 0056b4e0 through 0056b5e0.
    // Byte values use indices 1 through 256.
    extern std::uint8_t gRuntimeMultibyteTypes[257];

    // Original data: 0056b5e4.
    extern LCID gRuntimeMultibyteLocale;

    // Original data: 0056b5e8.
    extern HANDLE gRuntimeHeap;

    // Original data: 0056b5ec.
    extern std::uint32_t gRuntimeHeapMode;

    // Original data: 0056b5f0.
    extern LONG gRuntimeLocaleChangeState;

    // Original data: 0056b5f4.
    extern LONG gRuntimeLocaleReaderCount;

    // Original data: 0056b600 through 0056b6ff.
    // Each allocated block contains 32 descriptor records.
    extern std::uint8_t* gRuntimeDescriptorBlocks[64];

    // Original data: 0056b700.
    // Runtime descriptor initialization sets this to 32 after allocating block zero.
    extern std::uint32_t gRuntimeDescriptorCapacity;

    // Original address: 0056b704.
    extern std::int32_t gRuntimeEnvironmentInitialized;

    // Original data: 0056b708.
    extern int gRuntimeMultibyteInitialized;

    // Original data: 0056b70c.
    extern RuntimeOnExitCallback* gRuntimeOnExitEnd;

    // Original data: 0056b710.
    extern RuntimeOnExitCallback* gRuntimeOnExitBegin;

    // Layout and table assertions

    static_assert(sizeof(RuntimeFindData) == 0x118, "Unexpected runtime find data size");

    static_assert(offsetof(RuntimeFindData, name) == 0x14, "Unexpected runtime find name offset");

    static_assert(sizeof(RuntimeSmallBlockRegion) == 0x14);

    static_assert(offsetof(RuntimeSmallBlockRegion, reservation) == 0x0C);

    static_assert(offsetof(RuntimeSmallBlockRegion, metadata) == 0x10);

    static_assert(sizeof(RuntimeSmallBlockPageEntryMode2) == 8);

    static_assert(sizeof(RuntimeSmallBlockRegionMode2) == 0x2020);

    static_assert(offsetof(RuntimeSmallBlockRegionMode2, searchStart) == 0x08);

    static_assert(offsetof(RuntimeSmallBlockRegionMode2, reservation) == 0x10);

    static_assert(offsetof(RuntimeSmallBlockRegionMode2, pages) == 0x18);

    static_assert(offsetof(RuntimeSmallBlockRegionMode2, trailingStorage) == 0x2018);

    static_assert(sizeof(void*) == 4, "The reconstructed runtime requires x86");

    static_assert(sizeof(CRITICAL_SECTION) == 0x18);

    static_assert(sizeof(RuntimeDescriptorRecord) == 0x24);

    static_assert(offsetof(RuntimeDescriptorRecord, flags) == 0x04);

    static_assert(offsetof(RuntimeDescriptorRecord, lookaheadCharacter) == 0x05);

    static_assert(offsetof(RuntimeDescriptorRecord, lockInitialized) == 0x08);

    static_assert(offsetof(RuntimeDescriptorRecord, lock) == 0x0C);

    static_assert(sizeof(RuntimeExceptionAction) == 0x0C);

    static_assert(offsetof(RuntimeExceptionAction, action) == 0x08);

    static_assert(sizeof(gRuntimeDefaultExceptionActions) == 0x78);

    static_assert(sizeof(void*) == 4, "RuntimeFormatStream requires an x86 build");

    static_assert(sizeof(RuntimeFormatStream) == 0x20);

    static_assert(offsetof(RuntimeFormatStream, position) == 0x00);

    static_assert(offsetof(RuntimeFormatStream, remainingBytes) == 0x04);

    static_assert(offsetof(RuntimeFormatStream, buffer) == 0x08);

    static_assert(offsetof(RuntimeFormatStream, flags) == 0x0C);

    static_assert(offsetof(RuntimeFormatStream, descriptor) == 0x10);

    static_assert(offsetof(RuntimeFormatStream, bufferSize) == 0x18);

    static_assert(sizeof(gRuntimeErrorMappings) / sizeof(gRuntimeErrorMappings[0]) == 45, "The original error table contains 45 entries");

    static_assert(sizeof(gRuntimeStaticLocks) == 0x60);

    static_assert(sizeof(gRuntimeLockTable) == 0xC0);

    static_assert(sizeof(gRuntimeStaticStreams) == 0x280);

    static_assert(sizeof(gRuntimeFormatTable) == 0x5C);

    static_assert(sizeof(gRuntimeDescriptorBlocks) == 0x100);

    static_assert(sizeof(RuntimeDescriptorRecord) * 32 == 0x480);

    static_assert(sizeof(RuntimeFloatConversionState) == 0x10);

    static_assert(offsetof(RuntimeFloatConversionState, sign) == 0x00);

    static_assert(offsetof(RuntimeFloatConversionState, decimalPosition) == 0x04);

    static_assert(offsetof(RuntimeFloatConversionState, conversionResult) == 0x08);

    static_assert(offsetof(RuntimeFloatConversionState, digits) == 0x0C);

    static_assert(sizeof(RuntimeFloatIntermediate) == 0x0C);

    static_assert(offsetof(RuntimeFloatIntermediate, word2) == 0x08);

    static_assert(sizeof(RuntimeFloatDecimalResult) == 0x1C);

    static_assert(offsetof(RuntimeFloatDecimalResult, sign) == 0x02);

    static_assert(offsetof(RuntimeFloatDecimalResult, digitCount) == 0x03);

    static_assert(offsetof(RuntimeFloatDecimalResult, digits) == 0x04);

    static_assert(sizeof(Runtime_PositivePowerFactors) == 0x15C);

    static_assert(sizeof(Runtime_NegativePowerFactors) == 0x15C);

    static_assert(sizeof(va_list) == 4, "Argument access requires the MSVC x86 va_list layout");

    static_assert(sizeof(CRITICAL_SECTION) == 0x18, "Descriptor locks require the Win32 critical-section layout");

    static_assert(sizeof(void*) == 4, "These CRT descriptor accesses require an x86 build");

    static_assert(sizeof(wchar_t) == 2, "The runtime requires Windows UTF-16 wchar_t");

    static_assert(sizeof(RuntimeCalendarTime) == 0x24, "Unexpected runtime calendar size");

    static_assert(offsetof(RuntimeCalendarTime, year) == 0x14, "Unexpected calendar year offset");

    static_assert(offsetof(RuntimeCalendarTime, yearDay) == 0x1C, "Unexpected calendar day offset");

} // namespace NXBasics
