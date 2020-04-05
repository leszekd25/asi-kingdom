#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace ASI
{
	enum SF_Version { SF_154 = 154, SF_161 = 161 };
	
	///MemoryRegion is used for rewriting code in a given memory block
	struct MemoryRegion 
	{ 
		int memory_offset; 
		int memory_length; 
		DWORD old_reg_perm; 
		MemoryRegion() {}
		MemoryRegion(int off, int len)
		{
			memory_offset = off;
			memory_length = len;
		}
	};
	
	struct Pointer
	{
		void* ptr;
		Pointer()
		{

		}
		Pointer(int val)
		{
			ptr = (void*)val;
		}
		Pointer(unsigned int val)
		{
			ptr = (void*)val;
		}
		Pointer(const Pointer& p)
		{
			ptr = p.ptr;
		}
		Pointer(void* p)
		{
			ptr = p;
		}
		inline Pointer operator[](int offset)
		{
			return *this + offset;
		}
		inline Pointer operator+(int offset)
		{
			return Pointer((int)ptr + offset);
		}
		inline Pointer operator-(int offset)
		{
			return Pointer((int)ptr - offset);
		}
		inline Pointer Follow()
		{
			return Pointer((int)(*(void**)ptr));
		}
		inline Pointer operator*()
		{
			return Pointer((int)(*(void**)ptr));
		}
		inline bool operator==(Pointer p)
		{
			return ptr == p.ptr;
		}
		inline bool operator!=(Pointer p)
		{
			return ptr != p.ptr;
		}
		template<class T>
		inline T& AsRef()
		{
			return *(T*)ptr;
		}
		template<class T>
		inline operator T ()
		{
			return *(T*)ptr;
		}
	};


	const int GAME_BASE = 0x400000;              // start of game code, above this is probably stack?
	const int WINDOW_OFFSET = 0x97CB5C;          // pointer to game window is statically allocated in game exe under address GAME_BASE + WINDOW_OFFSET
	 
	bool BeginRewrite(MemoryRegion& mem_region);
	bool EndRewrite(MemoryRegion& mem_region);

	bool Init(HMODULE lib_module);
	bool CheckSFVersion(SF_Version sf_version);

	/// returns "real" virtual address of given memory offset
	inline int AddrOf(int offset)
	{
		return GAME_BASE + offset;
	}

	template<unsigned int Addr, typename ... Args>
	inline void CallGlobalProc(Args... vals)
	{
		((void(__stdcall*)(Args...))(Addr))(vals...);
	}

	template<unsigned int Addr, typename Result, typename ... Args>
	inline Result CallGlobalFunc(Args... vals)
	{
		return ((Result(__stdcall*)(Args...))(Addr))(vals...);
	}

	template<unsigned int Addr, typename ... Args>
	inline void CallGlobalCDeclProc(Args... vals)
	{
		((void(__cdecl*)(Args...))(Addr))(vals...);
	}

	template<unsigned int Addr, typename Result, typename ... Args>
	inline Result CallGlobalCDeclFunc(Args... vals)
	{
		return ((Result(__cdecl*)(Args...))(Addr))(vals...);
	}

	template<unsigned int Addr, typename ... Args>
	inline void CallClassProc(ASI::Pointer _this, Args... vals)
	{
		((void(__thiscall*)(void*, Args...))(Addr))(_this.ptr, vals...);
	}

	template<unsigned int Addr, typename Result, typename ... Args>
	inline Result CallClassFunc(ASI::Pointer _this, Args... vals)
	{
		return ((Result(__thiscall*)(void*, Args...))(Addr))(_this.ptr, vals...);
	}

	template<typename ... Args>
	inline void CallAsClassProc(void* FPtr, ASI::Pointer _this, Args... vals)
	{
		((void(__thiscall*)(void*, Args...))(FPtr))(_this.ptr, vals...);
	}

	template<typename Result, typename ... Args>
	inline Result CallAsClassFunc(void* FPtr, ASI::Pointer _this, Args... vals)
	{
		return ((Result(__thiscall*)(void*, Args...))(FPtr))(_this.ptr, vals...);
	}
}