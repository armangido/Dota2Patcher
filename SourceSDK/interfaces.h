#pragma once
#include "CSource2Client.h"
#include "CSource2Server.h"
#include "CEngineClient.h"
#include "CDOTAGamerules.h"
#include "CDOTACamera.h"
#include "CGameResourceService.h"
#include "CGameEntitySystem.h"
#include "CSchemaSystem.h"
#include "CParticleSystemMgr.h"
#include "CCvar.h"

static class dota_interfaces {
public:
    static inline CSource2Client* client = nullptr;
    static inline CSource2Server* server = nullptr;
    static inline CEngineClient* engine = nullptr;
    static inline CGameResourceService* game_resource = nullptr;
    static inline CDOTAGamerules* gamerules = nullptr;
    static inline CDOTACamera* camera = nullptr;
    static inline CGameEntitySystem* c_entity_system = nullptr;
    static inline CGameEntitySystem* s_entity_system = nullptr;
    static inline CSchemaSystem* schema_system = nullptr;
    static inline CParticleSystemMgr* particles = nullptr;
    static inline CCvar* cvar = nullptr;
} vmt;
