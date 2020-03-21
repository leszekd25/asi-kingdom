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
}