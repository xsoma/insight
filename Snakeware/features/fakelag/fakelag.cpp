#include "fakelag.h"
#include "../../helpers/math.hpp"
#include "../../options.hpp"

void Fakelag::OnCreateMove (CUserCmd * pCmd) {

	if (!g_Options.misc_fakelag)                     return;

	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive()) return;

	if (g_EngineClient->IsVoiceRecording())          return;


	if (g_Options.misc_fakelag_on_shot && (pCmd->buttons & IN_ATTACK)) return;

	 bStandTrigger    = false;
	 bMoveTrigger     = false;
	 bHighMoveTrigger = false;
	 bAirTrigger      = false;
	 bSlowWalkTrigger = false;
	

	flSpeed2D   = g_LocalPlayer->m_vecVelocity().Length2D();
	flSpeed     = g_LocalPlayer->m_vecVelocity().Length  ();

	

	FakelagTriggererd (); // B1g meme :D
	bool bTriggered = bStandTrigger || bMoveTrigger || bHighMoveTrigger || bAirTrigger || bSlowWalkTrigger;


	iLlamaTick = std::ceilf((68.0 / (g_GlobalVars->interval_per_tick * flSpeed)));
	iRandomTick = Math::RandomInt(2, bTriggered ? g_Options.misc_fakelag_triggered_ticks : g_Options.misc_fakelag_ticks);

	switch (g_Options.misc_fakelag_type) {

	case 0 : // Default
		if (bTriggered)
			iChoke = std::min < int >(g_Options.misc_fakelag_triggered_ticks, 16);
		else
		    iChoke = std::min < int > (g_Options.misc_fakelag_ticks, 14);
		break;

	case 1: // Onetap like adaptive uf yeah 

		if (iLlamaTick > 14 || flSpeed2D < 7) {

			if (bTriggered)
				iChoke = g_Options.misc_fakelag_ticks;
			else
				iChoke = g_Options.misc_fakelag_triggered_ticks;

		}
		else {

			iChoke = iLlamaTick;
		}
		break;

	case 2: // Random 
		iChoke = iRandomTick;
		break;


	}

	Snakeware::bSendPacket = (iChoked > iChoke);

	if (Snakeware::bSendPacket)
		iChoked = 0;
	else
		iChoked++;

}

void Fakelag::ForceChoke (int iForceChoked) {

	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive()) return;

	Snakeware::bSendPacket = g_ClientState->iChokedCommands >= iForceChoked;

}

void Fakelag::FakelagTriggererd () {

	
	if (g_Options.misc_fakelag_triggers[0]) {

		if (g_LocalPlayer->m_fFlags() & FL_ONGROUND && flSpeed2D < 5)
			bStandTrigger = true;

	}

	if (g_Options.misc_fakelag_triggers[1]) {

		if (g_LocalPlayer->m_fFlags() & FL_ONGROUND && flSpeed2D > 10)
			bMoveTrigger = true;

	}

	if (g_Options.misc_fakelag_triggers[2]) {

		if (g_LocalPlayer->m_fFlags() & FL_ONGROUND && flSpeed2D > 155 )
			bHighMoveTrigger = true;

	}

	if (g_Options.misc_fakelag_triggers[3]) {

		if ( !(g_LocalPlayer->m_fFlags() & FL_ONGROUND) || g_LocalPlayer->m_vecVelocity().z > 100)
			bAirTrigger = true;

	}

	if (g_Options.misc_fakelag_triggers[4]) {

		if (g_LocalPlayer->m_fFlags() & FL_ONGROUND && flSpeed2D > 5 && GetAsyncKeyState(g_Options.misc_slowwalk_key))
			bSlowWalkTrigger = true;

	}



}