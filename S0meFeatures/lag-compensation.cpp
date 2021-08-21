#include "../../../valve_sdk/csgostructs.hpp"
#include "../../../helpers/math.hpp"
#include "../player-log/player-log.h"
#include "lag-compensation.h"
#include "../../../options.hpp"
#include "../resolver/resolver.h"
#include "../ragebot.h"

// pLagComp by @Snake
// Credit's : Selfcode,onetapv2/v3,desync.vip,gucci framework & gladiatorz.
// Snakeware project

void LagCompensation::Instance() const {

	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive() || !g_LocalPlayer->m_hActiveWeapon()) {

		Reset();
		return;

	}

	for (auto iIndex = 1; iIndex < g_GlobalVars->maxClients; iIndex++) {

		auto Entity = static_cast<C_BasePlayer*> (g_EntityList->GetClientEntity(iIndex));

	
		if (!bShouldLagCompensate (Entity)) {
			Snakeware::pLagRecords[iIndex - 1].Reset();
			Snakeware::pLagRecords[iIndex - 1].pRecords->clear();
			continue;
		}

		// Раньше я делал по концепту gucci, но нахуя нам обновлять анимации далеко до стора самой даты..
	
		
		SetInterpolationFlags          (Entity, DISABLE_INTERPOLATION);   // Disable data interpolation's.
		UpdatePlayerRecordData         (Entity);                          // Update record data && Update player data..
		Resolver::Get().DetectFakeSide (Entity);                          // Find fake side..
		
		
		
	}

}

void LagCompensation::StoreRecordData (C_BasePlayer* pPlayer, cTickRecord* pRecordData) {
	if (!pPlayer) return;

	pRecordData->vecOrigin      = pPlayer->m_vecOrigin();
	pRecordData->vecabsOrigin   = pPlayer->m_angAbsOrigin();
	pRecordData->vecVelocity    = pPlayer->m_vecVelocity();
	pRecordData->vecObsMins     = pPlayer->m_vecMins();
	pRecordData->vecObsMaxs     = pPlayer->m_vecMaxs();
	pRecordData->m_eyeAngles    = pPlayer->m_angEyeAngles();
	pRecordData->m_absEyeAngles = pPlayer->m_angAbsAngles();
	pRecordData->iSequence      = pPlayer->m_nSequence();
	pRecordData->iEntFlags      = pPlayer->m_fFlags();
	pRecordData->flSimTime      = pPlayer->m_flSimulationTime();
	pRecordData->flLby          = pPlayer->m_flLowerBodyYawTarget();
	pRecordData->flCycle        = pPlayer->m_flCycle();
	pRecordData->flDuckAmmount  = pPlayer->m_flDuckAmount();

	pRecordData->flPoseParamaters = pPlayer->m_flPoseParameter();
	//pRecordData->flRagdollPositions = pPlayer->ragdoll();

	pRecordData->m_bDataFilled = true;
}

void LagCompensation::ApplyRecordData (C_BasePlayer* pPlayer, cTickRecord* pRecordData) {
	if (!pPlayer || !pRecordData->m_bDataFilled) return;

	pPlayer->m_vecOrigin()            = pRecordData->vecOrigin;
	pPlayer->m_angAbsOrigin()         = pRecordData->vecabsOrigin;
	pPlayer->m_vecVelocity()          = pRecordData->vecVelocity;
	pPlayer->m_vecMins()              = pRecordData->vecObsMins;
	pPlayer->m_vecMaxs()              = pRecordData->vecObsMaxs;
	pPlayer->m_angEyeAngles()         = pRecordData->m_eyeAngles;
	pPlayer->m_angAbsAngles()         = pRecordData->m_absEyeAngles;
	pPlayer->m_nSequence()            = pRecordData->iSequence;
	pPlayer->m_fFlags()               = pRecordData->iEntFlags;
	pPlayer->m_flSimulationTime()     = pRecordData->flSimTime;
	pPlayer->m_flLowerBodyYawTarget() = pRecordData->flLby;
	pPlayer->m_flCycle()              = pRecordData->flCycle;
	pPlayer->m_flDuckAmount()         = pRecordData->flDuckAmmount;

	pPlayer->m_flPoseParameter()      = pRecordData->flPoseParamaters;

	pPlayer->SetAbsOrigin               (pRecordData->vecOrigin);
}



void LagCompensation::UpdateAnimationsData(C_BasePlayer* pPlayer) {

	auto pState = pPlayer->GetPlayerAnimState();
	
	auto pIndex  = pPlayer->EntIndex() - 1;
	auto pRecord = &Snakeware::pLagRecords[pIndex];

	if (pPlayer)
		FixNetvarCompression (pPlayer);

	if (pRecord->pRecords->size() < 2) 
		return;

	AnimationLayer backupLayers[13];
	const float    backupAbsFrame  = g_GlobalVars->absoluteframetime;
	const float    backupFrametime = g_GlobalVars->frametime;
	const float    backupCurtime   = g_GlobalVars->curtime;
	const float    backupRealtime  = g_GlobalVars->realtime;
	const auto     backupAbsOrigin = pPlayer->m_angAbsOrigin();
	const auto     backupVecOrigin = pPlayer->m_vecOrigin();

	std::memcpy (backupLayers, pPlayer->GetAnimOverlays(), (sizeof(AnimationLayer) * pPlayer->GetNumAnimOverlays()));

	pPlayer->SetAbsOrigin (pPlayer->m_vecOrigin());

	g_GlobalVars->absoluteframetime    = g_GlobalVars->interval_per_tick;
	g_GlobalVars->frametime            = g_GlobalVars->interval_per_tick;
	g_GlobalVars->curtime              = pPlayer->m_flSimulationTime();
	g_GlobalVars->realtime             = pPlayer->m_flSimulationTime();

	pPlayer->m_iEFlags()       &= ~0x1000;

	if (pPlayer->m_fFlags() & FL_ONGROUND && pPlayer->m_vecVelocity().Length() > 0.0f && backupLayers[6].m_flWeight <= 0.0f)
		pPlayer->m_vecVelocity().Zero(); // Fix by LED42

	pPlayer->m_vecAbsVelocity() = pPlayer->m_vecVelocity();

	if (pState->m_iLastClientSideAnimationUpdateFramecount == g_GlobalVars->framecount)
		pState->m_iLastClientSideAnimationUpdateFramecount =  g_GlobalVars->framecount - 1;


	pPlayer->m_bClientSideAnimation() = true;
	Snakeware::UpdateAnims            = true;

 	pPlayer->UpdateClientSideAnimation();

	Snakeware::UpdateAnims            = false;
	pPlayer->m_bClientSideAnimation() = false;


	std::memcpy (pPlayer->GetAnimOverlays(), backupLayers, (sizeof(AnimationLayer) * pPlayer->GetNumAnimOverlays()));

	g_GlobalVars->absoluteframetime = backupAbsFrame;
	g_GlobalVars->frametime         = backupFrametime;
	g_GlobalVars->curtime           = backupCurtime;
	g_GlobalVars->realtime          = backupRealtime;
	pPlayer->m_vecOrigin()          = backupVecOrigin;

	

	pPlayer->ForceBoneRebuilid();
	pPlayer->SetupBonesRebulid();

}



bool LagCompensation::IsTimeDelta2Large (cTickRecord cRecord) const {
	
	// Large delta from gucci / onetap..
	// Ling : https://github.com/notphage/gucci/blob/67b375672137631669b45f25ec64758b97b4568e/csgo/c_lagcompensation.cpp#L133
	const auto Net          = g_EngineClient->GetNetChannelInfo();
	static auto sv_maxunlag = g_CVar->FindVar("sv_maxunlag");

	const auto outgoing = Net->GetLatency(FLOW_OUTGOING);
	const auto incoming = Net->GetLatency(FLOW_INCOMING);

	float Correct = 0.f;
		
	Correct += outgoing;
	Correct += incoming;
	Correct += LerpTime();

	Correct = std::clamp(Correct, 0.f, sv_maxunlag->GetFloat());

	float deltaTime = std::abs(Correct - (g_GlobalVars->curtime - cRecord.flSimTime));

	return deltaTime > 0.2f;


}

void LagCompensation::SimulationMovement    (cSimulationRecord* pData) {

	// Simulation movement (gucci yeap)
}


void LagCompensation::Extrapolate  (C_BasePlayer* pPlayer, Vector& vecOrigin, Vector& vecVelocity, int& iFlags, bool bOnGround) {
	// Pasted from desync.vip by L3D


	static const auto sv_gravity      = g_CVar->FindVar("sv_gravity");
	static const auto sv_jump_impulse = g_CVar->FindVar("sv_jump_impulse");

	if (!(iFlags & FL_ONGROUND))
		vecVelocity.z -= TICKS_TO_TIME(sv_gravity->GetFloat());
	else if (pPlayer->m_fFlags() & FL_ONGROUND && !bOnGround)
		vecVelocity.z = sv_jump_impulse->GetFloat();

	const auto src = vecOrigin;
	auto end = src + vecVelocity * g_GlobalVars->interval_per_tick;

	Ray_t r;
	r.Init(src, end, pPlayer->m_vecMins(), pPlayer->m_vecMaxs());

	trace_t t;
	CTraceFilter   filter;
	filter.pSkip = pPlayer;

	g_EngineTrace->TraceRay(r, MASK_PLAYERSOLID, &filter, &t);

	if (t.fraction != 1.f) {

		for (auto i = 0; i < 2; i++) {

			vecVelocity -= t.plane.normal * vecVelocity.Dot(t.plane.normal);

			const auto dot = vecVelocity.Dot(t.plane.normal);
			if (dot < 0.f)
				vecVelocity -= Vector(dot * t.plane.normal.x,
					dot     * t.plane.normal.y, dot * t.plane.normal.z);

			end = t.endpos + vecVelocity * TICKS_TO_TIME(1.f - t.fraction);

			r.Init(t.endpos, end, pPlayer->m_vecMins(), pPlayer->m_vecMaxs());
			g_EngineTrace->TraceRay(r, MASK_PLAYERSOLID, &filter, &t);

			if (t.fraction == 1.f)
				break;
		}

	}

	vecOrigin = end = t.endpos;
	end.z    -= 2.f;

	r.Init(vecOrigin, end, pPlayer->m_vecMins(), pPlayer->m_vecMaxs());
	g_EngineTrace->TraceRay(r, MASK_PLAYERSOLID, &filter, &t);

	iFlags &= FL_ONGROUND;

	if (t.DidHit() && t.plane.normal.z > .7f)
		iFlags |= FL_ONGROUND;
}

void LagCompensation::RebulidCrouchAnimation (C_BasePlayer * pPlayer) {

	// OneTap 17 july.
	// From onetap with love.
	// But i don't know about use, maybe for local player xD

	auto BackupPose   = pPlayer->m_flPoseParameter ();
	auto BackupCycle  = pPlayer->GetAnimOverlays()[3].m_flCycle;
	auto BackupWeight = pPlayer->GetAnimOverlays()[3].m_flCycle;

	if (pPlayer->m_fFlags () & FL_ONGROUND && pPlayer->m_vecVelocity().Length2D () < 0.15f) {


		if (pPlayer->GetAnimOverlays()[3].m_nSequence == 979) {

			pPlayer->GetAnimOverlays()[3].m_flCycle   = 0.0f;
			pPlayer->GetAnimOverlays()[3].m_flWeight  = 0.0f;

		}


	}

	pPlayer->m_flPoseParameter()             = BackupPose;
	pPlayer->GetAnimOverlays()[3].m_flCycle  = BackupCycle;
	pPlayer->GetAnimOverlays()[3].m_flWeight = BackupWeight;
	// @sharklaser1 you are crazy..

}

void LagCompensation::PredictPlayer (C_BasePlayer* pPlayer, cTickRecord* curRecord, cTickRecord* nextRecord) const {

	if (!bShouldLagCompensate (pPlayer)) return;

	// Create Simulation Data
	cSimulationRecord SimData;

	SimData.pPlayer       = pPlayer;
	SimData.vecOrigin     = curRecord->vecOrigin;
	SimData.vecVelocity   = curRecord->vecVelocity;
	SimData.bOnGround     = curRecord->iEntFlags & FL_ONGROUND;
	SimData.m_bDataFilled = true;

	// Calculate Delta's without lerp..
	// Gucci hack..
	auto flSimTimeDelta  = curRecord->flSimTime - nextRecord->flSimTime;
	auto iSimTicksDelta  = std::clamp(TIME_TO_TICKS(flSimTimeDelta), 1, 15);
	auto iDeltaTicks     = (std::clamp(TIME_TO_TICKS(g_EngineClient->GetNetChannelInfo()->GetAvgLatency(1) + g_EngineClient->GetNetChannelInfo()->GetAvgLatency(0)) + g_GlobalVars->tickcount - TIME_TO_TICKS(curRecord->flSimTime + LerpTime()), 0, 100)) - iSimTicksDelta;

	// Calculate movement delta
	auto CurVelocityAngle   = Math::Rad2Deg (atan2(curRecord->vecVelocity.y, curRecord->vecVelocity.x));
	auto NextVelocityAngle  = Math::Rad2Deg (atan2(nextRecord->vecVelocity.y, nextRecord->vecVelocity.x));
	auto VelocityAngleDelta = Math::NormalizeYaw(CurVelocityAngle - NextVelocityAngle); // wtf bro
	auto VelocityMoveDelta  = VelocityAngleDelta / flSimTimeDelta;

	bool bInAir = false;

	if (!(pPlayer->m_fFlags() & FL_ONGROUND) || !(curRecord->iEntFlags & FL_ONGROUND))
		bInAir = true;

	if (iDeltaTicks > 0 && SimData.m_bDataFilled) {
		for (; iDeltaTicks >= 0; iDeltaTicks -= iSimTicksDelta) 	{
			auto SkipTicks = iSimTicksDelta;

			do {
				Extrapolate(pPlayer, SimData.vecOrigin, SimData.vecVelocity, pPlayer->m_fFlags(), bInAir);
				curRecord->flSimTime += g_GlobalVars->interval_per_tick;

				--SkipTicks;
			} while (SkipTicks);
		}

		curRecord->vecOrigin       = SimData.vecOrigin;
		curRecord->vecabsOrigin    = SimData.vecOrigin;
	}
}


int LagCompensation::StartLagCompensation (C_BasePlayer* pPlayer, int iTick, cTickRecord* outRecord) const {
	if (!bShouldLagCompensate(pPlayer))                                       return -1;

	auto pIndex        = pPlayer->EntIndex() - 1;
	auto pRecord       = Snakeware::pLagRecords [pIndex];

	if (pRecord.pRecords->empty() || iTick + 1 > pRecord.pRecords->size() - 1) return -1;

	auto curRecord     = pRecord.pRecords->at (iTick);
	auto nextRecord    = pRecord.pRecords->at (iTick + 1);

	if (!curRecord.m_bDataFilled || !nextRecord.m_bDataFilled || iTick > 0 && IsTimeDelta2Large(curRecord))
		return -1;

	if (iTick == 0 && (curRecord.vecOrigin - nextRecord.vecOrigin).LengthSqr() > 4096.f) {
		PredictPlayer              (pPlayer, &curRecord, &nextRecord);
		ApplyRecordData            (pPlayer, &curRecord);
		pPlayer->ForceBoneRebuilid ();
		pPlayer->SetupBonesRebulid ();
		StoreRecordData            (pPlayer, &curRecord);
	}
	// Some rebulides

	if  (outRecord != nullptr && curRecord.m_bDataFilled)
		*outRecord = curRecord;

	return TIME_TO_TICKS(curRecord.flSimTime + LerpTime());
}

void LagCompensation::StartPositionAdjustment () const {
	

	for (auto Index = 1; Index < g_GlobalVars->maxClients; Index++) {
		auto Entity = static_cast<C_BasePlayer*> (g_EntityList->GetClientEntity(Index));

		if (!bShouldLagCompensate(Entity))
			continue;

		auto pRecord = &Snakeware::pLagRecords[Entity->EntIndex() - 1];

		if (pRecord->pRecords->size() <= 2)
			return;

		pRecord->bLagCompensated = true;
		StartPositionAdjustment   (Entity);
		pRecord->bLagCompensated = false;
	}
}

void LagCompensation::FixDormantAnimation(C_BasePlayer* pPlayer) {

	// By @Snake
	auto pState = pPlayer->GetPlayerAnimState();

	if (pPlayer && pPlayer->IsDormant()) {

		if (pPlayer->m_fFlags() & FL_ONGROUND) {
			pState->m_bOnGround             = true;
			pState->m_bInHitGroundAnimation = false;
		}

		pState->m_flTimeSinceInAir()  = 0.0f;
		pState->m_flGoalFeetYaw       = Math::NormalizeYaw(pPlayer->m_angEyeAngles().yaw);
	}

}


void LagCompensation::StartPositionAdjustment (C_BasePlayer* pPlayer) const {

	if (!bShouldLagCompensate(pPlayer))
		return;

	auto TestAndApplyRecord = [](C_BasePlayer* Player, cPlayerRecord* pRecord, cTickRecord* RestoreRecord, cTickRecord* CorrectedRecord, int iTickCount) -> bool 	{
		ApplyRecordData (Player, CorrectedRecord);

		pRecord->TickCount       = iTickCount;
		pRecord->m_eyeAngles     = CorrectedRecord->m_eyeAngles;
		pRecord->HitboxPositions = CorrectedRecord->vecHitboxPos;

		ApplyRecordData(Player, RestoreRecord);
		return true;
	};

	auto pIndex        = pPlayer->EntIndex() - 1;
	auto pRecord       = &Snakeware::pLagRecords[pIndex];
	auto pWeapon       = g_LocalPlayer->m_hActiveWeapon();

	if (pRecord->pRecords->empty())
		return;

	pRecord->TickCount = -1;
	pRecord->HitboxPositions.Clear();
	pRecord->m_eyeAngles.Clear    ();


	StoreRecordData(pPlayer, &pRecord->RestoreRecord);

	
	// Just normally loop as we would
	for (auto index = 0; index < pRecord->pRecords->size(); index++) {
		if (index + 1 > pRecord->pRecords->size() - 1)
			break;

		cTickRecord corrected_record;
		auto curRecord = &pRecord->pRecords->at(index);

		auto tick_count = StartLagCompensation(pPlayer, index, &corrected_record);

		if (tick_count == -1 || !corrected_record.m_bDataFilled)
			break;

		if (TestAndApplyRecord(pPlayer, pRecord, &pRecord->RestoreRecord, &corrected_record, tick_count))
			return;
	}
}


void LagCompensation::FinishPositionAdjustment () const {
	
	for (auto Index = 1; Index < g_GlobalVars->maxClients; Index++) {
		auto Entity = static_cast<C_BasePlayer*> (g_EntityList->GetClientEntity(Index));

		if (!bShouldLagCompensate(Entity))
			continue;

		FinishPositionAdjustment (Entity);
	}
}


void LagCompensation::FinishPositionAdjustment (C_BasePlayer* pPlayer) {
	if (!bShouldLagCompensate(pPlayer))
		return;


	auto pIndex = pPlayer->EntIndex() - 1;
	auto pRecord = &Snakeware::pLagRecords[pIndex];

	if (!pRecord->RestoreRecord.m_bDataFilled)
		return;

	ApplyRecordData(pPlayer, &pRecord->RestoreRecord);

	pRecord->RestoreRecord.m_bDataFilled = false; // Set to false so that we dont apply this record again if its not set next time
}


void LagCompensation::SetInterpolationFlags (C_BasePlayer* pPlayer, int iFlag) {
	auto varMap          = reinterpret_cast<uintptr_t>(pPlayer) + 36; // tf2 = 20
	auto varMapListCount = *reinterpret_cast<int*>    (varMap + 20);

	for (auto Index = 0; Index < varMapListCount; Index++)
		*reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(varMap) + Index * 12) = iFlag;
}

void LagCompensation::Reset () {

	for (auto i = 0; i < 64; i++)
		Snakeware::pLagRecords[i].Reset();
}


void LagCompensation::UpdatePlayerRecordData (C_BasePlayer* pPlayer) const {
	
	if (!pPlayer) return;

	auto pIndex   = pPlayer->EntIndex() - 1;
	auto pRecord  = &Snakeware::pLagRecords[pIndex];

	if (pPlayer->m_flSimulationTime() > 0.f && (pRecord->pRecords->empty() || 
		pRecord->pRecords->size()     > 0   && pRecord->pRecords->at(0).flSimTime != pPlayer->m_flSimulationTime())) {


		UpdateAnimationsData (pPlayer);

		cTickRecord newRecord;

		StoreRecordData (pPlayer, &newRecord);

		
		if (abs(pRecord->flLby - pPlayer->m_flLowerBodyYawTarget()) != 0.f) {
			// From gucci
			pRecord->flLby           = pPlayer->m_flLowerBodyYawTarget();
			pRecord->flLbyLastUpdate = pPlayer->m_flSimulationTime();
		}

		if (newRecord.m_bDataFilled) {
			pRecord->pRecords->push_front(newRecord);

			while (pRecord->pRecords->size() > LAG_COMPENSATION_TICKS)
				pRecord->pRecords->pop_back();
		}

		// Old removed, new added..
		FixDormantAnimation (pPlayer);

		// SetAbsAngles too...
		pPlayer->SetAbsOrigin  (newRecord.vecOrigin);

		if (pPlayer->m_fFlags() & FL_ONGROUND)
		pPlayer->m_angEyeAngles().roll = 0.f;

		// Safepoint Part
		float flAngle  = newRecord.m_eyeAngles.yaw;
		auto  AbsAngle = newRecord.m_absEyeAngles;
		// Null matix
		
		const auto flPoses = pPlayer->m_flPoseParameter();
		const auto angEyeAngles = pPlayer->m_angEyeAngles();
		const auto vecVelocity = pPlayer->m_vecVelocity();
		const auto vecOrigin = pPlayer->m_vecOrigin();
		const auto flDuckAmount = pPlayer->m_flDuckAmount();
		const auto flSimtime = pPlayer->m_flSimulationTime();
		const auto fFlags = pPlayer->m_fFlags();


		Resolver::Get().PreverseSafePoint (pPlayer, 0, newRecord.flSimTime);

		
		pPlayer->ForceBoneRebuilid();
		pPlayer->m_vecVelocity()      = vecVelocity;
		pPlayer->m_vecOrigin()        = vecOrigin;
		pPlayer->m_flDuckAmount()     = flDuckAmount;
		pPlayer->m_flSimulationTime() = flSimtime;
		pPlayer->m_angEyeAngles()     = angEyeAngles;
		pPlayer->m_fFlags()           = fFlags;
		pPlayer->m_vecAbsVelocity()   = vecVelocity;

		Resolver::Get().PreverseSafePoint(pPlayer, -1, newRecord.flSimTime);

		pPlayer->ForceBoneRebuilid ();
		pPlayer->m_vecVelocity()      = vecVelocity;
		pPlayer->m_vecOrigin()        = vecOrigin;
		pPlayer->m_flDuckAmount()     = flDuckAmount;
		pPlayer->m_flSimulationTime() = flSimtime;
		pPlayer->m_angEyeAngles()     = angEyeAngles;
		pPlayer->m_fFlags()           = fFlags;
		pPlayer->m_vecAbsVelocity()   = vecVelocity;

		Resolver::Get().PreverseSafePoint(pPlayer, 1, newRecord.flSimTime);

		pPlayer->ForceBoneRebuilid();
		pPlayer->m_vecVelocity()      = vecVelocity;
		pPlayer->m_vecOrigin()        = vecOrigin;
		pPlayer->m_flDuckAmount()     = flDuckAmount;
		pPlayer->m_flSimulationTime() = flSimtime;
		pPlayer->m_angEyeAngles()     = angEyeAngles;
		pPlayer->m_fFlags()           = fFlags;
		pPlayer->m_vecAbsVelocity()   = vecVelocity;

		

	}
}

float LagCompensation::LerpTime ()   {

	static ConVar* updaterate = g_CVar->FindVar("cl_updaterate");
	static ConVar* minupdate  = g_CVar->FindVar("sv_minupdaterate");
	static ConVar* maxupdate  = g_CVar->FindVar("sv_maxupdaterate");
	static ConVar* lerp       = g_CVar->FindVar("cl_interp");
	static ConVar* cmin       = g_CVar->FindVar("sv_client_min_interp_ratio");
	static ConVar* cmax       = g_CVar->FindVar("sv_client_max_interp_ratio");
	static ConVar* ratio      = g_CVar->FindVar("cl_interp_ratio");

	float lerpurmom = lerp->GetFloat(), maxupdateurmom = maxupdate->GetFloat(),
		ratiourmom  = ratio->GetFloat(), cminurmom    = cmin->GetFloat(), cmaxurmom = cmax->GetFloat();

	int updaterateurmom  = updaterate->GetInt(),
		sv_maxupdaterate = maxupdate->GetInt(), sv_minupdaterate = minupdate->GetInt();

	if (sv_maxupdaterate && sv_minupdaterate)
		updaterateurmom  = maxupdateurmom;

	if (ratiourmom == 0)
		ratiourmom = 1.0f;

	if (cmin && cmax && cmin->GetFloat() != 1)
		ratiourmom = std::clamp(ratiourmom, cminurmom, cmaxurmom);

	return std::max(lerpurmom, ratiourmom / updaterateurmom);
}




bool LagCompensation::bShouldLagCompensate (C_BasePlayer *pPlayer)  {

	if (!g_LocalPlayer /*g_Options.ragebot_enabled*/)
		return false;

	if (!pPlayer)
		return false;

	if (!pPlayer->IsPlayer())
		return false;

	if (pPlayer == g_LocalPlayer)
		return false;

	if (pPlayer->IsDormant())
		return false;

	if (!pPlayer->IsAlive())
		return false;

	if (pPlayer->IsTeam())
		return false;

	if (pPlayer->m_bGunGameImmunity())
		return false;

	return true;
}


void LagCompensation::FixNetvarCompression (C_BasePlayer* pPlayer) {
	// Repasted from eexomi hack.
	// by 5n4k3
	static auto sv_gravity       = g_CVar->FindVar("sv_gravity");
	auto& HistoryRecord          = Snakeware::pLagRecords[pPlayer->EntIndex() - 1].pRecords;

	if (HistoryRecord->size() < 2 || pPlayer == nullptr || sv_gravity == nullptr)
		return;

	auto& Record     = HistoryRecord->front();
	auto& preRecord  = HistoryRecord->at(1);

	

	float chokedTime = pPlayer->m_flSimulationTime() - Record.flSimTime;
	chokedTime       = Math::Clamp(chokedTime, g_GlobalVars->interval_per_tick, 1.0f);

	Vector vecOrigin   = pPlayer->m_vecOrigin();
	Vector deltaOrigin = (vecOrigin - Record.vecOrigin) * (1.0f / chokedTime);

	float penultimateChoke = Record.flSimTime - preRecord.flSimTime;
	penultimateChoke       = Math::Clamp(penultimateChoke, g_GlobalVars->interval_per_tick, 1.0f);

	float Delta = RAD2DEG(atan2((Record.vecOrigin.y - preRecord.vecOrigin.y) * (1.0f / penultimateChoke),
		(Record.vecOrigin.x - preRecord.vecOrigin.x) * (1.0f / penultimateChoke)));

	float Direction = RAD2DEG(atan2(deltaOrigin.y, deltaOrigin.x));
	float deltaDirection = Math::NormalizeYaw(Direction - Delta);

	deltaDirection = DEG2RAD(deltaDirection * 0.5f + Direction);

	float dirCos = cos(deltaDirection), dirSin = sin(deltaDirection);

	float move = deltaOrigin.Length2D();
	Vector velocity;
	pPlayer->m_vecVelocity().x = dirCos * move;
	pPlayer->m_vecVelocity().y = dirSin * move;
	pPlayer->m_vecVelocity().z = deltaOrigin.z;

	if (!(pPlayer->m_fFlags() & FL_ONGROUND)) {
		pPlayer->m_vecVelocity().z -= sv_gravity->GetFloat() * chokedTime * 0.5f;
	}
}


void LagCompensation::UpdateAnimationsInfo (C_BasePlayer* pPlayer, cTickRecord* fFrom) {


	auto ResolveInfo = &Resolver::Get().ResolveRecord[pPlayer->EntIndex() - 1];

	pPlayer->SetAbsOrigin (pPlayer->m_vecOrigin());

	auto iLag = fFrom == nullptr ? 1 : TICKS_TO_TIME(pPlayer->m_flSimulationTime() - fFrom->flSimTime);
	    iLag  = Math::Clamp(iLag, 1, 17);

		if (fFrom) {

			pPlayer->m_angEyeAngles().pitch = std::clamp(Math::NormalizePitch(pPlayer->m_angEyeAngles().pitch), -89.f, 89.f);

		}


		if (fFrom == nullptr ||  (iLag - 1) <= 1) {

			pPlayer->GetPlayerAnimState()->m_flFeetYawRate = 0.f;
			pPlayer->UpdateClientSideAnimation ();
			
			return;
		}

		for (auto i = 0; i < iLag; i++) {
			const auto flTime = fFrom->flSimTime + TICKS_TO_TIME(i + 1);
			const auto flLerp = 1.f - (pPlayer->m_flSimulationTime() - flTime) / (pPlayer->m_flSimulationTime() - fFrom->flSimTime);
			const auto backupCurtime   = g_GlobalVars->curtime;
			const auto backupRealtime  = g_GlobalVars->realtime;
			const auto backupFrametime = g_GlobalVars->frametime;


			pPlayer->m_flDuckAmount() = Math::Interpolate(fFrom->flDuckAmmount, pPlayer->m_flDuckAmount(), flLerp);


			auto flEyeAngles              = Math::Interpolate(fFrom->m_eyeAngles, pPlayer->m_angEyeAngles(), flLerp);
			flEyeAngles.yaw               = Math::NormalizeYaw(flEyeAngles.yaw);
			pPlayer->m_angEyeAngles().yaw = flEyeAngles.yaw;

			// fix feet spin.
			pPlayer->GetPlayerAnimState()->m_flFeetCycle    =  fFrom->AnimLayers[6].m_flCycle;
			pPlayer->GetPlayerAnimState()->m_flFeetYawRate  =  fFrom->AnimLayers[6].m_flWeight;

			g_GlobalVars->realtime  = flTime;
			g_GlobalVars->curtime   = flTime;
			g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;

			pPlayer->UpdateClientSideAnimation();

			g_GlobalVars->realtime = backupRealtime;
			g_GlobalVars->curtime  = backupCurtime;
			g_GlobalVars->frametime = backupFrametime;


		}

}