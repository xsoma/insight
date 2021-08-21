#include "bhop.hpp"

#include "../valve_sdk/csgostructs.hpp"
#include "../helpers/math.hpp"
void BunnyHop::OnCreateMove(CUserCmd* cmd)
{
  static bool jumped_last_tick = false;
  static bool should_fake_jump = false;

  if (!g_LocalPlayer)
	  return;

  if (!g_LocalPlayer->IsAlive())
	  return;

  if (g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER || g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP)
	  return;

  if (g_LocalPlayer->m_fFlags() & FL_INWATER)
	  return;

  if (!jumped_last_tick && should_fake_jump) {
	  should_fake_jump = false;
	  cmd->buttons |= IN_JUMP;
  }
  else if (cmd->buttons & IN_JUMP) {
	  if (g_LocalPlayer->m_fFlags() & FL_ONGROUND) {
		  jumped_last_tick = true;
		  should_fake_jump = true;
	  }
	  else {
		  cmd->buttons &= ~IN_JUMP;
		  jumped_last_tick = false;
	  }
  }
  else {
	  jumped_last_tick = false;
	  should_fake_jump = false;
  }
}
template<class T, class U>
T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}



void BunnyHop::AutoStrafe(CUserCmd * pCmd) {


	if (g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER || !g_LocalPlayer->IsAlive()) return;

	// If we're not jumping or want to manually move out of the way/jump over an obstacle don't strafe.
	if (!g_InputSystem->IsButtonDown(ButtonCode_t::KEY_SPACE) ||
		g_InputSystem->IsButtonDown(ButtonCode_t::KEY_A) ||
		g_InputSystem->IsButtonDown(ButtonCode_t::KEY_D) ||
		g_InputSystem->IsButtonDown(ButtonCode_t::KEY_S) ||
		g_InputSystem->IsButtonDown(ButtonCode_t::KEY_W))
		return;

	if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND)) {
		if (pCmd->mousedx > 1 || pCmd->mousedx < -1) {
			pCmd->sidemove = clamp(pCmd->mousedx < 0.f ? -400.f : 400.f, -400, 400);
		}
		else {
			if (g_LocalPlayer->m_vecVelocity().Length2D() == 0 || g_LocalPlayer->m_vecVelocity().Length2D() == NAN || g_LocalPlayer->m_vecVelocity().Length2D() == INFINITE)
			{
				pCmd->forwardmove = 400;
				return;
			}
			pCmd->forwardmove = clamp(5850.f / g_LocalPlayer->m_vecVelocity().Length2D(), -400, 400);
			if (pCmd->forwardmove < -400 || pCmd->forwardmove > 400)
				pCmd->forwardmove = 0;
			pCmd->sidemove = clamp((pCmd->command_number % 2) == 0 ? -400.f : 400.f, -400, 400);
			if (pCmd->sidemove < -400 || pCmd->sidemove > 400)
				pCmd->sidemove = 0;
		}
	}
}
	
