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

		game_window = *(HWND*)(ASI::AddrOf(ASI::WINDOW_OFFSET));

		//ASI::MemoryRegion mreg(ASI::AddrOf(0x0AD7DA), 10);
		//ASI::MemoryRegion mreg2(ASI::AddrOf(0x0AD963), 13);
		ASI::MemoryRegion mreg3(ASI::AddrOf(0x906DDC), 4);

		//ASI::BeginRewrite(mreg);
		//*(int*)(ASI::AddrOf(0x0AD7E0)) = 0x0000001;
		//ASI::EndRewrite(mreg);

		/*ASI::BeginRewrite(mreg2);
		*(int*)(ASI::AddrOf(0x0AD963)) = 0x9090905A;
		*(int*)(ASI::AddrOf(0x0AD967)) = 0x90909090;
		*(int*)(ASI::AddrOf(0x0AD96B)) = 0x90909090;
		*(unsigned char*)(ASI::AddrOf(0x0AD96F)) = 0x90;
		ASI::EndRewrite(mreg2);*/

		ASI::BeginRewrite(mreg3);
		*(int*)(ASI::AddrOf(0x906DDC)) = 0x00000000;
		ASI::EndRewrite(mreg3);
	}
	break;
	case DLL_PROCESS_DETACH:
		// do cleanup here if necessary
		break;
	}
	return TRUE;
}

