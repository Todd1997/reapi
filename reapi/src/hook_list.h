#pragma once

#define MAX_REGION_RANGE		1024
#define BEGIN_FUNC_REGION(x)			(MAX_REGION_RANGE * hooklist_t::hooks_tables_e::ht_##x)
#define MAX_HOOK_FORWARDS		1024

typedef bool (*ablfunc_t)();
typedef int (*regfunc_t)(AMX *, const char *);
typedef void (*regchain_t)();

struct regfunc
{
	template<typename T, typename R>
	regfunc(R (*)(T *, int, edict_t *, int, const char *, int, float, int, int)) {
		func = [](AMX *amx, const char *name) { return g_amxxapi.RegisterSPForwardByName(amx, name, FP_CELL, FP_CELL, FP_CELL, FP_STRING, FP_CELL, FP_FLOAT, FP_CELL, FP_CELL, FP_DONE); };
	}

	template<typename T, typename R>
	regfunc(R (*)(T *, IGameClient *, bool, const char *)) {
		func = [](AMX *amx, const char *name) { return g_amxxapi.RegisterSPForwardByName(amx, name, FP_CELL, FP_CELL, FP_STRING, FP_DONE); };
	}

	template<typename T, typename R>
	regfunc(R (*)(T *, int)) {
		func = [](AMX *amx, const char *name) { return g_amxxapi.RegisterSPForwardByName(amx, name, FP_CELL, FP_DONE); };
	}

	template<typename T, typename R>
	regfunc(R (*)(T *, struct cvar_s *, const char *)) {
		func = [](AMX *amx, const char *name) { return g_amxxapi.RegisterSPForwardByName(amx, name, FP_CELL, FP_STRING, FP_DONE); };
	}

	template<typename T, typename R>
	regfunc(R (*)(T *, CBasePlayer *)) {
		func = [](AMX *amx, const char *name) { return g_amxxapi.RegisterSPForwardByName(amx, name, FP_CELL, FP_DONE); };
	}

	template<typename T, typename R>
	regfunc(R (*)(T *, CBasePlayer *, entvars_t *, float, Vector&, TraceResult *, int)) {
		func = [](AMX *amx, const char *name) { return g_amxxapi.RegisterSPForwardByName(amx, name, FP_CELL, FP_CELL, FP_FLOAT, FP_ARRAY, FP_CELL, FP_CELL, FP_DONE); };
	}

	template<typename T, typename R>
	regfunc(R (*)(T *, CBasePlayer *, entvars_t *, entvars_t *, float, int)) {
		func = [](AMX *amx, const char *name) { return g_amxxapi.RegisterSPForwardByName(amx, name, FP_CELL, FP_CELL, FP_CELL, FP_FLOAT, FP_CELL, FP_DONE); };
	}

	template<typename T, typename R>
	regfunc(R (*)(T *, CBasePlayer *, float, int)) {
		func = [](AMX *amx, const char *name) { return g_amxxapi.RegisterSPForwardByName(amx, name, FP_CELL, FP_FLOAT, FP_CELL, FP_DONE); };
	}

	template<typename T, typename R>
	regfunc(R (*)(T *, CBasePlayer *, entvars_t *, int)) {
		func = [](AMX *amx, const char *name) { return g_amxxapi.RegisterSPForwardByName(amx, name, FP_CELL, FP_CELL, FP_CELL, FP_DONE); };
	}

	template<typename T, typename R>
	regfunc(R (*)(T *, CBasePlayer *, int, BOOL)) {
		func = [](AMX *amx, const char *name) { return g_amxxapi.RegisterSPForwardByName(amx, name, FP_CELL, FP_CELL, FP_DONE); };
	}

	template<typename T, typename R>
	regfunc(R (*)(T *, CBasePlayer *, CBasePlayerItem *)) {
		func = [](AMX *amx, const char *name) { return g_amxxapi.RegisterSPForwardByName(amx, name, FP_CELL, FP_CELL, FP_DONE); };
	}

	template<typename T, typename R>
	regfunc(R (*)(T *, CBasePlayer *, int, char *, int)) {
		func = [](AMX *amx, const char *name) { return g_amxxapi.RegisterSPForwardByName(amx, name, FP_CELL, FP_CELL, FP_STRING, FP_CELL, FP_DONE); };
	}

	template<typename T, typename R>
	regfunc(R (*)(T *, CBasePlayer *, float, float, float, int)) {
		func = [](AMX *amx, const char *name) { return g_amxxapi.RegisterSPForwardByName(amx, name, FP_FLOAT, FP_FLOAT, FP_FLOAT, FP_CELL, FP_DONE); };
	}

	template<typename T, typename R>
	regfunc(R (*)(T *, CBasePlayer *, int, bool)) {
		func = [](AMX *amx, const char *name) { return g_amxxapi.RegisterSPForwardByName(amx, name, FP_CELL, FP_CELL, FP_DONE); };
	}

	regfunc(const char *error) { UTIL_SysError(error); }	// to cause a amxx module failure.
	operator regfunc_t() const { return func; }
	regfunc_t func;

	static int current_cell;				// the counter of cells
};

struct hook_t
{
	std::vector<class CAmxxHook *> pre;		// pre forwards
	std::vector<class CAmxxHook *> post;		// post forwards

	const char *func_name;			// function name
	const char *depend_name;		// platform dependency

	ablfunc_t checkRequirements;
	regfunc_t registerForward;		// AMXX forward registration function
	regchain_t registerHookchain;		// register re* API hook
	regchain_t unregisterHookchain;		// unregister re* API hook
};

extern hook_t hooklist_engine[];
extern hook_t hooklist_gamedll[];
extern hook_t hooklist_player[];
extern const size_t hooklist_engine_size;
extern const size_t hooklist_gamedll_size;
extern const size_t hooklist_player_size;

struct hooklist_t
{
	hook_t *operator[](size_t hook) const
	{
#define CASE(h)	case ht_##h: if (index < hooklist_##h##_size) return &hooklist_##h[index]; else break;

		const auto table = hooks_tables_e(hook / MAX_REGION_RANGE);
		const auto index = hook & (MAX_REGION_RANGE - 1);

		switch (table) {
			CASE(engine)
			CASE(gamedll)
			CASE(player)
		}

		return nullptr;
		/*const auto table = hooks_tables_e(hook / MAX_REGION_RANGE);
		const auto index = hook & (MAX_REGION_RANGE - 1);

		switch (table) {
		case ht_engine: return &hooklist_engine[index];
		case ht_gamedll: return &hooklist_gamedll[index];
		case ht_player: return &hooklist_player[index];
		}

		return nullptr;*/
	}

	//static hook_t *getHook(size_t hook);

	enum hooks_tables_e
	{
		ht_engine,
		ht_gamedll,
		ht_player,

		ht_end
	};
};

enum EngineFunc
{
	//RH_Steam_NotifyClientConnect = 0,
	//RH_SV_ConnectClient,
	//RH_SV_GetIDString,
	//RH_SV_SendServerinfo,
	//RH_SV_CheckProtocol,
	//RH_SVC_GetChallenge,
	//RH_SV_CheckKeyInfo,
	//RH_SV_CheckIPRestrictions,
	//RH_SV_FinishCertificateCheck,
	//RH_Steam_NotifyBotConnect,
	//RH_SV_SerializeSteamid,
	//RH_SV_CompareUserID,
	//RH_Steam_NotifyClientDisconnect,
	//RH_NET_GetPacketPreprocessor,
	//RH_ValidateCommand,
	//RH_ClientConnected,
	//RH_SV_HandleClientMessage,
	//RH_Mod_LoadBrushModel,
	//RH_Mod_LoadStudioModel,
	//RH_ExecuteServerStringCmd,
	//RH_SV_EmitEvents,
	//RH_EV_PlayReliableEvent,
	RH_SV_StartSound = BEGIN_FUNC_REGION(engine),
	//RH_PF_Remove_I,
	//RH_PF_BuildSoundMsg_I,
	//RH_SV_WriteFullClientUpdate,
	//RH_SV_CheckConsistencyResponse,
	RH_SV_DropClient,
	RH_SV_ActivateServer,
	//RH_SV_WriteVoiceCodec,
	//RH_Steam_GSGetSteamID,
	//RH_SV_TransferConsistencyInfo,
	//RH_Steam_GSBUpdateUserData,
	RH_Cvar_DirectSet,
	//RH_SV_EstablishTimeBase,
	//RH_SV_Spawn_f,

	// [...]
	RH_EngineFunc_End
};

enum GamedllFunc
{
	RH_GetForceCamera = BEGIN_FUNC_REGION(gamedll),

	// [...]
	RH_GameDLL_End
};

enum GamedllFunc_CBasePlayer
{
	// CBasePlayer virtual
	RH_CBasePlayer_Spawn = BEGIN_FUNC_REGION(player),
	RH_CBasePlayer_Precache,
	RH_CBasePlayer_ObjectCaps,
	RH_CBasePlayer_Classify,
	RH_CBasePlayer_TraceAttack,
	RH_CBasePlayer_TakeDamage,
	RH_CBasePlayer_TakeHealth,
	RH_CBasePlayer_Killed,
	RH_CBasePlayer_AddPoints,
	RH_CBasePlayer_AddPointsToTeam,
	RH_CBasePlayer_AddPlayerItem,
	RH_CBasePlayer_RemovePlayerItem,
	RH_CBasePlayer_GiveAmmo,
	RH_CBasePlayer_ResetMaxSpeed,
	RH_CBasePlayer_Jump,
	RH_CBasePlayer_Duck,
	RH_CBasePlayer_PreThink,
	RH_CBasePlayer_PostThink,
	RH_CBasePlayer_UpdateClientData,
	RH_CBasePlayer_ImpulseCommands,
	RH_CBasePlayer_RoundRespawn,
	RH_CBasePlayer_Blind,

	RH_CBasePlayer_Observer_IsValidTarget,

	// [...]
	RH_CBasePlayer_End
};
