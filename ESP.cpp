#include "ESP.h"
#include "CDrawManager.h"
#include "Util.h"
/*

aaaaaaaaaaaaaaaaaaAAAAAAAAAAAAAAAAAAAAAAHHHHHHHHHHHHHHHHHHHH


*/
CESP gESP;

void CESP::Run(CBaseEntity* pLocal)
{
	if (!gCvars.esp_active)
		return;

	if (!Util->IsKeyPressed(gCvars.esp_key))
		return;

	// Start at index 1 for players
	for (int i = 1; i <= gInts.Engine->GetMaxClients(); i++)
	{
		if (!gCvars.esp_local && i == me)
			continue;

		static ConVar* crosshair = gInts.cvar->FindVar("crosshair");
		if (gCvars.esp_crosshair)
		{
			gDraw.DrawCrosshair(1);
			crosshair->SetValue(0);
		}
		else
			crosshair->SetValue(1);

		CBaseEntity* pEntity = GetBaseEntity(i);
		if (!pEntity || pEntity->IsDormant() || pEntity->GetLifeState() != LIFE_ALIVE)
			continue;
		if (gCvars.esp_enemyonly && pEntity->GetTeamNum() == pLocal->GetTeamNum())
			continue;

		Player_ESP(pLocal, pEntity);
	}

	// Skip all player indexes so we can do buildings
	for (int i = gInts.Engine->GetMaxClients() + 1; i <= gInts.EntList->GetHighestEntityIndex(); i++)
	{
		if (!gCvars.esp_buildings)
			break;

		CBaseEntity *pEntity = gInts.EntList->GetClientEntity(i);
		if (!pEntity || pEntity->IsDormant() || pEntity->GetClientClass()->iClassID != 88)
			continue;
		if (gCvars.esp_enemyonly && pEntity->GetTeamNum() == pLocal->GetTeamNum())
			continue;


		int x, y, w, h;
		SetupBounds(pEntity, x, y, w, h);

		int iY = 0;
		int iHp = pEntity->GetHealth(), iMaxHp = pEntity->GetMaxHealth();
		if (iHp > iMaxHp)
			iHp = iMaxHp;

		Vector vOrigin, yousell;
		pEntity->GetWorldSpaceCenter(vOrigin);
		if (!gDraw.WorldToScreen(vOrigin, yousell))
			continue;

		int hpBuilding = pEntity->GetBuildingHealth();
		Color clrBuildingColor = gDraw.GetPlayerColor(pEntity), clrTeam = Color(255, 255, 255, 255);
		if (pEntity->GetTeamNum() == 2)
			clrTeam = Color(255, 20, 20, 255); //red
		else if (pEntity->GetTeamNum() == 3)
			clrTeam = Color(0, 153, 255, 255);//blue

		if (gCvars.esp_name)
		{
			gDraw.DrawString(x + w + 2, y + iY, clrBuildingColor, "Sentry");
			iY += gDraw.GetESPHeight();
		}
		if (gCvars.esp_health_buildings == 0 && gCvars.esp_health_enabled || gCvars.esp_health_buildings == 2 && gCvars.esp_health_enabled)
		{
			gDraw.DrawString(x + w + 2, y + iY, clrBuildingColor, "%d HP", hpBuilding);
			iY += gDraw.GetESPHeight();
		}
		if (gCvars.esp_health_buildings == 1 && gCvars.esp_health_enabled || gCvars.esp_health_buildings == 2 && gCvars.esp_health_enabled)
		{
			gDraw.OutlineRect(x - 6, y - 1, 5, h, Color::Black());
			gDraw.DrawRect(x - 5, y + (h - (h / iMaxHp * hpBuilding)) - 1, 3, h / iMaxHp * hpBuilding, Color::Green());
		}
		if (gCvars.esp_box_buildings)
		{
			switch (gCvars.esp_box_selection)
			{
			case 0://Outlined
				gDraw.OutlineRect(x - 1, y - 1, w + 2, h + 2, Color::Black());
				gDraw.OutlineRect(x, y, w, h, clrBuildingColor);
				gDraw.OutlineRect(x + 1, y + 1, w - 2, h - 2, Color::Black());
				break;
			case 1://Not outlined
				gDraw.OutlineRect(x, y, w, h, clrBuildingColor);
				break;
			}
		}
	}
}

void CESP::Player_ESP(CBaseEntity* pLocal, CBaseEntity* pEntity)
{
	player_info_t pInfo;
	if (!gInts.Engine->GetPlayerInfo(pEntity->GetIndex(), &pInfo))
		return;

	int x, y, w, h;
	SetupBounds(pEntity, x, y, w, h);

	int x_strings = x + 3;

	Color clrPlayerCol = gDraw.GetPlayerColor(pEntity);
	Color clrBoneCol = !gCvars.esp_bones ? Color::White() : gCvars.esp_bones == 1 ? Color::Green() : clrPlayerCol;
	int iY = 0;
	//iHp is only for health bar
	int iHp = pEntity->GetHealth(), iMaxHp = pEntity->GetMaxHealth();
	if (iHp > iMaxHp)
		iHp = iMaxHp;

	int curCond = pEntity->GetCond();
	if (gCvars.esp_removeDisguise && curCond & tf_cond::TFCond_Disguised)
		pEntity->SetCond(curCond & ~tf_cond::TFCond_Disguised);
	if (gCvars.esp_removeCloak && curCond & tf_cond::TFCond_Cloaked)
			pEntity->SetCond(curCond & ~tf_cond::TFCond_Cloaked);
	if (gCvars.esp_removeTaunt && curCond & tf_cond::TFCond_Taunting)
		pEntity->SetCond(curCond & ~tf_cond::TFCond_Taunting);

	if (gCvars.esp_health == 1 && gCvars.esp_health_enabled || gCvars.esp_health == 2 && gCvars.esp_health_enabled)
	{
		gDraw.OutlineRect(x - 6, y - 1, 5, h, Color::Black());
		gDraw.DrawRect(x - 5, y + (h - (h / iMaxHp * iHp)) - 1, 3, h / iMaxHp * iHp, Color::Green());
	}
	if (gCvars.esp_name)
	{
		gDraw.DrawString(x + w + 2, y + iY, clrPlayerCol, pInfo.name);
		iY += gDraw.GetESPHeight();
	}
	if (gCvars.esp_class)
	{
		gDraw.DrawString(x + w + 2, y + iY, clrPlayerCol, "%s", pEntity->szGetClass());
		iY += gDraw.GetESPHeight();
	}
	if (gCvars.esp_health == 0 && gCvars.esp_health_enabled || gCvars.esp_health == 2 && gCvars.esp_health_enabled)
	{
		gDraw.DrawString(x + w + 2, y + iY, Color::Green(), "%d HP", pEntity->GetHealth());
		iY += gDraw.GetESPHeight();
	}
	
	if (CBaseCombatWeapon* pWep = pEntity->GetActiveWeapon())
	{
		if (gCvars.esp_weapons)
		{
			std::string name = pWep->GetName();
			Util->ReplaceString(name, "#", "");
			Util->ReplaceString(name, "_", " ");
			Util->ReplaceString(name, "TF", "");
			Util->ReplaceString(name, "Weapon", "");
			Util->ReplaceString(name, "tf weapon", "");
			Util->ReplaceString(name, "syringegun medic", "syringegun");
			Util->TrimStart(name);
			gDraw.DrawString(x + w + 2, y + iY, clrPlayerCol, "%s", name.c_str());
			iY += gDraw.GetESPHeight();
		}
	}

	if (gCvars.esp_bones_enabled) //bones
	{
		static int iLeftArmBones[] = { 8, 7, 6, 4 };
		static int iRightArmBones[] = { 11, 10, 9, 4 };
		static int iHeadBones[] = { 0, 4, 1 };
		static int iLeftLegBones[] = { 14, 13, 1 };
		static int iRightLegBones[] = { 17, 16, 1 };

		DrawBone(pEntity, iLeftArmBones, 4, clrBoneCol);
		DrawBone(pEntity, iRightArmBones, 4, clrBoneCol);

		DrawBone(pEntity, iHeadBones, 3, clrBoneCol);

		DrawBone(pEntity, iLeftLegBones, 3, clrBoneCol);
		DrawBone(pEntity, iRightLegBones, 3, clrBoneCol);
	}
	if (gCvars.esp_box)
	{
		switch (gCvars.esp_box_selection)
		{
		case 0://Outlined
			gDraw.OutlineRect(x - 1, y - 1, w + 2, h + 2, Color::Black());
			gDraw.OutlineRect(x, y, w, h, clrPlayerCol);
			gDraw.OutlineRect(x + 1, y + 1, w - 2, h - 2, Color::Black());
			break;
		case 1://Not outlined
			gDraw.OutlineRect(x, y, w, h, clrPlayerCol);
			break;
		}
	}

	if (gCvars.esp_face)
		DrawFace(pEntity, clrPlayerCol);
	{
		if (gCvars.esp_playerCond_enabled)
		{
			char* chCond = "**";
			if (gCvars.esp_playerCond)
				chCond = "[]";

			if (curCond & TFCond_Cloaked)
			{
				gDraw.DrawString(x + w + 2, y + iY, Color(102, 178, 255, 255), "%cCLOAKED%c", chCond[0], chCond[1]);
				iY += gDraw.GetESPHeight();
			}
			if (curCond & TFCond_Disguised)
			{
				gDraw.DrawString(x + w + 2, y + iY, Color(102, 178, 255, 255), "%cDisguised%c", chCond[0], chCond[1]);
				iY += gDraw.GetESPHeight();
			}
			if (curCond & TFCond_Bonked)
			{
				gDraw.DrawString(x + w + 2, y + iY, Color(102, 178, 255, 255), "%cBonked%c", chCond[0], chCond[1]);
				iY += gDraw.GetESPHeight();
			}
			if (curCond & TFCond_Slowed)
			{
				gDraw.DrawString(x + w + 2, y + iY, Color(102, 178, 255, 255), "%cSlowed%c", chCond[0], chCond[1]);
				iY += gDraw.GetESPHeight();
			}
			if (curCond & TFCond_OnFire)
			{
				gDraw.DrawString(x + w + 2, y + iY, Color(102, 178, 255, 255), "%cBurning%c", chCond[0], chCond[1]);
				iY += gDraw.GetESPHeight();
			}
			if (curCond & TFCond_Zoomed)
			{
				gDraw.DrawString(x + w + 2, y + iY, Color(102, 178, 255, 255), "%cZoomed%c", chCond[0], chCond[1]);
				iY += gDraw.GetESPHeight();
			}
			if (curCond & TFCond_Jarated)
			{
				gDraw.DrawString(x + w + 2, y + iY, Color(102, 178, 255, 255), "%cJarated%c", chCond[0], chCond[1]);
				iY += gDraw.GetESPHeight();
			}
			if ((curCond & TFCond_Ubercharged) || (curCond & TFCond_MegaHeal))
			{
				gDraw.DrawString(x + w + 2, y + iY, Color(102, 178, 255, 255), "%cUbered%c", chCond[0], chCond[1]);
				iY += gDraw.GetESPHeight();
			}
			else if (curCond & TFCond_UberchargeFading)
			{
				gDraw.DrawString(x + w + 2, y + iY, Color(102, 178, 255, 255), "%cUber Fading%c", chCond[0], chCond[1]);
				iY += gDraw.GetESPHeight();
			}
			if (curCond & TFCond_Healing)
			{
				gDraw.DrawString(x + w + 2, y + iY, Color(102, 178, 255, 255), "%cOver Healed%c", chCond[0], chCond[1]);
				iY += gDraw.GetESPHeight();
			}
		}
	}
}

void CESP::DrawFace(CBaseEntity* pEntity, Color clr)
{
	Vector head = pEntity->GetHitboxPosition(0);
	Vector screen;

	if (!gDraw.WorldToScreen(head, screen))
		return;

	gDraw.DrawRect(screen.x, screen.y, 25, 25, clr);
	gDraw.DrawRect(screen.x + 5, screen.y + 5, 2, 2, Color::Black()); // left eye
	gDraw.DrawRect(screen.x + 5, screen.y + 10, 2, 2, Color::Black());
	gDraw.DrawRect(screen.x + 15, screen.y + 3, 3, 15, Color::Black());

}

void CESP::SetupBounds(CBaseEntity* pEntity, int& x, int& y, int& w, int& h)
{
	if (!pEntity)
		return;

	const matrix3x4& vMatrix = pEntity->GetRgflCoordinateFrame();
	Vector vMin = pEntity->GetCollideableMins(), vMax = pEntity->GetCollideableMaxs();

	Vector vPointList[] = {
		Vector(vMin.x, vMin.y, vMin.z),
		Vector(vMin.x, vMax.y, vMin.z),
		Vector(vMax.x, vMax.y, vMin.z),
		Vector(vMax.x, vMin.y, vMin.z),
		Vector(vMax.x, vMax.y, vMax.z),
		Vector(vMin.x, vMax.y, vMax.z),
		Vector(vMin.x, vMin.y, vMax.z),
		Vector(vMax.x, vMin.y, vMax.z)
	};
	Vector a;
	Vector vTransformed[8];

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 3; j++)
			vTransformed[i][j] = vPointList[i].Dot((Vector&)vMatrix[j]) + vMatrix[j][3];

	Vector flb, brt, blb, frt, frb, brb, blt, flt;

	if (!gDraw.WorldToScreen(vTransformed[3], flb) ||
		!gDraw.WorldToScreen(vTransformed[0], blb) ||
		!gDraw.WorldToScreen(vTransformed[2], frb) ||
		!gDraw.WorldToScreen(vTransformed[6], blt) ||
		!gDraw.WorldToScreen(vTransformed[5], brt) ||
		!gDraw.WorldToScreen(vTransformed[4], frt) ||
		!gDraw.WorldToScreen(vTransformed[1], brb) ||
		!gDraw.WorldToScreen(vTransformed[7], flt))
		return;

	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	float left = flb.x;
	float top = flb.y;
	float right = flb.x;
	float bottom = flb.y;

	for (int i = 0; i < 8; i++)
	{
		if (left > arr[i].x)
			left = arr[i].x;
		if (top < arr[i].y)
			top = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (bottom > arr[i].y)
			bottom = arr[i].y;
	}

	x = left, y = bottom, w = right - left, h = top - bottom;

	// ESP boxes too thicc ;3
	x += ((right - left) / 8);
	w -= ((right - left) / 8) * 2;
}

//My code, but creds to f1ssion for giving me the idea
void CESP::DrawBone(CBaseEntity* pEntity, int* iBones, int count, Color clrCol)
{
	for (int i = 0; i < count; i++)
	{
		if (i == count - 1)
			continue;

		Vector vBone1 = pEntity->GetHitboxPosition(iBones[i]);
		Vector vBone2 = pEntity->GetHitboxPosition(iBones[i + 1]);

		Vector vScr1, vScr2;

		if (!gDraw.WorldToScreen(vBone1, vScr1) || !gDraw.WorldToScreen(vBone2, vScr2))
			continue;

		gDraw.DrawLine(vScr1.x, vScr1.y, vScr2.x, vScr2.y, clrCol);
	}
}
