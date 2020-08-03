// dllmain.cpp : Defines the entry point for the DLL application.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <fstream>
#include <unordered_map>
#include <tuple>
#include "SF_ASI.h"
#include "SF_UTL_String.h"

int UPGRADE_EXEC_ABSOLUTE;

std::unordered_map<unsigned short, std::pair<int, unsigned short>> unit_upgrade_data;
char max_upgrade_index = 54;          // base in game

void __stdcall get_upgraded_unit_variant_id(ASI::Pointer unknown, ASI::Pointer unit_data)
{
    ASI::Pointer stack_pointer(0);
    ASI::Pointer __this(0);
    __asm mov stack_pointer.ptr, esp
    __asm mov __this.ptr, ecx

    unsigned short unit_id = (unsigned short)(unit_data[0x0]);
    std::unordered_map<unsigned short, std::pair<int, unsigned short>>::const_iterator uud_val = unit_upgrade_data.find(unit_id);
    if (uud_val != unit_upgrade_data.end())
    {
        if (ASI::CallClassFunc<0x641F70, unsigned int, void*, int>
            ((*__this[0x48]), unknown.ptr, (*uud_val).second.first))
            (unit_data[0x0]).AsRef<unsigned short>() = (*uud_val).second.second;
    }

    return;
}

void __declspec(naked) get_upgraded_unit_variant_id_hook()
{
    __asm
    {
        call get_upgraded_unit_variant_id
        jmp UPGRADE_EXEC_ABSOLUTE
    }
}


bool InitializeUnitUpgradeData()
{
    std::fstream myfile("upgrades_config", std::ios_base::in);
    if (!myfile.is_open())
        return false;

    unsigned short unit_base_id;
    int upgrade_id;
    unsigned short unit_upgrade_id;
    while (!myfile.fail())
    {
        myfile >> unit_base_id;
        if (myfile.eof())
            break;

        myfile >> upgrade_id;
        myfile >> unit_upgrade_id;

        unit_upgrade_data.emplace(unit_base_id, std::pair<int, unsigned short>(upgrade_id, unit_upgrade_id));

        if (upgrade_id > max_upgrade_index)
            max_upgrade_index = (char)upgrade_id;
    }

    myfile.close();

    if (myfile.fail())
        return false;


    /*unit_upgrade_data.emplace(540, std::pair<int, unsigned short>(2, 1223));
    unit_upgrade_data.emplace(541, std::pair<int, unsigned short>(3, 1224));
    unit_upgrade_data.emplace(539, std::pair<int, unsigned short>(4, 1225));
    unit_upgrade_data.emplace(542, std::pair<int, unsigned short>(5, 1226));
    unit_upgrade_data.emplace(538, std::pair<int, unsigned short>(6, 1227));
    unit_upgrade_data.emplace(545, std::pair<int, unsigned short>(8, 1229));
    unit_upgrade_data.emplace(552, std::pair<int, unsigned short>(9, 1232));
    unit_upgrade_data.emplace(546, std::pair<int, unsigned short>(10, 1240));
    unit_upgrade_data.emplace(547, std::pair<int, unsigned short>(10, 1239));
    unit_upgrade_data.emplace(548, std::pair<int, unsigned short>(10, 1435));
    unit_upgrade_data.emplace(551, std::pair<int, unsigned short>(11, 1230));
    unit_upgrade_data.emplace(549, std::pair<int, unsigned short>(12, 1231));
    unit_upgrade_data.emplace(569, std::pair<int, unsigned short>(14, 1234));
    unit_upgrade_data.emplace(570, std::pair<int, unsigned short>(15, 1238));
    unit_upgrade_data.emplace(572, std::pair<int, unsigned short>(16, 1235));
    unit_upgrade_data.emplace(573, std::pair<int, unsigned short>(16, 1236));
    unit_upgrade_data.emplace(575, std::pair<int, unsigned short>(17, 1237));
    unit_upgrade_data.emplace(553, std::pair<int, unsigned short>(20, 1249));
    unit_upgrade_data.emplace(560, std::pair<int, unsigned short>(20, 1250));
    unit_upgrade_data.emplace(556, std::pair<int, unsigned short>(21, 1247));
    unit_upgrade_data.emplace(555, std::pair<int, unsigned short>(22, 1246));
    unit_upgrade_data.emplace(558, std::pair<int, unsigned short>(23, 1245));
    unit_upgrade_data.emplace(559, std::pair<int, unsigned short>(24, 1248));
    unit_upgrade_data.emplace(583, std::pair<int, unsigned short>(26, 1284));
    unit_upgrade_data.emplace(584, std::pair<int, unsigned short>(27, 1285));
    unit_upgrade_data.emplace(580, std::pair<int, unsigned short>(28, 1286));
    unit_upgrade_data.emplace(578, std::pair<int, unsigned short>(29, 1290));
    unit_upgrade_data.emplace(579, std::pair<int, unsigned short>(29, 1289));
    unit_upgrade_data.emplace(577, std::pair<int, unsigned short>(30, 1287));
    unit_upgrade_data.emplace(581, std::pair<int, unsigned short>(30, 1288));
    unit_upgrade_data.emplace(561, std::pair<int, unsigned short>(32, 1294));
    unit_upgrade_data.emplace(562, std::pair<int, unsigned short>(33, 1297));
    unit_upgrade_data.emplace(566, std::pair<int, unsigned short>(34, 1295));
    unit_upgrade_data.emplace(565, std::pair<int, unsigned short>(35, 1299));
    unit_upgrade_data.emplace(564, std::pair<int, unsigned short>(35, 1296));
    unit_upgrade_data.emplace(568, std::pair<int, unsigned short>(36, 1298));
    // new upgrades
    unit_upgrade_data.emplace(543, std::pair<int, unsigned short>(55, 2997));

    max_upgrade_index = 55;*/

    return true;
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

        if (InitializeUnitUpgradeData())
            return FALSE;

        UPGRADE_EXEC_ABSOLUTE = ASI::AddrOf(0x25B1A2);
        ASI::MemoryRegion mreg(ASI::AddrOf(0x25B19D), 5);
        ASI::MemoryRegion mreg2(ASI::AddrOf(0x63A1D3), 1);
        // A3A1D0

        ASI::BeginRewrite(mreg);
        *(unsigned char*)(ASI::AddrOf(0x25B19D)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x25B19E)) = (int)(&get_upgraded_unit_variant_id_hook) - ASI::AddrOf(0x25B1A2);  // not D9, DE - skip one instruction?
        ASI::EndRewrite(mreg);

        ASI::BeginRewrite(mreg2);
        *(unsigned char*)(ASI::AddrOf(0x63A1D3)) = (char)(max_upgrade_index+1);
        ASI::EndRewrite(mreg2);

        break;
    }
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

