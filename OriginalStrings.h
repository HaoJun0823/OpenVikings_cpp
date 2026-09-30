#pragma once

// Exact NUL-terminated bytes recovered from Game.exe; sorted by original address.
namespace OriginalStrings
{
    // Original data: 004fe164.
    inline constexpr char Dot[4] = ".";

    // Original data: 004fe238.
    inline constexpr char UnknownPlayerName[] = "?";

    // Original data: 004ff054.
    inline constexpr char WindowsRootPath[4] = "\\";

    // Original data: 004ff058
    inline constexpr char FileOpenForReadingError[] = "NXBasics::CFile::OpenForReading\n\nfile %s - errno %d: %s";

    // Original data: 004ff090
    inline constexpr char FileOpenForWritingError[] = "NXBasics::CFile::OpenForWriting\n\nfile %s - errno %d: %s";

    // Original data: 004ff0c8
    inline constexpr char FileCloseError[] = "NXBasics::CFile::Close\n\nfile %s - errno %d: %s";

    // Original data: 004ff994
    inline constexpr char CannotSaveVirtualBitmap[] = "NXBasics::CBitmap::Storable_SaveData\n\ncannot save virtual bitmap";

    // Original data: 0050071c
    inline constexpr char DirectPlayErrorSeparator[] = " - ";

    // Original data: 00500818.
    inline constexpr char DirectoryAndFileNameFormat[] = "%s\\%s";

    // Original data: 0050716c.
    inline constexpr char RightArrow[] = ">";

    // Original data: 005087fc
    inline constexpr char NetworkLogTimeFormat[] = "%s ";

    // Original data: 00509630
    inline constexpr char NewLine[] = "\n";

    // Original data: 0050978c
    inline constexpr char NetworkLogIndentation[] = "\t";

    // Original data: 00509910.
    inline constexpr char FileTransferServiceUnavailable[] = "No file transfer service - message ignored\n";

    // Original data: 00509dec
    inline constexpr char NetworkPeerDestroyedLog[] = "DirectPlay peer object destroyed. Cannot re-initialize DirectPlay";

    // Original data: 00509e30
    inline constexpr char NetworkPeerDestroyedError[] = "CDPNet::ReInitialize(): DirectPlay peer object destroyed. Cannot re-initialize DirectPlay.";

    // Original data: 00509e8c
    inline constexpr char NetworkReinitializationFailed[] = "IDirectPlay8Peer-Reinitialization failed";

    // Original data: 00509eb8
    inline constexpr char NetworkInitializeOperation[] = "CDPNet::ReInitialize(): IDirectPlay8Peer::Initialize";

    // Original data: 0050a698
    inline constexpr char NetworkReturnBufferOperation[] = "CDPNet::ReleaseReceivedMessages(): IDirectPlay8Peer::ReturnBuffer";

    // Original data: 0050a9c8
    inline constexpr char TimerTimeoutLabel[] = "\nTimeout [ms]: ";

    // Original data: 0050a9d8
    inline constexpr char TimerThreadFormat[] = "%x";

    // Original data: 0050a9dc
    inline constexpr char TimerThreadLabel[] = "Thread ID: 0x";

    // Original data: 0050a9ec
    inline constexpr char TimerNotRespondingWarning[] = "CDPTimer::~CDPTimer() - Warning: Thread stopped responding.\n";

}
