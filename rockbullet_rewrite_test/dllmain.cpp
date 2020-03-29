// dllmain.cpp : Defines the entry point for the DLL application.
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <string>

#include "SF_ASI.h"
#include "SF_SpellManager.h"
#include "SF_SpellData.h"
#include "SF_GdFigure.h"
#include "SF_XData.h"
#include "SF_Unknown.h"

int SPELL_MANAGER_HANDLE_SPELLS_ABSOLUTE;

void __stdcall effect_rock_bullet(int spell_index)
{
    // these are important! effect_rock_bullet is a member function of (to be determined), we need ecx to be retrieved before anything else
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto spell_index_copy = spell_index;

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_offset = figure_index * 691;
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);
    auto figure_offset2 = figure_index2 * 691;

    auto val3 = ASI::SpellParam4(spell_manager, spell_index);

    int tmp_sentinel = 0;

    ASI::SF_Unk1 some_val1;

    if (val3 == 1)
    {
        if (figure_index != 0)
        {
            auto figuredata = ASI::GetFigureManager(spell_manager);

            if(ASI::IsAlive(figuredata, figure_index))
            {
                if (ASI::CallClassFunc<0x7FA8F9, unsigned int, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x24), figure_index) != 0)
                {
                    auto spell_id = ASI::SpellID(spell_manager, spell_index);
                    ASI::SF_SpellData spell_data;
                    // get_spell_data
                    ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, spell_id);
                    if(ASI::IsAlive(figuredata, figure_index2))
                    {
                        some_val1 = ASI::GetUnk1(spell_manager, spell_index);
                        tmp_sentinel = 1;
                    }

                    auto val5 = ASI::SpellXData(spell_manager, spell_index);
                    unsigned short figure_index3 = ASI::GetXData(ASI::GetXDataPointer(spell_manager), val5, 45);
                    if ((figure_index3 == 0)||(!ASI::IsAlive(figuredata, figure_index3)))
                        figure_index3 = figure_index2;

                    // calculate_resist_chance
                    unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                    // randint
                    unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x4), 100);
                    if (rand_int <= resist)
                    {
                        ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                            (spell_manager->data, spell_index, figure_index);
                        ASI::Pointer some_pointer1000 = ASI::GetUnknownPointer(spell_manager, 0x4);
                        auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                        ASI::SF_Unk1 params2;
                        ASI::SF_Unk1 params3(1, figure_index, 0, 0);
                        ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
                            (spell_manager->data, spell_index, 11, &some_val1, &params3, some_value1002, 40, &params2);
                    }
                    else
                    {
                        ASI::SF_PositionQuery val6(figuredata[figure_offset + 0x2].ptr);
                        ASI::SF_PositionQuery val7(figuredata[figure_index3*691 + 0x2].ptr);

                        unsigned short val15 = (ASI::FastDistance(val6, val7) * 7) / 25;

                        if ((val15 & 0xFFFF) == 0)
                            val15 = 1;

                        auto spell_damage = ASI::GetSpellParameter(spell_data, 0);
                        // reduce_dmg_by_armor
                        unsigned int reduced_dmg = ASI::CallClassFunc<0x7E3AD0, unsigned int, unsigned int, unsigned int, unsigned int>
                            (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, spell_damage);

                        if(ASI::IsAlive(figuredata, figure_index2))
                        {
                            auto val16 = (unsigned char)figuredata[figure_offset2 + 0x2B4];
                            if (val16 == 8)
                            {
                                // randint
                                unsigned short rand_int2 = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x4), 100);
                                if (rand_int2 <= 25)
                                    reduced_dmg *= 2;
                            }
                        }

                        auto figure_unitid = (unsigned short)figuredata[figure_offset + 0x28];
                        switch (figure_unitid)
                        {
                        case 2236:
                        case 2238:
                        case 2239:
                        case 2244:
                        case 2245:
                        case 2249:
                            reduced_dmg /= 10;
                            break;
                        default: break;
                        }

                        // START MOD
                        auto spelljobstartnode = ASI::GetSpellJobStartNode(figuredata, figure_index);
                        while (spelljobstartnode != 0)
                        {
                            ASI::Pointer some_pointer3(ASI::GetUnknownPointer(spell_manager, 0x10));
                            ASI::Pointer some_pointer31(*some_pointer3[0]);
                            auto some_val2 = ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobstartnode);// (unsigned short)ASI::Pointer(some_pointer31[spelljobstartnode * 6 + 4]);
                            auto spell_type2 = ASI::SpellType(spell_manager, some_val2);
                            if ((spell_type2 == 16) || (spell_type2 == 17))
                            {
                                reduced_dmg = (reduced_dmg * 3) / 2;;
                                break;
                            }
                            spelljobstartnode = ASI::GetSpellJobNextNode(some_pointer31, spelljobstartnode);// (unsigned short)ASI::Pointer((unsigned int)(some_pointer31.ptr) + spelljobstartnode * 6);
                        }
                        // END MOD

                        ASI::Pointer some_pointer2(ASI::GetUnknownPointer(spell_manager, 0x4));
                        auto val17 = (unsigned int)some_pointer2[0xC];
                        ASI::SF_Unk1 params0;
                        ASI::SF_Unk1 params1(1, figure_index, 0, 0);
                        unsigned short param5 = 0;
                        unsigned short param6 = 0;
                        //GdEffect::Add
                        unsigned int result = ASI::CallClassFunc<0x8192D0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
                            (ASI::GetUnknownPointer(spell_manager, 0x14), (unsigned int)3, &some_val1, &params1, val17, val15, &params0);
                        ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 17, spell_index);
                        ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 9, spell_id);
                        ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 30, reduced_dmg);
                        ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 51, 1);
                        ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 47, figure_index2);
                        ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 48, 1);
                        ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 26, figure_index);
                        ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 19, 1);
                        ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 45, figure_index3);
                    }
                }
                else
                    goto end;

            }
            else
                goto end;
        }
        else
            goto end;
    }
    else
        goto end;

end:
    ASI::CallClassProc<(unsigned int)0x7B9820, unsigned int, unsigned int>(spell_manager->data, spell_index, 0);

}



void __stdcall effect_icestrike1(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto spell_index_copy = spell_index;

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_offset = figure_index * 691;
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);
    auto figure_offset2 = figure_index2 * 691;

    auto val3 = ASI::SpellParam4(spell_manager, spell_index);

    if (val3 == 1)
    {
        if (figure_index != 0)
        {
            auto figuredata = ASI::GetFigureManager(spell_manager);

            if(ASI::IsAlive(figuredata, figure_index))
            {
                auto spell_id = ASI::SpellID(spell_manager, spell_index);
                ASI::SF_SpellData spell_data;
                // get_spell_data
                ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, spell_id);

                auto val5 = ASI::SpellXData(spell_manager, spell_index);
                unsigned short tick_count = ASI::GetXData(ASI::GetXDataPointer(spell_manager), val5, 18);
                if (tick_count == 0)        // first ticck
                {
                    ASI::Pointer some_pointer1000 = ASI::GetUnknownPointer(spell_manager, 0x4);
                    auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                    ASI::SF_Unk1 some_val1 = ASI::GetUnk1(spell_manager, spell_index);
                    ASI::SF_Unk1 params0;
                    ASI::SF_Unk1 params1(1, figure_index, 0, 0);
                    // calculate_resist_chance
                    unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                    // randint
                    unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x4), 100);

                    if (rand_int <= resist)
                    {
                        ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                            (spell_manager->data, spell_index, figure_index);

                        ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
                            (spell_manager->data, spell_index, 11, &some_val1, &params1, some_value1002, 10, &params0);
                            
                        goto end;
                    }
                    else
                    {
                        ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
                            (spell_manager->data, spell_index, 3, &some_val1, &params1, some_value1002, ASI::GetSpellParameter(spell_data, 2) / 100, &params0);


                        unsigned int tmp_val1002 = 1;
                        ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
                            (spell_manager->data, spell_index, 4, &some_val1, &params1, some_value1002, 25, &params0);

                        auto dmg = (unsigned short)ASI::GetSpellParameter(spell_data, 0);

                        ASI::SetXData(ASI::GetXDataPointer(spell_manager), val5, 5, (ASI::GetSpellParameter(spell_data, 2) / ASI::GetSpellParameter(spell_data, 1))-1);
                        ASI::SetXData(ASI::GetXDataPointer(spell_manager), val5, 18, dmg);
                            
                        if(ASI::IsAlive(figuredata, figure_index2))
                        {
                            auto val16 = (unsigned char)figuredata[figure_offset2 + 0x2B4];
                            if (val16 == 8)
                            {
                                // randint
                                unsigned short rand_int2 = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x4), 100);
                                if (rand_int2 <= 25)
                                {
                                    ASI::SetXData(ASI::GetXDataPointer(spell_manager), val5, 38, 1);
                                    dmg *= 2;
                                }
                            }
                        }

                        ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                            (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index2, figure_index, dmg, 1, 0, 0);

                        if(ASI::IsAlive(figuredata, figure_index))
                        {
                            figuredata[figure_offset + 0x16].AsRef<unsigned int>() |= 0x120;
                            ASI::SpellParam6(spell_manager, spell_index) |= 0x3;
                            unsigned char result = ASI::CallClassFunc<0x740700, unsigned char>(figuredata[figure_offset + 0x20B]);
                            if ((result >= 9)&&(result <= 12))
                            {
                                unsigned short result2 = ASI::CallClassFunc<0x7406A0, unsigned short>(figuredata[figure_offset + 0x20B]);
                                if (ASI::CallClassFunc<0x815FB0, unsigned int, unsigned int>
                                        (ASI::GetUnknownPointer(spell_manager, 0x20), result2) != 0)
                                    ASI::CallClassProc<0x406EE0, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                                        (ASI::GetUnknownPointer(spell_manager, 0x20), figure_index, 3, 0, 0, 0);
                            }

                            ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 1) / 100;
                            return;
                        }
                    }
                }
                else
                {
                    // START MOD
                    ASI::SpellParam6(spell_manager, spell_index) &= 0xFD;
                    unsigned int some_val1003 = ASI::SpellParam4(spell_manager, spell_index);
                    ASI::CallClassProc<0x7B9550, unsigned int, unsigned int*>(spell_manager->data, spell_index, &some_val1003);
                    ASI::CallClassProc<0x7B9660, unsigned int, unsigned int*>(spell_manager->data, spell_index, 0);

                    goto end;
                    // END MOD
                        
                    // MOD START

                    /*ASI::Pointer some_pointer1000 = ASI::GetUnknownPointer(spell_manager, 0x4);
                    auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                    ASI::SF_Unk1 some_val1 = ASI::GetUnk1(spell_manager, spell_index);
                    ASI::SF_Unk1 params0;
                    ASI::SF_Unk1 params1(1, figure_index, 0, 0);
                    ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
                        (spell_manager->data, spell_index, 4, &some_val1, &params1, some_value1002, 25, &params0);

                    unsigned short some_xdata1 = ASI::GetXData(ASI::GetXDataPointer(spell_manager), val5, 5);
                    if (some_xdata1 != 0)
                        ASI::SubXData(ASI::GetXDataPointer(spell_manager), val5, 5, 1);
                    else
                    {
                        ASI::SpellParam6(spell_manager, spell_index) &= 0xFD;
                        unsigned int some_val1003 = ASI::SpellParam4(spell_manager, spell_index);
                        ASI::CallClassProc<0x7B9550, unsigned int, unsigned int*>(spell_manager->data, spell_index, &some_val1003);
                        ASI::CallClassProc<0x7B9660, unsigned int, unsigned int*>(spell_manager->data, spell_index, 0);
                    }

                    int remove_effect_01 = 0;
                    unsigned int cur_dmg = ASI::SubXData(ASI::GetXDataPointer(spell_manager), val5, 18, 1);
                    if(cur_dmg == 0)
                        remove_effect_01 = 1;

                    ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 1) / 100;

                    unsigned short some_xdata2 = ASI::GetXData(ASI::GetXDataPointer(spell_manager), val5, 38);
                    if (some_xdata2 == 1)
                        cur_dmg *= 2;

                    ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index2, figure_index, cur_dmg, 1, 0, 1);

                    if (remove_effect_01 == 1)
                        goto end;
                    else
                        return;*/

                    // MOD END
                }
            }
        }
    }
    end:
        ASI::CallClassProc<0x7B9820, unsigned int, unsigned int>(spell_manager->data, spell_index, 0);
        return;
}

void __stdcall effect_death(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto spell_index_copy = spell_index;

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_offset = figure_index * 691;
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);
    auto figure_offset2 = figure_index2 * 691;

    auto val3 = ASI::SpellParam4(spell_manager, spell_index);

    if (val3 == 1)
    {
        if (figure_index != 0)
        {
            auto figuredata = ASI::GetFigureManager(spell_manager);

            if (ASI::IsAlive(figuredata, figure_index))
            {
                auto spell_id = ASI::SpellID(spell_manager, spell_index);
                ASI::SF_SpellData spell_data;
                // get_spell_data
                ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, spell_id);

                // calculate_resist_chance
                unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                    (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                // randint
                unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                    (ASI::GetUnknownPointer(spell_manager, 0x4), 100);

                if (rand_int <= resist)
                {
                    ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                        (spell_manager->data, spell_index, figure_index);
                    ASI::Pointer some_pointer1000 = ASI::GetUnknownPointer(spell_manager, 0x4);
                    auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                    ASI::SF_Unk1 some_val1;
                    ASI::SF_Unk1 params2;
                    ASI::SF_Unk1 params3(1, figure_index, 0, 0);
                    ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
                        (spell_manager->data, spell_index, 11, &some_val1, &params3, some_value1002, 40, &params2);

                    goto end;
                }
                else
                {
                    ASI::Pointer some_pointer1000 = ASI::GetUnknownPointer(spell_manager, 0x4);
                    auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                    ASI::SF_Unk1 some_val1 = ASI::GetUnk1(spell_manager, spell_index);
                    ASI::SF_Unk1 params0;
                    ASI::SF_Unk1 params1(1, figure_index, 0, 0);
                    ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
                        (spell_manager->data, spell_index, 3, &some_val1, &params1, some_value1002, 25, &params0);

                    auto dmg = (unsigned short)ASI::GetSpellParameter(spell_data, 0);
                    // START MOD
                    auto ratio = 100 + (100*(ASI::GetCurrentHealthMax(figuredata, figure_index)-(ASI::GetCurrentHealth(figuredata, figure_index))) / ASI::GetCurrentHealthMax(figuredata, figure_index));
                    dmg = (dmg * ratio) / 100;
                    // END MOD

                    if (ASI::IsAlive(figuredata, figure_index2))
                    {
                        auto val16 = (unsigned char)figuredata[figure_offset2 + 0x2B4];
                        if (val16 == 8)
                        {
                            // randint
                            unsigned short rand_int2 = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x4), 100);
                            if (rand_int2 <= 25)
                                dmg *= 2;
                        }
                    }

                    ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index2, figure_index, dmg, 1, 0, 0);

                    goto end;
                }
            }
        }
    }
    end:
        ASI::CallClassProc<0x7B9820, unsigned int, unsigned int>(spell_manager->data, spell_index, 0);
        return;
}

void __stdcall effect_fireburst(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto spell_index_copy = spell_index;

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_offset = figure_index * 691;
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);
    auto figure_offset2 = figure_index2 * 691;

    auto val3 = ASI::SpellParam4(spell_manager, spell_index);

    if (val3 == 1)
    {
        if (figure_index != 0)
        {
            auto figuredata = ASI::GetFigureManager(spell_manager);

            if (ASI::IsAlive(figuredata, figure_index))
            {
                auto spell_id = ASI::SpellID(spell_manager, spell_index);
                ASI::SF_SpellData spell_data;
                // get_spell_data
                ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, spell_id);

                auto val5 = ASI::SpellXData(spell_manager, spell_index);
                unsigned short tick_count = ASI::GetXData(ASI::GetXDataPointer(spell_manager), val5, 18);
                if (tick_count == 0)        // first ticck
                {
                    ASI::Pointer some_pointer1000 = ASI::GetUnknownPointer(spell_manager, 0x4);
                    auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                    unsigned int tmp_val1001 = 1;
                    ASI::SF_Unk1 some_val1 = ASI::GetUnk1(spell_manager, spell_index);
                    ASI::SF_Unk1 params0;
                    ASI::SF_Unk1 params1(1, figure_index, 0, 0);
                    // calculate_resist_chance
                    unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                    // randint
                    unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x4), 100);

                    if (rand_int <= resist)
                    {
                        ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                            (spell_manager->data, spell_index, figure_index);

                        ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
                            (spell_manager->data, spell_index, 11, &some_val1, &params1, some_value1002, 10, &params0);

                        goto end;
                    }
                    else
                    {
                        // START MOD
                        int dmg_mul = 0;
                        if (ASI::HasEffectExcept(spell_manager, figure_index, 1, spell_index) != -1)
                            dmg_mul = 1;
                        // END MOD

                        ASI::SF_Unk1 params3 = ASI::GetUnk1(spell_manager, spell_index);
                        ASI::SF_Unk1 params2;
                        ASI::CallClassProc<0x649670, ASI::SF_Unk1*, ASI::SF_Unk1*, ASI::SF_Unk1*>
                            (ASI::GetUnknownPointer(spell_manager, 0x44), &params2, &params3, &params1);


                        ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1 *, unsigned int, unsigned int, ASI::SF_Unk1*>
                            (spell_manager->data, spell_index, 3, &some_val1, &params1, some_value1002, 10, &params0);


                        ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
                            (spell_manager->data, spell_index, 4, &some_val1, &params1, some_value1002, 25, &params0);

                        auto dmg = (unsigned short)ASI::GetSpellParameter(spell_data, 0);

                        ASI::AddXData(ASI::GetXDataPointer(spell_manager), val5, 18, 1);

                        ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 3) / 100;

                        if (ASI::IsAlive(figuredata, figure_index2))
                        {
                            auto val16 = (unsigned char)figuredata[figure_offset2 + 0x2B4];
                            if (val16 == 8)
                            {
                                // randint
                                unsigned short rand_int2 = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x4), 100);
                                if (rand_int2 <= 25)
                                {
                                    ASI::SetXData(ASI::GetXDataPointer(spell_manager), val5, 38, 1);
                                    dmg *= 2;
                                }
                            }
                        }

                        // START MOD
                        if (dmg_mul)
                            dmg = (dmg * 6) / 5;
                        // END MOD

                        ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                            (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index2, figure_index, dmg, 1, 0, 0);

                        return;
                    }
                }
                else
                {
                    ASI::Pointer some_pointer1000 = ASI::GetUnknownPointer(spell_manager, 0x4);
                    auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                    ASI::SF_Unk1 some_val1 = ASI::GetUnk1(spell_manager, spell_index);
                    ASI::SF_Unk1 params0;
                    ASI::SF_Unk1 params1(1, figure_index, 0, 0);
                    ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
                        (spell_manager->data, spell_index, 4, &some_val1, &params1, some_value1002, 25, &params0);

                    int remove_effect = 0;
                    if (tick_count >= ASI::GetSpellParameter(spell_data, 2))
                        remove_effect = 1;
                    else
                    {
                        ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 3) / 100;
                        ASI::AddXData(ASI::GetXDataPointer(spell_manager), val5, 18, 1);
                    }

                    unsigned short cur_dmg = ASI::GetSpellParameter(spell_data, 1);
                    unsigned short some_xdata2 = ASI::GetXData(ASI::GetXDataPointer(spell_manager), val5, 38);
                    if (some_xdata2 == 1)
                        cur_dmg *= 2;

                    ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index2, figure_index, cur_dmg, 1, 0, 1);

                    if (remove_effect)
                        goto end;
                    else
                        return;
                }
            }
        }
    }
end:
    ASI::CallClassProc<0x7B9820, unsigned int, unsigned int>(spell_manager->data, spell_index, 0);
    return;
}

void _stdcall effect_fireshield2(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto spell_index_copy = spell_index;

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_offset = figure_index * 691;
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);
    auto figure_offset2 = figure_index2 * 691;

    auto val3 = ASI::SpellParam4(spell_manager, spell_index);

    if (val3 == 1)
    {
        if (figure_index != 0)
        {
            auto figuredata = ASI::GetFigureManager(spell_manager);

            if (ASI::IsAlive(figuredata, figure_index))
            {
                auto spell_id = ASI::SpellID(spell_manager, spell_index);
                ASI::SF_SpellData spell_data;
                // get_spell_data
                ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, spell_id);

                // calculate_resist_chance
                unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                    (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                // randint
                unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                    (ASI::GetUnknownPointer(spell_manager, 0x4), 100);

                if (rand_int <= resist)
                {
                    ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                        (spell_manager->data, spell_index, figure_index);
                    ASI::Pointer some_pointer1000 = ASI::GetUnknownPointer(spell_manager, 0x4);
                    auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                    ASI::SF_Unk1 some_val1;
                    ASI::SF_Unk1 params2;
                    ASI::SF_Unk1 params3(1, figure_index, 0, 0);
                    ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
                        (spell_manager->data, spell_index, 11, &some_val1, &params3, some_value1002, 10, &params2);

                    goto end;
                }
                else
                {
                    ASI::Pointer some_pointer1000 = ASI::GetUnknownPointer(spell_manager, 0x4);
                    auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                    ASI::SF_Unk1 some_val1 = ASI::GetUnk1(spell_manager, spell_index);
                    ASI::SF_Unk1 params0;
                    ASI::SF_Unk1 params1(1, figure_index, 0, 0);
                    ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
                        (spell_manager->data, spell_index, 3, &some_val1, &params1, some_value1002, 25, &params0);

                    auto dmg = (unsigned short)ASI::GetSpellParameter(spell_data, 0);
                    // START MOD
                    if (ASI::HasEffect(spell_manager, figure_index, 1) != -1)
                        dmg = (dmg * 18) / 10;
                    // END MOD

                    if (ASI::IsAlive(figuredata, figure_index2))
                    {
                        auto val16 = (unsigned char)figuredata[figure_offset2 + 0x2B4];
                        if (val16 == 8)
                        {
                            // randint
                            unsigned short rand_int2 = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x4), 100);
                            if (rand_int2 <= 25)
                                dmg *= 2;
                        }
                    }

                    ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index2, figure_index, dmg, 1, 0, 0);

                    goto end;
                }
            }
        }
    }
end:
    ASI::CallClassProc<0x7B9820, unsigned int, unsigned int>(spell_manager->data, spell_index, 0);
    return;
}


void __stdcall effect_healing(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto spell_index_copy = spell_index;

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_offset = figure_index * 691;
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);
    auto figure_offset2 = figure_index2 * 691;

    auto val3 = ASI::SpellParam4(spell_manager, spell_index);

    if (val3 == 1)
    {
        if (figure_index != 0)
        {
            auto figuredata = ASI::GetFigureManager(spell_manager);

            if (ASI::IsAlive(figuredata, figure_index))
            {
                auto spell_id = ASI::SpellID(spell_manager, spell_index);
                ASI::SF_SpellData spell_data;
                // get_spell_data
                ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, spell_id);

                auto charisma = ASI::GetCurrentCharisma(figuredata, figure_index2);
                auto heal = (ASI::GetSpellParameter(spell_data, 0) * (charisma + 15)) / 50;

                if (ASI::IsAlive(figuredata, figure_index2))
                {
                    auto val16 = (unsigned char)figuredata[figure_offset2 + 0x2B4];
                    if (val16 == 9)
                    {
                        // randint
                        unsigned short rand_int2 = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x4), 100);
                        if (rand_int2 <= 25)
                            heal *= 2;
                    }
                }

                auto remediless_index = ASI::HasEffect(spell_manager, figure_index, 41);
                if (remediless_index == -1)
                {
                    ASI::Pointer some_pointer1000 = ASI::GetUnknownPointer(spell_manager, 0x4);
                    auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                    ASI::SF_Unk1 some_val1 = ASI::GetUnk1(spell_manager, spell_index);
                    ASI::SF_Unk1 params0;
                    ASI::SF_Unk1 params1(1, figure_index, 0, 0);
                    ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
                        (spell_manager->data, spell_index, 3, &some_val1, &params1, some_value1002, 35, &params0);

                    if (heal > 32766)
                        heal = 32766;

                    short neg_heal = -heal;
                    ASI::CallClassProc<0x66B230, unsigned short, short>(figuredata, figure_index, neg_heal);
                }
                // START MOD
                else
                {
                    //ASI::Pointer some_pointer2(class_pointer[remediless_index * 27]);
                    auto remediless_id = ASI::SpellID(spell_manager, remediless_index);
                    ASI::SF_SpellData remediless_spell_data;
                    // get_spell_data
                    ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x3C), &remediless_spell_data, remediless_id);

                    auto reflect_back = (heal * ASI::GetSpellParameter(remediless_spell_data, 1)) / 100;
                    ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index, ASI::SpellTarget(spell_manager, remediless_index), reflect_back, 1, 0, 0);   // THIS COULD CRASH THE GAME POTENTIALLY
                }

                // END MOD
            }
        }
    }
end:
    ASI::CallClassProc<0x7B9820, unsigned int, unsigned int>(spell_manager->data, spell_index, 0);
    return;
}

void _stdcall effect_petrify(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_offset = figure_index * 691;
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);
    auto figure_offset2 = figure_index2 * 691;

    if (ASI::SpellParam4(spell_manager, spell_index) == 1)
    {
        if (figure_index != 0)
        {
            auto figuredata = ASI::GetFigureManager(spell_manager);

            if (ASI::IsAlive(figuredata, figure_index))
            {
                if (ASI::CallClassFunc<0x7FA8F9, unsigned int, unsigned short>
                    (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index) != 0)
                {
                    ASI::SF_SpellData spell_data;
                    // get_spell_data
                    ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, ASI::SpellID(spell_manager, spell_index));

                    auto spell_xdata = ASI::SpellXData(spell_manager, spell_index);       // byte ptr one time, word ptr another
                    if (ASI::AddXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5, 1) == 1)
                    {
                        auto target_level = (unsigned char)figuredata[figure_offset + 0x1B];
                        int chance = 0;
                        if (target_level <= ASI::GetSpellParameter(spell_data, 0))
                            chance = 100;
                        else if (target_level <= ASI::GetSpellParameter(spell_data, 1))
                            chance = 50;
                        else if (target_level <= ASI::GetSpellParameter(spell_data, 2))
                            chance = 25;

                        ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                            (spell_manager->data, spell_index, figure_index);

                        unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                            (ASI::GetUnknownPointer(spell_manager, 0x4), 100);

                        if (rand_int <= chance)
                        {
                            if (ASI::CallClassFunc<0x7B8E60, unsigned int, unsigned short>
                                (spell_manager->data, spell_index) != 0)
                            {
                                ASI::Pointer some_pointer1000 = ASI::GetUnknownPointer(spell_manager, 0x4);
                                auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                                ASI::SF_Unk1 some_val1 = ASI::GetUnk1(spell_manager, spell_index);
                                ASI::SF_Unk1 params0;
                                ASI::SF_Unk1 params1(1, figure_index, 0, 0);
                                ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
                                    (spell_manager->data, spell_index, 3, &some_val1, &params1, some_value1002, (ASI::GetSpellParameter(spell_data, 3) * ASI::GetSpellParameter(spell_data, 4)) / 100, &params0);

                                figuredata[figure_offset + 0x16].AsRef<unsigned int>() |= 0x140;
                                ASI::SpellParam6(spell_manager, spell_index) |= 0x5;
                                unsigned char result = ASI::CallClassFunc<0x740700, unsigned char>(figuredata[figure_offset + 0x20B]);
                                if ((result >= 9) && (result <= 12))
                                {
                                    unsigned short result2 = ASI::CallClassFunc<0x7406A0, unsigned short>(figuredata[figure_offset + 0x20B]);
                                    if (ASI::CallClassFunc<0x815FB0, unsigned int, unsigned int>
                                        (ASI::GetUnknownPointer(spell_manager, 0x20), result2) != 0)
                                        ASI::CallClassProc<0x406EE0, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                                        (ASI::GetUnknownPointer(spell_manager, 0x20), figure_index, 3, 0, 0, 0);
                                }
                                ASI::SpellToDoCount(spell_manager, spell_index) = (ASI::GetSpellParameter(spell_data, 3) * ASI::GetSpellParameter(spell_data, 4)) / 100;
                                return;
                            }
                            else
                                goto end;
                        }
                        else
                        {
                            ASI::Pointer some_pointer1000 = ASI::GetUnknownPointer(spell_manager, 0x4);
                            auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                            ASI::SF_Unk1 some_val1 = ASI::GetUnk1(spell_manager, spell_index);
                            ASI::SF_Unk1 params2;
                            ASI::SF_Unk1 params3(1, figure_index, 0, 0);
                            ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
                                (spell_manager->data, spell_index, 11, &some_val1, &params3, some_value1002, 10, &params2);

                            goto end;
                        }
                    }
                    else
                    {
                        ASI::CallClassProc<0x7B9660, unsigned int, unsigned int*>(spell_manager->data, spell_index, 0);
                        ASI::CallClassProc<0x7B95D0, unsigned int, unsigned int*>(spell_manager->data, spell_index, 0);
                        ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>(spell_manager->data, spell_index, figure_index);
                        goto end;
                    }
                }
                else return;
            }
            else return;
        }
        else return;
    }

end:
    ASI::CallClassProc<0x7B9820, unsigned int, unsigned int>(spell_manager->data, spell_index, 0);
    return;
}

void __stdcall effect_ability_benefactions(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);
    auto figure_offset2 = figure_index2 * 691;

    ASI::AreaEntityQuery area;

    ASI::CallClassProc<0x7A2120, short, short, short, short>
        (area.data, 0, 0, 1023, 1023);

    if (ASI::SpellParam4(spell_manager, spell_index) == 1)
    {
        ASI::SF_SpellData spell_data;
        // get_spell_data
        ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>
            (ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, ASI::SpellID(spell_manager, spell_index));

        auto figuredata = ASI::GetFigureManager(spell_manager);

        auto remediless_index = ASI::HasEffect(spell_manager, figure_index2, 41);
        if (remediless_index == -1)
        {
            if (ASI::GetCurrentHealthPercentLeft(figuredata, figure_index2) < 100)
            {
                short heal = (ASI::GetCurrentHealthMax(figuredata, figure_index2) * ASI::GetSpellParameter(spell_data, 0)) / 100;
                if (heal > ASI::GetCurrentHealthMissing(figuredata, figure_index2))
                    heal = ASI::GetCurrentHealthMissing(figuredata, figure_index2);

                ASI::Pointer some_pointer1000 = ASI::GetUnknownPointer(spell_manager, 0x4);
                auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                ASI::SF_Unk1 some_val1 = ASI::GetUnk1(spell_manager, spell_index);
                ASI::SF_Unk1 params2;
                ASI::SF_Unk1 params3(1, figure_index2, 0, 0);
                ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
                    (spell_manager->data, spell_index, 3, &some_val1, &params3, some_value1002, 10, &params2);

                ASI::CallClassProc<0x66B230, unsigned short, short>(figuredata, figure_index2, -heal);
            }
        }

        // here's the difficult part :)
        ASI::CallClassProc<0x7A23E0, ASI::Pointer, ASI::Pointer, ASI::Pointer>
            (area.data, figuredata, ASI::GetUnknownPointer(spell_manager, 0x40), ASI::GetUnknownPointer(spell_manager, 0x48));

        auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
        auto figure_offset = figure_index * 691;

        auto figure_pos = (unsigned int)figuredata[figure_index * 691 + 0x2];

        ASI::CallClassProc<0x7A19E0, unsigned int*, int>
            (area.data, &figure_pos, ASI::GetSpellParameter(spell_data, 2));  // area radius

        int figures_affected = 0;

        auto figure_index3 = ASI::CallClassFunc<0x7A2250, unsigned short>
            (area.data);

        while (figure_index3 != 0)
        {
            if (ASI::IsAlive(figuredata, figure_index3))
            {
                if (figure_index3 != figure_index2)
                {
                    if (ASI::CallClassFunc<0x7FAC31, int, unsigned short, unsigned short>
                        (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index3, figure_index2) != 0)
                    {
                        remediless_index = ASI::HasEffect(spell_manager, figure_index3, 41);
                        if (remediless_index == -1)
                        {
                            figures_affected += 1;

                            if (ASI::GetCurrentHealthPercentLeft(figuredata, figure_index3) < 100)
                            {
                                short heal = (ASI::GetCurrentHealthMax(figuredata, figure_index3) * ASI::GetSpellParameter(spell_data, 0)) / 100;
                                if (heal > ASI::GetCurrentHealthMissing(figuredata, figure_index3))
                                    heal = ASI::GetCurrentHealthMissing(figuredata, figure_index3);

                                ASI::Pointer some_pointer1000 = ASI::GetUnknownPointer(spell_manager, 0x4);
                                auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                                ASI::SF_Unk1 some_val1 = ASI::GetUnk1(spell_manager, spell_index);
                                ASI::SF_Unk1 params2;
                                ASI::SF_Unk1 params3(1, figure_index3, 0, 0);
                                ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
                                    (spell_manager->data, spell_index, 3, &some_val1, &params3, some_value1002, 10, &params2);

                                ASI::CallClassProc<0x66B230, unsigned short, short>(figuredata, figure_index3, -heal);
                            }

                            if (figures_affected == ASI::GetSpellParameter(spell_data, 1))
                                break;
                        }
                    }
                }
            }

            figure_index3 = ASI::CallClassFunc<0x7A2250, unsigned short>
                (area.data);
        }
    }

    ASI::Pointer unk_ptr(*ASI::Pointer(area.data + 0x34));
    ASI::CallClassProc<0x7B9820, unsigned int, unsigned int>(spell_manager->data, spell_index, 0);
    if (((unsigned int*)(area.data))[0xE] != 0)
    {
        ASI::Pointer unk_ptr2(*(unk_ptr[0x4]));
        while ((unk_ptr2.ptr != 0))
        {
            ASI::CallClassProc<0x624DD0, unsigned int>
                (area.data + 0x34, (unsigned int)unk_ptr2[0xC]);
            ASI::CallClassProc<0x624D90, unsigned int, unsigned int>
                (area.data + 0x34, (unsigned int)unk_ptr2.ptr, 1);
            unk_ptr2 = *unk_ptr2[0x8];
        }

        unk_ptr[0x8].AsRef<unsigned int>() = (unsigned int)(unk_ptr.ptr);
        unk_ptr[0x4].AsRef<unsigned int>() = 0;
        unk_ptr[0xC].AsRef<unsigned int>() = (unsigned int)(unk_ptr.ptr);
        ASI::Pointer(area.data + 0x38).AsRef<unsigned int>() = 0;
    }

    if (unk_ptr.ptr != 0)
        ASI::CallGlobalProc<0x405B20, void*, int>(unk_ptr.ptr, 0x14);
}


void __stdcall effect_ability_berserk(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);
    auto figure_offset2 = figure_index2 * 691;

    if (ASI::SpellParam4(spell_manager, spell_index) == 1)
    {
        auto figuredata = ASI::GetFigureManager(spell_manager);

        ASI::SF_SpellData spell_data;
        // get_spell_data
        ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>
            (ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, ASI::SpellID(spell_manager, spell_index));

        if (figure_index2 != 0)
        {
            if (ASI::IsAlive(figuredata, figure_index2))
            {
                auto spell_xdata = ASI::SpellXData(spell_manager, spell_index);       // byte ptr one time, word ptr another
                if (ASI::AddXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5, 1) == 1)
                {
                    ASI::Pointer some_pointer1000 = ASI::GetUnknownPointer(spell_manager, 0x4);
                    auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                    ASI::SF_Unk1 some_val1 = ASI::GetUnk1(spell_manager, spell_index);
                    ASI::SF_Unk1 params2;
                    ASI::SF_Unk1 params3(1, figure_index2, 0, 0);

                    ASI::SF_Unk1 params4;
                    ASI::CallClassProc<0x7B8C00, ASI::SF_Unk1*, int, unsigned int, ASI::SF_Unk1*>
                        (spell_manager->data, &params2, spell_index, ASI::GetSpellParameter(spell_data, 3), &params4);

                    ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
                        (spell_manager->data, spell_index, 3, &some_val1, &params3, some_value1002, 150, &params2);

                    ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 0) / 100;
                    ASI::SpellParam6(spell_manager, spell_index) |= 0x2;
                    figuredata[figure_offset2 + 0x16].AsRef<unsigned int>() |= 0x20;

                    return;
                }
                else
                {
                    if (ASI::IsAlive(figuredata, figure_index2))
                    {
                        auto spelljobnode = ASI::GetSpellJobStartNode(figuredata, figure_index2);
                        if (spelljobnode != 0)
                        {
                            ASI::SpellParam6(spell_manager, spell_index) &= 0xFD;
                            if (ASI::SpellParam4(spell_manager, spell_index) == 1)
                            {
                                spelljobnode = ASI::GetSpellJobStartNode(figuredata, figure_index2);
                                if (spelljobnode == 0)
                                {
                                    ASI::RemoveFlags(figuredata, figure_index2, 0x20);
                                    goto end;
                                }
                            }

                            while (true)
                            {
                                ASI::Pointer some_pointer3(ASI::GetUnknownPointer(spell_manager, 0x10));
                                ASI::Pointer some_pointer31(*some_pointer3[0]);

                                if (ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode) != spell_index)
                                {
                                    if (ASI::SpellParam6(spell_manager, spell_index) != 2)
                                        break;
                                }

                                spelljobnode = ASI::GetSpellJobNextNode(some_pointer31, spelljobnode);
                                if (spelljobnode == 0)
                                {
                                    ASI::RemoveFlags(figuredata, figure_index2, 0x20);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
end:
    ASI::CallClassProc<0x7B9820, unsigned int, unsigned int>(spell_manager->data, spell_index, 0);
    return;
}



void __stdcall spell_manager_handle_spells()
{
    // important
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned int spell_index = 1;
    if (ASI::SpellCount(spell_manager) < spell_index)
        return;

    while (true)
    {
        if (ASI::SpellID(spell_manager, spell_index) != 0)
        {
            ASI::SpellToDoCount(spell_manager, spell_index) -= 1;
            if (ASI::SpellToDoCount(spell_manager, spell_index) < 0)
                ASI::SpellToDoCount(spell_manager, spell_index) = 0;

            if (ASI::SpellToDoCount(spell_manager, spell_index) == 0)
            {
                switch (ASI::SpellLine(spell_manager, spell_index))
                {
                case ASI::SL_ABILITY_BENEFACTIONS: ASI::CallAsClassProc<unsigned int>(effect_ability_benefactions, spell_manager, spell_index); break;//ASI::CallClassProc<0x7BC9D0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ABILITY_BERSERK: ASI::CallAsClassProc<unsigned int>(effect_ability_berserk, spell_manager, spell_index); break;//ASI::CallClassProc<0x7BD040, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ABILITY_BOONS: ASI::CallClassProc<0x7BD2B0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ABILITY_CRITICAL_HITS: ASI::CallClassProc<0x7BD4C0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ABILITY_DURABILITY: ASI::CallClassProc<0x7BD710, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ABILITY_ENDURANCE: ASI::CallClassProc<0x7BD980, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ABILITY_PATRONIZE: ASI::CallClassProc<0x7BDE40, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ABILITY_RIPOSTE: ASI::CallClassProc<0x7BE300, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ABILITY_SALVO: ASI::CallClassProc<0x7BE550, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ABILITY_SHELTER: ASI::CallClassProc<0x7BE7C0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ABILITY_SHIFT_LIFE: ASI::CallClassProc<0x7BEA30, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ABILITY_STEELSKIN: ASI::CallClassProc<0x7BEF30, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ABILITY_TRUESHOT: ASI::CallClassProc<0x7BF1A0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ABILITY_WARCRY: ASI::CallClassProc<0x7BF410, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ACID_CLOUD: ASI::CallClassProc<0x7BF8D0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ALMIGHTNESS_BLACK: ASI::CallClassProc<0x7BFD60, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ALMIGHTNESS_ELEMENTAL : ASI::CallClassProc<0x7C0020, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ALMIGHTNESS_ELEMENTAL2: ASI::CallClassProc<0x7C0310, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ALMIGHTNESS_MENTAL: ASI::CallClassProc<0x7C06F0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ALMIGHTNESS_WHITE: ASI::CallClassProc<0x7C09F0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_AMOK: ASI::CallClassProc<0x7C0CB0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ARROWTOWER: ASI::CallClassProc<0x7C0FA0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_DARKNESS_AREA: ASI::CallClassProc<0x7C1350, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ASSISTANCE: ASI::CallClassProc<0x7C17F0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_BEFRIEND: ASI::CallClassProc<0x7C1A10, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_AURA: ASI::CallClassProc<0x7C1C50, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_UNKNOWN1: ASI::CallClassProc<0x7C2AD0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_BLIZZARD: ASI::CallClassProc<0x7C2D40, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_BRILLIANCE: ASI::CallClassProc<0x7C30A0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_CANNIBALIZE: ASI::CallClassProc<0x7C32B0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_CHARISMA: ASI::CallClassProc<0x7C3430, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_CHAIN: ASI::CallClassProc<0x7C3620, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_CHARM: ASI::CallClassProc<0x7C3F30, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_CHARM_ANIMAL: ASI::CallClassProc<0x7C4590, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_CHILL_RESISTANCE: ASI::CallClassProc<0x7C4C20, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_CONFUSE: ASI::CallClassProc<0x7C4EA0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_CONFUSE_AREA: ASI::CallClassProc<0x7C5200, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_CONSERVATION: ASI::CallClassProc<0x7C5840, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_CURE_DISEASE: ASI::CallClassProc<0x7C5B90, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_CURE_POISON: ASI::CallClassProc<0x7C5DC0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_DARK_BANISHING: ASI::CallClassProc<0x7C5FF0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_DEATH: ASI::CallAsClassProc<unsigned int>(effect_death, spell_manager, spell_index); break;
                case ASI::SL_DEATH_GRASP: ASI::CallClassProc<0x7C64D0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_DECAY: ASI::CallClassProc<0x7C6710, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_DECAY2: ASI::CallClassProc<0x7C6980, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_DEMORALIZATION: ASI::CallClassProc<0x7C6E30, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_DETECT_MAGIC: ASI::CallClassProc<0x7C7120, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_DETECT_METAL: ASI::CallClassProc<0x7C7440, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_DEXTERITY: ASI::CallClassProc<0x7C7750, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_DISENCHANT: ASI::CallClassProc<0x7C79B0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_DISPEL_BLACK_AURA: ASI::CallClassProc<0x7C7C60, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_DISPEL_WHITE_AURA: ASI::CallClassProc<0x7C7F50, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_DISRUPT: ASI::CallClassProc<0x7C8250, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_DISTRACT: ASI::CallClassProc<0x7C85A0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_DOMINATE: ASI::CallClassProc<0x7C8940, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_DOMINATE_ANIMAL: ASI::CallClassProc<0x7C9010, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_DOMINATE_UNDEAD: ASI::CallClassProc<0x7C9770, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_UNKNOWN2: ASI::CallClassProc<0x7C9E40, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ENDURANCE: ASI::CallClassProc<0x7C9FE0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ENLIGHTENMENT: ASI::CallClassProc<0x7CA290, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ESSENCE_BLACK: ASI::CallClassProc<0x7CA600, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ESSENCE_ELEMENTAL: ASI::CallClassProc<0x7CA8C0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ESSENCE_ELEMENTAL2: ASI::CallClassProc<0x7CABB0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ESSENCE_MENTAL: ASI::CallClassProc<0x7CAF20, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ESSENCE_WHITE: ASI::CallClassProc<0x7CB220, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ETERNITY: ASI::CallClassProc<0x7CB4E0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_EXTINCT: ASI::CallClassProc<0x7CB920, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_EXTINCT_TOWER: ASI::CallClassProc<0x7CBDC0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FAKESPELLONEFIGURE: ASI::CallClassProc<0x7CC280, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FAST_FIGHTING: ASI::CallClassProc<0x7CC380, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FEET_OF_CLAY: ASI::CallClassProc<0x7CC5E0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FEIGN_DEATH: ASI::CallClassProc<0x7CCB50, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FIREBALL1: ASI::CallClassProc<0x7CCD30, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FIREBALL2: ASI::CallClassProc<0x7CD010, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FIREBULLET: ASI::CallClassProc<0x7CD470, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FIREBURST: ASI::CallAsClassProc<unsigned int>(effect_fireburst, spell_manager, spell_index); break;//ASI::CallClassProc<0x7CD660, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FIRE_RESISTANCE: ASI::CallClassProc<0x7CDB10, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FIRESHIELD1: ASI::CallClassProc<0x7CDD50, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FIRESHIELD2: ASI::CallAsClassProc<unsigned int>(effect_fireshield2, spell_manager, spell_index); break;//ASI::CallClassProc<0x7CDFB0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FLEXIBILITY: ASI::CallClassProc<0x7CE1A0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FLEXIBILITY_AREA: ASI::CallClassProc<0x7CE400, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FOG: ASI::CallClassProc<0x7CE7F0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FORGET: ASI::CallClassProc<0x7CEBB0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FREEZE: ASI::CallClassProc<0x7CEE80, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FREEZE_AREA: ASI::CallClassProc<0x7CF150, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_GUARD: ASI::CallClassProc<0x7CF5D0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_HALLOW: ASI::CallClassProc<0x7CF780, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_HEALING: ASI::CallAsClassProc<unsigned int>(effect_healing, spell_manager, spell_index); break;//ASI::CallClassProc<0x7CFCC0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_UNKNOWN: ASI::CallClassProc<0x7CFED0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_HEALING_AREA: ASI::CallClassProc<0x7D0070, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_HEALING_AURA: ASI::CallClassProc<0x7D04D0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_HOLY_TOUCH: ASI::CallClassProc<0x7D0630, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ICESHIELD: ASI::CallClassProc<0x7D0740, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_HYPNOTIZE_AREA: ASI::CallClassProc<0x7D0C50, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_HYPNOTIZE: ASI::CallClassProc<0x7D1420, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ICE1: ASI::CallClassProc<0x7D18E0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ICE2: ASI::CallClassProc<0x7D1C50, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ICESTRIKE1:ASI::CallAsClassProc<unsigned int>(effect_icestrike1, spell_manager, spell_index); break;
                case ASI::SL_ICESTRIKE2: ASI::CallClassProc<0x7D2540, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ICESHIELD2: ASI::CallClassProc<0x7D2A40, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ICESHIELD3: ASI::CallClassProc<0x7D2C90, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ILLUMINATE: ASI::CallClassProc<0x7D2F30, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_INABILITY: ASI::CallClassProc<0x7D32D0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_INVISIBILITY: ASI::CallClassProc<0x7D34E0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_INVULNERABILITY: ASI::CallClassProc<0x7D3840, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_INFLEXIBILITY: ASI::CallClassProc<0x7D3AA0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_INFLEXIBILITY_AREA: ASI::CallClassProc<0x7D3CB0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_LIFETAP: ASI::CallClassProc<0x7D4170, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_LIFETAP_AURA: ASI::CallClassProc<0x7D4470, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_MANADRAIN: ASI::CallClassProc<0x7D4720, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_MANASHIELD: ASI::CallClassProc<0x7D4960, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_MANATAP: ASI::CallClassProc<0x7D4BC0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_MANATAP_AURA: ASI::CallClassProc<0x7D4E50, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_MEDITATION: ASI::CallClassProc<0x7D50E0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_MELT_RESISTANCE: ASI::CallClassProc<0x7D5210, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_MIRAGE: ASI::CallClassProc<0x7D5490, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_MUTATION: ASI::CallClassProc<0x7D5A70, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_PAIN: ASI::CallClassProc<0x7D6720, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_PAIN_AREA: ASI::CallClassProc<0x7D6990, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_UNKNOWN3: ASI::CallClassProc<0x7D6DD0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_PESTILENCE: ASI::CallClassProc<0x7D6FE0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_PETRIFY: ASI::CallAsClassProc<unsigned int>(effect_petrify, spell_manager, spell_index); break;//ASI::CallClassProc<0x7D7420, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_PLAGUE_AREA: ASI::CallClassProc<0x7D7740, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_POISON: ASI::CallClassProc<0x7D7CC0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_CLOAK_OF_NOR: ASI::CallClassProc<0x7D8240, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_QUICKNESS: ASI::CallClassProc<0x7D83F0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_QUICKNESS_AREA: ASI::CallClassProc<0x7D8650, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_RAISE_DEAD: ASI::CallClassProc<0x7D8A30, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_RAIN_OF_FIRE: ASI::CallClassProc<0x7D8F50, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_REGENERATE: ASI::CallClassProc<0x7D92B0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_REINFORCEMENT: ASI::CallClassProc<0x7D94E0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_REMEDILESS: ASI::CallClassProc<0x7D9AB0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_REMOVE_CURSE: ASI::CallClassProc<0x7D9D30, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_RETENTION: ASI::CallClassProc<0x7D9F80, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_REVENGE: ASI::CallClassProc<0x7DA190, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ROCK_BULLET: ASI::CallAsClassProc<unsigned int>(effect_rock_bullet, spell_manager, spell_index); break;
                case ASI::SL_ROOTS: ASI::CallClassProc<0x7DAC90, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ROOTS_AREA: ASI::CallClassProc<0x7DB130, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_SACRIFICE_MANA: ASI::CallClassProc<0x7DB850, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_SELF_ILLUSION: ASI::CallClassProc<0x7DB9F0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_SHIFT_MANA: ASI::CallClassProc<0x7DC100, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_SHOCK: ASI::CallClassProc<0x7DC620, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_SHOCKWAVE: ASI::CallClassProc<0x7DC8F0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_SENTINEL_HEALING: ASI::CallClassProc<0x7DCDB0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_SLOW_FIGHTING: ASI::CallClassProc<0x7DD310, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_SLOWNESS: ASI::CallClassProc<0x7DD520, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_SLOWNESS_AREA: ASI::CallClassProc<0x7DD7B0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_SPARK: ASI::CallClassProc<0x7DDC50, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_STONE: ASI::CallClassProc<0x7DDFD0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_STONE_RAIN: ASI::CallClassProc<0x7DE150, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_STONE_TOWER: ASI::CallClassProc<0x7DE4B0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_STRENGTH: ASI::CallClassProc<0x7DE8A0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_STRENGTH_AREA: ASI::CallClassProc<0x7DEB00, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_SUFFOCATION: ASI::CallClassProc<0x7DEEE0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_SUICIDE_DEATH: ASI::CallClassProc<0x7DF130, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_SUICIDE_HEAL: ASI::CallClassProc<0x7DF3A0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_SUMMON: ASI::CallClassProc<0x7DF5C0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_THORNSHIELD: ASI::CallClassProc<0x7DFD70, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_THORNSHIELD_HIT: ASI::CallClassProc<0x7DFA30, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_TORTURE: ASI::CallClassProc<0x7DFC10, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FEEDBACK: ASI::CallClassProc<0x7E01C0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_WAVE: ASI::CallClassProc<0x7E0410, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_WEAKEN: ASI::CallClassProc<0x7E08E0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_WEAKEN_AREA: ASI::CallClassProc<0x7E0B70, unsigned int>(spell_manager, spell_index); break;




                default:
                    break;
                }
            }
        }
        spell_index += 1;
        if (ASI::SpellCount(spell_manager) < spell_index)
            break;
    }
}

// hook here is required (?), because we need to jump near - replacing call to far function takes 6 bytes, while jump to relative is still 4 bytes :)
void __declspec(naked) spell_manager_handle_spells_hook()
{
    __asm
    {
        call spell_manager_handle_spells
        jmp SPELL_MANAGER_HANDLE_SPELLS_ABSOLUTE
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        if (!ASI::Init(hModule))
            return FALSE;
        if (!ASI::CheckSFVersion(ASI::SF_154))
            return FALSE;

        SPELL_MANAGER_HANDLE_SPELLS_ABSOLUTE = ASI::AddrOf(0x1B3365);

        ASI::MemoryRegion mreg(ASI::AddrOf(0x1B3360), 5);

        ASI::BeginRewrite(mreg);
        *(unsigned char*)(ASI::AddrOf(0x1B3360)) = 0xE9;   // absolute far jmp instruction, call opcode
        *(int*)(ASI::AddrOf(0x1B3361)) = (int)(&spell_manager_handle_spells_hook) - ASI::AddrOf(0x1B3365);
        ASI::EndRewrite(mreg);
    }
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

