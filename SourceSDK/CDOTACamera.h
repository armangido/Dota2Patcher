#pragma once
#include "../Dota2Patcher.h"
#include "../Memory.h"

// CDOTA_Camera 21 vfunc
// 
// sub                          rsp, 48h
// movaps[rsp + 48h + var_18],  xmm6
// movaps                       xmm6, xmm1
// movaps[rsp + 48h + var_28],  xmm7
// call                         get_camera_distance <<<

class CDOTACamera {
public:
    static void set_distance(float distance) {
        Memory::write_memory(camera_base_, distance);
    }

    static void set_fow(float fow) {
        Memory::write_memory(camera_base_ + 0x4, fow);
    }

    static void set_r_farz(float r_farz) {
        Memory::write_memory(camera_base_ + 0x14, r_farz);
    }

    static bool find_camera() {
        // mov		edx, cs:TlsIndex
        // mov		rax, gs:58h
        // mov		ecx, 40h ; '@'
        // mov		rax, [rax+rdx*8]
        // mov		eax, [rcx+rax]
        // cmp		cs:dword_184FE0578, eax
        // jg		short loc_1816542DF
        // lea		rax, dword_184FE0560 <<<<<
        // add		rsp, 20h
        // pop		rbx
        // retn

        const auto base = Memory::pattern_scan("client.dll", Patches::Patterns::CDOTACamera);
        if (!base)
            return false;

        const auto camera_base_address = Memory::absolute_address<uintptr_t>(base.value(), 3, 7);
        if (!camera_base_address)
            return false;

        printf("[+] CDOTA_Camera: %p\n", reinterpret_cast<void*>(camera_base_address.value()));
        camera_base_ = camera_base_address.value();
        return true;
    }

private:
    static inline uintptr_t camera_base_;
};
