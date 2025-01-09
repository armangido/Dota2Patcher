#pragma once
#include "Dota2Patcher.h"
#include "Memory.h"

// CDOTA_Camera 21 vfunc
// 
// sub							rsp, 48h
// movaps[rsp + 48h + var_18],	xmm6
// movaps						xmm6, xmm1
// movaps[rsp + 48h + var_28],	xmm7
// call							get_camera_distance <<<

class CDOTACamera {
public:
	void set_distance(HANDLE hProc, float distance) {
		Memory memory;
		memory.write_memory(hProc, camera_base, distance);
	}

	void set_fow(HANDLE hProc, float fow) {
		Memory memory;
		memory.write_memory(hProc, camera_base + 0x4, fow);
	}

	void set_r_farz(HANDLE hProc, float r_farz) {
		Memory memory;
		memory.write_memory(hProc, camera_base + 0x14, r_farz);
	}

	bool find_camera(HANDLE hProc, Memory::ModuleInfo module) {
		Memory memory;

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

		uintptr_t base = memory.pattern_scan(hProc, module, "48 8D ? ? ? ? ? 48 83 C4 ? 5B C3 48 8D ? ? ? ? ? E8 ? ? ? ? 83 3D 76 B2 98 03");
		if (base == -1)
			return false;

		uintptr_t camera_base_address = memory.absolute_address(hProc, base, 3, 7);
		if (camera_base_address == -1)
			return false;

		printf("[+] CDOTA_Camera: %p\n", (void*)camera_base_address);
		camera_base = camera_base_address;
		return true;
	}

private:
	uintptr_t camera_base = -1;
};
