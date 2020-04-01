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

	inline unsigned short GetCurrentHealthPercentLeft(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return ASI::CallClassFunc<0x7BCFC0, unsigned short>
			(figuredata[figure_index * 691 + 0x42]);
	}

	inline short GetCurrentHealth(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return ASI::CallClassFunc<0x5C79D0, short, unsigned short>
			(figuredata, figure_index);
	}

	inline short GetCurrentHealthMax(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return ASI::CallClassFunc<0x669090, short, unsigned short>
			(figuredata, figure_index);
	}

	inline short GetCurrentHealthMissing(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return GetCurrentHealthMax(figuredata, figure_index) - GetCurrentHealth(figuredata, figure_index);
	}

	inline unsigned short GetLifeAction(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return (unsigned short)(figuredata[figure_index * 691 + 0x48]);
	}

	inline unsigned char GetRace(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return (unsigned char)(figuredata[figure_index * 691 + 0x1A]);
	}

	inline unsigned short GetCurrentMana(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return ASI::CallClassFunc<0x6690F0, unsigned short, unsigned short>
			(figuredata, figure_index);
	}

	inline unsigned short GetCurrentCharisma(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return ASI::CallClassFunc<0x667C70, unsigned short, unsigned short>
			(figuredata, figure_index);
	}

	inline unsigned short GetSpellJobStartNode(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return ASI::CallClassFunc<0x6695D0, unsigned short, unsigned short>
			(figuredata, figure_index);
	}

	inline unsigned short GetSpellJobNextNode(ASI::Pointer spelljobdata, unsigned short spelljobnode)   // [ebp+0x10]
	{
		return (unsigned short)ASI::Pointer((unsigned int)(spelljobdata.ptr) + spelljobnode * 6);
	}

	inline unsigned short GetSpellJobNodeSpellIndex(ASI::Pointer spelljobdata, unsigned short spelljobnode)
	{
		return (unsigned short)ASI::Pointer(spelljobdata[spelljobnode * 6 + 4]);
	}

	// returns effect index in table if it exists, -1 otherwise
	int HasEffect(ASI::SF_SpellManager* spell_manager, unsigned short figure_index, unsigned short spellline_id)
	{
		auto spelljobnode = GetSpellJobStartNode(GetFigureManager(spell_manager), figure_index);
		while (spelljobnode != 0)
		{
			ASI::Pointer some_pointer3(GetUnknownPointer(spell_manager, 0x10));
			ASI::Pointer some_pointer31(*some_pointer3[0]);
			auto spell_index = GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode);
			if(SpellType(spell_manager, spell_index) == spellline_id)
				return spell_index;
			spelljobnode = GetSpellJobNextNode(some_pointer31, spelljobnode);
		}
		return -1;
	}

	int HasEffectExcept(ASI::SF_SpellManager* spell_manager, unsigned short figure_index, unsigned short spellline_id, unsigned short except_effect_id)
	{
		auto spelljobnode = GetSpellJobStartNode(GetFigureManager(spell_manager), figure_index);
		while (spelljobnode != 0)
		{
			ASI::Pointer some_pointer3(GetUnknownPointer(spell_manager, 0x10));
			ASI::Pointer some_pointer31(*some_pointer3[0]);
			auto spell_index = GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode);
			if ((spell_index != except_effect_id) && (SpellType(spell_manager, spell_index) == spellline_id))
				return spell_index;
			spelljobnode = GetSpellJobNextNode(some_pointer31, spelljobnode);
		}
		return -1;
	}

	inline void RemoveFlags(ASI::Pointer figuredata, unsigned short figure_index, unsigned int flags)
	{
		ASI::CallClassProc<0x664050, unsigned short>
			(figuredata, figure_index, flags);
	}
}