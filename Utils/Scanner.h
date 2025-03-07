#pragma once
#include "..\SourceSDK\interfaces.h"

class Scanner {
public:
    static bool find_all() {
        return find_CGameEntitySystem() && find_CDOTACamera();
    }

    static bool find_CGameEntitySystem() {
        if (vmt.entity_system)
            return true;

        const auto entity_system = vmt.game_resource->get_entity_system();
        if (!entity_system)
            return false;

        vmt.entity_system = entity_system;
        LOG::INFO("CGameEntitySystem -> [{}]", TO_VOID(entity_system));
        return true;
    }

    // #STR: "Failed to create camera\n", "Failed to init camera with file %s", "Usage: <camera_file>\n"
    // lea		rax, ??_7CDOTACameraManager@@6B@ ; const CDOTACameraManager::`vftable'
    // mov		cs:byte_1851E0C18, 0
    // mov		cs:qword_1851E0B90, rax
    // lea		rcx, sub_182FD0C10 ; void (__cdecl *)() <<<<
    // lea		rax, ??_7CDOTACameraManager@@6B@_0 ; const CDOTACameraManager::`vftable'
    // mov		cs:dword_1851E0C1C, 0FFFFFFFFh
    // mov		cs:qword_1851E0B98, rax
    // call		atexit
    // lea		rcx, dword_1851E0C20
    // call		_Init_thread_footer
    // jmp		loc_181713F7F
    // endp
    //
    // sub_182FD0C10:
    // lea		rax, ??_7CDOTACameraManager@@6B@ ; const CDOTACameraManager::`vftable'
    // mov		cs:qword_1851E0B90, rax
    // lea		rcx, qword_1851E0B90 <<<<
    // lea		rax, ??_7CDOTACameraManager@@6B@_0 ; const CDOTACameraManager::`vftable'
    // mov		cs:qword_1851E0B98, rax
    // jmp		sub_181499FC0
    // endp
    static bool find_CDOTACamera() {
        if (vmt.camera)
            return true;

        const auto base = Memory::pattern_scan("client.dll", Patches::Patterns::CDOTACamera);
        if (!base)
            return false;

        const auto camera_base_address = Memory::absolute_address<uintptr_t>(base.value());
        if (!camera_base_address || camera_base_address.value_or(0) == 0)
            return false;

        vmt.camera = reinterpret_cast<CDOTACamera*>(camera_base_address.value() - 0x40);
        LOG::INFO("CDOTA_Camera -> [{}]", TO_VOID(vmt.camera));
        return true;
    }

    static bool find_CDOTAGamerules() {
        const auto dota_gamerules_proxy_ptr = vmt.entity_system->find_by_name(ENTITY_NAME_TYPE::BINARY_NAME, "C_DOTAGamerulesProxy");
        if (!dota_gamerules_proxy_ptr)
            return false;

        C_DOTAGamerulesProxy* dota_gamerules_proxy = reinterpret_cast<C_DOTAGamerulesProxy*>(dota_gamerules_proxy_ptr.value());

        const auto dota_gamerules_ptr = dota_gamerules_proxy->gamerules();
        if (!dota_gamerules_ptr)
            return false;

        if (vmt.gamerules == dota_gamerules_ptr)
            return false;

        vmt.gamerules = dota_gamerules_ptr;
        LOG::INFO("C_DOTAGamerules_Proxy -> [{}]", TO_VOID(dota_gamerules_proxy_ptr.value()));
        LOG::INFO("CDOTAGamerules -> [{}]", TO_VOID(vmt.gamerules));
        return true;
    }
};
