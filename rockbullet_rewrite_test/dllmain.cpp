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

int SPELL_MANAGER_HANDLE_SPELLS_ABSOLUTE;

inline int ABS(int x) {
    return x > 0 ? x : -x;
}




void __stdcall effect_rock_bullet(int spell_index)
{
    // these are important! effect_rock_bullet is a member function of (to be determined), we need ecx to be retrieved before anything else
    ASI::Pointer class_pointer;
    __asm mov class_pointer.ptr, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto spell_index_copy = spell_index;

    ASI::Pointer some_pointer(class_pointer[spell_index * 27]);

    auto figure_index = (unsigned short)some_pointer[0x66];
    auto figure_offset = figure_index * 691;
    auto figure_index2 = (unsigned short)some_pointer[0x5F];
    auto figure_offset2 = figure_index2 * 691;

    auto val3 = (unsigned char)some_pointer[0x65];

    int tmp_sentinel = 0;

    unsigned char some_val1[8]; for (int i = 0; i < 8; i++) some_val1[i] = 0;

    if (val3 == 1)
    {
        if (figure_index != 0)
        {
            auto figuredata = *class_pointer[0x1C];

            if(ASI::IsAlive(figuredata, figure_index))
            {
                if (ASI::CallClassFunc<0x7FA8F9, unsigned int, unsigned int>(*class_pointer[0x24], figure_index) != 0)
                {
                    auto spell_id = (unsigned short)some_pointer[0x58];
                    ASI::SF_SpellData spell_data;
                    // get_spell_data
                    ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(*class_pointer[0x3C], &spell_data, spell_id);
                    if(ASI::IsAlive(figuredata, figure_index2))
                    {
                        some_val1[0] = (unsigned char)(((unsigned int)some_pointer[0x5E]) & 0xFF);
                        *((unsigned short*)(some_val1 + 1)) = (unsigned short)(some_pointer[0x5F]);
                        *((unsigned short*)(some_val1 + 3)) = (unsigned short)(some_pointer[0x61]);
                        *((unsigned short*)(some_val1 + 5)) = (unsigned short)(some_pointer[0x63]);
                        tmp_sentinel = 1;
                    }

                    auto val5 = (unsigned short)class_pointer[(spell_index_copy * 3 + 12) * 9];
                    unsigned short figure_index3 = ASI::GetXData(*class_pointer[0x50], val5, 45);
                    if ((figure_index3 == 0)||(!ASI::IsAlive(figuredata, figure_index3)))
                        figure_index3 = figure_index2;

                    // calculate_resist_chance
                    unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                        (*class_pointer[0x2C], figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                    // randint
                    unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                        (*class_pointer[0x4], 100);
                    if (rand_int <= resist)
                    {
                        ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                            (class_pointer, spell_index, figure_index);
                        ASI::Pointer some_pointer1000 = *class_pointer[0x4];
                        auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                        unsigned char params2[8]; for (int i = 0; i < 8; i++) params2[i] = 0;
                        unsigned char params3[8]; for (int i = 0; i < 8; i++) params2[i] = 0;
                        params3[0] = 1;
                        *((unsigned short*)(params3 + 1)) = figure_index;
                        ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, unsigned char(*)[8], unsigned char(*)[8], unsigned int, unsigned int, unsigned char(*)[8]>
                            (class_pointer, spell_index, 11, &some_val1, &params3, some_value1002, 40, &params2);
                    }
                    else
                    {
                        auto val6 = (unsigned int)figuredata[figure_offset + 0x2];

                        auto figure_offset3 = figure_index3 * 691;
                        auto val7 = (unsigned int)figuredata[figure_offset3 + 0x2];

                        auto val8 = (val7 & 0xFFFF) - (val6 & 0xFFFF);    // x difference?

                        auto val9 = ABS(val8);    // bigger of deltas

                        auto val10 = (val7 >> 16) - (val6 >> 16);

                        auto val11 = ABS(val10);  // smaller of deltas

                        if (val11 > val9)
                        {
                            auto tmp_swap = val11;
                            val11 = val9;
                            val9 = tmp_swap;
                        }

                        unsigned int val15 = ((val11 + val9) * 7) / 25;

                        if ((val15 & 0xFFFF) == 0)
                            val15 = 1;

                        auto spell_damage = ASI::GetSpellParameter(spell_data, 0);
                        // reduce_dmg_by_armor
                        unsigned int reduced_dmg = ASI::CallClassFunc<0x7E3AD0, unsigned int, unsigned int, unsigned int, unsigned int>
                            (*class_pointer[0x2C], figure_index2, figure_index, spell_damage);

                        if(ASI::IsAlive(figuredata, figure_index2))
                        {
                            auto val16 = (unsigned char)figuredata[figure_offset2 + 0x2B4];
                            if (val16 == 8)
                            {
                                // randint
                                unsigned short rand_int2 = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>(*class_pointer[0x4], 100);
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
                        auto spelljobstartnode = ASI::CallClassFunc<0x6695D0, unsigned short, unsigned short>(figuredata, figure_index);
                        while (spelljobstartnode != 0)
                        {
                            ASI::Pointer some_pointer3(*class_pointer[0x10]);
                            ASI::Pointer some_pointer31(*some_pointer3[0]);
                            auto some_val2 = (unsigned short)ASI::Pointer(some_pointer31[spelljobstartnode*6+4]);
                            auto spell_type2 = (unsigned short)ASI::Pointer(class_pointer[some_val2 * 27 + 0x5A]);
                            if ((spell_type2 == 16) || (spell_type2 == 17))
                            {
                                reduced_dmg = (reduced_dmg * 3) / 2;;
                                break;
                            }
                            spelljobstartnode = (unsigned short)ASI::Pointer((unsigned int)(some_pointer31.ptr) + spelljobstartnode * 6);
                        }
                        // END MOD

                        ASI::Pointer some_pointer2(*class_pointer[0x4]);
                        auto val17 = (unsigned int)some_pointer2[0xC];
                        // figure_index
                        unsigned char params0[8]; for (int i = 0; i < 8; i++) params0[i] = 0;
                        unsigned char params1[8]; for (int i = 0; i < 8; i++) params0[i] = 0;
                        params1[0] = 1;
                        *((unsigned short*)(params1 + 1)) = figure_index;
                        unsigned short param5 = 0;
                        unsigned short param6 = 0;
                        //GdEffect::Add
                        unsigned int result = ASI::CallClassFunc<0x8192D0, unsigned int, unsigned int, unsigned char(*)[8], unsigned char(*)[8], unsigned int, unsigned int, unsigned char(*)[8]>
                            (*class_pointer[0x14], (unsigned int)3, &some_val1, &params1, val17, val15, &params0);
                        ASI::XDataListAdd(*class_pointer[0x14], result, 17, spell_index);
                        ASI::XDataListAdd(*class_pointer[0x14], result, 9, spell_id);
                        ASI::XDataListAdd(*class_pointer[0x14], result, 30, reduced_dmg);
                        ASI::XDataListAdd(*class_pointer[0x14], result, 51, 1);
                        ASI::XDataListAdd(*class_pointer[0x14], result, 47, figure_index2);
                        ASI::XDataListAdd(*class_pointer[0x14], result, 48, 1);
                        ASI::XDataListAdd(*class_pointer[0x14], result, 26, figure_index);
                        ASI::XDataListAdd(*class_pointer[0x14], result, 19, 1);
                        ASI::XDataListAdd(*class_pointer[0x14], result, 45, figure_index3);
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
    ASI::CallClassProc<(unsigned int)0x7B9820, unsigned int, unsigned int>(class_pointer, spell_index, 0);

}



void __stdcall effect_icestrike1(int spell_index)
{
    ASI::Pointer class_pointer;
    __asm mov class_pointer.ptr, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto spell_index_copy = spell_index;

    ASI::Pointer some_pointer(class_pointer[spell_index * 27]);

    auto figure_index = (unsigned short)some_pointer[0x66];
    auto figure_offset = figure_index * 691;
    auto figure_index2 = (unsigned short)some_pointer[0x5F];
    auto figure_offset2 = figure_index2 * 691;

    auto val3 = (unsigned char)some_pointer[0x65];

    if (val3 == 1)
    {
        if (figure_index != 0)
        {
            auto figuredata = *class_pointer[0x1C];

            if(ASI::IsAlive(figuredata, figure_index))
            {
                auto spell_id = (unsigned short)some_pointer[0x58];
                ASI::SF_SpellData spell_data;
                // get_spell_data
                ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(*class_pointer[0x3C], &spell_data, spell_id);

                auto val5 = (unsigned short)class_pointer[(spell_index_copy * 3 + 12) * 9];
                unsigned short tick_count = ASI::GetXData(*class_pointer[0x50], val5, 18);
                //unsigned short tick_count = ASI::CallClassFunc<0x7A4E00, unsigned int, unsigned int, unsigned int>
                //    (*class_pointer[0x50], val5, 18);
                if (tick_count == 0)        // first ticck
                {
                    ASI::Pointer some_pointer1000 = *class_pointer[0x4];
                    auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                    auto some_val1 = (unsigned char*)some_pointer[0x5E];
                    unsigned int tmp_val1001 = 1;
                    unsigned char params0[8]; for (int i = 0; i < 8; i++) params0[i] = 0;
                    unsigned char params1[8]; for (int i = 0; i < 8; i++) params0[i] = 0;
                    params1[0] = 1;
                    *((unsigned short*)(params1 + 1)) = figure_index;
                    // calculate_resist_chance
                    unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                        (*class_pointer[0x2C], figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                    // randint
                    unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                        (*class_pointer[0x4], 100);

                    if (rand_int <= resist)
                    {
                        ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                            (class_pointer, spell_index, figure_index);

                        ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, unsigned char(*)[8], unsigned char(*)[8], unsigned int, unsigned int, unsigned char(*)[8]>
                            (class_pointer, spell_index, 11, (unsigned char(*)[8])some_val1, &params1, some_value1002, 10, &params0);
                            
                        goto end;
                    }
                    else
                    {
                        ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, unsigned char(*)[8], unsigned char(*)[8], unsigned int, unsigned int, unsigned char(*)[8]>
                            (class_pointer, spell_index, 3, (unsigned char(*)[8])some_val1, &params1, some_value1002, ASI::GetSpellParameter(spell_data, 2) / 100, &params0);


                        unsigned int tmp_val1002 = 1;
                        ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, unsigned char(*)[8], unsigned char(*)[8], unsigned int, unsigned int, unsigned char(*)[8]>
                            (class_pointer, spell_index, 4, (unsigned char(*)[8])some_val1, &params1, some_value1002, 25, &params0);

                        auto dmg = (unsigned short)ASI::GetSpellParameter(spell_data, 0);

                        ASI::SetXData(*class_pointer[0x50], val5, 5, (ASI::GetSpellParameter(spell_data, 2) / ASI::GetSpellParameter(spell_data, 1))-1);
                        ASI::SetXData(*class_pointer[0x50], val5, 18, dmg);
                            
                        if(ASI::IsAlive(figuredata, figure_index2))
                        {
                            auto val16 = (unsigned char)figuredata[figure_offset2 + 0x2B4];
                            if (val16 == 8)
                            {
                                // randint
                                unsigned short rand_int2 = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>(*class_pointer[0x4], 100);
                                if (rand_int2 <= 25)
                                {
                                    ASI::SetXData(*class_pointer[0x50], val5, 38, 1);
                                    dmg *= 2;
                                }
                            }
                        }

                        ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                            (*class_pointer[0x24], figure_index2, figure_index, dmg, 1, 0, 0);

                        if(ASI::IsAlive(figuredata, figure_index))
                        {
                            figuredata[figure_offset + 0x16].AsRef<unsigned int>() |= 0x120;
                            some_pointer[0x70].AsRef<unsigned char>() |= 0x3;
                            unsigned char result = ASI::CallClassFunc<0x740700, unsigned char>(figuredata[figure_offset + 0x20B]);
                            if ((result >= 9)&&(result <= 12))
                            {
                                unsigned short result2 = ASI::CallClassFunc<0x7406A0, unsigned short>(figuredata[figure_offset + 0x20B]);
                                if (ASI::CallClassFunc<0x815FB0, unsigned int, unsigned int>(*class_pointer[0x20], result2) != 0)
                                    ASI::CallClassProc<0x406EE0, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                                        (*class_pointer[0x20], figure_index, 3, 0, 0, 0);
                            }

                            some_pointer[0x56].AsRef<unsigned short>() = ASI::GetSpellParameter(spell_data, 1) / 100;
                            return;
                        }
                    }
                }
                else
                {
                    some_pointer[0x70].AsRef<unsigned char>() &= 0xFD;
                    unsigned int some_val1003 = (unsigned int)some_pointer[0x65];
                    ASI::CallClassProc<0x7B9550, unsigned int, unsigned int*>(class_pointer, spell_index, &some_val1003);
                    ASI::CallClassProc<0x7B9660, unsigned int, unsigned int*>(class_pointer, spell_index, 0);

                    goto end;
                        
                    // MOD START

                    /*ASI::Pointer some_pointer1000 = *class_pointer[0x4];
                    auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                    auto some_val1 = (unsigned int)some_pointer[0x5E];

                    unsigned char params0[8]; for (int i = 0; i < 8; i++) params0[i] = 0;
                    unsigned char params1[8]; for (int i = 0; i < 8; i++) params0[i] = 0;
                    params1[0] = 1;
                    *((unsigned short*)(params1 + 1)) = figure_index;
                    ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, unsigned char(*)[8], unsigned char(*)[8], unsigned int, unsigned int, unsigned char(*)[8]>
                        (class_pointer, spell_index, 4, &some_val1, &params1, some_value1002, 25, &params0);

                    unsigned short some_xdata1 = ASI::GetXData(*class_pointer[0x50], val5, 5);
                    if (some_xdata1 != 0)
                        ASI::SubXData(*class_pointer[0x50], val5, 5, 1);
                    else
                    {
                        some_pointer[0x70].AsRef<unsigned char>() &= 0xFD;
                        unsigned int some_val1003 = (unsigned int)some_pointer[0x65];
                        ASI::CallClassProc<0x7B9550, unsigned int, unsigned int*>(class_pointer, spell_index, &some_val1003);
                        ASI::CallClassProc<0x7B9660, unsigned int, unsigned int*>(class_pointer, spell_index, 0);
                    }

                    int remove_effect_01 = 0;
                    unsigned int cur_dmg = ASI::SubXData(*class_pointer[0x50], val5, 18, 1);
                    if(cur_dmg == 0)
                        remove_effect_01 = 1;

                    some_pointer[0x56].AsRef<unsigned short>() = ASI::GetSpellParameter(spell_data, 1) / 100;

                    unsigned short some_xdata2 = ASI::GetXData(*class_pointer[0x50], val5, 38);
                    if (some_xdata2 == 1)
                        cur_dmg *= 2;

                    ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                        (*class_pointer[0x24], figure_index2, figure_index, cur_dmg, 1, 0, 1);

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
        ASI::CallClassProc<0x7B9820, unsigned int, unsigned int>(class_pointer, spell_index, 0);
        return;
}

void __stdcall effect_death(int spell_index)
{
    ASI::Pointer class_pointer;
    __asm mov class_pointer.ptr, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto spell_index_copy = spell_index;

    ASI::Pointer some_pointer(class_pointer[spell_index * 27]);

    auto figure_index = (unsigned short)some_pointer[0x66];
    auto figure_offset = figure_index * 691;
    auto figure_index2 = (unsigned short)some_pointer[0x5F];
    auto figure_offset2 = figure_index2 * 691;

    auto val3 = (unsigned char)some_pointer[0x65];

    if (val3 == 1)
    {
        if (figure_index != 0)
        {
            auto figuredata = *class_pointer[0x1C];

            if (ASI::IsAlive(figuredata, figure_index))
            {
                auto spell_id = (unsigned short)some_pointer[0x58];
                ASI::SF_SpellData spell_data;
                // get_spell_data
                ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(*class_pointer[0x3C], &spell_data, spell_id);

                // calculate_resist_chance
                unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                    (*class_pointer[0x2C], figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                // randint
                unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                    (*class_pointer[0x4], 100);

                if (rand_int <= resist)
                {
                    ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                        (class_pointer, spell_index, figure_index);
                    ASI::Pointer some_pointer1000 = *class_pointer[0x4];
                    auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                    unsigned char some_val1[8]; for (int i = 0; i < 8; i++) some_val1[i] = 0;
                    unsigned char params2[8]; for (int i = 0; i < 8; i++) params2[i] = 0;
                    unsigned char params3[8]; for (int i = 0; i < 8; i++) params2[i] = 0;
                    params3[0] = 1;
                    *((unsigned short*)(params3 + 1)) = figure_index;
                    ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, unsigned char(*)[8], unsigned char(*)[8], unsigned int, unsigned int, unsigned char(*)[8]>
                        (class_pointer, spell_index, 11, &some_val1, &params3, some_value1002, 40, &params2);

                    goto end;
                }
                else
                {
                    ASI::Pointer some_pointer1000 = *class_pointer[0x4];
                    auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                    auto some_val1 = (unsigned char*)some_pointer[0x5E];

                    unsigned char params0[8]; for (int i = 0; i < 8; i++) params0[i] = 0;
                    unsigned char params1[8]; for (int i = 0; i < 8; i++) params0[i] = 0;
                    params1[0] = 1;
                    *((unsigned short*)(params1 + 1)) = figure_index;
                    ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, unsigned char(*)[8], unsigned char(*)[8], unsigned int, unsigned int, unsigned char(*)[8]>
                        (class_pointer, spell_index, 3, (unsigned char(*)[8])some_val1, &params1, some_value1002, 25, &params0);

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
                            unsigned short rand_int2 = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>(*class_pointer[0x4], 100);
                            if (rand_int2 <= 25)
                                dmg *= 2;
                        }
                    }

                    ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                        (*class_pointer[0x24], figure_index2, figure_index, dmg, 1, 0, 0);

                    goto end;
                }
            }
        }
    }
    end:
        ASI::CallClassProc<0x7B9820, unsigned int, unsigned int>(class_pointer, spell_index, 0);
        return;
}

void __stdcall effect_fireburst(int spell_index)
{
    ASI::Pointer class_pointer;
    __asm mov class_pointer.ptr, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto spell_index_copy = spell_index;

    ASI::Pointer some_pointer(class_pointer[spell_index * 27]);

    auto figure_index = (unsigned short)some_pointer[0x66];
    auto figure_offset = figure_index * 691;
    auto figure_index2 = (unsigned short)some_pointer[0x5F];
    auto figure_offset2 = figure_index2 * 691;

    auto val3 = (unsigned char)some_pointer[0x65];

    if (val3 == 1)
    {
        if (figure_index != 0)
        {
            auto figuredata = *class_pointer[0x1C];

            if (ASI::IsAlive(figuredata, figure_index))
            {
                auto spell_id = (unsigned short)some_pointer[0x58];
                ASI::SF_SpellData spell_data;
                // get_spell_data
                ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(*class_pointer[0x3C], &spell_data, spell_id);

                auto val5 = (unsigned short)class_pointer[(spell_index_copy * 3 + 12) * 9];
                unsigned short tick_count = ASI::GetXData(*class_pointer[0x50], val5, 18);
                if (tick_count == 0)        // first ticck
                {
                    ASI::Pointer some_pointer1000 = *class_pointer[0x4];
                    auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                    auto some_val1 = (unsigned char*)some_pointer[0x5E];
                    unsigned int tmp_val1001 = 1;
                    unsigned char params0[8]; for (int i = 0; i < 8; i++) params0[i] = 0;
                    unsigned char params1[8]; for (int i = 0; i < 8; i++) params0[i] = 0;
                    params1[0] = 1;
                    *((unsigned short*)(params1 + 1)) = figure_index;
                    // calculate_resist_chance
                    unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                        (*class_pointer[0x2C], figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                    // randint
                    unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                        (*class_pointer[0x4], 100);

                    if (rand_int <= resist)
                    {
                        ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                            (class_pointer, spell_index, figure_index);

                        ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, unsigned char(*)[8], unsigned char(*)[8], unsigned int, unsigned int, unsigned char(*)[8]>
                            (class_pointer, spell_index, 11, (unsigned char(*)[8])some_val1, &params1, some_value1002, 10, &params0);

                        goto end;
                    }
                    else
                    {
                        // START MOD
                        int dmg_mul = 0;
                        if (ASI::HasEffectExcept(class_pointer, figure_index, 1, spell_index) != -1)
                            dmg_mul = 1;
                        // END MOD

                        unsigned char params3[8]; for (int i = 0; i < 8; i++) params3[i] = (unsigned char)(some_pointer[0x5E+i]);
                        unsigned char params2[8]; for (int i = 0; i < 8; i++) params2[i] = 0;
                        ASI::CallClassProc<0x649670, unsigned char(*)[8], unsigned char(*)[8], unsigned char(*)[8]>
                            (*class_pointer[0x44], &params2, &params3, &params1);


                        ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, unsigned char(*)[8], unsigned char(*)[8], unsigned int, unsigned int, unsigned char(*)[8]>
                            (class_pointer, spell_index, 3, (unsigned char(*)[8])some_val1, &params1, some_value1002, 10, &params0);


                        ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, unsigned char(*)[8], unsigned char(*)[8], unsigned int, unsigned int, unsigned char(*)[8]>
                            (class_pointer, spell_index, 4, (unsigned char(*)[8])some_val1, &params1, some_value1002, 25, &params0);

                        auto dmg = (unsigned short)ASI::GetSpellParameter(spell_data, 0);

                        ASI::AddXData(*class_pointer[0x50], val5, 18, 1);

                        some_pointer[0x56].AsRef<unsigned short>() = ASI::GetSpellParameter(spell_data, 3) / 100;

                        if (ASI::IsAlive(figuredata, figure_index2))
                        {
                            auto val16 = (unsigned char)figuredata[figure_offset2 + 0x2B4];
                            if (val16 == 8)
                            {
                                // randint
                                unsigned short rand_int2 = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>(*class_pointer[0x4], 100);
                                if (rand_int2 <= 25)
                                {
                                    ASI::SetXData(*class_pointer[0x50], val5, 38, 1);
                                    dmg *= 2;
                                }
                            }
                        }

                        // START MOD
                        if (dmg_mul)
                            dmg = (dmg * 6) / 5;
                        // END MOD

                        ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                            (*class_pointer[0x24], figure_index2, figure_index, dmg, 1, 0, 0);

                        return;
                    }
                }
                else
                {
                    ASI::Pointer some_pointer1000 = *class_pointer[0x4];
                    auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                    auto some_val1 = (unsigned int)some_pointer[0x5E];

                    unsigned char params0[8]; for (int i = 0; i < 8; i++) params0[i] = 0;
                    unsigned char params1[8]; for (int i = 0; i < 8; i++) params0[i] = 0;
                    params1[0] = 1;
                    *((unsigned short*)(params1 + 1)) = figure_index;
                    ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, unsigned char(*)[8], unsigned char(*)[8], unsigned int, unsigned int, unsigned char(*)[8]>
                        (class_pointer, spell_index, 4, (unsigned char(*)[8])some_val1, &params1, some_value1002, 25, &params0);

                    int remove_effect = 0;
                    if (tick_count >= ASI::GetSpellParameter(spell_data, 2))
                        remove_effect = 1;
                    else
                    {
                        some_pointer[0x56].AsRef<unsigned short>() = ASI::GetSpellParameter(spell_data, 3) / 100;
                        ASI::AddXData(*class_pointer[0x50], val5, 18, 1);
                    }

                    unsigned short cur_dmg = ASI::GetSpellParameter(spell_data, 1);
                    unsigned short some_xdata2 = ASI::GetXData(*class_pointer[0x50], val5, 38);
                    if (some_xdata2 == 1)
                        cur_dmg *= 2;

                    ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                        (*class_pointer[0x24], figure_index2, figure_index, cur_dmg, 1, 0, 1);

                    if (remove_effect)
                        goto end;
                    else
                        return;
                }
            }
        }
    }
end:
    ASI::CallClassProc<0x7B9820, unsigned int, unsigned int>(class_pointer, spell_index, 0);
    return;
}

void _stdcall effect_fireshield2(int spell_index)
{
    ASI::Pointer class_pointer;
    __asm mov class_pointer.ptr, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto spell_index_copy = spell_index;

    ASI::Pointer some_pointer(class_pointer[spell_index * 27]);

    auto figure_index = (unsigned short)some_pointer[0x66];
    auto figure_offset = figure_index * 691;
    auto figure_index2 = (unsigned short)some_pointer[0x5F];
    auto figure_offset2 = figure_index2 * 691;

    auto val3 = (unsigned char)some_pointer[0x65];

    if (val3 == 1)
    {
        if (figure_index != 0)
        {
            auto figuredata = *class_pointer[0x1C];

            if (ASI::IsAlive(figuredata, figure_index))
            {
                auto spell_id = (unsigned short)some_pointer[0x58];
                ASI::SF_SpellData spell_data;
                // get_spell_data
                ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(*class_pointer[0x3C], &spell_data, spell_id);

                // calculate_resist_chance
                unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                    (*class_pointer[0x2C], figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                // randint
                unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                    (*class_pointer[0x4], 100);

                if (rand_int <= resist)
                {
                    ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                        (class_pointer, spell_index, figure_index);
                    ASI::Pointer some_pointer1000 = *class_pointer[0x4];
                    auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                    unsigned char some_val1[8]; for (int i = 0; i < 8; i++) some_val1[i] = 0;
                    unsigned char params2[8]; for (int i = 0; i < 8; i++) params2[i] = 0;
                    unsigned char params3[8]; for (int i = 0; i < 8; i++) params2[i] = 0;
                    params3[0] = 1;
                    *((unsigned short*)(params3 + 1)) = figure_index;
                    ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, unsigned char(*)[8], unsigned char(*)[8], unsigned int, unsigned int, unsigned char(*)[8]>
                        (class_pointer, spell_index, 11, &some_val1, &params3, some_value1002, 40, &params2);

                    goto end;
                }
                else
                {
                    ASI::Pointer some_pointer1000 = *class_pointer[0x4];
                    auto some_value1002 = (unsigned int)some_pointer1000[0xC];
                    auto some_val1 = (unsigned char*)some_pointer[0x5E];

                    unsigned char params0[8]; for (int i = 0; i < 8; i++) params0[i] = 0;
                    unsigned char params1[8]; for (int i = 0; i < 8; i++) params0[i] = 0;
                    params1[0] = 1;
                    *((unsigned short*)(params1 + 1)) = figure_index;
                    ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, unsigned char(*)[8], unsigned char(*)[8], unsigned int, unsigned int, unsigned char(*)[8]>
                        (class_pointer, spell_index, 3, (unsigned char(*)[8])some_val1, &params1, some_value1002, 25, &params0);

                    auto dmg = (unsigned short)ASI::GetSpellParameter(spell_data, 0);
                    // START MOD
                    if (ASI::HasEffect(class_pointer, figure_index, 1) != -1)
                        dmg = (dmg * 18) / 10;
                    // END MOD

                    if (ASI::IsAlive(figuredata, figure_index2))
                    {
                        auto val16 = (unsigned char)figuredata[figure_offset2 + 0x2B4];
                        if (val16 == 8)
                        {
                            // randint
                            unsigned short rand_int2 = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>(*class_pointer[0x4], 100);
                            if (rand_int2 <= 25)
                                dmg *= 2;
                        }
                    }

                    ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                        (*class_pointer[0x24], figure_index2, figure_index, dmg, 1, 0, 0);

                    goto end;
                }
            }
        }
    }
end:
    ASI::CallClassProc<0x7B9820, unsigned int, unsigned int>(class_pointer, spell_index, 0);
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
        if (ASI::SpellParam1(spell_manager, spell_index) != 0)
        {
            ASI::SpellParam0(spell_manager, spell_index) -= 1;
            if (ASI::SpellParam0(spell_manager, spell_index) < 0)
                ASI::SpellParam0(spell_manager, spell_index) = 0;

            if (ASI::SpellParam0(spell_manager, spell_index) == 0)
            {
                switch (ASI::SpellParam3(spell_manager, spell_index))
                {
                case ASI::SL_ABILITY_BENEFACTIONS: ASI::CallClassProc<0x7BC9D0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ABILITY_BERSERK: ASI::CallClassProc<0x7BD040, unsigned int>(spell_manager, spell_index); break;
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
                case ASI::SL_HEALING: ASI::CallClassProc<0x7CFCC0, unsigned int>(spell_manager, spell_index); break;
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
                case ASI::SL_PESTILENCE: ASI::CallClassProc<0x7DD6FE0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_PETRIFY: ASI::CallClassProc<0x7D7420, unsigned int>(spell_manager, spell_index); break;
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

