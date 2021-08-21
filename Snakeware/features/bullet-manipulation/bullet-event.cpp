#include "bullet-event.h"
#include "../../valve_sdk/interfaces/IVRenderBeams.h"
#include "../../features/ragebot/ragebot.h"


void BulletImpactEvent::FireGameEvent(IGameEvent *event) {
	if (!g_LocalPlayer || !event) return;
	static ConVar* sv_showimpacts = g_CVar->FindVar("sv_showimpacts");
	if (g_Options.misc_bullet_impacts)
		sv_showimpacts->SetValue(1);
	else
		sv_showimpacts->SetValue(0);

	if (g_Options.misc_bullet_tracer) {
		if (g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) == g_EngineClient->GetLocalPlayer() && g_LocalPlayer && g_LocalPlayer->IsAlive())
		{
			float x = event->GetFloat("x"), y = event->GetFloat("y"), z = event->GetFloat("z");
			bulletImpactInfo.push_back({ g_GlobalVars->curtime, Vector(x, y, z) });
		}
	}

	
		int32_t userid = g_EngineClient->GetPlayerForUserID(event->GetInt("userid"));

		if (userid == g_EngineClient->GetLocalPlayer()) {

			if (RageBot::Get().iTargetID != NULL) {

				auto player = C_BasePlayer::GetPlayerByIndex(RageBot::Get().iTargetID);
				if (!player)	return;

				//here resolver
				/*int32_t idx = player->EntIndex();
				auto &player_recs = Resolver::Get().ResolveRecord[idx];

				if (!player->IsDormant())
				{
					int32_t tickcount = g_GlobalVars->tickcount;

					if (tickcount != tickHitWall)
					{
						tickHitWall = tickcount;
						originalShotsMissed = player_recs.iMissedShots;
						

						if (tickcount != tickHitPlayer) {
							tickHitWall = tickcount;
							++player_recs.iMissedShots;

				
						}
					}
				}*/
			}
		}
	
	

}

int BulletImpactEvent::GetEventDebugID(void)
{
	return EVENT_DEBUG_ID_INIT;
}

void BulletImpactEvent::RegisterSelf()
{
	g_GameEvents->AddListener(this, "bullet_impact", false);
}

void BulletImpactEvent::UnregisterSelf()
{
	g_GameEvents->RemoveListener(this);
}

void BulletImpactEvent::Paint(void)
{
	if (!g_Options.misc_bullet_tracer) return;

	if (!g_EngineClient->IsInGame() || !g_LocalPlayer || !g_LocalPlayer->IsAlive()) {
		bulletImpactInfo.clear();
		return;
	}

	std::vector<BulletImpactInfo> &impacts = bulletImpactInfo;

	if (impacts.empty()) return;
	Color current_color(g_Options.color_bullet_tracer);
	for (size_t i = 0; i < impacts.size(); i++)
	{
		auto current_impact = impacts.at(i);

		BeamInfo_t beamInfo;

		beamInfo.m_nType = TE_BEAMPOINTS;
		beamInfo.m_pszModelName = "sprites/purplelaser1.vmt";
		beamInfo.m_nModelIndex = -1;
		beamInfo.m_flHaloScale = 0.0f;
		beamInfo.m_flLife = 3.3f;
		beamInfo.m_flWidth = 4.f;
		beamInfo.m_flEndWidth = 4.f;
		beamInfo.m_flFadeLength = 0.0f;
		beamInfo.m_flAmplitude = 2.0f;
		beamInfo.m_flBrightness = 255.f;
		beamInfo.m_flSpeed = 0.2f;
		beamInfo.m_nStartFrame = 0;
		beamInfo.m_flFrameRate = 0.f;
		beamInfo.m_flRed = current_color.r();
		beamInfo.m_flGreen = current_color.g();
		beamInfo.m_flBlue = current_color.b();
		beamInfo.m_nSegments = 2;
		beamInfo.m_bRenderable = true;
		beamInfo.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM | FBEAM_FADEIN;

		beamInfo.m_vecStart = g_LocalPlayer->GetEyePos();
		beamInfo.m_vecEnd = current_impact.m_vecHitPos;

		auto beam = g_RenderBeam->CreateBeamPoints(beamInfo);
		if (beam)   g_RenderBeam->DrawBeam(beam);


		impacts.erase(impacts.begin() + i);
	}
}

void BulletImpactEvent::AddSound(C_BasePlayer *p) {
	
}