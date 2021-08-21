#include "player-hurt.h"
#include <algorithm>
#include "../../options.hpp"
#include "../../render.hpp"
#include "../../helpers/math.hpp"
#include "../../features/ragebot/ragebot.h"
#include "../../fonts/Sound_bite.h"

IGameEvent* event_ex;


static void DrawHitbox(C_BasePlayer * pPlayer, Color col, float duration)
{
	if (!pPlayer)
		return;

	studiohdr_t* pStudioModel = g_MdlInfo->GetStudioModel((model_t*)pPlayer->GetModel());
	if (!pStudioModel)
		return;

	static matrix3x4_t pBoneToWorldOut[128];
	if (!pPlayer->SetupBones(pBoneToWorldOut, 128, 256, 0))
		return;

	mstudiohitboxset_t* pHitboxSet = pStudioModel->GetHitboxSet(0);
	if (!pHitboxSet)
		return;
	auto VectorTransform2 = [](const Vector in1, matrix3x4_t in2, Vector& out)
	{

		out[0] = Math::DotProduct(in1, Vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3];
		out[1] = Math::DotProduct(in1, Vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3];
		out[2] = Math::DotProduct(in1, Vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3];
	};

	for (int i = 0; i < pHitboxSet->numhitboxes; i++)
	{
		mstudiobbox_t* pHitbox = pHitboxSet->GetHitbox(i);
		if (!pHitbox)
			continue;

		Vector vMin, vMax;
		VectorTransform2(pHitbox->bbmin, pBoneToWorldOut[pHitbox->bone], vMin); //nullptr???
		VectorTransform2(pHitbox->bbmax, pBoneToWorldOut[pHitbox->bone], vMax);

		if (pHitbox->m_flRadius > -1)
		{
			g_DebugOverlay->AddCapsuleOverlay(vMin, vMax, pHitbox->m_flRadius, col.r(), col.g(), col.b(), col.a(), duration);
		}
	}
}
void PlayerHurtEvent::FireGameEvent(IGameEvent *event)
{
	if (!g_LocalPlayer || !event)
		return;
	
	Snakeware::bShotFound = false;

	if (g_Options.misc_hitmarker)
	{
		if (g_EngineClient->GetPlayerForUserID(event->GetInt("attacker")) == g_EngineClient->GetLocalPlayer() && g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) != g_EngineClient->GetLocalPlayer())
		{
			hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
			if (g_Options.misc_hitsound)
			{
				//Resolver::Get().shots_hit[Resolver::Get().LastMissedShotIndex]++;
				switch (g_Options.misc_hitsound_type)
				{
				case 0: g_EngineClient->ExecuteClientCmd("play buttons\\arena_switch_press_02.wav");     break;
				case 1: g_EngineClient->ExecuteClientCmd("play fatality.wav");     break;
				case 2: g_EngineClient->ExecuteClientCmd("play cod.wav");     break;
				case 3: g_EngineClient->ExecuteClientCmd("play hit2.wav");     break;
				case 4: g_EngineClient->ExecuteClientCmd("play rifk.wav");     break;


				}
			}
			
		}
	}


	if (g_Options.misc_hiteffect)
	{

		if (g_EngineClient->GetPlayerForUserID(event->GetInt("attacker")) == g_EngineClient->GetLocalPlayer() && g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) != g_EngineClient->GetLocalPlayer())
		g_LocalPlayer->m_flHealthShotBoostExpirationTime() = g_GlobalVars->curtime + g_Options.misc_hiteffect_duration;
	}


	if (g_Options.shot_hitboxes)
	{
		int32_t attacker = g_EngineClient->GetPlayerForUserID(event->GetInt("attacker"));
		int32_t userid = g_EngineClient->GetPlayerForUserID(event->GetInt("userid"));
		auto Player = static_cast<C_BasePlayer *>(g_EntityList->GetClientEntity(userid));
		Color Hitbox = Color(g_Options.color_shot_hitboxes);

		if (attacker == g_EngineClient->GetLocalPlayer() && userid != g_EngineClient->GetLocalPlayer()) {
			// Nano-tech by Snake <3
			// Selfcoded who?
			Snakeware::bShotFound = true;
			if (g_Options.shot_hitboxes_type < 1)
			DrawHitbox(Player, Hitbox, g_Options.shot_hitboxes_duration);
			else {

				Player->InvalidateBoneCache(); // Invalidate.
				Player->SetupBones(Snakeware::mOnShotMatrix, 128, 256, 0); // 0 Что-бы матрица никак не обновлялась.
			}

		}
	}

	event_ex = event;

	//here resolver
	/*
	int32_t userid = event->GetInt("userid");
	auto player = C_BasePlayer::GetPlayerByUserId(userid);
	if (!player)
		return;

	int32_t idx = player->EntIndex();
	auto &player_recs = Resolver::Get().ResolveRecord[idx];

	if (!player->IsDormant()) {
		int32_t local_id = g_EngineClient->GetLocalPlayer();
		int32_t attacker = g_EngineClient->GetPlayerForUserID(event->GetInt("attacker"));

		if (attacker == local_id) {
			bool airJump = !(player->m_fFlags() & FL_ONGROUND) && player->m_vecVelocity().Length2D() > 1.0f;
			int32_t tickcount = g_GlobalVars->tickcount;

			if (tickHitWall == tickcount) {
				player_recs.iMissedShots = originalShotsMissed;
		
			}
			if (tickcount != tickHitPlayer) 	{
				tickHitPlayer = tickcount;
				player_recs.iMissedShots = 0;

			
			}
		}
	}
	*/
	
}

int PlayerHurtEvent::GetEventDebugID(void)
{
	return EVENT_DEBUG_ID_INIT;
}

void PlayerHurtEvent::RegisterSelf()
{
	g_GameEvents->AddListener(this, "player_hurt", false);
}

void PlayerHurtEvent::UnregisterSelf()
{
	g_GameEvents->RemoveListener(this);
}

static bool active_text = false;

std::string text;

void KillText_()
{
	{
		static int move_x = 0;
		static int move_y = 0;
		static int alpha = 255;


		static int randomposx = 0;
		static int randomposy = 0;



		if (active_text)
		{
			if (randomposx == 0)
			{
				randomposx = rand() % 1000;
				randomposy = rand() % 1000;
			}

			move_x = move_x + 2;
			move_y = move_y + 1;
			alpha--;
			Render::Get().RenderTextGiantNoOutnline(text, randomposx + move_x, randomposy + move_y, 30.f, Color(255, 0, alpha / 2, alpha));
		}

		if (alpha < 1)
		{

			active_text = false;
			alpha = 255;
			move_x = 0;
			move_y = 0;
			randomposx = 0;
			randomposy = 0;

		}
	}
}

void PlayerHurtEvent::Paint(void)
{
	static int width = 0, height = 0;
	if (width == 0 || height == 0)
		g_EngineClient->GetScreenSize(width, height);

	float alpha = 0.f;
	 
	
	if (g_Options.misc_hitmarker)
	{
		for (size_t i = 0; i < hitMarkerInfo.size(); i++)
		{
			float diff = hitMarkerInfo.at(i).m_flExpTime - g_GlobalVars->curtime;

			if (diff < 0.f)
			{
				hitMarkerInfo.erase(hitMarkerInfo.begin() + i);
				continue;
			}

			int dist = 24;

			float ratio = 1.f - (diff / 0.8f);
			alpha = 0.8f - diff / 0.8f;
			Color Snakeware = Color(255, 0, 175, 255);
			Render::Get().RenderText(std::to_string(hitMarkerInfo.at(i).m_iDmg).c_str(), width / 2 + 6 + ratio * dist / 2, height / 2 + 6 + ratio * dist, 16.f, Snakeware);
			active_text = true;


		}

		if (hitMarkerInfo.size() > 0)
		{
			Color White = Color(g_Options.color_hitmarker);
			int lineSize = g_Options.misc_hitmarker_size; // custom values
	
			Render::Get().RenderLine(width / 2 - lineSize / 2, height / 2 - lineSize / 2, width / 2 + lineSize / 2, height / 2 + lineSize / 2, White);
			Render::Get().RenderLine(width / 2 + lineSize / 2, height / 2 - lineSize / 2, width / 2 - lineSize / 2, height / 2 + lineSize / 2, White);


		}

		int shots = std::clamp(g_LocalPlayer->m_iShotsFired(), 0, 10);

		if (g_LocalPlayer->IsAlive())
		{
			switch (shots)
			{
			case 10: text = "DESTROYED"; break;
			case 9: text = "SUPERKILL"; break;
			case 8: text = "MACHINE  "; break;
			case 7: text = "MEGAKILL "; break;
			case 6: text = "ULTRAKILL"; break;
			case 5: text = "SUPERKILL"; break;
			case 4: text = "MEGASHOT "; break;
			case 3: text = "HEADSHOT "; break;
			case 2: text = "KILLER   "; break;
			case 1: text = "SUPERKILL"; break;
			}
		}
		KillText_();
	}
}

