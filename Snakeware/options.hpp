#pragma once

#include <string>
#include <memory>
#include <vector>
#include "valve_sdk/Misc/Color.hpp"
#include <map>
#include "valve_sdk/csgostructs.hpp"
#include <limits>
// think about it
#include "skin-changer/kit_parser.h"
#include "skin-changer/item_definitions.h"
#include "features/ragebot/lagcompensation/lag-compensation.h"

#define A( s ) #s
#define OPTION(type, var, val) type var = val // changed

template <typename T = bool>
class Var {
public:
	std::string name;
	std::shared_ptr<T> value;
	int32_t size;
	Var(std::string name, T v) : name(name) {
		value = std::make_shared<T>(v);
		size = sizeof(T);
	}
	operator T() { return *value; }
	operator T* () { return &*value; }
	operator T() const { return *value; }

};
class CUserCmd;
namespace Snakeware
{

	extern float flNextSecondaryAttack;
	extern int MissedShots[65];
	extern int CmdTick;
	extern C_BasePlayer* GetPlayer;
	extern int BaseTick;
	extern Vector HeadPos;
	extern CUserCmd* cmd;
	extern bool bAimbotting;
	extern bool			  LBY_Update;
	extern QAngle LocalAngle;
	extern QAngle FakeAngle;
	extern QAngle RealAngle;
	extern Vector Angle;
	extern QAngle Aimangle;
	extern bool bSendPacket;
	extern bool OnShot;
	extern bool UpdateAnims;
	extern bool DoubleTapCharged;
	extern uint32_t ShotCmd;
	extern int UnpredTick;
	extern float g_flVelocityModifer;
	extern bool g_bOverrideVelMod;
	extern int m_nTickbaseShift;
	extern int m_nBaseTick;
	extern int SkipTicks;
	extern bool bBoneSetup;
	extern matrix3x4_t mOnShotMatrix[MAXSTUDIOBONES];
	extern matrix3x4_t FakeMatrix[128];
	extern matrix3x4_t FakeLagMatrix[128];
	extern bool bVisualAimbotting;
	extern bool bShotFound;
	extern QAngle vecVisualAimbotAngs;


	extern std::string EyeDelta;
	extern std::string Delta;
	extern std::string Delta2;
	extern std::string Delta3;
	extern std::string Delta4;

	

}
class Cmd {
public:
	void SetCommand(CUserCmd* cmd) { cmdshka = cmd; }
	CUserCmd* GetCommand();
private:
	CUserCmd* cmdshka;
};
extern Cmd g_Cmd;
struct item_setting
{
	char name[32] = "default";
	bool enabled = false;
	int definition_vector_index = 0;
	int definition_index = 1;
	int paint_kit_vector_index = 0;
	int paint_kit_index = 0;
	int definition_override_vector_index = 0;
	int definition_override_index = 0;
	int seed = 0;
	int stat_trak = 0;
	float wear = 0;
	char custom_name[32] = "";
};
struct legitbot_settings {
	bool enabled = false;
	bool deathmatch = false;
	bool backtrack = false;
	bool autopistol = false;
	bool smoke_check = false;
	bool flash_check = false;
	bool jump_check = false;
	bool autowall = false;
	bool silent = false;
	bool autofire = false;
	bool on_key = true;
	bool rcs = false;
	bool rcs_fov_enabled = false;
	bool rcs_smooth_enabled = false;
	bool autostop = false;
	float backtrack_time = 0.0f;
	bool only_in_zoom = true;
	int autofire_key = 1;
	int key = 1;
	int aim_type = 1;
	int priority = 0;
	int fov_type = 0;
	int smooth_type = 0;
	int rcs_type = 0;
	int hitbox = 1;
	float fov = 0.f;
	float silent_fov = 0.f;
	float rcs_fov = 0.f;
	float smooth = 1;
	float rcs_smooth = 1;
	int shot_delay = 0;
	int kill_delay = 0;
	int rcs_x = 100;
	int rcs_y = 100;
	int rcs_start = 1;
	int min_damage = 1;


	// triggerbot

	bool triggerbot_enable;
	bool triggerbot_head;
	bool triggerbot_body;
	bool triggerbot_other;
	int triggerbot_delay;
	float triggerbot_hitchance;
};


enum WEAPON_GROUPS {
	PISTOLS,
	H_PISTOLS,
	RIFLES,
	SHOTGUNS,
	SCOUT,
	AUTO,
	AWP,
	SMG,
	UNKNOWN
};


class Options
{
public:



	std::map <int, legitbot_settings> legitbot_items = { };



	// 
	// ESP
	// 
	OPTION(bool, legitbot_auto_weapon, false);
	OPTION(bool, legitbot_trigger_pressed, false);
	OPTION(int, legitbot_trigger_key, 0);
	OPTION(bool, remove_sleeves, false);
	OPTION(bool, remove_smoke, false);
	OPTION(bool, remove_flash, false);
	OPTION(bool, remove_visual_recoil, false);
	OPTION(bool, remove_scope, false);

	OPTION(bool, esp_enabled, false);
	OPTION(bool, shot_hitboxes, false);
	OPTION(float, shot_hitboxes_duration, 1.f);
	OPTION(int, shot_hitboxes_type, 0);
	OPTION(bool, sound_esp, false);
	OPTION(int, sound_esp_type, 0);
	OPTION(float, sound_esp_radius, 12.0f);
	OPTION(float, sound_esp_time, 0.8f);
	OPTION(bool, esp_enemies_only, false);
	OPTION(bool, esp_player_boxes, false);
	OPTION(bool, esp_player_ammo, false);
	OPTION(int, esp_player_boxes_type, 1);
	OPTION(bool, esp_player_names, false);
	OPTION(bool, esp_player_skeleton, false);
	OPTION(bool, esp_head_dot, false);
	OPTION(bool, esp_player_health, false);
	OPTION(bool, esp_player_armour, false);

	OPTION(bool, esp_player_flags, false);
	OPTION(bool, esp_player_flags_scoped, false);
	OPTION(bool, esp_player_flags_player, false);
	OPTION(bool, esp_flags_kit, false);
	OPTION(bool, esp_molotov_timer, false);
	OPTION(bool, esp_flags_move, false);
	OPTION(bool, esp_flags_armour, false);
	OPTION(bool, esp_flags_choking, false);
	OPTION(bool, esp_flags_flash, false);
	OPTION(float, esp_model_ambient, 0.000);
	OPTION(bool, esp_player_weapons, false);
	OPTION(int, esp_player_weapons_type, 0);
	OPTION(bool, esp_player_snaplines, false);
	OPTION(bool, esp_crosshair, false);
	OPTION(bool, esp_recoil_crosshair, false);
	OPTION(bool, esp_dropped_weapons, false);
	OPTION(bool, esp_defuse_kit, false);
	OPTION(bool, esp_planted_c4, false);
	OPTION(bool, esp_items, false);
	OPTION(bool, esp_nade_prediction, false);
	OPTION(bool, esp_nightmode, false);
	OPTION(bool, esp_fullbright, false);
	OPTION(float, esp_nightmode_bright, 1);
	OPTION(bool, esp_draw_fov, false);
	OPTION(bool, esp_angle_lines, false);
	OPTION(bool, esp_bloom_effect, false);
	OPTION(int, esp_bloom_factor, 1);
	OPTION(int, esp_bloom_scale, 0);
	OPTION(int, esp_font_size, 25);
	OPTION(int, esp_flash_ammount, 0);

	OPTION(bool, esp_offscreen, false);
	OPTION(float, esp_offscreen_size, 15.f);
	OPTION(float, esp_offscreen_range, 215.f);
	// 
	// GLOW
	// 
	OPTION(bool, glow_enabled, false);
	OPTION(int, glow_type, 1);
	OPTION(int, glow_team_type, 1);
	OPTION(bool, glow_enemies_only, false);
	OPTION(bool, glow_players, false);
	OPTION(bool, glow_chickens, false);
	OPTION(bool, glow_c4_carrier, false);
	OPTION(bool, glow_planted_c4, false);
	OPTION(bool, glow_defuse_kits, false);
	OPTION(bool, glow_weapons, false);

	//
	// CHAMS
	//
	OPTION(bool, chams_player_enabled, false);
	OPTION(bool, chams_player_enabled_l, false);
	OPTION(bool, chams_hands_enabled_l, false);
	OPTION(bool, chams_weapon_enabled_l, false);
	OPTION(bool, chams_fake, false);
	OPTION(int, chams_fake_types, 0);
	OPTION(bool, chams_player_backtrack, false);
	OPTION(int, chams_backtrack_types, 0);
	OPTION(int, chams_player_backtrack_type, 0);
	OPTION(int, chams_player_type, 0);
	OPTION(int, chams_player_type_layer, 0);
	//chams_player_type_layer
	OPTION(int, chams_arms_type, 0);
	OPTION(int, chams_arms_type_layer, 0);
	//chams_arms_type_layer
	OPTION(bool, chams_player_enemies_only, false);
	OPTION(bool, chams_player_wireframe, false);
	OPTION(bool, chams_player_flat, false);
	OPTION(bool, chams_player_ignorez, false);
	OPTION(bool, chams_player_glass, false);
	OPTION(bool, chams_arms_enabled, false);
	OPTION(bool, chams_arms_wireframe, false);
	OPTION(bool, chams_arms_flat, false);
	OPTION(bool, chams_arms_ignorez, false);
	OPTION(bool, chams_arms_glass, false);
	OPTION(float, chams_phonk, 0.f);
	OPTION(float, chams_pearles, 0.f);
	OPTION(float, esp_aspect_ratio, 0.f);

	OPTION(bool, chams_weapons, false);
	OPTION(int, chams_weapons_type, 0);
	OPTION(int, onshot_chams_type, 0);
	OPTION(int, chams_weapons_type_layer, 0);
	//
	// MISC
	//
	OPTION(bool, misc_bhop, false);
	OPTION(bool, misc_unlock_inventory, false);
	OPTION(bool, misc_exaggerated_ragdolls, false);
	OPTION(int, misc_exaggerated_ragdolls_force, 10);
	OPTION(bool, misc_fake_fps, false);
	OPTION(bool, misc_clantag, false);
	OPTION(bool, misc_chat_spam, false);
	OPTION(bool, misc_spectator_list, false);
	OPTION(bool, misc_autostrafe, false);
	OPTION(bool, misc_anti_untrusted, true);
	OPTION(bool, misc_anti_screenshot, false);
	OPTION(bool, misc_no_hands, false);
	OPTION(bool, misc_legit_resolver, false);
	OPTION(bool, misc_jump_throw, false);
	OPTION(bool, misc_bullet_tracer, false);
	OPTION(bool, misc_bullet_impacts, false);
	OPTION(bool, misc_thirdperson, false);
	OPTION(bool, misc_showranks, true);
	OPTION(bool, misc_watermark, true);
	OPTION(float, misc_thirdperson_dist, 50.f);
	OPTION(int, misc_thirdperson_key, 7);
	OPTION(int, viewmodel_fov, 68);
	OPTION(int, world_fov, 3);
	OPTION(float, mat_ambient_light_r, 0.0f);
	OPTION(float, mat_ambient_light_g, 0.0f);
	OPTION(float, mat_ambient_light_b, 0.0f);
	OPTION(bool, misc_fakelag, false);
	OPTION(bool, misc_knifebot, false);
	OPTION(bool, misc_auto_knifebot, false);
	OPTION(bool, misc_knifebot_360, false);
	OPTION(bool, misc_hitmarker, false);
	OPTION(bool, misc_hiteffect, false);
	OPTION(float, misc_hiteffect_duration, 2.f);
	OPTION(float, misc_hitmarker_size, 13.f);
	OPTION(bool, misc_hitsound, false);
	OPTION(int, misc_hitsound_type, 0);
	OPTION(bool, misc_fakelag_on_shot, false);
	OPTION(bool, misc_radar, false);
	OPTION(bool, misc_left_knife, false);
	OPTION(float, misc_radar_range, 0.0f);
	OPTION(bool, misc_fakelag_safety, false);
	OPTION(bool, misc_engine_radar, false);
	OPTION(int, misc_fakelag_ticks, 0);
	OPTION(int, misc_fakelag_triggered_ticks, 0);
	OPTION(int, misc_fakelag_type, 0);
	OPTION(bool, misc_legit_antihit, false);
	OPTION(bool, misc_legit_antihit_lby, false);
	OPTION(int, misc_legit_antihit_key, 0);
	OPTION(bool, exploit_doubletap, false);
	OPTION(bool, exploit_hideshots, false);
	OPTION(int, exploit_doubletap_key, 0);
	OPTION(bool, misc_infinity_duck, false);
	OPTION(bool, misc_slowwalk, false);
	OPTION(int, misc_slowwalk_key, 0);
	OPTION(int, misc_slowwalk_speed, 30);
	OPTION(bool, misc_fakeduck, false);
	OPTION(int, misc_fakeduck_key, 0);
	OPTION(int, misc_fakeduck_ticks, 14);

	OPTION(bool, misc_event_log, false);
	OPTION(bool, event_log_hit, false);
	OPTION(bool, event_log_item, false);
	OPTION(bool, event_log_plant, false);
	OPTION(bool, event_log_defuse, false);

	bool  misc_fakelag_triggers[4] = { false, false, false, false};
	// 
	// COLORS
	// 
	float color_menu_ui[4] = { 0.1f, 0.1f, 1.f, 1.0f };
	float color_sound_esp[4] = { 1.f, 0.1f, 1.f, 1.0f };
	float color_esp_ally_visible[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float color_esp_enemy_visible[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float color_esp_ally_occluded[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float color_esp_enemy_occluded[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float color_esp_crosshair[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float color_esp_offscreen[4] = { 0.3f, 0.1f, 1.f, 1.0f };
	float color_molotov[4] = { 0.3f, 0.1f, 1.f, 1.0f };
	OPTION(Color, color_esp_weapons, Color(128, 0, 128));
	OPTION(Color, color_esp_defuse, Color(0, 128, 255));
	OPTION(Color, color_esp_c4, Color(255, 255, 0));
	OPTION(Color, color_esp_item, Color(255, 255, 255));

	float color_glow_ally[4] = { 0.1f, 0.1f, 1.f, 1.0f };
	float color_glow_enemy[4] = { 1.f, 0.1f, 0.1f, 1.0f };
	OPTION(Color, color_glow_chickens, Color(0, 128, 0));
	OPTION(Color, color_glow_c4_carrier, Color(255, 255, 0));
	OPTION(Color, color_glow_planted_c4, Color(128, 0, 128));
	OPTION(Color, color_glow_defuse, Color(255, 255, 255));
	OPTION(Color, color_glow_weapons, Color(255, 128, 0));

	float color_chams_player_ally_visible[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float color_shot_hitboxes[4] = { 0.9f, 0.1f, 0.85f, 1.0f };
	float color_head_dot[4] = { 1.f, 1.f, 1.f, 1.0f };
	float color_player_chams_backtrack[4] = { 1.f, 1.f, 1.f, 1.0f };
	float color_hitmarker[4] = { 1.f, 1.f, 1.f, 1.0f };
	float color_bullet_tracer[4] = { 0.f, 0.f, 1.f, 1.0f };
	float color_chams_weapons[4] = { 0.f, 0.f, 0.9f, 1.0f };
	float color_chams_weapons_layer[4] = { 0.f, 0.f, 0.9f, 1.0f };
	float color_chams_fake[4] = { 0.f, 0.f, 1.f, 1.0f };
	float color_chams_player_ally_occluded[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float color_chams_player_enemy_visible[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float color_chams_player_enemy_visible_l[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float color_chams_player_enemy_occluded[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float color_chams_arms_visible[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float color_chams_arms_visible_layer[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float sky_color[4] = { 1.f, 0.f, 1.f, 0.9f };
	float world_color[4] = { 1.f, 0.f, 1.f, 1.f };
	float prop_color[4] = { 1.f, 0.f, 1.f, 0.9f };
	int prop_material;

	float menu_color[4] = { 1.f, 0.f, 1.f, 1.0f };
	int   menu_scale = 10;

	OPTION(Color, color_chams_arms_occluded, Color(0, 128, 255));
	OPTION(Color, color_watermark, Color(0, 128, 255)); // no menu config cuz its useless



	OPTION(bool, legit_desync_switch, false);
	OPTION(bool, rage_desync_switch, false);


	//
	// ANTIHIT
	//

	OPTION(bool, antihit_enabled, false);
	OPTION(bool, antihit_stabilize_lby, false);
	OPTION(int, antihit_pitch, 0);
	OPTION(int, antihit_yaw, 0);
	OPTION(int, antihit_lby, 0);
	OPTION(int, antihit_jitter_type, 0);
	OPTION(int, antihit_fake, 0);
	OPTION(int, antihit_jitter_radius, 0);
	OPTION(int, antihit_body_lean, 0);
	OPTION(int, antihit_invert_body_lean, 0);
	OPTION(int, antihit_fake_switch_key, 5);
	OPTION(float, antihit_fake_ammount, 58);

	// MANUAL ANTIHIT
	OPTION(int, antihit_manual_left, 40);
	OPTION(int, antihit_manual_right, 41);
	OPTION(int, antihit_manual_back, 42);
	
	OPTION(int, misc_silentwalk_key, 55);
	OPTION(int, autopeek_key, 55);
	OPTION(int, playerModelT, 0);
	OPTION(int, playerModelCT, 0);


	OPTION(bool, ragebot_enabled, false);
	OPTION(bool, ragebot_ignore_limbs, false);
	OPTION(bool, ragebot_autowall, false);
	OPTION(bool, ragebot_remove_recoil, false);
	OPTION(bool, ragebot_resolver, false);
	OPTION(bool, ragebot_remove_spread, false);
	
	OPTION(bool, ragebot_autofire, false);
	OPTION(bool, ragebot_hitchance_consider, false);
	OPTION(bool, ragebot_position_adj, false);
	OPTION(bool, ragebot_limit_fov, false);
	OPTION(bool, ragebot_multipoint, false);
	OPTION(bool, ragebot_beetweenshots, false);
	bool  ragebot_alternative_hitchance[9] = { false, false, false, false, false, false, false, false };
	bool  ragebot_delayshot[9] = { false, false, false, false, false, false, false, false };
	bool  ragebot_autoscope;
	bool  ragebot_autostop;
	bool  ragebot_autostop_bs;
	bool   ragebot_silent;
	int   ragebot_autostop_type;
	bool  ragebot_autostop_if[9] = { false, false, false, false, false, false, false, false };
	bool  ragebot_static_pointscale[9] = { false, false, false, false, false, false, false, false };
	bool  ragebot_autocrouch[9] = { false, false, false, false, false, false, false, false };
	bool  ragebot_baim_in_air[9] = { false, false, false, false, false, false, false, false };
	bool  ragebot_baim_in_move[9] = { false, false, false, false, false, false, false, false };
	bool  ragebot_baim_if_lethal[9] = { false, false, false, false, false, false, false, false };
	bool  ragebot_adaptive_baim[9] = { false, false, false, false, false, false, false, false };
	int ragebot_mindamage[9] = { 0, 0, 0, 0, 0 , 0, 0, 0, 0, };
	int ragebot_vis_mindamage[9] = { 0, 0 ,0, 0, 0, 0, 0, 0, 0, };
	float ragebot_mindamage_override[9] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	float ragebot_hitchance[9] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	float ragebot_pointscale[9] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	float ragebot_bodyscale[9] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	float ragebot_otherscale[9] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	float ragebot_baim_if_hp[9] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	OPTION(int, ragebot_mindamage_override_key, 22);
	OPTION(int, ragebot_baim_key, 23);
	OPTION(int, ragebot_selection, 1);
	OPTION(int, ragebot_fov, 228);
	OPTION(int, ragebot_max_miss, 5);
	bool            ragebot_hitbox[8][9] = { false, false, false, false, false, false, false, false };
	bool            ragebot_baimhitbox[8][9] = { false, false, false, false, false, false, false, false };
	OPTION(int, ragebot_force_safepoint, 104);
	//
	// RAGEBOT
	// 

	int cheatmode;


	bool loaded_lua[100];

	struct {
		std::map<int, item_setting> m_items;
		std::unordered_map<std::string, std::string> m_icon_overrides;
		auto get_icon_override(const std::string original) const -> const char*
		{
			return m_icon_overrides.count(original) ? m_icon_overrides.at(original).data() : nullptr;
		}
	} m_skins;




};
extern int realAimSpot[];
extern int realHitboxSpot[];
inline Options g_Options;
inline bool   g_Unload;