#include "core/hooking/DetourHook.hpp"
#include "core/renderer/Renderer.hpp"
#include "game/hooks/Hooks.hpp"

namespace YimMenu::Hooks
{
	LRESULT Window::WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
	{
		if (umsg == WM_SYSKEYDOWN && wparam == VK_RETURN)
		{
			LOG(VERBOSE) << "Alt+Enter";
			return 0;
		}

		if (g_Running)
			Renderer::WndProc(hwnd, umsg, wparam, lparam);

		return BaseHook::Get<Window::WndProc, DetourHook<WNDPROC>>()->Original()(hwnd, umsg, wparam, lparam);
	}
}