#pragma once
#include "core/frontend/manager/Category.hpp"
#include "game/frontend/items/Items.hpp"
#include "types/stats/sStatData.hpp"

namespace YimMenu::Submenus
{
	union StatValue;
	std::shared_ptr<Category> BuildStatEditorMenu();
	static void WriteStat(std::uint32_t hash, StatValue& value, sStatData* data);
	static void ReadStat(StatValue& value, sStatData* data);
}