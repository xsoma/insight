#include "legitbot.h"
#include "../../helpers/math.hpp"
#include "../../options.hpp"
#include "../../helpers/input.hpp"
#include "legit-autowall/legit-autowall.h"
#include <chrono>
#include "../../Protected/enginer.h"
//--------------------------------------------------------------------------------
bool LegitBot::IsRcs() {
	return g_LocalPlayer->m_iShotsFired() >= settings.rcs_start + 1;
}
//--------------------------------------------------------------------------------
float GetRealDistanceFOV(float distance, QAngle angle, QAngle viewangles) {
	Vector aimingAt;
	Math::AngleVectors(viewangles, aimingAt);
	aimingAt *= distance;
	Vector aimAt;
	Math::AngleVectors(angle, aimAt);
	aimAt *= distance;
	return aimingAt.DistTo(aimAt) / 5;
}
//--------------------------------------------------------------------------------
float LegitBot::GetFovToPlayer(QAngle viewAngle, QAngle aimAngle) {
	QAngle delta = aimAngle - viewAngle;
	Math::FixAngles(delta);
	return sqrtf(powf(delta.pitch, 2.0f) + powf(delta.yaw, 2.0f));
}
//--------------------------------------------------------------------------------
bool LegitBot::IsLineGoesThroughSmoke(Vector vStartPos, Vector vEndPos) {
	static auto LineGoesThroughSmokeFn = (bool(*)(Vector vStartPos, Vector vEndPos))Utils::PatternScan(GetModuleHandleA(solution::Get().Module), "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0");
	return LineGoesThroughSmokeFn(vStartPos, vEndPos);
}
//--------------------------------------------------------------------------------
bool LegitBot::IsEnabled(CUserCmd *pCmd) {
	if (!g_EngineClient->IsConnected() || !g_LocalPlayer || !g_LocalPlayer->IsAlive()) {
		return false;
	}

	auto pWeapon = g_LocalPlayer->m_hActiveWeapon();
	if (!pWeapon || !(pWeapon->IsSniper() || pWeapon->IsPistol() || pWeapon->IsRifle())) {
		return false;
	}

	auto weaponData = pWeapon->GetCSWeaponData();
	auto weapontype = weaponData->iWeaponType;
	settings = g_Options.legitbot_items[pWeapon->m_Item().m_iItemDefinitionIndex()];
	if (!settings.enabled) {
		return false;
	}

	if ((pWeapon->m_Item().m_iItemDefinitionIndex() == WEAPON_AWP || pWeapon->m_Item().m_iItemDefinitionIndex() == WEAPON_SSG08) && settings.only_in_zoom && !g_LocalPlayer->m_bIsScoped()) {
		return false;
	}

	if (!pWeapon->HasBullets() || pWeapon->IsReloading()) {
		return false;
	}

	return !settings.on_key || InputSys::Get().IsKeyDown(settings.key);
}
//--------------------------------------------------------------------------------
float LegitBot::GetSmooth() {
	float smooth = IsRcs() && settings.rcs_smooth_enabled ? settings.rcs_smooth : settings.smooth;
	return smooth;
}
//--------------------------------------------------------------------------------
void LegitBot::Smooth(QAngle currentAngle, QAngle aimAngle, QAngle& angle) {
	auto smooth_value = GetSmooth();
	if (smooth_value <= 1) {
		return;
	}

	QAngle delta = aimAngle - currentAngle;
	Math::FixAngles(delta);
	if (settings.smooth_type == 1) {
		float deltaLength = fmaxf(sqrtf((delta.pitch * delta.pitch) + (delta.yaw * delta.yaw)), 0.01f);
		delta *= (1.0f / deltaLength);

		RandomSeed(g_GlobalVars->tickcount);
		float randomize = Math::RandomFloat(-0.1f, 0.1f);
		smooth_value = fminf((g_GlobalVars->interval_per_tick * 64.0f) / (randomize + smooth_value * 0.15f), deltaLength);
	}
	else {
		smooth_value = (g_GlobalVars->interval_per_tick * 64.0f) / smooth_value;
	}

	delta *= smooth_value;
	angle = currentAngle + delta;
	Math::FixAngles(angle);
}
//--------------------------------------------------------------------------------
void LegitBot::RCS(QAngle &angle, C_BasePlayer* target, bool should_run) {
	if (!settings.rcs) {
		RCSLastPunch.Init();
		return;
	}

	if (settings.rcs_x == 0 && settings.rcs_y == 0) {
		RCSLastPunch.Init();
		return;
	}

	QAngle punch = g_LocalPlayer->m_aimPunchAngle() * 2.0f;

	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();
	if (weapon && weapon->m_flNextPrimaryAttack() > g_GlobalVars->curtime) {
		auto delta_angles = punch - RCSLastPunch;
		auto delta = weapon->m_flNextPrimaryAttack() - g_GlobalVars->curtime;
		if (delta >= g_GlobalVars->interval_per_tick)
			punch = RCSLastPunch + delta_angles / static_cast<float>(TIME_TO_TICKS(delta));
	}

	CurrentPunch = punch;
	if (settings.rcs_type == 0 && !should_run)
		punch -= { RCSLastPunch.pitch, RCSLastPunch.yaw, 0.f };

	RCSLastPunch = CurrentPunch;
	if (!IsRcs()) {
		return;
	}

	angle.pitch -= punch.pitch * (settings.rcs_x / 100.0f);
	angle.yaw -= punch.yaw * (settings.rcs_y / 100.0f);

	Math::FixAngles(angle);
}
//--------------------------------------------------------------------------------
float LegitBot::GetFov() {
	if (IsRcs() && settings.rcs && settings.rcs_fov_enabled) return settings.rcs_fov;
	if (!silent_enabled) return settings.fov;
	return settings.silent_fov > settings.fov ? settings.silent_fov : settings.fov;
}
//--------------------------------------------------------------------------------
C_BasePlayer* LegitBot::GetClosestPlayer(CUserCmd* cmd, int &bestBone) {
	QAngle ang;
	Vector eVecTarget;
	Vector pVecTarget = g_LocalPlayer->GetEyePos();
	if (target && !kill_delay && settings.kill_delay > 0 && target->IsNotTarget()) {
		target = NULL;
		shot_delay = false;
		kill_delay = true;
		kill_delay_time = (int)GetTickCount() + settings.kill_delay;
	}
	if (kill_delay) {
		if (kill_delay_time <= (int)GetTickCount()) kill_delay = false;
		else return NULL;
	}

	C_BasePlayer* player;
	target = NULL;
	int bestHealth = 100.f;
	float bestFov = 9999.f;
	float bestDamage = 0.f;
	float bestBoneFov = 9999.f;
	float bestDistance = 9999.f;
	int health;
	float fov;
	float damage;
	float distance;
	int fromBone = settings.aim_type == 1 ? 0 : settings.hitbox;
	int toBone = settings.aim_type == 1 ? 7 : settings.hitbox;
	for (int i = 1; i < g_EngineClient->GetMaxClients(); ++i) {
		damage = 0.f;
		player = (C_BasePlayer*)g_EntityList->GetClientEntity(i);
		if (player->IsNotTarget()) {
			continue;
		}
		if (!settings.deathmatch && player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum()) {
			continue;
		}
		for (int bone = fromBone; bone <= toBone; bone++) {
			eVecTarget = player->GetHitboxPos(bone);
			Math::VectorAngles(eVecTarget - pVecTarget, ang);
			Math::FixAngles(ang);
			distance = pVecTarget.DistTo(eVecTarget);
			if (settings.fov_type == 1)
				fov = GetRealDistanceFOV(distance, ang, cmd->viewangles + RCSLastPunch);
			else
				fov = GetFovToPlayer(cmd->viewangles + RCSLastPunch, ang);

			if (fov > GetFov())
				continue;

			if (!g_LocalPlayer->CanSeePlayer(player, eVecTarget)) {

				if (!settings.autowall)
					continue;

				damage = LegitAutowall::GetDamage(eVecTarget);
				if (damage < settings.min_damage)
					continue;

			}
			if ((settings.priority == 1 || settings.priority == 2) && damage == 0.f)
				damage = LegitAutowall::GetDamage(eVecTarget);

			health = player->m_iHealth() - damage;
			if (settings.smoke_check && IsLineGoesThroughSmoke(pVecTarget, eVecTarget))
				continue;

			bool OnGround = (g_LocalPlayer->m_fFlags() & FL_ONGROUND);
			if (settings.jump_check && !OnGround)
				continue;

			if (settings.aim_type == 1 && bestBoneFov < fov) {
				continue;
			}
			bestBoneFov = fov;
			if (
				(settings.priority == 0 && bestFov > fov) ||
				(settings.priority == 1 && bestHealth > health) ||
				(settings.priority == 2 && bestDamage < damage) ||
				(settings.priority == 3 && distance < bestDistance)
				) {
				bestBone = bone;
				target = player;
				bestFov = fov;
				bestHealth = health;
				bestDamage = damage;
				bestDistance = distance;
			}
		}
	}
	return target;
}
//--------------------------------------------------------------------------------
bool LegitBot::IsNotSilent(float fov) {
	return IsRcs() || !silent_enabled || (silent_enabled && fov > settings.silent_fov);
}
//--------------------------------------------------------------------------------
void LegitBot::OnMove(CUserCmd *pCmd) {
	if (!IsEnabled(pCmd)) {
		if (g_LocalPlayer && g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive() && settings.enabled && settings.rcs_type == 0) {
			auto pWeapon = g_LocalPlayer->m_hActiveWeapon();
			if (pWeapon && (pWeapon->IsSniper() || pWeapon->IsPistol() || pWeapon->IsRifle())) {
				RCS(pCmd->viewangles, target, false);
				Math::FixAngles(pCmd->viewangles);
				g_EngineClient->SetViewAngles(&pCmd->viewangles);
			}
		}
		else {
			RCSLastPunch = { 0, 0, 0 };
		}

		is_delayed = false;
		shot_delay = false;
		kill_delay = false;
		silent_enabled = settings.silent && settings.silent_fov > 0;
		target = NULL;
		return;
	}

	RandomSeed(pCmd->command_number);

	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();
	if (!weapon)
		return;

	auto weapon_data = weapon->GetCSWeaponData();
	if (!weapon_data)
		return;

	bool should_do_rcs = false;
	QAngle angles = pCmd->viewangles;
	QAngle current = angles;
	float fov = 180.f;
	if (!(settings.flash_check && g_LocalPlayer->IsFlashed())) {
		int bestBone = -1;
		if (GetClosestPlayer(pCmd, bestBone)) {
			Math::VectorAngles(target->GetHitboxPos(bestBone) - g_LocalPlayer->GetEyePos(), angles);
			Math::FixAngles(angles);
			if (settings.fov_type == 1)
				fov = GetRealDistanceFOV(g_LocalPlayer->GetEyePos().DistTo(target->GetHitboxPos(bestBone)), angles, pCmd->viewangles);
			else
				fov = GetFovToPlayer(pCmd->viewangles, angles);

			should_do_rcs = true;

			if (!settings.silent && !is_delayed && !shot_delay && settings.shot_delay > 0) {
				is_delayed = true;
				shot_delay = true;
				shot_delay_time = GetTickCount() + settings.shot_delay;
			}

			if (shot_delay && shot_delay_time <= GetTickCount()) {
				shot_delay = false;
			}

			if (shot_delay) {
				pCmd->buttons &= ~IN_ATTACK;
			}

			if (settings.autofire) {
				if (!settings.autofire_key || InputSys::Get().IsKeyDown(settings.autofire_key)) {
					if (!weapon_data->bFullAuto) {
						if (pCmd->command_number % 2 == 0) {
							pCmd->buttons &= ~IN_ATTACK;
						}
						else {
							pCmd->buttons |= IN_ATTACK;
						}
					}
					else {
						pCmd->buttons |= IN_ATTACK;
					}
				}
			}

			if (settings.autostop) {
				pCmd->forwardmove = pCmd->sidemove = 0;
			}
		}
	}

	if (IsNotSilent(fov) && (should_do_rcs || settings.rcs_type == 0)) {
		RCS(angles, target, should_do_rcs);
	}

	if (target && IsNotSilent(fov)) {
		Smooth(current, angles, angles);
	}

	Math::FixAngles(angles);
	pCmd->viewangles = angles;

	if (IsNotSilent(fov)) {
		g_EngineClient->SetViewAngles(&angles);
	}

	silent_enabled = false;
	if (g_LocalPlayer->m_hActiveWeapon()->IsPistol() && settings.autopistol) {
		float server_time = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
		float next_shot = g_LocalPlayer->m_hActiveWeapon()->m_flNextPrimaryAttack() - server_time;
		if (next_shot > 0) {
			pCmd->buttons &= ~IN_ATTACK;
		}
	}
}

long getMils()
{
	auto duration = std::chrono::system_clock::now().time_since_epoch();

	return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

void LegitBot::TriggerBot(CUserCmd* cmd)
{
	auto Key = g_Options.legitbot_trigger_key;
	auto pWeapon = g_LocalPlayer->m_hActiveWeapon();
	
	if (!pWeapon) return;
	
	settings = g_Options.legitbot_items[pWeapon->m_Item().m_iItemDefinitionIndex()];
	if (!settings.triggerbot_enable)
		return;

	if (Key < 1)
	{
		g_Options.legitbot_trigger_pressed = true;
	}
	else if (Key > 0 && GetAsyncKeyState(Key))
	{
		g_Options.legitbot_trigger_pressed = true;
	}
	else if (Key > 0 && !GetAsyncKeyState(Key))
	{
		g_Options.legitbot_trigger_pressed = false;
	}

	if (!g_Options.legitbot_trigger_pressed)
		return;

	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	if (!Utils::HitChance(settings.triggerbot_hitchance))
		return;

	if (settings.flash_check && g_LocalPlayer->IsFlashed())
		return;

	long currentTime_ms = getMils();
	static long timeStamp = currentTime_ms;
	long oldTimeStamp;

	Vector traceStart, traceEnd;
	trace_t tr;

	QAngle viewAngles;
    g_EngineClient->GetViewAngles(&viewAngles);
	QAngle viewAngles_rcs = viewAngles + g_LocalPlayer->m_aimPunchAngle() * 2.0f;

	Math::AngleVectors(viewAngles_rcs, traceEnd);

	traceStart = g_LocalPlayer->GetEyePos();
	traceEnd = traceStart + (traceEnd * 8192.0f);

	Utils::TraceLine(traceStart, traceEnd, 0x46004003, g_LocalPlayer, &tr);

	oldTimeStamp = timeStamp;
	timeStamp = currentTime_ms;

	C_BasePlayer* Player = (C_BasePlayer*)tr.hit_entity;
	if (!Player)
		return;

	if (Player->GetClientClass()->m_ClassID != ClassId_CCSPlayer)
		return;

	if (Player == g_LocalPlayer || Player->IsDormant() || !Player->IsAlive() || Player->m_bGunGameImmunity())
		return;

	if (Player->IsTeam() && settings.deathmatch)
		return;

	if (!((settings.triggerbot_head && tr.hitgroup == HITGROUP_HEAD)
		|| (settings.triggerbot_body && (tr.hitgroup == HITGROUP_CHEST
			|| tr.hitgroup == HITGROUP_STOMACH)) || (settings.triggerbot_other && (tr.hitgroup >= HITGROUP_LEFTARM && tr.hitgroup <= HITGROUP_RIGHTLEG))))
		return;

	if (settings.smoke_check &&  IsLineGoesThroughSmoke(g_LocalPlayer->GetEyePos(), tr.endpos))
		return;

	C_BaseCombatWeapon* activeWeapon = g_LocalPlayer->m_hActiveWeapon();

	if (!activeWeapon || !activeWeapon->HasBullets())
		return;

	int itemDefinitionIndex = activeWeapon->m_Item().m_iItemDefinitionIndex();

	if (itemDefinitionIndex == WEAPON_KNIFE || itemDefinitionIndex >= WEAPON_KNIFE_BAYONET)
		return;

	int weaponType = activeWeapon->GetCSWeaponData()->iWeaponType;
	if (weaponType == WEAPONTYPE_C4 || weaponType == WEAPONTYPE_GRENADE)
		return;

	if (activeWeapon->m_flNextPrimaryAttack() > g_GlobalVars->curtime)
	{
		if (activeWeapon->m_Item().m_iItemDefinitionIndex() == WEAPON_REVOLVER)
		{
			cmd->buttons &= ~IN_ATTACK2;
		}
		else
		{
			cmd->buttons &= ~IN_ATTACK;
		}
	}
	else
	{
		if ((settings.triggerbot_delay > 0) && currentTime_ms - oldTimeStamp < (settings.triggerbot_delay * 10))
		{
			timeStamp = oldTimeStamp;
			return;
		}

		if (activeWeapon->m_Item().m_iItemDefinitionIndex() == WEAPON_REVOLVER)
		{
			cmd->buttons |= IN_ATTACK2;
		}
		else
		{
			cmd->buttons |= IN_ATTACK;
		}
	}

	timeStamp = currentTime_ms;
}

LegitBot g_LegitBot;
