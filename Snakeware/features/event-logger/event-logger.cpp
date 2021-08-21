#include "event-logger.h"
#include "../../helpers/math.hpp"
#include "../ragebot/ragebot.h"






void EventLogs::Draw() {
	if (logs.empty()) return;

	auto last_y = 6;


	for (unsigned int i = 0; i < logs.size(); i++) {
		auto & log = logs.at(i);

		if (Utils::EpochTime() - log.log_time > 2700) {
			float factor = (log.log_time + 3100) - Utils::EpochTime();
			factor /= 1000;

			auto opacity = int(255 * factor);

			if (opacity < 2) {
				logs.erase(logs.begin() + i);
				continue;
			}

			

			log.x += 1 * (factor * 1.25);
			log.y += 1 * (factor * 1.25);
		}

		const auto text = log.message.c_str();
		const auto pidoras = ("Insight");
		
		auto sz = g_pDefaultFont->CalcTextSizeA(16.f, FLT_MAX, 0.0f, pidoras);
	

		Render::Get().RenderText(pidoras, log.x, last_y + log.y, 16.f,Color(255,0,255,255));
		Render::Get().RenderText(text, log.x + 3 + sz.x, last_y + log.y, 16.f, log.color);

		last_y += 14;
	}
}


int EventLogs::GetEventDebugID(void)
{
	return EVENT_DEBUG_ID_INIT;
}

void EventLogs::RegisterSelf()
{
	g_GameEvents->AddListener(this, "bullet_impact", false);
	g_GameEvents->AddListener(this, "player_hurt", false);
	g_GameEvents->AddListener(this, "round_prestart", false);
	g_GameEvents->AddListener(this, "round_freeze_end", false);
	g_GameEvents->AddListener(this, "item_purchase", false);
	g_GameEvents->AddListener(this, "bomb_beginplant", false);
	g_GameEvents->AddListener(this, "bomb_begindefuse", false);
}

void EventLogs::UnregisterSelf()
{
	g_GameEvents->RemoveListener(this);
}


void EventLogs::FireGameEvent(IGameEvent * event) {
	static auto get_hitgroup_name = [](int hitgroup) -> std::string {
		switch (hitgroup) {
		case HITGROUP_HEAD:
			return "head";
		case HITGROUP_LEFTLEG:
			return "left leg";
		case HITGROUP_RIGHTLEG:
			return "right leg";
		case HITGROUP_STOMACH:
			return "stomach";
		case HITGROUP_LEFTARM:
			return "left arm";
		case HITGROUP_RIGHTARM:
			return "right arm";
		default:
			return "body";
		}
	};

	constexpr char * hasdefusekit[2] = { "without defuse kit.","with defuse kit." };
	constexpr char * hasbomb[2] = { "without the bomb.","with the bomb." };
	constexpr char * hitgroups[7] = { "head" };
	


	if (strstr(event->GetName(), "bullet_impact")) {
//	   Resolver::Get().OnBulletImpact(event);
	}



	if (g_Options.event_log_hit && strstr(event->GetName(), "player_hurt")) {
		auto
			userid = event->GetInt("userid"),
			attacker = event->GetInt("attacker");

		if (!userid || !attacker) {
			return;
		}

		auto
			userid_id = g_EngineClient->GetPlayerForUserID(userid),
			attacker_id = g_EngineClient->GetPlayerForUserID(attacker);

		player_info_t userid_info, attacker_info;
		if (!g_EngineClient->GetPlayerInfo(userid_id, &userid_info)) {
			return;
		}

		if (!g_EngineClient->GetPlayerInfo(attacker_id, &attacker_info)) {
			return;
		}
		

		auto m_victim = static_cast<C_BasePlayer *>(g_EntityList->GetClientEntity(userid_id));

		std::stringstream ss;
		if (attacker_id == g_EngineClient->GetLocalPlayer()) {
			ss << "you hurt " << userid_info.szName << " in the " << get_hitgroup_name(event->GetInt("hitgroup")) << " for " << event->GetInt("dmg_health");
			ss << " ( " << Math::Clamp(m_victim->m_iHealth() - event->GetInt("dmg_health"), 0, 100) << " health remaining )";
			Add(ss.str(), Color::White);

		}
		else if (userid_id == g_EngineClient->GetLocalPlayer()) {
			ss << "you took " << event->GetInt("dmg_health") << " damage from " << attacker_info.szName << "in the " << get_hitgroup_name(event->GetInt("hitgroup"));
			ss << " ( " << Math::Clamp(m_victim->m_iHealth() - event->GetInt("dmg_health"), 0, 100) << " health remaining )";

			Add(ss.str(), Color::White);
		}
	}

	if (g_Options.event_log_item && strstr(event->GetName(), "item_purchase")) {
		auto userid = event->GetInt("userid");

		if (!userid) {
			return;
		}

		std::stringstream ss;

		auto userid_id = g_EngineClient->GetPlayerForUserID(userid);

		player_info_t userid_info;
		if (!g_EngineClient->GetPlayerInfo(userid_id, &userid_info)) {
			return;
		}

		auto m_player = static_cast<C_BasePlayer *>(g_EntityList->GetClientEntity(userid_id));

		if (!g_LocalPlayer || !m_player) {
			return;
		}

		if (m_player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum()) {
			return;
		}

		
		ss << userid_info.szName << " purchased a(n) " << event->GetString("weapon");

		Add(ss.str(), Color::White);
	}

	

	if (g_Options.event_log_plant && strstr(event->GetName(), "bomb_beginplant")) {
		auto userid = event->GetInt("userid");

		if (!userid) {
			return;
		}

		auto userid_id = g_EngineClient->GetPlayerForUserID(userid);

		player_info_t userid_info;
		if (!g_EngineClient->GetPlayerInfo(userid_id, &userid_info)) {
			return;
		}

		auto m_player = static_cast<C_BasePlayer *>(g_EntityList->GetClientEntity(userid_id));

		if (!m_player) {
			return;
		}

		std::stringstream ss;
		ss << userid_info.szName << " has began planting the bomb at site " << event->GetInt("site") << ".";

		Add(ss.str(), Color::White);
	}

	if (g_Options.event_log_defuse && strstr(event->GetName(), "bomb_begindefuse")) {
		auto userid = event->GetInt("userid");

		if (!userid) {
			return;
		}

		auto userid_id = g_EngineClient->GetPlayerForUserID(userid);

		player_info_t userid_info;
		if (!g_EngineClient->GetPlayerInfo(userid_id, &userid_info)) {
			return;
		}

		auto m_player = static_cast<C_BasePlayer *>(g_EntityList->GetClientEntity(userid_id));

		if (!m_player) {
			return;
		}

		std::stringstream ss;
		ss << userid_info.szName << " has began defusing the bomb " << hasdefusekit[event->GetBool("haskit")];

		Add(ss.str(), Color::White);
	}
}
void EventLogs::Add(std::string text, Color color) {
	logs.push_front(loginfo_t(Utils::EpochTime(), text, color));
}
