#include "config-system.h"
#include "../options.hpp"
#include "../Lua/API.h"
#include "../options.hpp"

#include <fstream>
#include <filesystem> // hack
#include <lua.h>
#include <lua.hpp>

void ConfigSystem::SetupValue(int& value, int def, std::string category, std::string name) { value = def; ints.push_back(new ConfigValue< int >(category, name, &value, def)); }
void ConfigSystem::SetupValue(char* value, char* def, std::string category, std::string name) { value = def; chars.push_back(new ConfigValue< char >(category, name, value, *def)); }
void ConfigSystem::SetupValue(float& value, float def, std::string category, std::string name) { value = def; floats.push_back(new ConfigValue< float >(category, name, &value, def)); }
void ConfigSystem::SetupValue(bool& value, bool def, std::string category, std::string name) { value = def; bools.push_back(new ConfigValue< bool >(category, name, &value, def)); }
void ConfigSystem::SetupColor(float value[4], std::string name) {
	SetupValue(value[0], value[0], ("colors"), name + "_r");
	SetupValue(value[1], value[1], ("colors"), name + "_g");
	SetupValue(value[2], value[2], ("colors"), name + "_b");
	SetupValue(value[3], value[3], ("colors"), name + "_a");
}

void ConfigSystem::SetupRage()
{
		SetupValue(g_Options.ragebot_enabled, false, ( "ragebot"), ("enabled"));
		SetupValue(g_Options.ragebot_silent, false, ("ragebot"), ("ragebotsilent"));
		SetupValue(g_Options.ragebot_autowall, false, ("ragebot"), ("autowall"));
		SetupValue(g_Options.ragebot_remove_recoil, false, ("ragebot"), ("remove_recoil"));
		SetupValue(g_Options.ragebot_autofire, false, ("ragebot"), ("autofire"));
		SetupValue(g_Options.ragebot_fov, 228,       ("ragebot"),  ("ragebot_fov"));
		SetupValue(g_Options.ragebot_autostop, false, ("ragebot"), ("autostop"));
		SetupValue(g_Options.ragebot_autostop_bs, false, ("ragebot"), ("autostopbs"));
		SetupValue(g_Options.ragebot_autostop_type, 0, ("ragebot"), ("auto_stoptype"));

		for (int type = WEAPON_GROUPS::PISTOLS; type <= WEAPON_GROUPS::SMG; type++) {
			SetupValue(g_Options.ragebot_mindamage[type], 0, ("ragebot"), ("ragebot_mindamage"));
			SetupValue(g_Options.ragebot_vis_mindamage[type], 0, ("ragebot"), ("ragebot_vismindamage"));
			SetupValue(g_Options.ragebot_hitchance[type], 0, ("ragebot"), ("ragebot_hitchance"));
			SetupValue(g_Options.ragebot_pointscale[type], 0, ("ragebot"), ("ragebot_pointscale"));
			SetupValue(g_Options.ragebot_bodyscale[type], 0, ("ragebot"), ("ragebot_bodyscale"));
			SetupValue(g_Options.ragebot_otherscale[type], 0, ("ragebot"), ("ragebot_otherscale"));
		

		}





		// antiaim :
		SetupValue(g_Options.antihit_enabled, false, ("antihit"), ("enabledaa"));
		SetupValue(g_Options.antihit_stabilize_lby, false, ("antihit"), ("stabilizelby"));
		SetupValue(g_Options.antihit_pitch, 0, ("antihit"), ("pitch"));
		SetupValue(g_Options.antihit_yaw,   0, ("antihit"), ("yaw"));
		SetupValue(g_Options.antihit_fake, 0, ("antihit"), ("fake"));
		SetupValue(g_Options.antihit_lby, 0, ("antihit"), ("lby"));
		SetupValue(g_Options.antihit_jitter_type, 0, ("antihit"), ("jitter"));
		SetupValue(g_Options.antihit_jitter_radius, 0, ("antihit"), ("jitter_rad"));
		SetupValue(g_Options.antihit_fake_switch_key, 0, ("antihit"), ("fake_switch"));
		SetupValue(g_Options.antihit_fake_ammount, 58, ("antihit"), ("fake_ammount"));

}
void ConfigSystem::SetupVisuals()
{
	SetupValue(g_Options.sound_esp, false, ("sound-esp"), ("sound_enabled"));
	
	SetupValue(g_Options.sound_esp, 12, ("sound-esp"), ("sound_esp_radius"));
	SetupValue(g_Options.sound_esp_type, 0, ("sound-esp"), ("sound_esp_type"));
	SetupValue(g_Options.esp_enabled, false, ("player-esp"), ("enabled"));
	SetupValue(g_Options.esp_enemies_only, false, ("player-esp"), ("enemies_onlyesp"));
	SetupValue(g_Options.esp_player_boxes, false, ("player-esp"), ("boxes"));
	SetupValue(g_Options.esp_player_boxes_type, 1, ("player-esp"), ("boxes_type"));
	SetupValue(g_Options.esp_player_names, false, ("player-esp"), ("names"));
	SetupValue(g_Options.esp_player_health, false, ("player-esp"), ("health"));
	SetupValue(g_Options.esp_player_armour, false, ("player-esp"), ("armour"));
	SetupValue(g_Options.esp_player_weapons, false, ("player-esp"), ("weapons"));
	SetupValue(g_Options.esp_player_snaplines, false, ("player-esp"), ("snaplines"));
	SetupValue(g_Options.esp_player_skeleton, false, ("player-esp"), ("skeleton"));
	SetupValue(g_Options.esp_head_dot, false, ("player-esp"), ("head_dot"));
	SetupValue(g_Options.esp_player_flags, false, ("player-esp"), ("flags"));
	SetupValue(g_Options.esp_player_flags_player, false, ("player-esp"), ("flags_player"));
	SetupValue(g_Options.esp_player_flags_scoped, false, ("player-esp"), ("flags_scoped"));
	SetupValue(g_Options.esp_flags_kit, false, ("player-esp"), ("flags_kit"));
	SetupValue(g_Options.esp_flags_armour, false, ("player-esp"), ("flags_armour"));
	SetupValue(g_Options.esp_flags_move, false, ("player-esp"), ("flags_move"));
	SetupValue(g_Options.esp_flags_choking, false, ("player-esp"), ("flags_choke"));
	SetupValue(g_Options.esp_flags_flash, false, ("player-esp"), ("flags_flash"));
	SetupValue(g_Options.sound_esp, false, ("player-esp"), ("sound_esp"));
	SetupValue(g_Options.sound_esp_radius, 12, ("player-esp"), ("sound_esp_radius"));
	// glow
	SetupValue(g_Options.glow_enabled, false, ("player-glow"), ("glow_enabled"));
	SetupValue(g_Options.glow_players, false, ("player-glow"), ("glow_players"));
	SetupValue(g_Options.glow_enemies_only, false, ("player-glow"), ("glow_enemies"));
	SetupValue(g_Options.glow_type, 1, ("player-glow"), ("glow_type"));
	SetupValue(g_Options.glow_team_type, 1, ("player-glow"), ("glow_team_type"));
	//chams
	SetupValue(g_Options.chams_player_enabled, false, ("player-chams"), ("chams_enabled"));
	SetupValue(g_Options.chams_player_enabled_l, false, ("player-chams"), ("chams_ploverlay_enabled"));
	SetupValue(g_Options.chams_player_type_layer, 1, ("player-chams"), ("chams_type_l"));
	SetupValue(g_Options.chams_player_type, 1, ("player-chams"), ("chams_type"));
	SetupValue(g_Options.chams_player_ignorez, false, ("player-chams"), ("chams_ignore_wall"));
	SetupValue(g_Options.chams_player_enemies_only, false, ("player-chams"), ("chams_ignore_team"));
	SetupValue(g_Options.chams_arms_enabled, false, ("player-arms"), ("chams_arms"));
	SetupValue(g_Options.misc_no_hands, false, ("player-arms"), ("no_arms"));
	SetupValue(g_Options.chams_hands_enabled_l, false, ("player-arms"), ("arms_overlay"));
	SetupValue(g_Options.chams_arms_type, 1, ("player-arms"), ("arms_type"));
	SetupValue(g_Options.chams_arms_type_layer, 1, ("player-arms"), ("arms_layer_type"));
	SetupValue(g_Options.viewmodel_fov, 69, ("viewmodel"), ("viewmodel_type"));

	SetupValue(g_Options.chams_weapons, false, ("player-weapon"), ("chams_weapons"));
	SetupValue(g_Options.chams_weapons_type, 1, ("player-weapon"), ("chams_weapon_type"));

	SetupValue(g_Options.chams_weapon_enabled_l, false, ("player-weapon"), ("chams_weaponsl_enabled"));
	SetupValue(g_Options.chams_weapons_type_layer, 1, ("player-weapon"), ("chams_weapons_type_layer"));
 
	SetupValue(g_Options.world_fov, 3, ("viewmodel"), ("world_fov"));
	SetupValue(g_Options.esp_aspect_ratio, 0, ("viewmodel"), ("aspect_ratio"));

	SetupValue(g_Options.esp_nightmode, false, ("world"), ("nightmode"));
	SetupValue(g_Options.remove_flash, false, ("world"), ("removeflash"));
	SetupValue(g_Options.remove_smoke, false, ("world"), ("removesmoke"));
	SetupValue(g_Options.remove_scope, false, ("world"), ("removescope"));
	SetupValue(g_Options.remove_visual_recoil, false, ("world"), ("remove_visrecoil"));
	SetupValue(g_Options.esp_flash_ammount, 1, ("world"), ("flash_ammount"));
	SetupValue(g_Options.esp_fullbright, false, ("world"), ("fullbright"));
	SetupValue(g_Options.esp_bloom_effect, false, ("world"), ("bloom"));
	SetupValue(g_Options.esp_bloom_factor, 1, ("world"), ("bloom_value"));
	SetupValue(g_Options.esp_model_ambient, 1, ("world"), ("model_ambient"));
	SetupValue(g_Options.esp_nightmode_bright, 1, ("world"), ("nightmode_bright"));
	SetupValue(g_Options.mat_ambient_light_r, 0, ("world"), ("world_red"));
	SetupValue(g_Options.mat_ambient_light_g, 0, ("world"), ("world_green"));
	SetupValue(g_Options.mat_ambient_light_b, 0, ("world"), ("world_blue"));
	SetupValue(g_Options.esp_nade_prediction, false, ("world"), ("nade_prediction"));
	SetupValue(g_Options.esp_crosshair, false, ("other-esp"), ("draw_crosshair"));
	SetupValue(g_Options.esp_draw_fov, false, ("other-esp"), ("draw_fov"));
	SetupValue(g_Options.esp_angle_lines, false, ("other-esp"), ("draw_angles"));
	SetupValue(g_Options.shot_hitboxes, false, ("world"), ("draw_hc"));
	SetupValue(g_Options.shot_hitboxes_duration, 1, ("world"), ("draw_hc_duration"));
}
void ConfigSystem::SetupMisc()
{
	SetupColor(g_Options.menu_color, "menu_color");
	SetupValue(g_Options.legitbot_trigger_key, 1, ("legitbot"), ("trigger_key"));
	SetupValue(g_Options.esp_font_size, 25, ("misc-extra"), ("font_size"));
	SetupValue(g_Options.misc_anti_untrusted, true, ("misc-extra"), ("anti_untrusted"));
	SetupValue(g_Options.misc_infinity_duck, false, ("misc-extra"), ("infinity_duck"));
	SetupValue(g_Options.misc_anti_screenshot, false, ("misc-extra"), ("anti_screenshot"));
	SetupValue(g_Options.misc_legit_resolver, false, ("misc-extra"), ("legit_resolver"));
	SetupValue(g_Options.misc_legit_antihit, false, ("misc-extra"), ("legit_antihit"));
	SetupValue(g_Options.misc_legit_antihit_lby, false, ("misc-extra"), ("legit_antihit_lby"));
	SetupValue(g_Options.misc_legit_antihit_key, 3, ("misc-extra"), ("antihit_key"));

	SetupValue(g_Options.misc_bhop, false, ("movement"), ("bhop"));
	SetupValue(g_Options.misc_autostrafe, false, ("movement"), ("autostrafe"));
	SetupValue(g_Options.misc_thirdperson, false, ("movement"), ("thirdperson"));
	SetupValue(g_Options.misc_thirdperson_dist, 100, ("movement"), ("third_distanse"));
	SetupValue(g_Options.misc_thirdperson_key, 8, ("movement"), ("thirdperson_key"));
	SetupValue(g_Options.misc_showranks, false, ("misc"), ("show_ranks"));
	SetupValue(g_Options.misc_clantag, false, ("misc"), ("clantag"));
	SetupValue(g_Options.misc_fake_fps, false, ("misc"), ("fakefps"));
	SetupValue(g_Options.misc_watermark, false, ("misc"), ("watermark"));
	SetupValue(g_Options.misc_spectator_list, false, ("misc"), ("spec_list"));
	SetupValue(g_Options.misc_radar, false, ("misc"), ("radar"));
	SetupValue(g_Options.misc_radar_range, 1, ("misc"), ("radar_range"));
	SetupValue(g_Options.misc_left_knife, false, ("misc"), ("left_knife"));
	SetupValue(g_Options.misc_exaggerated_ragdolls, false, ("misc"), ("ragdoll"));
	//g_Options.misc_hitsound_type
	SetupValue(g_Options.misc_hitsound_type, 0, ("misc"), ("type_sound"));
	SetupValue(g_Options.misc_exaggerated_ragdolls_force, 10, ("misc"), ("ragdoll_force"));

	SetupValue(g_Options.misc_engine_radar ,false, ("misc"), ("engine_radar"));
	SetupValue(g_Options.misc_knifebot, false, ("misc"), ("knifebot"));
	SetupValue(g_Options.misc_auto_knifebot, false, ("misc"), ("auto_knifebot"));
	SetupValue(g_Options.misc_knifebot_360, false, ("misc"), ("auto_knifebot_360"));

	SetupValue(g_Options.misc_bullet_impacts, false, ("misc"), ("bullet_impacts"));
	SetupValue(g_Options.misc_bullet_tracer, false, ("misc"), ("bullet_tracer"));
	SetupValue(g_Options.misc_hitmarker, false, ("misc"), ("hitmarker"));
	SetupValue(g_Options.misc_hitsound, false, ("misc"), ("hitsound"));
	SetupValue(g_Options.misc_hiteffect, false, ("misc"), ("hiteffect"));
	SetupValue(g_Options.misc_hitmarker_size, 13, ("misc"), ("hitmarker_size"));
	SetupValue(g_Options.misc_hiteffect_duration, 2, ("misc"), ("hiteffect_duration"));

	SetupValue(g_Options.misc_fakelag, false, ("movement"), ("fakelag"));
	SetupValue(g_Options.misc_fakelag_on_shot, false, ("movement"), ("fakelag_shot"));
	SetupValue(g_Options.misc_fakelag_safety, false, ("movement"), ("fakelag_safety"));
	SetupValue(g_Options.misc_fakelag_ticks, 1, ("misc"), ("fakelag_ticks"));
	SetupValue(g_Options.misc_fakelag_type, 1, ("misc"), ("fakelag_type"));
	SetupValue(g_Options.misc_slowwalk, false, ("movement"), ("slowwalk"));
	SetupValue(g_Options.misc_slowwalk_key, 1, ("movement"), ("slowwalk_key"));
	SetupValue(g_Options.misc_slowwalk_speed, 30, ("movement"), ("slowwalk_speed"));
	SetupValue(g_Options.exploit_doubletap, false, ("exploits"), ("doubletap"));
	SetupValue(g_Options.exploit_doubletap_key, 1, ("exploits"), ("doubletap_key"));
	SetupValue(g_Options.autopeek_key, 55, ("misc"), ("autopeek"));
}
std::map<int, const char*> weapon_names =
{
{ 0, "none" },
{ 7, "ak-47" },
{ 8, "aug" },
{ 9, "awp" },
{ 63, "cz75 auto" },
{ 1, "desert-eage" },
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
std::map<int, const char*> skin_names =
{
{ 42, "knife" },
{ 5028, "glove" },
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


void ConfigSystem::SetupLegit()
{
	for (auto&[key, val] : weapon_names) {
		SetupValue(g_Options.legitbot_items[key].enabled, false, (val), ("enabled"));
		SetupValue(g_Options.legitbot_items[key].triggerbot_enable, false, (val), ("enabled_trigger"));
		SetupValue(g_Options.legitbot_items[key].backtrack, false, (val), ("backtrack"));
		SetupValue(g_Options.legitbot_items[key].deathmatch, false, (val), ("deathmatch"));
		SetupValue(g_Options.legitbot_items[key].autopistol, false, (val), ("autopistol"));
		SetupValue(g_Options.legitbot_items[key].smoke_check, false, (val), ("smoke_check"));
		SetupValue(g_Options.legitbot_items[key].flash_check, false, (val), ("flash_check"));
		SetupValue(g_Options.legitbot_items[key].jump_check, false, (val), ("jump_check"));
		SetupValue(g_Options.legitbot_items[key].silent, false, (val), ("silent"));
		SetupValue(g_Options.legitbot_items[key].on_key, true, (val), ("on_key"));
		SetupValue(g_Options.legitbot_items[key].rcs, false, (val), ("rcs"));
		SetupValue(g_Options.legitbot_items[key].rcs_fov_enabled, false, (val), ("rcs_fov_enabled"));
		SetupValue(g_Options.legitbot_items[key].rcs_smooth_enabled, false, (val), ("rcs_smooth_enabled"));
		SetupValue(g_Options.legitbot_items[key].autostop, false, (val), ("autostop"));
		SetupValue(g_Options.legitbot_items[key].backtrack_time, 0.0f, (val), ("backtrack_time"));
		SetupValue(g_Options.legitbot_items[key].only_in_zoom, false, (val), ("only_in_zoom"));
		SetupValue(g_Options.legitbot_items[key].key, 1, (val), ("key"));
		SetupValue(g_Options.legitbot_items[key].aim_type, 1, (val), ("aim_type"));
		SetupValue(g_Options.legitbot_items[key].priority, 0, (val), ("priority"));
		SetupValue(g_Options.legitbot_items[key].fov_type, 0, (val), ("fov_type"));
		SetupValue(g_Options.legitbot_items[key].rcs_type, 0, (val), ("rcs_type"));
		SetupValue(g_Options.legitbot_items[key].smooth_type, 0, (val), ("smooth_type"));
		SetupValue(g_Options.legitbot_items[key].hitbox, 1, (val), ("hitbox"));
		SetupValue(g_Options.legitbot_items[key].fov, 0.f, (val), ("fov"));
		SetupValue(g_Options.legitbot_items[key].silent_fov, 0.f, (val), ("silent_fov"));
		SetupValue(g_Options.legitbot_items[key].rcs_fov, 0.f, (val), ("rcs_fov"));
		SetupValue(g_Options.legitbot_items[key].smooth, 1, (val), ("smooth"));
		SetupValue(g_Options.legitbot_items[key].rcs_smooth, 1, (val), ("rcs_smooth"));
		SetupValue(g_Options.legitbot_items[key].shot_delay, 0, (val), ("shot_delay"));
		SetupValue(g_Options.legitbot_items[key].kill_delay, 0, (val), ("kill_delay"));
		SetupValue(g_Options.legitbot_items[key].rcs_x, 100, (val), ("rcs_x"));
		SetupValue(g_Options.legitbot_items[key].rcs_y, 100, (val), ("rcs_y"));
		SetupValue(g_Options.legitbot_items[key].rcs_start, 1, (val), ("rcs_start"));

		SetupValue(g_Options.legitbot_items[key].triggerbot_hitchance, 100, (val), ("trigger_hitchance"));
		SetupValue(g_Options.legitbot_items[key].triggerbot_delay, 0, (val), ("trigger_delay"));

		SetupValue(g_Options.legitbot_items[key].triggerbot_head, false, (val), ("enabled_trigger_head"));
		SetupValue(g_Options.legitbot_items[key].triggerbot_body, false, (val), ("enabled_trigger_body"));
		SetupValue(g_Options.legitbot_items[key].triggerbot_other, false, (val), ("enabled_trigger_other"));

	}

}
void ConfigSystem::SetupColors()
{
	SetupColor(g_Options.color_esp_ally_visible, "color_player_esp_ally_visible");
	SetupColor(g_Options.color_esp_enemy_visible, "color_player_esp_enemy_visible");
	SetupColor(g_Options.color_esp_ally_occluded, "color_player_esp_ally_occluded");
	SetupColor(g_Options.color_esp_enemy_occluded, "color_player_esp_enemy_occluded");

	SetupColor(g_Options.color_glow_ally, "color_player_glow_ally");
	SetupColor(g_Options.color_glow_enemy, "color_player_glow_enemy");

	SetupColor(g_Options.color_chams_player_ally_visible, "color_player_chams_player_ally_visible");
	SetupColor(g_Options.color_chams_player_ally_occluded, "color_player_chams_player_ally_occluded");
	SetupColor(g_Options.color_chams_player_enemy_visible, "color_player_chams_player_enemy_visible");
	SetupColor(g_Options.color_chams_player_enemy_occluded, "color_player_chams_player_enemy_occluded");
	SetupColor(g_Options.color_chams_player_enemy_visible_l, "color_player_chams_player_enemy_overlay");
	SetupColor(g_Options.color_chams_arms_visible_layer, "colorarms_layer");
	SetupColor(g_Options.color_chams_arms_visible, "color_arms");
	SetupColor(g_Options.color_chams_weapons, "color_weapons");
	SetupColor(g_Options.color_esp_crosshair, "color_crosshair");
	SetupColor(g_Options.color_hitmarker, "color_hitmarker");
	SetupColor(g_Options.sky_color, "sky_color");
	SetupColor(g_Options.color_sound_esp, "sound_color");
	SetupColor(g_Options.color_shot_hitboxes, "shot_hc_color");
	SetupColor(g_Options.color_head_dot, "head_dot_color");
	SetupColor(g_Options.color_molotov, "molotov_timer");
	SetupColor(g_Options.color_esp_offscreen, "esp_offscreen");
}
void ConfigSystem::Setup() {

	for (int i = 1; i < 30; i++) {
		SetupValue(g_Options.loaded_lua[i], false, "lua_saves", "lua_active" + i);
	}

	ConfigSystem::SetupRage();
	ConfigSystem::SetupVisuals();
	ConfigSystem::SetupMisc();
	ConfigSystem::SetupLegit();
	ConfigSystem::SetupColors();
	ConfigSystem::SetupSkins();

	
}

void ConfigSystem::SetupSkins()
{  // not all
	for (auto&[key, val] : skin_names) {
		SetupValue(g_Options.m_skins.m_items[key].enabled, false, (val), ("skins_enabled"));
		SetupValue(g_Options.m_skins.m_items[key].stat_trak, 0, (val), ("skins_stattrack"));
		SetupValue(g_Options.m_skins.m_items[key].definition_override_index, 0, (val), ("skins_overide_index"));
		SetupValue(g_Options.m_skins.m_items[key].definition_index, 0, (val), ("skins_def_index"));
		SetupValue(g_Options.m_skins.m_items[key].definition_override_vector_index, 0, (val), ("skins_ovveride_vector_index"));
		SetupValue(g_Options.m_skins.m_items[key].definition_vector_index, 0, (val), ("skins_vector_index"));
		SetupValue(g_Options.m_skins.m_items[key].wear, 0, (val), ("skins_wear"));
		SetupValue(g_Options.m_skins.m_items[key].paint_kit_index, 0, (val), ("skins_paintkit"));
		SetupValue(g_Options.m_skins.m_items[key].paint_kit_vector_index ,0, (val), ("skins_paintkit_vector"));

	}
}
void ConfigSystem::Save(const std::string& name) {
	if (name.empty())
		return;

	CreateDirectoryA(u8"C:\\snakeware\\", NULL);
	std::string file = u8"C:\\snakeware\\" + name;

	for (auto value : ints) {
		WritePrivateProfileStringA(value->category.c_str(), value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());
	}

	for (auto value : floats) WritePrivateProfileStringA(value->category.c_str(), value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());
	for (auto value : bools) WritePrivateProfileStringA(value->category.c_str(), value->name.c_str(), *value->value ? "true" : "false", file.c_str());


}

void ConfigSystem::Load(const std::string& name) {
	if (name.empty())
		return;

	g_ClientState->ForceFullUpdate();

	CreateDirectoryA(u8"C:\\snakeware\\", NULL);
	std::string file = u8"C:\\snakeware\\" + name;

	char value_l[32] = { '\0' };
	for (auto value : ints) {
		GetPrivateProfileStringA(value->category.c_str(), value->name.c_str(), "0", value_l, 32, file.c_str()); *value->value = atoi(value_l);
	}

	for (auto value : floats) {
		GetPrivateProfileStringA(value->category.c_str(), value->name.c_str(), "0.0f", value_l, 32, file.c_str()); *value->value = atof(value_l);
	}

	for (auto value : bools) {
		GetPrivateProfileStringA(value->category.c_str(), value->name.c_str(), "false", value_l, 32, file.c_str()); *value->value = !strcmp(value_l, "true");
	}
}

ConfigSystem* Config = new ConfigSystem();
