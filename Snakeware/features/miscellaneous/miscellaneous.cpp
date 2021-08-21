#include "miscellaneous.h"
#include "../../options.hpp"
#include "../../helpers/math.hpp"
#include <chrono>
#include "../../helpers/input.hpp"
#include "../../menu.hpp"
#include "../../Protected/enginer.h"


bool Miscellaneous::RemoveSleeves(const char* modelName) noexcept
{
	return g_Options.remove_sleeves && std::strstr(modelName, "sleeve");
}




void Miscellaneous::RemoveSmoke() noexcept {
	if (!g_Options.remove_smoke || !g_Options.esp_enabled)
		return;

	if (!g_EngineClient->IsConnected() && !g_EngineClient->IsInGame())
		return;

	static auto smoke_count = *reinterpret_cast<uint32_t **>(Utils::PatternScan(GetModuleHandleA(solution::Get().Module), "A3 ? ? ? ? 57 8B CB") + 1); // shit pattern

	static std::vector<const char*> smoke_materials = {
		"particle/vistasmokev1/vistasmokev1_fire",
		"particle/vistasmokev1/vistasmokev1_smokegrenade",
		"particle/vistasmokev1/vistasmokev1_emods",
		"particle/vistasmokev1/vistasmokev1_emods_impactdust"
	};

	for (auto material_name : smoke_materials) {
		auto  smoke = g_MatSystem->FindMaterial(material_name, TEXTURE_GROUP_OTHER);
		smoke->IncrementReferenceCount();
		smoke->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);

		*(int*)smoke_count = 0;
	}
	// credit's : @Snake & UC
}





//Credit : @Soufiw
	//Last edited by d4rkd3n


bool Miscellaneous::KnifeBestTarget() {
	float bestDist = 75;
	for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		auto pBaseEntity = C_BasePlayer::GetPlayerByIndex(i);
		if (!pBaseEntity)
			continue;
		if (!pBaseEntity->IsAlive() || pBaseEntity->IsDormant())
			continue;
		if (pBaseEntity == g_LocalPlayer)
			continue;
		if (pBaseEntity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
			continue;

		Vector localPos = g_LocalPlayer->m_vecOrigin(); localPos.z += 50;
		Vector basePos = pBaseEntity->m_vecOrigin(); basePos.z += 50;
		if (!pBaseEntity->IsVisible(false))
			continue;

		float curDist = localPos.DistTo(basePos);
		if (curDist < bestDist)
		{
			bestDist = curDist;
			m_nBestIndex = i;
			pBestEntity = pBaseEntity;
		}
	}

	m_nBestDist = bestDist;
	return m_nBestIndex != -1;
}

void Miscellaneous::KnifeBot(CUserCmd* cmd) {
	m_nBestIndex = -1;
	if (!g_LocalPlayer || !g_Options.misc_knifebot) return;
	auto weapon = g_LocalPlayer->m_hActiveWeapon();

	if (!weapon) return;

	if (!weapon->IsKnife())
		return;

	if (!this->KnifeBestTarget())
		return;

	Vector tempLocalOrigin = g_LocalPlayer->m_vecOrigin();
	Vector tempBestOrigin = pBestEntity->m_vecOrigin();
	tempBestOrigin.z = tempLocalOrigin.z;
	QAngle entAng = Math::CalcAngle(tempLocalOrigin, tempBestOrigin);

	if (!lastAttacked)
	{
		bool stab = false;
		if (g_Options.misc_auto_knifebot)
		{
			int health = pBestEntity->m_iHealth();
			if (pBestEntity->m_ArmorValue())
			{
				if (health <= 55 &&
					health > 34)
					stab = true;
			}
			else
			{
				if (health <= 65 &&
					health > 40)
					stab = true;
			}

			stab = stab && m_nBestDist < 60;

			if (stab)
				cmd->buttons |= IN_ATTACK2;
			else
				cmd->buttons |= IN_ATTACK;
		}
		else
		{
			stab = cmd->buttons & IN_ATTACK2;
		}

		if (g_Options.misc_knifebot_360)
		{
			float server_time = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
			float next_shot = (stab ? weapon->m_flNextSecondaryAttack() : weapon->m_flNextPrimaryAttack()) - server_time;
		

			if (!(next_shot > 0) && (g_Options.misc_auto_knifebot || cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2))
			{
				cmd->viewangles = entAng;
				Snakeware::bSendPacket = false;
			}
		}
	}

	lastAttacked = !lastAttacked;
}





float AngleDiff(float destAngle, float srcAngle) {
	float delta;

	delta = fmodf(destAngle - srcAngle, 360.0f);
	if (destAngle > srcAngle) {
		if (delta >= 180)
			delta -= 360;
	}
	else {
		if (delta <= -180)
			delta += 360;
	}
	return delta;
}


void Miscellaneous::Ragdoll()
{
	
	if (!g_Options.misc_exaggerated_ragdolls) return;
	for (int i = 1; i <= g_EntityList->GetHighestEntityIndex(); i++) {
		auto e = static_cast<C_BasePlayer *>(g_EntityList->GetClientEntity(i));
		// need code optimization
		if (!e)
			continue;

		if (e->IsDormant())
			continue;

		if (e->GetClientClass()->m_ClassID == ClassId_CCSRagdoll) {
			auto ragdoll = (C_BaseRagdoll*)e;

		
				Vector
					new_force = ragdoll->m_vecForce() * g_Options.misc_exaggerated_ragdolls_force,
					new_velocity = ragdoll->m_vecRagdollVelocity() * 10;

				ragdoll->m_vecForce() = new_force;
				ragdoll->m_vecRagdollVelocity() = new_velocity;
			
		}
	}
}

void Miscellaneous::LegitResolver(C_BasePlayer * enemy)
{

	bool can_resolve = true;
	auto animstate   = enemy->GetPlayerAnimState();
	if (!animstate)   return;
	if (g_EngineClient->IsInGame() && g_Options.misc_legit_resolver) {
		if (enemy->m_vecVelocity().Length2D() <= 0.15f)
		{
		
			float LowerBodyYaw = remainderf(enemy->m_flLowerBodyYawTarget(), 360.f);
			if (LowerBodyYaw - remainderf(enemy->m_angEyeAngles().yaw, 360.f) >= 60.f && can_resolve)
			{
				animstate->m_flGoalFeetYaw = Math::NormalizeYaw(remainderf(enemy->m_flLowerBodyYawTarget() + 180.f, 360.f));

				
			}
		}
	}

}
void Miscellaneous::CallLegitResolver()
{
	for (int i = 1; i < g_GlobalVars->maxClients; i++)
	{

		C_BasePlayer* player = C_BasePlayer::GetPlayerByIndex(i);


		if (!player)                 continue;

		if (!player->IsAlive())      continue;

		if (player == g_LocalPlayer) continue;

		LegitResolver(player);


	}
}

void Miscellaneous::SetThirdpersonAngles(ClientFrameStage_t stage)
{
	if (stage != FRAME_RENDER_START)
		return;

	if (g_EngineClient->IsInGame() && g_LocalPlayer)
	{
		if (g_LocalPlayer->IsAlive() && g_Input->m_fCameraInThirdPerson)
			g_LocalPlayer->SetSnakewareAngles(Snakeware::LocalAngle);
	}
}
void Miscellaneous::UpdateBreaker(CUserCmd * cmd)
{
	
	float curtime = g_GlobalVars->curtime;

	auto animstate = g_LocalPlayer->GetPlayerAnimState();
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive()) return;
	if (!animstate)                                  return;

	if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND))  return;

	if (animstate->speed_2d > 0.1f)
	{
		next_lby = curtime + 0.22f;
	}

	if (next_lby < curtime)
	{
		next_lby = curtime + 1.1f;
		
	}
}




void Miscellaneous::LegitAntiAim(CUserCmd *cmd)
{
	
	float minimal_move;
	bool should_move;
	static bool broke_lby = false;
	if (!g_Options.misc_legit_antihit)                                                               return;
	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();
	if (!weapon)                                                                                     return;
	if (cmd->buttons & IN_ATTACK)                                                                    return;
	if (((cmd->buttons & IN_ATTACK) || (cmd->buttons & IN_ATTACK2)) && weapon->m_fThrowTime() > 0.f) return;
	
	if (InputSys::Get().WasKeyPressed( g_Options.misc_legit_antihit_key) && !Menu::Get().IsVisible())
		g_Options.legit_desync_switch = !g_Options.legit_desync_switch;

	if (g_Options.misc_legit_antihit_lby)
	{
	
		UpdateBreaker(cmd);
		if (next_lby >= g_GlobalVars->curtime) {
			if (!broke_lby && Snakeware::bSendPacket && g_ClientState->iChokedCommands > 0)
				return;

			broke_lby = false;
			Snakeware::bSendPacket = false;
			if (g_Options.legit_desync_switch)
				cmd->viewangles.yaw += 120.f;
			else
				cmd->viewangles.yaw -= 120.f;
		}
		else {
			broke_lby = true;
			Snakeware::bSendPacket = false;
			if (g_Options.legit_desync_switch)
				cmd->viewangles.yaw -= 120.f;
			else
				cmd->viewangles.yaw += 120.f;
		}

	}
	else {
		minimal_move = 1.1f;
		if (g_LocalPlayer->m_fFlags() & FL_DUCKING)
			minimal_move *= 2.1f; // 3 old
		if (cmd->buttons & IN_WALK)
			minimal_move *= 2.1f; // 3 old too
		should_move = g_LocalPlayer->m_vecVelocity().Length2D() <= 0.f || std::fabsf(g_LocalPlayer->m_vecVelocity().z) <= 100.f;
		if (cmd->command_number % 2 == 1) {

			{    // start desyncing
				if (g_Options.legit_desync_switch)
					cmd->viewangles.yaw += 120.f;
				else 
					cmd->viewangles.yaw -= 120.f;
			} // set angles

			if (should_move)
				cmd->sidemove -= minimal_move;

			Snakeware::bSendPacket = false;
		}
		else if (should_move)
			cmd->sidemove += minimal_move;
	}


	Math::FixAngles(cmd->viewangles); // angles fix

}
#include "../../imgui/imgui.h"
#include "../../render.hpp"
#include "../ragebot/antihit/antihit.h"

int IsCheater() {

	const auto choked_ticks = g_ClientState->iChokedCommands;

	return choked_ticks;
}

void Miscellaneous::RenderIndicators()
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	ImGui::SetNextWindowSize(ImVec2{ 300, 200 }, ImGuiSetCond_Once);

	static bool watermark = true;


	int min_value;

	if (g_LocalPlayer->m_vecVelocity().Length2D() > 112)
	{
		min_value = 60 / (g_LocalPlayer->m_vecVelocity().Length2D() / 122);
	}
	else
	{
		min_value = 60;
	}
	int velocity = g_LocalPlayer->m_vecVelocity().Length2D();
	int desync = min_value;
	int spread = IsCheater();



	if (ImGui::Begin("Indicators_text", &watermark, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
		ImGui::SetCursorPos(ImVec2(25, 15 + 15));

		ImGui::BeginChild("Indicators", ImVec2(200 - 10, 165), true, ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoInputs);
		{
			ImGui::PushFont(g_pPixel);
			{


				ImGui::Text(" ");
				ImGui::SetCursorPosY(+26);

				ImGui::Text("Velocity");   // player speed
				ImGui::SameLine();
				ImGui::SliderIntNoText("Velocity", &velocity, 0, 250);
				ImGui::Text("Desync");     // player desync upd: get it from vel 
				ImGui::SameLine();
				ImGui::SliderIntNoText("Desync", &desync, 0, 60);
				ImGui::Text("Choked");     // Choked packets
				ImGui::SameLine();
				ImGui::SliderIntNoText("Spread", &spread, 0, 15);

			}
			ImGui::PopFont();
		}
		ImGui::EndChild();
	}
	ImGui::End();


}
void Miscellaneous::SpectatorList()
{
	if (!g_Options.misc_spectator_list)
		return;

	std::string spectators;

	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	for (int i = 1; i <= g_EngineClient->GetMaxClients(); i++)
		{

			auto ent = C_BasePlayer::GetPlayerByIndex(i);

			if (!ent || ent->IsAlive() || ent->IsDormant())
				continue;

			auto target = (C_BasePlayer*)ent->m_hObserverTarget();

			if (!target || target != g_LocalPlayer)
				continue;

			if (ent == target)
				continue;

			auto info = ent->GetPlayerInfo();

			spectators += std::string(info.szName) + "\n";
		}
	
	bool open = true;
	const auto size = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, NULL, spectators.c_str());
	ImGui::SetNextWindowSize(ImVec2{ 300, 200}, ImGuiSetCond_Once);

	if (ImGui::Begin("Spectators_lol", &open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {

		ImGui::SetCursorPos(ImVec2(25, 15 + 15));
		ImGui::BeginChild("Spectators", ImVec2(200 - 10, 110), true, ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoInputs);
		{
			ImGui::PushFont(g_pPixel);
			{

				int fps = g_GlobalVars->absoluteframetime;
				ImGui::Text(" ");
				ImGui::SetCursorPosY(+26);

				ImGui::SetCursorPos(ImVec2(ImGui::GetStyle().FramePadding.x, ImGui::GetStyle().FramePadding.y * 3 + ImGui::GetFontSize()));
				ImGui::Text(spectators.c_str());

			}
			ImGui::PopFont();
		}
		ImGui::EndChild();
	}
	ImGui::End();


}
void Miscellaneous::ClanTag()
{
	
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	
	
	static size_t lastTime = 0;

	if (GetTickCount() > lastTime)
	{
		snakeware += snakeware.at(0);
		snakeware.erase(0, 1);

		Utils::SetClantag(snakeware.c_str());

		lastTime = GetTickCount() + 650;
	}
	
	

}
void Miscellaneous::LeftKnife()
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (!g_LocalPlayer)	return;
	ConVar* cl_righthand = g_CVar->FindVar("cl_righthand");
	auto weapon = g_LocalPlayer->m_hActiveWeapon();
	if (!weapon) return;

	if (g_Options.misc_left_knife && weapon->IsKnife())
		cl_righthand->SetValue(0);
	
	else 
		cl_righthand->SetValue(1);
	


}
void Miscellaneous::FakeDuck (CUserCmd * cmd) {

	// From old snakeware aka subliminal paste
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())                                return;
	if (!g_LocalPlayer->m_fFlags() & FL_ONGROUND)                                   return;
	if (!g_Options.misc_fakeduck || !GetAsyncKeyState(g_Options.misc_fakeduck_key)) return;
	auto NetChannel = g_ClientState->pNetChannel;

	if (!NetChannel)
		return;

	int limit = g_Options.misc_fakeduck_ticks;
	int choked_goal = limit / 2;
	bool shouldCrouch = NetChannel->m_nChokedPackets >= choked_goal;

	if (shouldCrouch) {
		cmd->buttons |= IN_DUCK;
	
	}
	else {
		cmd->buttons &= ~IN_DUCK;
		
	}
	if (NetChannel->m_nChokedPackets < limit)
	{
		Snakeware::bSendPacket = false;
	}
	else
	{
		Snakeware::bSendPacket = true;
	}
	
}
void Miscellaneous::SlowWalk(CUserCmd * cmd)
{
	if (!g_Options.misc_slowwalk || !(GetAsyncKeyState(g_Options.misc_slowwalk_key)))
		return;

	auto speed = g_Options.misc_slowwalk_speed;


	float min_speed = (float)(Math::FASTSQRT((cmd->forwardmove)*(cmd->forwardmove) + (cmd->sidemove)*(cmd->sidemove) + (cmd->upmove)*(cmd->upmove)));
	if (min_speed <= 3.f) return;

	if (cmd->buttons & IN_DUCK)
		speed *= 2.94117647f;

	if (min_speed <= speed) return;

	float finalSpeed = (speed / min_speed);

	cmd->forwardmove *= finalSpeed;
	cmd->sidemove *= finalSpeed;
	cmd->upmove *= finalSpeed;
}


void Miscellaneous::ChatSpamer()
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	long curTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	static long timestamp = curTime;

	if ((curTime - timestamp) < 850)
		return;

	if (g_Options.misc_chat_spam)
	{
		if (spam.empty())
			return;

		std::srand(time(NULL));

		std::string msg = spam;

		std::string str;
		str.append("say ");
		str.append(msg);

		g_EngineClient->ExecuteClientCmd(str.c_str());
	}
	timestamp = curTime;
}



void Miscellaneous::BloomEffect()
{
	static int iBloomValue = 1;
	static int iScalarValue = 0.1;
	if (!g_Options.esp_bloom_effect) return;
	
	ConVar * mat_bloom = g_CVar->FindVar("mat_bloomscale");
	ConVar * mat_scale = g_CVar->FindVar("mat_bloom_scalefactor_scalar");
		if (iBloomValue != g_Options.esp_bloom_scale)
		{
			
			mat_bloom->SetValue(g_Options.esp_bloom_scale);
			iBloomValue = g_Options.esp_bloom_scale;
		}
		if (iBloomValue != g_Options.esp_bloom_factor)
		{
			
			mat_scale->SetValue(g_Options.esp_bloom_factor);
			iBloomValue = g_Options.esp_bloom_factor;
		}
	
}

void Miscellaneous::ModelAmbient()
{
	if (!g_Options.esp_bloom_effect) return;
	static auto ambient = g_CVar->FindVar("r_modelAmbientMin");
	ambient->SetValue(g_Options.esp_model_ambient);
}

void Miscellaneous::RenderRadar()
{
	if (!g_Options.misc_radar)
		return;

	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	float prevAlpha = ImGui::GetStyle().Alpha;

	bool radar = g_Options.misc_radar;
	float Alpha = 90; 

	ImGui::GetStyle().Alpha = Alpha;

	ImGui::SetNextWindowSize(ImVec2{ 300, 300 }, ImGuiSetCond_Once);

	if (ImGui::Begin("Radar_model", &radar, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
		ImGui::SetCursorPos(ImVec2(25, 15 + 15));

		ImGui::BeginChild("Radar", ImVec2(250, 250), true, ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoInputs);
		{


			

		}
		ImGui::EndChild();
	}
	ImGui::End();


	ImGui::GetStyle().Alpha = prevAlpha;
}

void Miscellaneous::RadarRange(float* x, float* y, float range)
{
	if (fabs((*x)) > range || fabs((*y)) > range)
	{
		if ((*y) > (*x))
		{
			if ((*y) > -(*x))
			{
				(*x) = range * (*x) / (*y);
				(*y) = range;
			}
			else
			{
				(*y) = -range * (*y) / (*x);
				(*x) = -range;
			}
		}
		else
		{
			if ((*y) > -(*x))
			{
				(*y) = range * (*y) / (*x);
				(*x) = range;
			}
			else
			{
				(*x) = -range * (*x) / (*y);
				(*y) = -range;
			}
		}
	}
}

void Miscellaneous::CalcRadarPoint(Vector vOrigin, int& screenx, int& screeny)
{
	QAngle vAngle;

	g_EngineClient->GetViewAngles(&vAngle);

	float dx = vOrigin.x - g_LocalPlayer->GetEyePos().x;
	float dy = vOrigin.y - g_LocalPlayer->GetEyePos().y;

	float fYaw = float(vAngle.yaw * (M_PI / 180.0));

	float fsin_yaw = sin(fYaw);
	float fminus_cos_yaw = -cos(fYaw);

	float x = dy * fminus_cos_yaw + dx * fsin_yaw;
	float y = dx * fminus_cos_yaw - dy * fsin_yaw;

	float range = g_Options.misc_radar_range; 

	RadarRange(&x, &y, range);

	ImVec2 DrawPos = ImGui::GetCursorScreenPos();
	ImVec2 DrawSize = ImGui::GetContentRegionAvail();

	int rad_x = (int)DrawPos.x;
	int rad_y = (int)DrawPos.y;

	float r_siz_x = DrawSize.x;
	float r_siz_y = DrawSize.y;

	int x_max = (int)r_siz_x + rad_x - 5;
	int y_max = (int)r_siz_y + rad_y - 5;

	screenx = rad_x + ((int)r_siz_x / 2 + int(x / range * r_siz_x));
	screeny = rad_y + ((int)r_siz_y / 2 + int(y / range * r_siz_y));

	if (screenx > x_max)
		screenx = x_max;

	if (screenx < rad_x)
		screenx = rad_x;

	if (screeny > y_max)
		screeny = y_max;

	if (screeny < rad_y)
		screeny = rad_y;
}
void Miscellaneous::InfinityDuck(CUserCmd * cmd)
{   // check's
	if (!g_LocalPlayer) return;
	if (!g_EngineClient->IsInGame()) return;
	if (!g_EngineClient->IsConnected()) return;


	// func
	cmd->buttons |= IN_BULLRUSH;
}
void Miscellaneous::OnRenderPlayer()
{
	float Alpha = 150;

	for (size_t i = 0; i < g_EntityList->GetHighestEntityIndex(); ++i)
	{
		if (!g_EngineClient->IsInGame())
			continue;

		C_BasePlayer* player = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));

		if (!player || !player->IsPlayer() || player == g_LocalPlayer || player->IsDormant() || !player->IsAlive() || !player->IsEnemy())
			continue;

		int screenx = 0, screeny = 0;
		float Color_R = 255;
		float Color_G = 0;
		float Color_B = 0;

		CalcRadarPoint(player->GetRenderOrigin(), screenx, screeny);

		ImDrawList* Draw = ImGui::GetWindowDrawList();

		Draw->AddRectFilled(ImVec2((float)screenx, (float)screeny),
			ImVec2((float)screenx + 5, (float)screeny + 5),
			ImColor(Color_R, Color_G, Color_B, Alpha));
	}
}




void Miscellaneous::JumpThrow(CUserCmd * cmd)
{
	// by @Blick1337 from midnight.im
	// C + P
	if (!g_LocalPlayer)
		return;

	auto pWeapon = g_LocalPlayer->m_hActiveWeapon();

	if (!pWeapon)
		return;

	if  (g_Options.misc_jump_throw && pWeapon->IsGrenade() && !(cmd->buttons & IN_ATTACK) && !(g_LocalPlayer->m_fFlags() & FL_ONGROUND) && g_LocalPlayer->m_vecVelocity().z > 265.f)
	{
		cmd->buttons |= IN_ATTACK;
	}
}

void Miscellaneous::FakeFps()
{
	if (!g_Options.misc_fake_fps) return;
	g_GlobalVars->absoluteframetime = 1.f / 1338.f;
}

void Miscellaneous::Binds()
{

	bool open = false;
	ImGui::SetNextWindowSize(ImVec2{ 300, 200 }, ImGuiSetCond_Once);

	if (ImGui::Begin("Binds_lol", &open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {

		ImGui::SetCursorPos(ImVec2(25, 15 + 15));
		ImGui::BeginChild("Binds", ImVec2(200 - 10, 110), true, ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoInputs);
		{
			ImGui::PushFont(g_pPixel);
			{

				int fps = g_GlobalVars->absoluteframetime;
				ImGui::Text(" ");
				ImGui::SetCursorPosY(+26);

				if (GetAsyncKeyState(g_Options.misc_slowwalk_key))
				{
					if (g_Options.misc_slowwalk)
					{
						ImGui::Text("Slowwalk");
						ImGui::SameLine();	
						ImGui::SetCursorPosX(140);
						ImGui::Text("[Hold]");
					}
				}
					

				if (GetAsyncKeyState(g_Options.misc_fakeduck_key))
				{
					if (g_Options.misc_fakeduck)
					{
						ImGui::Text("Fakeduck");
						ImGui::SameLine();
						ImGui::SetCursorPosX(140);
						ImGui::Text("[Hold]");
					}
				}

				if (GetAsyncKeyState(g_Options.exploit_doubletap_key))
				{
					if (g_Options.exploit_doubletap)
					{
						ImGui::Text("Fastfire");
						ImGui::SameLine();
						ImGui::SetCursorPosX(140);
						ImGui::Text("[Hold]");
					}
				}

				if (AntiHit::Get().switchSide)
				{
					if (g_Options.antihit_enabled)
					{
						ImGui::Text("Desync switched");
						ImGui::SameLine();
						ImGui::SetCursorPosX(140);
						ImGui::Text("[Toogle]");
					}
				}


				if (GetAsyncKeyState(g_Options.ragebot_force_safepoint))
				{
					
						ImGui::Text("Force safe-point");
						ImGui::SameLine();
						ImGui::SetCursorPosX(140);
						ImGui::Text("[Hold]");
					
				}


			}
			ImGui::PopFont();
		}
		ImGui::EndChild();
	}
	ImGui::End();
}



Vector quickPeekStartPos;
bool   hasShot;

void Miscellaneous::AutoPeekStart(CUserCmd* cmd) {

	Vector playerLoc = g_LocalPlayer->m_angAbsOrigin();

	float Yaw         = cmd->viewangles.yaw;
	Vector VecForward = playerLoc - quickPeekStartPos;

	Vector translatedVelocity = Vector {
		(float)(VecForward.x * cos(Yaw / 180 * (float)M_PI) + VecForward.y * sin(Yaw / 180 * (float)M_PI)),
		(float)(VecForward.y * cos(Yaw / 180 * (float)M_PI) - VecForward.x * sin(Yaw / 180 * (float)M_PI)),
		VecForward.z
	};

	cmd->forwardmove = -translatedVelocity.x * 20.f;
	cmd->sidemove    =  translatedVelocity.y * 20.f;
}

void Miscellaneous::AutoPeek(CUserCmd* cmd) {

	if (!g_LocalPlayer || g_LocalPlayer->IsDormant() || !g_LocalPlayer->IsAlive()) return;

	if (GetAsyncKeyState(g_Options.autopeek_key)) {
		if (quickPeekStartPos == Vector(0, 0, 0)) {

			quickPeekStartPos = g_LocalPlayer->m_angAbsOrigin();
		}
		else {
			if (cmd->buttons & IN_ATTACK) 
				hasShot = true;

			if (hasShot) {
				AutoPeekStart(cmd);
			}
		}
	}
	else {
		hasShot = false;
		quickPeekStartPos = Vector{ 0, 0, 0 };
	}
}

void Miscellaneous::SilentWalk(CUserCmd * cmd) {
	
	if (!g_Options.misc_silentwalk_key || !g_LocalPlayer || !g_LocalPlayer->IsAlive()) return;

	Vector moveDir = Vector(0.f, 0.f, 0.f);
	float maxSpeed = 130.f; // Can be a 134..
	int movetype = g_LocalPlayer->m_nMoveType();
	bool InAir = !(g_LocalPlayer->m_fFlags() & FL_ONGROUND);
	if (movetype == MOVETYPE_FLY || movetype == MOVETYPE_NOCLIP || InAir || cmd->buttons & IN_DUCK || !(cmd->buttons & IN_SPEED))
		return;
	// Move dir's setup
	moveDir.x = cmd->sidemove;
	moveDir.y = cmd->forwardmove;
	moveDir   = Math::ClampMagnitude(moveDir, maxSpeed);

	// Move's setup
	cmd->sidemove = moveDir.x;
	cmd->forwardmove = moveDir.y;
	if (!(g_LocalPlayer->m_vecVelocity().Length2D() > maxSpeed + 1))
		cmd->buttons &= ~IN_SPEED;
}