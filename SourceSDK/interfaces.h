#pragma once
#include "CEngineClient.h"
#include "CSource2Client.h"
#include "CDOTAGamerules.h"
#include "CDOTACamera.h"
#include "CGameEntitySystem.h"
#include "CSchemaSystem.h"
#include "CCvar.h"
#include "../Hacks/Hacks.h"

class Scanner {
public:
    static inline bool find_CGameEntitySystem();
    static inline bool find_CDOTACamera();
    static inline bool find_CDOTAGamerules();

    static inline bool find_all();
};

static class dota_interfaces {
public:
    static inline bool find_all() {
        return Scanner::find_all();
    }

    static inline CEngineClient* engine = nullptr;
    static inline CSource2Client* client = nullptr;
    static inline CDOTAGamerules* gamerules = nullptr;
    static inline CDOTACamera* camera = nullptr;
    static inline CGameEntitySystem* entity_system = nullptr;
    static inline CSchemaSystem* schema_system = nullptr;
    static inline CCvar* cvar = nullptr;
} vmt;


bool Scanner::find_all() {
    bool status = true;

    status &= find_CGameEntitySystem();
    status &= find_CDOTACamera();

    return status;
}

bool Scanner::find_CGameEntitySystem() {
    if (vmt.entity_system)
        return true;

    const auto base = Memory::virtual_function<uintptr_t>(vmt.client, 30);
    if (!base)
        return false;

    const auto absolute_address_ptr = Memory::absolute_address<uintptr_t>(base.value());
    if (!absolute_address_ptr)
        return false;

    const auto CGameEntitySystem_ptr = Memory::read_memory<uintptr_t>(absolute_address_ptr.value());
    if (!CGameEntitySystem_ptr || CGameEntitySystem_ptr.value_or(0) == 0)
        return false;

    vmt.entity_system = reinterpret_cast<CGameEntitySystem*>(CGameEntitySystem_ptr.value());
    LOG::INFO("CGameEntitySystem -> [{}]", TO_VOID(CGameEntitySystem_ptr.value()));
    return true;
}

// CDOTA_Camera 20'th vfunc (offset 0xA0):
// sub                          rsp, 48h
// movaps[rsp + 48h + var_18],  xmm6
// movaps                       xmm6, xmm1
// movaps[rsp + 48h + var_28],  xmm7
// call                         get_camera_distance <<<
// 
// get_camera_distance:
// mov                          edx, cs:TlsIndex
// mov                          rax, gs:58h
// mov                          ecx, 40h ; '@'
// mov                          rax, [rax+rdx*8]
// mov                          eax, [rcx+rax]
// cmp                          cs:dword_184FE0578, eax
// jg                           short loc_1816542DF
// lea                          rax, dword_184FE0560 <<<<<
// add                          rsp, 20h
// pop                          rbx
// retn
bool Scanner::find_CDOTACamera() {
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

bool Scanner::find_CDOTAGamerules() {
    if (vmt.gamerules)
        return true;

    const auto dota_gamerules_proxy_ptr = vmt.entity_system->find_by_name(ENTITY_NAME_TYPE::BINARY_NAME, "C_DOTAGamerulesProxy");
    if (!dota_gamerules_proxy_ptr)
        return false;

    C_DOTAGamerulesProxy* dota_gamerules_proxy = reinterpret_cast<C_DOTAGamerulesProxy*>(dota_gamerules_proxy_ptr.value());
    LOG::INFO("C_DOTAGamerules_Proxy -> [{}]", TO_VOID(dota_gamerules_proxy_ptr.value()));

    const auto dota_gamerules_ptr = dota_gamerules_proxy->gamerules();
    if (!dota_gamerules_ptr)
        return false;

    vmt.gamerules = reinterpret_cast<CDOTAGamerules*>(dota_gamerules_ptr);
    LOG::INFO("CDOTAGamerules -> [{}]", TO_VOID(vmt.gamerules));
    return true;
}
