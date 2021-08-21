#include "animation-system.h"
#include "../../../options.hpp"
#include "../player-log/player-log.h"
#include "../lagcompensation/lag-compensation.h"

void Animations::OnPlayerLog(PlayerLog* log, PlayerRecord* record, PlayerRecord* previous) {
	const auto state = log->m_pPlayer->GetPlayerAnimState();

	const auto movement_backup = MovementBackup(log->m_pPlayer);

	log->interpolate(record, previous);

	if (log->m_pPlayer->IsEnemy()
		&& !log->m_pPlayer->GetPlayerInfo().fakeplayer) {
		const auto animation_backup = AnimationBackup(log->m_pPlayer);

		for (auto i = 1u; i <= 2u; i++) {

			Update(log, record, previous, static_cast<ResolverState>(i));

			record->mResolver.AnimLayers.at(i) = log->m_pPlayer->GetGayLayers();
			record->mResolver.flAbsYaw.at(i - 1u) = state->m_flGoalFeetYaw;
			record->mResolver.flPoses.at(i - 1u) = log->m_pPlayer->m_flPoseParameter();

			log->m_pPlayer->GetGayLayers() = record->ServerLayers;

			record->BulidBones(log->m_pPlayer, static_cast<ResolverState>(i));

			animation_backup.Restore(log->m_pPlayer);
		}

	/*	if (Math::angle_diff(record->m_eye_angles.y, record->m_resolver.m_abs_yaw.at(RESOLVER_STATE_RIGHT - 1u)) < 0.f) {
			std::swap(record->m_bones.at(RESOLVER_STATE_LEFT), record->m_bones.at(RESOLVER_STATE_RIGHT));

			std::swap(record->m_resolver.m_layers.at(RESOLVER_STATE_LEFT), record->m_resolver.m_layers.at(RESOLVER_STATE_RIGHT));
			std::swap(record->m_resolver.m_poses.at(RESOLVER_STATE_LEFT - 1u), record->m_resolver.m_poses.at(RESOLVER_STATE_RIGHT - 1u));
			std::swap(record->m_resolver.m_abs_yaw.at(RESOLVER_STATE_LEFT - 1u), record->m_resolver.m_abs_yaw.at(RESOLVER_STATE_RIGHT - 1u));
		}*/
	}

	Update(log, record, previous, RESOLVER_STATE_NONE);

	record->mResolver.AnimLayers.at(RESOLVER_STATE_NONE) = log->m_pPlayer->GetGayLayers();

	log->m_pPlayer->GetGayLayers() = record->ServerLayers;

	record->BulidBones(log->m_pPlayer, RESOLVER_STATE_NONE);

	movement_backup.Restore(log->m_pPlayer);
}

void Animations::Update(PlayerLog* log, PlayerRecord* record, PlayerRecord* previous, ResolverState r_state) {

	const auto state = log->m_pPlayer->GetPlayerAnimState();

	const auto interpolated_backup = InterPolatedBackup(log->m_pPlayer, state);

	if (previous) {
		log->m_pPlayer->GetGayLayers() = previous->ServerLayers;

		//state->m_acceleration_weight = previous->m_server_layers.at(ANIMATION_LAYER_LEAN).m_weight;

		state->m_flFeetCycle   = previous->ServerLayers.at(6).m_flCycle;
		state->m_flFeetYawRate = previous->ServerLayers.at(6).m_flWeight;
	}
	else {
		log->m_pPlayer->GetGayLayers() = record->ServerLayers;

		if (record->mFlags.Has(FL_ONGROUND)) {
			state->m_bOnGround             = true;
			state->m_bInHitGroundAnimation = false;
		}

		state->m_flLastClientSideAnimationUpdateTime = record->flAnimTime - g_GlobalVars->interval_per_tick;
	}

	if (record->iTotalCommands <= 1) {
		log->mInterpolated.front().Restore(log->m_pPlayer);

		if (r_state != RESOLVER_STATE_NONE) {
//			state->m_flGoalFeetYaw = Math::NormalizeYaw(record->eyeAngles.y + (r_state == RESOLVER_STATE_LEFT ? -60.f : 60.f));
		}

		UpdateClientSideAnimations(log->m_pPlayer, record->flAnimTime);
	}
	else {
		const auto shot_in_this_cycle = record->flLastShotTime >= record->flAnimTime && record->flLastShotTime <= record->flSimTime;
		const auto ticks_to_final_eye_angles = shot_in_this_cycle ? TIME_TO_TICKS(record->flLastShotTime - record->flAnimTime) + 1 : record->iTotalCommands;

		for (auto i = 1; i <= record->iTotalCommands; i++) {
			auto& interpolated = log->mInterpolated.at(i - 1u);

			interpolated.Restore(log->m_pPlayer);

			if (r_state != RESOLVER_STATE_NONE
				&& i < ticks_to_final_eye_angles) {
			//	state->m_goal_feet_yaw = math::normalize(record->m_eye_angles.y + (r_state == RESOLVER_STATE_LEFT ? -60.f : 60.f));
			}

			UpdateClientSideAnimations(log->m_pPlayer, interpolated.flSimTime);
		}
	}

	interpolated_backup.Restore(log->m_pPlayer, state);

	log->m_pPlayer->InvalidatePhysics(C_BaseEntity::animation_changed);
}


void Animations::UpdateClientSideAnimations (C_BasePlayer * pPlayer, float Time) {

	auto BackupRealtime     = g_GlobalVars->curtime;
	auto BackupCurtime      = g_GlobalVars->curtime;
	auto BackupFrametime    = g_GlobalVars->frametime;
	auto BackupAbsFrametime = g_GlobalVars->absoluteframetime;
	auto BackupFramecount   = g_GlobalVars->framecount;
	auto BackupTickcount    = g_GlobalVars->tickcount;


	

	g_GlobalVars->realtime          = Time;
	g_GlobalVars->curtime           = Time;
	g_GlobalVars->frametime         = g_GlobalVars->interval_per_tick;
	g_GlobalVars->absoluteframetime = g_GlobalVars->interval_per_tick;
	g_GlobalVars->framecount        = TIME_TO_TICKS(Time);
	g_GlobalVars->tickcount         = TIME_TO_TICKS(Time);


	pPlayer->m_iEFlags() &= ~C_BaseEntity::E_FL_DIRTYABSVELOCITY; // Nano tech.

	pPlayer->GetPlayerAnimState()->m_iLastClientSideAnimationUpdateFramecount = g_GlobalVars->framecount - 1;

	pPlayer->m_bClientSideAnimation() = true;
	
	Snakeware::UpdateAnims = true;

	pPlayer->UpdateClientSideAnimation();

	Snakeware::UpdateAnims = false;

	pPlayer->m_bClientSideAnimation() = false;

	g_GlobalVars->curtime           = BackupCurtime;
	g_GlobalVars->realtime          = BackupRealtime;
	g_GlobalVars->frametime         = BackupFrametime;
	g_GlobalVars->absoluteframetime = BackupAbsFrametime;
	g_GlobalVars->framecount        = BackupFramecount;
	g_GlobalVars->tickcount         = BackupTickcount;

}

void Animations::FakeAnimation()
{
	//static bool ShouldInitAnimstate = false;

	if ((!g_EngineClient->IsConnected() && !g_EngineClient->IsInGame()) || !g_LocalPlayer) {
		return;
	}
	if (!g_Options.chams_fake) return;

	if (!g_LocalPlayer->IsAlive()) {
		return;
	}

	static CBaseHandle* selfhandle = nullptr;
	static float spawntime = g_LocalPlayer->m_flSpawnTime();

	auto alloc = FakeAnimstate == nullptr;
	auto change = !alloc && selfhandle != &g_LocalPlayer->GetRefEHandle();
	auto reset = !alloc && !change && g_LocalPlayer->m_flSpawnTime() != spawntime;

	if (change) {
		memset(&FakeAnimstate, 0, sizeof(FakeAnimstate));
		selfhandle = (CBaseHandle*)&g_LocalPlayer->GetRefEHandle();
	}
	if (reset) {
		g_LocalPlayer->ResetAnimationState(FakeAnimstate);
		spawntime = g_LocalPlayer->m_flSpawnTime();
	}

	if (alloc || change) {
		FakeAnimstate = reinterpret_cast<CCSGOPlayerAnimState*>(g_MemAlloc->Alloc(sizeof(CCSGOPlayerAnimState)));
		if (FakeAnimstate)
			g_LocalPlayer->CreateAnimationState(FakeAnimstate);
	}

	if (FakeAnimstate->m_iLastClientSideAnimationUpdateFramecount == g_GlobalVars->framecount)
		FakeAnimstate->m_iLastClientSideAnimationUpdateFramecount -= 1.f;

	g_LocalPlayer->GetEffect() |= 0x8;

	g_LocalPlayer->InvalidateBoneCache();

	AnimationLayer backup_layers[13];
	if (g_LocalPlayer->m_flSimulationTime() != g_LocalPlayer->m_flOldSimulationTime())
	{
		bool pidoras = true;
		std::memcpy(backup_layers, g_LocalPlayer->GetAnimOverlays(),
			(sizeof(AnimationLayer) * g_LocalPlayer->GetNumAnimOverlays()));

		g_LocalPlayer->UpdateAnimationState(FakeAnimstate, Snakeware::FakeAngle); // update animstate
		g_LocalPlayer->SetAbsAngles(QAngle(0, FakeAnimstate->m_flGoalFeetYaw, 0));
		g_LocalPlayer->GetAnimOverlay(12)->m_flWeight = FLT_EPSILON;
		g_LocalPlayer->SetupBones(Snakeware::FakeMatrix, 128, 0x7FF00, g_GlobalVars->curtime);// setup matrix

		for (auto& i : Snakeware::FakeMatrix)
		{
			i[0][3] -= g_LocalPlayer->GetRenderOrigin().x;
			i[1][3] -= g_LocalPlayer->GetRenderOrigin().y;
			i[2][3] -= g_LocalPlayer->GetRenderOrigin().z;
		}


		std::memcpy(g_LocalPlayer->GetAnimOverlays(), backup_layers,
			(sizeof(AnimationLayer) * g_LocalPlayer->GetNumAnimOverlays()));
	}

	//csgo->animstate = FakeAnimstate; usseles

	g_LocalPlayer->GetEffect() &= ~0x8;
}


bool CanFix () {

	if (g_Options.antihit_enabled)        return true;
	if (g_Options.misc_legit_antihit)     return true;
	if (g_Options.misc_fakelag_ticks >= 1 || g_ClientState->iChokedCommands > 1) return true;

	return false;
}

void Animations::FixLocalPlayer() {

	auto animstate = g_LocalPlayer->GetPlayerAnimState();
	if (!animstate)
		return;
	if (!g_LocalPlayer->IsAlive()) return;
	if (!g_EngineClient->IsInGame()) return;
	if (!CanFix) return;

	const auto interpolation       = g_GlobalVars->interpolation_amount;
	const auto backup_frametime    = g_GlobalVars->frametime;
	const auto backup_curtime      = g_GlobalVars->curtime;
	const auto backup_absframetime = g_GlobalVars->absoluteframetime;
	const auto backup_realtime     = g_GlobalVars->realtime;

	animstate->m_flGoalFeetYaw = Snakeware::RealAngle.yaw;

	if (animstate->m_iLastClientSideAnimationUpdateFramecount == g_GlobalVars->framecount)
		animstate->m_iLastClientSideAnimationUpdateFramecount -= 1.f;

	g_GlobalVars->absoluteframetime    = g_GlobalVars->interval_per_tick;
	g_GlobalVars->frametime            = g_GlobalVars->interval_per_tick;
	g_GlobalVars->curtime              = g_LocalPlayer->m_flSimulationTime();
	g_GlobalVars->realtime             = g_LocalPlayer->m_flSimulationTime();
	g_GlobalVars->interpolation_amount = 0.f;

	g_LocalPlayer->m_iEFlags() &= ~0x1000;
	g_LocalPlayer->m_vecAbsVelocity() = g_LocalPlayer->m_vecVelocity();

	static float angle = animstate->m_flGoalFeetYaw;

	animstate->m_flFeetYawRate = 0.f;

	AnimationLayer backup_layers[13];
	if (g_LocalPlayer->m_fFlags() & FL_ONGROUND && g_LocalPlayer->m_vecVelocity().Length2D() < 0.15) {

		// B1g ot fix

	}

	if (g_LocalPlayer->m_flSimulationTime() != g_LocalPlayer->m_flOldSimulationTime()) {
		std::memcpy(backup_layers, g_LocalPlayer->GetAnimOverlays(),
			(sizeof(AnimationLayer) * g_LocalPlayer->GetNumAnimOverlays()));

		g_LocalPlayer->m_bClientSideAnimation() = true;
		g_LocalPlayer->UpdateAnimationState(animstate, Snakeware::FakeAngle);
		g_LocalPlayer->UpdateClientSideAnimation();


		angle = animstate->m_flGoalFeetYaw;

		std::memcpy(g_LocalPlayer->GetAnimOverlays(), backup_layers, (sizeof(AnimationLayer) * g_LocalPlayer->GetNumAnimOverlays()));
	}



	animstate->m_flGoalFeetYaw      = Math::NormalizeYaw (angle);
	g_GlobalVars->curtime           = backup_curtime;
	g_GlobalVars->realtime = backup_realtime;
	g_GlobalVars->frametime         = backup_frametime;
	g_GlobalVars->absoluteframetime = backup_absframetime;

	// Backup Interpolation data.
	g_GlobalVars->interpolation_amount = interpolation;
}
void Animations::SetLocalPlayerAnimations()
{
	auto animstate = g_LocalPlayer->GetPlayerAnimState();
	if (!animstate) return;
	// weawe.su reverse
	if (g_EngineClient->IsInGame())
	{
		if (g_Input->m_fCameraInThirdPerson && g_Options.misc_thirdperson)
			g_LocalPlayer->SetSnakewareAngles(Snakeware::FakeAngle);

		if (g_LocalPlayer->m_fFlags() & FL_ONGROUND) {
			animstate->m_bOnGround = true;
			animstate->m_bInHitGroundAnimation = false;
		}

		g_LocalPlayer->SetAbsAngles(QAngle(0, g_LocalPlayer->GetPlayerAnimState()->m_flGoalFeetYaw, 0));
	}

}


