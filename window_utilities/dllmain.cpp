// dllmain.cpp : Defines the entry point for the DLL application.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include "SF_ASI.h"

int WND_INSTR1_ABSOLUTE;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	HWND game_window;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		if (!ASI::Init(hModule))
			return FALSE;
		if (!ASI::CheckSFVersion(ASI::SF_154))
			return FALSE;

		// make window not pause on tab out
		ASI::MemoryRegion mreg_windowproc(ASI::AddrOf(0x0AD600), 0x810);

		game_window = *(HWND*)(ASI::AddrOf(ASI::WINDOW_OFFSET));

		ASI::BeginRewrite(mreg_windowproc);
		// make it so the cursor is no longer clipped
		*(unsigned char*)(ASI::AddrOf(0x0AD710)) = 0x58;
		*(unsigned char*)(ASI::AddrOf(0x0AD711)) = 0x90;
		*(unsigned char*)(ASI::AddrOf(0x0AD712)) = 0x90;
		*(unsigned char*)(ASI::AddrOf(0x0AD713)) = 0x90;
		*(unsigned char*)(ASI::AddrOf(0x0AD714)) = 0x90;
		*(unsigned char*)(ASI::AddrOf(0x0AD715)) = 0x90;

		// skip allocating/deallocating OS resources when activating/deactivating window
		*(unsigned char*)(ASI::AddrOf(0x0AD946)) = 0xE9;
		*(unsigned char*)(ASI::AddrOf(0x0AD947)) = 0xDC;
		*(unsigned char*)(ASI::AddrOf(0x0AD948)) = 0xFD;
		*(unsigned char*)(ASI::AddrOf(0x0AD949)) = 0xFF;
		*(unsigned char*)(ASI::AddrOf(0x0AD94A)) = 0xFF;

		*(unsigned char*)(ASI::AddrOf(0x0AD9F5)) = 0x01;

		*(unsigned char*)(ASI::AddrOf(0x0AD9F9)) = 0xE9;
		*(unsigned char*)(ASI::AddrOf(0x0AD9FA)) = 0x2F;
		*(unsigned char*)(ASI::AddrOf(0x0AD9FB)) = 0xFD;
		*(unsigned char*)(ASI::AddrOf(0x0AD9FC)) = 0xFF;
		*(unsigned char*)(ASI::AddrOf(0x0AD9FD)) = 0xFF;

		// todo: work out why mouse stops properly working when the window is minimized or alt-tabbed

		ASI::EndRewrite(mreg_windowproc);
	}
	break;
	case DLL_PROCESS_DETACH:
		// do cleanup here if necessary
		break;
	}
	return TRUE;
}

