#include "native_hooks.h"


#include "game/backend/NativeHooks.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu
{
	void ParagonNativeHooks::Init()
	{
		// TODO: Fix server stat DB so we don't need this: only affects Enhanced
		NativeHooks::AddHook(NativeHooks::ALL_SCRIPTS, NativeIndex::STAT_GET_LOAD_SAFE_TO_PROGRESS_TO_MP_FROM_SP, ProgressMpSp);
		NativeHooks::AddHook(NativeHooks::ALL_SCRIPTS, NativeIndex::NET_GAMESERVER_USE_SERVER_TRANSACTIONS, UseTransactions);
		NativeHooks::AddHook(NativeHooks::ALL_SCRIPTS, NativeIndex::STAT_GET_INT, StatGetInt);
		LOG(INFO) << "Paragon native hooks initialized";
	}

	void ParagonNativeHooks::ProgressMpSp(rage::scrNativeCallContext* src)
	{
		src->SetReturnValue<bool>(true);
	}

	void ParagonNativeHooks::UseTransactions(rage::scrNativeCallContext* src)
	{
		src->SetReturnValue<bool>(false);
	}

	void ParagonNativeHooks::StatGetInt(rage::scrNativeCallContext* src) {
		const auto hash = src->GetArg<Hash>(0);
		int* out        = src->GetArg<int*>(1);
		switch (hash) {
		case "MP0_CHAR_IS_NGPC_VERSION"_J:
		case "MP1_CHAR_IS_NGPC_VERSION"_J:
		case "MP1_CHAR_IS_NG_VERSION"_J:
		case "MP0_CHAR_IS_NG_VERSION"_J:
		case "MP0_CHAR_IS_NG_VERSION_SA"_J:
		case "MP1_CHAR_IS_NG_VERSION_SA"_J:
			*out             = 1;
			src->SetReturnValue<INT>(1);
			return;
		default:
			src->SetReturnValue<INT>(STATS::STAT_GET_INT(hash, out, src->GetArg<int>(2)));
		}
	}
}
