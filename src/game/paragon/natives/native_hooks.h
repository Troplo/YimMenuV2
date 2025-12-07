#ifndef NATIVE_HOOKS_H
#define NATIVE_HOOKS_H
#include "game/backend/NativeHooks.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu
{
	class ParagonNativeHooks
	{
		static void ProgressMpSp(rage::scrNativeCallContext* src);
		static void UseTransactions(rage::scrNativeCallContext* src);
		static void StatGetInt(rage::scrNativeCallContext* src);

	public:
		static void Init();
	};
}
#endif //NATIVE_HOOKS_H
