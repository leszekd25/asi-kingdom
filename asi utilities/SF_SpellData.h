#pragma once

#include "SF_ASI.h"

namespace ASI
{
	enum SF_SpellLines
	{
		SL_ABILITY_BENEFACTIONS = 106,
		SL_ABILITY_BERSERK = 109,
		SL_ABILITY_BOONS = 110,
		SL_ABILITY_CRITICAL_HITS = 160,
		SL_ABILITY_DURABILITY = 112,
		SL_ABILITY_ENDURANCE = 108,
		SL_ABILITY_PATRONIZE = 107,
		SL_ABILITY_RIPOSTE = 159,
		SL_ABILITY_SALVO = 115,
		SL_ABILITY_SHELTER = 111,
		SL_ABILITY_SHIFT_LIFE = 158,
		SL_ABILITY_STEELSKIN = 114,
		SL_ABILITY_TRUESHOT = 113,
		SL_ABILITY_WARCRY = 105,
		SL_ACID_CLOUD = 64,
		SL_ALMIGHTNESS_BLACK = 133,
		SL_ALMIGHTNESS_ELEMENTAL = 137,
		SL_ALMIGHTNESS_ELEMENTAL2 = 141,
		SL_ALMIGHTNESS_MENTAL = 139,
		SL_ALMIGHTNESS_WHITE = 135,
		SL_AMOK = 61,
		SL_ARROWTOWER = 96,
		SL_ASSISTANCE = 142,
		SL_AURA = 73,
		SL_BEFRIEND = 93,
		SL_UNKNOWN1 = 7,
		SL_BLIZZARD = 67,
		SL_BRILLIANCE = 57,
		SL_CANNIBALIZE = 146,
		SL_CHAIN = 162,
		SL_CHARISMA = 89,
		SL_CHARM = 95,
		SL_CHARM_ANIMAL = 69,
		SL_CHILL_RESISTANCE = 88,
		SL_CONFUSE = 62,
		SL_CONFUSE_AREA = 156,
		SL_CONSERVATION = 101,
		SL_CURE_DISEASE = 26,
		SL_CURE_POISON = 5,
		SL_DARK_BANISHING = 32,
		SL_DARKNESS_AREA = 148,
		SL_DEATH = 3,
		SL_DEATH_GRASP = 28,
		SL_DECAY = 13,
		SL_DECAY2 = 21,
		SL_DEMORALIZATION = 117,
		SL_DETECT_MAGIC = 30,
		SL_DETECT_METAL = 100,
		SL_DEXTERITY = 80,
		SL_DISENCHANT = 124,
		SL_DISPEL_BLACK_AURA = 83,
		SL_DISPEL_WHITE_AURA = 82,
		SL_DISRUPT = 63,
		SL_DISTRACT = 99,
		SL_DOMINATE = 94,
		SL_DOMINATE_ANIMAL = 84,
		SL_DOMINATE_UNDEAD = 147,
		SL_UNKNOWN2 = 123,
		SL_ENDURANCE = 78,
		SL_ENLIGHTENMENT = 90,
		SL_ESSENCE_BLACK = 132,
		SL_ESSENCE_ELEMENTAL = 136,
		SL_ESSENCE_ELEMENTAL2 = 140,
		SL_ESSENCE_MENTAL = 138,
		SL_ESSENCE_WHITE = 134,
		SL_ETERNITY = 145,
		SL_EXTINCT = 71,
		SL_EXTINCT_TOWER = 129,
		SL_FAKESPELLONEFIGURE = 165,
		SL_FAST_FIGHTING = 79,
		SL_FEET_OF_CLAY = 155,
		SL_FEIGN_DEATH = 122,
		SL_FIREBALL1 = 17,
		SL_FIREBALL2 = 103,
		SL_FIREBULLET = 128,
		SL_FIREBURST = 1,
		SL_FIRE_RESISTANCE = 131,
		SL_FIRESHIELD1 = 18,
		SL_FIRESHIELD2 = 50,
		SL_FLEXIBILITY = 43,
		SL_FLEXIBILITY_AREA = 44,
		SL_FOG = 9,
		SL_FORGET = 55,
		SL_FREEZE = 12,
		SL_FREEZE_AREA = 149,
		SL_GUARD = 47,
		SL_HALLOW = 49,
		SL_HEALING = 2,
		SL_UNKNOWN = 125,
		SL_HEALING_AREA = 38,
		SL_HEALING_AURA = 97,
		SL_HOLY_TOUCH = 143,
		SL_ICE1 = 126,
		SL_ICE2 = 127,
		SL_ICESTRIKE1 = 8,
		SL_ICESTRIKE2 = 98,
		SL_ICESHIELD = 15,
		SL_HYPNOTIZE = 118,
		SL_HYPNOTIZE_AREA = 154,
		SL_ILLUMINATE = 16,
		SL_ICESHIELD2 = 19,
		SL_ICESHIELD3 = 22,
		SL_INABILITY = 76,
		SL_INFLEXIBILITY = 29,
		SL_INFLEXIBILITY_AREA = 34,
		SL_INVISIBILITY = 70,
		SL_INVULNERABILITY = 6,
		SL_LIFETAP = 20,
		SL_LIFETAP_AURA = 102,
		SL_MANADRAIN = 60,
		SL_MANASHIELD = 161,
		SL_MANATAP = 59,
		SL_MANATAP_AURA = 130,
		SL_MEDITATION = 91,
		SL_MELT_RESISTANCE = 87,
		SL_MIRAGE = 164,
		SL_MUTATION = 157,
		SL_PAIN = 14,
		SL_PAIN_AREA = 27,
		SL_UNKNOWN3 = 119,
		SL_PESTILENCE = 25,
		SL_PETRIFY = 24,
		SL_PLAGUE_AREA = 36,
		SL_POISON = 4,
		SL_CLOAK_OF_NOR = 121,
		SL_QUICKNESS = 40,
		SL_QUICKNESS_AREA = 41,
		SL_RAIN_OF_FIRE = 65,
		SL_RAISE_DEAD = 52,
		SL_REGENERATE = 48,
		SL_REINFORCEMENT = 153,
		SL_REMEDILESS = 37,
		SL_REMOVE_CURSE = 72,
		SL_RETENTION = 56,
		SL_REVENGE = 151,
		SL_ROCK_BULLET = 86,
		SL_ROOTS = 163,
		SL_ROOTS_AREA = 144,
		SL_SACRIFICE_MANA = 58,
		SL_SELF_ILLUSION = 104,
		SL_SHIFT_MANA = 150,
		SL_SHOCK = 66,
		SL_SHOCKWAVE = 92,
		SL_SENTINEL_HEALING = 39,
		SL_SLOW_FIGHTING = 75,
		SL_SLOWNESS = 11,
		SL_SLOWNESS_AREA = 33,
		SL_SPARK = 116,
		SL_STONE = 10,
		SL_STONE_RAIN = 68,
		SL_STONE_TOWER = 120,
		SL_STRENGTH = 45,
		SL_STRENGTH_AREA = 46,
		SL_SUFFOCATION = 74,
		SL_SUICIDE_DEATH = 77,
		SL_SUICIDE_HEAL = 81,
		SL_SUMMON = 23,
		SL_THORNSHIELD = 42,
		SL_THORNSHIELD_HIT = 51,
		SL_TORTURE = 152,
		SL_FEEDBACK = 166,
		SL_WAVE = 85,
		SL_WEAKEN = 31,
		SL_WEAKEN_AREA = 35,
		// max = 166?
	};

	struct SF_SpellData
	{
		unsigned char data[76];
	};

	inline unsigned short GetSpellIndex(SF_SpellData& spelldata)
	{
		return ((unsigned short*)spelldata.data)[0];
	}

	inline unsigned short GetSpellType(SF_SpellData& spelldata)
	{
		return ((unsigned short*)spelldata.data)[1];
	}

	inline unsigned int GetSpellParameter(SF_SpellData& spelldata, int index)
	{
		return ((unsigned int*)spelldata.data)[8 + index];
	}
}