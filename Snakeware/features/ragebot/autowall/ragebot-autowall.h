#pragma once

#include "../../../valve_sdk/csgostructs.hpp"
#include "../../../valve_sdk/sdk.hpp"
#include "../../../singleton.hpp"

struct AutoWallInfo {
	int iDamage           = -1;
	int iHitgroup         = -1;
	int iWalls            = 4;
	bool bDidPenerate     = false;
	float flTickness      = 1.f;
	C_BasePlayer* pPlayer = 0;
	Vector End;
};

struct AutoWallBullet {
	Vector vecStart;
	Vector vecEnd;
	Vector vecPos;
	Vector vecDir;

	ITraceFilter* Filter = nullptr;
	trace_t       Trace;

	float  flThickness = 1.f;
	float  flDamage = 1.f;
	int    iWalls = 4;
};


class AutoWall : public Singleton < AutoWall > {
public :
	AutoWallInfo Start (Vector start, Vector end, C_BasePlayer* from, C_BasePlayer* to);
	float GetPointDamage(Vector point, C_BasePlayer * e);
private :
	void ScaleDamage               (trace_t* trace, CCSWeaponInfo* inf, int& hitgroup, float& damage);
	void ClipTrace2Player          (Vector start,  Vector end, C_BasePlayer* player, unsigned int mask, ITraceFilter* filter, trace_t* trace);
	bool HandleBulletPenetration   (CCSWeaponInfo* inf, AutoWallBullet& bullet);
	bool Trace2Exit                (trace_t* enter_trace, Vector& start, Vector& dir, trace_t* exit_trace);
	bool IsBreakable               (C_BasePlayer* player);

	

};