#include "engine-prediction.h"
#include "../options.hpp"
void EnginePrediction::PreStart() {

	if (Snakeware::g_flVelocityModifer < 1.f)
		*(bool*)((uintptr_t)g_Prediction + 0x24) = true;

	if (g_ClientState->iDeltaTick > 0)
		g_Prediction->Update(g_ClientState->iDeltaTick, true, g_ClientState->nLastCommandAck, g_ClientState->nLastOutgoingCommand + g_ClientState->iChokedCommands);
}


void EnginePrediction::Predict (C_BasePlayer* pPlayer, CUserCmd* pCmd) {

	if (!pPlayer->IsAlive()) return;

	//m_player = player;
	*iRandomSeed = pCmd->random_seed;

	*(bool*)(uintptr_t(g_Prediction) + 8) = true; // InPrediction
	// One more + gamefunc

	g_GlobalVars->curtime    = TICKS_TO_TIME(pPlayer->m_nTickBase());
	g_GlobalVars->frametime  = pPlayer->m_fFlags() & FL_FROZEN ? 0.f : g_GlobalVars->interval_per_tick;

	g_MoveHelper->SetHost                       (pPlayer);

	g_GameMovement->StartTrackPredictionErrors  (pPlayer);

	g_GameMovement->ProcessMovement             (pPlayer, MoveData);

	g_Prediction->FinishMove                    (pPlayer, pCmd, MoveData);

	g_GameMovement->FinishTrackPredictionErrors (pPlayer);

	g_MoveHelper->SetHost                       (nullptr);

	//m_player = nullptr;
	*iRandomSeed = -1;

	const auto pWeapon = pPlayer->m_hActiveWeapon();

	if (!pWeapon) {
		flSpread = flInaccuracy = 0.f;
		return;
	}
	
	pWeapon->UpdateAccuracyPenalty ();

	flSpread     = pWeapon->GetSpread();

	flInaccuracy = pWeapon->GetInaccuracy();
}

void EnginePrediction::ProcessPredict (C_BasePlayer* pPlayer, CUserCmd* pCmd) {

	Backup.Store();

	//m_player = player;
	*iRandomSeed = pCmd->random_seed;

	g_GlobalVars->curtime   = TICKS_TO_TIME (pPlayer->m_nTickBase());
	g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;

	g_GameMovement->StartTrackPredictionErrors (pPlayer);

	g_Prediction->SetupMove (pPlayer, pCmd, g_MoveHelper, MoveData);

	Predict (pPlayer, pCmd);
}

void EnginePrediction::Restore (C_BasePlayer* pPlayer, CUserCmd* pCmd) {
	//m_player = nullptr;
	*iRandomSeed = -1;

	Backup.Restore();
}

// Need more cmd-fix