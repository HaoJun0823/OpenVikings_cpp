#include "WindowsVideoPlayer.h"
#include "XFile.h"
#include <cstring>

namespace NWindowsVideo
{
	// Complete seven-GUID Windows region 004FC47C..004FC4EB.
	static const GUID gFilterGraphInterface = { 0x56A868A9, 0x0AD4, 0x11CE,{ 0xB0, 0x3A, 0, 0x20, 0xAF, 0x0B, 0xA7, 0x70 } };
	static const GUID gDrawMediaStreamInterface = { 0xF4104FCE, 0x9A70, 0x11D0,{ 0x8F, 0xDE, 0, 0xC0, 0x4F, 0xD9, 0x18, 0x9D } };
	static const GUID gMultiMediaStreamClass = { 0x49C47CE5, 0x9BA4, 0x11D0,{ 0x82, 0x12, 0, 0xC0, 0x4F, 0xC3, 0x2C, 0x45 } };
	static const GUID gMultiMediaStreamInterface = { 0xBEBE595C, 0x9A6F, 0x11D0,{ 0x8F, 0xDE, 0, 0xC0, 0x4F, 0xD9, 0x18, 0x9D } };
	static const GUID gFilterGraphClass = { 0xE436EBB3, 0x524F, 0x11CE,{ 0x9F, 0x53, 0, 0x20, 0xAF, 0x0B, 0xA7, 0x70 } };
	static const GUID gPrimaryAudioPurpose = { 0xA35FF56B, 0x9FDA, 0x11D0,{ 0x8F, 0xDF, 0, 0xC0, 0x4F, 0xD9, 0x18, 0x9D } };
	static const GUID gPrimaryVideoPurpose = { 0xA35FF56A, 0x9FDA, 0x11D0,{ 0x8F, 0xDF, 0, 0xC0, 0x4F, 0xD9, 0x18, 0x9D } };

	// Original function: FUN_00404452.
	CVideoPlayer::CVideoPlayer()
	{
		sTheObjectPtr = this;
		std::memset(this, 0, sizeof(*this));
	}

	// Original function: FUN_0040446c.
	CVideoPlayer::~CVideoPlayer()
	{
		Stop();
		Close();
		sTheObjectPtr = nullptr;
	}

	// Original function: FUN_004046da. Only S_OK accepts the probe.
	bool CVideoPlayer::l_CheckAvailability()
	{
		CoInitialize(nullptr);
		IUnknown* graph;
		if (CoCreateInstance(gFilterGraphClass, nullptr, CLSCTX_INPROC_SERVER, gFilterGraphInterface, reinterpret_cast<void**>(&graph)) == S_OK)
		{
			graph->Release();
			mAvailable = 1;
			return true;
		}
		mAvailable = 0;
		return false;
	}

	// Original function: FUN_00404484.
	bool CVideoPlayer::Open(const char* fileName, IDirectDraw* directDraw, bool enableAudio)
	{
		if (!l_CheckAvailability()) return false;
		char resolvedFile[MAX_PATH];
		if (!NXBasics::CFile::FileSystem_Tool_GetRealFilename(fileName, resolvedFile)) return false;
		IAMMultiMediaStream* stream = nullptr;
		HRESULT result = CoCreateInstance(gMultiMediaStreamClass, nullptr, CLSCTX_INPROC_SERVER, gMultiMediaStreamInterface, reinterpret_cast<void**>(&stream));
		if (SUCCEEDED(result)) result = stream->Initialize(STREAMTYPE_READ, 1, nullptr);
		if (SUCCEEDED(result))
		{
			mDirectDraw = directDraw;
			directDraw->AddRef();
			result = stream->AddMediaStream(directDraw, &gPrimaryVideoPurpose, 0, nullptr);
		}
		if (SUCCEEDED(result))
		{
			if (enableAudio) stream->AddMediaStream(nullptr, &gPrimaryAudioPurpose, 1, nullptr);
			WCHAR wideFile[MAX_PATH];
			MultiByteToWideChar(CP_ACP, 0, resolvedFile, -1, wideFile, MAX_PATH);
			result = stream->OpenFile(wideFile, 0);
		}
		if (SUCCEEDED(result))
		{
			mMultiMediaStream = stream;
			result = stream->GetMediaStream(gPrimaryVideoPurpose, &mMediaStream);
		}
		if (SUCCEEDED(result)) result = mMediaStream->QueryInterface(gDrawMediaStreamInterface, reinterpret_cast<void**>(&mDrawMediaStream));
		if (SUCCEEDED(result)) result = mDrawMediaStream->CreateSample(nullptr, nullptr, 0, &mSample);
		if (SUCCEEDED(result)) result = mSample->GetSurface(&mSurface, &mSourceRectangle);
		if (SUCCEEDED(result)) return true;
		// Before assignment to +4 the local stream is deliberately not released.
		Close();
		mAvailable = 0;
		return false;
	}

	// Original function: FUN_004045cb. Playing state is not cleared here.
	void CVideoPlayer::Close()
	{
		if (mSurface != nullptr)
		{
			mSurface->Release(); mSurface = nullptr;
		}
		if (mSample != nullptr)
		{
			mSample->Release(); mSample = nullptr;
		}
		if (mDrawMediaStream != nullptr)
		{
			mDrawMediaStream->Release(); mDrawMediaStream = nullptr;
		}
		if (mMediaStream != nullptr)
		{
			mMediaStream->Release(); mMediaStream = nullptr;
		}
		if (mMultiMediaStream != nullptr)
		{
			mMultiMediaStream->Release(); mMultiMediaStream = nullptr;
		}
		if (mDirectDraw != nullptr)
		{
			mDirectDraw->Release(); mDirectDraw = nullptr;
		}
		mAvailable = 0;
	}

	// Original function: FUN_00404635. Seek precedes the availability check.
	void CVideoPlayer::Start()
	{
		mMultiMediaStream->Seek(0);
		if (mAvailable != 0)
		{
			mMultiMediaStream->SetState(STREAMSTATE_RUN);
			mPlaying = 1;
		}
	}

	// Original function: FUN_0040465c.
	void CVideoPlayer::Stop()
	{
		if (mAvailable != 0 && mPlaying != 0)
		{
			mMultiMediaStream->SetState(STREAMSTATE_STOP);
			mPlaying = 0;
		}
	}

	// Original function: FUN_0040467c. Sample update requires exact S_OK.
	bool CVideoPlayer::Update(IDirectDrawSurface* destination, const RECT* destinationRectangle)
	{
		if (mAvailable == 0) return false;
		RECT rectangle = destinationRectangle != nullptr ? *destinationRectangle : mSourceRectangle;
		if (mSample->Update(0, nullptr, nullptr, 0) != S_OK)
		{
			mPlaying = 0;
			return false;
		}
		destination->Blt(&rectangle, mSurface, &mSourceRectangle, 0, nullptr);
		return true;
	}
}
