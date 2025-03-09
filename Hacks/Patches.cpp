#include "Hacks.h"
#include "..\Utils\Config.h"

void Hacks::apply_patches() {
// CParticleCollection 95'th vfunc (offset 0x5F)
// #STR: "Error in child list of particle system %s [%p], parent: %p, "m_Children.m_pHead: [%p]\n", "Address of m_Children.m_pHead: [%p]\n"
// mov		r11, rsp
// push		rbp
// push		rsi
// push		r12
// push		r13
// sub		rsp, 118h
// movzx	eax, byte ptr [rcx+9A4h]
// xor		r13d, r13d
// shr		al, 7
// movzx	r12d, dl
// mov		[rsp+138h+arg_8], r13d
// mov		rsi, rcx
// mov		ebp, r13d
// cmp		dl, al
// jz		loc_18003BE04
// mov		[r11+20h], r14
// mov		[r11-28h], r15
// test		dl, dl
// jz		short loc_18003BAA2
// mov		rdx, [rcx+310h]
// test		rdx, rdx
// jz		short loc_18003BAA2
// mov		rax, [rdx+0A8h]
// mov		r8, 0EFFFFFFFFFFFFFFFh
// mov		rcx, rax
// and		rcx, r8
// cmp		rax, rcx
// jz		short loc_18003BAA2
// mov		[rdx+0A8h], rcx
// mov		rcx, cs:qword_180635B78
// mov		rax, [rcx]
// call		qword ptr [rax+3C8h]
// and		byte ptr [rsi+9A4h], 7Fh <<<<
	if (ConfigManager::config_entries["set_rendering_enabled"]) {
		Patches::add_patch({
			"set_rendering_enabled",
			"particles.dll",
			Patches::Patterns::set_rendering_enabled,
			PATCH_TYPE::CUSTOM,
			2,
			"A0"
			});
	}

	// #STR: "particles/generic_gameplay/screen_death_indicator.vpcf"
	// push		rbx
	// sub		rsp, 40h
	// mov		rbx, rcx
	// call		sub_18159CC20   ; #STR: "dota_portrait_unit_stats_changed", "dota_force_portrait_update" <<<<
	//
	// sub_18159CC20:
	// push		rbx
	// push		rdi
	// push		r14
	// sub		rsp, 40h
	// mov		[rsp+58h+arg_10], rsi
	// mov		rdi, rcx
	// mov		esi, edx
	// call		sub_180271290
	// call		sub_181281B10
	// mov		rcx, cs:qword_1851BA2C0
	// mov		ebx, eax
	// mov		r8, [rcx]
	// call		qword ptr [r8+130h]
	// xor		r14d, r14d
	// test		al, al
	// jnz		short loc_18159CCAA
	// mov		rcx, cs:qword_1851BA2C0
	// mov		rdx, [rcx]
	// call		qword ptr [rdx+288h]
	// test		al, al
	// jnz		short loc_18159CCAA <<<<
	if (ConfigManager::config_entries["visible_by_enemy"]) {
		Patches::add_patch({
			"visible_by_enemy",
			"client.dll",
			Patches::Patterns::visible_by_enemy,
			PATCH_TYPE::JMP
			});
	}

	for (const auto& patch : Patches::g_patches) {
		const auto patch_addr = Memory::pattern_scan(patch.module, patch.pattern);
		if (!patch_addr) {
			LOG::ERR("Pattern for \"{}\" not found!", patch.name);
			continue;
		}

		LOG::INFO("\"{}\" patch addr -> [{}]", patch.name, TO_VOID(patch_addr.value()));

		if (!Memory::patch(patch_addr.value() + patch.offset, patch.patch_type, patch.custom_patch_bytes)) {
			LOG::ERR("Failed to patch \"{}\"!", patch.name);
			continue;
		}

		LOG::INFO("\"{}\" patched successfully", patch.name);
	}
}
