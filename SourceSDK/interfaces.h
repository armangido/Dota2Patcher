#pragma once
#include "CEngineClient.h"
#include "CDOTACamera.h"
#include "CSource2Client.h"

class Scanner {
public:
    static bool find_CDOTACamera();
    static bool find_all();
};

static class dota_interfaces {
public:
    bool find_all() {
        return Scanner::find_all();
    }

    CEngineClient* engine;
    CSource2Client* client;
    CDOTACamera* camera;
} vmt;

bool Scanner::find_CDOTACamera() {
// CDOTA_Camera 21 vfunc:
// 
// sub                          rsp, 48h
// movaps[rsp + 48h + var_18],  xmm6
// movaps                       xmm6, xmm1
// movaps[rsp + 48h + var_28],  xmm7
// call                         get_camera_distance <<<
// 
// get_camera_distance:
// 
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

    const auto base = Memory::pattern_scan("client.dll", Patches::Patterns::CDOTACamera);
    if (!base)
        return false;

    const auto camera_base_address = Memory::absolute_address<uintptr_t>(base.value());
    if (!camera_base_address)
        return false;

    printf("[+] CDOTA_Camera -> [%p]\n", reinterpret_cast<void*>(camera_base_address.value()));
    vmt.camera = (CDOTACamera*)camera_base_address.value();
    return true;
}

// Expandable for future
bool Scanner::find_all() {
    bool status = true;

    status &= find_CDOTACamera();

    return status;
}
