//
// Created by Troplo on 16/07/2026.
//

#ifndef VEHPACKHANDLER_H
#define VEHPACKHANDLER_H
#include <complex.h>
#include <winnt.h>

namespace YimMenu {

class VehPackHandler {
private:
	static uintptr_t m_PointerLoadTarget;
	static uintptr_t m_ExecTarget;
	static uintptr_t m_WriteTarget;
	static void* m_VehHandle;
	static bool m_Complete;

	static LONG VehHandler(EXCEPTION_POINTERS* info);
public:
	static void InitializeVehHooks(void* pointerLoadTarget, void* execTarget);
	static bool IsCompleted() { return m_Complete; }
};

} // YimMenu

#endif //VEHPACKHANDLER_H
