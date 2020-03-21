#pragma once


#include "SF_ASI.h"

namespace ASI
{
	inline int GetXData(ASI::Pointer xdata, unsigned int xdata_unk, unsigned char key)
	{
		return ASI::CallClassFunc<0x7A4E00, unsigned int, unsigned int, unsigned char>
			(xdata, xdata_unk, key);
	}

	inline void SetXData(ASI::Pointer xdata, unsigned int xdata_unk, unsigned char key, int value)
	{
		ASI::CallClassProc<0x7A4F00, unsigned int, unsigned char, unsigned int>
			(xdata, xdata_unk, key, value);
	}

	inline int SubXData(ASI::Pointer xdata, unsigned int xdata_unk, unsigned char key, int value)
	{
		return ASI::CallClassFunc<0x7A55E0, unsigned int, unsigned char, unsigned int>
			(xdata, xdata_unk, key, value);
	}

	inline void XDataListAdd(ASI::Pointer xdatalist, unsigned int effect_index, unsigned char key, int value)
	{
		ASI::CallClassProc<0x81A440, unsigned int, unsigned char, int>(xdatalist, effect_index, key, value);
	}
}