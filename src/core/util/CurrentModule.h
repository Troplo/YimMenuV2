#ifndef CURRENT_MODULE_HPP
#define CURRENT_MODULE_HPP
#include "Joaat.hpp"
#include "core/memory/Module.hpp"
#include <string>

inline std::string cachedModule;
inline YimMenu::joaat_t cachedModuleJ = 0;

inline std::string GetCurrentModule() {
	if (!cachedModule.empty()) return cachedModule;
	const char* modules[] = { "GTA5_Enhanced.exe", "Paragon_Enhanced.exe" };

	for (int i = 0; i < 2; ++i) {
		if (GetModuleHandle(modules[i]) != NULL) {
			cachedModule = modules[i];
			return modules[i];
		}
	}
	return "";
}

inline YimMenu::joaat_t GetCurrentModuleJ() {
	if (cachedModuleJ) return cachedModuleJ;

	std::string module = GetCurrentModule();
	LOG(INFO) << module;
	YimMenu::joaat_t moduleJ = YimMenu::Joaat(module.data());

	cachedModuleJ = moduleJ;
	return moduleJ;
}

#endif //CURRENT_MODULE_HPP
