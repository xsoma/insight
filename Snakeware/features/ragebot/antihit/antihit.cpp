#include "antihit.h"
#include "../../../options.hpp"
#include "../../../helpers/input.hpp"
#include <algorithm>
#include <iostream>

#include "../../../helpers/math.hpp"




void AntiHit::createMove (CUserCmd * pcmd) {

	// CUserCmd init.
	this->userpcmd = pcmd;
	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();

	if (!g_Options.antihit_enabled)     return; // Enabled check.
	if (pcmd->buttons & IN_USE)         return; // In use check.
	if (pcmd->buttons & IN_ATTACK)      return; // In attack check.
	if (!weapon)                        return; // Weapon check.


	if (g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER) return; // ladder & noclip check
	if (g_LocalPlayer->m_fFlags() & FL_FROZEN) return; // Freez check.
	if (weapon->IsGrenade    ()) // Grenade check's.
	if (weapon->m_fThrowTime () > 0)  return;

	if (InputSys::Get().WasKeyPressed(g_Options.antihit_fake_switch_key)) switchSide = !switchSide;


	// Manual antihit part..
	if (InputSys::Get().WasKeyPressed(g_Options.antihit_manual_left)) {
		sideLeft  = true;
		sideRight = false;
		sideBack  = false;
	}
	else if (InputSys::Get().WasKeyPressed(g_Options.antihit_manual_right)) {
		sideLeft  = false;
		sideRight = true;
		sideBack  = false;
	}
	else if (InputSys::Get().WasKeyPressed(g_Options.antihit_manual_back)) {
		sideLeft  = false;
		sideRight = false;
		sideBack  = true;
	}


    // Init part.
	pitchType    = g_Options.antihit_pitch;
	yawType      = g_Options.antihit_yaw;
	lbyType      = g_Options.antihit_lby;
	jitterType   = g_Options.antihit_jitter_type;
	jitterRadius = g_Options.antihit_jitter_radius;



	// Func call.
	Pitch ();
	Fake  ();
	Lby   ();

}

float AntiHit::jitterRange (int min,int max) {
	// need some work with this
	static bool _switch = true;
	_switch = !_switch;
	switch (jitterType) {
	case 0:
		return Math::RandomFloat(min, max); // Default jitter.
		break;
	}
	
}

float AntiHit::manualYaw() {
	// 02.09.20
	// Selfcoded by @Snake
	float flReturnValue = 1.f;

	if      (sideLeft)  flReturnValue =    90.f;
	else if (sideRight) flReturnValue =   -90.f;
	else if (sideBack)  flReturnValue =   180.f;

	return flReturnValue;
}

float AntiHit::Yaw () {
	
	 float returnYaw = 0.f;

	// Def backwards.
	 switch (yawType) {
	 case 0:
		 returnYaw += jitterType < 1 ? 180 + jitterRange(-jitterRadius, jitterRadius) : fmodf(g_GlobalVars->tickcount * 10.f, jitterRadius); // Default.
		 break;

	 case 1:
		 returnYaw += manualYaw();
		 break;
	 }
	return returnYaw; // Return nullptr.
}

float AntiHit::EmotionPitch () {

	// Simple pitch calculate like valve.
	// Emotion like onetap (but use flMinPitch in animstate (by@Snake).
	// But is realy meme and wrong..

	auto   pState = g_LocalPlayer->GetPlayerAnimState ( );

	return pState->m_flMinPitch();
	// return userpcmd->viewangles.pitch = std::clamp (90.0f, -90.1f, 90.1f); Old variant
}

void AntiHit::Pitch() {

	switch (pitchType) 	{
	case 0: // Down .
		userpcmd->viewangles.pitch = 88.9f;
		break;
	case 1: // Emotion.
		userpcmd->viewangles.pitch = EmotionPitch();
		break;
	case 2: // Zero pitch.
		userpcmd->viewangles.pitch =  0.f;
		break;
	case 3: // Up pitch.
		userpcmd->viewangles.pitch = -88.9f;
		break;
	case 4: // Fake up
		userpcmd->viewangles.pitch = g_Options.misc_anti_untrusted ? -89.f : -540.f;
		break;
	case 5: // Fake down
		userpcmd->viewangles.pitch = g_Options.misc_anti_untrusted ? 89.f  :  540.f;
		break;
	}

}
int AntiHit::getTickBase (CUserCmd* ucmd)  {

	static int g_tick = 0;
	static CUserCmd* g_pLastCmd = nullptr;

	if (!ucmd)
		return g_tick;

	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) {
		g_tick = g_LocalPlayer->m_nTickBase();
	}
	else {
		// Required because prediction only runs on frames, not ticks
		// So if your framerate goes below tickrate, m_nTickBase won't update every tick
		++g_tick;
	}

	g_pLastCmd = ucmd;
	return g_tick;
}

void AntiHit::updateLbyBreaker (CUserCmd *usercmd) {

	if (!g_LocalPlayer || g_LocalPlayer->IsAlive()) return;
	if (!g_EngineClient->IsInGame()) return;
	if (!g_EngineClient->IsConnected()) return;

	    bool
		allocate = (m_serverAnimState == nullptr),
		change = (!allocate) && (&g_LocalPlayer->GetRefEHandle() != m_ulEntHandle),
		reset = (!allocate && !change) && (g_LocalPlayer->m_flSpawnTime() != m_flSpawnTime);

	// player changed, free old animation state.
	if (change)
		g_MemAlloc->Free(m_serverAnimState);

	// need to reset? (on respawn)
	if (reset)
	{
		// reset animation state.
		C_BasePlayer::ResetAnimationState(m_serverAnimState);

		// note new spawn time.
		m_flSpawnTime = g_LocalPlayer->m_flSpawnTime();
	}

	// need to allocate or create new due to player change.
	if (allocate || change)
	{
		// only works with games heap alloc.
		CCSGOPlayerAnimState *state = (CCSGOPlayerAnimState*)g_MemAlloc->Alloc(sizeof(CCSGOPlayerAnimState));

		if (state != nullptr)
			g_LocalPlayer->CreateAnimationState(state);

		// used to detect if we need to recreate / reset.
		m_ulEntHandle = const_cast<CBaseHandle*>(&g_LocalPlayer->GetRefEHandle());
		m_flSpawnTime = g_LocalPlayer->m_flSpawnTime();

		// note anim state for future use.
		m_serverAnimState = state;
	}

	float_t curtime = TICKS_TO_TIME( getTickBase () ) ;
	if (!g_ClientState->iChokedCommands && m_serverAnimState)
	{
		C_BasePlayer::UpdateAnimationState(m_serverAnimState, usercmd->viewangles);

		// calculate delta.
		float_t delta = std::abs(Math::ClampYaw(usercmd->viewangles.yaw - g_LocalPlayer->m_flLowerBodyYawTarget()));

		// walking, delay next update by .22s.
		if (m_serverAnimState->speed_2d > 0.1f && (g_LocalPlayer->m_fFlags() & FL_ONGROUND))
			m_flNextBodyUpdate = curtime + 0.22f;

		else if (curtime >= m_flNextBodyUpdate)
		{
			if (delta > 35.f)
				; // Server will update lby.

			m_flNextBodyUpdate = curtime + 1.1f;
		}
	}

	// if was jumping and then onground and bsendpacket true, we're gonna update.
	m_bBreakLowerBody = (g_LocalPlayer->m_fFlags() & FL_ONGROUND) && ((m_flNextBodyUpdate - curtime) <= g_GlobalVars->interval_per_tick);
}


void AntiHit::oppositeLby () {
	if (!Snakeware::bSendPacket) {
		if (m_bBreakLowerBody) {

			Snakeware::bSendPacket = false; // For choked eye.
			userpcmd->viewangles.yaw += switchSide ? -116.f : 116.f;
			userpcmd->viewangles.yaw = Math::NormalizeYaw(userpcmd->viewangles.yaw);
		}
	}
}

void AntiHit::swayLby() {
	static bool sway;
	if (!Snakeware::bSendPacket) {
		if (m_bBreakLowerBody) {

			Snakeware::bSendPacket = false; // For choked eye.

			if (!sway)
			userpcmd->viewangles.yaw += switchSide ? -120.f : 120.f;

			userpcmd->viewangles.yaw = Math::NormalizeYaw(userpcmd->viewangles.yaw);
			sway = !sway;
		}
	}
}

void AntiHit::lowdeltaLby() {
	if (!Snakeware::bSendPacket) {
		if (m_bBreakLowerBody) {

			Snakeware::bSendPacket = false; // For choked eye.
			userpcmd->viewangles.yaw += switchSide ? -60.f : 60.f;

			userpcmd->viewangles.yaw = Math::NormalizeYaw(userpcmd->viewangles.yaw);
		}
	}
}


void AntiHit::Lby() {
	static bool flip = false;
	            flip = !flip;
	float sideMove    = 2.28f;  // Side-move value
	float forwardMove = 1.1f;  // Forward-move value | Old move : 1.01f;

	if (g_Options.antihit_stabilize_lby && !Snakeware::bSendPacket && g_LocalPlayer->m_fFlags() & FL_ONGROUND)
	    userpcmd->forwardmove += flip ? -forwardMove : forwardMove;

	switch (lbyType) {
	case 0: // Micr0 move's.

		if (g_LocalPlayer->m_fFlags() & FL_ONGROUND && userpcmd->sidemove < 3.28 && userpcmd->sidemove > -3.28
			&& g_LocalPlayer->m_vecVelocity().Length2D() < 0.15) {
			static bool Switch_ = false;

			if (userpcmd->buttons & IN_DUCK)
				sideMove *= 3;

			if (userpcmd->tick_count % 2)
				Switch_ =  !Switch_;

			
			if (Switch_)
				userpcmd->sidemove = sideMove;
			else
				userpcmd->sidemove = -sideMove;

			// Switch_ = !Switch;
		}

		break;
	case 1: // Opposite lby.
		oppositeLby ();
		break;
	case 2:
		swayLby ();
		break;
	case 3:
		lowdeltaLby ();
		break;
	}
	

}


void AntiHit::Fake () {

	float       realYaw     = Yaw ();
	float       customFake  = std::clamp<float> (g_Options.antihit_fake_ammount, -65.f, 65.f); // Nan0 tech by @snake.
	float       nOlikDesync = switchSide ? -customFake : customFake;
	static bool Switch      = true;

	// Desynchronize animation's.
	if (Snakeware::bSendPacket) {

		// Real angle.
		userpcmd->viewangles.yaw += realYaw;
		userpcmd->viewangles.yaw += switchSide ? g_Options.antihit_body_lean * 3.6f : g_Options.antihit_invert_body_lean * 3.6f; //Body lean
	}
	else {
		// Fake angle.
		if (g_Options.antihit_fake < 1) // Static desync.
			userpcmd->viewangles.yaw += realYaw + nOlikDesync;
		else {
			// LagSync desync. 

			Switch = !Switch; 
			userpcmd->viewangles.yaw += Switch ? realYaw - nOlikDesync : realYaw + nOlikDesync;
		}

	}
	// Body-lean set.
	

}

// Selfoded anti-aim.
// Credit's : @Snake.