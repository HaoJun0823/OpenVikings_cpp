#include "XBitmap.h"
#include "XBitmapTools.h"
#include "XPixelFormatConverter.h"
#include "XPaletteAccess.h"

#include <cstring>
#include <new>

// Original allocation-release wrapper.
extern void FUN_004e5e48(void* allocation);

namespace NXBasics
{
	// Original function: FUN_0040c624
	CBitmap::CBitmap(std::int32_t width, std::int32_t height, std::int32_t depth) : mRectangle(0, 0, 0, 0)
	{
		l_InitObject();
		SRectangle rectangle(0, 0, width, height);
		l_ConstructNonVirtual(rectangle, depth);
	}

	// Original function: FUN_0040c65f
	CBitmap::CBitmap(CBitmap* sourceBitmap, const SRectangle& rectangle) : mRectangle(0, 0, 0, 0)
	{
		l_InitObject();
		l_ConstructVirtual(sourceBitmap, rectangle);
	}

	// Original function: FUN_0040c953
	void CBitmap::l_SetRawMemoryPtr()
	{
		const auto* memoryBytes = static_cast<const std::byte*>(mMemory);

		if (mDepth == 1)
		{
			std::memcpy(&mRawMemory, memoryBytes + 0x08, sizeof(mRawMemory));

			std::uint16_t* highColorMemory;
			std::memcpy(&highColorMemory, memoryBytes + 0x08, sizeof(highColorMemory));

			mTrueColorMemory = nullptr;
			mHighColorMemory = highColorMemory;
			mBytesPerPixel = 2;
		}
		else if (mDepth == 2)
		{
			std::memcpy(&mRawMemory, memoryBytes + 0x08, sizeof(mRawMemory));

			std::uint32_t* trueColorMemory;
			std::memcpy(&trueColorMemory, memoryBytes + 0x08, sizeof(trueColorMemory));

			mHighColorMemory = nullptr;
			mTrueColorMemory = trueColorMemory;
			mBytesPerPixel = 4;
		}
		else
		{
			std::uint8_t* rawMemory;
			std::memcpy(&rawMemory, memoryBytes + 0x08, sizeof(rawMemory));

			mHighColorMemory = nullptr;
			mTrueColorMemory = nullptr;
			mRawMemory = rawMemory;
			mBytesPerPixel = 1;
		}
	}

	// Original function: FUN_0040c931
	void CBitmap::l_InitObject()
	{
		std::memset(reinterpret_cast<std::byte*>(this) + 0x04, 0, 0x50);
		mDepth = 0;
		mRectangle.Init();
	}

	// Original function: FUN_0040c778
	void CBitmap::l_ConstructNonVirtual(const SRectangle& rectangle, std::int32_t depth)
	{
		const std::uint32_t pixelCount = static_cast<std::uint32_t>(rectangle.width) * static_cast<std::uint32_t>(rectangle.height);

		std::uint32_t allocationSize;

		if (depth == 1)
		{
			mDepth = 1;
			allocationSize = pixelCount * 2u;
		}
		else if (depth == 2)
		{
			mDepth = 2;
			allocationSize = pixelCount * 4u;
		}
		else
		{
			mDepth = 0;
			allocationSize = pixelCount;
		}

		void* memoryStorage = ::operator new(0x0C, std::nothrow);
		void* memory = nullptr;

		if (memoryStorage != nullptr)
		{
			memory = FUN_0042f805(memoryStorage, allocationSize);
		}

		mMemory = memory;

		l_SetRawMemoryPtr();

		const std::uint32_t rowPixelCount = static_cast<std::uint32_t>(rectangle.width);

		mRowByteCount = mBytesPerPixel * rowPixelCount;
		mRowPixelCount = rowPixelCount;

		mRectangle.SetVariables(0, 0, rectangle.width, rectangle.height);

		mRightBits = static_cast<std::uint32_t>(mRectangle.x) + static_cast<std::uint32_t>(mRectangle.width) - 1u;
		mBottomBits = static_cast<std::uint32_t>(mRectangle.y) + static_cast<std::uint32_t>(mRectangle.height) - 1u;

		mIsVirtual = 0;
		mSourceX = 0;
		mSourceY = 0;
	}

	// Original function: FUN_0040c836
	void CBitmap::l_ConstructVirtual(CBitmap* sourceBitmap, const SRectangle& rectangle)
	{
		mIsVirtual = 1;
		mMemory = nullptr;
		mRawMemory = nullptr;
		mHighColorMemory = nullptr;
		mTrueColorMemory = nullptr;

		mDepth = sourceBitmap->mDepth;

		SRectangle clippedRectangle(rectangle);

		if (sourceBitmap->mRawMemory == nullptr)
		{
			return;
		}

		if (!clippedRectangle.IsTouching(sourceBitmap->mRectangle))
		{
			return;
		}

		clippedRectangle.CutInside(sourceBitmap->mRectangle);

		const std::int32_t sourceY = clippedRectangle.y;
		const std::uint32_t pixelOffset = sourceBitmap->mRowPixelCount * static_cast<std::uint32_t>(sourceY) + static_cast<std::uint32_t>(clippedRectangle.x);

		// Preserve the original 32-bit address arithmetic.
		if (mDepth == 1)
		{
			const std::uint32_t byteOffset = pixelOffset * 2u;

			mRawMemory = reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uintptr_t>(sourceBitmap->mRawMemory) + byteOffset);
			mHighColorMemory = reinterpret_cast<std::uint16_t*>(reinterpret_cast<std::uintptr_t>(sourceBitmap->mHighColorMemory) + byteOffset);
			mTrueColorMemory = nullptr;
		}
		else if (mDepth == 2)
		{
			const std::uint32_t byteOffset = pixelOffset * 4u;

			mRawMemory = reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uintptr_t>(sourceBitmap->mRawMemory) + byteOffset);
			mTrueColorMemory = reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(sourceBitmap->mTrueColorMemory) + byteOffset);
			mHighColorMemory = nullptr;
		}
		else
		{
			mRawMemory = reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uintptr_t>(sourceBitmap->mRawMemory) + pixelOffset);
			mHighColorMemory = nullptr;
			mTrueColorMemory = nullptr;
		}

		mSourceX = clippedRectangle.x;
		mSourceY = sourceY;
		mSourceBitmap = sourceBitmap;

		mRectangle.SetVariables(0, 0, clippedRectangle.width, clippedRectangle.height);

		mRightBits = static_cast<std::uint32_t>(mRectangle.x) + static_cast<std::uint32_t>(mRectangle.width) - 1u;
		mBottomBits = static_cast<std::uint32_t>(mRectangle.y) + static_cast<std::uint32_t>(mRectangle.height) - 1u;

		mRowPixelCount = sourceBitmap->mRowPixelCount;
		mBytesPerPixel = sourceBitmap->mBytesPerPixel;
		mRowByteCount = sourceBitmap->mRowByteCount;
	}

	// Original function: FUN_0040ee96
	std::uint16_t CBitmap::GetHighColorWord(std::uint8_t colorIndex) const
	{
		const std::uint32_t paletteIndex = colorIndex;
		void* palette = mPalette;

		if (palette == nullptr)
		{
			palette = DAT_005106dc;
		}

		return Palette_GetHighColorWord(palette, paletteIndex);
	}

	// Original function: FUN_0040eec8
	std::uint32_t CBitmap::GetTrueColorWord(std::uint8_t colorIndex) const
	{
		const std::uint32_t paletteIndex = colorIndex;
		void* palette = mPalette;

		if (palette == nullptr)
		{
			palette = DAT_005106dc;
		}

		return Palette_GetTrueColorWord(palette, paletteIndex);
	}

	// Original function: FUN_0040c9a8
	void CBitmap::Fill(std::uint8_t colorIndex)
	{
		std::uint8_t* pixels = mRawMemory;

		if (pixels == nullptr)
		{
			return;
		}

		switch (mDepth)
		{
		case 0:
			XB_Tool_Byte_FillBlock(pixels, static_cast<std::uint32_t>(mRectangle.width), mRectangle.height, mRowPixelCount, colorIndex);
			break;

		case 1:
		{
			const std::uint16_t packedColor = GetHighColorWord(colorIndex);
			Fill(packedColor);
			break;
		}

		case 2:
		{
			const std::uint32_t packedColor = GetTrueColorWord(colorIndex);
			Fill(packedColor);
			break;
		}
		}
	}

	// Original function: FUN_0040ca04
	void CBitmap::Fill(std::uint16_t color)
	{
		std::uint16_t* pixels = mHighColorMemory;

		if (pixels == nullptr || mDepth != 1)
		{
			return;
		}

		XB_Tool_Word_FillBlock(pixels, mRectangle.width, mRectangle.height, mRowPixelCount, color);
	}

	// Original function: FUN_0040ca2a
	void CBitmap::Fill(std::uint32_t color)
	{
		std::uint32_t* pixels = mTrueColorMemory;

		if (pixels == nullptr || mDepth != 2)
		{
			return;
		}

		XB_Tool_Long_FillBlock(pixels, mRectangle.width, mRectangle.height, mRowPixelCount, color);
	}

	// Original function: FUN_0040ca50
	void CBitmap::Fill(const SColorRGB& color)
	{
		switch (mDepth)
		{
		case 0:
		{
			void* palette = mPalette;

			if (palette == nullptr)
			{
				palette = DAT_005106dc;
			}

			const std::uint8_t colorIndex = Palette_FindMatchingColor(palette, color);
			Fill(colorIndex);
			break;
		}

		case 1:
		{
			const std::uint16_t packedColor = g16BitPixelConverter->PackColor(color);
			Fill(packedColor);
			break;
		}

		case 2:
		{
			const std::uint32_t packedColor = g32BitPixelConverter->PackColor(color);
			Fill(packedColor);
			break;
		}
		}
	}

	// Original function: FUN_0040daa7
	void CBitmap::Draw_Box(SRectangle rectangle, std::uint8_t colorIndex)
	{
		if (mRawMemory == nullptr)
		{
			return;
		}

		rectangle.Validate();

		if (!rectangle.IsTouching(mRectangle))
		{
			return;
		}

		rectangle.CutInside(mRectangle);

		switch (mDepth)
		{
		case 0:
		{
			const std::uint32_t rowPixelCount = mRowPixelCount;
			const std::uint32_t pixelOffset = rowPixelCount * static_cast<std::uint32_t>(rectangle.y) + static_cast<std::uint32_t>(rectangle.x);

			auto* pixels = reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uintptr_t>(mRawMemory) + pixelOffset);

			XB_Tool_Byte_FillBlock(pixels, static_cast<std::uint32_t>(rectangle.width), rectangle.height, rowPixelCount, colorIndex);
			break;
		}

		case 1:
		{
			const std::uint32_t pixelOffset = mRowPixelCount * static_cast<std::uint32_t>(rectangle.y) + static_cast<std::uint32_t>(rectangle.x);
			const std::uint32_t byteOffset = pixelOffset * 2u;

			auto* pixels = reinterpret_cast<std::uint16_t*>(reinterpret_cast<std::uintptr_t>(mHighColorMemory) + byteOffset);

			const std::uint16_t packedColor = GetHighColorWord(colorIndex);

			XB_Tool_Word_FillBlock(pixels, rectangle.width, rectangle.height, mRowPixelCount, packedColor);
			break;
		}

		case 2:
		{
			const std::uint32_t pixelOffset = mRowPixelCount * static_cast<std::uint32_t>(rectangle.y) + static_cast<std::uint32_t>(rectangle.x);
			const std::uint32_t byteOffset = pixelOffset * 4u;

			auto* pixels = reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(mTrueColorMemory) + byteOffset);

			const std::uint32_t packedColor = GetTrueColorWord(colorIndex);

			XB_Tool_Long_FillBlock(pixels, rectangle.width, rectangle.height, mRowPixelCount, packedColor);
			break;
		}
		}
	}

	// Original function: FUN_0040db69
	void CBitmap::Draw_Box(SRectangle rectangle, std::uint16_t color)
	{
		rectangle.Validate();

		if (mRawMemory == nullptr || mDepth != 1)
		{
			return;
		}

		if (!rectangle.IsTouching(mRectangle))
		{
			return;
		}

		rectangle.CutInside(mRectangle);

		const std::uint32_t rowPixelCount = mRowPixelCount;
		const std::uint32_t pixelOffset = rowPixelCount * static_cast<std::uint32_t>(rectangle.y) + static_cast<std::uint32_t>(rectangle.x);
		const std::uint32_t byteOffset = pixelOffset * 2u;

		auto* pixels = reinterpret_cast<std::uint16_t*>(reinterpret_cast<std::uintptr_t>(mHighColorMemory) + byteOffset);

		XB_Tool_Word_FillBlock(pixels, rectangle.width, rectangle.height, rowPixelCount, color);
	}

	// Original function: FUN_0040dbc6
	void CBitmap::Draw_Box(SRectangle rectangle, std::uint32_t color)
	{
		rectangle.Validate();

		if (mRawMemory == nullptr || mDepth != 2)
		{
			return;
		}

		if (!rectangle.IsTouching(mRectangle))
		{
			return;
		}

		rectangle.CutInside(mRectangle);

		const std::uint32_t rowPixelCount = mRowPixelCount;
		const std::uint32_t pixelOffset = rowPixelCount * static_cast<std::uint32_t>(rectangle.y) + static_cast<std::uint32_t>(rectangle.x);
		const std::uint32_t byteOffset = pixelOffset * 4u;

		auto* pixels = reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(mTrueColorMemory) + byteOffset);

		XB_Tool_Long_FillBlock(pixels, rectangle.width, rectangle.height, rowPixelCount, color);
	}

	// Original function: FUN_0040dffe
	void CBitmap::Draw_VerticalLine(std::int32_t x, std::uint32_t y, std::int32_t length, std::uint8_t colorIndex)
	{
		// Preserve the coordinate's original 32-bit representation.
		std::int32_t rectangleY;
		std::memcpy(&rectangleY, &y, sizeof(rectangleY));

		SRectangle rectangle(x, rectangleY, 1, length);

		Draw_Box(rectangle, colorIndex);
	}

	// Original function: FUN_0040e037
	void CBitmap::Draw_VerticalLine(std::int32_t x, std::uint32_t y, std::int32_t length, std::uint16_t color)
	{
		// Preserve the coordinate's original 32-bit representation.
		std::int32_t rectangleY;
		std::memcpy(&rectangleY, &y, sizeof(rectangleY));

		SRectangle rectangle(x, rectangleY, 1, length);

		Draw_Box(rectangle, color);
	}

	// Original function: FUN_0040e070
	void CBitmap::Draw_VerticalLine(std::int32_t x, std::uint32_t y, std::int32_t length, std::uint32_t color)
	{
		// Preserve the coordinate's original 32-bit representation.
		std::int32_t rectangleY;
		std::memcpy(&rectangleY, &y, sizeof(rectangleY));

		SRectangle rectangle(x, rectangleY, 1, length);

		Draw_Box(rectangle, color);
	}

	// Original function: FUN_0040e0a9
	void CBitmap::Draw_VerticalLine(std::int32_t x, std::uint32_t y, std::int32_t length, const SColorRGB& color)
	{
		switch (mDepth)
		{
		case 0:
		{
			void* palette = mPalette;

			if (palette == nullptr)
			{
				palette = DAT_005106dc;
			}

			const std::uint8_t colorIndex = Palette_FindMatchingColor(palette, color);
			Draw_VerticalLine(x, y, length, colorIndex);
			break;
		}

		case 1:
		{
			const std::uint16_t packedColor = g16BitPixelConverter->PackColor(color);
			Draw_VerticalLine(x, y, length, packedColor);
			break;
		}

		case 2:
		{
			const std::uint32_t packedColor = g32BitPixelConverter->PackColor(color);
			Draw_VerticalLine(x, y, length, packedColor);
			break;
		}
		}
	}

	// Original function: LAB_0040d00a
	std::uint32_t CBitmap::Storable_GetId()
	{
		return 0x3F3;
	}

	// Original function: LAB_0040c775
	std::uint32_t CBitmap::Storable_GetVersion()
	{
		return 0;
	}

	// Original function: FUN_0040c734
	void CBitmap::Storable_SaveData(void* file)
	{
		if (mIsVirtual != 0)
		{
			FUN_00406935(DAT_004ff994);
		}

		FUN_004061bc(file, &mDepth, 0x04);
		FUN_004061bc(file, &mRectangle, 0x10);
		FUN_0041067e(mMemory, file);
	}

	// Original function: FUN_0040c6bf
	CBitmap::~CBitmap()
	{
		void* memory = mMemory;

		if (memory != nullptr)
		{
			// Invoke the memory object's deleting destructor with flag 1.
			using DeletingDestructor = void* (__thiscall*)(void*, std::uint32_t);

			auto* vtable = *reinterpret_cast<DeletingDestructor**>(memory);
			vtable[3](memory, 1);

			mMemory = nullptr;
		}
	}

	// Original function: FUN_004e5e48
	void CBitmap::operator delete(void* allocation) noexcept
	{
		FUN_004e5e48(allocation);
	}
}