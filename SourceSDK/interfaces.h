#pragma once
#include "CEngineClient.h"
#include "CSource2Client.h"
#include "CDOTAGamerules.h"
#include "CDOTACamera.h"
#include "CGameResourceService.h"
#include "CGameEntitySystem.h"
#include "CSchemaSystem.h"
#include "CCvar.h"

static class dota_interfaces {
public:
    static inline CEngineClient* engine = nullptr;
    static inline CGameResourceService* game_resource = nullptr;
    static inline CSource2Client* client = nullptr;
    static inline CDOTAGamerules* gamerules = nullptr;
    static inline CDOTACamera* camera = nullptr;
    static inline CGameEntitySystem* entity_system = nullptr;
    static inline CSchemaSystem* schema_system = nullptr;
    static inline CCvar* cvar = nullptr;
} vmt;
