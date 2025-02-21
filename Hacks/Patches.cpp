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
// movzx	eax, byte ptr [rcx+0A64h]
// xor		r13d, r13d
// shr		al, 7
// movzx	r12d, dl
// mov		[rsp+138h+arg_8], r13d
// mov		rsi, rcx
// mov		ebp, r13d
// cmp		dl, al
// jz		loc_18003A334 <<<<
	if (ConfigManager::set_rendering_enabled) {
		Patches::add_patch({
			"set_rendering_enabled",
			"particles.dll",
			Patches::Patterns::set_rendering_enabled,
			PATCH_TYPE::TEST,
			1
			});

		// idk for some reason set_rendering_enabled causes to crash without this fix
		// C_SceneEntity almost the last vfunc
		// #STR: "C_SceneEntity::SetupClientOnlyScene:  Couldn't determine d, "!self", "couldn't load scene file %s\n", "Failed to find soundevent '%s' when falling back from miss"
		// In the middle there will be #STR: "blank"
		// xor		edx, edx
		// lea		rcx, [rsp+180h+var_120]
		// call		cs:?Purge@CBufferString@@QEAAXH@Z ; CBufferString::Purge(int)
		// mov		rdi, [rsi+598h]
		// lea		rdx, [rbp+80h+arg_18]
		// mov		rcx, r15
		// call		sub_1802CFE30
		// mov		r8, rax
		// mov		[rbp+80h+arg_0], rbx
		// lea		rdx, [rbp+80h+arg_0]
		// mov		rcx, rdi
		// call		sub_1829D6BD0 ; #STR: "blank" <<<<
		// jmp		loc_1814170EA
		//
		// sub_1829D6BD0:
		// mov		r13, [rax]
		// mov[rsp + 88h + arg_0],	r13
		// test		r13, r13
		// jz		loc_1829D6ED8 <<<<
		// mov		rbx, [rbx]
		// test		rbx, rbx
		// jz		short loc_1829D6C42
		// mov		rbx, [rbx]
		// loc_1829D6C42: ; CODE XREF: sub_1829D6BD0+6D↑j
		Patches::add_patch({
			"set_rendering_enabled_fix",
			"client.dll",
			Patches::Patterns::set_rendering_enabled_fix,
			PATCH_TYPE::TEST,
			1
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
	if (ConfigManager::visible_by_enemy) {
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
