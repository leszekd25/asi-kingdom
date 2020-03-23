#pragma once

#include "SF_ASI.h"

namespace ASI
{
	inline unsigned int IsAlive(ASI::Pointer figuredata, unsigned short figure_index)
	{
        return (
			(((short)figuredata[figure_index*691 + 0x1C]) != -1) && 
			(((unsigned int)figuredata[figure_index * 691 + 0x16] & 0xA) == 0));
	}

	inline unsigned short GetCurrentHealth(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return ASI::CallClassFunc<0x5C79D0, unsigned short, unsigned short>
			(figuredata, figure_index);
	}

	inline unsigned short GetCurrentHealthMax(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return ASI::CallClassFunc<0x669090, unsigned short, unsigned short>
			(figuredata, figure_index);
	}

	int HasEffect(ASI::Pointer class_pointer, unsigned short figure_index, unsigned short spellline_id)
	{
		auto spelljobstartnode = ASI::CallClassFunc<0x6695D0, unsigned short, unsigned short>(*class_pointer[0x1C], figure_index);
		while (spelljobstartnode != 0)
		{
			ASI::Pointer some_pointer3(*class_pointer[0x10]);
			ASI::Pointer some_pointer31(*some_pointer3[0]);
			auto some_val2 = (unsigned short)ASI::Pointer(some_pointer31[spelljobstartnode * 6 + 4]);
			auto spell_type2 = (unsigned short)ASI::Pointer(class_pointer[some_val2 * 27 + 0x5A]);
			if (spell_type2 == spellline_id)
				return some_val2;
			spelljobstartnode = (unsigned short)ASI::Pointer((unsigned int)(some_pointer31.ptr) + spelljobstartnode * 6);
		}
		return -1;
	}

	int HasEffectExcept(ASI::Pointer class_pointer, unsigned short figure_index, unsigned short spellline_id, unsigned short except_effect_id)
	{
		auto spelljobstartnode = ASI::CallClassFunc<0x6695D0, unsigned short, unsigned short>(*class_pointer[0x1C], figure_index);
		ASI::Pointer some_pointer3(*class_pointer[0x10]);
		ASI::Pointer some_pointer31(*some_pointer3[0]);
		while (spelljobstartnode != 0)
		{
			auto some_val2 = (unsigned short)ASI::Pointer(some_pointer31[spelljobstartnode * 6 + 4]);
			if (some_val2 != except_effect_id)
			{
				auto spell_type2 = (unsigned short)ASI::Pointer(class_pointer[some_val2 * 27 + 0x5A]);
				if (spell_type2 == spellline_id)
					return some_val2;
			}
			spelljobstartnode = (unsigned short)ASI::Pointer((unsigned int)(some_pointer31.ptr) + spelljobstartnode * 6);
		}
		return -1;
	}
}