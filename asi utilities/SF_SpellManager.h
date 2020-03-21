#pragma once

#include "SF_ASI.h"

namespace ASI
{
	struct SF_SpellManager
	{
		unsigned char data[21698];
	};


	inline unsigned int OffsetToSpell(int spell_index)
	{
		return 27 * spell_index + 86;
	}

	inline unsigned short& SpellCount(SF_SpellManager* man)
	{
		return *((unsigned short*)(man->data + 84));
	}

	inline short& SpellParam0(SF_SpellManager* man, int spell_index)
	{
		return *((short*)(man->data + OffsetToSpell(spell_index) + 0));
	}

	inline short& SpellParam1(SF_SpellManager* man, int spell_index)
	{
		return *((short*)(man->data + OffsetToSpell(spell_index) + 2));
	}

	inline unsigned short& SpellParam3(SF_SpellManager* man, int spell_index)
	{
		return *((unsigned short*)(man->data + OffsetToSpell(spell_index) + 6));
	}
}