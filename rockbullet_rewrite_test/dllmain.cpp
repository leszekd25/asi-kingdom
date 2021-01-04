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
#include "SF_GdEffect.h"

int SPELL_MANAGER_HANDLE_SPELLS_ABSOLUTE;
int SPELL_MANAGER_CAST_SPELL_HOOK_START;
int SPELL_MANAGER_CAST_SPELL_HOOK_END;
int SPELL_MANAGER_CAST_SPELL_JUMP_LOCATION;

void __stdcall effect_ability_benefactions(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);
    auto figure_offset2 = figure_index2 * 691;

    ASI::AreaEntityQuery area;
    ASI::SetArea(area, 0, 0, 1023, 1023);

    //ASI::CallClassProc<0x7A2120, short, short, short, short>
    //    (area.data, 0, 0, 1023, 1023);

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

                ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index2, 0, 0, 10, ASI::SF_Unk1());

                ASI::DealDamageUnconditional(figuredata, figure_index2, -heal);
                //ASI::CallClassProc<0x66B230, unsigned short, short>(figuredata, figure_index2, -heal);
            }
        }

        // here's the difficult part :)
        
        ASI::BindEntityContainersToArea(area, figuredata, ASI::GetUnknownPointer(spell_manager, 0x40), ASI::GetUnknownPointer(spell_manager, 0x48));
        //ASI::CallClassProc<0x7A23E0, ASI::Pointer, ASI::Pointer, ASI::Pointer>
        //    (area.data, figuredata, ASI::GetUnknownPointer(spell_manager, 0x40), ASI::GetUnknownPointer(spell_manager, 0x48));

        auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
        auto figure_offset = figure_index * 691;

        auto figure_pos = (unsigned int)figuredata[figure_index * 691 + 0x2];

        ASI::FindAllEntitiesInArea(area, figure_pos, ASI::GetSpellParameter(spell_data, 2));
        //ASI::CallClassProc<0x7A19E0, unsigned int*, int>
        //    (area.data, &figure_pos, ASI::GetSpellParameter(spell_data, 2));  // area radius

        int figures_affected = 0;

        auto figure_index3 = ASI::GetNextEntity(area);
        //auto figure_index3 = ASI::CallClassFunc<0x7A2250, unsigned short>
            //(area.data);

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

                                ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index3, 0, 0, 10, ASI::SF_Unk1());

                                ASI::DealDamageUnconditional(figuredata, figure_index3, -heal);
                                //ASI::CallClassProc<0x66B230, unsigned short, short>(figuredata, figure_index3, -heal);
                            }

                            if (figures_affected == ASI::GetSpellParameter(spell_data, 1))
                                break;
                        }
                    }
                }
            }

            figure_index3 = ASI::GetNextEntity(area);
            //figure_index3 = ASI::CallClassFunc<0x7A2250, unsigned short>
                //(area.data);
        }
    }

    ASI::Pointer unk_ptr(*ASI::Pointer(area.data + 0x34));

    ASI::GdEffect_Remove(spell_manager, spell_index);

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
        ASI::CallGlobalCDeclProc<0x405B20, void*, int>(unk_ptr.ptr, 0x14);
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
                    ASI::SF_Unk1 params2;
                    ASI::SF_Unk1 params4;
                    ASI::CallClassProc<0x7B8C00, ASI::SF_Unk1*, int, unsigned int, ASI::SF_Unk1*>
                        (spell_manager->data, &params2, spell_index, ASI::GetSpellParameter(spell_data, 3), &params4);

                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index2, 0, 0, 150, params2);

                    ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 0) / 100;
                    ASI::SpellFlags(spell_manager, spell_index) |= 0x2;
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
                            ASI::SpellFlags(spell_manager, spell_index) &= 0xFD;

                            ASI::SF_Unk1 unk = ASI::GetUnk2(spell_manager, spell_index);
                            if (unk.c1 == 1)
                            {
                                spelljobnode = ASI::GetSpellJobStartNode(figuredata, unk.s1);
                                if (spelljobnode != 0)
                                {
                                    while (true)
                                    {
                                        ASI::Pointer some_pointer3(ASI::GetUnknownPointer(spell_manager, 0x10));
                                        ASI::Pointer some_pointer31(*some_pointer3[0]);

                                        if (ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode) != spell_index)
                                        {
                                            if ((ASI::SpellFlags(spell_manager, ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode)) & 2) != 0)
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
                                else
                                    ASI::RemoveFlags(figuredata, figure_index2, 0x20);
                            }
                        }
                    }
                }
            }
        }
    }
end:
    ASI::GdEffect_Remove(spell_manager, spell_index);
    return;
}

void __stdcall effect_ability_boons(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);
    auto figure_offset2 = figure_index2 * 691;

    if (ASI::SpellParam4(spell_manager, spell_index) == 1)
    {
        ASI::SF_SpellData spell_data;
        // get_spell_data
        ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>
            (ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, ASI::SpellID(spell_manager, spell_index));

        auto figuredata = ASI::GetFigureManager(spell_manager);

        if (figure_index2 != 0)
        {
            if (ASI::IsAlive(figuredata, figure_index2))
            {
                auto remediless_index = ASI::HasEffect(spell_manager, figure_index2, 41);
                if (remediless_index == -1)
                {
                    if (ASI::GetCurrentHealthPercentLeft(figuredata, figure_index2) < 100)
                    {
                        short heal = (ASI::GetCurrentHealthMax(figuredata, figure_index2) * ASI::GetSpellParameter(spell_data, 0)) / 100;
                        if (heal > ASI::GetCurrentHealthMissing(figuredata, figure_index2))
                            heal = ASI::GetCurrentHealthMissing(figuredata, figure_index2);

                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index2, 0, 0, 10, ASI::SF_Unk1());

                        ASI::CallClassProc<0x66B230, unsigned short, short>(figuredata, figure_index2, -heal);
                    }
                }
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
    return;
}

void __stdcall effect_ability_critical_hits(int spell_index)
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
                auto spell_xdata = ASI::SpellXData(spell_manager, spell_index);
                if (ASI::AddXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5, 1) == 1)
                {
                    ASI::SF_Unk1 params2;
                    ASI::SF_Unk1 params4;
                    ASI::CallClassProc<0x7B8C00, ASI::SF_Unk1*, int, unsigned int, ASI::SF_Unk1*>
                        (spell_manager->data, &params2, spell_index, ASI::GetSpellParameter(spell_data, 3), &params4);

                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index2, 0, 0, 150, params2);

                    ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 0) / 100;
                    ASI::SpellFlags(spell_manager, spell_index) |= 0x2;
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
                            ASI::SpellFlags(spell_manager, spell_index) &= 0xFD;

                            ASI::SF_Unk1 unk = ASI::GetUnk2(spell_manager, spell_index);
                            if (unk.c1 == 1)
                            {
                                spelljobnode = ASI::GetSpellJobStartNode(figuredata, unk.s1);
                                if (spelljobnode != 0)
                                {
                                    while (true)
                                    {
                                        ASI::Pointer some_pointer3(ASI::GetUnknownPointer(spell_manager, 0x10));
                                        ASI::Pointer some_pointer31(*some_pointer3[0]);

                                        if (ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode) != spell_index)
                                        {
                                            if ((ASI::SpellFlags(spell_manager, ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode)) & 2) != 0)
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
                                else
                                    ASI::RemoveFlags(figuredata, figure_index2, 0x20);
                            }
                        }
                    }
                }
            }
        }
    }
end:
    ASI::GdEffect_Remove(spell_manager, spell_index);
    return;
}

void __stdcall effect_ability_durability(int spell_index)
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
                auto spell_xdata = ASI::SpellXData(spell_manager, spell_index);
                if (ASI::AddXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5, 1) == 1)
                {
                    ASI::SF_Unk1 params2;
                    ASI::SF_Unk1 params4;
                    ASI::CallClassProc<0x7B8C00, ASI::SF_Unk1*, int, unsigned int, ASI::SF_Unk1*>
                        (spell_manager->data, &params2, spell_index, ASI::GetSpellParameter(spell_data, 3), &params4);

                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index2, 0, 0, 150, params2);

                    ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 0) / 100;
                    ASI::SpellFlags(spell_manager, spell_index) |= 0x2;
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
                            ASI::SpellFlags(spell_manager, spell_index) &= 0xFD;

                            ASI::SF_Unk1 unk = ASI::GetUnk2(spell_manager, spell_index);
                            if (unk.c1 == 1)
                            {
                                spelljobnode = ASI::GetSpellJobStartNode(figuredata, unk.s1);
                                if (spelljobnode != 0)
                                {
                                    while (true)
                                    {
                                        ASI::Pointer some_pointer3(ASI::GetUnknownPointer(spell_manager, 0x10));
                                        ASI::Pointer some_pointer31(*some_pointer3[0]);

                                        if (ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode) != spell_index)
                                        {
                                            if ((ASI::SpellFlags(spell_manager, ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode)) & 2) != 0)
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
                                else
                                    ASI::RemoveFlags(figuredata, figure_index2, 0x20);
                            }
                        }
                    }
                }
            }
        }
    }
end:
    ASI::GdEffect_Remove(spell_manager, spell_index);
    return;
}

void __stdcall effect_ability_endurance(int spell_index)
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

        auto spell_xdata = ASI::SpellXData(spell_manager, spell_index);
        if (ASI::AddXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5, 1) == 1)
        {
            ASI::SF_Unk1 params2;
            ASI::SF_Unk1 params4;
            ASI::CallClassProc<0x7B8C00, ASI::SF_Unk1*, int, unsigned int, ASI::SF_Unk1*>
                (spell_manager->data, &params2, spell_index, ASI::GetSpellParameter(spell_data, 3), &params4);

            ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index2, 0, 0, 100, params2);

            ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 0) / 100;
            ASI::SpellFlags(spell_manager, spell_index) |= 0x2;
            figuredata[figure_offset2 + 0x16].AsRef<unsigned int>() |= 0x20;
            
            
            // here's the difficult part :)
            ASI::CallClassProc<0x7A23E0, ASI::Pointer, ASI::Pointer, ASI::Pointer>
                (area.data, figuredata, ASI::GetUnknownPointer(spell_manager, 0x40), ASI::GetUnknownPointer(spell_manager, 0x48));

            auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
            auto figure_offset = figure_index * 691;

            auto figure_pos = (unsigned int)figuredata[figure_index * 691 + 0x2];

            ASI::CallClassProc<0x7A19E0, unsigned int*, int>
                (area.data, &figure_pos, ASI::GetSpellParameter(spell_data, 3));  // area radius

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
                            auto endurance_index = ASI::HasEffect(spell_manager, figure_index3, 151);
                            if (endurance_index == -1)
                            {
                                figures_affected += 1;

                                ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index3, 0, 0, 100, ASI::SF_Unk1());

                                figuredata[figure_index3 * 691 + 0x16].AsRef<unsigned int>() |= 0x20;

                                ASI::CallClassProc<0x7F0903, unsigned short, int>
                                    (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index3, spell_index);

                                if (figures_affected == ASI::GetSpellParameter(spell_data, 2))
                                    goto end2;
                            }
                        }
                    }
                }

                figure_index3 = ASI::CallClassFunc<0x7A2250, unsigned short>
                    (area.data);
            }

            goto end2;
        }
        else
        {
            auto counter = 1;
            while (counter <= (unsigned short)figuredata[0])
            {
                auto figure_offset = counter * 691;
                if (ASI::IsAlive(figuredata, counter))
                {
                    auto spelljobnode = ASI::GetSpellJobStartNode(figuredata, counter);
                    if (spelljobnode != 0)
                    {
                        if (ASI::CallClassFunc<0x7F6CE5, int, unsigned short, unsigned short>
                            (ASI::GetUnknownPointer(spell_manager, 0x24), counter, spell_index) != 0)
                        {
                            while (true)
                            {
                                ASI::Pointer some_pointer3(ASI::GetUnknownPointer(spell_manager, 0x10));
                                ASI::Pointer some_pointer31(*some_pointer3[0]);

                                if (ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode) != spell_index)
                                {
                                    if ((ASI::SpellFlags(spell_manager, ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode)) & 2) != 0)
                                        break;
                                }

                                spelljobnode = ASI::GetSpellJobNextNode(some_pointer31, spelljobnode);
                                if (spelljobnode == 0)
                                {
                                    ASI::RemoveFlags(figuredata, counter, 0x20);
                                    break;
                                }
                            }
                        }
                    }
                    else
                        ASI::RemoveFlags(figuredata, counter, 0x20);
                }

                counter += 1;
            }
        }
    }

end1:
    ASI::GdEffect_Remove(spell_manager, spell_index);

end2:
    ASI::Pointer unk_ptr(*ASI::Pointer(area.data + 0x34));
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
        ASI::CallGlobalCDeclProc<0x405B20, void*, int>(unk_ptr.ptr, 0x14);
}

void __stdcall effect_ability_patronize(int spell_index)
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

        auto spell_xdata = ASI::SpellXData(spell_manager, spell_index);
        if (ASI::AddXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5, 1) == 1)
        {
            ASI::SF_Unk1 params2;
            ASI::SF_Unk1 params4;
            ASI::CallClassProc<0x7B8C00, ASI::SF_Unk1*, int, unsigned int, ASI::SF_Unk1*>
                (spell_manager->data, &params2, spell_index, ASI::GetSpellParameter(spell_data, 3), &params4);

            ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index2, 0, 0, 100, params2);

            ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 0) / 100;
            ASI::SpellFlags(spell_manager, spell_index) |= 0x2;
            figuredata[figure_offset2 + 0x16].AsRef<unsigned int>() |= 0x20;


            // here's the difficult part :)
            ASI::CallClassProc<0x7A23E0, ASI::Pointer, ASI::Pointer, ASI::Pointer>
                (area.data, figuredata, ASI::GetUnknownPointer(spell_manager, 0x40), ASI::GetUnknownPointer(spell_manager, 0x48));

            auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
            auto figure_offset = figure_index * 691;

            auto figure_pos = (unsigned int)figuredata[figure_index * 691 + 0x2];

            ASI::CallClassProc<0x7A19E0, unsigned int*, int>
                (area.data, &figure_pos, ASI::GetSpellParameter(spell_data, 3));  // area radius

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
                            auto endurance_index = ASI::HasEffect(spell_manager, figure_index3, 150);
                            if (endurance_index == -1)
                            {
                                figures_affected += 1;

                                ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index3, 0, 0, 100, ASI::SF_Unk1());

                                figuredata[figure_index3 * 691 + 0x16].AsRef<unsigned int>() |= 0x20;

                                ASI::CallClassProc<0x7F0903, unsigned short, int>
                                    (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index3, spell_index);

                                if (figures_affected == ASI::GetSpellParameter(spell_data, 2))
                                    goto end2;
                            }
                        }
                    }
                }

                figure_index3 = ASI::CallClassFunc<0x7A2250, unsigned short>
                    (area.data);
            }

            goto end2;
        }
        else
        {
            auto counter = 1;
            while (counter <= (unsigned short)figuredata[0])
            {
                auto figure_offset = counter * 691;
                if (ASI::IsAlive(figuredata, counter))
                {
                    auto spelljobnode = ASI::GetSpellJobStartNode(figuredata, counter);
                    if (spelljobnode != 0)
                    {
                        if (ASI::CallClassFunc<0x7F6CE5, int, unsigned short, unsigned short>
                            (ASI::GetUnknownPointer(spell_manager, 0x24), counter, spell_index) != 0)
                        {
                            while (true)
                            {
                                ASI::Pointer some_pointer3(ASI::GetUnknownPointer(spell_manager, 0x10));
                                ASI::Pointer some_pointer31(*some_pointer3[0]);

                                if (ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode) != spell_index)
                                {
                                    if ((ASI::SpellFlags(spell_manager, ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode)) & 2) != 0)
                                        break;
                                }

                                spelljobnode = ASI::GetSpellJobNextNode(some_pointer31, spelljobnode);
                                if (spelljobnode == 0)
                                {
                                    ASI::RemoveFlags(figuredata, counter, 0x20);
                                    break;
                                }
                            }
                        }
                    }
                    else
                        ASI::RemoveFlags(figuredata, counter, 0x20);
                }

                counter += 1;
            }
        }
    }

end1:
    ASI::GdEffect_Remove(spell_manager, spell_index);

end2:
    ASI::Pointer unk_ptr(*ASI::Pointer(area.data + 0x34));
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
        ASI::CallGlobalCDeclProc<0x405B20, void*, int>(unk_ptr.ptr, 0x14);
}

void __stdcall effect_ability_riposte(int spell_index)
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
                    ASI::SF_Unk1 params2;
                    ASI::SF_Unk1 params4;
                    ASI::CallClassProc<0x7B8C00, ASI::SF_Unk1*, int, unsigned int, ASI::SF_Unk1*>
                        (spell_manager->data, &params2, spell_index, ASI::GetSpellParameter(spell_data, 3), &params4);

                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index2, 0, 0, 150, params2);

                    ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 0) / 100;
                    ASI::SpellFlags(spell_manager, spell_index) |= 0x2;
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
                            ASI::SpellFlags(spell_manager, spell_index) &= 0xFD;

                            ASI::SF_Unk1 unk = ASI::GetUnk2(spell_manager, spell_index);
                            if (unk.c1 == 1)
                            {
                                spelljobnode = ASI::GetSpellJobStartNode(figuredata, unk.s1);
                                if (spelljobnode != 0)
                                {
                                    while (true)
                                    {
                                        ASI::Pointer some_pointer3(ASI::GetUnknownPointer(spell_manager, 0x10));
                                        ASI::Pointer some_pointer31(*some_pointer3[0]);

                                        if (ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode) != spell_index)
                                        {
                                            if ((ASI::SpellFlags(spell_manager, ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode)) & 2) != 0)
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
                                else
                                    ASI::RemoveFlags(figuredata, figure_index2, 0x20);
                            }
                        }
                    }
                }
            }
        }
    }
end:
    ASI::GdEffect_Remove(spell_manager, spell_index);
    return;
}

void __stdcall effect_ability_salvo(int spell_index)
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
                    ASI::SF_Unk1 params2;
                    ASI::SF_Unk1 params4;
                    ASI::CallClassProc<0x7B8C00, ASI::SF_Unk1*, int, unsigned int, ASI::SF_Unk1*>
                        (spell_manager->data, &params2, spell_index, ASI::GetSpellParameter(spell_data, 3), &params4);

                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index2, 0, 0, 150, params2);

                    ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 0) / 100;
                    ASI::SpellFlags(spell_manager, spell_index) |= 0x2;
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
                            ASI::SpellFlags(spell_manager, spell_index) &= 0xFD;

                            ASI::SF_Unk1 unk = ASI::GetUnk2(spell_manager, spell_index);
                            if (unk.c1 == 1)
                            {
                                spelljobnode = ASI::GetSpellJobStartNode(figuredata, unk.s1);
                                if (spelljobnode != 0)
                                {
                                    while (true)
                                    {
                                        ASI::Pointer some_pointer3(ASI::GetUnknownPointer(spell_manager, 0x10));
                                        ASI::Pointer some_pointer31(*some_pointer3[0]);

                                        if (ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode) != spell_index)
                                        {
                                            if ((ASI::SpellFlags(spell_manager, ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode)) & 2) != 0)
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
                                else
                                    ASI::RemoveFlags(figuredata, figure_index2, 0x20);
                            }
                        }
                    }
                }
            }
        }
    }
end:
    ASI::GdEffect_Remove(spell_manager, spell_index);
    return;
}

void __stdcall effect_ability_shelter(int spell_index)
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
                    ASI::SF_Unk1 params2;
                    ASI::SF_Unk1 params4;
                    ASI::CallClassProc<0x7B8C00, ASI::SF_Unk1*, int, unsigned int, ASI::SF_Unk1*>
                        (spell_manager->data, &params2, spell_index, ASI::GetSpellParameter(spell_data, 3), &params4);

                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index2, 0, 0, 150, params2);

                    ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 0) / 100;
                    ASI::SpellFlags(spell_manager, spell_index) |= 0x2;
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
                            ASI::SpellFlags(spell_manager, spell_index) &= 0xFD;

                            ASI::SF_Unk1 unk = ASI::GetUnk2(spell_manager, spell_index);
                            if (unk.c1 == 1)
                            {
                                spelljobnode = ASI::GetSpellJobStartNode(figuredata, unk.s1);
                                if (spelljobnode != 0)
                                {
                                    while (true)
                                    {
                                        ASI::Pointer some_pointer3(ASI::GetUnknownPointer(spell_manager, 0x10));
                                        ASI::Pointer some_pointer31(*some_pointer3[0]);

                                        if (ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode) != spell_index)
                                        {
                                            if ((ASI::SpellFlags(spell_manager, ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode)) & 2) != 0)
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
                                else
                                    ASI::RemoveFlags(figuredata, figure_index2, 0x20);
                            }
                        }
                    }
                }
            }
        }
    }
end:
    ASI::GdEffect_Remove(spell_manager, spell_index);
    return;
}

void __stdcall effect_ability_shift_life(int spell_index)
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

        if (ASI::GetLifeAction(figuredata, figure_index2) != 0)
        {
            ASI::AreaEntityQuery area;

            ASI::CallClassProc<0x7A2120, short, short, short, short>
                (area.data, 0, 0, 1023, 1023);

            ASI::CallClassProc<0x7A23E0, ASI::Pointer, ASI::Pointer, ASI::Pointer>
                (area.data, figuredata, ASI::GetUnknownPointer(spell_manager, 0x40), ASI::GetUnknownPointer(spell_manager, 0x48));

            auto figure_pos2 = (unsigned int)figuredata[figure_index2 * 691 + 0x2];

            ASI::CallClassProc<0x7A19E0, unsigned int*, int>
                (area.data, &figure_pos2, ASI::GetSpellParameter(spell_data, 0));  // area radius

            unsigned int unknown_intD0 = 0;
            unsigned int unknown_struct[3]; for (int i = 0; i < 3; i++) unknown_struct[i] = 0;

            auto figure_index3 = ASI::CallClassFunc<0x7A2250, unsigned short>
                (area.data);
            unknown_intD0 = figure_index3;

            while (figure_index3 != 0)
            {
                if (ASI::IsAlive(figuredata, figure_index3))
                {
                    if (ASI::CallClassFunc<0x7FAC31, int, unsigned short, unsigned short>
                        (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index2, figure_index3) != 0)
                    {
                        if (unknown_intD0 != figure_index2)
                        {
                            if (unknown_struct[1] == unknown_struct[2])
                                ASI::CallClassProc<0x649EB0, unsigned int, unsigned int*, unsigned char*, int, int>
                                    (unknown_struct, unknown_struct[1], &unknown_intD0, 0, 1, 1);
                            else
                            {
                                if (unknown_struct[1] != 0)
                                    *((unsigned short*)(unknown_struct[1])) = unknown_intD0;
                                unknown_struct[1] += 2;
                            }
                        }
                    }
                }

                figure_index3 = ASI::CallClassFunc<0x7A2250, unsigned short>
                    (area.data);
                unknown_intD0 = figure_index3;
            }

            auto unk_ptr3 = unknown_struct[0];
            auto result = (unknown_struct[1] - unknown_struct[0]) / 2;
            if (result != 0)
            {
                auto hp = ((ASI::GetLifeAction(figuredata, figure_index2) * 100) / result) / 100;
                while (unknown_struct[0] != unknown_struct[1])
                {
                    unsigned short figure_index3 = unknown_struct[0];
                    auto hp2 = ASI::GetCurrentHealth(figuredata, figure_index3);
                    if (hp2 < hp)
                    {
                        if (hp2 != 0)
                            hp2 -= 1;
                        else
                            goto skip_heal;
                    }
                    if (hp2 == 0)
                        goto skip_heal;

                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index3, 0, 0, 25, ASI::SF_Unk1());

                    ASI::CallClassProc<0x66B230, unsigned short, short>(figuredata, figure_index3, hp2);

                    ASI::CallClassProc<0x66B230, unsigned short, short>(figuredata, figure_index2, -hp2);

                skip_heal:
                    result -= 1;
                    if (hp2 < hp)
                    {
                        if (result != 0)
                        {
                            hp = ((ASI::GetLifeAction(figuredata, figure_index2) * 100) / result) / 100;
                        }
                    }

                    unknown_struct[0] += 2;
                }
            }

            if (unk_ptr3 != 0)
            {
                auto result2 = (unknown_struct[2] - unk_ptr3);
                if (result2 <= 128)
                    ASI::CallGlobalCDeclProc<0xB02000, unsigned int>(unk_ptr3);
                else
                    ASI::CallGlobalCDeclProc<0x405B20, unsigned int, unsigned int>(unk_ptr3, result2);
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

            ASI::CallClassProc<0x624D90, void*, unsigned int>
                (area.data + 0x34, unk_ptr.ptr, 1);
        }
    }

end:
    ASI::GdEffect_Remove(spell_manager, spell_index);
    return;
}

void __stdcall effect_ability_steelskin(int spell_index)
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
                    ASI::SF_Unk1 params2;
                    ASI::SF_Unk1 params4;
                    ASI::CallClassProc<0x7B8C00, ASI::SF_Unk1*, int, unsigned int, ASI::SF_Unk1*>
                        (spell_manager->data, &params2, spell_index, ASI::GetSpellParameter(spell_data, 3), &params4);

                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index2, 0, 0, 150, params2);

                    ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 0) / 100;
                    ASI::SpellFlags(spell_manager, spell_index) |= 0x2;
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
                            ASI::SpellFlags(spell_manager, spell_index) &= 0xFD;

                            ASI::SF_Unk1 unk = ASI::GetUnk2(spell_manager, spell_index);
                            if (unk.c1 == 1)
                            {
                                spelljobnode = ASI::GetSpellJobStartNode(figuredata, unk.s1);
                                if (spelljobnode != 0)
                                {
                                    while (true)
                                    {
                                        ASI::Pointer some_pointer3(ASI::GetUnknownPointer(spell_manager, 0x10));
                                        ASI::Pointer some_pointer31(*some_pointer3[0]);

                                        if (ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode) != spell_index)
                                        {
                                            if ((ASI::SpellFlags(spell_manager, ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode)) & 2) != 0)
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
                                else
                                    ASI::RemoveFlags(figuredata, figure_index2, 0x20);
                            }
                        }
                    }
                }
            }
        }
    }
end:
    ASI::GdEffect_Remove(spell_manager, spell_index);
    return;
}

void __stdcall effect_ability_trueshot(int spell_index)
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
                    ASI::SF_Unk1 params2;
                    ASI::SF_Unk1 params4;
                    ASI::CallClassProc<0x7B8C00, ASI::SF_Unk1*, int, unsigned int, ASI::SF_Unk1*>
                        (spell_manager->data, &params2, spell_index, ASI::GetSpellParameter(spell_data, 3), &params4);

                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index2, 0, 0, 150, params2);

                    ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 0) / 100;
                    ASI::SpellFlags(spell_manager, spell_index) |= 0x2;
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
                            ASI::SpellFlags(spell_manager, spell_index) &= 0xFD;

                            ASI::SF_Unk1 unk = ASI::GetUnk2(spell_manager, spell_index);
                            if (unk.c1 == 1)
                            {
                                spelljobnode = ASI::GetSpellJobStartNode(figuredata, unk.s1);
                                if (spelljobnode != 0)
                                {
                                    while (true)
                                    {
                                        ASI::Pointer some_pointer3(ASI::GetUnknownPointer(spell_manager, 0x10));
                                        ASI::Pointer some_pointer31(*some_pointer3[0]);

                                        if (ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode) != spell_index)
                                        {
                                            if ((ASI::SpellFlags(spell_manager, ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode)) & 2) != 0)
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
                                else
                                    ASI::RemoveFlags(figuredata, figure_index2, 0x20);
                            }
                        }
                    }
                }
            }
        }
    }
end:
    ASI::GdEffect_Remove(spell_manager, spell_index);
    return;
}

void __stdcall effect_ability_warcry(int spell_index)
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

        auto spell_xdata = ASI::SpellXData(spell_manager, spell_index);
        if (ASI::AddXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5, 1) == 1)
        {
            ASI::SF_Unk1 params2;
            ASI::SF_Unk1 params4;
            ASI::CallClassProc<0x7B8C00, ASI::SF_Unk1*, int, unsigned int, ASI::SF_Unk1*>
                (spell_manager->data, &params2, spell_index, ASI::GetSpellParameter(spell_data, 3), &params4);

            ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index2, 0, 0, 100, params2);

            ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 0) / 100;
            ASI::SpellFlags(spell_manager, spell_index) |= 0x2;
            figuredata[figure_offset2 + 0x16].AsRef<unsigned int>() |= 0x20;


            // here's the difficult part :)
            ASI::CallClassProc<0x7A23E0, ASI::Pointer, ASI::Pointer, ASI::Pointer>
                (area.data, figuredata, ASI::GetUnknownPointer(spell_manager, 0x40), ASI::GetUnknownPointer(spell_manager, 0x48));

            auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
            auto figure_offset = figure_index * 691;

            auto figure_pos = (unsigned int)figuredata[figure_index * 691 + 0x2];

            ASI::CallClassProc<0x7A19E0, unsigned int*, int>
                (area.data, &figure_pos, ASI::GetSpellParameter(spell_data, 3));  // area radius

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
                            auto warcry_index = ASI::HasEffect(spell_manager, figure_index3, 148);
                            if (warcry_index == -1)
                            {
                                figures_affected += 1;

                                ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index2, 0, 0, 100, ASI::SF_Unk1());

                                figuredata[figure_index3 * 691 + 0x16].AsRef<unsigned int>() |= 0x20;

                                ASI::CallClassProc<0x7F0903, unsigned short, int>
                                    (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index3, spell_index);

                                if (figures_affected == ASI::GetSpellParameter(spell_data, 2))
                                    goto end2;
                            }
                        }
                    }
                }

                figure_index3 = ASI::CallClassFunc<0x7A2250, unsigned short>
                    (area.data);
            }

            goto end2;
        }
        else
        {
            auto counter = 1;
            while (counter <= (unsigned short)figuredata[0])
            {
                auto figure_offset = counter * 691;
                if (ASI::IsAlive(figuredata, counter))
                {
                    auto spelljobnode = ASI::GetSpellJobStartNode(figuredata, counter);
                    if (spelljobnode != 0)
                    {
                        if (ASI::CallClassFunc<0x7F6CE5, int, unsigned short, unsigned short>
                            (ASI::GetUnknownPointer(spell_manager, 0x24), counter, spell_index) != 0)
                        {
                            while (true)
                            {
                                ASI::Pointer some_pointer3(ASI::GetUnknownPointer(spell_manager, 0x10));
                                ASI::Pointer some_pointer31(*some_pointer3[0]);

                                if (ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode) != spell_index)
                                {
                                    if ((ASI::SpellFlags(spell_manager, ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode)) & 2) != 0)
                                        break;
                                }

                                spelljobnode = ASI::GetSpellJobNextNode(some_pointer31, spelljobnode);
                                if (spelljobnode == 0)
                                {
                                    ASI::RemoveFlags(figuredata, counter, 0x20);
                                    break;
                                }
                            }
                        }
                    }
                    else
                        ASI::RemoveFlags(figuredata, counter, 0x20);
                }

                counter += 1;
            }
        }
    }

end1:
    ASI::GdEffect_Remove(spell_manager, spell_index);

end2:
    ASI::Pointer unk_ptr(*ASI::Pointer(area.data + 0x34));
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
        ASI::CallGlobalCDeclProc<0x405B20, void*, int>(unk_ptr.ptr, 0x14);
}

void __stdcall effect_acid_cloud(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    ASI::AreaEntityQuery area;

    ASI::CallClassProc<0x7A2120, short, short, short, short>
        (area.data, 0, 0, 1023, 1023);

    auto val3 = ASI::SpellParam4(spell_manager, spell_index);

    if (val3 == 5)
    {
        auto figuredata = ASI::GetFigureManager(spell_manager);

        auto spell_id = ASI::SpellID(spell_manager, spell_index);
        ASI::SF_SpellData spell_data;
        // get_spell_data
        ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, spell_id);

        auto val5 = ASI::SpellXData(spell_manager, spell_index);
        if (ASI::GetXData(ASI::GetXDataPointer(spell_manager), val5, 5) == 0)
        {
            auto spell_time = (ASI::GetSpellParameter(spell_data, 1) * ASI::GetSpellParameter(spell_data, 2)) / 100;

            ASI::SF_Unk1 params2;
            ASI::SF_Unk1 params4;
            ASI::CallClassProc<0x7B8C00, ASI::SF_Unk1*, int, unsigned int, ASI::SF_Unk1*>
                (spell_manager->data, &params2, spell_index, ASI::GetSpellParameter(spell_data, 3), &params4);

            ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 4, 0, ASI::SpellParamX(spell_manager, spell_index), ASI::SpellParamY(spell_manager, spell_index), spell_time, params2);
        }

        if (ASI::AddXData(ASI::GetXDataPointer(spell_manager), val5, 5, 1) < ASI::GetSpellParameter(spell_data, 2))
        {
            ASI::CallClassProc<0x7A23E0, ASI::Pointer, ASI::Pointer, ASI::Pointer>
                (area.data, figuredata, ASI::GetUnknownPointer(spell_manager, 0x40), ASI::GetUnknownPointer(spell_manager, 0x48));

            auto figure_pos2 = ASI::SpellParamPos(spell_manager, spell_index);

            ASI::CallClassProc<0x7A19E0, unsigned int*, int>
                (area.data, &figure_pos2, ASI::GetSpellParameter(spell_data, 0));  // area radius

            auto figure_index3 = ASI::CallClassFunc<0x7A2250, unsigned short>
                (area.data);

            while (figure_index3 != 0)
            {
                if (ASI::IsAlive(figuredata, figure_index3))
                {
                    if (ASI::CallClassFunc<0x7FA8F9, unsigned int, unsigned short>
                        (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index3) != 0)
                    {
                        if ((ASI::CallClassFunc<0x5F7550, unsigned int, unsigned char>
                            (ASI::GetUnknownPointer(spell_manager, 0x3C), ASI::GetRace(figuredata, figure_index3)) & 2) != 0)
                        {
                            if (figure_index3 != ASI::SpellSource(spell_manager, spell_index))
                            {
                                // calculate_resist_chance
                                unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                                    (ASI::GetUnknownPointer(spell_manager, 0x2C), ASI::SpellSource(spell_manager, spell_index), figure_index3, ASI::GetSpellIndex(spell_data));
                                // randint
                                unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                                    (ASI::GetUnknownPointer(spell_manager, 0x4), 100);

                                if (rand_int <= resist)
                                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 11, 1, figure_index3, 0, 0, 10, ASI::SF_Unk1());
                                else
                                {
                                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index3, 0, 0, 10, ASI::SF_Unk1());

                                    ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                                        (ASI::GetUnknownPointer(spell_manager, 0x24), ASI::SpellSource(spell_manager, spell_index), figure_index3, ASI::GetSpellParameter(spell_data, 3), 1, 0, 0);
                                }
                            }
                        }
                    }
                }
                figure_index3 = ASI::CallClassFunc<0x7A2250, unsigned short>
                    (area.data);
            }

            ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 1) / 100;
            goto end2;
        }
    }

    // TODO: check these!
end1:
    ASI::GdEffect_Remove(spell_manager, spell_index);

end2:
    ASI::Pointer unk_ptr(*ASI::Pointer(area.data + 0x34));
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
        ASI::CallGlobalCDeclProc<0x405B20, void*, int>(unk_ptr.ptr, 0x14);
}

void __stdcall effect_almightness_black(int spell_index)
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
                if (ASI::GetXData(ASI::GetXDataPointer(spell_manager), val5, 5) == 0)
                {
                    // draw aggro
                    ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                        (spell_manager->data, spell_index, figure_index);
                    // calculate_resist_chance
                    unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                    // randint
                    unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x4), 100);

                    if (rand_int <= resist)
                    {
                        ASI::SpellToDoCount(spell_manager, spell_index) += ASI::GetSpellParameter(spell_data, 4) / 100;

                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, ASI::GetSpellParameter(spell_data, 4) / 100, ASI::SF_Unk1());

                        ASI::AddXData(ASI::GetXDataPointer(spell_manager), val5, 5, 1);

                        auto dmg = ASI::GetSpellParameter(spell_data, 0);
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

                        ASI::CallClassProc<0x66B230, unsigned short, short>
                            (figuredata, figure_index2, -(dmg * ASI::GetSpellParameter(spell_data, 1) / 100));

                        ASI::CallClassProc<0x66AEA0, unsigned short, char>
                            (figuredata, figure_index, -ASI::GetSpellParameter(spell_data, 2));

                        ASI::CallClassProc<0x66B1D0, unsigned short, char>
                            (figuredata, figure_index, -ASI::GetSpellParameter(spell_data, 3));

                        return;
                    }
                }
                else
                {
                    ASI::CallClassProc<0x66AEA0, unsigned short, char>
                        (figuredata, figure_index, ASI::GetSpellParameter(spell_data, 2));

                    ASI::CallClassProc<0x66B1D0, unsigned short, char>
                        (figuredata, figure_index, ASI::GetSpellParameter(spell_data, 3));
                }
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
}

void __stdcall effect_almightness_elemental(int spell_index)
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

    ASI::SF_Unk1 some_val1;

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

                if (ASI::IsAlive(figuredata, figure_index2))
                    some_val1 = ASI::GetUnk1(spell_manager, spell_index);

                ASI::SF_PositionQuery val6(figuredata[figure_offset + 0x2].ptr);
                ASI::SF_PositionQuery val7(figuredata[figure_offset2 + 0x2].ptr);

                unsigned short val15 = (ASI::FastDistance(val6, val7) * 7) / 25;

                if ((val15 & 0xFFFF) == 0)
                    val15 = 1;

                ASI::SF_Unk1 params0;
                ASI::SF_Unk1 params4(1, figure_index, 0, 0);
                ASI::CallClassProc<0x7B8C00, ASI::SF_Unk1*, int, unsigned int, ASI::SF_Unk1*>
                    (spell_manager->data, &params0, spell_index, 0, &params4);

                //GdEffect::Add
                unsigned int result = ASI::GdEffect_Add(spell_manager, spell_index, 3, 1, figure_index, 0, 0, val15, some_val1, params0);

                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 17, spell_index);
                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 9, spell_id);
                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 28, ASI::GetSpellParameter(spell_data, 0));
                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 51, 1);
                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 47, figure_index2);
                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 48, 1);
                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 26, figure_index);
                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 19, 1);
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
}

void __stdcall effect_almightness_elemental2(int spell_index)
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

    ASI::SF_Unk1 some_val1;

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

                auto spell_xdata = ASI::SpellXData(spell_manager, spell_index);
                if (ASI::AddXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5, 1) == 1)
                {
                    ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                        (spell_manager->data, spell_index, figure_index);

                    // calculate_resist_chance
                    unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                    // randint
                    unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x4), 100);

                    if (rand_int <= resist)
                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 11, 1, figure_index, 0, 0, 10, ASI::SF_Unk1());
                    else
                    {
                        ASI::SpellToDoCount(spell_manager, spell_index) += ASI::GetSpellParameter(spell_data, 6) / 100;

                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, ASI::SpellToDoCount(spell_manager, spell_index), ASI::SF_Unk1());

                        auto dmg = (unsigned short)(ASI::GetSpellParameter(spell_data, 0)+ASI::GetSpellParameter(spell_data, 2));

                        if (ASI::IsAlive(figuredata, figure_index2))
                        {
                            auto val16 = (unsigned char)figuredata[figure_offset2 + 0x2B4];
                            if (val16 == 8)
                            {
                                // randint
                                unsigned short rand_int2 = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x4), 100);
                                if (rand_int2 <= 25)
                                {
                                    dmg *= 2;
                                    ASI::SetXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 38, 1);
                                }
                            }
                        }

                        auto phys_damage = ASI::GetSpellParameter(spell_data, 4);
                        // reduce_dmg_by_armor
                        unsigned short reduced_dmg = ASI::CallClassFunc<0x7E3AD0, unsigned short, unsigned int, unsigned int, unsigned int>
                            (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, phys_damage);

                        ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                            (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index2, figure_index, dmg + reduced_dmg, 1, 0, 0);

                        return;
                    }
                }
                else
                {
                    auto dmg = (unsigned short)(ASI::GetSpellParameter(spell_data, 1) + ASI::GetSpellParameter(spell_data, 3));

                    if (ASI::GetXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 38) == 1)
                        dmg *= 2;

                    auto last_tick = 0;
                    if (ASI::GetXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5) < ASI::GetSpellParameter(spell_data, 5))
                        ASI::SpellToDoCount(spell_manager, spell_index) += ASI::GetSpellParameter(spell_data, 6) / 100;
                    else
                        last_tick = 1;

                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 4, 1, figure_index, 0, 0, 10, ASI::SF_Unk1());

                    ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index2, figure_index, dmg, 1, 0, 1);

                    if (last_tick != 0)
                        return;
                }
            }
        }
    }

end:
    ASI::GdEffect_Remove(spell_manager, spell_index);
}

void __stdcall effect_almightness_mental(int spell_index)
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

    ASI::SF_Unk1 some_val1;

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

                ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                    (spell_manager->data, spell_index, figure_index);

                // calculate_resist_chance
                unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                    (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                // randint
                unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                    (ASI::GetUnknownPointer(spell_manager, 0x4), 100);

                if (rand_int > resist)
                {
                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, ASI::SpellToDoCount(spell_manager, spell_index), ASI::SF_Unk1());

                    auto intelligence = ASI::GetCurrentIntelligence(figuredata, figure_index);

                    auto dmg = CLAMP(ASI::GetSpellParameter(spell_data, 0), ASI::GetSpellParameter(spell_data, 1), ASI::GetSpellParameter(spell_data, 2), ASI::GetSpellParameter(spell_data, 3), intelligence);

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

                    auto charisma = ASI::GetCurrentCharisma(figuredata, figure_index2);

                    auto manasteal = ((charisma + 15) * (ASI::GetSpellParameter(spell_data, 4))) / 50;

                    if (ASI::GetCurrentMana(figuredata, figure_index) < manasteal)
                        manasteal = ASI::GetCurrentMana(figuredata, figure_index);

                    ASI::CallClassProc<0x66B260, unsigned short, short>
                        (figuredata, figure_index, manasteal);

                    if(ASI::IsAlive(figuredata, figure_index2))
                        ASI::CallClassProc<0x66B260, unsigned short, short>
                        (figuredata, figure_index, -manasteal);

                    if (ASI::GetLevel(figuredata, figure_index) < ASI::GetSpellParameter(spell_data, 5))
                    {
                        if ((ASI::GetFlags(figuredata, figure_index) & 0x1000000) == 0)
                        {
                            ASI::CallClassProc<0x6642D0, unsigned short, unsigned short>
                                (figuredata, figure_index, figure_index2);
                        }
                    }
                }
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
}

void __stdcall effect_almightness_white(int spell_index)
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

                auto spell_xdata = ASI::SpellXData(spell_manager, spell_index);       // byte ptr one time, word ptr another
                if (ASI::GetXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5) == 0)
                {
                    if (ASI::CallClassFunc<0x7B8E60, int, int>
                        (spell_manager->data, spell_index) != 0)
                    {
                        ASI::SpellToDoCount(spell_manager, spell_index) += (ASI::GetSpellParameter(spell_data, 4) / 100);
                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, ASI::SpellToDoCount(spell_manager, spell_index), ASI::SF_Unk1());

                        ASI::GetFlags(figuredata, figure_index) |= (0x20 | 0x40);
                        ASI::SpellFlags(spell_manager, spell_index) |= 0x6;
                        ASI::CallClassProc<0x66B230, unsigned short, short>(figuredata, figure_index, -(int)ASI::GetSpellParameter(spell_data, 1));

                        ASI::CallClassProc<0x66AF30, unsigned short, char>
                            (figuredata, figure_index, (char)ASI::GetSpellParameter(spell_data, 2));
                        ASI::CallClassProc<0x66B1D0, unsigned short, char>
                            (figuredata, figure_index, (char)ASI::GetSpellParameter(spell_data, 3));

                        ASI::AddXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5, 1);
                    }
                }
                else
                {
                    ASI::CallClassProc<0x7B95D0, int, int>
                        (spell_manager->data, spell_index, 0);

                    ASI::SpellFlags(spell_manager, spell_index) &= 0xFD;

                    ASI::SF_Unk1 unk = ASI::GetUnk2(spell_manager, spell_index);
                    if (unk.c1 == 1)
                    {
                        auto spelljobnode = ASI::GetSpellJobStartNode(figuredata, unk.s1);
                        if (spelljobnode != 0)
                        {
                            while (true)
                            {
                                ASI::Pointer some_pointer3(ASI::GetUnknownPointer(spell_manager, 0x10));
                                ASI::Pointer some_pointer31(*some_pointer3[0]);

                                if (ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode) != spell_index)
                                {
                                    if ((ASI::SpellFlags(spell_manager, ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode)) & 2) != 0)
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
                        else
                            ASI::RemoveFlags(figuredata, figure_index2, 0x20);
                    }
                    ASI::CallClassProc<0x66AF30, unsigned short, char>
                        (figuredata, figure_index, -(char)ASI::GetSpellParameter(spell_data, 2));
                    ASI::CallClassProc<0x66B1D0, unsigned short, char>
                        (figuredata, figure_index, -(char)ASI::GetSpellParameter(spell_data, 3));
                }
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
}

void __stdcall effect_amok(int spell_index)
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

                auto spell_xdata = ASI::SpellXData(spell_manager, spell_index);       // byte ptr one time, word ptr another
                if (ASI::AddXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5, 1) == 1)
                {
                    // calculate_resist_chance
                    unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                    // randint
                    unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x4), 100);

                    if((rand_int <= resist)
                    || (ASI::GetLevel(figuredata, figure_index) > ASI::GetSpellParameter(spell_data, 1))
                    || ((ASI::GetFlags(figuredata, figure_index) & 0x1000000) == 0))
                    {
                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 11, 1, figure_index, 0, 0, 10, ASI::SF_Unk1());

                        ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                            (spell_manager->data, spell_index, figure_index);

                        ASI::CallClassProc<0x7B95D0, int, int>
                            (spell_manager->data, spell_index, 0);
                    }
                    else
                    {
                        if (ASI::CallClassFunc<0x7B8E60, unsigned int, unsigned short>
                            (spell_manager->data, spell_index) != 0)
                        {
                            ASI::GetFlags(figuredata, figure_index) |= 0x40;
                            ASI::SpellFlags(spell_manager, spell_index) |= 0x4;

                            auto charisma = ASI::GetCurrentCharisma(figuredata, figure_index2);
                            auto time = ((charisma + 15) * (ASI::GetSpellParameter(spell_data, 0))) / 10;
                            ASI::SpellToDoCount(spell_manager, spell_index) += time / 1000;

                            ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, ASI::SpellToDoCount(spell_manager, spell_index), ASI::SF_Unk1());
                            ASI::CallClassProc<0x664080, unsigned short>
                                (figuredata, figure_index);
                            return;
                        }
                    }
                }
                else
                {
                    ASI::CallClassProc<0x7B95D0, int, int>
                        (spell_manager->data, spell_index, 0);

                    ASI::CallClassProc<0x7B95D0, unsigned short>
                        (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index);
                }
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
}

void __stdcall effect_arrowtower(int spell_index)
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

    if (val3 == 0)
    {
        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 5, 4, 0, ASI::SpellParamX(spell_manager, spell_index), ASI::SpellParamY(spell_manager, spell_index), 10, ASI::SF_Unk1());
    }
    else if (val3 == 1)
    {
        if (figure_index != 0)
        {
            auto figuredata = ASI::GetFigureManager(spell_manager);

            if (ASI::IsAlive(figuredata, figure_index))
            {
                ASI::SF_PositionQuery val6(figuredata[figure_offset + 0x2].ptr);
                ASI::SF_PositionQuery val7(figuredata[figure_index2 * 691 + 0x2].ptr);

                unsigned short val15 = (ASI::FastDistance(val6, val7) * 7) / 25;
                if (val15 == 0)
                    val15 = 1;

                auto spell_id = ASI::SpellID(spell_manager, spell_index);
                ASI::SF_SpellData spell_data;
                // get_spell_data
                ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, spell_id);

                auto spell_damage = ASI::GetSpellParameter(spell_data, 0);
                // reduce_dmg_by_armor
                unsigned int reduced_dmg = ASI::CallClassFunc<0x7E3AD0, unsigned int, unsigned int, unsigned int, unsigned int>
                    (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, spell_damage);

                auto figure_unitid = ASI::GetFigureID(figuredata, figure_index);
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

                ASI::SF_Unk1 some_val1(2, ASI::GetBuilding(figuredata, figure_index), 0, 0);

                unsigned int result = ASI::GdEffect_Add(spell_manager, spell_index, 3, 1, figure_index, 0, 0, val15, some_val1, ASI::SF_Unk1());

                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 17, spell_index);
                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 9, spell_id);
                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 30, reduced_dmg);
                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 51, 1);
                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 47, figure_index2);
                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 48, 1);
                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 26, figure_index);
                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 19, 1);
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
}

void __stdcall effect_assistance(int spell_index)
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
        auto figuredata = ASI::GetFigureManager(spell_manager);

        ASI::SF_SpellData spell_data;
        // get_spell_data
        ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>
            (ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, ASI::SpellID(spell_manager, spell_index));

        if (figure_index != 0)
        {
            if (ASI::IsAlive(figuredata, figure_index))
            {
                auto spell_xdata = ASI::SpellXData(spell_manager, spell_index);       // byte ptr one time, word ptr another
                if (ASI::AddXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5, 1) == 1)
                {
                    ASI::SpellFlags(spell_manager, spell_index) |= 0x2;
                    ASI::GetFlags(figuredata, figure_index) |= 0x20;
                    ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 2) / 100;

                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, ASI::SpellToDoCount(spell_manager, spell_index), ASI::SF_Unk1());
                    return;
                }
                else
                {
                    ASI::SpellFlags(spell_manager, spell_index) &= 0xFD;

                    ASI::SF_Unk1 unk = ASI::GetUnk2(spell_manager, spell_index);
                    if (unk.c1 == 1)
                    {
                        auto spelljobnode = ASI::GetSpellJobStartNode(figuredata, unk.s1);
                        if (spelljobnode != 0)
                        {
                            while (true)
                            {
                                ASI::Pointer some_pointer3(ASI::GetUnknownPointer(spell_manager, 0x10));
                                ASI::Pointer some_pointer31(*some_pointer3[0]);

                                if (ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode) != spell_index)
                                {
                                    if ((ASI::SpellFlags(spell_manager, ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode)) & 2) != 0)
                                        break;
                                }

                                spelljobnode = ASI::GetSpellJobNextNode(some_pointer31, spelljobnode);
                                if (spelljobnode == 0)
                                {
                                    ASI::RemoveFlags(figuredata, figure_index, 0x20);
                                    break;
                                }
                            }
                        }
                        else
                            ASI::RemoveFlags(figuredata, figure_index, 0x20);
                    }
                }
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
}

void __stdcall effect_aura(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    ASI::AreaEntityQuery f_area;

    ASI::CallClassProc<0x7A2120, short, short, short, short>
        (f_area.data, 0, 0, 1023, 1023);

    unsigned int unk_otherints[3];
    unk_otherints[0] = 0;
    unk_otherints[1] = 0;
    unk_otherints[2] = 0;

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);

    auto val3 = ASI::SpellParam4(spell_manager, spell_index);

    auto figuredata = ASI::GetFigureManager(spell_manager);
    auto spell_xdata = ASI::SpellXData(spell_manager, spell_index);

    if (val3 == 1)
    {
        if (ASI::IsAlive(figuredata, figure_index2))
        {
            ASI::SF_SpellData spell_data;
            // get_spell_data
            ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>
                (ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, ASI::SpellID(spell_manager, spell_index));

            ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 0) / 100;

            if (ASI::AddXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5, 1) == 1)
            {
                ASI::GetFlags(figuredata, figure_index2) |= 0x8000;

                ASI::SF_Unk1 some_val1(1, figure_index2, 0, 0);
                unsigned int result = ASI::GdEffect_Add(spell_manager, spell_index, 3, 1, figure_index2, 0, 0, 0, some_val1, ASI::SF_Unk1());

                ASI::SetXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 6, result);
                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 17, spell_index);
                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 9, ASI::SpellID(spell_manager, spell_index));
            }

            auto aura_type_sentinel = 1;
            auto aura_type = ASI::GetSpellParameter(spell_data, 5);
            if ((aura_type < 1) || (aura_type > 3))
                aura_type_sentinel = 0;

            auto aura_mana_sentinel = 1;
            if (ASI::GetCurrentMana(figuredata, figure_index2) < ASI::GetSpellParameter(spell_data, 8))
                aura_mana_sentinel = 0;

            if (aura_type_sentinel)
            {
                if (aura_mana_sentinel)
                {
                    unsigned short actual_distance = 0xFFFF;
                    auto spell_type = ASI::GetSpellType(spell_data);
                    if ((spell_type == 223) || ((spell_type >= 225) && (spell_type <= 228)))    // siege auras
                    {
                        auto buildingdata = ASI::GetBuildingManager(spell_manager);
                        ASI::AreaEntityQuery b_area;

                        ASI::CallClassProc<0x7A1D90, short, short, short, short>
                            (b_area.data, 0, 0, 1023, 1023);

                        ASI::CallClassProc<0x7A20E0, ASI::Pointer, ASI::Pointer, ASI::Pointer>
                            (b_area.data, buildingdata, ASI::GetUnknownPointer(spell_manager, 0x40), ASI::GetUnknownPointer(spell_manager, 0x48));
                        
                        auto figure_pos = ASI::GetPosition(figuredata, figure_index2);

                        ASI::CallClassProc<0x7A19E0, unsigned int*, int>
                            (b_area.data, &figure_pos, ASI::GetSpellParameter(spell_data, 2)+10);  // area radius

                        auto building_index = ASI::CallClassFunc<0x7A1F80, unsigned short>(b_area.data);
                        if (building_index != 0)
                        {
                            while (building_index != 0)
                            {
                                if ((short)(buildingdata[building_index * 52 + 0x11]) > 0)
                                {
                                    if (ASI::GetSpellParameter(spell_data, 5) == 1)
                                    {
                                        if (ASI::CallClassFunc<0x81FF40, int, unsigned short, unsigned short>
                                            (ASI::GetUnknownPointer(spell_manager, 0x0C), figure_index2, building_index) == 0)
                                        {
                                            if ((short)(buildingdata[building_index * 52 + 0x8]) != ASI::GetInUse(figuredata, figure_index2))
                                            {
                                                building_index = ASI::CallClassFunc<0x7A1F80, unsigned short>(b_area.data);
                                                continue;
                                            }
                                        }
                                    }
                                    else if (ASI::GetSpellParameter(spell_data, 5) == 2)
                                    {
                                        if (ASI::CallClassFunc<0x81FD90, int, unsigned short, unsigned short>
                                            (ASI::GetUnknownPointer(spell_manager, 0x0C), figure_index2, building_index) == 0)
                                        {
                                            building_index = ASI::CallClassFunc<0x7A1F80, unsigned short>(b_area.data);
                                            continue;
                                        }
                                    }
                                    else
                                    {
                                        building_index = ASI::CallClassFunc<0x7A1F80, unsigned short>(b_area.data);
                                        continue;
                                    }

                                    ASI::SF_Unk1 unk = ASI::GetUnk2(spell_manager, figure_index2);
                                    ASI::CallClassProc<0x81F510, ASI::SF_Unk1*, unsigned short, unsigned short, int>
                                        (ASI::GetUnknownPointer(spell_manager, 0x0C), &unk, building_index, figure_index2, 1);

                                    ASI::SF_PositionQuery pos1(unk.s2, unk.s3);
                                    ASI::SF_PositionQuery pos2(figure_pos);

                                    auto dist = ASI::FastDistance(pos1, pos2);
                                    if ((dist < ASI::GetSpellParameter(spell_data, 2)) && (dist < actual_distance))
                                        actual_distance = dist;
                                }
                            }

                            if (actual_distance != 0xFFFF)
                            {
                                auto some_val1 = ASI::GetUnk1(spell_manager, spell_index);

                                unsigned int result = ASI::GdEffect_Add(spell_manager, spell_index, 3, 2, building_index, 0, 0, 20, some_val1, ASI::SF_Unk1());

                                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 17, spell_index);
                                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 9, ASI::SpellID(spell_manager, spell_index));
                                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 30, ASI::GetSpellParameter(spell_data, 7));
                                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 51, 1);
                                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 47, figure_index2);
                                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 48, 1);
                                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 26, building_index);
                                ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 19, 2);
                            }
                        }

                        ASI::Pointer unk_ptr(*ASI::Pointer(b_area.data + 0x34));
                        if (((unsigned int*)(b_area.data))[0xE] != 0)
                        {
                            ASI::Pointer unk_ptr2(*(unk_ptr[0x4]));
                            ASI::CallClassProc<0x624DD0, unsigned int>
                                (b_area.data + 0x34, (unsigned int)unk_ptr2.ptr);

                            unk_ptr[0x8].AsRef<unsigned int>() = (unsigned int)(unk_ptr.ptr);
                            unk_ptr[0x4].AsRef<unsigned int>() = 0;
                            unk_ptr[0xC].AsRef<unsigned int>() = (unsigned int)(unk_ptr.ptr);
                            ASI::Pointer(b_area.data + 0x38).AsRef<unsigned int>() = 0;

                        }
                        ASI::CallClassProc<0x624CC0>
                            (b_area.data + 0x34);
                    }
                    else  // other auras
                    {
                        ASI::CallClassProc<0x7A23E0, ASI::Pointer, ASI::Pointer, ASI::Pointer>
                            (f_area.data, figuredata, ASI::GetUnknownPointer(spell_manager, 0x40), ASI::GetUnknownPointer(spell_manager, 0x48));

                        auto figure_pos = ASI::GetPosition(figuredata, figure_index2);

                        ASI::CallClassProc<0x7A19E0, unsigned int*, int>
                            (f_area.data, &figure_pos, ASI::GetSpellParameter(spell_data, 2));  // area radius

                        for(auto figure_index3 = ASI::CallClassFunc<0x7A2250, unsigned short>(f_area.data); figure_index3 != 0; figure_index3 = ASI::CallClassFunc<0x7A2250, unsigned short>(f_area.data))
                        {
                            auto unk_sentinel = 1;
                            if (ASI::IsAlive(figuredata, figure_index3))
                            {
                                if (ASI::CheckCompatibleTargets(spell_manager, figuredata, figure_index2, figure_index3, ASI::GetSpellParameter(spell_data, 5)) == 0)
                                    continue;

                                switch (spell_type)
                                {
                                case 88:
                                    if (ASI::HasEffect(spell_manager, figure_index3, 35) == -1)
                                        goto _default;
                                    break;
                                case 89:
                                    if (ASI::HasEffect(spell_manager, figure_index3, 99) == -1)
                                        goto _default;
                                    break;
                                case 94:
                                    if (ASI::HasEffect(spell_manager, figure_index3, 101) == -1)
                                        goto _default;
                                    break;
                                case 95:
                                    if (ASI::HasEffect(spell_manager, figure_index3, 34) == -1)
                                        goto _default;
                                    break;
                                case 97:
                                    if (ASI::HasEffect(spell_manager, figure_index3, 4) == -1)
                                        goto _default;
                                    break;
                                case 98:
                                    if (ASI::HasEffect(spell_manager, figure_index3, 100) == -1)
                                        goto _default;
                                    break;
                                case 102:
                                    if (ASI::HasEffect(spell_manager, figure_index3, 52) == -1)
                                        goto _default;
                                    break;

                                case 103:
                                    if (ASI::GetLifeAction(figuredata, figure_index3) != 0)
                                    {
                                        unsigned int unk_ints[3];
                                        ASI::CallClassProc<0x668DF0, unsigned int(*)[3], unsigned short>
                                            (figuredata, &unk_ints, figure_index3);

                                        for (unsigned int tmp_unk = unk_ints[0]; tmp_unk != unk_ints[1];)
                                        {
                                            unsigned short unk_spell_id = *((unsigned short*)(tmp_unk + 2));
                                            auto unk_spell_type = ASI::CallClassFunc<0x5F7F50, unsigned short, unsigned short>
                                                (ASI::GetUnknownPointer(spell_manager, 0x3C), unk_spell_id);
                                            if ((unk_spell_type == 2) || (unk_spell_type == 144) || (unk_spell_type == 166))
                                            {
                                                ASI::SF_SpellData unk_spell_data;
                                                // get_spell_data
                                                ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>
                                                    (ASI::GetUnknownPointer(spell_manager, 0x3C), &unk_spell_data, unk_spell_id);

                                                if (ASI::GetSpellParameter(unk_spell_data, 0) >= ASI::GetLifeAction(figuredata, figure_index3))
                                                    unk_sentinel = 0;
                                            }

                                            tmp_unk += 4;
                                        }

                                        if (unk_ints[0] != 0)
                                            ASI::CallGlobalProc<0x404C90, unsigned int, unsigned int>
                                                (unk_ints[0], ((unk_ints[2] - unk_ints[0]) >> 2) * 4);

                                        if (unk_sentinel != 0)
                                            goto _default;
                                    }
                                    break;

                                case 104:
                                    if (ASI::HasEffect(spell_manager, figure_index3, 117) == -1)
                                        goto _default;
                                    break;
                                case 107:
                                    if (ASI::HasEffect(spell_manager, figure_index3, 56) == -1)
                                        goto _default;
                                    break;
                                case 110:
                                    if (ASI::HasEffect(spell_manager, figure_index3, 118) == -1)
                                        goto _default;
                                    break;
                                case 111:
                                    if (ASI::HasEffect(spell_manager, figure_index3, 50) == -1)
                                        goto _default;
                                    break;
                                case 113:
                                    if (ASI::HasEffect(spell_manager, figure_index3, 48) == -1)
                                        goto _default;
                                    break;

                                case 114:
                                    if (ASI::HasEffect(spell_manager, figure_index3, 58) == -1)
                                    {
                                        if (ASI::HasEffect(spell_manager, figure_index3, 58) == -1)
                                        {
                                            if ((ASI::GetFlags(figuredata, figure_index3) & 1) != 0)
                                                goto _default;
                                        }
                                    }
                                    break;

                                case 115:
                                    if (ASI::HasEffect(spell_manager, figure_index3, 116) == -1)
                                        goto _default;
                                    break;

                                case 127:
                                    if (ASI::HasEffect(spell_manager, figure_index3, 21) == -1)
                                    {
                                        if (ASI::HasEffect(spell_manager, figure_index3, 161) == -1)
                                        {
                                            if (ASI::HasEffect(spell_manager, figure_index3, 167) == -1)
                                                goto _default;
                                        }
                                    }
                                    break;

                                case 129:
                                    if (ASI::HasEffect(spell_manager, figure_index3, 65) == -1)
                                        goto _default;
                                    break;

                                case 131:
                                    if (ASI::GetCurrentMana(figuredata, figure_index3) != 0)
                                        goto _default;
                                    break;

                                case 192:
                                    if (ASI::HasEffect(spell_manager, figure_index3, 229) == -1)
                                        goto _default;
                                    break;
                                default:
                                _default:

                                    if (unk_otherints[1] == unk_otherints[2])
                                    {
                                        auto unk_val = MAX((unk_otherints[1] - unk_otherints[0]) / 2, 1) * 4;

                                        unsigned int unk_ptr;
                                        if (unk_val == 0)
                                            unk_ptr = 0;
                                        else
                                        {
                                            if (unk_val > 128)
                                                unk_ptr = ASI::CallGlobalCDeclFunc<0xB1111E, unsigned int, unsigned int>(unk_val);
                                            else
                                                unk_ptr = ASI::CallGlobalCDeclFunc<0x402EA0, unsigned int, unsigned int>(unk_val);
                                        }

                                        unsigned int unk_otherval = unk_ptr;
                                        auto unk_diff = unk_otherints[1] - unk_otherints[0];
                                        if (unk_diff != 0)
                                            unk_otherval = ASI::CallGlobalCDeclFunc<0xB11470, unsigned int, unsigned int, unsigned int>
                                            (unk_ptr, unk_otherints[0], unk_diff) + unk_diff;

                                        unsigned int unk_otherval2 = ASI::CallGlobalCDeclFunc<0x78DAB0, unsigned int, unsigned int, unsigned int, unsigned short*>
                                            (unk_otherval, 1, &figure_index3);
                                        ASI::CallClassProc<0x6003F0>(unk_otherints);
                                        ASI::CallClassProc<0x600420, unsigned int, unsigned int, unsigned int>
                                            (unk_otherints, unk_ptr, unk_otherval2, unk_ptr + unk_val);
                                    }
                                    else
                                    {
                                        if (unk_otherints[1] != 0)
                                            *((unsigned short*)(unk_otherints[1])) = figure_index3;
                                        unk_otherints[1] += 2;
                                    }

                                    break;
                                }
                            }
                        }
                        
                    }

                    auto units_in_range = (unk_otherints[1] - unk_otherints[0]) / 2;
                    if (units_in_range != 0)
                    {
                        unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                            (ASI::GetUnknownPointer(spell_manager, 0x4), units_in_range - 1);

                        unsigned short figure_index4 = *((unsigned short*)(unk_otherints[0]+rand_int*2));

                        ASI::SF_PositionQuery pos1(ASI::GetPosition(figuredata, figure_index2));
                        ASI::SF_PositionQuery pos2(ASI::GetPosition(figuredata, figure_index4));

                        auto time = ASI::FastDistance(pos1, pos2) * 7 / 20;
                        if (time == 0)
                            time = 1;

                        auto some_val1 = ASI::GetUnk1(spell_manager, spell_index);

                        ASI::SF_Unk1 params2;
                        ASI::SF_Unk1 params4(1, figure_index4, 0, 0);
                        ASI::CallClassProc<0x7B8C00, ASI::SF_Unk1*, int, unsigned int, ASI::SF_Unk1*>
                            (spell_manager->data, &params2, spell_index, 0, &params4);

                        unsigned int result = ASI::GdEffect_Add(spell_manager, spell_index, 23, 1, figure_index4, 0, 0, time, some_val1, params2);

                        ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 17, spell_index);
                        ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 9, ASI::SpellID(spell_manager, spell_index));
                        ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 28, ASI::GetSpellParameter(spell_data, 6));
                        ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 51, 1);
                        ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 47, figure_index2);
                        ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 48, 1);
                        ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 26, figure_index4);
                        ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 19, 1);

                        ASI::CallClassProc<0x66B260, unsigned short, short>
                            (figuredata, figure_index2, ASI::GetSpellParameter(spell_data, 8));


                    }
                }
                goto end2;
            }
            else
                goto end1;
        }
        else
            goto end2;
    }
    
end1:
    if (figure_index2 != 0)
        ASI::RemoveFlags(figuredata, figure_index2, 0x8000);

    ASI::CallClassProc<0x819520, int>
        (ASI::GetUnknownPointer(spell_manager, 0x14), ASI::GetXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 6));
    ASI::SetXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 6, 0);
    ASI::GdEffect_Remove(spell_manager, spell_index);

end2:
    if (unk_otherints[0] != 0)
    {
        auto unk_anotherval3 = unk_otherints[2] - unk_otherints[0];
        if (unk_anotherval3 <= 80)
        {
            ASI::CallGlobalCDeclProc<0x402B80, unsigned int>(0xD62A7C); // Target parameter for InterlockedExchange
            auto unk_anotherptr = 0xD62A3C + 4 * ((unk_anotherval3 - 1) << 3);
            *((unsigned int*)(unk_otherints[0])) = *((unsigned int*)unk_anotherptr);
            *((unsigned int*)unk_anotherptr) = unk_otherints[0];
            *((unsigned int*)(0xD62A7C)) = 0;
        }
        else
            ASI::CallGlobalCDeclProc<0xB02000, unsigned int>(unk_otherints[0]);
    }

    ASI::Pointer unk_ptr(*ASI::Pointer(f_area.data + 0x34));
    if (((unsigned int*)(f_area.data))[0xE] != 0)
    {
        ASI::Pointer unk_ptr2(*(unk_ptr[0x4]));
        while ((unk_ptr2.ptr != 0))
        {
            ASI::CallClassProc<0x624DD0, unsigned int>
                (f_area.data + 0x34, (unsigned int)unk_ptr2[0xC]);
            ASI::CallClassProc<0x624D90, unsigned int, unsigned int>
                (f_area.data + 0x34, (unsigned int)unk_ptr2.ptr, 1);

            if (((unsigned int)(unk_ptr2.ptr)) != 0)
                ASI::CallGlobalCDeclProc<0x404C90, void*, unsigned int>(unk_ptr2.ptr, 20);

            unk_ptr2 = *unk_ptr2[0x8];
        }

        unk_ptr[0x8].AsRef<unsigned int>() = (unsigned int)(unk_ptr.ptr);
        unk_ptr[0x4].AsRef<unsigned int>() = 0;
        unk_ptr[0xC].AsRef<unsigned int>() = (unsigned int)(unk_ptr.ptr);
        ASI::Pointer(f_area.data + 0x38).AsRef<unsigned int>() = 0;
    }

    if (unk_ptr.ptr != 0)
        ASI::CallGlobalCDeclProc<0x405B20, void*, int>(unk_ptr.ptr, 0x14);
}

void __stdcall effect_befriend(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto spell_index_copy = spell_index;

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);

    auto val3 = ASI::SpellParam4(spell_manager, spell_index);

    if (val3 == 1)
    {
        if (figure_index != 0)
        {
            auto figuredata = ASI::GetFigureManager(spell_manager);

            if (ASI::IsAlive(figuredata, figure_index))
            {
                if (ASI::CallClassFunc<0x7FA9DD, int, unsigned short, unsigned short>(ASI::GetUnknownPointer(spell_manager, 0x24), figure_index2, figure_index) != 0)
                {

                    auto spell_id = ASI::SpellID(spell_manager, spell_index);
                    ASI::SF_SpellData spell_data;
                    // get_spell_data
                    ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, spell_id);

                    if (ASI::GetSpellParameter(spell_data, 0) >= ASI::GetLevel(figuredata, figure_index))
                    {

                        // calculate_resist_chance
                        unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                            (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                        // randint
                        unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                            (ASI::GetUnknownPointer(spell_manager, 0x4), 100);

                        if ((rand_int <= resist)||((ASI::GetFlags(figuredata, figure_index) & 0x100000) != 0))
                        {
                            ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 11, 1, figure_index, 0, 0, 10, ASI::SF_Unk1());
                            ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                                (spell_manager->data, spell_index, figure_index);
                        }
                        else
                        {
                            ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, ASI::SpellToDoCount(spell_manager, spell_index), ASI::SF_Unk1());
                            ASI::CallClassProc<0x7F6A45, unsigned short, unsigned short>
                                (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index, figure_index2);
                        }
                    }
                }
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
}

void __stdcall effect_UNKNOWN1(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);

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

                auto spell_xdata = ASI::SpellXData(spell_manager, spell_index);
                if (ASI::GetXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5) == 0)
                {
                    // draw aggro
                    ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                        (spell_manager->data, spell_index, figure_index);

                    // calculate_resist_chance
                    unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                    // randint
                    unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x4), 100);

                    if (rand_int <= resist)
                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 11, 1, figure_index, 0, 0, 10, ASI::SF_Unk1());
                    else
                    {
                        ASI::GetFlags(figuredata, figure_index) |= 0x40;
                        ASI::SpellFlags(spell_manager, spell_index) |= 0x4;
                        ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 0) / 100;

                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, ASI::SpellToDoCount(spell_manager, spell_index), ASI::SF_Unk1());
                        ASI::AddXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5, 1);
                    }
                }
                else
                    ASI::CallClassProc<0x7B95D0, int, int>(spell_manager->data, spell_index, 0);
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
}

void __stdcall effect_blizzard(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);

    ASI::SF_Unk1 unk1;

    auto val3 = ASI::SpellParam4(spell_manager, spell_index);

    if ((val3 == 4)||(val3 == 5))
    {
        auto figuredata = ASI::GetFigureManager(spell_manager);

        auto spell_id = ASI::SpellID(spell_manager, spell_index);
        ASI::SF_SpellData spell_data;
        // get_spell_data
        ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, spell_id);

        if (ASI::IsAlive(figuredata, figure_index2))
            unk1 = ASI::GetUnk1(spell_manager, spell_index);

        auto spell_xdata = ASI::SpellXData(spell_manager, spell_index);
        if (ASI::GetXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5) == 0)
        {
            ASI::SF_Unk1 params2;
            ASI::SF_Unk1 params4;
            ASI::CallClassProc<0x7B8C00, ASI::SF_Unk1*, int, unsigned int, ASI::SF_Unk1*>
                (spell_manager->data, &params2, spell_index, ASI::GetSpellParameter(spell_data, 2), &params4);

            ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 4, 0, ASI::SpellParamX(spell_manager, spell_index), ASI::SpellParamY(spell_manager, spell_index), 25, params2);
        }
        if (ASI::AddXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5, 1) <= ASI::GetSpellParameter(spell_data, 1))
        {
            auto time = ASI::GetSpellParameter(spell_data, 0) / 100;
            auto drops_per_tick = ASI::GetSpellParameter(spell_data, 3);
            auto counter = 0;
            short unk_x;
            short unk_y;
            if (drops_per_tick != 0)
            {
                do
                {
                    ASI::Pointer unk_ptr = ASI::GetUnknownPointer(spell_manager, 0x48);
                    unsigned short unk_bound = (unsigned short)(unk_ptr[0x11187]);
                    do
                    {
                        do
                        {
                            do
                            {
                                do
                                {
                                    unsigned int rand_int = (unsigned short)(ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                                        (ASI::GetUnknownPointer(spell_manager, 0x4), drops_per_tick * (drops_per_tick + 1) * 4)) * 7;

                                    unk_x = (short)(unk_ptr[rand_int]) + ASI::SpellParamX(spell_manager, spell_index);
                                    unk_y = (short)(unk_ptr[rand_int + 2]) + ASI::SpellParamY(spell_manager, spell_index);
                                } while (unk_x <= 0);
                            } while (unk_x >= unk_bound - 1);
                        } while (unk_y <= 0);
                    } while (unk_y >= unk_bound - 1);

                    unsigned short rand_int2 = (unsigned short)(ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x4), time));

                    ASI::SF_Unk1 some_val1 = ASI::GetUnk1(spell_manager, spell_index);
                    unsigned int result = ASI::GdEffect_Add(spell_manager, spell_index, 15, 4, 0, unk_x, unk_y, 20, some_val1, ASI::SF_Unk1(), rand_int2);

                    ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 17, spell_index);
                    ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 9, spell_id);
                    ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 28, ASI::GetSpellParameter(spell_data, 4));
                    ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 29, 16);
                    ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 47, figure_index2);
                    ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 48, ASI::SpellParam3(spell_manager, spell_index));
                    ASI::XDataListAdd(ASI::GetUnknownPointer(spell_manager, 0x14), result, 51, 0);

                    counter += 1;
                } while (counter < drops_per_tick);

            }

            ASI::SpellToDoCount(spell_manager, spell_index) = time;
            return;
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
}

void __stdcall effect_brilliance(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);

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
                if (ASI::GetXData(ASI::GetXDataPointer(spell_manager), val5, 5) == 0)
                {
                    if(ASI::CallClassFunc<0x7B8E60, int, int>
                        (spell_manager->data, spell_index) != 0)
                    {
                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, ASI::GetSpellParameter(spell_data, 1) / 100, ASI::SF_Unk1());
                        ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 1) / 100;
                        ASI::AddXData(ASI::GetXDataPointer(spell_manager), val5, 5, 1);

                        auto mana = ASI::GetCurrentManaMax(figuredata, figure_index);
                        ASI::CallClassProc<0x66B410, unsigned short, char>(figuredata, figure_index, (char)ASI::GetSpellParameter(spell_data, 0));
                        ASI::CallClassProc<0x7FC67B, unsigned short>(ASI::GetUnknownPointer(spell_manager, 0x24), figure_index);
                        ASI::CallClassProc<0x66D1C0, unsigned short, unsigned short>(figuredata, figure_index, mana);
                        return;
                    }
                }
                else
                {
                    auto mana = ASI::GetCurrentManaMax(figuredata, figure_index);
                    ASI::CallClassProc<0x66B410, unsigned short, char>(figuredata, figure_index, -((char)ASI::GetSpellParameter(spell_data, 0)));
                    ASI::CallClassProc<0x7FC67B, unsigned short>(ASI::GetUnknownPointer(spell_manager, 0x24), figure_index);
                    ASI::CallClassProc<0x66D1C0, unsigned short, unsigned short>(figuredata, figure_index, mana);
                }
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
}

void __stdcall effect_cannibalize(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);

    auto val3 = ASI::SpellParam4(spell_manager, spell_index);

    if (val3 == 1)
    {
        if (figure_index != 0)
        {
            auto figuredata = ASI::GetFigureManager(spell_manager);

            if (ASI::IsAlive(figuredata, figure_index))
            {
                if (figure_index == figure_index2)
                {
                    auto spell_id = ASI::SpellID(spell_manager, spell_index);
                    ASI::SF_SpellData spell_data;
                    // get_spell_data
                    ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, spell_id);

                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, 25, ASI::SF_Unk1());
                    ASI::CallClassProc<0x66B260, unsigned short, short>(figuredata, figure_index2, -ASI::GetSpellParameter(spell_data, 1));
                    ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index2, figure_index, ASI::GetSpellParameter(spell_data, 0), 1, 0, 0);
                }
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
}

void __stdcall effect_chain(int spell_index)
{
    /*ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);

    ASI::AreaEntityQuery figure_query;

    ASI::Pointer unk_ptr;

    if (ASI::SpellParam4(spell_manager, spell_index) == 1)
    {
        auto figuredata = ASI::GetFigureManager(spell_manager);

        int unk_sentinel = 0;

        if (ASI::IsAlive(figuredata, figure_index2))
        {
            auto spell_id = ASI::SpellID(spell_manager, spell_index);
            ASI::SF_SpellData spell_data;
            // get_spell_data
            ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, spell_id);

            ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 0) / 100;
            unsigned int unk_pos = ASI::GetPosition(figuredata, figure_index2);

            auto spell_xdata = ASI::SpellXData(spell_manager, spell_index);       // byte ptr one time, word ptr another
            unsigned short figure_index3;
            if (ASI::AddXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5, 1) != 1)
            {
                ASI::Pointer result = ASI::Pointer(ASI::CallClassFunc<0x60DC20, int*, int*>
                    (spell_manager->data + 21686, &spell_index));
                ASI::Pointer unk_ptr = ASI::Pointer(*((int**)(spell_manager->data + 21686)));
                if (result != unk_ptr)
                {
                    auto unk_count = ((unsigned int)result[0x18] - (unsigned int)result[0x14]) / 2;
                    if (unk_count != 0)
                    {
                        auto unk_figure_index = *((unsigned short*)((unsigned int)result[0x14] + (unk_count - 1) * 2));
                        if (unk_figure_index != 0)
                        {
                            if (ASI::GetInUse(figuredata, unk_figure_index) != -1)
                            {
                                unk_pos = ASI::GetPosition(figuredata, unk_figure_index);
                            }
                        }
                    }
                }

                ASI::CallClassProc<0x7A2120, short, short, short, short>
                    (figure_query.data, 0, 0, 1023, 1023);
                ASI::CallClassProc<0x7A23E0, ASI::Pointer, ASI::Pointer, ASI::Pointer>
                    (figure_query.data, figuredata, ASI::GetUnknownPointer(spell_manager, 0x40), ASI::GetUnknownPointer(spell_manager, 0x48));

                for (int unk_counter = 0; unk_counter < 3; unk_counter++)
                {
                    ASI::CallClassProc<0x7A19E0, unsigned int*, int>
                        (figure_query.data, &unk_pos, ASI::GetSpellParameter(spell_data, 1));  // area radius

                    for (figure_index3 = ASI::CallClassFunc<0x7A2250, unsigned short>(figure_query.data); figure_index3 != 0; figure_index3 = ASI::CallClassFunc<0x7A2250, unsigned short>(figure_query.data))
                    {
                        ASI::SF_PositionQuery pos1(unk_pos);
                        ASI::SF_PositionQuery pos2(ASI::GetPosition(figuredata, figure_index3));

                        auto dist = ASI::FastDistance(pos1, pos2);

                        if (unk_counter == 0)
                        {
                            if ((dist < 3) || (dist > 5))
                                break;
                        }
                        else if (unk_counter == 1)
                        {
                            if (dist > 2)
                                break;
                        }

                        if (ASI::IsAlive(figuredata, figure_index3))
                        {
                            if (ASI::CheckCompatibleTargets(spell_manager, figuredata, figure_index2, figure_index3, ASI::GetSpellParameter(spell_data, 2)) != 0)
                            {
                                unk_ptr = ASI::Pointer(*((int**)(spell_manager->data + 21686)));
                                if (result != unk_ptr)
                                {
                                    auto unk_count = ((unsigned int)result[0x18] - (unsigned int)result[0x14]) / 2;
                                    if (unk_count != 0)
                                    {
                                        for (int i = 0; i < unk_count; i++)
                                        {
                                            if ((unsigned short)((*result[0x14])[2 * i]) == figure_index3)
                                                break;
                                        }
                                        unk_sentinel = 1;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                ASI::Pointer unk_ptr(*ASI::Pointer(figure_query.data + 0x34));
                if (((unsigned int*)(figure_query.data))[0xE] != 0)
                {
                    ASI::Pointer unk_ptr2(*(unk_ptr[0x4]));

                    ASI::CallClassProc<0x624DD0, unsigned int>
                        (figure_query.data + 0x34, (unsigned int)unk_ptr2.ptr);

                    unk_ptr[0x8].AsRef<unsigned int>() = (unsigned int)(unk_ptr.ptr);
                    unk_ptr[0x4].AsRef<unsigned int>() = 0;
                    unk_ptr[0xC].AsRef<unsigned int>() = (unsigned int)(unk_ptr.ptr);
                    ASI::Pointer(figure_query.data + 0x38).AsRef<unsigned int>() = 0;
                }
                ASI::CallClassProc<0x624CC0>
                    (figure_query.data + 0x34);
            }
            else
                figure_index3 = figure_index;

            if ((ASI::GetSpellParameter(spell_data, 2) >= 1) && (ASI::GetSpellParameter(spell_data, 2) <= 3))
            {
                if (unk_sentinel == 0)
                {
                    if (ASI::IsAlive(figuredata, figure_index2))
                    {
                        // calculate_resist_chance
                        unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                            (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index3, ASI::GetSpellIndex(spell_data));
                        // randint
                        unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                            (ASI::GetUnknownPointer(spell_manager, 0x4), 100);

                        if (rand_int <= resist)
                        {
                            ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 11, 1, figure_index, 0, 0, 10, ASI::SF_Unk1());
                        }
                        else
                        {
                            tbd;
                        }
                    }
                }
            }
        }
    }*/
}

void __stdcall effect_charisma(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);

    if (ASI::SpellParam4(spell_manager, spell_index) == 1)
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
                if (ASI::GetXData(ASI::GetXDataPointer(spell_manager), val5, 5) == 0)
                {
                    if (ASI::CallClassFunc<0x7B8E60, int, int>
                        (spell_manager->data, spell_index) != 0)
                    {
                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, ASI::GetSpellParameter(spell_data, 1) / 100, ASI::SF_Unk1());
                        ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 1) / 100;
                        ASI::AddXData(ASI::GetXDataPointer(spell_manager), val5, 5, 1);

                        ASI::CallClassProc<0x66AF00, unsigned short, char>(figuredata, figure_index, ASI::GetSpellParameter(spell_data, 0));
                        return;
                    }
                }
                else
                {
                    ASI::CallClassProc<0x66AF00, unsigned short, char>(figuredata, figure_index, -ASI::GetSpellParameter(spell_data, 0));
                }
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
}

void __stdcall effect_chill_resistance(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);

    if (ASI::SpellParam4(spell_manager, spell_index) == 1)
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
                if (ASI::GetXData(ASI::GetXDataPointer(spell_manager), val5, 5) == 0)
                {
                    // calculate_resist_chance
                    unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                    // randint
                    unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x4), 100);

                    if (rand_int <= resist)
                    {
                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 11, 1, figure_index, 0, 0, 10, ASI::SF_Unk1());
                    }
                    else
                    {
                        if (ASI::CallClassFunc<0x7B8E60, int, int>
                            (spell_manager->data, spell_index) != 0)
                        {
                            ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, ASI::GetSpellParameter(spell_data, 1) / 100, ASI::SF_Unk1());
                            ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 1) / 100;
                            ASI::AddXData(ASI::GetXDataPointer(spell_manager), val5, 5, 1);
                            ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                                (spell_manager->data, spell_index, figure_index);

                            ASI::CallClassProc<0x66B2C0, unsigned short, char>(figuredata, figure_index, -ASI::GetSpellParameter(spell_data, 0));
                            return;
                        }
                    }
                }
                else
                {
                    ASI::CallClassProc<0x66B2C0, unsigned short, char>(figuredata, figure_index, ASI::GetSpellParameter(spell_data, 0));
                }
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
}

void __stdcall effect_confuse(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);

    if (ASI::SpellParam4(spell_manager, spell_index) == 1)
    {
        if (figure_index != 0)
        {
            auto figuredata = ASI::GetFigureManager(spell_manager);

            if (ASI::IsAlive(figuredata, figure_index))
            {
                if (ASI::CallClassFunc<0x7FA9DD, int, unsigned short, unsigned short>
                    (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index2, figure_index) != 0)
                {
                    auto spell_id = ASI::SpellID(spell_manager, spell_index);
                    ASI::SF_SpellData spell_data;
                    // get_spell_data
                    ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, spell_id);

                    auto val5 = ASI::SpellXData(spell_manager, spell_index);
                    if (ASI::AddXData(ASI::GetXDataPointer(spell_manager), val5, 5, 1) == 1)
                    {
                        // draw aggro
                        ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                            (spell_manager->data, spell_index, figure_index);
                        // calculate_resist_chance
                        unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                            (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                        // randint
                        unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                            (ASI::GetUnknownPointer(spell_manager, 0x4), 100);

                        if (rand_int <= resist)
                        {
                            ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 11, 1, figure_index, 0, 0, 10, ASI::SF_Unk1());
                        }
                        else
                        {
                            if (ASI::CallClassFunc<0x7B8E60, int, int>
                                (spell_manager->data, spell_index) != 0)
                            {
                                ASI::GetFlags(figuredata, figure_index) |= 0x60;
                                ASI::SpellFlags(spell_manager, spell_index) |= 0x6;
                                auto charisma = ASI::GetCurrentCharisma(figuredata, figure_index);
                                auto duration = (charisma + 15) * (ASI::GetSpellParameter(spell_data, 0)) / 50;
                                ASI::SpellToDoCount(spell_manager, spell_index) += duration / 100;

                                ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, ASI::SpellToDoCount(spell_manager, spell_index), ASI::SF_Unk1());
                                return;
                            }
                        }
                    }
                    else
                    {
                        ASI::SpellFlags(spell_manager, spell_index) &= 0xFD;

                        ASI::SF_Unk1 unk = ASI::GetUnk2(spell_manager, spell_index);
                        if (unk.c1 == 1)
                        {
                            auto spelljobnode = ASI::GetSpellJobStartNode(figuredata, unk.s1);
                            if (spelljobnode != 0)
                            {
                                while (true)
                                {
                                    ASI::Pointer some_pointer3(ASI::GetUnknownPointer(spell_manager, 0x10));
                                    ASI::Pointer some_pointer31(*some_pointer3[0]);

                                    if (ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode) != spell_index)
                                    {
                                        if ((ASI::SpellFlags(spell_manager, ASI::GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode)) & 2) != 0)
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
                            else
                                ASI::RemoveFlags(figuredata, figure_index2, 0x20);
                        }

                        ASI::CallClassProc<0x7B95D0, unsigned int, unsigned int*>(spell_manager->data, spell_index, 0);
                    }
                }
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
}

void __stdcall effect_dark_banishing(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);

    if (ASI::SpellParam4(spell_manager, spell_index) == 1)
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
                if (ASI::GetXData(ASI::GetXDataPointer(spell_manager), val5, 5) == 0)
                {
                    // calculate_resist_chance
                    unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                    // randint
                    unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x4), 100);

                    if (rand_int <= resist)
                    {
                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 11, 1, figure_index, 0, 0, 10, ASI::SF_Unk1());
                    }
                    else
                    {
                        if (ASI::CallClassFunc<0x7B8E60, int, int>
                            (spell_manager->data, spell_index) != 0)
                        {
                            ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, ASI::GetSpellParameter(spell_data, 1) / 100, ASI::SF_Unk1());
                            ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 1) / 100;
                            ASI::AddXData(ASI::GetXDataPointer(spell_manager), val5, 5, 1);
                            ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                                (spell_manager->data, spell_index, figure_index);

                            ASI::CallClassProc<0x66B290, unsigned short, char>(figuredata, figure_index, -ASI::GetSpellParameter(spell_data, 0));
                            return;
                        }
                    }
                }
                else
                {
                    ASI::CallClassProc<0x66B290, unsigned short, char>(figuredata, figure_index, ASI::GetSpellParameter(spell_data, 0));
                }
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
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
                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 11, 1, figure_index, 0, 0, 10, ASI::SF_Unk1());
                }
                else
                {
                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, 25, ASI::SF_Unk1());

                    auto dmg = (unsigned short)ASI::GetSpellParameter(spell_data, 0);

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
                }
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
}

void __stdcall effect_decay(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);

    if (ASI::SpellParam4(spell_manager, spell_index) == 1)
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
                if (ASI::AddXData(ASI::GetXDataPointer(spell_manager), val5, 5, 1) == 1)
                {
                    // calculate_resist_chance
                    unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                    // randint
                    unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x4), 100);

                    if (rand_int <= resist)
                    {
                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 11, 1, figure_index, 0, 0, 25, ASI::SF_Unk1());
                    }
                    else
                    {
                        if (ASI::CallClassFunc<0x7B8E60, int, int>
                            (spell_manager->data, spell_index) != 0)
                        {
                            short duration = (ASI::GetSpellParameter(spell_data, 0) * ASI::GetSpellParameter(spell_data, 1)) / 100;
                            ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, duration, ASI::SF_Unk1());
                            ASI::SpellToDoCount(spell_manager, spell_index) = duration;
                            ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                                (spell_manager->data, spell_index, figure_index);

                            ASI::CallClassProc<0x66AE70, unsigned short, char>(figuredata, figure_index, -ASI::GetSpellParameter(spell_data, 2));
                            return;
                        }
                    }
                }
                else
                {
                    ASI::CallClassProc<0x66AE70, unsigned short, char>(figuredata, figure_index, ASI::GetSpellParameter(spell_data, 2));
                }
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
}

void __stdcall effect_demoralization(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);

    if (ASI::SpellParam4(spell_manager, spell_index) == 1)
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
                if (ASI::GetXData(ASI::GetXDataPointer(spell_manager), val5, 5) == 0)
                {
                    ASI::CallClassProc<0x7B8AC0, unsigned int, unsigned int>
                        (spell_manager->data, spell_index, figure_index);
                    if (ASI::GetLevel(figuredata, figure_index) < ASI::GetSpellParameter(spell_data, 1))
                    {
                        // calculate_resist_chance
                        unsigned short resist = ASI::CallClassFunc<0x7E42A0, unsigned int, unsigned int, unsigned int, unsigned int>
                            (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, ASI::GetSpellIndex(spell_data));
                        // randint
                        unsigned short rand_int = ASI::CallClassFunc<0x671C70, unsigned int, unsigned int>
                            (ASI::GetUnknownPointer(spell_manager, 0x4), 100);

                        if (rand_int > resist)
                        {
                            ASI::SpellToDoCount(spell_manager, spell_index) += (ASI::GetSpellParameter(spell_data, 0) )/ 100;

                            ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, ASI::SpellToDoCount(spell_manager, spell_index), ASI::SF_Unk1());
                            ASI::AddXData(ASI::GetXDataPointer(spell_manager), val5, 5, 1);

                            ASI::GetFlags(figuredata, figure_index) |= 0x40;
                            ASI::SpellFlags(spell_manager, spell_index) |= 0x4;
                        }
                        else
                            ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 11, 1, figure_index, 0, 0, 10, ASI::SF_Unk1());
                    }
                    else
                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 11, 1, figure_index, 0, 0, 10, ASI::SF_Unk1());
                }
                else
                    ASI::CallClassProc<0x7B95D0, int, int>(spell_manager->data, spell_index, 0);
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
}

void __stdcall effect_detect_magic(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);
    auto figure_offset2 = figure_index2 * 691;

    ASI::AreaEntityQuery area;

    ASI::CallClassProc<0x7A2410, short, short, short, short>
        (area.data, 0, 0, 1023, 1023);

    if (ASI::SpellParam4(spell_manager, spell_index) == 1)
    {
        auto figuredata = ASI::GetFigureManager(spell_manager);

        ASI::SF_SpellData spell_data;
        // get_spell_data
        ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>
            (ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, ASI::SpellID(spell_manager, spell_index));

        ASI::SF_Unk1 params2;
        ASI::SF_Unk1 params4;
        ASI::CallClassProc<0x7B8C00, ASI::SF_Unk1*, int, unsigned int, ASI::SF_Unk1*>
            (spell_manager->data, &params2, spell_index, ASI::GetSpellParameter(spell_data, 0), &params4);

        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 4, 0, ASI::GetX(figuredata, figure_index2), ASI::GetY(figuredata, figure_index2), 20, params2);

        ASI::CallClassProc<0x7A26C0, ASI::Pointer, ASI::Pointer, ASI::Pointer>
            (area.data, ASI::GetObjectManager(spell_manager), ASI::GetUnknownPointer(spell_manager, 0x40), ASI::GetUnknownPointer(spell_manager, 0x48));

        unsigned int pos = ASI::GetPosition(figuredata, figure_index2);
        ASI::FindAllEntitiesInArea(area, pos, ASI::GetSpellParameter(spell_data, 0));

        // not finished
    }

    ASI::Pointer unk_ptr(*ASI::Pointer(area.data + 0x34));

    ASI::GdEffect_Remove(spell_manager, spell_index);

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
        ASI::CallGlobalCDeclProc<0x405B20, void*, int>(unk_ptr.ptr, 0x14);
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

                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 11, 1, figure_index, 0, 0, 10, ASI::SF_Unk1());

                        goto end;
                    }
                    else
                    {
                        ASI::SF_Unk1 params1(1, figure_index, 0, 0);
                        ASI::SF_Unk1 params3 = ASI::GetUnk1(spell_manager, spell_index);
                        ASI::SF_Unk1 params2;
                        ASI::CallClassProc<0x649670, ASI::SF_Unk1*, ASI::SF_Unk1*, ASI::SF_Unk1*>
                            (ASI::GetUnknownPointer(spell_manager, 0x44), &params2, &params3, &params1);

                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, params2.c1, params2.s1, params2.s2, params2.s3, 10, ASI::SF_Unk1());
                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 4, params2.c1, params2.s1, params2.s2, params2.s3, 25, ASI::SF_Unk1());

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

                        ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                            (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index2, figure_index, dmg, 1, 0, 0);

                        return;
                    }
                }
                else
                {
                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 4, 1, figure_index, 0, 0, 25, ASI::SF_Unk1());

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
    ASI::GdEffect_Remove(spell_manager, spell_index);
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

                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 11, 1, figure_index, 0, 0, 10, ASI::SF_Unk1());
                }
                else
                {
                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, 25, ASI::SF_Unk1());

                    auto dmg = (unsigned short)ASI::GetSpellParameter(spell_data, 0);

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
                }
            }
        }
    }

    ASI::GdEffect_Remove(spell_manager, spell_index);
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
                int heal = (ASI::GetSpellParameter(spell_data, 0) * (charisma + 15)) / 50;

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
                    ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, 35, ASI::SF_Unk1());

                    if (heal > 32766)
                        heal = 32766;

                    short neg_heal = -heal;
                    ASI::CallClassProc<0x66B230, unsigned short, short>(figuredata, figure_index, neg_heal);
                }
            }
        }
    }
end:
    ASI::GdEffect_Remove(spell_manager, spell_index);
    return;
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

                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 11, 1, figure_index, 0, 0, 10, ASI::SF_Unk1());

                        goto end;
                    }
                    else
                    {
                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, ASI::GetSpellParameter(spell_data, 2) / 100, ASI::SF_Unk1());
                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 4, 1, figure_index, 0, 0, 25, ASI::SF_Unk1());

                        auto dmg = (unsigned short)ASI::GetSpellParameter(spell_data, 0);

                        ASI::SetXData(ASI::GetXDataPointer(spell_manager), val5, 5, (ASI::GetSpellParameter(spell_data, 2) / ASI::GetSpellParameter(spell_data, 1)) - 1);
                        ASI::SetXData(ASI::GetXDataPointer(spell_manager), val5, 18, dmg);

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

                        ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                            (ASI::GetUnknownPointer(spell_manager, 0x24), figure_index2, figure_index, dmg, 1, 0, 0);

                        if (ASI::IsAlive(figuredata, figure_index))
                        {
                            figuredata[figure_offset + 0x16].AsRef<unsigned int>() |= 0x120;
                            ASI::SpellFlags(spell_manager, spell_index) |= 0x3;
                            unsigned char result = ASI::CallClassFunc<0x740700, unsigned char>(figuredata[figure_offset + 0x20B]);
                            if ((result >= 9) && (result <= 12))
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
                    ASI::Pointer some_pointer1000 = ASI::GetUnknownPointer(spell_manager, 0x4);
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
                        ASI::SpellFlags(spell_manager, spell_index) &= 0xFD;
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
                        return;
                }
            }
        }
    }
end:
    ASI::GdEffect_Remove(spell_manager, spell_index);
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
                                ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index, 0, 0, (ASI::GetSpellParameter(spell_data, 3) * ASI::GetSpellParameter(spell_data, 4)) / 100, ASI::SF_Unk1());

                                figuredata[figure_offset + 0x16].AsRef<unsigned int>() |= 0x140;
                                ASI::SpellFlags(spell_manager, spell_index) |= 0x5;
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
                            ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 11, 1, figure_index, 0, 0, 10, ASI::SF_Unk1());

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
    ASI::GdEffect_Remove(spell_manager, spell_index);
    return;
}

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

            if (ASI::IsAlive(figuredata, figure_index))
            {
                if (ASI::CallClassFunc<0x7FA8F9, unsigned int, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x24), figure_index) != 0)
                {
                    auto spell_id = ASI::SpellID(spell_manager, spell_index);
                    ASI::SF_SpellData spell_data;
                    // get_spell_data
                    ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, spell_id);
                    if (ASI::IsAlive(figuredata, figure_index2))
                        some_val1 = ASI::GetUnk1(spell_manager, spell_index);

                    auto val5 = ASI::SpellXData(spell_manager, spell_index);
                    unsigned short figure_index3 = ASI::GetXData(ASI::GetXDataPointer(spell_manager), val5, 45);
                    if ((figure_index3 == 0) || (!ASI::IsAlive(figuredata, figure_index3)))
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
                        ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 11, 1, figure_index, 0, 0, 10, ASI::SF_Unk1());
                    }
                    else
                    {
                        ASI::SF_PositionQuery val6(figuredata[figure_offset + 0x2].ptr);
                        ASI::SF_PositionQuery val7(figuredata[figure_index3 * 691 + 0x2].ptr);

                        unsigned short val15 = (ASI::FastDistance(val6, val7) * 7) / 25;

                        if ((val15 & 0xFFFF) == 0)
                            val15 = 1;

                        auto spell_damage = ASI::GetSpellParameter(spell_data, 0);
                        // reduce_dmg_by_armor
                        unsigned int reduced_dmg = ASI::CallClassFunc<0x7E3AD0, unsigned int, unsigned int, unsigned int, unsigned int>
                            (ASI::GetUnknownPointer(spell_manager, 0x2C), figure_index2, figure_index, spell_damage);

                        if (ASI::IsAlive(figuredata, figure_index2))
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

                        auto figure_unitid = ASI::GetFigureID(figuredata, figure_index);
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

                        //GdEffect::Add
                        unsigned int result = ASI::GdEffect_Add(spell_manager, spell_index, 3, 1, figure_index, 0, 0, val15, some_val1, ASI::SF_Unk1());
                            
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
            }
        }
    }

end:
    ASI::GdEffect_Remove(spell_manager, spell_index);

}

void __stdcall effect_summon(int spell_index)
{
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    unsigned char some_data[12];
    for (int i = 0; i < 12; i++)
        some_data[i] = 0xFF;

    auto figure_index = ASI::SpellTarget(spell_manager, spell_index);
    auto figure_index2 = ASI::SpellSource(spell_manager, spell_index);

    auto figuredata = ASI::GetFigureManager(spell_manager);
    
    ASI::SF_SpellData spell_data;
    // get_spell_data
    ASI::CallClassProc<0x5F7E30, ASI::SF_SpellData*, unsigned int>
        (ASI::GetUnknownPointer(spell_manager, 0x3C), &spell_data, ASI::SpellID(spell_manager, spell_index));
    
    auto spell_xdata = ASI::SpellXData(spell_manager, spell_index);
    if (ASI::GetXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5) == 0)
    {
        int summon_index = ASI::CreateFigureAsSummon(ASI::GetUnknownPointer(spell_manager, 0x24), figure_index2, ASI::GetSpellParameter(spell_data, 2));
        if (summon_index != 0)
        {
            // START EDIT
            /*if ((ASI::SpellType(spell_manager, spell_index) == 106)
                || (ASI::SpellType(spell_manager, spell_index) == 109)
                || (ASI::SpellType(spell_manager, spell_index) == 188))
            {
                int old_spell_index = ASI::HasEffectExcept(spell_manager, figure_index2, 106, spell_index);
                unsigned short figure_index3;
                if (old_spell_index != -1)
                {
                    figure_index3 = ASI::SpellTarget(spell_manager, old_spell_index);
                    ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x24), 0, figure_index3, 0x7FFF, 1, 0, 0);
                }

                old_spell_index = ASI::HasEffectExcept(spell_manager, figure_index2, 109, spell_index);
                if (old_spell_index != -1)
                {
                    figure_index3 = ASI::SpellTarget(spell_manager, old_spell_index);
                    ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x24), 0, figure_index3, 0x7FFF, 1, 0, 0);
                }

                old_spell_index = ASI::HasEffectExcept(spell_manager, figure_index2, 188, spell_index);
                if (old_spell_index != -1)
                {
                    figure_index3 = ASI::SpellTarget(spell_manager, old_spell_index);
                    ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                        (ASI::GetUnknownPointer(spell_manager, 0x24), 0, figure_index3, 0x7FFF, 1, 0, 0);
                }
            }*/
            // END EDIT

            ASI::AddSpellEffect(ASI::GetUnknownPointer(spell_manager, 0x24), summon_index, spell_index);
            ASI::AddXData(ASI::GetXDataPointer(spell_manager), spell_xdata, 5, 1);
            ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 0) / 100;

            ASI::SpellParam4(spell_manager, spell_index) = 1;
            ASI::SpellTarget(spell_manager, spell_index) = summon_index;
            ASI::SpellParamX(spell_manager, spell_index) = 0;
            ASI::SpellParamY(spell_manager, spell_index) = 0;
        }
        else
        {
            ASI::GdEffect_Remove(spell_manager, spell_index);
        }
    }
    else
    {
        if (ASI::GetCurrentMana(figuredata, figure_index2) >= ASI::GetSpellParameter(spell_data, 1))
        {
            ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 0) / 100;
            ASI::ReduceCurrentMana(figuredata, figure_index2, ASI::GetSpellParameter(spell_data, 1));
        }
        else
        {
            ASI::GdEffect_VisualOnlyAddEx(spell_manager, spell_index, 3, 1, figure_index, 0, 0, 25, ASI::SF_Unk1(), ASI::SF_Unk1(1, figure_index, 0, 0));
            // kill summoned unit
            ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                (ASI::GetUnknownPointer(spell_manager, 0x24), 0, figure_index, 0x7FFF, 1, 0, 0);
        }
    }
}

void __stdcall effect_short_lived_NEW(int spell_index)
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
                    ASI::SF_Unk1 params2;
                    ASI::SF_Unk1 params4;
                    ASI::CallClassProc<0x7B8C00, ASI::SF_Unk1*, int, unsigned int, ASI::SF_Unk1*>
                        (spell_manager->data, &params2, spell_index, ASI::GetSpellParameter(spell_data, 3), &params4);

                    //ASI::GdEffect_VisualOnlyAdd(spell_manager, spell_index, 3, 1, figure_index2, 0, 0, 150, params2);

                    ASI::SpellToDoCount(spell_manager, spell_index) = ASI::GetSpellParameter(spell_data, 0) / 100;
                    //ASI::SpellFlags(spell_manager, spell_index) |= 0x2;
                    //figuredata[figure_offset2 + 0x16].AsRef<unsigned int>() |= 0x20;

                    return;
                }
                else
                {
                    if (ASI::IsAlive(figuredata, figure_index2))
                    {
                        // kill self
                        ASI::CallClassProc<0x7EE9C6, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>
                            (ASI::GetUnknownPointer(spell_manager, 0x24), 0, figure_index2, 0x7FFF, 1, 0, 0);
                    }
                }
            }
        }
    }
end:
    ASI::GdEffect_Remove(spell_manager, spell_index);
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
                case ASI::SL_ABILITY_BENEFACTIONS: ASI::CallClassProc<0x7BC9D0, unsigned int>(spell_manager, spell_index); break; //ASI::CallAsClassProc<unsigned int>(effect_ability_benefactions, spell_manager, spell_index); break;//ASI::CallClassProc<0x7BC9D0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ABILITY_BERSERK: ASI::CallClassProc<0x7BD040, unsigned int>(spell_manager, spell_index); break; //ASI::CallAsClassProc<unsigned int>(effect_ability_berserk, spell_manager, spell_index); break;//ASI::CallClassProc<0x7BD040, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ABILITY_BOONS: ASI::CallClassProc<0x7BD2B0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ABILITY_CRITICAL_HITS: ASI::CallClassProc<0x7BD4C0, unsigned int>(spell_manager, spell_index); break; //ASI::CallAsClassProc<unsigned int>(effect_ability_critical_hits, spell_manager, spell_index); break;//ASI::CallClassProc<0x7BD4C0, unsigned int>(spell_manager, spell_index); break;
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
                case ASI::SL_ALMIGHTNESS_MENTAL: ASI::CallClassProc<0x7C06F0, unsigned int>(spell_manager, spell_index); break; //ASI::CallAsClassProc<unsigned int>(effect_almightness_mental, spell_manager, spell_index); break;//ASI::CallClassProc<0x7C06F0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ALMIGHTNESS_WHITE: ASI::CallClassProc<0x7C09F0, unsigned int>(spell_manager, spell_index); break; //ASI::CallAsClassProc<unsigned int>(effect_almightness_white, spell_manager, spell_index); break; //ASI::CallClassProc<0x7C09F0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_AMOK: ASI::CallClassProc<0x7C0CB0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ARROWTOWER: ASI::CallClassProc<0x7C0FA0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_DARKNESS_AREA: ASI::CallClassProc<0x7C1350, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ASSISTANCE: ASI::CallClassProc<0x7C17F0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_AURA: ASI::CallClassProc<0x7C1C50, unsigned int>(spell_manager, spell_index); break; //ASI::CallAsClassProc<unsigned int>(effect_aura, spell_manager, spell_index); break; //ASI::CallClassProc<0x7C1C50, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_BEFRIEND: ASI::CallClassProc<0x7C1A10, unsigned int>(spell_manager, spell_index); break; //ASI::CallAsClassProc<unsigned int>(effect_befriend, spell_manager, spell_index); break; //ASI::CallClassProc<0x7C1A10, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_UNKNOWN1: ASI::CallClassProc<0x7C2AD0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_BLIZZARD: ASI::CallClassProc<0x7C2D40, unsigned int>(spell_manager, spell_index); break; //ASI::CallAsClassProc<unsigned int>(effect_blizzard, spell_manager, spell_index); break; //ASI::CallClassProc<0x7C2D40, unsigned int>(spell_manager, spell_index); break;
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
                case ASI::SL_DEATH: ASI::CallClassProc<0x7C6270, unsigned int>(spell_manager, spell_index); break; //ASI::CallAsClassProc<unsigned int>(effect_death, spell_manager, spell_index); break;
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
                case ASI::SL_DOMINATE_BREAK: ASI::CallClassProc<0x7C9E40, unsigned int>(spell_manager, spell_index); break;
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
                case ASI::SL_FIREBURST: ASI::CallClassProc<0x7CD660, unsigned int>(spell_manager, spell_index); break; //ASI::CallAsClassProc<unsigned int>(effect_fireburst, spell_manager, spell_index); break;//ASI::CallClassProc<0x7CD660, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FIRE_RESISTANCE: ASI::CallClassProc<0x7CDB10, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FIRESHIELD1: ASI::CallClassProc<0x7CDD50, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FIRESHIELD2: ASI::CallClassProc<0x7CDFB0, unsigned int>(spell_manager, spell_index); break;//ASI::CallAsClassProc<unsigned int>(effect_fireshield2, spell_manager, spell_index); break;//ASI::CallClassProc<0x7CDFB0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FLEXIBILITY: ASI::CallClassProc<0x7CE1A0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FLEXIBILITY_AREA: ASI::CallClassProc<0x7CE400, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FOG: ASI::CallClassProc<0x7CE7F0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FORGET: ASI::CallClassProc<0x7CEBB0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FREEZE: ASI::CallClassProc<0x7CEE80, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_FREEZE_AREA: ASI::CallClassProc<0x7CF150, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_GUARD: ASI::CallClassProc<0x7CF5D0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_HALLOW: ASI::CallClassProc<0x7CF780, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_HEALING: ASI::CallClassProc<0x7CFCC0, unsigned int>(spell_manager, spell_index); break;//ASI::CallAsClassProc<unsigned int>(effect_healing, spell_manager, spell_index); break;//ASI::CallClassProc<0x7CFCC0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_UNKNOWN: ASI::CallClassProc<0x7CFED0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_HEALING_AREA: ASI::CallClassProc<0x7D0070, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_HEALING_AURA: ASI::CallClassProc<0x7D04D0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_HOLY_TOUCH: ASI::CallClassProc<0x7D0630, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ICESHIELD: ASI::CallClassProc<0x7D0740, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_HYPNOTIZE_AREA: ASI::CallClassProc<0x7D0C50, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_HYPNOTIZE: ASI::CallClassProc<0x7D1420, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ICE1: ASI::CallClassProc<0x7D18E0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ICE2: ASI::CallClassProc<0x7D1C50, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_ICESTRIKE1: ASI::CallClassProc<0x7D1FC0, unsigned int>(spell_manager, spell_index); break; //ASI::CallAsClassProc<unsigned int>(effect_icestrike1, spell_manager, spell_index); break;
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
                case ASI::SL_PAIN_TOWER: ASI::CallClassProc<0x7D6DD0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_PESTILENCE: ASI::CallClassProc<0x7D6FE0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_PETRIFY: ASI::CallClassProc<0x7D7420, unsigned int>(spell_manager, spell_index); break;//ASI::CallAsClassProc<unsigned int>(effect_petrify, spell_manager, spell_index); break;//
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
                case ASI::SL_ROCK_BULLET: ASI::CallClassProc<0x7DA7F0, unsigned int>(spell_manager, spell_index); break; //ASI::CallAsClassProc<unsigned int>(effect_rock_bullet, spell_manager, spell_index); break;
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
                case ASI::SL_STRENGHT: ASI::CallClassProc<0x7DE8A0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_STRENGHT_AREA: ASI::CallClassProc<0x7DEB00, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_SUFFOCATION: ASI::CallClassProc<0x7DEEE0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_SUICIDE_DEATH: ASI::CallClassProc<0x7DF130, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_SUICIDE_HEAL: ASI::CallClassProc<0x7DF3A0, unsigned int>(spell_manager, spell_index); break;
                case ASI::SL_SUMMON: ASI::CallClassProc<0x7DF5C0, unsigned int>(spell_manager, spell_index); break; //ASI::CallAsClassProc<unsigned int>(effect_summon, spell_manager, spell_index); break;//
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

/*
// returns new spell effect index
unsigned short __stdcall spell_manager_cast_spell(int spell_id, int time_delay_maybe, ASI::SF_Unk1* source_data, ASI::SF_Unk1* target_data, int unknown)
{    // important
    ASI::SF_SpellManager* spell_manager;
    __asm mov spell_manager, ecx

    if (target_data->c1 == 1)
    {
        if (ASI::CallClassFunc<0x7FA8F9, unsigned int, unsigned int>(ASI::GetUnknownPointer(spell_manager, 0x24), target_data->s1) == 0)
            return 0;
    }

    // find a slot for new spell effect
    unsigned short i;
    for (i = 1; i < 800; i++)
    {
        if (ASI::SpellType(spell_manager, i) == 0)
            break;
    }
    if(i == 800)
        return 0;

    if (i > ASI::SpellCount(spell_manager))
        ASI::SpellCount(spell_manager) = i;


    // initialize effect
    ASI::SpellID(spell_manager, i) = (short)spell_id;
    ASI::SpellType(spell_manager, i) = ASI::CallClassFunc<0x5F7F50, short, int>
        (ASI::GetUnknownPointer(spell_manager, 0x3C), spell_id);

    ASI::CallClassProc<0x7B8A20, ASI::SF_Unk1*>
        (ASI::SpellSourcePointer(spell_manager, i), source_data);
    ASI::CallClassProc<0x7B8A20, ASI::SF_Unk1*>
        (ASI::SpellTargetPointer(spell_manager, i), target_data);

    ASI::Pointer some_pointer1000 = ASI::GetUnknownPointer(spell_manager, 0x4);
    auto some_value1002 = (unsigned int)some_pointer1000[0xC];

    ASI::SpellToDoCount(spell_manager, i) = time_delay_maybe - some_value1002;

    // spell-specific initialization
    switch (ASI::SpellType(spell_manager, i))
    {
    case ASI::ST_SHIFT_LIFE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ABILITY_SHIFT_LIFE;
        break;
    case ASI::ST_ALMIGHTNESS_ELEMENTAL:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ALMIGHTNESS_ELEMENTAL;
        break;
    case ASI::ST_ALMIGHTNESS_MENTAL:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ALMIGHTNESS_MENTAL;
        break;
    case ASI::ST_ARROWTOWER:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ARROWTOWER;
        break;
    // auras
    case 88:
    case 89:
    case 91:
    case 94:
    case 95:
    case 97:
    case 98:
    case 102:
    case 103:
    case 104:
    case 107:
    case 110:
    case 111:
    case 113:
    case 114:
    case 115:
    case 127:
    case 129:
    case 131:
    case 192:
    case 223:
    case 225:
    case 226:
    case 227:
    case 228:
        ASI::SpellLine(spell_manager, i) = ASI::SL_AURA;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);
        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 6, 0);

        break;
    case ASI::ST_ALMIGHTNESS_ELEMENTAL2:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ALMIGHTNESS_ELEMENTAL2;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);
        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 38, 0);

        break;
    case ASI::ST_BEFRIEND:
        ASI::SpellLine(spell_manager, i) = ASI::SL_BEFRIEND;
        break;
    case ASI::ST_CANNIBALIZE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_CANNIBALIZE;
        break;
    case ASI::ST_CURE_DISEASE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_CURE_DISEASE;
        break;
    case ASI::ST_CURE_POISON:
        ASI::SpellLine(spell_manager, i) = ASI::SL_CURE_POISON;
        break;
    case ASI::ST_DEATH:
        ASI::SpellLine(spell_manager, i) = ASI::SL_DEATH;
        break;
    case ASI::ST_ESSENCE_ELEMENTAL2:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ESSENCE_ELEMENTAL2;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);
        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 38, 0); 
        
        break;
        // hallow
    case 58:
    case 230:
        ASI::SpellLine(spell_manager, i) = ASI::SL_HALLOW;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);
        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 38, 0);

        break;
    case ASI::ST_FIREBALL2:
        ASI::SpellLine(spell_manager, i) = ASI::SL_FIREBALL2;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 18, 0);
        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 38, 0);

        break;
    case 1:
    case 159:
    case 234:
        ASI::SpellLine(spell_manager, i) = ASI::SL_FIREBURST;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 18, 0);
        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 38, 0);

        break;
    case ASI::ST_DETECT_MAGIC:
        ASI::SpellLine(spell_manager, i) = ASI::SL_DETECT_MAGIC;
        break;
    case ASI::ST_DETECT_METAL:
        ASI::SpellLine(spell_manager, i) = ASI::SL_DETECT_METAL;
        break;
    case ASI::ST_DISENCHANT:
        ASI::SpellLine(spell_manager, i) = ASI::SL_DISENCHANT;
        break;
    // icestrike
    case 14:
    case 235:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ICESTRIKE1;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);
        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 18, 0);
        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 38, 0);

        break;
    case ASI::ST_DEXTERITY:
        ASI::SpellLine(spell_manager, i) = ASI::SL_DEXTERITY;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);
        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 10, 0);

        break;
    case ASI::ST_ENDURANCE_EFFECT:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ENDURANCE;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);
        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 10, 0);

        break;
    case ASI::ST_DISRUPT:
        ASI::SpellLine(spell_manager, i) = ASI::SL_DISRUPT;
        break;
    case ASI::ST_DISTRACT:
        ASI::SpellLine(spell_manager, i) = ASI::SL_DISTRACT;
        break;
    case ASI::ST_ENLIGHTENMENT:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ENLIGHTENMENT;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);
        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 10, 0);
        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 39, 0);

        break;
    case ASI::ST_FAST_FIGHTING:
        ASI::SpellLine(spell_manager, i) = ASI::SL_FAST_FIGHTING;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);
        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 10, 0);

        break;
    case ASI::ST_FLEXIBILITY:
        ASI::SpellLine(spell_manager, i) = ASI::SL_FLEXIBILITY;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);
        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 10, 0);

        break;
    case ASI::ST_ESSENCE_ELEMENTAL1:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ESSENCE_ELEMENTAL;
        break;
    case ASI::ST_ESSENCE_MENTAL:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ESSENCE_MENTAL;
        break;
    case ASI::ST_ETERNITY:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ETERNITY;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);
        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 10, 0);
        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 39, 0);
        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 43, 0);
        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 44, 0);

        break;
    case ASI::ST_EXTINCT:
        ASI::SpellLine(spell_manager, i) = ASI::SL_EXTINCT;
        break;
    case ASI::ST_EXTINCT_TOWER:
        ASI::SpellLine(spell_manager, i) = ASI::SL_EXTINCT_TOWER;
        break;
    case ASI::ST_FAKESPELLONEFIGURE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_FAKESPELLONEFIGURE;
        break;
        // fireball
    case 13:
    case 239:
        ASI::SpellLine(spell_manager, i) = ASI::SL_FIREBALL1;
        break;
    case ASI::ST_LAVABULLET:
        ASI::SpellLine(spell_manager, i) = ASI::SL_FIREBULLET;
        break;
    case ASI::ST_CONSERVATION:
        ASI::SpellLine(spell_manager, i) = ASI::SL_CONSERVATION;


        PROBLEM HERE!

        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 11, 0);
        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_ESSENCE_BLACK:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ESSENCE_BLACK;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_FIRESHIELD2:
        ASI::SpellLine(spell_manager, i) = ASI::SL_FIRESHIELD2;
        break;
    case ASI::ST_FORGET:
        ASI::SpellLine(spell_manager, i) = ASI::SL_FORGET;
        break;
        // healing
    case 2:
    case 45:
    case 166:
        ASI::SpellLine(spell_manager, i) = ASI::SL_HEALING;
        break;
    case ASI::ST_HEAL_AREA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_HEALING_AREA;
        break;
    case ASI::ST_HEAL2:
        ASI::SpellLine(spell_manager, i) = ASI::SL_HEALING_AURA;
        break;
    case ASI::ST_HOLY_TOUCH:
        ASI::SpellLine(spell_manager, i) = ASI::SL_HOLY_TOUCH;
        break;
    case ASI::ST_ICESTRIKE2:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ICESTRIKE2;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);
        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 18, 0);

        break;
    case ASI::ST_ICESHIELD2:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ICESHIELD2;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_BENEFACTIONS:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ABILITY_BENEFACTIONS;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_BERSERK:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ABILITY_BERSERK;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_BOONS:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ABILITY_BOONS;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_CRITICAL_HITS:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ABILITY_CRITICAL_HITS;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_DURABILITY:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ABILITY_DURABILITY;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_ENDURANCE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ABILITY_ENDURANCE;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_PATRONIZE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ABILITY_PATRONIZE;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_RIPOSTE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ABILITY_RIPOSTE;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_SALVO:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ABILITY_SALVO;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_SHELTER:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ABILITY_SHELTER;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_STEELSKIN:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ABILITY_STEELSKIN;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_TRUESHOT:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ABILITY_TRUESHOT;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_WARCRY:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ABILITY_WARCRY;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_ACID_CLOUD:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ACID_CLOUD;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_ALMIGHTNESS_BLACK:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ALMIGHTNESS_BLACK;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_ALMIGHTNESS_WHITE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ALMIGHTNESS_WHITE;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_AMOK:
        ASI::SpellLine(spell_manager, i) = ASI::SL_AMOK;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_ASSISTANCE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ASSISTANCE;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_UNK1:
        ASI::SpellLine(spell_manager, i) = ASI::SL_UNKNOWN1;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_BLIZZARD:
        ASI::SpellLine(spell_manager, i) = ASI::SL_BLIZZARD;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_BRILLIANCE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_BRILLIANCE;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case 190:
    case 193:
    case 196:
    case 201:
    case 202:
    case 204:
    case 205:
    case 208:
    case 212:
    case 214:
    case 217:
        ASI::SpellLine(spell_manager, i) = ASI::SL_CHAIN;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_CHARISMA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_CHARISMA;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case 122:
    case 237:
        ASI::SpellLine(spell_manager, i) = ASI::SL_CHARM;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_CHARM_ANIMAL:
        ASI::SpellLine(spell_manager, i) = ASI::SL_CHARM_ANIMAL;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_CHILL_RESISTANCE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_CHILL_RESISTANCE;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_CONFUSE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_CONFUSE;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_CONFUSE_AREA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_CONFUSE_AREA;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_DARK_BANISHING:
        ASI::SpellLine(spell_manager, i) = ASI::SL_DARK_BANISHING;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_DARKNESS_AREA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_DARKNESS_AREA;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_DEATH_GRASP:
        ASI::SpellLine(spell_manager, i) = ASI::SL_DEATH_GRASP;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_DECAY1:
        ASI::SpellLine(spell_manager, i) = ASI::SL_DECAY;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_DECAY2:
        ASI::SpellLine(spell_manager, i) = ASI::SL_DECAY2;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_DEMORALIZATION:
        ASI::SpellLine(spell_manager, i) = ASI::SL_DEMORALIZATION;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_DISPEL_BLACK_AURA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_DISPEL_BLACK_AURA;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_DISPEL_WHITE_AURA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_DISPEL_WHITE_AURA;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_DOMINATE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_DOMINATE;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_DOMINATE_ANIMAL:
        ASI::SpellLine(spell_manager, i) = ASI::SL_DOMINATE_ANIMAL;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_DOMINATE_UNDEAD:
        ASI::SpellLine(spell_manager, i) = ASI::SL_DOMINATE_UNDEAD;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_UNK2:
        ASI::SpellLine(spell_manager, i) = ASI::SL_DOMINATE_BREAK;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_ESSENCE_WHITE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ESSENCE_WHITE;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_FEIGN_DEATH:
        ASI::SpellLine(spell_manager, i) = ASI::SL_FEIGN_DEATH;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_FIRESHIELD:
        ASI::SpellLine(spell_manager, i) = ASI::SL_FIRESHIELD1;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_FLEXIBILITY_AREA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_FLEXIBILITY_AREA;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_FOG:
        ASI::SpellLine(spell_manager, i) = ASI::SL_FOG;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_FREEZE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_FREEZE;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_FREEZE_AREA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_FREEZE_AREA;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_GUARD:
        ASI::SpellLine(spell_manager, i) = ASI::SL_GUARD;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case 21:
    case 167:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ICESHIELD;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_HYPNOTIZE_AREA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_HYPNOTIZE_AREA;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_HYPNOTIZE_TOWER:
        ASI::SpellLine(spell_manager, i) = ASI::SL_HYPNOTIZE;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_FREEZE2:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ICE1;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_FREEZE3:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ICE2;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_MANASHIELD:
        ASI::SpellLine(spell_manager, i) = ASI::SL_MANASHIELD;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_MELT_RESISTANCE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_MELT_RESISTANCE;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_MIRAGE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_MIRAGE;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_PETRIFY:
        ASI::SpellLine(spell_manager, i) = ASI::SL_PETRIFY;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_CLOAK_OF_NOR:
        ASI::SpellLine(spell_manager, i) = ASI::SL_CLOAK_OF_NOR;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_QUICKNESS_AREA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_QUICKNESS_AREA;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_RAIN_OF_FIRE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_RAIN_OF_FIRE;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_REGENERATE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_REGENERATE;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_REMEDILESS:
        ASI::SpellLine(spell_manager, i) = ASI::SL_REMEDILESS;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_REMOVE_CURSE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_REMOVE_CURSE;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_RETENTION:
        ASI::SpellLine(spell_manager, i) = ASI::SL_RETENTION;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_ROOTS:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ROOTS;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_ROOTS_AREA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ROOTS_AREA;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_CLAY_FEET:
        ASI::SpellLine(spell_manager, i) = ASI::SL_FEET_OF_CLAY;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_SELF_ILLUSION:
        ASI::SpellLine(spell_manager, i) = ASI::SL_SELF_ILLUSION;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_SLOW_FIGHTING:
        ASI::SpellLine(spell_manager, i) = ASI::SL_SLOW_FIGHTING;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_SLOWNESS:
        ASI::SpellLine(spell_manager, i) = ASI::SL_SLOWNESS;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_SLOWNESS_AREA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_SLOWNESS_AREA;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_STONE_RAIN:
        ASI::SpellLine(spell_manager, i) = ASI::SL_STONE_RAIN;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_STRENGHT_AREA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_STRENGHT_AREA;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_SUFFOCATION:
        ASI::SpellLine(spell_manager, i) = ASI::SL_SUFFOCATION;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
        // summon
    case 20:
    case 29:
    case 31:
    case 33:
    case 92:
    case 106:
    case 109:
    case 133:
    case 136:
    case 141:
    case 188:
    case 198:
    case 203:
    case 206:
    case 209:
        ASI::SpellLine(spell_manager, i) = ASI::SL_SUMMON;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_THORN_SHIELD:
        ASI::SpellLine(spell_manager, i) = ASI::SL_THORNSHIELD;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_FEEDBACK:
        ASI::SpellLine(spell_manager, i) = ASI::SL_FEEDBACK;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_WEAKEN:
        ASI::SpellLine(spell_manager, i) = ASI::SL_WEAKEN;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_WEAKEN_AREA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_WEAKEN_AREA;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_ICESHIELD:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ICESHIELD3;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_LIFETAP_AURA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_LIFETAP_AURA;
        break;
    case ASI::ST_MANADRAIN:
        ASI::SpellLine(spell_manager, i) = ASI::SL_MANADRAIN;
        break;
    case 67:
    case 232:
        ASI::SpellLine(spell_manager, i) = ASI::SL_MANATAP;
        break;
    case ASI::ST_MANATAP_AURA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_MANATAP_AURA;
        break;
    case ASI::ST_MEDITATION:
        ASI::SpellLine(spell_manager, i) = ASI::SL_MEDITATION;
        break;
    case 18:
    case 240:
        ASI::SpellLine(spell_manager, i) = ASI::SL_PAIN;
        break;
    case ASI::ST_PAIN_AREA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_PAIN_AREA;
        break;
    case ASI::ST_PAIN_TOWER:
        ASI::SpellLine(spell_manager, i) = ASI::SL_PAIN_TOWER;
        break;
    case ASI::ST_RAISE_DEAD:
        ASI::SpellLine(spell_manager, i) = ASI::SL_RAISE_DEAD;
        break;
    case ASI::ST_REINFORCEMENT:
        ASI::SpellLine(spell_manager, i) = ASI::SL_REINFORCEMENT;
        break;
    case ASI::ST_REVENGE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_REVENGE;
        break;
    case ASI::ST_SACRIFICE_MANA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_SACRIFICE_MANA;
        break;
    case ASI::ST_SHIFT_MANA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_SHIFT_MANA;
        break;
        // shock
    case 69:
    case 238:
        ASI::SpellLine(spell_manager, i) = ASI::SL_SHOCK;
        break;
    case ASI::ST_SHOCKWAVE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_SHOCKWAVE;
        break;
    case ASI::ST_SENTINEL_HEAL:
        ASI::SpellLine(spell_manager, i) = ASI::SL_SENTINEL_HEALING;
        break;
    case ASI::ST_SPARK:
        ASI::SpellLine(spell_manager, i) = ASI::SL_SPARK;
        break;
    case ASI::ST_STONE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_STONE;
        break;
    case ASI::ST_STONE_TOWER:
        ASI::SpellLine(spell_manager, i) = ASI::SL_STONE_TOWER;
        break;
    case ASI::ST_SUICIDE_DEATH:
        ASI::SpellLine(spell_manager, i) = ASI::SL_SUICIDE_DEATH;
        break;
    case ASI::ST_SUICIDE_HEAL:
        ASI::SpellLine(spell_manager, i) = ASI::SL_SUICIDE_HEAL;
        break;
    case ASI::ST_THORNS:
        ASI::SpellLine(spell_manager, i) = ASI::SL_THORNSHIELD_HIT;
        break;
    case ASI::ST_TORTURE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_TORTURE;
        break;
        // lifetap
    case 19:
    case 231:
        ASI::SpellLine(spell_manager, i) = ASI::SL_LIFETAP;

        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 38, 0);

        break;
        // mutation
    case 199:
    case 233:
        ASI::SpellLine(spell_manager, i) = ASI::SL_MUTATION;

        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);
        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 10, 0);
        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 39, 0);
        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 43, 0);
        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 44, 0);

        break;   
    case ASI::ST_PESTILENCE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_PESTILENCE;

        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);
        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 14, 0);
        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 38, 0);

        break;
    case ASI::ST_PLAGUE_AREA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_PLAGUE_AREA;

        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);
        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 38, 0);

        break;
    case ASI::ST_POISON:
        ASI::SpellLine(spell_manager, i) = ASI::SL_POISON;

        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 18, 0);
        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 38, 0);

        break;
    case ASI::ST_QUICKNESS:
        ASI::SpellLine(spell_manager, i) = ASI::SL_QUICKNESS;

        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);
        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 10, 0);

        break;
        // rock bullet
    case 139:
    case 236:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ROCK_BULLET;

        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 45, (short)unknown);

        break;
    case ASI::ST_STRENGHT:
        ASI::SpellLine(spell_manager, i) = ASI::SL_STRENGHT;

        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, (short)unknown);
        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 10, (short)unknown);

        break;
        // waves
    case 134:
    case 137:
    case 142:
        ASI::SpellLine(spell_manager, i) = ASI::SL_WAVE;

        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, (short)unknown);
        ASI::XDataListAdd2(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 6, (short)unknown);

        break;
    case ASI::ST_ILLUMINATE:
        ASI::SpellLine(spell_manager, i) = ASI::SL_ILLUMINATE;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_INABILITY:
        ASI::SpellLine(spell_manager, i) = ASI::SL_INABILITY;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_INFLEXIBILITY:
        ASI::SpellLine(spell_manager, i) = ASI::SL_INFLEXIBILITY;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_INFLEXIBILITY_AREA:
        ASI::SpellLine(spell_manager, i) = ASI::SL_INFLEXIBILITY_AREA;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_INVISIBILITY:
        ASI::SpellLine(spell_manager, i) = ASI::SL_INVISIBILITY;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    case ASI::ST_INVULNERABILITY:
        ASI::SpellLine(spell_manager, i) = ASI::SL_INVULNERABILITY;

        ASI::XDataListAdd(ASI::GetXDataPointer(spell_manager), ASI::SpellXData(spell_manager, i), 5, 0);

        break;
    default:
        break;
    }


    // unknown???
    if (target_data->c1 == 1)
    {
        if (ASI::SpellType(spell_manager, i) != 90)
        {
            //GdFigure::AddSpellEffect
            ASI::CallClassProc<0x7F0903, int, int>
                (ASI::GetUnknownPointer(spell_manager, 0x24), target_data->s1, i);
        }
    }

    return i;
}
*/

// hook here is required (?), because we need to jump near - replacing call to far function takes 6 bytes, while jump to relative is still 4 bytes :)
void __declspec(naked) spell_manager_handle_spells_hook()
{
    __asm
    {
        call spell_manager_handle_spells
        jmp SPELL_MANAGER_HANDLE_SPELLS_ABSOLUTE
    }
}

/*
void __declspec(naked) spell_manager_cast_spell_hook()
{
    __asm
    {
        mov eax, [esp+0x14]
        push eax
        mov eax, [esp+0x14]
        push eax
        mov eax, [esp+0x14]
        push eax
        mov eax, [esp+0x14]
        push eax
        mov eax, [esp+0x14]
        push eax
        call spell_manager_cast_spell
        jmp SPELL_MANAGER_CAST_SPELL_JUMP_LOCATION
    }
}
*/

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
        SPELL_MANAGER_CAST_SPELL_JUMP_LOCATION = ASI::AddrOf(0x3B84EA);

        ASI::MemoryRegion mreg(ASI::AddrOf(0x1B3360), 5);
        ASI::MemoryRegion mreg2(ASI::AddrOf(0x3B7580), 5);

        ASI::BeginRewrite(mreg);
        *(unsigned char*)(ASI::AddrOf(0x1B3360)) = 0xE9;   // absolute far jmp instruction, call opcode
        *(int*)(ASI::AddrOf(0x1B3361)) = (int)(&spell_manager_handle_spells_hook) - ASI::AddrOf(0x1B3365);
        ASI::EndRewrite(mreg);


        /*ASI::BeginRewrite(mreg2);
        *(unsigned char*)(ASI::AddrOf(0x3B7580)) = 0xE9;   // absolute far jmp instruction, call opcode
        *(int*)(ASI::AddrOf(0x3B7581)) = (int)(&spell_manager_cast_spell_hook) - ASI::AddrOf(0x3B7585);
        ASI::EndRewrite(mreg2);*/
    }
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

