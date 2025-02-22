#include "Hacks.h"
#include "..\Utils\Config.h"
#include "..\SourceSDK\interfaces.h"

void Hacks::find_and_set_convars() {
	GameData::dota_range_display = vmt.cvar->g_convars["dota_range_display"];

	vmt.cvar->g_convars["sv_cheats"]->set<bool>(true);
	vmt.cvar->g_convars["fow_client_nofiltering"]->set<bool>(ConfigManager::config_entries["fow_client_nofiltering"]);
	vmt.cvar->g_convars["fog_enable"]->set<bool>(!ConfigManager::config_entries["fog_enabled"]);
	vmt.cvar->g_convars["cl_weather"]->set<DOTA_WEATHER>((DOTA_WEATHER)ConfigManager::config_entries["cl_weather"]);
}
