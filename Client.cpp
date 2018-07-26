#include "SDK.h"
#include "Client.h"
#include "Util.h"
#include "Aimbot.h"
#include "Triggerbot.h"
#include "AutoAirblast.h"


Vector qLASTTICK;

//============================================================================================
bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand)
{
	VMTManager& hook = VMTManager::GetHook(ClientMode); //Get a pointer to the instance of your VMTManager with the function GetHook.
	bool bReturn = hook.GetMethod<bool(__thiscall*)(PVOID, float, CUserCmd*)>(gOffsets.iCreateMoveOffset)(ClientMode, input_sample_frametime, pCommand); //Call the original.
	try
	{
		if (!pCommand->command_number)
			return false;

		CBaseEntity* pLocal = GetBaseEntity(me);

		if (!pLocal)
			return bReturn;

		gAim.Run(pLocal, pCommand);
		gTrigger.Run(pLocal, pCommand);
		gBlast.Run(pLocal, pCommand);
	}
	catch (...)
	{
		Log::Fatal("Failed Hooked_CreateMove");
	}
	qLASTTICK = pCommand->viewangles;
	return false/*bReturn*/;
}
//============================================================================================
void __fastcall Hooked_FrameStageNotify(void* _this, void* _edx, ClientFrameStage_t stage)
{
	if (gInts.Engine->IsInGame() && stage == FRAME_RENDER_START && gCvars.misc_thirdPerson) //shows antiaims just fine
	{
		CBaseEntity *oEntity = gInts.EntList->GetClientEntity(gInts.Engine->GetLocalPlayer());

		auto *m_angEyeAngles = reinterpret_cast<float*>(reinterpret_cast<DWORD>(oEntity) + gNetVars.get_offset("DT_BasePlayer", "pl", "deadflag") + 8);

		auto *HTC = reinterpret_cast<float*>(reinterpret_cast<DWORD>(oEntity) + gNetVars.get_offset("DT_BasePlayer", "pl", "deadflag") + 4);

		*HTC = qLASTTICK.x;
		*m_angEyeAngles = qLASTTICK.y;

		ConVar* sv_cheats = gInts.cvar->FindVar("sv_cheats");
		if (sv_cheats->GetInt() == 0) sv_cheats->SetValue(1);
		ConVar* pThirdCamYaw = gInts.cvar->FindVar("cam_idealyaw");
		gInts.Engine->ClientCmd_Unrestricted("thirdperson");

		pThirdCamYaw->SetValue(0);
	}
	else if (!gCvars.misc_thirdPerson)
	{
		gInts.Engine->ClientCmd_Unrestricted("firstperson");
	}

	bool bSkyNeedsUpdate = true;
	if (bSkyNeedsUpdate && gInts.Engine->IsInGame())
	{
		typedef bool(_cdecl* LoadNamedSkysFn)(const char*);
		static LoadNamedSkysFn LoadSkys = (LoadNamedSkysFn)gSignatures.GetEngineSignature("55 8B EC 81 EC ? ? ? ? 8B 0D ? ? ? ? 53 56 57 8B 01 C7 45");

		auto OriginalSky = gInts.cvar->FindVar("sv_skyname")->GetString();
		OriginalSky;
		if (gInts.cvar->FindVar("sv_skyname")->GetString() != "sky_night_01")
		{
			if (gCvars.sky_changer_value == 1)
			{
				LoadSkys("sky_night_01");
				bool bSkyNeedsUpdate = true;
			}
		}

		if (gInts.cvar->FindVar("sv_skyname")->GetString() != "sky_nightfall_01")
		{
			if (gCvars.sky_changer_value == 2)
			{
				LoadSkys("sky_nightfall_01");
				bool bSkyNeedsUpdate = true;
			}
		}

		if (gInts.cvar->FindVar("sv_skyname")->GetString() != "sky_harvest_night_01")
		{
			if (gCvars.sky_changer_value == 3)
			{
				LoadSkys("sky_harvest_night_01");
				bool bSkyNeedsUpdate = true;
			}
		}

		if (gInts.cvar->FindVar("sv_skyname")->GetString() != "sky_halloween")
		{
			if (gCvars.sky_changer_value == 4)
			{
				LoadSkys("sky_halloween");
				bool bSkyNeedsUpdate = true;
			}
		}

		if (gInts.cvar->FindVar("sv_skyname")->GetString() != "skybox")
		{
			if (gCvars.sky_changer_value == 0)
			{
				LoadSkys(OriginalSky);
				bool bSkyNeedsUpdate = true;
			}
		}
		bSkyNeedsUpdate = false;
	}
	
	for (auto i = 1; i <= gInts.Engine->GetMaxClients(); i++)
	{
		CBaseEntity* pEntity = gInts.EntList->GetClientEntity(i);
		CBaseEntity *entity = nullptr;
		player_info_t pInfo;

		if (!(entity = gInts.EntList->GetClientEntity(i)))
			continue;
		if (entity->IsDormant())
			continue;
		if (!gInts.Engine->GetPlayerInfo(i, &pInfo))
			continue;
		if (!entity->GetLifeState() == LIFE_ALIVE)
			continue;

		if (gCvars.misc_bighead)
		{
			auto *size = reinterpret_cast<float*>(reinterpret_cast<DWORD>(pEntity) + gNetVars.get_offset("DT_TFPlayer", "m_flHeadScale"));
			*size = gCvars.misc_bigheadsize;
			//*size = 7.0f;
		}
		else if (!gCvars.misc_bighead)
		{
			auto *size = reinterpret_cast<float*>(reinterpret_cast<DWORD>(pEntity) + gNetVars.get_offset("DT_TFPlayer", "m_flHeadScale"));
			*size = 1.0f;
		}
		if (gCvars.misc_bighead)
		{
			auto *size = reinterpret_cast<float*>(reinterpret_cast<DWORD>(pEntity) + gNetVars.get_offset("DT_TFPlayer", "m_flTorsoScale"));
			*size = gCvars.misc_torsosize;
		}
		else if (!gCvars.misc_bighead)
		{
			auto *size = reinterpret_cast<float*>(reinterpret_cast<DWORD>(pEntity) + gNetVars.get_offset("DT_TFPlayer", "m_flTorsoScale"));
			*size = 1.0f;
		}
		if (gCvars.misc_bighead)
		{
			auto *size = reinterpret_cast<float*>(reinterpret_cast<DWORD>(pEntity) + gNetVars.get_offset("DT_TFPlayer", "m_flHandScale"));
			*size = gCvars.misc_handsize;
		}
		else if (!gCvars.misc_bighead)
		{
			auto *size = reinterpret_cast<float*>(reinterpret_cast<DWORD>(pEntity) + gNetVars.get_offset("DT_TFPlayer", "m_flHandScale"));
			*size = 1.0f;
		}
	}

	if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		for (auto i = 1; i <= gInts.Engine->GetMaxClients(); i++)
		{
			CBaseEntity *entity = nullptr;
			player_info_t temp;

			if (!(entity = gInts.EntList->GetClientEntity(i)))
				continue;

			if (entity->IsDormant())
				continue;

			if (!gInts.Engine->GetPlayerInfo(i, &temp))
				continue;

			if (!entity->GetLifeState() == LIFE_ALIVE)
				continue;

			Vector vX = entity->GetAngles();
			Vector vY = entity->GetAnglesHTC();
			auto *m_angEyeAnglesX = reinterpret_cast<float*>(reinterpret_cast<DWORD>(entity) + gNetVars.get_offset("DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]"));
			auto *m_angEyeAnglesY = reinterpret_cast<float*>(reinterpret_cast<DWORD>(entity) + gNetVars.get_offset("DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[1]"));
			if (gCvars.aa_resolver)
			{
				if (vX.x == 90) //Fake Up resolver
				{
					*m_angEyeAnglesX = -89;
				}

				if (vX.x == -90) //Fake Down resolver
				{
					*m_angEyeAnglesX = 89;
				}
			}
		}
	}
	
	auto &hook = VMTManager::GetHook(gInts.Client);
	hook.GetMethod<void(__thiscall *)(PVOID, ClientFrameStage_t)>(35)(gInts.Client, stage);
}
//============================================================================================
int __fastcall Hooked_KeyEvent(PVOID CHLClient, int edx, int eventcode, int keynum, const char *currentBinding)
{
	/*if (eventcode == 1)
	{
		if (keynum == 72) //insert
		{
			gCheatMenu.bMenuActive = !gCheatMenu.bMenuActive;
		}

		if (gCheatMenu.bMenuActive)
		{
			if (keynum == 88 || keynum == 112) // Up
			{

				if (gCheatMenu.iMenuIndex > 0) gCheatMenu.iMenuIndex--;
				else gCheatMenu.iMenuIndex = gCheatMenu.iMenuItems - 1;
				return 0;

			}
			else if (keynum == 90 || keynum == 113) // Down
			{
				if (gCheatMenu.iMenuIndex < gCheatMenu.iMenuItems - 1) gCheatMenu.iMenuIndex++;
				else gCheatMenu.iMenuIndex = 0;
				return 0;

			}
			else if (keynum == 89 || keynum == 107) // Left
			{
				if (gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value)
				{
					gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value[0] -= gCheatMenu.pMenu[gCheatMenu.iMenuIndex].flStep;
					if (gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value[0] < gCheatMenu.pMenu[gCheatMenu.iMenuIndex].flMin)
						gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value[0] = gCheatMenu.pMenu[gCheatMenu.iMenuIndex].flMax;
				}
				return 0;

			}
			else if (keynum == 91 || keynum == 108) // Right
			{
				if (gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value)
				{
					gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value[0] += gCheatMenu.pMenu[gCheatMenu.iMenuIndex].flStep;
					if (gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value[0] > gCheatMenu.pMenu[gCheatMenu.iMenuIndex].flMax)
						gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value[0] = gCheatMenu.pMenu[gCheatMenu.iMenuIndex].flMin;
				}
				return 0;

			}

		}
	}
	*/
	VMTManager &hook = VMTManager::GetHook(CHLClient); // Get a pointer to the instance of your VMTManager with the function GetHook.
	return hook.GetMethod<int(__thiscall *)(PVOID, int, int, const char *)>(gOffsets.iKeyEventOffset)(CHLClient, eventcode, keynum, currentBinding); // Call the original.
}
