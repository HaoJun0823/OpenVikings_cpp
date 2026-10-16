#pragma once

#include <cstddef>
#include <cstdint>

namespace NXBasics
{
	class CBitmap; class CPalette;
}

namespace NToolGouraudShader
{
	struct SVertex
	{
		std::int32_t x, y, u, v, brightness, red, green, blue;
	};

	class CGouraudShader
	{
	public:
		NXBasics::CBitmap* bitmap;
		const std::uint8_t* texture;
		const std::uint8_t* alphaTexture;
		std::uint32_t flags;
		SVertex vertices[3];
		NXBasics::CPalette* palette;
		std::uint32_t unresolved74;
		std::int32_t firstY, height, skipRows, clipLeft, clipRight, clipTop, clipBottom;

		CGouraudShader();
		void XShade();
		void XShade_VertexColor();
		void XShade_Alpha();
		void XShade_VertexColor_Alpha();
		void l_SetupAndClip();
		void XShade_32Bit();
		void XShade_VertexColor_32Bit();
		void XShade_Alpha_32Bit();
		void XShade_VertexColor_Alpha_32Bit();
		void XShade_MMX();
		void XShade_MMX_32Bit();
		void XShade_Alpha_MMX();
		void XShade_Alpha_MMX_32Bit();
	};

	static_assert(sizeof(SVertex) == 32);
	static_assert(sizeof(CGouraudShader) == 148);
	static_assert(offsetof(CGouraudShader, vertices) == 0x10);
	static_assert(offsetof(CGouraudShader, palette) == 0x70);
	static_assert(offsetof(CGouraudShader, skipRows) == 0x80);
}
