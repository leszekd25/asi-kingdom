// dllmain.cpp : Defines the entry point for the DLL application.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include "SF_ASI.h"

int SPEEDUP_EXEC_ABSOLUTE;
int SLOWDOWN_EXEC_ABSOLUTE;

void speed_up()
{
	ASI::Pointer AppScript(0xD678C0);
	AppScript = *AppScript;
	ASI::Pointer AppSession = AppScript[0x8];
	ASI::CallClassProc<0x469E10, float>(AppSession.ptr, 20.0f);
}

void slow_down()
{
	ASI::Pointer AppScript(0xD678C0);
	AppScript = *AppScript;
	ASI::Pointer AppSession = AppScript[0x8];
	ASI::CallClassProc<0x469E10, float>(AppSession.ptr, 1.0f);
}

// __declspec(naked) means only precisely this assembly code will be executed when this function is called
// injecting this to game code at precise place will execute this assembly code without trouble
void __declspec(naked) speed_up_hook()
{
	__asm
	{
		push esi
		call speed_up
		pop esi
		mov ecx, [esi+30576]
		jmp SPEEDUP_EXEC_ABSOLUTE
	}
}

void __declspec(naked) slow_down_hook()
{
	__asm
	{
		push esi
		call slow_down
		pop esi
		mov ecx, [esi+34988]
		jmp SLOWDOWN_EXEC_ABSOLUTE
	}
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	//HWND game_window;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		if (!ASI::Init(hModule))
			return FALSE;
		if (!ASI::CheckSFVersion(ASI::SF_154))
			return FALSE;

		SPEEDUP_EXEC_ABSOLUTE = ASI::AddrOf(0x59B0FE);
		SLOWDOWN_EXEC_ABSOLUTE = ASI::AddrOf(0x59B23F);

		ASI::MemoryRegion speedup_reg(ASI::AddrOf(0x59B0F8), 6);
		ASI::MemoryRegion slowdown_reg(ASI::AddrOf(0x59B239), 6);

		ASI::BeginRewrite(speedup_reg);
		*(unsigned char*)(ASI::AddrOf(0x59B0F8)) = 0xE9;   // jmp instruction
		*(int*)(ASI::AddrOf(0x59B0F9)) = (int)(&speed_up_hook) - ASI::AddrOf(0x59B0FD);
		*(unsigned char*)(ASI::AddrOf(0x59B0FD)) = 0x90;   // nop instruction
		ASI::EndRewrite(speedup_reg);

		ASI::BeginRewrite(speedup_reg);
		*(unsigned char*)(ASI::AddrOf(0x59B239)) = 0xE9;   // jmp instruction
		*(int*)(ASI::AddrOf(0x59B23A)) = (int)(&slow_down_hook) - ASI::AddrOf(0x59B23E);
		*(unsigned char*)(ASI::AddrOf(0x59B23E)) = 0x90;   // nop instruction
		ASI::EndRewrite(speedup_reg);
	}
	break;
	case DLL_PROCESS_DETACH:
		// do cleanup here if necessary
		break;
	}
	return TRUE;
}

