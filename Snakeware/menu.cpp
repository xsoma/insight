#include "Menu.hpp"
#define NOMINMAX
#include <Windows.h>
#include <chrono>

#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "options.hpp"
#include "ui.hpp"
#include "config.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "imgui/impl/imgui_impl_dx9.h"
#include "imgui/impl/imgui_impl_win32.h"
#include "imgui/imgui.h"
#include "imgui/smoke.h"

// config
#include "conifg-system/config-system.h"
#include <filesystem>
#include "fonts/Main_googlefont.h"
#include "Protected/enginer.h"
#include "render.hpp"
#include "Lua/API.h"
namespace fs = std::filesystem;




namespace ImGuiEx
{
	inline bool ColorEdit1337(const char* label, Color* v, bool show_alpha = true)
	{
		auto clr = ImVec4{
			v->r() / 255.0f,
			v->g() / 255.0f,
			v->b() / 255.0f,
			v->a() / 255.0f
		};

		if (ImGui::ColorEdit4(label, &clr.x, show_alpha)) {
			v->SetColor(clr.x, clr.y, clr.z, clr.w);
			return true;
		}
		return false;
	}
	inline bool ColorEdit3(const char* label, Color* v)
	{
		return ColorEdit1337(label, v, false);
	}
}

template<size_t N>
void render_tabs(char* (&names)[N], int& activetab, float w, float h, bool sameline)
{
	bool values[N] = { false };

	values[activetab] = true;

	for (auto i = 0; i < N; ++i) {
		if (ImGui::ToggleButton(names[i], &values[i], ImVec2{ w, h })) {
			activetab = i;
		}
		if (sameline && i < N - 1)
			ImGui::SameLine();
	}
}


//==============================================================================================================
static int weapon_index = 7;
static int weapon_vector_index = 0;
struct WeaponName_t {
	constexpr WeaponName_t(int32_t definition_index, const char* name) :
		definition_index(definition_index),
		name(name) {
	}

	int32_t definition_index = 0;
	const char* name = nullptr;
};

std::vector< WeaponName_t> WeaponNames =
{

{ 0 ,"none"},
{ 7, "ak-47" },
{ 8, "aug" },
{ 9, "awp" },
{ 63, "cz75 auto" },
{ 1, "desert-eagle" },
{ 2, "dual-berettas" },
{ 10, "famas" },
{ 3, "five-seveN" },
{ 11, "g3sg1" },
{ 13, "galil ar" },
{ 4, "glock-18" },
{ 14, "m249" },
{ 60, "m4a1-s" },
{ 16, "m4a4" },
{ 17, "mac-10" },
{ 27, "mag-7" },
{ 33, "mp7" },
{ 23, "mp5" },
{ 34, "mp9" },
{ 28, "negev" },
{ 35, "nova" },
{ 32, "p2000" },
{ 36, "p250" },
{ 19, "p90" },
{ 26, "pp-bizon" },
{ 64, "r8 revolver" },
{ 29, "sawed-Off" },
{ 38, "scar20" },
{ 40, "ssg-08" },
{ 39, "sg 553" },
{ 30, "tec-9" },
{ 24, "ump-45" },
{ 61, "usp-s" },
{ 25, "xm1014" }

};

void RenderCurrentWeaponButton(float width) {

	if (!g_EngineClient->IsInGame() || !g_LocalPlayer || !g_LocalPlayer->IsAlive()) return;
	auto weapon = g_LocalPlayer->m_hActiveWeapon();
	if (!weapon)   return;
	ImGui::SameLine();
	if (!g_Options.legitbot_auto_weapon) {
		if (ImGui::Button("current")) {
			short wpn_idx = weapon->m_Item().m_iItemDefinitionIndex();
			auto wpn_it = std::find_if(WeaponNames.begin(), WeaponNames.end(), [wpn_idx](const WeaponName_t& a) {
				return a.definition_index == wpn_idx;
				});
			if (wpn_it != WeaponNames.end()) {
				weapon_index = wpn_idx;
				weapon_vector_index = std::abs(std::distance(WeaponNames.begin(), wpn_it));
			}
		}
	}
	else
	{
		short wpn_idx = weapon->m_Item().m_iItemDefinitionIndex();
		auto wpn_it = std::find_if(WeaponNames.begin(), WeaponNames.end(), [wpn_idx](const WeaponName_t& a) {
			return a.definition_index == wpn_idx;
			});
		if (wpn_it != WeaponNames.end()) {
			weapon_index = wpn_idx;
			weapon_vector_index = std::abs(std::distance(WeaponNames.begin(), wpn_it));
		}
	}
}
// =====================================================================================================================


void RenderWatermark()
{
	if (!g_Options.misc_watermark) return;
	ImGui::SetNextWindowSize(ImVec2{ 150, 30 }, ImGuiSetCond_Once);

	static bool watermark = true;

	if (ImGui::Begin(" by @snake | @ba1m0v ", &watermark, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {
		ImGui::Text("Insight [alpha] | CS:GO");
	}
	ImGui::End();


}
void RenderRageBotTab()
{
	//  SUB TABS  // 

	static int SubTabs = 1;

	ImGuiEX::SubTabButton("Ragebot", &SubTabs, 0, 4);
	ImGuiEX::SubTabButton("AA", &SubTabs, 1, 4);
	ImGuiEX::SubTabButton("Legit", &SubTabs, 2, 4);
	ImGuiEX::SubTabButton("Other", &SubTabs, 3, 4);



	//  SUB END  //

	switch (SubTabs)
	{
	case 0:
	{
		bool placeholder_true = true;

		auto& style = ImGui::GetStyle();
		float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

		ImGui::SetCursorPos(ImVec2(22, 100));

		ImGui::BeginChild("Ragebot", ImVec2(310, 360), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);
			ImGui::CheckboxEX("Enabled##rage", &g_Options.ragebot_enabled, "Enabled ragebot", 1);
			const char* selection[] = { "Fov based","Health based", "Distance based" };
			ImGui::Combo("Target selection", &g_Options.ragebot_selection, selection, IM_ARRAYSIZE(selection));
			ImGui::CheckboxEX("Auto fire##rage", &g_Options.ragebot_autofire, "Automaticaly fire", 2);
			ImGui::CheckboxEX("Auto wall##rage", &g_Options.ragebot_autowall, "Automaticaly penerate", 3);
			ImGui::CheckboxEX("Silent aim##rage", &g_Options.ragebot_silent, "Enabled silent ragebot", 4);
			ImGui::CheckboxEX("No recoil##rage", &g_Options.ragebot_remove_recoil, "Compensate recoil", 5);
			ImGui::SliderInt("Ragebot fov", &g_Options.ragebot_fov, 0, 180);
			ImGui::CheckboxEX("Resolver", &g_Options.ragebot_resolver, "Fix enemy fake angle", 5);
			ImGui::Text("Force safe-point  :");
			ImGuiEX::Hotkey("##forcesp	  ", &g_Options.ragebot_force_safepoint, ImVec2(150, 20));

			ImGui::Text("Force body key   :");
			ImGuiEX::Hotkey("##forcebodykey ", &g_Options.ragebot_baim_key, ImVec2(150, 20));
		
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(22 + 310 + 17 + 10, 100));


		const char* weapon[] = { "Pistols","Rifles","SMG","Shotguns","Auto","Scout","AWP","Other" };

		static int curGroup;

		ImGui::BeginChild("Weapon", ImVec2(310, 118), true);
		{



			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);

		
			ImGui::PushFont(g_pCSGO_icons);
			{

				if (ImGui::Button("D", ImVec2(44 + 8, 22)))
					curGroup = WEAPON_GROUPS::AUTO;
				ImGui::SameLine();
				if (ImGui::Button("F", ImVec2(44 + 8, 22)))
					curGroup = WEAPON_GROUPS::SCOUT;
				ImGui::SameLine();
				if (ImGui::Button("g", ImVec2(44 + 8, 22)))
					curGroup = WEAPON_GROUPS::AWP;
				ImGui::SameLine();
				if (ImGui::Button("R", ImVec2(44 + 8, 22)))
					curGroup = WEAPON_GROUPS::H_PISTOLS;
				ImGui::SameLine();
				if (ImGui::Button("b", ImVec2(44 + 8, 22)))
					curGroup = WEAPON_GROUPS::PISTOLS;

			}
			ImGui::PopFont();

			ImGui::SliderFloat("Hit-chance", &g_Options.ragebot_hitchance[curGroup], 0, 99);
			ImGui::SliderInt("Min-damage", &g_Options.ragebot_mindamage[curGroup], 0, 120);
			ImGui::SliderInt("Vis-damage", &g_Options.ragebot_vis_mindamage[curGroup], 0, 140);


		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(22 + 310 + 17 + 10, 100 + 118 + 17 + 10));

		ImGui::BeginChild("Settings", ImVec2(310, 210), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);
			ImGui::CheckboxEX("Multipoint hitboxes", &g_Options.ragebot_multipoint, "Multipoint hitboxes etc", 6);
			if (g_Options.ragebot_multipoint) {


				ImGui::CheckboxEX("Static-pointscale", &g_Options.ragebot_static_pointscale[curGroup], "Multipoint hitboxes etc", 6);

				if (g_Options.ragebot_static_pointscale[curGroup]) {
					ImGui::SliderFloat("Head scale", &g_Options.ragebot_pointscale[curGroup], 0, 100);
					ImGui::SliderFloat("Body scale", &g_Options.ragebot_bodyscale[curGroup], 0, 100);
					ImGui::SliderFloat("Other scale", &g_Options.ragebot_otherscale[curGroup], 0, 100);
				}
				else {
					ImGui::Text("Dynamic-pointscale : enabled");
				}
			}
			ImGui::CheckboxEX("Smart body-aim", &g_Options.ragebot_adaptive_baim[curGroup], "Smart baim", 7);
			ImGui::CheckboxEX("Body-aim if lethal", &g_Options.ragebot_baim_if_lethal[curGroup], "Smart baim", 7);
			ImGui::CheckboxEX("Auto-scope", &g_Options.ragebot_autoscope, "Automaticaly scope", 8);
			ImGui::CheckboxEX("Auto-stop", &g_Options.ragebot_autostop, "Automaticaly stop", 9);
			const char* hitboxes[] = { "Head", "Neck", "Chest", "Body", "Arms", "Legs" };
			static std::string prevValue = "Select";
			if (ImGui::BeginCombo("Hitscan", "Select", 0))
			{
				//prevValue = "Hitscan";
				std::vector<std::string> vec;
				for (size_t i = 0; i < IM_ARRAYSIZE(hitboxes); i++)
				{
					ImGui::Selectable(hitboxes[i], &g_Options.ragebot_hitbox[i][curGroup], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
					if (g_Options.ragebot_hitbox[i][curGroup])
						vec.push_back(hitboxes[i]);
				}

				for (size_t i = 0; i < vec.size(); i++)
				{
					if (vec.size() == 1)
						prevValue += vec.at(i);
					else if (i != vec.size())
						prevValue += vec.at(i) + ", ";
					else
						prevValue += vec.at(i);
				}
				ImGui::EndCombo();
			}
		}
		ImGui::EndChild();

	}
	break;
	case 1:
	{
		bool placeholder_true = true;

		auto& style = ImGui::GetStyle();
		float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

		ImGui::SetCursorPos(ImVec2(22, 100));

		ImGui::BeginChild("General", ImVec2(310, 360), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);

			ImGui::CheckboxEX("Enabled##aa", &g_Options.antihit_enabled, "Changing ur angles to be anhitable", 9);
			ImGui::CheckboxEX("Stabilize lby", &g_Options.antihit_stabilize_lby, "Lower body yaw stab", 10);
			const char* pitch[] = {  "Down","Emotion","Zero","Up","Fake up", "Fake down" };
			const char* yaw[] = { "Backward's","Manual's" };
			const char* fake[] = { "Static","Lagsync" };
			const char* jitter[] = { "Default","Switch" };
			const char* lby[] = { "Default","Opposite","Sway","Low-delta" };
			ImGui::Combo("Pitch", &g_Options.antihit_pitch, pitch, IM_ARRAYSIZE(pitch));
			ImGui::Combo("Yaw", &g_Options.antihit_yaw, yaw, IM_ARRAYSIZE(yaw));
			ImGui::Combo("Fake", &g_Options.antihit_fake, fake, IM_ARRAYSIZE(fake));
			ImGui::Combo("Jitter", &g_Options.antihit_jitter_type, jitter, IM_ARRAYSIZE(jitter));
			ImGui::Combo("Lby-type", &g_Options.antihit_lby, lby, IM_ARRAYSIZE(lby));



		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(22 + 310 + 17 + 10, 100));

		ImGui::BeginChild("Customization", ImVec2(310, 360), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);
			ImGui::SliderFloat("Fake ammount", &g_Options.antihit_fake_ammount, 0, 65);
			ImGui::SliderInt("Jitter-radius", &g_Options.antihit_jitter_radius, 0, 120);
			ImGui::SliderInt("Body-lean", &g_Options.antihit_body_lean, 0, 69);
			ImGui::SliderInt("Inverted body-lean", &g_Options.antihit_invert_body_lean, 0, 69);
			ImGui::Text("Key-bind's :");
			ImGui::Text("Switch side  :");
			ImGuiEX::Hotkey("##fakekey	  ", &g_Options.antihit_fake_switch_key, ImVec2(150, 20));
			ImGui::Text("Manual-mode antiaim :");
			ImGui::Text("Left:");
			ImGuiEX::Hotkey("##leftaa	  ", &g_Options.antihit_manual_left, ImVec2(150, 20));

			ImGui::Text("Right:");
			ImGuiEX::Hotkey("##rightaa	  ", &g_Options.antihit_manual_right, ImVec2(150, 20));
			ImGui::Text("Back:");
			ImGuiEX::Hotkey("##backaa	  ", &g_Options.antihit_manual_back, ImVec2(150, 20));
						//   Manualback	  
		}
		ImGui::EndChild();


	}
	break;
	case 2:
	{
		bool placeholder_true = true;

		auto& style = ImGui::GetStyle();
		float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

		static char* priorities[] = {
		"Fov",
		"Health",
		"Damage",
		"Distance"
		};

		static char* aim_types[] = {
			"hitbox",
			"nearest"
		};
		static char* rcs_types[] = {
					"standalone",
					"on aim"
		};
		static char* smooth_types[] = {
			"static",
			"adaptive"
		};

		static char* fov_types[] = {
			"static",
			"adaptive"
		};

		static char* hitbox_list[] = {
			"head",
			"neck",
			"pelvis",
			"stomach",
			"lower chest",
			"chest",
			"upper chest",
		};


		ImGui::SetCursorPos(ImVec2(22, 100));

		ImGui::BeginChild("General", ImVec2(310, 118), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);

			if (ImGui::Combo(
				"##Weapon_aimbot", &weapon_vector_index,
				[](void* data, int32_t idx, const char** out_text) {
					auto vec = reinterpret_cast<std::vector< WeaponName_t >*>(data);
					*out_text = vec->at(idx).name;
					return true;
				}, (void*)(&WeaponNames), WeaponNames.size())) {
				weapon_index = WeaponNames[weapon_vector_index].definition_index;
			}

			RenderCurrentWeaponButton(1337);
			ImGui::CheckboxEX("Auto", &g_Options.legitbot_auto_weapon, "Auto-selection", 11);



		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(22, 100 + 118 + 17 + 10));


		auto settings = &g_Options.legitbot_items[weapon_index];


		ImGui::BeginChild("Settings", ImVec2(310, 210), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);

			ImGui::Text("General-legitbot");
			ImGui::CheckboxEX("Enabled", &settings->enabled, "Aim for valve servers", 12);
			ImGui::CheckboxEX("Friendly-fire", &settings->deathmatch, "Aiming for teemate", 13);
			ImGui::CheckboxEX("Backtrack", &settings->backtrack, "Returns the player in time", 14);
			ImGui::CheckboxEX("Silent-aim", &settings->silent, "NoVisible aim", 15);
			ImGui::Text("Check's");
			ImGui::CheckboxEX("Smoke-check", &settings->smoke_check, "NoAim in smoke", 16);
			ImGui::CheckboxEX("Flash-check ", &settings->flash_check, "NoAim while flashed",17);
			ImGui::CheckboxEX("Jump-check", &settings->jump_check, "NoAim while jump", 18);
			if (weapon_index == WEAPON_AWP || weapon_index == WEAPON_SSG08 ||
				weapon_index == WEAPON_AUG || weapon_index == WEAPON_SG553) {
				ImGui::CheckboxEX("In-zoom check", &settings->only_in_zoom, "Aim only with scope", 19);
			}
			if (weapon_index == WEAPON_P250 ||
				weapon_index == WEAPON_USPS ||
				weapon_index == WEAPON_GLOCK ||
				weapon_index == WEAPON_FIVESEVEN ||
				weapon_index == WEAPON_TEC9 ||
				weapon_index == WEAPON_DEAGLE ||
				weapon_index == WEAPON_ELITE ||
				weapon_index == WEAPON_P2000) {
				ImGui::CheckboxEX("Auto-pistol", &settings->autopistol, "RifleFire for pistols", 20);
			}



		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(22 + 310 + 17 + 10, 100));

		ImGui::BeginChild("Customization", ImVec2(310, 360), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);

			ImGui::CheckboxEX("Only on key", &settings->on_key, "Aimbot on key", 21);

			if (settings->on_key) {
				ImGui::Text("Bind");
				ImGui::SameLine();
				ImGuiEX::Hotkey("##Keybind", &settings->key, ImVec2(150, 20));
			}


			ImGui::Text("Legitbot setting's");

			ImGui::Combo("Aim-type", &settings->aim_type, aim_types, IM_ARRAYSIZE(aim_types));

			if (settings->aim_type == 0) {
				ImGui::Text("Hitbox:");
				ImGui::Combo("##aimbot.hitbox", &settings->hitbox, hitbox_list, IM_ARRAYSIZE(hitbox_list));
			}

			ImGui::Combo("Priority", &settings->priority, priorities, IM_ARRAYSIZE(priorities));

			ImGui::Combo("Fov-type", &settings->fov_type, fov_types, IM_ARRAYSIZE(fov_types));
			ImGui::Combo("Smooth-type", &settings->smooth_type, smooth_types, IM_ARRAYSIZE(smooth_types));
			if (settings->silent) {
				ImGui::SliderFloat("Silent fov", &settings->silent_fov, 0, 40);
				settings->fov = 0;
			}
			else
			{
				ImGui::SliderFloat("Fov", &settings->fov, 0, 40);
			}



			ImGui::SliderFloat("Smooth", &settings->smooth, 0, 25);

			ImGui::Text("delay's");
			if (!settings->silent) {
				ImGui::SliderInt("Shot delay", &settings->shot_delay, 0, 200);
			}

			ImGui::SliderInt("Kill delay", &settings->kill_delay, 0, 1001);
			if (settings->backtrack)
				ImGui::SliderFloat("Backtrack-time", &settings->backtrack_time, 0.f, 0.4f);

			ImGui::Text("Secoil setting's");

			ImGui::CheckboxEX("Auto recoil-control", &settings->rcs, "Auto-recoil control", 22);
			ImGui::Combo("Recoil type", &settings->rcs_type, rcs_types, IM_ARRAYSIZE(rcs_types));

			ImGui::SliderInt("Rcs x: ", &settings->rcs_x, 0, 100);
			ImGui::SliderInt("Rcs y: ", &settings->rcs_y, 0, 100);
			ImGui::SliderInt("Rcs start bullet: ", &settings->rcs_start, 1, 29);
			ImGui::Text("Recoil custom setting's");
			ImGui::CheckboxEX("Rcs custom-fov", &settings->rcs_fov_enabled, "Fov in recoil",23);
			if (settings->rcs_fov_enabled) {
				ImGui::SliderFloat("Rcs fov", &settings->rcs_fov, 0, 30);
			}
			ImGui::CheckboxEX("Rcs custom-smooth", &settings->rcs_smooth_enabled, "Smooth in recoil", 24);
			if (settings->rcs_smooth_enabled) {
				ImGui::SliderFloat("Rcs smooth", &settings->rcs_smooth, 1, 25);
			}


		}
		ImGui::EndChild();


	}
	break;
	case 3:
	{
		bool placeholder_true = true;

		auto& style = ImGui::GetStyle();
		float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;



		ImGui::SetCursorPos(ImVec2(22, 100));

		auto settings = &g_Options.legitbot_items[weapon_index];

		ImGui::BeginChild("General", ImVec2(310, 360), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);

			ImGui::Text("General-triggerbot");
			ImGui::CheckboxEX("Trigger", &settings->triggerbot_enable, "Autoshoot when player on crosshair", 25);
			if (settings->triggerbot_enable) {

				ImGui::Text("Trigger");
				ImGui::SameLine();
				ImGuiEX::Hotkey("##Triggerkeybind", &g_Options.legitbot_trigger_key, ImVec2(150, 20));
			}
			ImGui::Text("Trigger-bot setting's");
			{
				ImGui::SliderInt("Delay : ", &settings->triggerbot_delay, 0, 250);
				ImGui::SliderFloat("Hitchance :", &settings->triggerbot_hitchance, 0, 100);
			}
			ImGui::Text("Trigger-bot hitboxe's");
			{
				ImGui::CheckboxEX("Hit head", &settings->triggerbot_head, "Trigger head", 26);
				ImGui::CheckboxEX("Hit body", &settings->triggerbot_body, "Trigger body", 27);
				ImGui::CheckboxEX("Hit other-hitboxes", &settings->triggerbot_other, "Trigger legs,arms", 28);
			}



		}
		ImGui::EndChild();



	}
	break;



	}


}
ImU32 GetU32(Color _color)
{
	return ((_color[3] & 0xff) << 24) + ((_color[2] & 0xff) << 16) + ((_color[1] & 0xff) << 8)
		+ (_color[0] & 0xff);
}
template <class T>
inline void RenderLine(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f) {
	ImGui::GetWindowDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), GetU32(color), thickness);
}

template <class T>
inline void RenderCoalBox(T x1, T y1, T x2, T y2, Color color, float th = 1.f) {
	int w = x2 - x1;
	int h = y2 - y1;

	int iw = w / 4;
	int ih = h / 4;
	// top
	RenderLine(x1, y1, x1 + iw, y1, color, th);					// left
	RenderLine(x1 + w - iw, y1, x1 + w, y1, color, th);			// right
	RenderLine(x1, y1, x1, y1 + ih, color, th);					// top left
	RenderLine(x1 + w - 1, y1, x1 + w - 1, y1 + ih, color, th);	// top right
																// bottom
	RenderLine(x1, y1 + h, x1 + iw, y1 + h, color, th);			// left
	RenderLine(x1 + w - iw, y1 + h, x1 + w, y1 + h, color, th);	// right
	RenderLine(x1, y1 + h - ih, x1, y1 + h, color, th);			// bottom left
	RenderLine(x1 + w - 1, y1 + h - ih, x1 + w - 1, y1 + h, color, th);	// bottom right
}

void RenderEspTab()
{

	static int SubTabs = 1;

	ImGuiEX::SubTabButton("ESP", &SubTabs, 0, 3);
	ImGuiEX::SubTabButton("Models", &SubTabs, 1, 3);
	ImGuiEX::SubTabButton("World", &SubTabs, 2, 3);


	bool placeholder_true = true;

	auto& style = ImGui::GetStyle();
	float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

	switch (SubTabs)
	{
	case 0:
	{
		bool placeholder_true = true;

		auto& style = ImGui::GetStyle();
		float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

		ImGui::SetCursorPos(ImVec2(22, 100));

		ImGui::BeginChild("General", ImVec2(310, 360), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);

			ImGui::CheckboxEX("Enabled", &g_Options.esp_enabled, "Enable wallhack",29);
			ImGui::SameLine();

			ImGui::SetCursorPosX(240);
			ImGui::ColorEdit4("Ally visible", g_Options.color_esp_ally_visible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel, 121);
			ImGui::SameLine();
			ImGui::ColorEdit4("Ally occluded", g_Options.color_esp_ally_occluded, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel, 2);


			ImGui::CheckboxEX("Enemy-olny", &g_Options.esp_enemies_only, "Enemy wallhack", 30);
			ImGui::SameLine();

			ImGui::SetCursorPosX(240);
			ImGui::ColorEdit4("Enemy visible", g_Options.color_esp_enemy_visible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel, 3);
			ImGui::SameLine();
			ImGui::ColorEdit4("Enemy occluded", g_Options.color_esp_enemy_occluded, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel, 4);


			ImGui::CheckboxEX("Player boxes", &g_Options.esp_player_boxes, "Box around player", 31);
			const char* box_type[4] = { "Default", "Outlined", "Corner" , "Corner-outlined" };
			if (g_Options.esp_player_boxes)
				ImGui::Combo("Box-type", &g_Options.esp_player_boxes_type, box_type, IM_ARRAYSIZE(box_type));
			ImGui::CheckboxEX("Draw name", &g_Options.esp_player_names, "Draw player name", 32);
			ImGui::CheckboxEX("Draw healthbar", &g_Options.esp_player_health, "Draw player health", 33);
			ImGui::CheckboxEX("Draw armour", &g_Options.esp_player_armour, "Draw armour", 34);
			ImGui::CheckboxEX("Draw weapon", &g_Options.esp_player_weapons, "Show weapon", 35);
			if (g_Options.esp_player_weapons) {
				static const char* w_type[] = { "Text", "Icon", };
				ImGui::Combo("Weapon-type", &g_Options.esp_player_weapons_type, w_type, IM_ARRAYSIZE(w_type));
			}
			ImGui::CheckboxEX("Draw snaplines", &g_Options.esp_player_snaplines, "Line to player", 36);
			ImGui::CheckboxEX("Draw skeleton", &g_Options.esp_player_skeleton, "Draw skeleton", 37);
			ImGui::CheckboxEX("Draw ammo##1", &g_Options.esp_player_ammo, "Show ammo", 38);
			ImGui::CheckboxEX("Draw head-dot", &g_Options.esp_head_dot, "Pos of head", 39);

			ImGui::Checkbox("Flags enabled", &g_Options.esp_player_flags);

			if (g_Options.esp_player_flags)
			{
				ImGui::Text("Flags  :");
				ImGui::CheckboxEX("Player", &g_Options.esp_player_flags_player, "Is entity player", 40);
				ImGui::CheckboxEX("Scoped", &g_Options.esp_player_flags_scoped, "Is scoped", 41);
				ImGui::CheckboxEX("Armour", &g_Options.esp_flags_armour, "Have armour", 42);
				ImGui::CheckboxEX("Defuse-kit", &g_Options.esp_flags_kit, "Have D.Kits", 43);
				ImGui::CheckboxEX("On move", &g_Options.esp_flags_move, "Is moving", 44);
				ImGui::CheckboxEX("Choking", &g_Options.esp_flags_choking, "Is choking", 45);
				ImGui::CheckboxEX("Flashed", &g_Options.esp_flags_flash, "Is flashed", 46);
			}



		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(22 + 310 + 17 + 10, 100));

		ImGui::BeginChild("Preview", ImVec2(310, 360), true);
		{
			ImGuiWindow* window = GImGui->CurrentWindow;

			ImGui::SetCursorPos(ImVec2(46, 60));
			//Drawing csgo player model

			if (g_Options.chams_player_enabled)
			{
				ImGui::Image(Menu::Get().model_flat, ImVec2(163, 250), ImVec2(0, 0), ImVec2(1, 1), ImVec4(g_Options.color_chams_player_enemy_visible[0], g_Options.color_chams_player_enemy_visible[1], g_Options.color_chams_player_enemy_visible[2], g_Options.color_chams_player_enemy_visible[3]), ImVec4(0, 0, 0, 0)); 
				//Menu::Get().model_regular
			}
			else
			{
				ImGui::Image(Menu::Get().model, ImVec2(163, 250), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));
			}
			if (g_Options.esp_player_names){
				//Draw name ESP

				ImGui::SetCursorPos(ImVec2(133 - 25 + 8,58 - 13));
				ImGui::PushFont(g_pESP); {
					ImGui::Text("Name text");
				} ImGui::PopFont();
			}
			if (g_Options.esp_player_boxes) {
				//Boxes
				switch (g_Options.esp_player_boxes_type)
				{
				case 0: ImGui::GetWindowDrawList()->AddRect(window->Pos + ImVec2(82, 63), window->Pos + ImVec2(82, 63) + ImVec2(123, 252), ImColor(1.0f, 1.0f, 1.0f, 1.0f));
					break;
				case 1: ImGui::GetWindowDrawList()->AddRect(window->Pos + ImVec2(82, 63), window->Pos + ImVec2(82, 63) + ImVec2(123, 252), ImColor(1.0f, 1.0f, 1.0f, 1.0f));
						ImGui::GetWindowDrawList()->AddRect(window->Pos + ImVec2(82 - 2, 63 - 2), window->Pos + ImVec2(82 - 2, 63 - 2) + ImVec2(123 + 4, 252 + 4), ImColor(0.0f, 0.0f, 0.0f, 0.5f));
						ImGui::GetWindowDrawList()->AddRect(window->Pos + ImVec2(82 + 2, 63 + 2), window->Pos + ImVec2(82 + 2, 63 + 2) + ImVec2(123 - 4, 252 - 4), ImColor(0.0f, 0.0f, 0.0f, 0.5f));
					break;
				case 2:
						RenderCoalBox(window->Pos.x + 82, window->Pos.y + 63, window->Pos.x + 82 + 123, window->Pos.y + 63 + 252, Color(255,255,255));
					break;
				case 3:
					RenderCoalBox(window->Pos.x + 82, window->Pos.y + 63, window->Pos.x + 82 + 123, window->Pos.y + 63 + 252, Color(255, 255, 255));
					RenderCoalBox(window->Pos.x + 82 - 2, window->Pos.y + 63 - 2, window->Pos.x + 82 - 2 + 123 + 4, window->Pos.y + 63 + 252 + 2, Color(0, 0, 0,150));
					RenderCoalBox(window->Pos.x + 82 + 2, window->Pos.y + 63 + 2, window->Pos.x + 82 + 2 + 123 - 4, window->Pos.y + 63 + 252 - 2, Color(0, 0, 0,150));
					break;
				}
			}
			if (g_Options.esp_player_health) {
				//HealthBar
				ImGui::GetWindowDrawList()->AddRectFilled(window->Pos + ImVec2(208 + 3,59 + 6), window->Pos + ImVec2(208 + 3, 59 + 6) + ImVec2(3,242 + 13 - 6), ImColor(0.0f, 1.0f, 0.0f, 1.0f));
			}
			if (g_Options.esp_player_weapons) {
				//HealthBar
				ImGui::SetCursorPos(ImVec2(133 - 25 + 2, 58 - 13 + 264 + 9));
				ImGui::PushFont(g_pESP); {
					ImGui::Text("Weapon text");
				} ImGui::PopFont();
			}
		}
		ImGui::EndChild();

	}
	break;
	case 1:
	{
		bool placeholder_true = true;

		auto& style = ImGui::GetStyle();
		float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

		ImGui::SetCursorPos(ImVec2(22, 100));

		ImGui::BeginChild("Chams/Glow", ImVec2(310, 360), true);
		{
			
				ImGui::Text(" ");

				ImGui::SetCursorPosY(+15);

				ImGui::CheckboxEX("Chams enabled", &g_Options.chams_player_enabled, "Model chams", 47);
				ImGui::SameLine();

				ImGui::SetCursorPosX(240);
				ImGui::ColorEdit4("Enemy-visible", g_Options.color_chams_player_enemy_visible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel, 5);
				ImGui::SameLine();
				ImGui::ColorEdit4("Enemy-inivisble", g_Options.color_chams_player_enemy_occluded, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel, 6);

				static const char* glow_type[] = { "Outer", "Cover", "Inner" };
				const char* chams_type[5] = { "Material", "Flat", "Glow#1" , "Glow#2", "Metallic" };
				if (g_Options.chams_player_enabled)
					ImGui::Combo("Chams-type", &g_Options.chams_player_type, chams_type, IM_ARRAYSIZE(chams_type));
				ImGui::CheckboxEX("Ignore wall", &g_Options.chams_player_ignorez, "Draw behind wall", 48);
				ImGui::CheckboxEX("Ignore team", &g_Options.chams_player_enemies_only, "Only enemy", 49);
				ImGui::CheckboxEX("Overlay player", &g_Options.chams_player_enabled_l, "Draw second mat", 428);

				ImGui::SameLine();
				ImGui::SetCursorPosX(240 + 25 + 8);
				ImGui::ColorEdit4("Layer color", g_Options.color_chams_player_enemy_visible_l, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel, 7);

				if (g_Options.chams_player_enabled_l)
					ImGui::Combo("Layer-type", &g_Options.chams_player_type_layer, chams_type, IM_ARRAYSIZE(chams_type));

				ImGui::Text("Arms-chams");
				ImGui::CheckboxEX("Arms enabled", &g_Options.chams_arms_enabled, "Draw hands", 50);

				ImGui::SameLine();
				ImGui::SetCursorPosX(240 + 25 + 8);
				ImGui::ColorEdit4("Hands color", g_Options.color_chams_arms_visible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel, 8);

				ImGui::CheckboxEX("No arms", &g_Options.misc_no_hands, "Remove hands", 51);
				if (g_Options.chams_arms_enabled && !g_Options.misc_no_hands)
					ImGui::Combo("Arms-type", &g_Options.chams_arms_type, chams_type, IM_ARRAYSIZE(chams_type));

				ImGui::CheckboxEX("Overlay hands", &g_Options.chams_hands_enabled_l, "Draw second mat  ", 239);

				ImGui::SameLine();
				ImGui::SetCursorPosX(240 + 25 + 8);
				ImGui::ColorEdit4("Hands color overlay", g_Options.color_chams_arms_visible_layer, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel, 9);

				if (g_Options.chams_hands_enabled_l)
					ImGui::Combo("Layer-type hands", &g_Options.chams_arms_type_layer, chams_type, IM_ARRAYSIZE(chams_type));


				ImGui::CheckboxEX("Weapons enabled", &g_Options.chams_weapons, "Weapon chams", 52);

				ImGui::SameLine();
				ImGui::SetCursorPosX(240 + 25 + 8);
				ImGui::ColorEdit4("wep color overlay", g_Options.color_chams_weapons, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel, 10);

				if (g_Options.chams_weapons)
					ImGui::Combo("Gun-type", &g_Options.chams_weapons_type, chams_type, IM_ARRAYSIZE(chams_type));

				ImGui::CheckboxEX("Overlay weapon", &g_Options.chams_weapon_enabled_l, "Draw second mat ", 238);

				ImGui::SameLine();
				ImGui::SetCursorPosX(240 + 25 + 8);
				ImGui::ColorEdit4("wep overlay color overlay", g_Options.color_chams_weapons_layer, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel, 11);

				if (g_Options.chams_weapon_enabled_l)
					ImGui::Combo("Layer-type weapon", &g_Options.chams_weapons_type_layer, chams_type, IM_ARRAYSIZE(chams_type));


				ImGui::Text("Player glow-ESP");
				ImGui::CheckboxEX("Glow enabled", &g_Options.glow_enabled, "Player outline", 54);
				ImGui::SameLine();
				ImGui::SetCursorPosX(240 + 25 + 8);
				ImGui::ColorEdit4("glow color 1", g_Options.color_glow_enemy, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel, 12);

				ImGui::CheckboxEX("Glow player's", &g_Options.glow_players, "Players", 55);
				ImGui::CheckboxEX("Only enemy", &g_Options.glow_enemies_only, "Only enemy", 56);
				if (g_Options.glow_enabled)
				{
					if (g_Options.glow_enemies_only)
						ImGui::Combo("Enemy glow", &g_Options.glow_type, glow_type, IM_ARRAYSIZE(glow_type));
					else
						ImGui::Combo("Team glow", &g_Options.glow_team_type, glow_type, IM_ARRAYSIZE(glow_type));
				}

		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(22 + 310 + 17 + 10, 100));


		ImGui::BeginChild("Preview", ImVec2(310, 360), true);
		{
			ImGuiWindow* window = GImGui->CurrentWindow;

			ImGui::SetCursorPos(ImVec2(46, 60));
			//Drawing csgo player model

			if (g_Options.chams_player_enabled)
			{
				ImGui::Image(Menu::Get().model_flat, ImVec2(163, 250), ImVec2(0, 0), ImVec2(1, 1), ImVec4(g_Options.color_chams_player_enemy_visible[0], g_Options.color_chams_player_enemy_visible[1], g_Options.color_chams_player_enemy_visible[2], g_Options.color_chams_player_enemy_visible[3]), ImVec4(0, 0, 0, 0));
				//Menu::Get().model_regular
			}
			else
			{
				ImGui::Image(Menu::Get().model, ImVec2(163, 250), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));
			}
			if (g_Options.esp_player_names) {
				//Draw name ESP

				ImGui::SetCursorPos(ImVec2(133 - 25 + 8, 58 - 13));
				ImGui::PushFont(g_pESP); {
					ImGui::Text("Name text");
				} ImGui::PopFont();
			}
			if (g_Options.esp_player_boxes) {
				//Boxes
				switch (g_Options.esp_player_boxes_type)
				{
				case 0: ImGui::GetWindowDrawList()->AddRect(window->Pos + ImVec2(82, 63), window->Pos + ImVec2(82, 63) + ImVec2(123, 252), ImColor(1.0f, 1.0f, 1.0f, 1.0f));
					break;
				case 1: ImGui::GetWindowDrawList()->AddRect(window->Pos + ImVec2(82, 63), window->Pos + ImVec2(82, 63) + ImVec2(123, 252), ImColor(1.0f, 1.0f, 1.0f, 1.0f));
					ImGui::GetWindowDrawList()->AddRect(window->Pos + ImVec2(82 - 2, 63 - 2), window->Pos + ImVec2(82 - 2, 63 - 2) + ImVec2(123 + 4, 252 + 4), ImColor(0.0f, 0.0f, 0.0f, 0.5f));
					ImGui::GetWindowDrawList()->AddRect(window->Pos + ImVec2(82 + 2, 63 + 2), window->Pos + ImVec2(82 + 2, 63 + 2) + ImVec2(123 - 4, 252 - 4), ImColor(0.0f, 0.0f, 0.0f, 0.5f));
					break;
				case 2:
					RenderCoalBox(window->Pos.x + 82, window->Pos.y + 63, window->Pos.x + 82 + 123, window->Pos.y + 63 + 252, Color(255, 255, 255));
					break;
				case 3:
					RenderCoalBox(window->Pos.x + 82, window->Pos.y + 63, window->Pos.x + 82 + 123, window->Pos.y + 63 + 252, Color(255, 255, 255));
					RenderCoalBox(window->Pos.x + 82 - 2, window->Pos.y + 63 - 2, window->Pos.x + 82 - 2 + 123 + 4, window->Pos.y + 63 + 252 + 2, Color(0, 0, 0, 150));
					RenderCoalBox(window->Pos.x + 82 + 2, window->Pos.y + 63 + 2, window->Pos.x + 82 + 2 + 123 - 4, window->Pos.y + 63 + 252 - 2, Color(0, 0, 0, 150));
					break;
				}
			}
			if (g_Options.esp_player_health) {
				//HealthBar
				ImGui::GetWindowDrawList()->AddRectFilled(window->Pos + ImVec2(208 + 3, 59 + 6), window->Pos + ImVec2(208 + 3, 59 + 6) + ImVec2(3, 242 + 13 - 6), ImColor(0.0f, 1.0f, 0.0f, 1.0f));
			}
			if (g_Options.esp_player_weapons) {
				//HealthBar
				ImGui::SetCursorPos(ImVec2(133 - 25 + 2, 58 - 13 + 264 + 9));
				ImGui::PushFont(g_pESP); {
					ImGui::Text("Weapon text");
				} ImGui::PopFont();
			}
		}
		ImGui::EndChild();


	}
	break;
	case 2:
	{
		bool placeholder_true = true;

		auto& style = ImGui::GetStyle();
		float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

		ImGui::SetCursorPos(ImVec2(22, 100));

		ImGui::BeginChild("World", ImVec2(310, 360), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);

			ImGui::Text("Viewmodel-setting's");
			ImGui::SliderInt("Viewmodel_fov:", &g_Options.viewmodel_fov, 65, 135);
			ImGui::SliderInt("World_fov:", &g_Options.world_fov, 0, 65);
			ImGui::SliderFloat("Aspect ratio:", &g_Options.esp_aspect_ratio, 0, 5.5);
			ImGui::Text("Grenade prediction :");
			ImGui::CheckboxEX("Nade prediction", &g_Options.esp_nade_prediction, "Draw local grenade way", 57);
			ImGui::CheckboxEX("Molotov timer", &g_Options.esp_molotov_timer, "Molotov timer (sec)", 58);
			ImGui::Text("Sound esp :");
			ImGui::CheckboxEX("Sound esp", &g_Options.sound_esp, "Draw sound of player", 59);

			ImGui::SameLine();
			ImGui::SetCursorPosX(240 + 25 + 8);
			ImGui::ColorEdit4("sound esp color", g_Options.color_sound_esp, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel, 13);

			if (g_Options.sound_esp)
			{
				const char* soundesp_modes[] = { "Default","Beams" };

				ImGui::Combo("Sound esp type", &g_Options.sound_esp_type, soundesp_modes, IM_ARRAYSIZE(soundesp_modes));
				ImGui::SliderFloat("Sound esp radius", &g_Options.sound_esp_radius, 5, 30);
				ImGui::SliderFloat("Sound esp time", &g_Options.sound_esp_time, 0.3, 5);
			}

			ImGui::Text("Other :");
			ImGui::CheckboxEX("Draw crosshair", &g_Options.esp_crosshair, "Default crosshair", 60);
			ImGui::CheckboxEX("Draw recoil-crosshair", &g_Options.esp_recoil_crosshair, "Recoil based crosshair", 61);
			ImGui::CheckboxEX("Draw legitbot fov", &g_Options.esp_draw_fov, "Aim radius for legitbot", 62);
			ImGui::CheckboxEX("Draw angle's", &g_Options.esp_angle_lines, "Fake and Real angles", 63);
			ImGui::CheckboxEX("Draw shot-hitboxes", &g_Options.shot_hitboxes, "Hitbox Positions", 64);

			ImGui::SameLine();
			ImGui::SetCursorPosX(240 + 25 + 8);
			ImGui::ColorEdit4("shotbox clr", g_Options.color_shot_hitboxes, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel, 14);


			if (g_Options.shot_hitboxes)
				ImGui::SliderFloat("Shot-hitboxes duration", &g_Options.shot_hitboxes_duration, 0, 7);
			ImGui::CheckboxEX("OFESP", &g_Options.esp_offscreen, "Flag invisible players on screen", 65);

			ImGui::SameLine();
			ImGui::SetCursorPosX(240 + 25 + 8);
			ImGui::ColorEdit4("offesp esp color", g_Options.color_esp_offscreen, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel, 15);

			if (g_Options.esp_offscreen)
			{
				ImGui::SliderFloat("Offscreen-radius:", &g_Options.esp_offscreen_range, 0, 600);
				ImGui::SliderFloat("Offscreen-size", &g_Options.esp_offscreen_size, 10, 60);
			}



		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(22 + 310 + 17 + 10, 100));


		ImGui::BeginChild("Other", ImVec2(310, 360), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);

			ImGui::CheckboxEX("Remove smoke", &g_Options.remove_smoke, "No smoke effect", 67);
			ImGui::CheckboxEX("Remove flash", &g_Options.remove_flash, "No flasheffect", 68);
			if (g_Options.remove_flash)
				ImGui::SliderInt("Flash-ammount", &g_Options.esp_flash_ammount, 0, 255);
			ImGui::CheckboxEX("Remove scope", &g_Options.remove_scope, "Sniper crosshair", 69);

			ImGui::SameLine();
			ImGui::SetCursorPosX(240 + 25 + 8);
			ImGui::ColorEdit4("remove scope color", g_Options.color_esp_crosshair, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel, 16);

			ImGui::CheckboxEX("Remove vis-recoil", &g_Options.remove_visual_recoil, "No recoil", 70);
			ImGui::CheckboxEX("Remove sleeves", &g_Options.remove_sleeves, "Delete sleeves", 71);

			ImGui::CheckboxEX("Bloom effect", &g_Options.esp_bloom_effect, "CSGO bloom", 72);
			if (g_Options.esp_bloom_effect)
			{
				ImGui::Text("Bloom :");
				ImGui::SliderInt("  Bloom factor", &g_Options.esp_bloom_factor, 0, 100);
				ImGui::SliderFloat("Model ambient", &g_Options.esp_model_ambient, 0, 11.1f);
			}

			ImGui::SliderFloat("Red", &g_Options.mat_ambient_light_r, 0, 1);
			ImGui::SliderFloat("Green", &g_Options.mat_ambient_light_g, 0, 1);
			ImGui::SliderFloat("Blue", &g_Options.mat_ambient_light_b, 0, 1);

			ImGui::Text("Postprocessing:");
			ImGui::Checkbox("Nightmode", &g_Options.esp_nightmode);
			if (g_Options.esp_nightmode)
				ImGui::SliderFloat("Nightmode bright", &g_Options.esp_nightmode_bright, 0, 100);

			ImGui::CheckboxEX("Fullbright", &g_Options.esp_fullbright, "Remove shadows", 73);


		}
		ImGui::EndChild();

	}
	break;



	}
}
void RenderMiscTab()
{
	bool placeholder_true = true;

	auto& style = ImGui::GetStyle();
	float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

	static int SubTabs;

	ImGuiEX::SubTabButton("Main", &SubTabs, 0, 4);
	ImGuiEX::SubTabButton("Other", &SubTabs, 1, 4);
	ImGuiEX::SubTabButton("Skins", &SubTabs, 2, 4);
	ImGuiEX::SubTabButton("Info", &SubTabs, 3, 4);

	switch (SubTabs)
	{
	case 0:
	{
		bool placeholder_true = true;

		auto& style = ImGui::GetStyle();
		float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

		ImGui::SetCursorPos(ImVec2(22, 100));

		ImGui::BeginChild("General", ImVec2(310, 360), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);

			ImGui::ColorEdit4("Menu color", g_Options.menu_color, ImGuiColorEditFlags_NoInputs, 20);
			ImGui::SliderInt("MenuScale", &g_Options.menu_scale, 1, 50);
		
			ImGui::CheckboxEX("Anti-Untrusted", &g_Options.misc_anti_untrusted, "Remove untrusted angles", 74);
			ImGui::CheckboxEX("Infinity duck", &g_Options.misc_infinity_duck, "Nolimit for duck", 75);
			ImGui::CheckboxEX("Anti-obs", &g_Options.misc_anti_screenshot, "No ESP on OBS and screenshot", 76);
			ImGui::CheckboxEX("Legit resolver", &g_Options.misc_legit_resolver, "Resolve legit aa", 77);


			ImGui::CheckboxEX("Legit antihit", &g_Options.misc_legit_antihit, "local legit AA", 78);
			if (g_Options.misc_legit_antihit)
			{
				ImGui::CheckboxEX("LBY based", &g_Options.misc_legit_antihit_lby, "LBY mode", 79);
				ImGui::Text("Switch side key :");
				ImGuiEX::Hotkey("##3side	  ", &g_Options.misc_legit_antihit_key, ImVec2(150, 20));

			}

			ImGui::Text("Movement");
			ImGui::CheckboxEX("Auto-jump", &g_Options.misc_bhop, "Bunnyhop", 80);
			ImGui::CheckboxEX("Auto-strafe", &g_Options.misc_autostrafe, "StrafeBot", 81);
			ImGui::CheckboxEX("Jump-throw", &g_Options.misc_jump_throw, "Jump Throw props", 82); // @blick1337
			ImGui::CheckboxEX("Third-person", &g_Options.misc_thirdperson, "Local thirdperson", 83);
			if (g_Options.misc_thirdperson)
			{
				ImGui::Text("Thirdperson-key:");
				ImGuiEX::Hotkey("##3rdkey", &g_Options.misc_thirdperson_key, ImVec2(150, 20));
				//	ImGui::SliderFloat("distance", &g_Options.misc_thirdperson_dist, 0.f, 150.f);
			}
			ImGui::CheckboxEX("Inventory-access", &g_Options.misc_unlock_inventory, "change weapons in inventory", 84);
			ImGui::CheckboxEX("Left-knife", &g_Options.misc_left_knife, "Left hand if knife", 85);
			ImGui::CheckboxEX("Fake-fps", &g_Options.misc_fake_fps, "Boost FPS", 86);
			ImGui::CheckboxEX("Show-ranks", &g_Options.misc_showranks, "Show MM ranks", 87);
			ImGui::CheckboxEX("Clan-tag", &g_Options.misc_clantag, "Cheat tag", 88);
			ImGui::CheckboxEX("Chat-spam", &g_Options.misc_chat_spam, "Spam in chat", 89);

			static const char* models[] = { "Default","Special Agent Ava "," FBI","Operator "," FBI SWAT","Markus Delrow "," FBI HRT","Michael Syfers "," FBI Sniper","B Squadron Officer "," SAS","Seal Team 6 Soldier "," NSWC SEAL","Buckshot "," NSWC SEAL","Lt. Commander Ricksaw "," NSWC SEAL","Third Commando Company "," KSK","'Two Times' McCoy "," USAF TACP","Dragomir "," Sabre","Rezan The Ready "," Sabre","'The Doctor' Romanov "," Sabre","Maximus "," Sabre","Blackwolf "," Sabre","The Elite Mr. Muhlik "," Elite Crew","Ground Rebel "," Elite Crew","Osiris "," Elite Crew","Prof. Shahmat "," Elite Crew","Enforcer "," Phoenix","Slingshot "," Phoenix","Soldier "," Phoenix" };

			ImGui::Combo("Player t", &g_Options.playerModelT, models, IM_ARRAYSIZE(models));

			ImGui::Combo("Player ct", &g_Options.playerModelCT, models, IM_ARRAYSIZE(models));


			ImGui::CheckboxEX("Bullet-impact's", &g_Options.misc_bullet_impacts, "Bullet pos", 90);
			ImGui::CheckboxEX("Bullet-tracer", &g_Options.misc_bullet_tracer, "Bullet way", 91);

			ImGui::SameLine();
			ImGui::SetCursorPosX(240 + 25 + 8);
			ImGui::ColorEdit4("bullettr color sd", g_Options.color_bullet_tracer, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel, 21);


			ImGui::CheckboxEX("Hit-marker", &g_Options.misc_hitmarker, "Marker if hit", 92);

			ImGui::SameLine();
			ImGui::SetCursorPosX(240 + 25 + 8);
			ImGui::ColorEdit4("hitmarker color sd", g_Options.color_hitmarker, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel, 22);



		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(22 + 310 + 17 + 10, 100));

		ImGui::BeginChild("Server", ImVec2(310, 360), true);
		{

			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);
			ImGui::Text("Silent-walk key");
			ImGuiEX::Hotkey("##Silent-walk", &g_Options.misc_silentwalk_key, ImVec2(150, 20));
			ImGui::CheckboxEX("Slow-walk", &g_Options.misc_slowwalk, "Slowmotion", 93);
			if (g_Options.misc_slowwalk)
			{
				ImGuiEX::Hotkey("##slow-walk key", &g_Options.misc_slowwalk_key, ImVec2(150, 20));
				ImGui::SliderInt("Slow-walk speed %", &g_Options.misc_slowwalk_speed, 0, 99);
			}
			ImGui::CheckboxEX("Fake-duck", &g_Options.misc_fakeduck, "Fakeduck", 94);
			if (g_Options.misc_fakeduck)
			{
				ImGui::Text("fakeduck key :");
				ImGuiEX::Hotkey("##fakeduck key", &g_Options.misc_fakeduck_key, ImVec2(150, 20));
				ImGui::SliderInt("fakeduck tick's", &g_Options.misc_fakeduck_ticks, 0, 16);
			}
			const char* fakelag_type[]     = { "Factor","Adaptive","Random" };
			const char* fakelag_triggers[] = { "Stand","Move","HighMove" , "Air", "Slowwalk" };
			ImGui::CheckboxEX("Fakelag", &g_Options.misc_fakelag, "Fake lags", 95);
			ImGui::CheckboxEX("Shot ignore", &g_Options.misc_fakelag_on_shot, "No fl on shot", 96);
			if (g_Options.misc_fakelag)
			{
				ImGui::Combo("Fakelag-type", &g_Options.misc_fakelag_type, fakelag_type, IM_ARRAYSIZE(fakelag_type));
				ImGui::SliderInt("Fakelag ticks :", &g_Options.misc_fakelag_ticks, 0, 14);
				ImGui::SliderInt("Triggered ticks :", &g_Options.misc_fakelag_triggered_ticks, 0, 16);
				if (ImGui::BeginCombo("Triggers", "Select", 0))
				{
		
					for (size_t i = 0; i < IM_ARRAYSIZE(fakelag_triggers); i++)
					{
						ImGui::Selectable(fakelag_triggers[i], &g_Options.misc_fakelag_triggers[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						
					}

					ImGui::EndCombo();
				}
				

			}
			ImGui::CheckboxEX("Knife-bot", &g_Options.misc_knifebot, "Autoknife", 97);
			if (g_Options.misc_knifebot)
			{
				ImGui::CheckboxEX("Auto atack", &g_Options.misc_auto_knifebot, "Always attack", 98);
				ImGui::Checkbox("360 atack[untrusted]", &g_Options.misc_knifebot_360);
			}
			ImGui::CheckboxEX("Double-tap", &g_Options.exploit_doubletap, "Fast fire exploit", 99);
			if (g_Options.exploit_doubletap)
			{
				ImGuiEX::Hotkey("##rapidfire key", &g_Options.exploit_doubletap_key, ImVec2(150, 20));
				ImGui::CheckboxEX("Hide-shots", &g_Options.exploit_hideshots, "hideshots exploit", 100);
			}

		}
		ImGui::EndChild();

	}
	break;

	case 1:
	{
		bool placeholder_true = true;

		auto& style = ImGui::GetStyle();
		float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

		ImGui::SetCursorPos(ImVec2(22, 100));

		ImGui::BeginChild("Windows", ImVec2(310, 360), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);

			if (g_Options.misc_radar)
				ImGui::SliderFloat("radar-range", &g_Options.misc_radar_range, 200.f, 1600.f);




			ImGui::CheckboxEX("Watermark", &g_Options.misc_watermark, "Cheat watermark", 101);
			ImGui::CheckboxEX("Spectator-list", &g_Options.misc_spectator_list, "Spectator list", 102);
			ImGui::CheckboxEX("In-game radar", &g_Options.misc_engine_radar, "Csgo radar", 103);
			ImGui::CheckboxEX("Radar-window", &g_Options.misc_radar, "Cheat radar", 104);
			ImGui::Text("other # 2 :");
			ImGui::Checkbox("Gravity ragdolls", &g_Options.misc_exaggerated_ragdolls);
			if (g_Options.misc_exaggerated_ragdolls)
				ImGui::SliderInt("Ragdolls force :", &g_Options.misc_exaggerated_ragdolls_force, 1, 35);



		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(22 + 310 + 17 + 10, 100));


		ImGui::BeginChild("Other", ImVec2(310, 360), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);

			if (g_Options.misc_hitmarker)
				ImGui::SliderFloat("Hit-marker size", &g_Options.misc_hitmarker_size, 1.f, 30.f);
			ImGui::CheckboxEX("Hit-sound", &g_Options.misc_hitsound, "Sound if hit", 105);
			const char* sound_type[5] = { "Arena switch", "Bank" ,"Hit", "Metalic", "Rifk" };
			ImGui::Combo("HitSound type", &g_Options.misc_hitsound_type, sound_type, IM_ARRAYSIZE(sound_type));

			ImGui::CheckboxEX("Hit-effect", &g_Options.misc_hiteffect, "Effect if hit", 106);
			if (g_Options.misc_hiteffect)
				ImGui::SliderFloat("Hit-effect duration", &g_Options.misc_hiteffect_duration, 0.1f, 5.f);
			ImGui::Text("Event logger");
			ImGui::CheckboxEX("Event log's", &g_Options.misc_event_log, "CSGO event log", 107);
			ImGui::Text("Log's :");
			ImGui::CheckboxEX("Player hit", &g_Options.event_log_hit, "Log it, if was hit", 108);
			ImGui::CheckboxEX("Player purchases", &g_Options.event_log_item, "Log purchases", 109);
			ImGui::CheckboxEX("Planting", &g_Options.event_log_plant, "Log it, if bomb has been planted", 110);
			ImGui::CheckboxEX("Defusing", &g_Options.event_log_defuse, "Log it, if bomb is defusing", 111);


		}
		ImGui::EndChild();

	}
	break;

	case 2:
	{
		if (k_skins.size() == 0) {
			initialize_kits();
		}

		auto& entries = g_Options.m_skins.m_items;

		static auto definition_vector_index = 0;

		auto& selected_entry = entries[k_weapon_names[definition_vector_index].definition_index];
		selected_entry.definition_index = k_weapon_names[definition_vector_index].definition_index;
		selected_entry.definition_vector_index = definition_vector_index;

		ImGui::SetCursorPos(ImVec2(22, 100));

		ImGui::BeginChild("List", ImVec2(310, 360), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);


			ImGui::Checkbox("enabled", &selected_entry.enabled);
			ImGui::SliderInt("seed", &selected_entry.seed,0,1000);
			ImGui::SliderInt("stat-trak", &selected_entry.stat_trak, 0, 1000);
			ImGui::SliderFloat("wear", &selected_entry.wear, 0.f, 5);

			if (selected_entry.definition_index != GLOVE_T_SIDE)
			{
				ImGui::Combo("paint-kit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = k_skins[idx].name.c_str();
						return true;
					}, nullptr, k_skins.size(), 10);
				selected_entry.paint_kit_index = k_skins[selected_entry.paint_kit_vector_index].id;
			}
			else
			{
				ImGui::Combo("paint-kit##2", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = k_gloves[idx].name.c_str();
						return true;
					}, nullptr, k_gloves.size(), 10);
				selected_entry.paint_kit_index = k_gloves[selected_entry.paint_kit_vector_index].id;
			}
			if (selected_entry.definition_index == WEAPON_KNIFE)
			{
				ImGui::Combo("knife", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = k_knife_names.at(idx).name;
						return true;
					}, nullptr, k_knife_names.size(), 5);
				selected_entry.definition_override_index = k_knife_names.at(selected_entry.definition_override_vector_index).definition_index;
			}
			else if (selected_entry.definition_index == GLOVE_T_SIDE)
			{
				ImGui::Combo("glove", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = k_glove_names.at(idx).name;
						return true;
					}, nullptr, k_glove_names.size(), 5);
				selected_entry.definition_override_index = k_glove_names.at(selected_entry.definition_override_vector_index).definition_index;
			}
			else
			{
				static auto unused_value = 0;
				selected_entry.definition_override_vector_index = 0;
				ImGui::Combo("unavailable", &unused_value, "For knives or gloves\0");
			}

			ImGui::InputText("name-tag", selected_entry.custom_name, 32);
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(22 + 310 + 17 + 10, 100));

		ImGui::BeginChild("Configurate", ImVec2(310, 360), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);

			static std::string current_skin;

			if (ImGui::BeginCombo("Weapon Selection", current_skin.c_str()))
			{
		
				for (size_t w = 0; w < k_weapon_names.size(); w++) {
					if (ImGui::Selectable(k_weapon_names[w].name, definition_vector_index == w)) {
						definition_vector_index = w;
						current_skin = k_weapon_names[definition_vector_index].name;
					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("update", ImVec2(290, 25)))
				g_ClientState->ForceFullUpdate();

		}
		ImGui::EndChild();


	}
	break;

	case 3:
	{
		bool placeholder_true = true;

		auto& style = ImGui::GetStyle();
		float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

		ImGui::SetCursorPos(ImVec2(22, 100));


		ImGui::BeginChild("Info", ImVec2(310, 360), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);

			ImGui::Text("Coded by snake & ba1m0v ");
			ImGui::Text("Insight beta for csgo v0.0014 ");

			if (ImGui::Button("DISCORD SERVER", ImVec2(176, 40)))
				ShellExecute(nullptr, TEXT("open"), TEXT("https://discord.gg/uW4437Q"), nullptr, nullptr, 1);

			if (ImGui::Button("VK GROUP", ImVec2(176, 40)))
				ShellExecute(nullptr, TEXT("open"), TEXT("https://vk.com/snakewarecheat"), nullptr, nullptr, 1);

			if (ImGui::Button("FORUM", ImVec2(176, 40)))
				ShellExecute(nullptr, TEXT("open"), TEXT("https://snakeware.xyz/"), nullptr, nullptr, 1);


		}
		ImGui::EndChild();



	}
	break;



	}

}
void RenderSkinsTab()
{

	if (k_skins.size() == 0) {
		initialize_kits();
	}
	auto& entries = g_Options.m_skins.m_items;

	static auto definition_vector_index = 0;


	ImGui::SetCursorPos(ImVec2(22, 100));

	ImGui::BeginChild("List", ImVec2(310, 360), true);
	{
		ImGui::Text(" ");

		ImGui::SetCursorPosY(+15);
		ImGui::Text("skin's setting's :");

		auto& selected_entry = entries[k_weapon_names[definition_vector_index].definition_index];
		selected_entry.definition_index = k_weapon_names[definition_vector_index].definition_index;
		selected_entry.definition_vector_index = definition_vector_index;
		ImGui::Checkbox("enabled", &selected_entry.enabled);
		ImGui::InputInt("seed", &selected_entry.seed);
		ImGui::InputInt("stat-trak", &selected_entry.stat_trak);
		ImGui::SliderFloat("wear", &selected_entry.wear, 0.f, 5);
		if (selected_entry.definition_index != GLOVE_T_SIDE)
		{
			ImGui::Combo("paint-kit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text)
				{
					*out_text = k_skins[idx].name.c_str();
					return true;
				}, nullptr, k_skins.size(), 10);
			selected_entry.paint_kit_index = k_skins[selected_entry.paint_kit_vector_index].id;
		}
		else
		{
			ImGui::Combo("paint-kit##2", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text)
				{
					*out_text = k_gloves[idx].name.c_str();
					return true;
				}, nullptr, k_gloves.size(), 10);
			selected_entry.paint_kit_index = k_gloves[selected_entry.paint_kit_vector_index].id;
		}
		if (selected_entry.definition_index == WEAPON_KNIFE)
		{
			ImGui::Combo("knife", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
				{
					*out_text = k_knife_names.at(idx).name;
					return true;
				}, nullptr, k_knife_names.size(), 5);
			selected_entry.definition_override_index = k_knife_names.at(selected_entry.definition_override_vector_index).definition_index;
		}
		else if (selected_entry.definition_index == GLOVE_T_SIDE)
		{
			ImGui::Combo("glove", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
				{
					*out_text = k_glove_names.at(idx).name;
					return true;
				}, nullptr, k_glove_names.size(), 5);
			selected_entry.definition_override_index = k_glove_names.at(selected_entry.definition_override_vector_index).definition_index;
		}
		else
		{
			static auto unused_value = 0;
			selected_entry.definition_override_vector_index = 0;
			ImGui::Combo("unavailable", &unused_value, "For knives or gloves\0");
		}
		ImGui::InputText("name-tag", selected_entry.custom_name, 32);
	}
	ImGui::EndChild();

	ImGui::SetCursorPos(ImVec2(22 + 310 + 17 + 10, 100));

	ImGui::BeginChild("Configurate", ImVec2(310, 360), true);
	{
		ImGui::Text(" ");

		ImGui::SetCursorPosY(+15);


		static std::string current_config;
		static int CatalogId;

		if (ImGui::BeginCombo("Skins Selection", current_config.c_str()))
		{

			for (size_t w = 0; w < k_weapon_names.size(); w++) {
				if (ImGui::Selectable(k_weapon_names[w].name, definition_vector_index == w)) {
					definition_vector_index = w;
				}
			}
			ImGui::EndCombo();
		}

	}
	ImGui::EndChild();
}
void draw_lua_items() {
	for (size_t i_ = 0; i_ < 15; i_++)
	{
		auto i = lua::lua_m[i_];
		auto type = i.type;
		switch (type)
		{
		case lua::MENUITEM_CHECKBOX:
			break;

		case lua::MENUITEM_SLIDERINT:
			ImGui::SliderInt(i.label.c_str(), &i.var_int, i.i_min, i.i_max, i.format.c_str());

			break;
		case lua::MENUITEM_SLIDERFLOAT:
			ImGui::SliderFloat(i.label.c_str(), &i.var_float, i.f_min, i.f_max, i.format.c_str());

			break;

		case lua::MENUITEM_TEXT:
			ImGui::Text(i.label.c_str());
			break;

		default:
			break;
		}
	}
}
// Config helpers //
static int config_process;
//Config process
enum process_type {
	NONE = -1,
	SAVING = 2,
	LOADING = 3,
	FIRST_LOAD = 4,
	DELETING = 5
};
bool RenderWarningSystem(const char* warning_text)
{
	//Setup window
	static bool active = true;
	static int windowx;
	static int windowy;
	g_EngineClient->GetScreenSize(windowx, windowy);

	if ((active) && ((config_process == SAVING) || (config_process == LOADING) || (config_process == FIRST_LOAD) || (config_process == DELETING))) {
		ImGui::SetNextWindowSize(ImVec2(250, 105));
		ImGui::SetNextWindowPos(ImVec2((windowx / 2) - (250 / 2), (windowy / 2) - (105 / 2)));
		ImGui::Begin(" Config System  ", &active); {

			ImGui::Text(warning_text);
			if (ImGui::Button("Yes")) {
				config_process = NONE;
				active = false;
				return true;
			}
			ImGui::SameLine();
			if (ImGui::Button("No")) {
				config_process = NONE;
				active = false;
				return false;
			}
		}
		ImGui::End();
	}

}


void RenderConfigTab()
{
	static int SubTabs;
	std::stringstream config_lua;

	ImGuiEX::SubTabButton("Configs", &SubTabs, 0, 2);
	ImGuiEX::SubTabButton("Scripts", &SubTabs, 1, 2);

	switch (SubTabs)
	{
	case 0:
	{
		bool placeholder_true = true;

		auto& style = ImGui::GetStyle();
		float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

		ImGui::SetCursorPos(ImVec2(22, 100));

		ImGui::BeginChild("Config", ImVec2(310, 360), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);

			ImGui::Text("configuration system");
			static std::vector<std::string> configs;

			static auto load_configs = []() {
				std::vector<std::string> items = {};

				std::string path("C:\\snakeware");
				if (!fs::is_directory(path))
					fs::create_directories(path);

				for (auto& p : fs::directory_iterator(path))
					items.push_back(p.path().string().substr(path.length() + 1));

				return items;
			};

			static auto is_configs_loaded = false;
			if (!is_configs_loaded) {
				is_configs_loaded = true;
				configs = load_configs();
			}

			static std::string current_config;

			static char config_name[32];
			ImGui::PushItemWidth(250.f);




			if (ImGui::BeginCombo("select config", current_config.c_str()))
			{

				//
				for (auto& config : configs) {
					if (ImGui::Selectable(config.c_str(), config == current_config)) {
						current_config = config;
					}
				}

				ImGui::EndCombo();
			}

			ImGui::InputText("##config_name", config_name, sizeof(config_name));
			if (ImGui::Button("create", ImVec2(290, 25))) {
				current_config = std::string(config_name);

				Config->Save(current_config + ".xyz");
				is_configs_loaded = false;
				memset(config_name, 0, 32);
			}

			if (ImGui::Button("refresh", ImVec2(290, 25)));
			is_configs_loaded = false;
			
			if (ImGui::Button("open settings folder", ImVec2(290, 25)))
				ShellExecuteA(0, "open", "C:/snakeware", NULL, NULL, SW_NORMAL);
			if (!current_config.empty()) {


				config_lua << "autorun" << current_config.c_str() << ".lua";
				if (ImGui::Button("Load", ImVec2(290, 25))) {
					Config->Load(current_config);
					lua::load_script(lua::get_script_id(config_lua.str()));
					
				}	
				if (ImGui::Button("Save", ImVec2(290, 25)))
				{
					CreateDirectoryA(u8"C:\\snakeware\\", NULL);
					std::string file_name = config_lua.str();
					std::string file = u8"C:\\snakeware\\lua\\" + config_lua.str();
					std::ofstream out(file);

					out << "client.RegisterFunction(" << "\"" << "on_paint" <<"\"" << ", function()";
					for (auto& lua : lua::scripts) {
						if (lua::loaded.at(lua::get_script_id(lua.c_str() ))) {
							out << std::endl;
							out << "	client.RunScript(" << "\"" << lua.c_str() << "\"" << ")";
						}
					}
					out << std::endl;
					out << "end)";
					
					Config->Save(current_config);
					//Process last save
					config_process = SAVING;
				}
				if (ImGui::Button("Delete", ImVec2(290, 25)) && fs::remove("C:\\snakware\\" + current_config)) {
					current_config.clear();
					is_configs_loaded = false;
					//Process last delete
					config_process = DELETING;
				}

				if ((config_process != LOADING) || (config_process != SAVING) || (config_process != DELETING)){
					//Process none 
					config_process = FIRST_LOAD;
				}


				
			}



		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(22 + 310 + 17 + 10, 100));

		ImGui::BeginChild("Colors", ImVec2(310, 360), true);
		{
			ImGui::Text(" ");
			ImGui::SetCursorPosY(+15);
			ImGuiStyle& style = ImGui::GetStyle();

		}
		ImGui::EndChild();

	}
	break;
	case 1:
	{
		bool placeholder_true = true;

		auto& style = ImGui::GetStyle();
		float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

		ImGui::SetCursorPos(ImVec2(22, 100));

		ImGui::BeginChild("Scipts", ImVec2(310, 360), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);


				static std::string current_config;
				static int CatalogId;

				if (ImGui::BeginCombo("Lua elements", current_config.c_str()))
				{

					for (auto& lua : lua::scripts) {
						if (ImGui::Selectable(lua.c_str(), lua == current_config)) {
							current_config = lua;
						}
						
					}

					ImGui::EndCombo();
				}

				if (ImGui::Button("Refresh", { 290, 25 }))
				{
					lua::unload_all_scripts();
					lua::refresh_scripts();
					//lua::load_script(lua::get_script_id("autorun.lua"));
				}

				if (ImGui::Button("Reload All", { 290, 25 }))
					lua::reload_all_scripts();

				if (ImGui::Button("Unload All", { 290, 25 }))
					lua::unload_all_scripts();

				if (ImGui::Button("Unload selected", { 290, 25 }))
				{
					lua::unload_script(lua::get_script_id(current_config));
				}
				if (ImGui::Button("Load selected", { 290, 25 }))
				{
					lua::load_script(lua::get_script_id(current_config));
				}

				

			
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(22 + 310 + 17 + 10, 100));

		ImGui::BeginChild("Settings", ImVec2(310, 360), true);
		{
			ImGui::Text(" ");

			ImGui::SetCursorPosY(+15);

			ImGuiStyle& style = ImGui::GetStyle();

			draw_lua_items();


		}
		ImGui::EndChild();

	}
	break;



	}

	bool placeholder_true = true;

	auto& style = ImGui::GetStyle();
	float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;


}
void dpi_resize(float scale_factor, ImGuiStyle& style) //-V688
{
	ImGuiStyle& styles = ImGui::GetStyle();

	style.WindowPadding = (styles.WindowPadding * scale_factor);
	style.WindowRounding = (styles.WindowRounding * scale_factor);
	style.WindowMinSize = (styles.WindowMinSize * scale_factor);
	style.ChildRounding = (styles.ChildRounding * scale_factor);
	style.PopupRounding = (styles.PopupRounding * scale_factor);
	style.FramePadding = (styles.FramePadding * scale_factor);
	style.FrameRounding = (styles.FrameRounding * scale_factor);
	style.ItemSpacing = (styles.ItemSpacing * scale_factor);
	style.ItemInnerSpacing = (styles.ItemInnerSpacing * scale_factor);
	style.TouchExtraPadding = (styles.TouchExtraPadding * scale_factor);
	style.IndentSpacing = (styles.IndentSpacing * scale_factor);
	style.ColumnsMinSpacing = (styles.ColumnsMinSpacing * scale_factor);
	style.ScrollbarSize = (styles.ScrollbarSize * scale_factor);
	style.ScrollbarRounding = (styles.ScrollbarRounding * scale_factor);
	style.GrabMinSize = (styles.GrabMinSize * scale_factor);
	style.GrabRounding = (styles.GrabRounding * scale_factor);
	style.TabRounding = (styles.TabRounding * scale_factor);
	style.DisplayWindowPadding = (styles.DisplayWindowPadding * scale_factor);
	style.DisplaySafeAreaPadding = (styles.DisplaySafeAreaPadding * scale_factor);
	style.MouseCursorScale = (styles.MouseCursorScale * scale_factor);
}

void Menu::Initialize()
{
	CreateStyle();

	_visible = true;
}

void Menu::Shutdown()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Menu::OnDeviceLost()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Menu::OnDeviceReset()
{
	ImGui_ImplDX9_CreateDeviceObjects();
}

void Menu::Render()	
{
	ImGui::GetIO().MouseDrawCursor = _visible;

	RenderWatermark();





	if (!_visible) return;



	auto dpi_scale = g_Options.menu_scale / 10;
	ImGui::SetNextWindowSizeConstraints(ImVec2(691 * dpi_scale, 520 * dpi_scale), ImVec2(691 * dpi_scale, 520 * dpi_scale));


	if (ImGui::Begin("coded by snake | ba1m0v", &_visible, ImGuiWindowFlags_NoTitleBar || ImGuiWindowFlags_NoScrollbar || ImGuiWindowFlags_BackForce)) {
		static char* menu_tab_names[] = { "RAGEBOT","LEGITBOT", "VISUALS", "OTHER" ,"SKINS", "CONFIG" };
		static int active_menu_tab = -1;

		ImGuiWindow* window = GImGui->CurrentWindow;

		// mat sneika shluha

		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0, 0), ImVec2(4000, 4000), ImColor(0.f, 0.f, 0.f, 0.3f));


		ImGui::Image(smoke, ImVec2(window->Size.x, window->Size.y));

		ImGui::GetWindowDrawList()->AddRectFilled(window->Pos, window->Pos + window->Size, ImColor(0.14f, 0.13f, 0.14f, 0.85f));

		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(window->Pos, window->Pos + ImVec2(window->Size.x, 75), ImColor(0.11f, 0.1f, 0.11f, 1.0f), ImColor(0.11f, 0.1f, 0.11f, 1.0f), ImColor(0.11f, 0.1f, 0.11f, 0.4f), ImColor(0.11f, 0.1f, 0.11f, 0.4f));
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(window->Pos + ImVec2(212, 19), window->Pos + ImVec2(window->Size.x, 75), ImColor(0.0f, 0.0f, 0.0f, 0.15f), ImColor(0.0f, 0.0f, 0.0f, 0.15f), ImColor(0.0f, 0.0f, 0.0f, 0.00f), ImColor(0.0f, 0.0f, 0.0f, 0.00f));

		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(window->Pos, window->Pos + ImVec2(window->Size.x, 19), ImColor(0.11f, 0.10f, 0.11f, 0.7f), ImColor(0.11f, 0.10f, 0.11f, 0.7f), ImColor(0.16f, 0.15f, 0.16f, 0.8f), ImColor(0.16f, 0.15f, 0.16f, 0.8f));

		ImGui::GetWindowDrawList()->AddRectFilled(window->Pos + ImVec2(0, window->Size.y - 40), window->Pos + ImVec2(window->Size.x, window->Size.y), ImColor(0.0f, 0.0f, 0.0f, 0.15f));

		ImGui::GetWindowDrawList()->AddRectFilled(window->Pos + ImVec2(0, window->Size.y - 5), window->Pos + ImVec2(window->Size.x, window->Size.y), (ImColor(g_Options.menu_color[0], g_Options.menu_color[1], g_Options.menu_color[2], 0.8f)));
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(window->Pos + ImVec2(0, window->Size.y - 25), window->Pos + ImVec2(window->Size.x, window->Size.y), ImColor(g_Options.menu_color[0], g_Options.menu_color[1], g_Options.menu_color[2], 0.0f), ImColor(g_Options.menu_color[0], g_Options.menu_color[1], g_Options.menu_color[2], 0.0f), ImColor(g_Options.menu_color[0], g_Options.menu_color[1], g_Options.menu_color[2], 0.1f), ImColor(g_Options.menu_color[0], g_Options.menu_color[1], g_Options.menu_color[2], 0.1f));


		ImGui::GetWindowDrawList()->AddTextBig(window->Pos + ImVec2(60, 30), ImColor(0.75f, 0.75f, 0.75f, 1.0f), "Insight");

		const char* HeaderText_left = "Welcome back, User , Days left : 30";
		ImGui::GetWindowDrawList()->AddText(window->Pos + ImVec2(5, 2), ImColor(0.44f, 0.44f, 0.44f, 0.85f), HeaderText_left);

		const char* HeaderText_right = "Alpha build";
		ImGui::GetWindowDrawList()->AddText(window->Pos + ImVec2(3 + 620, 2), ImColor(0.44f, 0.44f, 0.44f, 0.85f), HeaderText_right);


		static auto ChildPose = [](int num) -> ImVec2 {
			return ImVec2(ImGui::GetWindowPos().x + 12 + (ImGui::GetWindowSize().x / 2 - 65) * num + 20 * num, ImGui::GetWindowPos().y + 42);
		};

		auto& style = ImGui::GetStyle();
		float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

		_style.Colors[ImGuiCol_CheckMark] = ImVec4(g_Options.menu_color[0], g_Options.menu_color[1], g_Options.menu_color[2], 1.f);

		ImGuiEX::TabButton("Aimbot", &active_menu_tab, 0, 6);
		ImGuiEX::TabButton("Visuals", &active_menu_tab, 1, 6);
		ImGuiEX::TabButton("Misc", &active_menu_tab, 2, 6);
		ImGuiEX::TabButton("Files", &active_menu_tab, 3, 6);




		switch (active_menu_tab)
		{


		case 0: // ragebot
			RenderRageBotTab();
			break;
		case 1: // esp
			RenderEspTab();
			break;
		case 2: // misc
			RenderMiscTab();
			break;
		case 3: // files
			RenderConfigTab();
			break;
		}


	} ImGui::End();

}

void Menu::Toggle()
{
	_visible = !_visible;
}

void Menu::CreateStyle()
{
	// IMGUI STYLES //
	ImGui::SetColorEditOptions(ImGuiColorEditFlags_HEX);
	_style.Alpha = 1.0f;
	_style.WindowPadding = ImVec2(8, 8);
	_style.WindowRounding = 0.0f;
	_style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	_style.FramePadding = ImVec2(4, 3);
	_style.FrameRounding = 6.0f;
	_style.ChildRounding = 8.0f;
	_style.ItemSpacing = ImVec2(8, 9);
	_style.ItemInnerSpacing = ImVec2(4, 4);
	_style.TouchExtraPadding = ImVec2(0, 0);
	_style.IndentSpacing = 21.0f;
	_style.ColumnsMinSpacing = 6.0f;
	_style.ScrollbarSize = 10.0f;
	_style.ScrollbarRounding = 7.0f;
	_style.GrabMinSize = 10.0f;
	_style.GrabRounding = 6.0f;
	_style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	_style.DisplayWindowPadding = ImVec2(22, 22);
	_style.DisplaySafeAreaPadding = ImVec2(4, 4);
	_style.AntiAliasedLines = true;
	_style.CurveTessellationTol = 1.25f;

	static bool Textured3 = false;
	if (Menu::Get().smoke == nullptr && !Textured3)
	{
		D3DPRESENT_PARAMETERS pp = {};
		D3DXCreateTextureFromFileInMemoryEx(g_D3DDevice9, &texture2, 54284, 1000, 1000, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &Menu::Get().smoke);
		Textured3 = true;
	}

	static bool Textured4 = false;
	if (Menu::Get().model == nullptr && !Textured4)
	{
		D3DPRESENT_PARAMETERS pp = {};
		D3DXCreateTextureFromFileInMemoryEx(g_D3DDevice9, &model22, 215590, 1000, 1000, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &Menu::Get().model);
		Textured4 = true;
	}

	static bool Textured5 = false;
	if (Menu::Get().model_flat == nullptr && !Textured5)
	{
		D3DPRESENT_PARAMETERS pp = {};
		D3DXCreateTextureFromFileInMemoryEx(g_D3DDevice9, &model22_flat, 25177, 1000, 1000, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &Menu::Get().model_flat);
		Textured5 = true;
	}

	static bool Textured6 = false;
	if (Menu::Get().model_regular == nullptr && !Textured6)
	{
		D3DPRESENT_PARAMETERS pp = {};
		D3DXCreateTextureFromFileInMemoryEx(g_D3DDevice9, &model4, 93888, 1000, 1000, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &Menu::Get().model_regular);
		Textured6 = true;
	}

	static bool Textured7 = false;
	if (Menu::Get().model_met == nullptr && !Textured7)
	{
		D3DPRESENT_PARAMETERS pp = {};
		D3DXCreateTextureFromFileInMemoryEx(g_D3DDevice9, &model6, 120403, 1000, 1000, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &Menu::Get().model_met);
		Textured7 = true;
	}

	// IMGUI COLORS //huehuehuehuehuehuehuehuehue

	static int hue = 140;
	ImVec4 col_text = ImColor(170, 170, 170);
	ImVec4 col_main = ImColor(36, 33, 36);
	ImVec4 col_back = ImColor(36, 33, 36);
	ImVec4 col_area = ImColor(56, 53, 56);

	_style.Colors[ImGuiCol_Text] = ImVec4(170 / 255.f, 170 / 255.f, 170 / 255.f, 1.00f);
	_style.Colors[ImGuiCol_TextDisabled] = ImVec4(70, 70, 70, 1.00f);
	_style.Colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.13f, 0.14f, 1.0f);
	_style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.11f, 0.1f, 0.11f, 0.6f);
	_style.Colors[ImGuiCol_Border] = ImVec4(0.27f, 0.27f, .27f, 1.00f);
	_style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	_style.Colors[ImGuiCol_FrameBg] = ImVec4(0.14f, 0.13f, 0.14f, 1.f);
	_style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.14f, 0.13f, 0.14f, 0.8f);
	_style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.11, 0.11, 0.11, 1.f);
	_style.Colors[ImGuiCol_TitleBg] = ImVec4(0.16f, 0.15f, 0.16f, 1.0f);
	_style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.16f, 0.15f, 0.16f, 1.0f);
	_style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.15f, 0.16f, 1.0f);
	_style.Colors[ImGuiCol_MenuBarBg] = ImVec4(.52f, 0.f, 0.52f, .7f);;
	_style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.95f);
	_style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.50f, 0.50f, 0.50f, 0.30f); //main half
	_style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.14f, 0.13f, 0.14f, 0.8f); //main half
	_style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.76, 0.08, 0.74, 1.f);
	_style.Colors[ImGuiCol_CheckMark] = ImVec4(g_Options.menu_color[0], g_Options.menu_color[1], g_Options.menu_color[2], 1.f);
	_style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.14f, 0.13f, 0.14f, 1.f); //main half
	_style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.76, 0.08, 0.74, 1.f);
	_style.Colors[ImGuiCol_Button] = ImVec4(0.14f, 0.13f, 0.14f, 1.f);
	_style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.14f, 0.13f, 0.14f, 0.7f);
	_style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.14f, 0.13f, 0.14f, 0.5f);
	_style.Colors[ImGuiCol_Header] = ImVec4(0.14f, 0.13f, 0.14f, 1.f);
	_style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.18f, 0.17f, 0.18f, 1.f); // combobox hover
	_style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.16f, 0.15f, 0.16f, 1.f);
	_style.Colors[ImGuiCol_Column] = ImVec4(col_text.x, col_text.y, col_text.z, 0.32f);
	_style.Colors[ImGuiCol_ColumnHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.78f);
	_style.Colors[ImGuiCol_ColumnActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	_style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
	_style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	_style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	_style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	_style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	_style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	_style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	_style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
	_style.Colors[ImGuiCol_PopupBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.92f);
	_style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	ImGui::GetStyle() = _style;
}
