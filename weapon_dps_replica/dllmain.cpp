// dllmain.cpp : Defines the entry point for the DLL application.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include "SF_ASI.h"
#include "SF_UTL_String.h"

int WPN_EXEC_ABSOLUTE;
int WPN_STR_ABSOLUTE;

int AV_EXEC_ABSOLUTE;

wchar_t wpn_buffer[0x200];
wchar_t av_buffer[0x200];

void wpn_dps()
{
	ASI::Pointer stack_pointer(0);
	__asm mov stack_pointer.ptr, ecx

	ASI::Pointer wpn_ptr = *stack_pointer[0x24];
	ASI::Pointer min_dmg_ptr = wpn_ptr[0x65];
	ASI::Pointer max_dmg_ptr = wpn_ptr[0x67];
	ASI::Pointer speed_ptr = wpn_ptr[0x6D];

	float dps = ((unsigned short)min_dmg_ptr
		+ (unsigned short)max_dmg_ptr)
		* (unsigned char)speed_ptr
		/ 200.0f;

	ASI::Pointer ret_buf_ptr = stack_pointer[0x1C];
	if (swprintf_s(wpn_buffer, 0x200, L" (%.1f)%s",
		dps,
		(wchar_t*)ret_buf_ptr) > 0)
		ret_buf_ptr.AsRef<wchar_t*>() = wpn_buffer;
	return;
}

void* avatar_level_xp(ASI::SF_UTL_String* str)
{
	ASI::Pointer stack_pointer(0);
	__asm mov stack_pointer.ptr, ecx

	ASI::Pointer lvl_text_buf_ptr = stack_pointer[0x38];

	int newstr_l = swprintf_s(av_buffer, 0x200, L"EXP: %i",//L"%s (EXP: %i/%i)"
		//(wchar_t*)lvl_text_buf_ptr,
		//0,
		(unsigned int)stack_pointer[0x2C]);
	if(newstr_l > 0)
	{
		lvl_text_buf_ptr.AsRef<wchar_t*>() = av_buffer;
		str->data = av_buffer;
		str->length = newstr_l;
	}

	return str;
}

// __declspec(naked) means only precisely this assembly code will be executed when this function is called
// injecting this to game code at precise place will execute this assembly code without trouble
void __declspec(naked) wpn_dps_hook()
{
	__asm
	{
		push ecx
		push ebx
		push eax
		mov ecx, esp
		call wpn_dps
		pop eax
		pop ebx
		pop ecx
		push WPN_STR_ABSOLUTE
		jmp WPN_EXEC_ABSOLUTE
	}
}

void __declspec(naked) avatar_level_xp_hook()
{
	__asm
	{
		push ecx
		push ebx
		push eax
		mov ecx, esp
		call avatar_level_xp
		pop ebx
		pop ebx
		pop ecx
		mov ecx, [ebx+0x3C0]
		jmp AV_EXEC_ABSOLUTE
	}
}


BOOL APIENTRY DllMain( HMODULE hModule,
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

		//game_window = *(HWND*)(ASI::AddrOf(ASI::WINDOW_OFFSET));

		WPN_EXEC_ABSOLUTE = ASI::AddrOf(0x5389DE);
		WPN_STR_ABSOLUTE = ASI::AddrOf(0x93B53C);
		ASI::MemoryRegion mreg(ASI::AddrOf(0x5389D9), 5);

		AV_EXEC_ABSOLUTE = ASI::AddrOf(0x63BD21);
		ASI::MemoryRegion mreg2(ASI::AddrOf(0x63BD1B), 5);

		ASI::BeginRewrite(mreg);
		*(unsigned char*)(ASI::AddrOf(0x5389D9)) = 0xE9;   // jmp instruction
		*(int*)(ASI::AddrOf(0x5389DA)) = (int)(&wpn_dps_hook) - ASI::AddrOf(0x5389DE);  // not D9, DE - skip one instruction?
		ASI::EndRewrite(mreg);

		ASI::BeginRewrite(mreg2);
		*(unsigned char*)(ASI::AddrOf(0x63BD1B)) = 0xE9;   // jmp instruction
		*(int*)(ASI::AddrOf(0x63BD1C)) = (int)(&avatar_level_xp_hook) - ASI::AddrOf(0x63BD20);
		ASI::EndRewrite(mreg2);

	}
		break;
    case DLL_PROCESS_DETACH:
		// do cleanup here if necessary
        break;
    }
    return TRUE;
}

