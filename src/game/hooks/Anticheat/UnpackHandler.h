//
// Created by Troplo on 16/07/2026.
//
#if ENABLE_PARAPAK
#ifndef UNPACKHANDLER_H
#define UNPACKHANDLER_H

namespace YimMenu {
	class UnpackHandler {
		struct UnpackContext {
			const uint8_t* data;
			const uint8_t* locations;
			const uint64_t* relocations;

			uint32_t key;
			uint64_t v14_val;
			uint64_t v2_val;

			// some functions can have a different decryption operation, xor, - etc
			std::function<uint32_t(uint32_t, uint32_t)> decryptionFunc;
		};
	public:
		static void Unpack(UnpackContext& ctx);
		static void DoUnpack();
		static void DecodeStream(const uint8_t** stream, uint32_t* out_v7, uint32_t* out_v8);
	};
} // Nexus
#endif
#endif //UNPACKHANDLER_H
