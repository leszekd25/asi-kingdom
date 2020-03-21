#include "SF_ASI.h"

namespace ASI
{
	/// allows rewriting code in a given memory region by overwriting permissions for that memory region
	bool BeginRewrite(MemoryRegion& mem_region)
	{
		return VirtualProtect((LPVOID)mem_region.memory_offset, (SIZE_T)mem_region.memory_length, 0x40, &mem_region.old_reg_perm);   // 0x40 - enable everything?
	}

	/// ends rewriting by restoring old permissions in a given memory region
	bool EndRewrite(MemoryRegion& mem_region)
	{
		DWORD tmp_old_region_permission;
		bool b = VirtualProtect((LPVOID)mem_region.memory_offset, (SIZE_T)mem_region.memory_length, mem_region.old_reg_perm, &tmp_old_region_permission) != 0;
		if (b)
			FlushInstructionCache((HANDLE)0xFFFFFFFF, 0, 0);
		return b;
	}
    
	/// required for everything to work... why?
	bool Init(HMODULE lib_module)
	{
		if (!DisableThreadLibraryCalls(lib_module))
			return false;
		return true;
	}

	/// check version of the game that was hooked into
	bool CheckSFVersion(SF_Version sf_version)
	{
		switch (sf_version)
		{
		case SF_154:
			if (*(int*)0x40EFF8 == 75000) // 1.54.75000
				return true;
			break;
		default:
			return false;
		}
		return false;
	}
}