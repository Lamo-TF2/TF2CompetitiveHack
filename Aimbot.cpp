#include "Aimbot.h"
#include "Util.h"
#include "Panels.h"

CAimbot gAim;

mstudiobbox_t *CAimbot::get_studio_hitbox(int Hitbox, studiohdr_t* Header)
{
	int HitboxSetIndex = *(int *)((uintptr_t)Header + 0xB0);
	mstudiohitboxset_t *Set = (mstudiohitboxset_t *)(((uintptr_t)Header) + HitboxSetIndex);

	return (mstudiobbox_t *)Set->pHitbox(Hitbox);
}

void CAimbot::vec_transform(Vector &in1, const matrix3x4 &in2, Vector &out)
{
	out[0] = (in1[0] * in2[0][0] + in1[1] * in2[0][1] + in1[2] * in2[0][2]) + in2[0][3];
	out[1] = (in1[0] * in2[1][0] + in1[1] * in2[1][1] + in1[2] * in2[1][2]) + in2[1][3];
	out[2] = (in1[0] * in2[2][0] + in1[1] * in2[2][1] + in1[2] * in2[2][2]) + in2[2][3];
}

Vector CAimbot::get_hitbox(CBaseEntity *ent, int Hitbox)
{
	Vector TargetHitbox;

	if (ent == NULL || ent->GetIndex() == 0)
		return TargetHitbox;

	studiohdr_t* StudioHdr = gInts.ModelInfo->GetStudiomodel(ent->GetModel());

	matrix3x4 BoneToWorld[128];
	if (!ent->SetupBones(BoneToWorld, 128, 0x100, 0))
		return TargetHitbox;


	mstudiobbox_t *box = get_studio_hitbox(Hitbox, StudioHdr);

	Vector Min, Max;

	vec_transform(box->bbmin, BoneToWorld[box->bone], Min);
	vec_transform(box->bbmax, BoneToWorld[box->bone], Max);

	TargetHitbox = (Min + Max) / 2;

	return TargetHitbox;
}

Vector CAimbot::GetHitbox(CBaseEntity* ent, int hitbox)
{
	DWORD* model = ent->GetModel();
	if (!model)
		return Vector();

	studiohdr_t* hdr = gInts.ModelInfo->GetStudiomodel(model);
	if (!hdr)
		return Vector();

	matrix3x4 matrix[128];
	if (!ent->SetupBones(matrix, 128, 0x100, gInts.globals->curtime))
		return Vector();

	mstudiohitboxset_t* set = hdr->GetHitboxSet(ent->GetHitboxSet());
	if (!set)
		return Vector();

	mstudiobbox_t* box = set->pHitbox(hitbox);
	if (!box)
		return Vector();

	Vector center = (box->bbmin + box->bbmax) * 0.5f;

	Vector hitboxpos;

	Util->VectorTransform(center, matrix[box->bone], hitboxpos);

	return hitboxpos;
}

float VectorialDistanceToGround(Vector origin)
{
	static trace_t* ground_trace = new trace_t();
	Ray_t ray;
	Vector endpos = origin;
	endpos.z -= 8192;
	ray.Init(origin, endpos);
	gInts.EngineTrace->TraceRay(ray, MASK_PLAYERSOLID, NULL, ground_trace);
	return 8192.0f * ground_trace->fraction;
}

Vector SimpleLatencyPrediction(CBaseEntity* ent, int hb)
{
	if (!ent) return Vector();
	Vector result;
	result = ent->GetHitboxPosition(hb);
	float latency = ((INetChannel*)gInts.Engine->GetNetChannelInfo())->GetLatency(FLOW_OUTGOING) +
		((INetChannel*)gInts.Engine->GetNetChannelInfo())->GetLatency(FLOW_INCOMING);
	Vector vel = Util->EstimateAbsVelocity(ent);
	result += vel * latency;
	return result;
}
float PlayerGravityMod(CBaseEntity* player)
{
	int movetype = player->GetMoveType();
	if (movetype == MOVETYPE_FLY || movetype == MOVETYPE_NOCLIP)
		return 0.0f;

	return 1.0f;
}
float CAimbot::DistanceToGround(CBaseEntity* ent)
{
	if (ent->GetFlags() & FL_ONGROUND) return 0;

	Vector& origin = ent->GetVecOrigin();
	float v1 = VectorialDistanceToGround(origin + Vector(10.0f, 10.0f, 0.0f));
	float v2 = VectorialDistanceToGround(origin + Vector(-10.0f, 10.0f, 0.0f));
	float v3 = VectorialDistanceToGround(origin + Vector(10.0f, -10.0f, 0.0f));
	float v4 = VectorialDistanceToGround(origin + Vector(-10.0f, -10.0f, 0.0f));
	return min(v1, min(v2, min(v3, v4)));
}


bool CAimbot::CanAmbassadorHeadshot(CBaseEntity* pLocal)
{
	return gInts.globals->curtime - pLocal->GetActiveWeapon()->m_flLastFireTime() >= 0.930;//1.0;
}
#include "CDrawManager.h"
bool isEntity = false;


void CAimbot::Run(CBaseEntity* pLocal, CUserCmd* pCommand)
{
	gCvars.iAimbotIndex = -1;

	if (!gCvars.aimbot_active) return;


	if (!pLocal->GetActiveWeapon()) return;

	auto id = pLocal->GetActiveWeapon()->GetItemDefinitionIndex();
	if (
#pragma region scout
		id == (int)scoutweapons::WPN_Sandman
		|| id == (int)scoutweapons::WPN_CritCola
		|| id == (int)scoutweapons::WPN_CritCola
		|| id == (int)scoutweapons::WPN_FlyingGuillotine1
		|| id == (int)scoutweapons::WPN_FlyingGuillotine2
		|| id == (int)scoutweapons::WPN_Milk
		|| id == (int)scoutweapons::WPN_MutatedMilk
		|| id == (int)scoutweapons::WPN_WrapAssassin
#pragma endregion
#pragma region soldier
		|| id == (int)soldierweapons::WPN_RocketLauncher
		|| id == (int)soldierweapons::WPN_FestiveRocketLauncher
		|| id == (int)soldierweapons::WPN_Airstrike
		|| id == (int)soldierweapons::WPN_BattalionBackup
		|| id == (int)soldierweapons::WPN_BeggersBazooka
		|| id == (int)soldierweapons::WPN_BlackBox
		|| id == (int)soldierweapons::WPN_FestiveBlackbox
		|| id == (int)soldierweapons::WPN_BuffBanner
		|| id == (int)soldierweapons::WPN_FestiveBuffBanner
		|| id == (int)soldierweapons::WPN_Concheror
		|| id == (int)soldierweapons::WPN_CowMangler
		|| id == (int)soldierweapons::WPN_DirectHit
		|| id == (int)soldierweapons::WPN_LibertyLauncher
		|| id == (int)soldierweapons::WPN_Original
		|| id == (int)soldierweapons::WPN_RighteousBison
		|| id == (int)soldierweapons::WPN_RocketJumper
#pragma endregion
#pragma region pyro
		|| id == (int)pyroweapons::WPN_Detonator
		|| id == (int)pyroweapons::WPN_FestiveFlaregun
		|| id == (int)pyroweapons::WPN_Flaregun
		|| id == (int)pyroweapons::WPN_ScorchShot
#pragma endregion
#pragma region demo
		|| id == (int)demomanweapons::WPN_FestiveGrenadeLauncher
		|| id == (int)demomanweapons::WPN_FestiveStickyLauncher
		|| id == (int)demomanweapons::WPN_GrenadeLauncher
		|| id == (int)demomanweapons::WPN_IronBomber
		|| id == (int)demomanweapons::WPN_LochNLoad
		|| id == (int)demomanweapons::WPN_LoooseCannon
		|| id == (int)demomanweapons::WPN_QuickieBombLauncher
		|| id == (int)demomanweapons::WPN_StickyLauncher
		|| id == (int)demomanweapons::WPN_ScottsSkullctter
#pragma endregion
#pragma region heavy
		|| id == (int)heavyweapons::WPN_FestiveSandvich
		|| id == (int)heavyweapons::WPN_RobotSandvich
		|| id == (int)heavyweapons::WPN_Sandvich
		|| id == (int)heavyweapons::WPN_Steak
		|| id == (int)heavyweapons::WPN_CandyBar
		|| id == (int)heavyweapons::WPN_Fishcake
#pragma endregion
#pragma region engineer
		|| id == (int)engineerweapons::WPN_Builder
		|| id == (int)engineerweapons::WPN_Builder2
		|| id == (int)engineerweapons::WPN_Destructor
		|| id == (int)engineerweapons::WPN_FestiveWrangler
		|| id == (int)engineerweapons::WPN_Pomson
		|| id == (int)engineerweapons::WPN_RescueRanger
		|| id == (int)engineerweapons::WPN_ShortCircut
		|| id == (int)engineerweapons::WPN_Toolbox
		|| id == (int)engineerweapons::WPN_Wrangler
#pragma endregion
#pragma region medic
		|| id == (int)medicweapons::WPN_Blutsauger
		|| id == (int)medicweapons::WPN_FestiveCrossbow
		|| id == (int)medicweapons::WPN_FestiveMedigun
		|| id == (int)medicweapons::WPN_Kritzkrieg
		|| id == (int)medicweapons::WPN_Medigun
		|| id == (int)medicweapons::WPN_Overdose
		|| id == (int)medicweapons::WPN_QuickFix
		|| id == (int)medicweapons::WPN_SyringeGun
		|| id == (int)medicweapons::WPN_Vaccinator

#pragma endregion
#pragma region sniper
		|| id == (int)sniperweapons::WPN_CompoundBow
		|| id == (int)sniperweapons::WPN_FestiveJarate
		|| id == (int)sniperweapons::WPN_FestiveHuntsman
		|| id == (int)sniperweapons::WPN_Huntsman
		|| id == (int)sniperweapons::WPN_Jarate
#pragma endregion
#pragma region spy
		|| id == (int)spyweapons::WPN_ApSap
		|| id == (int)spyweapons::WPN_DisguiseKit
		|| id == (int)spyweapons::WPN_RedTape1
		|| id == (int)spyweapons::WPN_RedTape2
		|| id == (int)spyweapons::WPN_Sapper
		|| id == (int)spyweapons::WPN_Sapper2
		|| id == (int)spyweapons::WPN_FestiveSapper
#pragma endregion
		)
		return;

	if (!Util->IsKeyPressed(gCvars.aimbot_key))
		return;


	CBaseEntity* pEntity = GetBaseEntity(GetBestTarget(pLocal, pCommand));

	if (!pEntity)
		return;

	if (pLocal->GetCond() & TFCond_Cloaked) return;

	auto pWep = pLocal->GetActiveWeapon();
	if (gCvars.aimbot_waitforcharge)
		if (pWep->GetItemDefinitionIndex() == spyweapons::WPN_Ambassador || pWep->GetItemDefinitionIndex() == spyweapons::WPN_FestiveAmbassador)
			if (!CanAmbassadorHeadshot(pLocal)) return;

	int iBestHitbox = GetBestHitbox(pLocal, pEntity);

	if (iBestHitbox == -1)
		return;
	Vector vEntity = pEntity->GetHitboxPosition(iBestHitbox);

	Vector vLocal = pLocal->GetEyePosition();

	Vector vAngs;
	VectorAngles((vEntity - vLocal), vAngs);

	ClampAngle(vAngs);

	gCvars.iAimbotIndex = pEntity->GetIndex();

	if (gCvars.aimbot_smooth > 0.0 && !gCvars.aimbot_silent)
	{
		Vector vDelta(pCommand->viewangles - vAngs);
		AngleNormalize(vDelta);
		vAngs = pCommand->viewangles - vDelta / gCvars.aimbot_smooth;
	}

	if (gCvars.aimbot_silent)
	{
		if (pCommand->buttons & IN_ATTACK)
		{
			w(true, vAngs, pCommand);
		}
		else if (gCvars.aimbot_autoshoot)
		{
			w(true, vAngs, pCommand);
			pCommand->buttons |= IN_ATTACK;
		}
		else
		{
			if (pCommand->buttons & IN_ATTACK)
				w(true, vAngs, pCommand);
		}
	}
	else
	{
		w(gCvars.aimbot_silent, vAngs, pCommand);
		if (gCvars.aimbot_autoshoot)
			pCommand->buttons |= IN_ATTACK;
	}

}

void CAimbot::w(bool silent, Vector vAngs, CUserCmd* pCommand)
{
	if (silent)
	{
		ClampAngle(vAngs);
		Util->SilentMovementFix(pCommand, vAngs);
		pCommand->viewangles = vAngs;

	}
	else
	{
		pCommand->viewangles = vAngs;
		gInts.Engine->SetViewAngles(pCommand->viewangles);
	}
}

float DistanceToGround(CBaseEntity* ent)
{
	if (ent->GetFlags() & FL_ONGROUND) return 0;

	Vector& origin = ent->GetVecOrigin();
	float v1 = VectorialDistanceToGround(origin + Vector(10.0f, 10.0f, 0.0f));
	float v2 = VectorialDistanceToGround(origin + Vector(-10.0f, 10.0f, 0.0f));
	float v3 = VectorialDistanceToGround(origin + Vector(10.0f, -10.0f, 0.0f));
	float v4 = VectorialDistanceToGround(origin + Vector(-10.0f, -10.0f, 0.0f));
	return min(v1, min(v2, min(v3, v4)));
}

int CAimbot::GetBestTarget(CBaseEntity* pLocal, CUserCmd* pCommand)
{
	int bestTarget = -1;
	float flDistToBest = 99999.0;
	float minimalDistance = 99999.0;

	Vector vLocal = pLocal->GetEyePosition();

	for (int i = 1; i <= gInts.Engine->GetMaxClients(); i++)
	{
		if (i == me)
			continue;

		CBaseEntity* pEntity = GetBaseEntity(i);

		if (!pEntity)
			continue;

		if (pEntity->IsDormant())
			continue;

		if (pEntity->GetLifeState() != LIFE_ALIVE ||
			pEntity->GetTeamNum() == pLocal->GetTeamNum())
			continue;

		int iBestHitbox = GetBestHitbox(pLocal, pEntity);

		if (iBestHitbox == -1)
			continue;

		Vector vEntity = pEntity->GetHitboxPosition(iBestHitbox);


		if (!gCvars.PlayerMode[i])
			continue;

		if (pEntity->GetCond() & TFCond_Ubercharged ||
			pEntity->GetCond() & TFCond_Bonked)
			continue;

		if (pEntity->GetCond() & TFCond_Cloaked)
			continue;
		if (gCvars.aimbot_ignore_disguise
			&& (pEntity->GetCond() & TFCond_Disguised))
			continue;
		if (gCvars.aimbot_ignore_taunt
			&& (pEntity->GetCond() & TFCond_Taunting))
			continue;

		auto pWep = pLocal->GetActiveWeapon();
		auto pClass = pWep->GetItemDefinitionIndex();
		auto demo = pClass == demomanweapons::WPN_Sword || pClass == demomanweapons::WPN_FestiveEyelander || pClass == demomanweapons::WPN_Golfclub || pClass == demomanweapons::WPN_ScottsSkullctter || pClass == demomanweapons::WPN_Headless;
		if (pWep->GetSlot() == 2 && !demo)
			minimalDistance = (float)8.4;

		if (pClass == demomanweapons::WPN_Sword || pClass == demomanweapons::WPN_FestiveEyelander || pClass == demomanweapons::WPN_Golfclub || pClass == demomanweapons::WPN_ScottsSkullctter || pClass == demomanweapons::WPN_Headless)
			minimalDistance = 12.0;

		if (pLocal->szGetClass() == "Pyro" && (pClass == pyroweapons::WPN_Backburner || pClass == pyroweapons::WPN_Degreaser || pClass == pyroweapons::WPN_FestiveBackburner || pClass == pyroweapons::WPN_FestiveFlamethrower || pClass == pyroweapons::WPN_Flamethrower || pClass == pyroweapons::WPN_Phlogistinator || pClass == pyroweapons::WPN_Rainblower || pClass == pyroweapons::WPN_NostromoNapalmer))
			minimalDistance = 17.0;



		if (pLocal->szGetClass() == "Sniper" && pLocal->GetActiveWeapon()->GetSlot() == 0)
		{
			float damage = pLocal->GetActiveWeapon()->GetChargeDamage();
			if (gCvars.aimbot_zoomedonly)
			{
				if (!(pLocal->GetCond() & tf_cond::TFCond_Zoomed))
					return -1;

				if (damage < 10.f)
					return -1;


			}
		}

		float flFOV = GetFOV(pCommand->viewangles, vLocal, vEntity);
		float distance = Util->flGetDistance(vEntity, pLocal->GetEyePosition());

		if (distance < minimalDistance)
		{
			if (flFOV < flDistToBest && flFOV < gCvars.aimbot_fov)
			{
				if (gCvars.PlayerMode[i] == 2)
					return i;
				flDistToBest = flFOV;
				gCvars.iAimbotIndex = i;
				bestTarget = i;
			}
		}

	}

	return bestTarget;
}
Vector CAimbot::calc_angle(Vector src, Vector dst)
{
	Vector AimAngles, delta;
	float hyp;
	delta = src - dst;
	hyp = sqrtf((delta.x * delta.x) + (delta.y * delta.y));
	AimAngles.x = atanf(delta.z / hyp) * RADPI;
	AimAngles.y = atanf(delta.y / delta.x) * RADPI;
	AimAngles.z = 0.0f;
	if (delta.x >= 0.0)
		AimAngles.y += 180.0f;
	return AimAngles;
}
void CAimbot::MakeVector(Vector angle, Vector& vector)
{
	float pitch, yaw, tmp;
	pitch = float(angle[0] * PI / 180);
	yaw = float(angle[1] * PI / 180);
	tmp = float(cos(pitch));
	vector[0] = float(-tmp * -cos(yaw));
	vector[1] = float(sin(yaw)*tmp);
	vector[2] = float(-sin(pitch));
}

float CAimbot::GetFOV(Vector angle, Vector src, Vector dst)
{
	Vector ang, aim;
	float mag, u_dot_v;
	ang = calc_angle(src, dst);


	MakeVector(angle, aim);
	MakeVector(ang, ang);

	mag = sqrtf(pow(aim.x, 2) + pow(aim.y, 2) + pow(aim.z, 2));
	u_dot_v = aim.Dot(ang);

	return RAD2DEG(acos(u_dot_v / (pow(mag, 2))));
}

int CAimbot::GetBestHitbox(CBaseEntity* pLocal, CBaseEntity* pEntity)
{
	int iBestHitbox = -1;

	if (!pLocal->GetActiveWeapon()) return iBestHitbox;

	if (Util->IsHeadshotWeapon(pLocal, pLocal->GetActiveWeapon()))
		iBestHitbox = 0;
	else
		iBestHitbox = (int)tf_hitbox::pelvis;

	if (gCvars.aimbot_hitscan)
	{
		for (int i = 0; i < 17; i++)
		{
			if (Util->IsVisible(pLocal, pEntity, pLocal->GetEyePosition(), pEntity->GetHitboxPosition(i)))
				return i;
		}
	}

	if (pEntity->GetHitboxPosition(iBestHitbox).IsZero())
		return -1;

	if (!Util->IsVisible(pLocal, pEntity, pLocal->GetEyePosition(), pEntity->GetHitboxPosition(iBestHitbox)))
		return -1;

	return iBestHitbox;
}
