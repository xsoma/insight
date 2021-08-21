#include "../../../valve_sdk/csgostructs.hpp"
#include "../../../helpers/math.hpp"
#include "../player-log/player-log.h"
#include "lag-compensation.h"
#include "../../../options.hpp"

#include "../ragebot.h"
#include <xmemory>
// pLagComp by @LNK1181

PlayerRecord::PlayerRecord (C_BasePlayer* pPlayer, PlayerRecord* Previous, bool Dormant) {
	bDormant = Dormant;

	flSimTime = pPlayer->m_flSimulationTime();
    iSimTicks = TIME_TO_TICKS(pPlayer->m_flSimulationTime() - pPlayer->m_flOldSimulationTime());

	if (!Previous
		|| Dormant
		|| iSimTicks <= 1) {
		   iTotalCommands = 1;
		   flAnimTime = flSimTime;
	}
	else {
		iTotalCommands = iSimTicks;
		flAnimTime     = flSimTime - TICKS_TO_TIME(iSimTicks - 1);
	}

	vecOrigin   = pPlayer->m_vecOrigin();
	vecVelocity = Previous ? (vecOrigin - Previous->vecOrigin) / TICKS_TO_TIME(iTotalCommands) : pPlayer->m_vecVelocity();

	mFlags       = pPlayer->m_fGayFlags();
	eyeAngles    = pPlayer->m_angEyeAngles();
	ServerLayers = pPlayer->GetGayLayers();

	const auto weapon = pPlayer->m_hActiveWeapon();

	if (mFlags.Has(FL_ONGROUND)
		&& iTotalCommands >= 2) {
		const auto average_speed = vecVelocity.Length2D();
		if (average_speed > 0.1f) {
			const auto alive_loop_weight = ServerLayers.at(11).m_flWeight;
			if (alive_loop_weight > 0.f
				&& alive_loop_weight < 1.f) {
				const auto unk = (1.f - alive_loop_weight) * 0.35f;
				if (unk > 0.f
					&& unk < 1.f) {
					const auto max_speed = weapon ? weapon->GetMaxSpeed() : 250.f;
					const auto modifier = ((unk + 0.55f) * max_speed) / average_speed;

					vecVelocity.x *= modifier;
					vecVelocity.y *= modifier;
				}
			}

			vecVelocity.z = 0.f;
		}
	}

	flLastShotTime = weapon ? weapon->m_fLastShotTime() : FLT_MAX;

	const auto Cache       = pPlayer->CachedBones();
	const auto cachedCount = pPlayer->CachedBonesCount();

	std::memcpy(mBones.at(RESOLVER_STATE_NONE).data(), Cache, cachedCount);
}

void PlayerRecord::BulidBones (C_BasePlayer* pPlayer, ResolverState rState) {

	//interfaces::global_vars->set(m_sim_time, TIME_TO_TICKS(m_sim_time), [&]() -> void {
		/* set smth */
	auto BackupRealtime     = g_GlobalVars->curtime;
	auto BackupCurtime      = g_GlobalVars->curtime;
	auto BackupFrametime    = g_GlobalVars->frametime;
	auto BackupAbsFrametime = g_GlobalVars->absoluteframetime;
	auto BackupFramecount   = g_GlobalVars->framecount;
	auto BackupTickcount    = g_GlobalVars->tickcount;


	

	g_GlobalVars->realtime          = flSimTime;
	g_GlobalVars->curtime           = flSimTime;
	g_GlobalVars->frametime         = g_GlobalVars->interval_per_tick;
	g_GlobalVars->absoluteframetime = g_GlobalVars->interval_per_tick;
	g_GlobalVars->framecount        = TIME_TO_TICKS(flSimTime);
	g_GlobalVars->tickcount         = TIME_TO_TICKS(flSimTime);

	pPlayer->GetEffect() |= 0x8;

	Snakeware::bBoneSetup = true;

	pPlayer->SetupBones(mBones.at(rState).data(), 128, BONE_USED_BY_ANYTHING, flSimTime);

	Snakeware::bBoneSetup = false;

	pPlayer->GetEffect() &= 0x8;

	g_GlobalVars->curtime            = BackupCurtime;
	g_GlobalVars->realtime           = BackupRealtime;
	g_GlobalVars->frametime          = BackupFrametime;
	g_GlobalVars->absoluteframetime  = BackupAbsFrametime;
	g_GlobalVars->framecount         = BackupFramecount;
	g_GlobalVars->tickcount          = BackupTickcount;


		/* restore smth */
	//});
}


void LagCompensation::Instance (ClientFrameStage_t stage) {
	if (stage != FRAME_NET_UPDATE_END)
		return;

	if (!g_LocalPlayer || !g_EngineClient->IsInGame())
		return;

	for (auto i = 1; i <= g_GlobalVars->maxClients; i++) {
		if (i == g_EngineClient->GetLocalPlayer())
			continue;

		const auto pPlayer = reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));

		auto& cur_log = mLogs.at(i - 1u);

		if (cur_log.m_pPlayer != pPlayer) {
			cur_log.Reset();
		}

		cur_log.m_pPlayer = pPlayer;

		if (!pPlayer || !pPlayer->IsAlive()) {
			cur_log.Reset();
			continue;
		}

		auto& records = cur_log.mRecords;

		if (records.size() >= 2u
			&& !pPlayer->IsEnemy()) {
			records.resize(1u);
		}

		if (pPlayer->IsDormant()) {
			if (const auto latest = cur_log.GetLatestRecord()) {
				if (!latest->bDormant) {
					records.emplace_front(pPlayer, nullptr, true);
				}

				continue;
			}

			records.emplace_front(pPlayer, nullptr, true);
			continue;
		}

		const auto state = pPlayer->GetPlayerAnimState();

		if (cur_log.flSpawnTime != pPlayer->m_flSpawnTime()) {

			pPlayer->ResetAnimationState(state);
			//state->Reset();

			cur_log.Reset();

			cur_log.flSpawnTime = pPlayer->m_flSpawnTime();
		}

		PlayerRecord* previous = nullptr;

		if (!records.empty()
			&& !records.front().bDormant) {
			previous = &records.front();
		}

		auto record = PlayerRecord(pPlayer, previous, false);

		//Animations::Get().OnPlayerLog (&cur_log, &record, previous);
		//here resolver

		records.push_front(std::move(record));
	}
}