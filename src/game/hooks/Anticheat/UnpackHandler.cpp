#if ENABLE_PARAPAK
//
// Created by Troplo on 16/07/2026.
//

#include "UnpackHandler.h"

#include "game/pointers/Pointers.hpp"

namespace YimMenu
{
	void UnpackHandler::DecodeStream(
	    const uint8_t** stream,
	    uint32_t* offset,
	    uint32_t* size)
	{
		uint32_t delta = 0;
		uint32_t shift = 0;

		while (true)
		{
			uint8_t b = *(*stream)++;
			delta |= (b & 0x7F) << shift;
			shift += 7;
			if (!(b & 0x80))
				break;
		}

		*offset += delta + *size;

		if (*offset == 0xFFFFFFFF)
			return;

		uint32_t newSize = 0;
		shift = 0;

		while (true)
		{
			uint8_t b = *(*stream)++;
			newSize |= (b & 0x7F) << shift;
			shift += 7;
			if (!(b & 0x80))
				break;
		}

		*size = newSize;
	}
	void UnpackHandler::Unpack(UnpackContext& ctx)
	{
		uint8_t* imageBase = (uint8_t*)GetModuleHandleA(nullptr);

		uint32_t bytesToCopy = 0;
		uint32_t blockOffset = 0;
		uint32_t blockSize = 0;
		uint32_t relocOffset = 0xFFFFFFFF;

		const uint32_t* relocStream = reinterpret_cast<const uint32_t*>(ctx.relocations);

		uint32_t hasPendingReloc = 0;
		uint32_t relocBytesRemaining = 0;

		uint8_t decryptedBuffer[4] = {0};
		uint32_t decryptedBufferOffset = 4;
		uint8_t relocBuffer[8] = {0};

		LOG(VERBOSE) << "Starting unpack, module base: " << (void*)imageBase;
		Logger::FlushQueue();

		while (true)
		{
			DecodeStream(&ctx.locations, &blockOffset, &blockSize);

			if (blockOffset == 0xFFFFFFFF)
			{
				LOG(INFO) << "End reached, blockOffset: -1";
				break;
			}

			uint8_t* outputPtr = imageBase + blockOffset;
			uint8_t* blockEnd = outputPtr + blockSize;
			LOG(VERBOSE) << "Processing block at " << (void*)outputPtr << " size " << blockSize;
			Logger::FlushQueue();

			while (outputPtr != blockEnd)
			{
				if (decryptedBufferOffset == 4)
				{
					uint32_t encryptedChunk = 0;
					std::memcpy(&encryptedChunk, ctx.data, 4);

					uint32_t decryptedChunk = ctx.decryptionFunc(encryptedChunk, ctx.key);
					std::memcpy(decryptedBuffer, &decryptedChunk, 4);

					ctx.data += 4;
					decryptedBufferOffset = 0;
				}

				bytesToCopy = static_cast<uint32_t>(blockEnd - outputPtr);

				if ((4 - decryptedBufferOffset) < bytesToCopy)
				{
					bytesToCopy = 4 - decryptedBufferOffset;
				}

				if (relocBytesRemaining == 0 && ctx.v14_val != 0)
				{
					if (hasPendingReloc == 0)
					{
						if (relocStream)
						{
							relocOffset = relocStream[0];
							relocStream += 2;
							hasPendingReloc = 1;
						}
					}

					if (relocOffset != 0xFFFFFFFF)
					{
						if (imageBase + relocOffset == outputPtr)
						{
							relocBytesRemaining = 8;
						}
						else if (static_cast<uint32_t>((imageBase + relocOffset) - outputPtr) < bytesToCopy)
						{
							bytesToCopy = static_cast<uint32_t>((imageBase + relocOffset) - outputPtr);
						}
					}
				}

				if (relocBytesRemaining != 0)
				{
					if (relocBytesRemaining < bytesToCopy)
						bytesToCopy = relocBytesRemaining;
					std::memcpy(
					    &relocBuffer[8 - relocBytesRemaining],
					    &decryptedBuffer[decryptedBufferOffset],
					    bytesToCopy);
					relocBytesRemaining -= bytesToCopy;
					if (relocBytesRemaining == 0)
					{
						uint64_t relocatedValue = ctx.v2_val + ctx.v14_val;
						std::memcpy(outputPtr, &relocatedValue, 8);

						hasPendingReloc = 0;
						outputPtr += 8;
					}
				}
				else
				{
					std::memcpy(outputPtr, &decryptedBuffer[decryptedBufferOffset], bytesToCopy);
					outputPtr += bytesToCopy;
				}

				decryptedBufferOffset += bytesToCopy;
			}
		}
	}

	void UnpackHandler::DoUnpack()
	{
		// some online functions
		{
			UnpackContext ctx = {};
			ctx.data = (uint8_t*)Pointers.NetworkFunctionData;
			ctx.locations = (uint8_t*)Pointers.NetworkFunctionLocations;
			ctx.relocations = reinterpret_cast<const uint64_t*>(0x0FFFFFFFFFFFFFFFF);

			ctx.key = 0x4CF737F8;
			ctx.decryptionFunc = [](uint32_t enc, uint32_t key) -> uint32_t {
				return enc ^ key;
			};

			Unpack(ctx);
		}
	}
}
#endif