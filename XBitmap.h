#pragma once

#include "XBasicTypes.h"
#include "XStorable.h"

#include <cstddef>
#include <cstdint>

namespace NXBasics
{
	class CBitmap : public CStorable
	{
	public:
		// Original function: FUN_0040c624
		CBitmap(std::int32_t width, std::int32_t height, std::int32_t depth);

		// Original function: FUN_0040c65f
		CBitmap(CBitmap* sourceBitmap, const SRectangle& rectangle);

		// Original function: FUN_0040cb4c
		void CopyIntoBitmap(CBitmap* destination, std::int32_t destinationX, std::int32_t destinationY) const;

		// Original function: FUN_0040d56b
		void CopyIntoBitmap_FitIn(CBitmap* destination) const;

		// Original function: FUN_0040eeb1
		const std::uint16_t* GetHighColorTablePtr() const;

		// Original function: FUN_0040eee3
		const std::uint32_t* GetTrueColorTablePtr() const;

		// Original function: FUN_0040d9ee
		std::uint8_t Draw_GetPixel(std::int32_t x, std::int32_t y) const;

		// Original function: FUN_0040da2b
		std::uint16_t Draw_GetPixel_Word(std::int32_t x, std::int32_t y) const;

		// Original function: FUN_0040da6a
		std::uint32_t Draw_GetPixel_Long(std::int32_t x, std::int32_t y) const;

		// Original function: FUN_0040ee96
		std::uint16_t GetHighColorWord(std::uint8_t colorIndex) const;

		// Original function: FUN_0040eec8
		std::uint32_t GetTrueColorWord(std::uint8_t colorIndex) const;

		// Original function: FUN_0040c9a8
		void Fill(std::uint8_t colorIndex);

		// Original function: FUN_0040ca04
		void Fill(std::uint16_t color);

		// Original function: FUN_0040ca2a
		void Fill(std::uint32_t color);

		// Original function: FUN_0040ca50
		void Fill(const SColorRGB& color);

		// Original function: FUN_0040daa7
		void Draw_Box(SRectangle rectangle, std::uint8_t colorIndex);

		// Original function: FUN_0040db69
		void Draw_Box(SRectangle rectangle, std::uint16_t color);

		// Original function: FUN_0040dbc6
		void Draw_Box(SRectangle rectangle, std::uint32_t color);

		// Original function: FUN_0040dffe
		void Draw_VerticalLine(std::int32_t x, std::uint32_t y, std::int32_t length, std::uint8_t colorIndex);

		// Original function: FUN_0040e037
		void Draw_VerticalLine(std::int32_t x, std::uint32_t y, std::int32_t length, std::uint16_t color);

		// Original function: FUN_0040e070
		void Draw_VerticalLine(std::int32_t x, std::uint32_t y, std::int32_t length, std::uint32_t color);

		// Original function: FUN_0040e0a9
		void Draw_VerticalLine(std::int32_t x, std::uint32_t y, std::int32_t length, const SColorRGB& color);

		// Original function: LAB_0040d00a
		std::uint32_t Storable_GetId() override;

		// Original function: LAB_0040c775
		std::uint32_t Storable_GetVersion() override;

		// Original function: FUN_0040c734
		void Storable_SaveData(void* file) override;

		// Original function: FUN_0040c6bf
		~CBitmap() override;

		// Original function: FUN_004e5e48
		static void operator delete(void* allocation) noexcept;

	private:
		// Original function: FUN_0040c931
		void l_InitObject();

		// Original function: FUN_0040c953
		void l_SetRawMemoryPtr();

		// Original function: FUN_0040c778
		void l_ConstructNonVirtual(const SRectangle& rectangle, std::int32_t depth);

		// Original function: FUN_0040c836
		void l_ConstructVirtual(CBitmap* sourceBitmap, const SRectangle& rectangle);

		std::int32_t mDepth;                  // +0x04
		SRectangle mRectangle;                // +0x08
		std::uint32_t mRightBits;             // +0x18
		std::uint32_t mBottomBits;            // +0x1C
		void* mMemory;                        // +0x20
		std::uint8_t* mRawMemory;             // +0x24
		std::uint16_t* mHighColorMemory;      // +0x28
		std::uint32_t* mTrueColorMemory;      // +0x2C
		std::uint32_t mRowPixelCount;         // +0x30
		std::uint32_t mBytesPerPixel;         // +0x34
		std::uint32_t mRowByteCount;          // +0x38
		std::uint8_t mIsVirtual;              // +0x3C
		std::byte mPadding3D[3];              // +0x3D
		std::int32_t mSourceX;                // +0x40
		std::int32_t mSourceY;                // +0x44
		CBitmap* mSourceBitmap;               // +0x48
		void* mPalette;                       // +0x4C
		std::byte mUnknown50[0x04];           // +0x50
	};

	static_assert(sizeof(void*) == 4, "This reconstruction requires an x86 build");
	static_assert(sizeof(CBitmap) == 0x54, "CBitmap must be 84 bytes");
}