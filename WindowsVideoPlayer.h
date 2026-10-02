#pragma once
#include <Windows.h>
#include <ddraw.h>
#include <amstream.h>
#include <ddstream.h>
#include <cstddef>
#include <cstdint>

namespace NWindowsVideo
{
    // The original class name is unresolved; fields follow verified COM identities.
    class CVideoPlayer
    {
    public:
        CVideoPlayer();
        ~CVideoPlayer();
        bool Open(const char* fileName, IDirectDraw* directDraw, bool enableAudio);
        void Close();
        void Start();
        void Stop();
        bool Update(IDirectDrawSurface* destination, const RECT* destinationRectangle);
        bool IsPlaying() const { return mPlaying != 0; }
        inline static CVideoPlayer* sTheObjectPtr = nullptr;
    private:
        bool l_CheckAvailability();
        IDirectDraw* mDirectDraw;
        IAMMultiMediaStream* mMultiMediaStream;
        IMediaStream* mMediaStream;
        IDirectDrawMediaStream* mDrawMediaStream;
        IDirectDrawSurface* mSurface;
        IDirectDrawStreamSample* mSample;
        RECT mSourceRectangle;
        std::uint8_t mAvailable;
        std::uint8_t mPlaying;
        std::byte mPadding[2];
    };
    static_assert(sizeof(CVideoPlayer) == 0x2C);
}
