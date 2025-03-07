#pragma once

static void draw_logo() {
	const char* ascii = R"(
  _____        _        ___  _____      _       _               
 |  __ \      | |      |__ \|  __ \    | |     | |              
 | |  | | ___ | |_ __ _   ) | |__) |_ _| |_ ___| |__   ___ _ __ 
 | |  | |/ _ \| __/ _` | / /|  ___/ _` | __/ __| '_ \ / _ \ '__|
 | |__| | (_) | || (_| |/ /_| |  | (_| | || (__| | | |  __/ |   
 |_____/ \___/ \__\__,_|____|_|   \__,_|\__\___|_| |_|\___|_|   

 [!] To open settings, launch Dota2Patcher with SHIFT pressed
)";
	cout << ascii << endl;
}

class Patches {
public:
	struct Patterns {
		static inline const string create_interface = "4C 8B ? ? ? ? ? 4C 8B ? 4C 8B ? 4D 85 ? 74 ? 49 8B ? ? 4D 8B";
		static inline const string CDOTACamera = "48 8D ? ? ? ? ? 48 8D ? ? ? ? ? 48 89 ? ? ? ? ? E9 ? ? ? ? CC CC CC CC CC CC CC CC 48 8D";
		static inline const string VMatrix = "48 8D 0D ? ? ? ? 48 C1 E0 06";

		static inline const string set_rendering_enabled = "0F 84 ? ? ? ? 4D 89 73";
		static inline const string set_rendering_enabled_fix = "0F 84 ? ? ? ? 48 8B ? 48 85 ? 74 ? 48 8B ? 48 8B";
		static inline const string visible_by_enemy = "75 ? 41 8B CE E8 ? ? ? ? 48 85 C0 74 ? 80 B8";
	};

	struct PatchInfo {
		string name;
		string module;
		string pattern;
		PATCH_TYPE patch_type;
		int offset = 0;
		string custom_patch_bytes;
	};

	static void add_patch(const PatchInfo& patch) {
		g_patches.push_back(patch);
	}

	static inline std::vector<PatchInfo> g_patches;
};
