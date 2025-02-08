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
		static inline const string CreateInterface = "4C 8B ? ? ? ? ? 4C 8B ? 4C 8B ? 4D 85 ? 74 ? 49 8B ? ? 4D 8B";
		static inline const string CDOTACamera = "48 8D 0D ? ? ? ? 48 8D 05 ? ? ? ? 48 89 05 ? ? ? ? E9";

		static inline const string sv_cheats = "75 ? 48 8B ? ? ? ? ? BA ? ? ? ? 8B 08 FF 15 ? ? ? ? 84 C0 74 ? 45 39";
		static inline const string fog_enable = "74 ? B0 ? EB ? 32 C0 88 85";
		static inline const string set_rendering_enabled = "0F 84 ? ? ? ? 4D 89 73";
		static inline const string set_rendering_enabled_fix = "0F 84 ? ? ? ? 48 8B ? 48 85 ? 74 ? 48 8B ? 48 8B";
	};

	struct PatchInfo {
		string name;
		string module;
		string pattern;
		string patch_bytes;
		int offset = 0;
	};

	static void add_patch(const PatchInfo& patch) {
		patches.push_back(patch);
	}

	static std::vector<PatchInfo> patches;
};
