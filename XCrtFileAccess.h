#pragma once

#include <Windows.h>

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace NXBasics
{
	struct RuntimeErrorMapping
	{
		std::uint32_t osError;
		std::uint32_t crtError;
	};

	// Original data: 0050c250 through 0050c3b7.
	inline constexpr RuntimeErrorMapping gRuntimeErrorMappings[] =
	{
		{ 0x0001u, 0x16u },
		{ 0x0002u, 0x02u },
		{ 0x0003u, 0x02u },
		{ 0x0004u, 0x18u },
		{ 0x0005u, 0x0Du },
		{ 0x0006u, 0x09u },
		{ 0x0007u, 0x0Cu },
		{ 0x0008u, 0x0Cu },
		{ 0x0009u, 0x0Cu },
		{ 0x000Au, 0x07u },
		{ 0x000Bu, 0x08u },
		{ 0x000Cu, 0x16u },
		{ 0x000Du, 0x16u },
		{ 0x000Fu, 0x02u },
		{ 0x0010u, 0x0Du },
		{ 0x0011u, 0x12u },
		{ 0x0012u, 0x02u },
		{ 0x0021u, 0x0Du },
		{ 0x0035u, 0x02u },
		{ 0x0041u, 0x0Du },
		{ 0x0043u, 0x02u },
		{ 0x0050u, 0x11u },
		{ 0x0052u, 0x0Du },
		{ 0x0053u, 0x0Du },
		{ 0x0057u, 0x16u },
		{ 0x0059u, 0x0Bu },
		{ 0x006Cu, 0x0Du },
		{ 0x006Du, 0x20u },
		{ 0x0070u, 0x1Cu },
		{ 0x0072u, 0x09u },
		{ 0x0006u, 0x16u }, // Preserve the duplicate and original order.
		{ 0x0080u, 0x0Au },
		{ 0x0081u, 0x0Au },
		{ 0x0082u, 0x09u },
		{ 0x0083u, 0x16u },
		{ 0x0084u, 0x0Du },
		{ 0x0091u, 0x29u },
		{ 0x009Eu, 0x0Du },
		{ 0x00A1u, 0x02u },
		{ 0x00A4u, 0x0Bu },
		{ 0x00A7u, 0x0Du },
		{ 0x00B7u, 0x11u },
		{ 0x00CEu, 0x02u },
		{ 0x00D7u, 0x0Bu },
		{ 0x0718u, 0x0Cu }
	};

	struct RuntimeFormatStream
	{
		char* position;                 // +0x00
		std::uint32_t remainingBytes;   // +0x04
		char* buffer;                   // +0x08
		std::uint32_t flags;            // +0x0C
		std::uint32_t descriptor;       // +0x10
		std::byte unknown14[4];         // +0x14
		std::uint32_t bufferSize;       // +0x18
		std::byte unknown1C[4];         // +0x1C
	};

	static_assert(sizeof(void*) == 4, "RuntimeFormatStream requires an x86 build");
	static_assert(sizeof(RuntimeFormatStream) == 0x20);
	static_assert(offsetof(RuntimeFormatStream, position) == 0x00);
	static_assert(offsetof(RuntimeFormatStream, remainingBytes) == 0x04);
	static_assert(offsetof(RuntimeFormatStream, buffer) == 0x08);
	static_assert(offsetof(RuntimeFormatStream, flags) == 0x0C);
	static_assert(offsetof(RuntimeFormatStream, descriptor) == 0x10);
	static_assert(offsetof(RuntimeFormatStream, bufferSize) == 0x18);

	static_assert(sizeof(gRuntimeErrorMappings) / sizeof(gRuntimeErrorMappings[0]) == 45, "The original error table contains 45 entries");

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
		0x08, 0x00, 0x00, 0x00
	};

	static_assert(sizeof(gRuntimeFormatTable) == 0x5C);

	// Original function: FUN_004e6a84
	inline std::int32_t Runtime_Read(std::uint32_t descriptor, void* destination, std::uint32_t byteCount)
	{
		if (descriptor < static_cast<std::uint32_t>(uNumber_0056b700))
		{
			const auto* descriptorBlocks = reinterpret_cast<const std::uint8_t*>(&DAT_0056b600);

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

	// Original function: FUN_004e6d27
	inline std::uint32_t Runtime_SeekNoLock(std::uint32_t descriptor, std::int32_t offset, std::uint32_t origin);

	// Original function: FUN_004e6cc2
	inline std::uint32_t Runtime_Seek(std::uint32_t descriptor, std::int32_t offset, std::uint32_t origin);

	// Original function: FUN_004e6ae9
	inline std::int32_t Runtime_ReadNoLock(std::uint32_t descriptor, void* destination, std::uint32_t byteCount);

	// Original function: FUN_004e6d27
	inline std::uint32_t Runtime_SeekNoLock(std::uint32_t descriptor, std::int32_t offset, std::uint32_t origin)
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

		const auto* descriptorBlocks = reinterpret_cast<const std::uint8_t*>(&DAT_0056b600);

		std::uint8_t* block;
		std::memcpy(&block, descriptorBlocks + (descriptor >> 5) * 4u, sizeof(block));

		const std::uint32_t entryOffset = (descriptor & 0x1Fu) * 0x24u;
		block[entryOffset + 0x04] &= 0xFD;

		return position;
	}

	// Original function: FUN_004e6cc2
	inline std::uint32_t Runtime_Seek(std::uint32_t descriptor, std::int32_t offset, std::uint32_t origin)
	{
		if (descriptor < static_cast<std::uint32_t>(uNumber_0056b700))
		{
			const auto* descriptorBlocks = reinterpret_cast<const std::uint8_t*>(&DAT_0056b600);

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

	// Original function: FUN_004e6ae9
	inline std::int32_t Runtime_ReadNoLock(std::uint32_t descriptor, void* destination, std::uint32_t byteCount)
	{
		if (byteCount == 0)
		{
			return 0;
		}

		const auto* descriptorBlocks = reinterpret_cast<const std::uint8_t*>(&DAT_0056b600);
		const std::uint32_t blockOffset = (descriptor >> 5) * 4u;
		const std::uint32_t entryOffset = (descriptor & 0x1Fu) * 0x24u;

		const auto getEntry = [descriptorBlocks, blockOffset, entryOffset]() -> std::uint8_t*
			{
				std::uint8_t* block;
				std::memcpy(&block, descriptorBlocks + blockOffset, sizeof(block));

				return block + entryOffset;
			};

		const auto getHandle = [&getEntry]() -> HANDLE
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

	// Original function: FUN_004e6d9a
	inline std::uint32_t Runtime_GetFileSize(std::uint32_t descriptor)
	{
		if (descriptor < static_cast<std::uint32_t>(uNumber_0056b700))
		{
			const auto* descriptorBlocks = reinterpret_cast<const std::uint8_t*>(&DAT_0056b600);

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

	// Original function: FUN_004eb99c
	inline void Runtime_FloatInsertDecimalPoint(char* text)
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
		std::memcpy(&decimalSeparator, &DAT_0050c608, sizeof(decimalSeparator));

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

	// Original function: LAB_004eb9f6
	inline void Runtime_FloatTrimTrailingZeros(char* text)
	{
		char decimalSeparator;
		std::memcpy(&decimalSeparator, &DAT_0050c608, sizeof(decimalSeparator));

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
	inline std::int32_t Runtime_FloatAtLeastReference(const double* value)
	{
		// Original reference value at DAT_004f3348: +0.0.
		return *value >= 0.0 ? 1 : 0;
	}

	struct RuntimeFloatConversionState
	{
		std::int32_t sign;               // +0x00
		std::int32_t decimalPosition;    // +0x04
		std::uint32_t conversionResult;  // +0x08
		char* digits;                    // +0x0C
	};

	static_assert(sizeof(RuntimeFloatConversionState) == 0x10);
	static_assert(offsetof(RuntimeFloatConversionState, sign) == 0x00);
	static_assert(offsetof(RuntimeFloatConversionState, decimalPosition) == 0x04);
	static_assert(offsetof(RuntimeFloatConversionState, conversionResult) == 0x08);
	static_assert(offsetof(RuntimeFloatConversionState, digits) == 0x0C);

	struct RuntimeFloatIntermediate
	{
		std::uint32_t word0;  // +0x00
		std::uint32_t word1;  // +0x04
		std::uint16_t word2;  // +0x08
	};

	static_assert(sizeof(RuntimeFloatIntermediate) == 0x0C);
	static_assert(offsetof(RuntimeFloatIntermediate, word2) == 0x08);

	struct RuntimeFloatDecimalResult
	{
		std::int16_t decimalPosition;  // +0x00
		std::int8_t sign;              // +0x02
		std::uint8_t digitCount;       // +0x03
		char digits[24];               // +0x04
	};

	static_assert(sizeof(RuntimeFloatDecimalResult) == 0x1C);
	static_assert(offsetof(RuntimeFloatDecimalResult, sign) == 0x02);
	static_assert(offsetof(RuntimeFloatDecimalResult, digitCount) == 0x03);
	static_assert(offsetof(RuntimeFloatDecimalResult, digits) == 0x04);

	// Original function: FUN_004f0b60
	inline void Runtime_DoubleToIntermediate(RuntimeFloatIntermediate* destination, const double* source)
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

	// Original function: FUN_004f20f9
	inline void Runtime_ShiftLeft96(std::uint32_t* value)
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
	inline void Runtime_ShiftRight96(std::uint32_t* value)
	{
		const std::uint32_t high = value[2];
		const std::uint32_t middle = value[1];

		value[1] = (middle >> 1) | (high << 31);

		const std::uint32_t low = value[0];

		value[2] = high >> 1;
		value[0] = (low >> 1) | (middle << 31);
	}

	// Original function: ___add_12 at 004f209b
	inline void Runtime_Add96(std::uint32_t* destination, const std::uint32_t* source)
	{
		if (FUN_004f207a(destination[0], source[0], &destination[0]) != 0)
		{
			if (FUN_004f207a(destination[1], 1u, &destination[1]) != 0)
			{
				++destination[2];
			}
		}

		if (FUN_004f207a(destination[1], source[1], &destination[1]) != 0)
		{
			++destination[2];
		}

		FUN_004f207a(destination[2], source[2], &destination[2]);
	}

	// Original function: FUN_004f221b
	inline std::uint32_t Runtime_IntermediateToDecimal(RuntimeFloatIntermediate value, std::int32_t requestedDigits, std::uint32_t options, RuntimeFloatDecimalResult* result)
	{
		const auto asSigned = [](std::uint32_t bits) -> std::int32_t
			{
				std::int32_t value;
				std::memcpy(&value, &bits, sizeof(value));
				return value;
			};

		const auto writeZero = [result]() -> std::uint32_t
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
				FUN_004e5c20(result->digits, "1#SNAN");
				result->digitCount = 6;
			}
			else if (sign != 0 && mantissaHigh == 0xC0000000u && mantissaLow == 0)
			{
				FUN_004e5c20(result->digits, "1#IND");
				result->digitCount = 5;
			}
			else if (isInfinity)
			{
				FUN_004e5c20(result->digits, "1#INF");
				result->digitCount = 5;
			}
			else
			{
				FUN_004e5c20(result->digits, "1#QNAN");
				result->digitCount = 6;
			}

			return 0;
		}

		// Original twelve-byte constant at local_20.
		std::uint32_t scaleFactor[3] =
		{
			0xCCCCCCCCu,
			0xCCCCCCCCu,
			0x3FFBCCCCu
		};

		// The working representation has a two-byte low extension.
		std::uint32_t working[3] =
		{
			mantissaLow << 16,
			(mantissaLow >> 16) | (mantissaHigh << 16),
			(mantissaHigh >> 16) | (exponent << 16)
		};

		const std::uint32_t estimateBits = exponent * 0x4D10u + ((exponent >> 8) + (mantissaHigh >> 24) * 2u) * 0x4Du + 0xECBced0Cu;

		std::int32_t decimalEstimate = asSigned(estimateBits) >> 16;

		FUN_004f26ce(working, -decimalEstimate, 1);

		if ((working[2] >> 16) >= 0x3FFFu)
		{
			++decimalEstimate;
			FUN_004f24ae(working, scaleFactor);
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

	// Original function: FUN_004f0b04
	inline RuntimeFloatConversionState* Runtime_PrepareFloatConversion(double value, RuntimeFloatConversionState* state, char* workspace)
	{
		RuntimeFloatIntermediate intermediate;
		RuntimeFloatDecimalResult decimal;

		Runtime_DoubleToIntermediate(&intermediate, &value);

		// The intermediate value occupies twelve stack bytes by value.
		state->conversionResult = Runtime_IntermediateToDecimal(intermediate, 17, 0, &decimal);

		state->sign = static_cast<std::int32_t>(decimal.sign);
		state->decimalPosition = static_cast<std::int32_t>(decimal.decimalPosition);

		FUN_004e5c20(workspace, decimal.digits);

		state->digits = workspace;

		return state;
	}

	// Original function: FUN_004f0a8d
	inline void Runtime_GenerateFloatDigits(char* buffer, std::int32_t digitCount, RuntimeFloatConversionState* state)
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
			FUN_004e71f0(buffer, buffer + 1, byteCount);
		}
	}

	// Original function: FUN_004eba9a
	inline char* Runtime_ConvertFloatScientific(const double* value, char* buffer, std::int32_t precision, std::int32_t uppercase)
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
		FUN_004ebafb(buffer, precision, uppercase, &state, 0);

		return buffer;
	}

	// Original function: FUN_004ebbbd
	inline char* Runtime_ConvertFloatFixed(const double* value, char* buffer, std::int32_t precision)
	{
		RuntimeFloatConversionState state;
		char workspace[24];

		FUN_004f0b04(*value, &state, workspace);

		const std::uint32_t digitCountBits = static_cast<std::uint32_t>(state.decimalPosition) + static_cast<std::uint32_t>(precision);

		std::int32_t digitCount;
		std::memcpy(&digitCount, &digitCountBits, sizeof(digitCount));

		char* digits = buffer + (state.sign == '-' ? 1 : 0);

		Runtime_GenerateFloatDigits(digits, precision, &state);
		FUN_004ebc12(buffer, precision, &state, 0);

		return buffer;
	}

	// Original function: FUN_004ebcb9
	inline void Runtime_ConvertFloatGeneral(const double* value, char* buffer, std::int32_t precision, std::int32_t uppercase)
	{
		RuntimeFloatConversionState state;
		char workspace[24];

		FUN_004f0b04(*value, &state, workspace);

		const std::uint32_t initialExponentBits = static_cast<std::uint32_t>(state.decimalPosition) - std::uint32_t{ 1 };

		std::int32_t initialExponent;
		std::memcpy(&initialExponent, &initialExponentBits, sizeof(initialExponent));

		char* digits = buffer + (state.sign == '-' ? 1 : 0);

		Runtime_PrepareFloatConversion(digits, precision, &state);

		// The digit-generation step may change the decimal position.
		const std::uint32_t roundedExponentBits = static_cast<std::uint32_t>(state.decimalPosition) - std::uint32_t{ 1 };

		std::int32_t roundedExponent;
		std::memcpy(&roundedExponent, &roundedExponentBits, sizeof(roundedExponent));

		const bool exponentIncreased = initialExponent < roundedExponent;

		if (roundedExponent < -4 || roundedExponent >= precision)
		{
			FUN_004ebafb(buffer, precision, uppercase, &state, 1);
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

		FUN_004ebc12(buffer, precision, &state, 1);
	}

	// Original function: __cfltcvt at 004ebd4c
	inline void Runtime_ConvertFloat(const double* value, char* buffer, std::int32_t format, std::int32_t precision, std::int32_t uppercase)
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

	// Original function: FUN_004e6f13
	inline void __stdcall Runtime_InitializeFloatFunctions()
	{
		PTR_FUN_0050c784 = &Runtime_FloatTrimTrailingZeros;
		PTR_FUN_0050c780 = &Runtime_ConvertFloat;
		PTR_FUN_0050c788 = &__fassign;
		PTR_FUN_0050c78c = &Runtime_FloatInsertDecimalPoint;
		PTR_FUN_0050c790 = &Runtime_FloatAtLeastReference;
		PTR_FUN_0050c794 = &Runtime_ConvertFloat;
	}

	// Original function: FUN_004e7575
	inline void Runtime_MapOsError(std::uint32_t osError)
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

	// Original function: FUN_004e64a5
	inline std::int32_t Runtime_Write(std::uint32_t descriptor, const void* data, std::uint32_t byteCount);

	// Original function: FUN_004e6cc2
	inline std::uint32_t Runtime_Seek(std::uint32_t descriptor, std::int32_t offset, std::uint32_t origin);

	// Original function: FUN_004e9772
	inline std::uint32_t Runtime_FlushAndWriteCharacter(std::uint32_t character, RuntimeFormatStream* stream)
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

			if (static_cast<const void*>(stream) == static_cast<const void*>(&DAT_0050ece8) || static_cast<const void*>(stream) == static_cast<const void*>(&DAT_0050ed08))
			{
				if (FUN_004ef178(descriptor) != 0)
				{
					allocateBuffer = false;
				}
			}

			if (allocateBuffer)
			{
				FUN_004ef134(stream);
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
					descriptorRecord = reinterpret_cast<const std::uint8_t*>(&DAT_0050c610);
				}
				else
				{
					// Reproduce the arithmetic right shift used for the block index.
					std::int32_t signedDescriptor;
					std::memcpy(&signedDescriptor, &descriptor, sizeof(signedDescriptor));

					const std::int32_t blockIndex = signedDescriptor >> 5;
					const auto* descriptorBlocks = reinterpret_cast<const std::byte*>(&DAT_0056b600);

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

	// Original function: FUN_004e75fa
	inline std::int32_t Runtime_FormatString(char* destination, std::int32_t capacity, const char* format, va_list arguments)
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

	// Original function: FUN_004e78b9
	inline std::int32_t Runtime_ToLower(std::int32_t character)
	{
		if (DAT_00569b98 == 0)
		{
			if (character >= 0x41 && character <= 0x5A)
			{
				return character + 0x20;
			}

			return character;
		}

		auto* localeUsers = reinterpret_cast<volatile LONG*>(&DAT_0056b5f4);
		bool localeLocked = false;

		InterlockedIncrement(localeUsers);

		if (DAT_0056b5f0 != 0)
		{
			InterlockedDecrement(localeUsers);
			FUN_004ebdeb(0x13);
			localeLocked = true;
		}

		const std::uint32_t resultBits = FUN_004e7928(character);

		if (localeLocked)
		{
			FUN_004ebe4c(0x13);
		}
		else
		{
			InterlockedDecrement(localeUsers);
		}

		std::int32_t result;
		std::memcpy(&result, &resultBits, sizeof(result));

		return result;
	}

	// Original function: FUN_00410bdd
	// Original function: FUN_004e7cea
	inline bool Runtime_IsDigit(char character)
	{
		const std::uint8_t byteValue = static_cast<std::uint8_t>(character);
		const std::int32_t signedCharacter = byteValue < 0x80 ? static_cast<std::int32_t>(byteValue) : static_cast<std::int32_t>(byteValue) - 0x100;

		if (cbMultiByte_0050c604 > 1)
		{
			return FUN_004ea094(signedCharacter, 0x04) != 0;
		}

		const auto* classificationTable = reinterpret_cast<const std::uint8_t*>(PTR_DAT_0050c3f8);

		return (classificationTable[signedCharacter * 2] & 0x04) != 0;
	}

	// Original function: FUN_004eb1bb
	inline void Runtime_InitializeThreadData(void* threadData)
	{
		auto* threadBytes = static_cast<std::uint8_t*>(threadData);

		const void* initialTable = &DAT_0050c968;
		std::memcpy(threadBytes + 0x50, &initialTable, sizeof(initialTable));

		const std::uint32_t initialValue = 1;
		std::memcpy(threadBytes + 0x14, &initialValue, sizeof(initialValue));
	}

	// Original function: FUN_004eb1ce
	inline DWORD* Runtime_GetThreadData()
	{
		const DWORD savedLastError = ::GetLastError();
		void* threadData = ::TlsGetValue(dwTlsIndex_0050c6e4);

		if (threadData == nullptr)
		{
			threadData = FUN_004ef3ac(1, 0x74);

			if (threadData == nullptr || !::TlsSetValue(dwTlsIndex_0050c6e4, threadData))
			{
				// Fatal CRT error; the original performs no local cleanup.
				__amsg_exit(0x10);
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

	// Original function: FUN_004e75e8
	inline DWORD* Runtime_GetErrnoPointer()
	{
		auto* threadBytes = reinterpret_cast<std::uint8_t*>(Runtime_GetThreadData());
		return reinterpret_cast<DWORD*>(threadBytes + 0x08);
	}

	// Original function: FUN_004e75f1
	inline DWORD* Runtime_GetDosErrnoPointer()
	{
		auto* threadBytes = reinterpret_cast<std::uint8_t*>(Runtime_GetThreadData());
		return reinterpret_cast<DWORD*>(threadBytes + 0x0C);
	}

	// Original function: FUN_004e7575
	inline void Runtime_MapOsError(std::uint32_t osError)
	{
		*Runtime_GetDosErrnoPointer() = osError;

		const auto* errorTable = reinterpret_cast<const std::uint8_t*>(&DAT_0050c250);

		for (std::uint32_t index = 0; index < 45u; ++index)
		{
			const auto* entry = errorTable + index * 8u;

			std::uint32_t tableOsError;
			std::memcpy(&tableOsError, entry, sizeof(tableOsError));

			if (tableOsError == osError)
			{
				// Obtain the errno pointer before reading the mapped value.
				DWORD* errorPointer = Runtime_GetErrnoPointer();

				std::uint32_t mappedError;
				std::memcpy(&mappedError, entry + 4u, sizeof(mappedError));

				*errorPointer = mappedError;
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

	// Original function: FUN_004eb167
	inline std::int32_t Runtime_InitializeThreading()
	{
		Runtime_InitializeLocks();

		const DWORD tlsIndex = ::TlsAlloc();
		dwTlsIndex_0050c6e4 = tlsIndex;

		if (tlsIndex == TLS_OUT_OF_INDEXES)
		{
			return 0;
		}

		auto* threadData = FUN_004ef3ac(1, 0x74);

		if (threadData == nullptr)
		{
			return 0;
		}

		if (!::TlsSetValue(dwTlsIndex_0050c6e4, threadData))
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

	// Original function: FUN_004ebdc2
	inline void Runtime_InitializeLocks()
	{
		auto* slots = reinterpret_cast<CRITICAL_SECTION* volatile*>(&lpCriticalSection_0050c798);

		::InitializeCriticalSection(slots[0x11]);
		::InitializeCriticalSection(slots[0x0D]);
		::InitializeCriticalSection(slots[0x09]);
		::InitializeCriticalSection(slots[0x01]);
	}

	// Original function: FUN_004ebe4c
	inline void Runtime_Unlock(std::int32_t lockIndex)
	{
		auto* slots = reinterpret_cast<CRITICAL_SECTION* volatile*>(&lpCriticalSection_0050c798);

		::LeaveCriticalSection(slots[lockIndex]);
	}

	// Original function: FUN_004ebdeb
	inline void Runtime_Lock(std::int32_t lockIndex)
	{
		auto* slots = reinterpret_cast<CRITICAL_SECTION* volatile*>(&lpCriticalSection_0050c798);
		auto* slot = slots + lockIndex;

		if (*slot == nullptr)
		{
			// Keep the original CRT allocation and deallocation pair.
			auto* candidate = static_cast<CRITICAL_SECTION*>(_malloc(0x18));

			if (candidate == nullptr)
			{
				__amsg_exit(0x11);
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
				FUN_004e8513(reinterpret_cast<std::uint8_t*>(candidate));
			}

			Runtime_Unlock(0x11);
		}

		::EnterCriticalSection(*slot);
	}

	// Original function: FUN_004e7928
	inline std::uint32_t Runtime_ToLowerNoLock(std::uint32_t character)
	{
		std::int32_t signedCharacter;
		std::memcpy(&signedCharacter, &character, sizeof(signedCharacter));

		if (DAT_00569b98 == 0)
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

			if (cbMultiByte_0050c604 > 1)
			{
				isUpper = FUN_004ea094(signedCharacter, 1) != 0;
			}
			else
			{
				const auto* characterTypes = reinterpret_cast<const std::uint8_t*>(PTR_DAT_0050c3f8);

				// Preserve signed indexing, including the original EOF case.
				isUpper = (characterTypes[static_cast<std::ptrdiff_t>(signedCharacter) * 2] & 0x01u) != 0;
			}

			if (!isUpper)
			{
				return character;
			}
		}

		const auto* characterTypes = reinterpret_cast<const std::uint8_t*>(PTR_DAT_0050c3f8);
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

		const int mappedLength = FUN_004ec998(DAT_00569b98, 0x100u, source, sourceLength, destination, 3, 0, 1);

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

	// Original function: FUN_00410bca
	// Original function: FUN_004e7cbc
	inline bool Runtime_IsLetter(char character)
	{
		const std::uint8_t byteValue = static_cast<std::uint8_t>(character);
		const std::int32_t signedCharacter = byteValue < 0x80 ? static_cast<std::int32_t>(byteValue) : static_cast<std::int32_t>(byteValue) - 0x100;

		if (cbMultiByte_0050c604 > 1)
		{
			return FUN_004ea094(signedCharacter, 0x0103) != 0;
		}

		const auto* classificationTable = reinterpret_cast<const std::uint8_t*>(PTR_DAT_0050c3f8);

		std::uint16_t classification;
		std::memcpy(&classification, classificationTable + signedCharacter * 2, sizeof(classification));

		return (classification & 0x0103) != 0;
	}

	// Original function: FUN_004e7d70
	inline std::int32_t Runtime_CompareStringsIgnoreCase(const char* first, const char* second)
	{
		const auto* left = reinterpret_cast<const std::uint8_t*>(first);
		const auto* right = reinterpret_cast<const std::uint8_t*>(second);

		if (DAT_00569b98 == 0)
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

		auto* activeReaders = reinterpret_cast<volatile LONG*>(&DAT_0056b5f4);
		const auto* localeChangeState = reinterpret_cast<const volatile LONG*>(&DAT_0056b5f0);

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

	// Original function: FUN_004e9fcb
	inline void Runtime_FormatWriteCharacter(std::uint32_t character, RuntimeFormatStream* stream, std::int32_t* writtenCharacters)
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
	inline void Runtime_FormatWriteRepeated(std::uint32_t character, std::int32_t count, RuntimeFormatStream* stream, std::int32_t* writtenCharacters)
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
	inline void Runtime_FormatWriteText(const char* text, std::int32_t length, RuntimeFormatStream* stream, std::int32_t* writtenCharacters)
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

	static_assert(sizeof(va_list) == 4, "Argument access requires the MSVC x86 va_list layout");

	// Original function: FUN_004ea069
	inline std::uint32_t Runtime_FormatReadArgument32(va_list* arguments)
	{
		*arguments += 4;

		std::uint32_t value;
		std::memcpy(&value, *arguments - 4, sizeof(value));

		return value;
	}

	// Original function: FUN_004ea076
	inline std::uint64_t Runtime_FormatReadArgument64(va_list* arguments)
	{
		*arguments += 8;

		std::uint64_t value;
		std::memcpy(&value, *arguments - 8, sizeof(value));

		return value;
	}

	// Original function: FUN_004ea086
	inline std::uint32_t Runtime_FormatReadArgument16(va_list* arguments)
	{
		*arguments += 4;

		const std::uint32_t advancedAddress = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(*arguments));

		std::uint16_t value;
		std::memcpy(&value, *arguments - 4, sizeof(value));

		// MOV AX preserves the upper half of the advanced argument address.
		return (advancedAddress & UINT32_C(0xFFFF0000)) | static_cast<std::uint32_t>(value);
	}

	// Original function: FUN_004ef1fa
	inline std::int32_t Runtime_WideCharacterToMultibyteNoLock(char* destination, std::uint32_t characterBits)
	{
		if (destination == nullptr)
		{
			return 0;
		}

		const WCHAR character = static_cast<WCHAR>(characterBits);

		if (DAT_00569b98 == 0)
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

		const int result = WideCharToMultiByte(CodePage_00569ba8, 0x220, &character, 1, destination, cbMultiByte_0050c604, nullptr, &usedDefaultCharacter);

		if (result == 0 || usedDefaultCharacter != FALSE)
		{
			*Runtime_GetErrnoPointer() = 0x2A;
			return -1;
		}

		return result;
	}

	// Original function: FUN_004ef1a1
	inline std::int32_t Runtime_WideCharacterToMultibyte(char* destination, std::uint32_t characterBits)
	{
		auto* localeUsers = reinterpret_cast<volatile LONG*>(&DAT_0056b5f4);

		InterlockedIncrement(localeUsers);

		bool localeLocked = false;

		if (DAT_0056b5f0 != 0)
		{
			InterlockedDecrement(localeUsers);
			FUN_004ebdeb(0x13);
			localeLocked = true;
		}

		const std::int32_t result = Runtime_WideCharacterToMultibyteNoLock(destination, characterBits);

		if (localeLocked)
		{
			FUN_004ebe4c(0x13);
		}
		else
		{
			InterlockedDecrement(localeUsers);
		}

		return result;
	}

	// Original function: FUN_004e988a
	inline std::int32_t Runtime_FormatToStream(RuntimeFormatStream* stream, const char* format, va_list arguments)
	{
		const auto asSigned = [](std::uint32_t bits) -> std::int32_t
			{
				std::int32_t value;
				std::memcpy(&value, &bits, sizeof(value));
				return value;
			};

		const auto signedCharacter = [](std::uint8_t value) -> std::int32_t
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

		const auto writeLiteral = [&]()
			{
				wideText = false;

				const auto* classification = reinterpret_cast<const std::uint8_t*>(PTR_DAT_0050c3f8);

				if ((classification[static_cast<std::uint32_t>(character) * 2u + 1u] & 0x80u) != 0)
				{
					Runtime_FormatWriteCharacter(static_cast<std::uint32_t>(signedCharacter(character)), stream, &writtenCharacters);
					character = static_cast<std::uint8_t>(*formatPosition++);
				}

				FUN_004e9fcb(signedCharacter(character), stream, &writtenCharacters);
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
							text = reinterpret_cast<const char*>(PTR_DAT_0050c3f4);
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
							text = reinterpret_cast<const char*>(PTR_DAT_0050c3f0);
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
						text = reinterpret_cast<const char*>(PTR_DAT_0050c3f0);
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
					std::memcpy(&convertFloat, &PTR_FUN_0050c780, sizeof(convertFloat));

					convertFloat(&value, scratch, signedCharacter(character), asSigned(precision), uppercaseFloat);

					if ((flags & 0x80u) != 0 && precision == 0)
					{
						FloatAdjustment adjustDecimalPoint;
						std::memcpy(&adjustDecimalPoint, &PTR_FUN_0050c78c, sizeof(adjustDecimalPoint));

						adjustDecimalPoint(scratch);
					}

					if (character == 'g' && (flags & 0x80u) == 0)
					{
						FloatAdjustment adjustGeneralFormat;
						std::memcpy(&adjustGeneralFormat, &PTR_FUN_0050c784, sizeof(adjustGeneralFormat));

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

	// Original function: FUN_004ea8a6
	inline void Runtime_LockDescriptor(std::uint32_t descriptor)
	{
		const std::uint32_t blockIndex = descriptor >> 5;
		const std::uint32_t entryOffset = (descriptor & 31u) * 36u;

		auto* block = reinterpret_cast<std::uint8_t*>(DAT_0056b600[blockIndex]);
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
		block = reinterpret_cast<std::uint8_t*>(DAT_0056b600[blockIndex]);
		auto* criticalSection = reinterpret_cast<CRITICAL_SECTION*>(block + entryOffset + 0x0C);

		::EnterCriticalSection(criticalSection);
	}

	// Original function: FUN_004ea905
	inline void Runtime_UnlockDescriptor(std::uint32_t descriptor)
	{
		const std::uint32_t blockIndex = descriptor >> 5;
		const std::uint32_t entryOffset = (descriptor & 31u) * 36u;

		auto* block = reinterpret_cast<std::uint8_t*>(DAT_0056b600[blockIndex]);
		auto* criticalSection = reinterpret_cast<CRITICAL_SECTION*>(block + entryOffset + 0x0C);

		::LeaveCriticalSection(criticalSection);
	}

	static_assert(sizeof(CRITICAL_SECTION) == 0x18, "Descriptor locks require the Win32 critical-section layout");

	// Original function: FUN_004ea864
	inline std::uint32_t Runtime_GetOsHandle(std::uint32_t descriptor)
	{
		if (descriptor < static_cast<std::uint32_t>(uNumber_0056b700))
		{
			const auto* block = reinterpret_cast<const std::uint8_t*>(DAT_0056b600[descriptor >> 5]);
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

	// Original function: FUN_004ea7e5
	inline std::int32_t Runtime_ClearOsHandle(std::uint32_t descriptor)
	{
		if (descriptor < static_cast<std::uint32_t>(uNumber_0056b700))
		{
			const std::uint32_t blockIndex = descriptor >> 5;
			const std::uint32_t entryOffset = (descriptor & 31u) * 36u;
			auto* block = reinterpret_cast<std::uint8_t*>(DAT_0056b600[blockIndex]);

			if ((block[entryOffset + 4u] & 0x01u) != 0)
			{
				std::uint32_t handleValue;
				std::memcpy(&handleValue, block + entryOffset, sizeof(handleValue));

				if (handleValue != 0xFFFFFFFFu)
				{
					if (DAT_0050c244 == 1)
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
					block = reinterpret_cast<std::uint8_t*>(DAT_0056b600[blockIndex]);

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

	// Original function: FUN_004e60de
	inline std::int32_t Runtime_CloseNoLock(std::uint32_t descriptor)
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

		auto* block = reinterpret_cast<std::uint8_t*>(DAT_0056b600[descriptor >> 5]);
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

	// Original function: FUN_004e6081
	inline std::int32_t Runtime_Close(std::uint32_t descriptor)
	{
		if (descriptor < static_cast<std::uint32_t>(uNumber_0056b700))
		{
			const auto* block = reinterpret_cast<const std::uint8_t*>(DAT_0056b600[descriptor >> 5]);
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

	// Original function: FUN_004e650a
	inline std::int32_t Runtime_WriteNoLock(std::uint32_t descriptor, const void* data, std::uint32_t byteCount)
	{
		if (byteCount == 0)
		{
			return 0;
		}

		const std::uint32_t blockIndex = descriptor >> 5;
		const std::uint32_t entryOffset = (descriptor & 31u) * 36u;

		// DAT_0056b600 remains the original, unresolved descriptor-block table.
		const auto getEntry = [blockIndex, entryOffset]() -> const std::uint8_t*
			{
				return reinterpret_cast<const std::uint8_t*>(DAT_0056b600[blockIndex]) + entryOffset;
			};

		const auto getHandle = [&getEntry]() -> HANDLE
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

	// Original function: FUN_004e64a5
	inline std::int32_t Runtime_Write(std::uint32_t descriptor, const void* data, std::uint32_t byteCount)
	{
		if (descriptor < static_cast<std::uint32_t>(uNumber_0056b700))
		{
			const auto* block = reinterpret_cast<const std::uint8_t*>(DAT_0056b600[descriptor >> 5]);
			const std::uint32_t entryOffset = (descriptor & 31u) * 36u;

			if ((block[entryOffset + 4u] & 0x01u) != 0)
			{
				FUN_004ea8a6(descriptor);

				const std::int32_t result = Runtime_WriteNoLock(descriptor, data, byteCount);

				FUN_004ea905(descriptor);
				return result;
			}
		}

		*Runtime_GetErrnoPointer() = 9;
		*Runtime_GetDosErrnoPointer() = 0;
		return -1;
	}

	// Original function: FUN_004e67b5
	inline std::int32_t Runtime_OpenShared(const char* fileName, std::uint32_t flags, std::uint32_t shareMode, std::uint32_t permissions)
	{
		SECURITY_ATTRIBUTES security{};
		security.nLength = sizeof(security);
		security.lpSecurityDescriptor = nullptr;
		security.bInheritHandle = (flags & 0x80u) == 0;

		std::uint8_t descriptorFlags = (flags & 0x80u) != 0 ? 0x10u : 0u;

		if ((flags & 0x8000u) == 0)
		{
			if ((flags & 0x4000u) != 0 || DAT_00569b2c != 0x8000)
			{
				descriptorFlags |= 0x80u;
			}
		}

		const auto failWithError = [](std::int32_t error) -> std::int32_t
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
			const std::uint32_t effectivePermissions = permissions & ~static_cast<std::uint32_t>(DAT_00569a00);

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

		const std::uint32_t descriptorBits = FUN_004ea646();

		if (descriptorBits == 0xFFFFFFFFu)
		{
			return failWithError(24);
		}

		std::int32_t descriptor;
		std::memcpy(&descriptor, &descriptorBits, sizeof(descriptor));

		const HANDLE handle = ::CreateFileA(fileName, desiredAccess, sharing, &security, creationDisposition, attributes, nullptr);

		const auto failWithLastError = [descriptor]() -> std::int32_t
			{
				const DWORD error = ::GetLastError();
				Runtime_MapOsError(error);
				FUN_004ea905(descriptor);
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

		FUN_004ea769(descriptor, handle);

		const std::uint32_t blockIndex = descriptorBits >> 5;
		const std::uint32_t entryOffset = (descriptorBits & 31u) * 36u;

		const auto getEntry = [blockIndex, entryOffset]() -> std::uint8_t*
			{
				return reinterpret_cast<std::uint8_t*>(DAT_0056b600[blockIndex]) + entryOffset;
			};

		descriptorFlags |= 0x01u;
		getEntry()[4] = descriptorFlags;

		const bool isDeviceOrPipe = (descriptorFlags & 0x48u) != 0;

		const auto closeAndFail = [descriptor]() -> std::int32_t
			{
				Runtime_Close(descriptor);
				FUN_004ea905(descriptor);
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
					const std::uint32_t resizeResult = FUN_004eb235(descriptor, endPosition);

					if (resizeResult == 0xFFFFFFFFu)
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

		FUN_004ea905(descriptor);
		return descriptor;
	}

	// Original function: FUN_004e679e
	inline std::int32_t Runtime_Open(const char* fileName, std::uint32_t flags, std::uint32_t permissions)
	{
		return Runtime_OpenShared(fileName, flags, 0x40u, permissions);
	}

	// Original functions: FUN_004e5f2c; forwarding wrapper: FUN_004e5fb7.
	inline std::int32_t Runtime_StringToInteger(const char* text)
	{
		const auto* cursor = reinterpret_cast<const std::uint8_t*>(text);

		const auto hasCharacterType = [](std::uint32_t character, std::uint32_t mask) -> bool
			{
				if (cbMultiByte_0050c604 > 1)
				{
					return FUN_004ea094(static_cast<std::int32_t>(character), mask) != 0;
				}

				const auto* characterTypes = reinterpret_cast<const std::uint8_t*>(PTR_DAT_0050c3f8);
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

	// Original function: FUN_004e7d12
	inline std::uint32_t Runtime_ClassifyAlphanumeric(std::int32_t character)
	{
		if (cbMultiByte_0050c604 > 1)
		{
			return static_cast<std::uint32_t>(FUN_004ea094(character, 0x107));
		}

		const auto* characterTypes = reinterpret_cast<const std::uint8_t*>(PTR_DAT_0050c3f8);

		std::uint16_t flags;
		std::memcpy(&flags, characterTypes + static_cast<std::ptrdiff_t>(character) * 2, sizeof(flags));

		return flags & 0x107u;
	}

	// Original function: FUN_00410c03
	inline bool Runtime_IsPropertyNameCharacter(char character)
	{
		const auto byteValue = static_cast<std::uint8_t>(character);
		const std::int32_t signedCharacter = byteValue < 0x80u ? static_cast<std::int32_t>(byteValue) : static_cast<std::int32_t>(byteValue) - 0x100;

		if (Runtime_ClassifyAlphanumeric(signedCharacter) != 0)
		{
			return true;
		}

		return byteValue == 0x5Fu;
	}

	static_assert(sizeof(void*) == 4, "These CRT descriptor accesses require an x86 build");
}