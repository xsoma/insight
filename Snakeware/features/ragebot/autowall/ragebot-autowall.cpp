#include "ragebot-autowall.h"
#include "../../../helpers/math.hpp"
#include "../../../valve_sdk/math/Vector.hpp"
#include "../../../options.hpp"
#include "../../../Protected/enginer.h"

// Supreamcy autowall + skeet.idn & otv2 code
// Credit's : @Snake



#define CHAR_TEX_ANTLION		'A'
#define CHAR_TEX_BLOODYFLESH	'B'
#define CHAR_TEX_CONCRETE		'C'
#define CHAR_TEX_DIRT			'D'
#define CHAR_TEX_EGGSHELL		'E'
#define CHAR_TEX_FLESH			'F'
#define CHAR_TEX_GRATE			'G'
#define CHAR_TEX_ALIENFLESH		'H'
#define CHAR_TEX_CLIP			'I'
#define CHAR_TEX_PLASTIC		'L'
#define CHAR_TEX_METAL			'M'
#define CHAR_TEX_SAND			'N'
#define CHAR_TEX_FOLIAGE		'O'
#define CHAR_TEX_COMPUTER		'P'
#define CHAR_TEX_SLOSH			'S'
#define CHAR_TEX_TILE			'T'
#define CHAR_TEX_CARDBOARD		'U'
#define CHAR_TEX_VENT			'V'
#define CHAR_TEX_WOOD			'W'
#define CHAR_TEX_GLASS			'Y'
#define CHAR_TEX_WARPSHIELD		'Z'


	

void AutoWall::ScaleDamage  (trace_t * Trace, CCSWeaponInfo * Inf, int & iHitgroup, float & flDamage) {

	auto weapon      = g_LocalPlayer->m_hActiveWeapon();
	bool has_heavy   = false;
	int armor_value  = ((C_BasePlayer*)Trace->hit_entity)->m_ArmorValue();
	int hit_group    = Trace->hitgroup;

	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	if (weapon->IsZeus() || weapon->IsKnife() || weapon->IsGrenade()) 
		return;

	auto is_armored = [&Trace]()->bool
	{
		auto* target = (C_BasePlayer*)Trace->hit_entity;
		switch (Trace->hitgroup)
		{
		case HITGROUP_HEAD:
			return !!target->m_bHasHelmet();
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			return true;
		default:
			return false;
		}
	};

	switch (hit_group)
	{
	case HITGROUP_HEAD:
		flDamage *= 2.f;
		break;
	case HITGROUP_CHEST:
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:

		flDamage *= 1.f;
		break;
	case HITGROUP_STOMACH:
		flDamage *= 1.25f;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		flDamage *= 0.75f;
		break;
	default:
		break;
	}

	if (armor_value > 0 && is_armored()) {
		float bonus_value = 1.f, armor_bonus_ratio = 0.5f, armor_ratio = Inf->flArmorRatio / 2.f;

		if (has_heavy)
		{
			armor_bonus_ratio = 0.33f;
			armor_ratio *= 0.5f;
			bonus_value = 0.33f;
		}

		auto new_damage = flDamage * armor_ratio;

		if (((flDamage - (flDamage * armor_ratio)) * (bonus_value * armor_bonus_ratio)) > armor_value)
		{
			new_damage = flDamage - (armor_value / armor_bonus_ratio);
		}

		flDamage = new_damage;
	}


}

void AutoWall::ClipTrace2Player (Vector start, Vector end, C_BasePlayer * player, unsigned int mask, ITraceFilter * filter, trace_t * trace) {

	if (!player)
		return;

	auto mins = player->GetCollideable()->OBBMins();
	auto maxs = player->GetCollideable()->OBBMaxs();

	Vector dir(end - start);
	Vector center = (maxs + mins) / 2, pos(center + player->m_angAbsOrigin());

	auto to = pos - start;
	auto range_along = dir.Dot(to);

	float range;
	if (range_along < 0.f)
		range = -to.Length();
	else if (range_along > dir.Length())
		range = -(pos - end).Length();
	else {
		auto ray(pos - (dir * range_along + start));
		range = ray.Length();
	}

	if (range <= 60.f) {
		trace_t newtrace;

		Ray_t ray;
		ray.Init(start, end);

		g_EngineTrace->ClipRayToCollideable(ray, mask, player->GetCollideable(), &newtrace);

		if (trace->fraction > newtrace.fraction)
			*trace = newtrace;
	}


}


bool AutoWall::HandleBulletPenetration(CCSWeaponInfo* Inf, AutoWallBullet& Bullet) {
	trace_t trace;

	auto sdata = g_PhysSurface->GetSurfaceData(Bullet.Trace.surface.surfaceProps);
	auto mat   = sdata->game.material;

	auto sp_mod = sdata->game.flPenetrationModifier; //*(float*)((uintptr_t)sdata + 88);
	auto dmg_mod = sdata->game.flDamageModifier;
	auto pnt_mod = 0.f;

	auto solid_surf = (Bullet.Trace.contents >> 3) & CONTENTS_SOLID;
	auto light_surf = (Bullet.Trace.surface.flags >> 7) & SURF_LIGHT;

	if (Bullet.iWalls <= 0
		|| (!Bullet.iWalls && !light_surf && !solid_surf && mat != CHAR_TEX_GLASS && mat != CHAR_TEX_GRATE)
		|| Inf->flPenetration <= 0.f
		|| (!this->Trace2Exit(&Bullet.Trace, Bullet.Trace.endpos, Bullet.vecDir, &trace)
			&& !(g_EngineTrace->GetPointContents(Bullet.Trace.endpos, MASK_SHOT_HULL | CONTENTS_HITBOX, NULL) & (MASK_SHOT_HULL | CONTENTS_HITBOX))))
		return false;

	auto e_sdata  = g_PhysSurface->GetSurfaceData(trace.surface.surfaceProps);
	auto e_mat   = e_sdata->game.material;
	auto e_sp_mod = e_sdata->game.flPenetrationModifier;

	if (mat == CHAR_TEX_GRATE || mat == CHAR_TEX_GLASS)
	{
		pnt_mod = 3.f;
		dmg_mod = 0.05f;
	}
	else if (light_surf || solid_surf)
	{
		pnt_mod = 1.f;
		dmg_mod = 0.16f;
	}
	else if (mat == CHAR_TEX_FLESH)
	{
		pnt_mod = 1.f;
		dmg_mod = 0.16f;
	}
	else
	{
		pnt_mod = (sp_mod + e_sp_mod) / 2.f;
		dmg_mod = 0.16f;
	}

	if (mat == e_mat)
	{
		if (e_mat == CHAR_TEX_CARDBOARD || e_mat == CHAR_TEX_WOOD)
			pnt_mod = 3.f;
		else if (e_mat == CHAR_TEX_PLASTIC)
			pnt_mod = 2.f;
	}

	auto thickness = (trace.endpos - Bullet.Trace.endpos).LengthSqr();
	auto modifier = fmaxf(0.f, 1.f / pnt_mod);

	auto lost_damage = fmaxf(
		((modifier * thickness) / 24.f)
		+ ((Bullet.flDamage * dmg_mod)
			+ (fmaxf(3.75f / Inf->flPenetration, 0.f) * 3.f * modifier)), 0.f);

	if (lost_damage > Bullet.flDamage)
		return false;

	if (lost_damage > 0.f)
		Bullet.flDamage -= lost_damage;

	if (Bullet.flDamage < 1.f)
		return false;

	Bullet.vecPos = trace.endpos;
	Bullet.iWalls--;

	return true;
}

bool AutoWall::Trace2Exit(trace_t * enter_trace, Vector & start, Vector & dir, trace_t * exit_trace) {
	Vector end;
	auto distance = 0.f;
	auto distance_check = 23;
	auto first_contents = 0;

	do {
		distance += 4.f;
		end = start + dir * distance;

		if (!first_contents)
			first_contents = g_EngineTrace->GetPointContents(end, MASK_SHOT | CONTENTS_GRATE, NULL);

		auto point_contents = g_EngineTrace->GetPointContents(end, MASK_SHOT | CONTENTS_GRATE, NULL);
		if (!(point_contents & (MASK_SHOT_HULL /*| CONTENTS_HITBOX*/)) || (point_contents & CONTENTS_HITBOX && point_contents != first_contents)) {
			auto new_end = end - (dir * 4.f);

			Ray_t ray;
			ray.Init(end, new_end);

			g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, nullptr, exit_trace);

			if (exit_trace->startsolid && exit_trace->surface.flags & SURF_HITBOX) {
				Ray_t ray1;
				ray1.Init(start, end);

				CTraceFilter filter;
				filter.pSkip = exit_trace->hit_entity;

				g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, exit_trace);

				if (exit_trace->DidHit() && !exit_trace->startsolid)
					return true;

				continue;
			}

			if (exit_trace->DidHit() && !exit_trace->startsolid) {
				if (enter_trace->surface.flags & SURF_NODRAW || !(exit_trace->surface.flags & SURF_NODRAW)) {
					if (exit_trace->plane.normal.Dot(dir) <= 1.f)
						return true;

					continue;
				}

				if (this->IsBreakable((C_BasePlayer*)enter_trace->hit_entity) && this->IsBreakable((C_BasePlayer*)exit_trace->hit_entity))
					return true;

				continue;
			}

			if (exit_trace->surface.flags & SURF_NODRAW) {
				if (this->IsBreakable((C_BasePlayer*)enter_trace->hit_entity) && this->IsBreakable((C_BasePlayer*)exit_trace->hit_entity))
					return true;
				else if (!(enter_trace->surface.flags & SURF_NODRAW))
					continue;
			}

			if ((!enter_trace->hit_entity || enter_trace->hit_entity->EntIndex() == 0) && this->IsBreakable((C_BasePlayer*)enter_trace->hit_entity)) {
				exit_trace = enter_trace;
				exit_trace->endpos = start + dir;
				return true;
			}

			continue;
		}

		distance_check--;
	} while (distance_check);

	return false;
}


AutoWallInfo AutoWall::Start (Vector start, Vector end, C_BasePlayer *from, C_BasePlayer *to) {
	AutoWallInfo rt{};

	AutoWallBullet bullet;
	bullet.vecStart = start;
	bullet.vecEnd = end;
	bullet.vecPos = start;
	bullet.flThickness = 0.f;
	bullet.iWalls = 4;
	bullet.vecDir = Math::CalcAngle(start, end).vector();

	auto flt_player = CTraceFilterOneEntity();
	flt_player.pEntity = to;

	auto flt_self = CTraceFilter();
	flt_self.pSkip = from;

	if (to)
		bullet.Filter = &flt_player;
	else
		bullet.Filter = &flt_self;

	auto wep = g_LocalPlayer->m_hActiveWeapon();
	if (!wep)
		return rt;

	auto inf = wep->GetCSWeaponData();
	if (!inf)
		return rt;

	end = start + bullet.vecDir * (inf->iWeaponType == WEAPONTYPE_KNIFE ? 45.f : inf->flRange);
	bullet.flDamage = inf->iDamage;

	while (bullet.flDamage > 0 && bullet.iWalls > 0) {
		rt.iWalls = bullet.iWalls;

		Ray_t ray;
		ray.Init(bullet.vecPos, end);

		CTraceFilter filter;
		filter.pSkip = from;

		g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &bullet.Trace);
		ClipTrace2Player(bullet.vecPos, bullet.vecPos + bullet.vecDir * 40.f, to, MASK_SHOT | CONTENTS_GRATE, bullet.Filter, &bullet.Trace);

		bullet.flDamage *= powf(inf->flRangeModifier, (bullet.Trace.endpos - start).Length() / 500.f);

		if (bullet.Trace.fraction == 1.f) {
			rt.iDamage = bullet.flDamage;
			rt.iHitgroup = -1;
			rt.End = bullet.Trace.endpos;
			rt.pPlayer = nullptr;
		}

		if (bullet.Trace.hitgroup > 0 && bullet.Trace.hitgroup <= 7) {
			if ((to && bullet.Trace.hit_entity->EntIndex() != to->EntIndex()) ||
				((C_BasePlayer*)bullet.Trace.hit_entity)->m_iTeamNum() == from->m_iTeamNum()) {
				rt.iDamage = -1;

				return rt;
			}

			ScaleDamage(&bullet.Trace, inf, bullet.Trace.hitgroup, bullet.flDamage);

			rt.iDamage = bullet.flDamage;
			rt.iHitgroup = bullet.Trace.hitgroup;
			rt.End = bullet.Trace.endpos;
			rt.pPlayer = (C_BasePlayer*)bullet.Trace.hit_entity;

			break;
		}

		if (!this->HandleBulletPenetration(inf, bullet))
			break;

		rt.bDidPenerate = true;
	}

	rt.iWalls = bullet.iWalls;
	return rt;
}

bool AutoWall::IsBreakable(C_BasePlayer* e) {
	using func = bool(__fastcall*)(C_BasePlayer*);
	static auto fn = reinterpret_cast<func>(Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 51 56 8B F1 85 F6 74 68 83 BE"));

	if (!e || !e->EntIndex())
		return false;

	auto take_damage{ (char*)((uintptr_t)e + *(size_t*)((uintptr_t)fn + 38)) };
	auto take_damage_backup{ *take_damage };

	auto cclass = g_CHLClient->GetAllClasses();

	if ((cclass->m_pNetworkName[1]) != 'F'
		|| (cclass->m_pNetworkName[4]) != 'c'
		|| (cclass->m_pNetworkName[5]) != 'B'
		|| (cclass->m_pNetworkName[9]) != 'h')
		* take_damage = 2;

	auto breakable = fn(e);
	*take_damage = take_damage_backup;

	return breakable;
}

float AutoWall::GetPointDamage(Vector point, C_BasePlayer* e) {
	return this->Start(g_LocalPlayer->GetEyePos(), point, g_LocalPlayer, e).iDamage;
}