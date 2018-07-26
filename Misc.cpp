#include "Misc.h"
#include "Util.h"
#include "CDrawManager.h"

CMisc gMisc;

void CMisc::Run(CBaseEntity* pLocal, CUserCmd* pCommand)
{
	static ConVar* viewmodel_fov = gInts.cvar->FindVar("viewmodel_fov");
	viewmodel_fov->SetValue(gCvars.misc_viewmodel_fov);
	//pLocal->setfov(gCvars.misc_fov); // im gonna keep this code here because its funny and cute - plasma
	
	if (gCvars.misc_no_push)
	{
		static ConVar* tf_avoidteammates_pushaway = gInts.cvar->FindVar("tf_avoidteammates_pushaway");
		if (tf_avoidteammates_pushaway->GetInt() == 1)
			tf_avoidteammates_pushaway->SetValue(0);
	}

	if (!(pLocal->GetFlags() & FL_ONGROUND) && pCommand->buttons & IN_JUMP)
	{
		if (gCvars.misc_autostrafe)
			if (pCommand->mousedx > 1 || pCommand->mousedx < -1)  //> 1 < -1 so we have some wiggle room
				pCommand->sidemove = pCommand->mousedx > 1 ? 450.f : -450.f;

		if (gCvars.misc_bunnyhop)
			pCommand->buttons &= ~IN_JUMP;
	}

	if (gCvars.misc_autobackstab && pLocal->GetActiveWeapon() && Util->IsReadyToBackstab(pLocal, pLocal->GetActiveWeapon()))
		pCommand->buttons |= IN_ATTACK;

	if (gCvars.misc_noisemaker_spam)
	{
		PVOID kv = Util->InitKeyValue();
		if (kv != NULL)
		{
			NoisemakerSpam(kv);
			gInts.Engine->ServerCmdKeyValues(kv);
		}
	}
	
	if (gCvars.misc_roll_speedhack && !(pCommand->buttons & IN_ATTACK) && (pCommand->buttons & IN_DUCK)) // who changed my comment >:(
	{
		Vector vLocalAngles = pCommand->viewangles;
		float speed = pCommand->forwardmove;
		if (fabs(speed) > 0.0f) {
			pCommand->forwardmove = -speed;
			pCommand->sidemove = 0.0f;
			pCommand->viewangles.y = vLocalAngles.y;
			pCommand->viewangles.y -= 180.0f;
			if (pCommand->viewangles.y < -180.0f) pCommand->viewangles.y += 360.0f;
			pCommand->viewangles.z = 90.0f;
		}
	}

	if (gCvars.misc_wowsweet && fLastTime + 0.5f < gInts.globals->curtime)
	{
		if (pLocal->szGetClass() == "Heavy" && pLocal->GetActiveWeapon() && pLocal->GetActiveWeapon()->GetSlot() == 1 && pLocal->GetFlags() & FL_DUCKING)
		{
			if (pCommand->tick_count % 2)
				gInts.Engine->ClientCmd_Unrestricted("say wow sweet.");
			else
				gInts.Engine->ClientCmd_Unrestricted("say wow sweet!");
			fLastTime = gInts.globals->curtime;
		}
	}

	if (gCvars.misc_voice && fLastSpam + 3.f < gInts.globals->curtime) //add an option for the spam delay later
	{
		char* voicemenu = NULL;
		switch ((int)gCvars.misc_voice)
		{
		case 0: voicemenu = "voicemenu 2 2"; break;
		case 1: voicemenu = "voicemenu 2 3"; break;
		case 2: voicemenu = "voicemenu 2 6"; break;
		case 3: voicemenu = "voicemenu 1 4"; break;
		case 4: voicemenu = "voicemenu 1 6"; break;
		case 5: voicemenu = "voicemenu 0 0"; break;
		case 6: voicemenu = "voicemenu 0 1"; break;
			break;
		}
		gInts.Engine->ClientCmd_Unrestricted(voicemenu);
		fLastSpam = gInts.globals->curtime;
	}

	if (gCvars.misc_lagger)
	{
		// This method is detected. You can instead try the 'use' command
		// You will need a higher lag amount for the same effect with it
		for (int i = 0; i < gCvars.misc_laggeramt; i++)
			gInts.Engine->ClientCmd("voicemenu 0 0 ");
	}
}

void CMisc::NoisemakerSpam(PVOID kv) //Credits gir https://www.unknowncheats.me/forum/team-fortress-2-a/141108-infinite-noisemakers.html
{
	char chCommand[30] = "use_action_slot_item_server";
	typedef int(__cdecl* HashFunc_t)(const char*, bool);
	static DWORD dwHashFunctionLocation = gSignatures.GetClientSignature("FF 15 ? ? ? ? 83 C4 08 89 06 8B C6");
	static HashFunc_t s_pfGetSymbolForString = (HashFunc_t)**(PDWORD*)(dwHashFunctionLocation + 0x2);
	*(PDWORD)((DWORD)kv + 0x4) = 0;
	*(PDWORD)((DWORD)kv + 0x8) = 0;
	*(PDWORD)((DWORD)kv + 0xC) = 0;
	*(PDWORD)((DWORD)kv + 0x10) = 0xDEADBEEF;
	*(PDWORD)((DWORD)kv + 0x14) = 0;
	*(PDWORD)((DWORD)kv + 0x18) = 0;
	*(PDWORD)((DWORD)kv + 0x1C) = 0;
	*(PDWORD)((DWORD)kv + 0) = s_pfGetSymbolForString(chCommand, 1);
}
