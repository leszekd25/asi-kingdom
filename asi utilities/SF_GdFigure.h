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
}