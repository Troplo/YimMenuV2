#if ENABLE_PARAPAK
//
// Created by Troplo on 16/07/2026.
//

#include "VehPackHandler.h"

#include <tlhelp32.h>

namespace YimMenu
{

	uintptr_t VehPackHandler::m_PointerLoadTarget{};
	uintptr_t VehPackHandler::m_ExecTarget{};
	uintptr_t VehPackHandler::m_WriteTarget{};
	void* VehPackHandler::m_VehHandle{};
	bool VehPackHandler::m_Complete{};
	// Basically the game writes over functions it previously unpacked, fucking them up and causing the encrypted/packed instructions to be ran
	// we can set the table to have no entries and have the loop terminate immediately and nothing will be overridden
	// hooking the custom memcpy func directly will be detected it seems but this works
	static constexpr uint8_t TABLE[] = {0xFF, 0xFF, 0xFF, 0xFF, 0x0F};

	LONG WINAPI VehPackHandler::VehHandler(EXCEPTION_POINTERS* info)
	{
		auto record = info->ExceptionRecord;
		auto ctx = info->ContextRecord;

		if (record->ExceptionCode == EXCEPTION_SINGLE_STEP)
		{
			bool handled = false;

			if (ctx->Dr6 & (1ull << 0))
			{
				if (ctx->Rip == m_PointerLoadTarget)
				{
					ctx->Rax = reinterpret_cast<uintptr_t>(&TABLE);
					m_Complete = true;
					LOG(INFO) << "Replaced the table";
				}

				ctx->Dr6 &= ~(1ull << 0);
				ctx->EFlags |= (1ull << 16);
				handled = true;
			}

			if (ctx->Dr6 & (1ull << 1))
			{
				if (ctx->Rip == m_ExecTarget)
				{
					void* a1 = reinterpret_cast<void*>(ctx->Rcx);
					void* a2 = reinterpret_cast<void*>(ctx->Rdx);
					uint32_t a3 = static_cast<uint32_t>(ctx->R8);

					LOG(VERBOSE) << std::format("Encryptor1 Execution a1 {} a2 {} a3 {}", a1, a2, a3);
				}

				ctx->Dr6 &= ~(1ull << 1);
				ctx->EFlags |= (1ull << 16);
				handled = true;
			}

			// if (ctx->Dr6 & (1ull << 2))
			// {
			// 	LOG(VERBOSE) << std::format("Write hit on target. Instruction after write RIP: {:X}", ctx->Rip);
			//
			// 	ctx->Dr6 &= ~(1ull << 2);
			// 	ctx->EFlags |= (1ull << 16);
			// 	handled = true;
			// }

			if (handled)
			{
				return EXCEPTION_CONTINUE_EXECUTION;
			}
		}

		return EXCEPTION_CONTINUE_SEARCH;
	}

	void VehPackHandler::InitializeVehHooks(void* pointerLoadTarget, void* execTarget /*, void* writeTarget*/)
	{
		if (m_VehHandle)
		{
			return;
		}

		m_PointerLoadTarget = reinterpret_cast<uintptr_t>(pointerLoadTarget);
		m_ExecTarget = reinterpret_cast<uintptr_t>(execTarget);
		// m_WriteTarget = reinterpret_cast<uintptr_t>(writeTarget);

		m_VehHandle = AddVectoredExceptionHandler(1, VehHandler);

		// It can be injected too late after the functions have already been replaced
		// we don't restore them, we just patch it so that they don't get replaced in the first place
		std::thread([]()
		{
			std::this_thread::sleep_for(std::chrono::seconds(10));

			if (!m_Complete)
			{
				MessageBoxA(
					nullptr,
					"Fatal timing error. Please restart the game.",
					"Fatal Error",
					MB_OK | MB_ICONERROR
				);

				TerminateProcess(GetCurrentProcess(), 1);
			}
		}).detach();

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (snapshot == INVALID_HANDLE_VALUE)
		{
			return;
		}

		DWORD currentPid = GetCurrentProcessId();
		DWORD currentTid = GetCurrentThreadId();
		THREADENTRY32 te32;
		te32.dwSize = sizeof(THREADENTRY32);

		if (Thread32First(snapshot, &te32))
		{
			do
			{
				if (te32.th32OwnerProcessID == currentPid && te32.th32ThreadID != currentTid)
				{
					HANDLE thread = OpenThread(THREAD_SET_CONTEXT | THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
					if (thread)
					{
						SuspendThread(thread);

						CONTEXT ctx{};
						ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;

						if (GetThreadContext(thread, &ctx))
						{
							if (m_PointerLoadTarget)
							{
								ctx.Dr0 = m_PointerLoadTarget;
								ctx.Dr7 |= (1ull << 0);
								ctx.Dr7 &= ~(3ull << 16);
								ctx.Dr7 &= ~(3ull << 18);
							}

							if (m_ExecTarget)
							{
								ctx.Dr1 = m_ExecTarget;
								ctx.Dr7 |= (1ull << 2);
								ctx.Dr7 &= ~(3ull << 20);
								ctx.Dr7 &= ~(3ull << 22);
							}

							// if (g_WriteTarget)
							// {
							// 	ctx.Dr2 = g_WriteTarget;
							// 	ctx.Dr7 |= (1ull << 4);
							// 	ctx.Dr7 |= (1ull << 24);
							// 	ctx.Dr7 &= ~(1ull << 25);
							// 	ctx.Dr7 &= ~(3ull << 26);
							// 	ctx.Dr7 |= (2ull << 26);
							// }

							SetThreadContext(thread, &ctx);
						}

						ResumeThread(thread);
						CloseHandle(thread);
					}
				}
			} while (Thread32Next(snapshot, &te32));
		}
		CloseHandle(snapshot);
	}
} // YimMenu
#endif