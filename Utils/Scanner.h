#pragma once
#include "..\SourceSDK\interfaces.h"
#include "..\SourceSDK\WorldToScreen.h"

class Scanner {
public:
    static bool find_all() {
        return find_CGameEntitySystem() && find_CDOTACamera() && find_ViewMatrix();
    }

    static bool find_CGameEntitySystem() {
        const auto c_entity_system = Memory::read_memory<CGameEntitySystem*>(vmt.game_resource + 0x58); // client
        const auto s_entity_system = Memory::read_memory<CGameEntitySystem*>(vmt.game_resource + 0xB8); // server
        
        if (c_entity_system.value_or(nullptr) == nullptr || s_entity_system.value_or(nullptr) == nullptr)
            return false;

        vmt.c_entity_system = c_entity_system.value();
        vmt.s_entity_system = s_entity_system.value();

        LOG::INFO("CGameEntitySystem [client] -> [{}]", TO_VOID(c_entity_system.value()));
        LOG::INFO("CGameEntitySystem [server] -> [{}]", TO_VOID(s_entity_system.value()));
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

    // CRenderGameSystem's last vfunc
    // movsxd	rax, edx
    // lea		rcx, unk_1851BFFF0 <<<<
    // shl		rax, 6
    // add		rax, rcx
    // retn
    // endp
    //
    // OR
    //
    // CViewRender 5'th vfunc
    // mov     rax, rsp
    // mov     [rax+10h], rbx
    // mov     [rax+18h], rbp
    // mov     [rax+20h], rsi
    // mov     [rax+8], rcx
    // push    rdi
    // push    r12
    // push    r13
    // push    r14
    // push    r15
    // sub     rsp, 90h
    // movaps  xmmword ptr [rax-38h], xmm6
    // xor     edx, edx
    // movaps  xmmword ptr [rax-48h], xmm7
    // mov     r15, rcx
    // call    sub_18152DF60
    // movss   xmm6, cs:Y
    // lea     r13, [r15+10h]
    // movss   xmm7, cs:dword_18305220C
    // xor     ebx, ebx
    // mov     byte ptr [r15+1330h], 1
    // lea     r15, cs:180000000h
    // nop     word ptr [rax+rax+00h]
    // mov     r12d, ebx
    // lea     rcx, rva unk_1851C08C0[r15]
    // imul    r14, r12, 990h
    // mov     edx, ebx
    // lea     rax, rva unk_1851BFFF0[r15] <<<<
    // shl     rdx, 6
    // lea     r9, rva unk_1851C0880[r15]
    // add     rcx, rdx
    // lea     r8, rva unk_1851C0840[r15]
    static bool find_ViewMatrix() {
        const auto base = Memory::pattern_scan("client.dll", Patches::Patterns::VMatrix);
        if (!base)
            return false;

        const auto VMatrix_ptr = Memory::absolute_address<uintptr_t>(base.value());
        if (!VMatrix_ptr || VMatrix_ptr.value_or(0) == 0)
            return false;

        WorldToScreen::g_VMatrix = VMatrix_ptr.value();
        LOG::INFO("ViewMatrix -> [{}]", TO_VOID(WorldToScreen::g_VMatrix));

        if (!WorldToScreen::get_windows_size())
            return false;

        LOG::INFO("Windows Size: {}x{}", WorldToScreen::g_windows_size.x, WorldToScreen::g_windows_size.y);

        return true;
    }

    static bool find_CDOTAGamerules() {
        const auto dota_gamerules_proxy_ptr = vmt.c_entity_system->find_by_name(ENTITY_NAME_TYPE::BINARY_NAME, "C_DOTAGamerulesProxy");
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
