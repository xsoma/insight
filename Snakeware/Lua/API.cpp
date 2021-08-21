
#include "API.h"
#include "../hooks.hpp"
#include <intrin.h>  
#include <algorithm>

#include "../render.hpp"
#include "../menu.hpp"
#include "../options.hpp"
#include "../helpers/input.hpp"
#include "../helpers/utils.hpp"
#include "../features/bhop.hpp"
#include "../features/chams.hpp"

//LUA EXTENDS
#include "../Lua/API.h"


#include "../features/visuals.hpp"
#include "../features/glow.hpp"
#include "../conifg-system/config-system.h"
#include "../features/miscellaneous/miscellaneous.h"
#include "../engine-prediction/engine-prediction.h"
#include "../features/ragebot/animation-system/animation-system.h"
#include "../grenade-prediction/grenade-prediction.h"
#include "../features/legitbot/legitbot.h"
#include "../features/legitbot/legit-backtrack/legit-backtrack.h"
#include "../features/night-mode/night-mode.h"
#include "../features/ragebot/antihit/antihit.h"
#include "../features/player-hurt/player-hurt.h"
#include "../features/bullet-manipulation/bullet-event.h"
#include "../skin-changer/skin-changer.h"
#include "../features/ragebot/ragebot.h"
#include "../features/ragebot/netvar-compensation/netvar-comp.h"
#include "../features/event-logger/event-logger.h"
#include "../features/tickbase-shift/tickbase-exploits.h"
// material system
#include "../materials/Materials.h"
#include "../Protected/enginer.h"
#include "../Achievment_sys.h"
#include "../Lua/API.h"

namespace lua {
	bool g_unload_flag = false;
	lua_State* g_lua_state = NULL;
	c_lua_hookManager* hooks = new c_lua_hookManager();
	std::vector<bool> loaded;
	std::vector<std::string> scripts;
	std::vector<std::filesystem::path> pathes;
	element lua_m[15];
	int extract_owner(sol::this_state st) {
		sol::state_view lua_state(st);
		sol::table rs = lua_state["debug"]["getinfo"](2, "S");
		std::string source = rs["source"];
		std::string filename = std::filesystem::path(source.substr(1)).filename().string();
		return get_script_id(filename);
	}

	namespace ns_client {

		void set_event_callback(sol::this_state s, std::string eventname, sol::function func) {
			sol::state_view lua_state(s);
			sol::table rs = lua_state["debug"]["getinfo"](2, ("S"));
			std::string source = rs["source"];
			std::string filename = std::filesystem::path(source.substr(1)).filename().string();

			hooks->registerHook(eventname, get_script_id(filename), func);

		}

		void run_script(std::string scriptname) {
			load_script(get_script_id(scriptname));
		}

		void reload_active_scripts() {
			reload_all_scripts();
		}

		void refresh() {
			unload_all_scripts();
			refresh_scripts();
			load_script(get_script_id("autorun.lua"));
		}
	};

	namespace ns_config {
		/*
		config.get(key)
		Returns value of given key or nil if key not found.
		*/
		std::tuple<sol::object, sol::object, sol::object, sol::object> get(sol::this_state s, std::string key) {
			std::tuple<sol::object, sol::object, sol::object, sol::object> retn = std::make_tuple(sol::nil, sol::nil, sol::nil, sol::nil);

			for (auto kv : Config->b)
				if (kv.first == key)
					retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);

			for (auto kv : Config->c)
				if (kv.first == key)
					retn = std::make_tuple(sol::make_object(s, (int)(kv.second[0] * 255)), sol::make_object(s, (int)(kv.second[1] * 255)), sol::make_object(s, (int)(kv.second[2] * 255)), sol::make_object(s, (int)(kv.second[3] * 255)));

			for (auto kv : Config->f)
				if (kv.first == key)
					retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);


			for (auto kv : Config->i)
				if (kv.first == key)
					retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);

			for (auto kv : Config->s)
				if (kv.first == key)
					retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);

			for (auto kv : Config->i_b)
				if (kv.first == key)
					retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);

			for (auto kv : Config->i_f)
				if (kv.first == key)
					retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);

			for (auto kv : Config->i_i)
				if (kv.first == key)
					retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);

			for (auto kv : Config->i_s)
				if (kv.first == key)
					retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);

			for (auto kv : Config->s_b)
				if (kv.first == key)
					retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);

			for (auto kv : Config->s_f)
				if (kv.first == key)
					retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);

			for (auto kv : Config->s_i)
				if (kv.first == key)
					retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);

			for (auto kv : Config->s_s)
				if (kv.first == key)
					retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);

			return retn;
		}



		/*
		config.load()
		Loads selected config
		*/
		void load(std::string id) {
			Config->Load(id);
		}

		/*
		config.save()
		Saves selected config
		*/
		void save(std::string id) {
			Config->Save(id);
		}
	};

	namespace ns_cvar {
		ConVar* find_var(std::string name) {
			return g_CVar->FindVar(name.c_str());
		}

	};

	namespace ns_ui {


	};

	namespace ns_convar {
		int get_int(ConVar* c_convar) {
			return c_convar->GetInt();
		}

		float get_float(ConVar* c_convar) {
			return c_convar->GetFloat();
		}
		Color get_Color(ConVar* c_convar) {
			return c_convar->GetColor();
		}

		void set_int(ConVar* c_convar, int value) {
			c_convar->SetValue(value);
		}

		void set_float(ConVar* c_convar, float value) {
			c_convar->SetValue(value);
		}

		void set_char(ConVar* c_convar, std::string value) {
			c_convar->SetValue(value.c_str());
		}
	};

	namespace ns_engine {
		void client_cmd(std::string cmd) {
			g_EngineClient->ClientCmd(cmd.c_str());
		}
		void client_cmd_unrestricted(std::string cmd) {
			g_EngineClient->ClientCmd_Unrestricted(cmd.c_str());
		}
		void execute_client_cmd(std::string cmd) {
			g_EngineClient->ExecuteClientCmd(cmd.c_str());
		}

		player_info_t get_player_info(int ent) {
			player_info_t p;
			g_EngineClient->GetPlayerInfo(ent, &p);
			return p;
		}

		int get_player_for_user_id(int userid) {
			return g_EngineClient->GetPlayerForUserID(userid);
		}

		int get_local_player_index() {
			return g_EngineClient->GetLocalPlayer();
		}

		QAngle get_view_angles() {
			QAngle va;
			g_EngineClient->GetViewAngles(&va);
			return va;
		}

		void set_view_angles(Vector va) {
			g_EngineClient->SetViewAngles(&QAngle(va.x, va.y, va.z));
		}

		int get_max_clients() {
			return g_EngineClient->GetMaxClients();
		}

		bool sendpacket_()
		{
			return Snakeware::bSendPacket;
		}

		bool is_in_game() {
			return g_EngineClient->IsInGame();
		}

		bool is_connected() {
			return g_EngineClient->IsConnected();
		}
		void GetScreenSize(int x, int y) {
			g_EngineClient->GetScreenSize(x, y);
		}
		int GetServerTick() {
			return g_EngineClient->GetServerTick();
		}
		void TakeScreenShot(char* filename) {
			g_EngineClient->WriteScreenshot(filename);
		}
	};

	namespace nc_input
	{
		bool IsHoldingKey(int key)
		{
			return GetAsyncKeyState(key);
		}
		bool IsToogled(int key)
		{
			return GetKeyState(key);
		}
	}

	namespace binds
	{
		int manual_back()
		{
			return g_Options.antihit_manual_back;
		}
		int manual_right()
		{
			return g_Options.antihit_manual_right;
		}
		int manual_left()
		{
			return g_Options.antihit_manual_left;
		}
		int dt()
		{
			return g_Options.exploit_doubletap_key;
		}
		int fakeduck()
		{
			return g_Options.misc_fakeduck_key;
		}
		int force_body()
		{
			return g_Options.ragebot_baim_key;
		}
	};

	namespace nc_math
	{
		bool WorldToScreen(Vector in, Vector out) {
			return Math::WorldToScreen(in, out);
		}
		int ClampValue(int in, int low, int high) {
			return Math::Clamp(in, low, high);
		}
		float RandomFloat(int min, int max) {
			return Math::RandomFloat(min, max);
		}
		int RandomInt(int min, int max) {
			return Math::RandomInt(min, max);
		}
		Vector CalculateAngle(Vector src, Vector dst) {
			return Math::CalculateAngle(src, dst);
		}
		float NormalizeYaw(float yaw) {
			return Math::NormalizeYaw(yaw);
		}
		float VectorNormalize(Vector vec) {
			return Math::VectorNormalize(vec);
		}

	}

	namespace ns_entity {

		Vector get_bone_position(C_BasePlayer* entity, int bone) {
			return entity->GetBonePos(bone);
		}
		Vector get_eye_position(C_BasePlayer* entity) {
			return entity->GetEyePos();
		}
		bool is_visible_none(C_BasePlayer* entity) {
			return entity->IsVisible(true);
		}
		bool is_enemy(C_BasePlayer* entity) {
			return entity->IsEnemy();
		}
		bool is_dormant(C_BasePlayer* entity) {
			return entity->IsDormant();
		}
		bool is_weapon(C_BasePlayer* entity) {
			return entity->IsWeapon();
		}
		bool is_alive(C_BasePlayer* entity) {
			return entity->IsAlive();
		}
		bool is_player(C_BaseEntity* entity) {
			return entity->isPlayer();
		}
		C_BaseCombatWeapon* get_active_weapon(C_BasePlayer* entity) {
			return entity->m_hActiveWeapon();
		}
		CCSWeaponInfo* get_weapon_data(C_BasePlayer* entity) {
			return entity->m_hActiveWeapon()->GetCSWeaponData();
		}
		float get_inaccuracy(C_BasePlayer* entity) {
			return entity->m_hActiveWeapon()->GetInaccuracy();
		}
		Vector get_abs_origin(C_BasePlayer* entity) {
			return entity->m_angAbsOrigin();
		}
		CCSGOPlayerAnimState* get_animstate(C_BasePlayer* entity) {
			return entity->GetPlayerAnimState();
		}
		bool is_in_reload(C_BasePlayer* entity) {
			return entity->m_hActiveWeapon()->IsReloading();
		}
		QAngle get_aim_punch(C_BasePlayer* entity) {
			return entity->m_aimPunchAngle();
		}

		//NETVAR
		int get_health(C_BasePlayer* entity) {
			return entity->m_iHealth();
		}
		int get_body(C_BasePlayer* entity) {
			return entity->GetBody();
		}
		int get_hitboxSet(C_BasePlayer* entity) {
			return entity->m_nHitboxSet();
		}
		Vector get_origin(C_BasePlayer* entity) {
			return entity->m_vecOrigin();
		}
		QAngle get_aim_punch_angle(C_BasePlayer* entity) {
			return entity->m_aimPunchAngle();
		}
		bool has_C4(C_BasePlayer* entity) {
			return entity->HasC4();
		}
		int ArmorValue(C_BasePlayer* entity) {
			return entity->m_ArmorValue();
		}
		bool has_armor(C_BasePlayer* entity) {
			return entity->m_bHasHeavyArmor();
		}
		float simulation_time(C_BasePlayer* entity) {
			return entity->m_flSimulationTime();
		}
		float old_simulation_time(C_BasePlayer* entity) {
			return entity->m_flOldSimulationTime();
		}
		int ShotFire(C_BasePlayer* entity) {
			return entity->m_iShotsFired();
		}
		bool Has_helmet(C_BasePlayer* entity) {
			return entity->m_bHasHelmet();
		}
		float NextAttack(C_BasePlayer* entity) {
			return entity->m_flNextAttack();
		}
		Vector ShootPos(C_BasePlayer* entity) {
			return entity->GetShootPos();
		}
		float GetSpread(C_BasePlayer* entity) {
			return entity->m_hActiveWeapon()->GetSpread();
		}
		bool IsReloading(C_BasePlayer* entity) {
			return entity->m_hActiveWeapon()->IsReloading();
		}
		Vector GetCollideable_MAX(C_BasePlayer* entity) {
			return entity->GetCollideable()->OBBMaxs();
		}
		Vector GetCollideable_MIN(C_BasePlayer* entity) {
			return entity->GetCollideable()->OBBMins(); 
		}
		Vector RenderBoundsMin(C_BasePlayer* entity)
		{
			Vector Min;
			Vector Max;
			entity->GetRenderBounds(Min,Max);
			return Min;
		}
		Vector RenderBoundsMax(C_BasePlayer* entity)
		{
			Vector Min;
			Vector Max;
			entity->GetRenderBounds(Min, Max);
			return Max;
		}
		bool CanFire(C_BasePlayer* entity) {
			return entity->m_hActiveWeapon()->CanFire();
		}
		bool IsKnife(C_BasePlayer* entity) {
			return entity->m_hActiveWeapon()->IsKnife();
		}
		bool CanShiftTickbase(C_BasePlayer* entity) {
			return entity->m_hActiveWeapon()->CanShiftTickbase();
		}
		int GetZoomLevel(C_BasePlayer* entity) {
			return entity->m_hActiveWeapon()->m_zoomLevel();
		}
		bool IsScoped(C_BasePlayer* entity) {
			return entity->m_bIsScoped();
		}
		std::string PlayerName(C_BasePlayer* entity) {
			return entity->GetName();
		}
		bool HasDefuseKits(C_BasePlayer* entity) {
			return entity->IsDefuseKit();
		}
		int EntityIndex(C_BasePlayer* entity) {
			return entity->GetIndex();
		}
		Vector GetVelocity(C_BasePlayer* entity) {
			return entity->m_vecVelocity();
		}
		int GetVelocitySpeed(C_BasePlayer* entity) {
			return entity->m_vecVelocity().Length2D();
		}
		float DuckAmount(C_BasePlayer* entity) {
			return entity->m_flDuckAmount();
		}
		float DuckSpeed(C_BasePlayer* entity) {
			return entity->m_flDuckSpeed();
		}
		float FlashDuration(C_BasePlayer* entity) {
			return entity->m_flFlashDuration();
		}
		float Tickbase(C_BasePlayer* entity) {
			return entity->m_nTickBase();
		}
		float anim_GoalFeetYaw(C_BasePlayer* entity) {
			return entity->GetPlayerAnimState()->m_flGoalFeetYaw;
		}
		float anim_m_flFeetYawRate(C_BasePlayer* entity) {
			return entity->GetPlayerAnimState()->m_flFeetYawRate;
		}
		float anim_m_flCycle(C_BasePlayer* entity) {
			return entity->GetAnimOverlays()->m_flCycle;
		}
		float anim_m_flPlaybackRate(C_BasePlayer* entity) {
			return entity->GetAnimOverlays()->m_flPlaybackRate;
		}
		float anim_m_flWeight(C_BasePlayer* entity) {
			return entity->GetAnimOverlays()->m_flWeight;
		}
		float GetSequenceActivity(C_BasePlayer* entity, int layer) {
			return entity->GetSequenceActivity(layer);
		}
		Vector EyeAngles(C_BasePlayer* entity) {
			return Vector(entity->m_angEyeAngles().pitch, entity->m_angEyeAngles().yaw, entity->m_angEyeAngles().roll);
		}
		C_BasePlayer* GetPlayer()
		{
			return Snakeware::GetPlayer;
		}
		int HeadPosX() {
			return Snakeware::HeadPos.x;
		}
		int HeadPosY() {
			return Snakeware::HeadPos.y;
		}
		



		//void set_pnetvar_int(Entity* entity, std::string class_name, std::string var_name, int offset, int var){}
		//void set_pnetvar_bool(Entity* entity, std::string class_name, std::string var_name, int offset, bool var){}
		//void set_pnetvar_float(Entity* entity, std::string class_name, std::string var_name, int offset, float var){}
		//void set_pnetvar_vector(Entity* entity, std::string class_name, std::string var_name, int offset, Vector var){}
	};

	enum font_flags {
		fontflag_none,
		fontflag_italic = 0x001,
		fontflag_underline = 0x002,
		fontflag_strikeout = 0x004,
		fontflag_symbol = 0x008,
		fontflag_antialias = 0x010,
		fontflag_gaussianblur = 0x020,
		fontflag_rotary = 0x040,
		fontflag_dropshadow = 0x080,
		fontflag_additive = 0x100,
		fontflag_outline = 0x200,
		fontflag_custom = 0x400,
		fontflag_bitmap = 0x800,
	};

	namespace ns_surface {

		void set_color(int r, int g, int b, int a = 255) {
			g_VGuiSurface->DrawSetColor(r, g, b, a);
		}
		void draw_line(float x0, float y0, float x1, float y1) {
			g_VGuiSurface->DrawLine(x0, y0, x1, y1);
		}
		void draw_filled_rect(float x0, float y0, float x1, float y1) {
			g_VGuiSurface->DrawFilledRect(x0, y0, x1, y1);
		}
		void draw_filled_rect_fade(int x, int y, int x2, int y2, int alpha, int alpha2, bool horizontal) {
			//g_VGuiSurface->DrawFilledRect(x, y, x2, y2, alpha, alpha2, horizontal);
		}
		void draw_outlined_rect(float x0, float y0, float x1, float y1) {
			g_VGuiSurface->DrawOutlinedRect(x0, y0, x1, y1);
		}
		void draw_circle(float x, float y, int startRadius, int radius) {
			g_VGuiSurface->DrawOutlinedCircle(x, y, startRadius, radius);
		}
		int get_screen_x() {
			int w, h;
			g_EngineClient->GetScreenSize(w, h);
			return w;
		}
		int get_screen_y() {
			int w, h;
			g_EngineClient->GetScreenSize(w, h);
			return h;
		}
		int font_create(std::string fontname, int w, int h, int blur, int flags) {

			switch (flags)
			{
			case 0:
			{
				auto f = g_VGuiSurface->CreateFont_();
				g_VGuiSurface->SetFontGlyphSet(f, fontname.c_str(), w, h, blur, 0, 0);
				if (f != NULL)
					return  f;
			}
			case 1:
			{
				auto f = g_VGuiSurface->CreateFont_();
				g_VGuiSurface->SetFontGlyphSet(f, fontname.c_str(), w, h, blur, 0, fontflag_antialias);
				if (f != NULL)
					return  f;
			}
			case 3:
			{
				auto f = g_VGuiSurface->CreateFont_();
				g_VGuiSurface->SetFontGlyphSet(f, fontname.c_str(), w, h, blur, 0, fontflag_dropshadow);
				if (f != NULL)
					return  f;
			}
			case 4:
			{
				auto f = g_VGuiSurface->CreateFont_();
				g_VGuiSurface->SetFontGlyphSet(f, fontname.c_str(), w, h, blur, 0, fontflag_additive);
				if (f != NULL)
					return  f;
			}
			case 5:
			{
				auto f = g_VGuiSurface->CreateFont_();
				g_VGuiSurface->SetFontGlyphSet(f, fontname.c_str(), w, h, blur, 0, fontflag_outline);
				if (f != NULL)
					return  f;
			}
			}

		}
		void set_text_font(int font) {
			g_VGuiSurface->DrawSetTextFont(font);
		}
		void set_text_color(int r, int g, int b, int a) {
			g_VGuiSurface->DrawSetTextColor(r, g, b, a);
		}
		void draw_text(std::wstring str, int x, int y) {
			g_VGuiSurface->DrawSetTextPos(x, y);
			g_VGuiSurface->DrawPrintText(str.c_str(), str.length());
		}
		Vector2D get_cursor_position() {
			POINT mousePosition;
			GetCursorPos(&mousePosition);
			ScreenToClient(FindWindowA(0, "Counter-Strike: Global Offensive"), &mousePosition);
			return { static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y) };
		}
		bool is_button_pressed(int buttonCode) {
			return GetAsyncKeyState(buttonCode);
		}
		/*void play_sound(std::string fileName) {
			g_interfaces.surface->playSound(fileName.c_str());
		}*/
	};

	namespace _EntityList
	{
		int GetHighestEntityIndex()
		{
			return g_EntityList->GetHighestEntityIndex();
		}
		C_BasePlayer* GetEntityByIndex(int index)
		{
			return static_cast<C_BasePlayer*> (g_EntityList->GetClientEntity(index));
		}
	}

	namespace ns_globalvars
	{
		float curtime() {
			return g_GlobalVars->curtime;
		}
		float framecount() {
			return g_GlobalVars->framecount;
		}
		float interval_per_tick() {
			return g_GlobalVars->interval_per_tick;
		}
		float tickcount() {
			return g_GlobalVars->tickcount;
		}
		float realtime() {
			return g_GlobalVars->realtime;
		}

	}

	void test_func() {
		for (auto hk : hooks->getHooks("on_test"))
		{
			try
			{
				auto result = hk.func();
				if (!result.valid()) {
					sol::error err = result;
				}
			}
			catch (const std::exception&)
			{

			}
		}
	}

	bool getdesync()
	{
		return AntiHit::Get().switchSide;
	}

	int desyncvalue()
	{
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
		return desync;
	}

	namespace ns_ui {
		std::string new_checkbox(std::string label, bool* var, int id) {
			element item;
			item.type = MENUITEM_CHECKBOX;
			item.label = label;
			item.var_bool = var;
			lua_m[id] = item;
			return label;
		}


	};
	namespace DME {
		IMaterial* FindMaterial(std::string name) {
			return g_MatSystem->FindMaterial(name.c_str(), TEXTURE_GROUP_MODEL);
		}
		IMaterial* GetMaterial(MaterialHandle_t id) {
			return g_MatSystem->GetMaterial(id);
		}
		MaterialHandle_t FirstMaterial() {
			return g_MatSystem->FirstMaterial();
		}
		MaterialHandle_t NextMaterial(int i) {
			return g_MatSystem->NextMaterial(i);
		}
		MaterialHandle_t InvalidMaterial() {
			return g_MatSystem->InvalidMaterial();
		}
		bool IsErrorMat(IMaterial* id) {
			return id->IsErrorMaterial();
		}
		std::string GetName(IMaterial* id) {
			return id->GetName();
		}
		std::string GetTextureName(IMaterial* id) {
			return id->GetTextureGroupName();
		}
		void Modulate(IMaterial* id, int r, int g, int b) {
			id->ColorModulate(r, g, b);
		}
	}
	namespace cmd
	{
		bool Onshot()
		{
			return Snakeware::cmd->buttons & IN_ATTACK;
		}
		bool Onshot2()
		{
			return Snakeware::cmd->buttons & IN_ATTACK2;
		}
		bool OnDuck()
		{
			return Snakeware::cmd->buttons & IN_DUCK;
		}
		bool OnGrenade()
		{
			return (Snakeware::cmd->buttons & IN_GRENADE1) || (Snakeware::cmd->buttons & IN_GRENADE2);
		}
	}
	Vector Vector_( int x , int y, int z)
	{
		return Vector(x, y, z);
	}
	C_BasePlayer* localplayer()
	{
		return g_LocalPlayer;
	}
	void init_command() {
		sol::state_view lua_state(g_lua_state);
		lua_state["test_func"] = []() { test_func(); };
		lua_state["exit"] = []() { g_unload_flag = true; };

		// Mouse - Keyboard keys
		/*	lua_state.new_enum("num_keys",
				//VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
				"VK_0", VK_0,
				"VK_1", VK_1,
				"VK_2", VK_2,
				"VK_3", VK_3,
				"VK_4", VK_4,
				"VK_5", VK_5,
				"VK_6", VK_6,
				"VK_7", VK_7,
				"VK_8", VK_8,
				"VK_9", VK_9
			);
			lua_state.new_enum("alp_keys",
				//VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
				"VK_A", VK_A,
				"VK_B", VK_B,
				"VK_C", VK_C,
				"VK_D", VK_D,
				"VK_E", VK_E,
				"VK_F", VK_F,
				"VK_G", VK_G,
				"VK_H", VK_H,
				"VK_I", VK_I,
				"VK_J", VK_J,
				"VK_K", VK_K,
				"VK_L", VK_L,
				"VK_M", VK_M,
				"VK_N", VK_N,
				"VK_O", VK_O,
				"VK_P", VK_P,
				"VK_Q", VK_Q,
				"VK_R", VK_R,
				"VK_S", VK_S,
				"VK_T", VK_T,
				"VK_U", VK_U,
				"VK_V", VK_V,
				"VK_W", VK_W,
				"VK_X", VK_X,
				"VK_Y", VK_Y,
				"VK_Z", VK_Z
			);
			lua_state.new_enum("other_keys",
				//Other
				"VK_ESCAPE", VK_ESCAPE,
				"VK_RETURN", VK_RETURN,
				"VK_TAB", VK_TAB,
				"VK_CAPITAL", VK_CAPITAL,
				"VK_MENU", VK_MENU,
				"VK_LWIN", VK_LWIN,
				"VK_RWIN", VK_RWIN,
				"VK_MENU", VK_MENU,
				"VK_SHIFT", VK_SHIFT,
				"VK_LSHIFT", VK_LSHIFT,
				"VK_RSHIFT", VK_RSHIFT,
				"VK_SPACE", VK_SPACE,
				"VK_BACK", VK_BACK,
				"VK_UP", VK_UP,
				"VK_DOWN", VK_DOWN,
				"VK_LEFT", VK_LEFT,
				"VK_RIGHT", VK_RIGHT,
				"VK_END", VK_END,
				"VK_INSERT", VK_INSERT,
				"VK_HOME", VK_HOME,
				"VK_PRIOR", VK_PRIOR,
				"VK_NEXT", VK_NEXT,
				"VK_DELETE", VK_DELETE,
				"VK_LBUTTON", VK_LBUTTON,
				"VK_RBUTTON", VK_RBUTTON,
				"VK_CANCEL", VK_CANCEL,
				"VK_MBUTTON", VK_MBUTTON,
				"VK_DELETE", VK_DELETE
			);

			lua_state.new_enum("f_keys",
				"VK_F1", VK_F1,
				"VK_F2", VK_F2,
				"VK_F3", VK_F3,
				"VK_F4", VK_F4,
				"VK_F5", VK_F5,
				"VK_F6", VK_F6,
				"VK_F7", VK_F7,
				"VK_F8", VK_F8,
				"VK_F9", VK_F9,
				"VK_F10", VK_F10,
				"VK_F11", VK_F11,
				"VK_F12", VK_F12
		);


			lua_state.new_enum("control_keys",
				"VK_NUMLOCK", VK_NUMLOCK,
				"VK_NUMPAD0", VK_NUMPAD0,
				"VK_NUMPAD1", VK_NUMPAD1,
				"VK_NUMPAD2", VK_NUMPAD2,
				"VK_NUMPAD3", VK_NUMPAD3,
				"VK_NUMPAD4", VK_NUMPAD4,
				"VK_NUMPAD5", VK_NUMPAD5,
				"VK_NUMPAD6", VK_NUMPAD6,
				"VK_NUMPAD7", VK_NUMPAD7,
				"VK_NUMPAD8", VK_NUMPAD8,
				"VK_NUMPAD9", VK_NUMPAD9,
				"VK_DECIMAL", VK_DECIMAL,
				"VK_MULTIPLY", VK_MULTIPLY,
				"VK_ADD", VK_ADD,
				"VK_SUBTRACT", VK_SUBTRACT,
				"VK_DIVIDE", VK_DIVIDE,
				"VK_PAUSE", VK_PAUSE,
				"VK_SCROLL", VK_SCROLL
			);

		lua_state.new_enum("CMDBUTTONS",
			"IN_ATTACK", IN_ATTACK,
			"IN_JUMP",IN_JUMP,
			"IN_DUCK",IN_DUCK,
			"IN_FORWARD",IN_FORWARD,
			"IN_BACK", IN_BACK,
			"IN_USE",IN_USE,
			"IN_MOVELEFT",IN_MOVELEFT,
			"IN_MOVERIGHT",IN_MOVERIGHT,
			"IN_ATTACK2",IN_ATTACK2,
			"IN_SCORE",IN_SCORE,
			"IN_BULLRUSH",IN_BULLRUSH);
		*/

		lua_state.new_usertype<Vector>("vector",
			"x", &Vector::x,
			"y", &Vector::y,
			"z", &Vector::z
			);

		lua_state.new_usertype<Color>("Color",
			sol::constructors<Color(), Color(int, int, int), Color(int, int, int, int)>(),
			"r", &Color::r,
			"g", &Color::g,
			"b", &Color::b,
			"a", &Color::a
			);

		auto cheat = lua_state.create_table();
		cheat["vector"] = Vector_;
		lua_state["cheat"] = cheat;

		auto cmd = lua_state.create_table();
		cmd["OnShot"] = cmd::Onshot;
		cmd["OnShot2"] = cmd::Onshot2;
		cmd["OnDuck"] = cmd::OnDuck;
		cmd["OnGrenade"] = cmd::OnGrenade;
		lua_state["cmd"] = cmd;

		auto DME = lua_state.create_table();
		DME["FindMaterial"] = DME::FindMaterial;
		DME["FirstMaterial"] = DME::FirstMaterial;
		DME["GetMaterial"] = DME::GetMaterial;
		DME["GetName"] = DME::GetName;
		DME["GetTextureName"] = DME::GetTextureName;
		DME["InvalidMaterial"] = DME::InvalidMaterial;
		DME["IsErrorMat"] = DME::IsErrorMat;
		DME["Modulate"] = DME::Modulate;
		DME["NextMaterial"] = DME::NextMaterial;
		lua_state["DME"] = cmd;

		auto clientstate = lua_state.create_table();
		lua_state["clientstate"] = clientstate;

		auto config = lua_state.create_table();
		config["get"] = ns_config::get;
		config["save"] = ns_config::save;
		config["load"] = ns_config::load;
		lua_state["config"] = config;

		auto binds2 = lua_state.create_table();
		binds2["manual_back"] = binds::manual_back;
		binds2["manual_right"] = binds::manual_right;
		binds2["manual_left"] = binds::manual_left;
		binds2["dt"] = binds::dt;
		binds2["force_body"] = binds::force_body;
		binds2["fakeduck"] = binds::fakeduck;
		lua_state["binds"] = binds2;

		auto render = lua_state.create_table();
		render["DrawLine"] = ns_surface::draw_line;
		render["SetBrushColor"] = ns_surface::set_color;
		render["DrawFilledRect"] = ns_surface::draw_filled_rect;
		render["DrawRect"] = ns_surface::draw_outlined_rect;
		render["DrawCircle"] = ns_surface::draw_circle;
		render["GetScreenX"] = ns_surface::get_screen_x;
		render["GetScreenY"] = ns_surface::get_screen_y;
		render["NewFont"] = ns_surface::font_create;
		render["PushFont"] = ns_surface::set_text_font;
		render["SetTextBrushColor"] = ns_surface::set_text_color;
		render["DrawText"] = ns_surface::draw_text;
		render["GetCursorPos"] = ns_surface::get_cursor_position;
		lua_state["render"] = render;
	
		auto entity_list = lua_state.create_table();
		entity_list["GetHighestEntityIndex"] = _EntityList::GetHighestEntityIndex;
		entity_list["GetEntityByIndex"] = _EntityList::GetEntityByIndex;
		lua_state["entity_list"] = entity_list;

		auto globalvars = lua_state.create_table();
		globalvars["curtime"] = ns_globalvars::curtime;
		globalvars["framecount"] = ns_globalvars::framecount;
		globalvars["interval_per_tick"] = ns_globalvars::interval_per_tick;
		globalvars["realtime"] = ns_globalvars::realtime;
		globalvars["tickcount"] = ns_globalvars::tickcount;
		lua_state["globalvars"] = globalvars;

		auto client = lua_state.create_table();
		client["RegisterFunction"] = ns_client::set_event_callback;
		client["RunScript"] = ns_client::run_script;
		client["ReloadScripts"] = ns_client::reload_active_scripts;
		client["Refresh"] = ns_client::refresh;
		client["DesyncSwitched"] = getdesync;
		client["DesyncValue"] = desyncvalue;
		lua_state["client"] = client;

		auto engine = lua_state.create_table();
		engine["ClientCmd"] = ns_engine::client_cmd;
		engine["ClientCmdUnrestricted"] = ns_engine::client_cmd_unrestricted;
		engine["ExecuteClientCmd"] = ns_engine::execute_client_cmd;
		engine["GetLocalPlayerIndex"] = ns_engine::get_local_player_index;
		engine["LocalPlayer"] = localplayer;
		engine["Maxclients"] = ns_engine::get_max_clients;
		engine["SendPacket"] = Snakeware::bSendPacket;
		engine["GetPlayerById"] = ns_engine::get_player_for_user_id;
		engine["GetPlayerInfo"] = ns_engine::get_player_info;
		engine["GetViewAngles"] = ns_engine::get_view_angles;
		engine["IsConnected"] = ns_engine::is_connected;
		engine["IsInGame"] = ns_engine::is_in_game;
		engine["SetViewAngles"] = ns_engine::set_view_angles;
		engine["GetScreenSize"] = ns_engine::GetScreenSize;
		engine["GetServerTick"] = ns_engine::GetServerTick;
		engine["TakeScreenShot"] = ns_engine::TakeScreenShot;
		lua_state["engine"] = engine;

		auto input = lua_state.create_table();
		input["IsHoldingKey"] = nc_input::IsHoldingKey;
		input["IsToogled"] = nc_input::IsToogled;
		lua_state["input"] = input;

		auto math = lua_state.create_table();
		math["CalculateAngle"] = nc_math::CalculateAngle;
		math["ClampValue"] = nc_math::ClampValue;
		math["NormalizeYaw"] = nc_math::NormalizeYaw;
		math["RandomFloat"] = nc_math::RandomFloat;
		math["RandomInt"] = nc_math::RandomInt;
		math["VectorNormalize"] = nc_math::VectorNormalize;
		math["WorldToScreen"] = nc_math::WorldToScreen;
		math["FixAngles"] = Math::FixAngles;
		math["Vector_Angles"] = Math::Vector_Angles;
		math["VectorRotate"] = Math::VectorRotate;
		lua_state["math"] = math;

		auto entity = lua_state.create_table();
		entity["BonePos"] = ns_entity::get_bone_position;
		entity["EyePos"] = ns_entity::get_eye_position;
		entity["IsVisible"] = ns_entity::is_visible_none;
		entity["IsEnemy"] = ns_entity::is_enemy;
		entity["IsDormant"] = ns_entity::is_dormant;
		entity["IsWeapon"] = ns_entity::is_weapon;
		entity["IsAlive"] = ns_entity::is_alive;
		entity["IsPlayer"] = ns_entity::is_player;
		entity["ActiveWeapon"] = ns_entity::get_active_weapon;
		entity["GetInaccuracy"] = ns_entity::get_inaccuracy;
		entity["GetSpread"] = ns_entity::GetSpread;
		entity["GetAbsOrigin"] = ns_entity::get_abs_origin;
		entity["GetAnimstate"] = ns_entity::get_animstate;
		entity["IsReloading"] = ns_entity::is_in_reload;
		entity["AimPunch"] = ns_entity::get_aim_punch;
		entity["GetHealth"] = ns_entity::get_health;
		entity["GetBody"] = ns_entity::get_body;
		entity["GetHitboxSet"] = ns_entity::get_hitboxSet;
		entity["GetOrigin"] = ns_entity::get_origin;
		entity["GetAimPunchAngle"] = ns_entity::get_aim_punch_angle;
		entity["GetArmorValue"] = ns_entity::ArmorValue;
		entity["CanFire"] = ns_entity::CanFire;
		entity["CanShiftTickBase"] = ns_entity::CanShiftTickbase;
		entity["DuckAmount"] = ns_entity::DuckAmount;
		entity["DuckSpeed"] = ns_entity::DuckSpeed;
		entity["GetEntityIndex"] = ns_entity::EntityIndex;
		entity["GetFlashDuration"] = ns_entity::FlashDuration;
		entity["GetCollideable_MAX"] = ns_entity::GetCollideable_MAX;
		entity["GetCollideable_MIN"] = ns_entity::GetCollideable_MIN;
		entity["RenderBoundsMax"] = ns_entity::RenderBoundsMax;
		entity["RenderBoundsMin"] = ns_entity::RenderBoundsMin;
		entity["GetVelocity"] = ns_entity::GetVelocity;
		entity["GetVelocitySpeed"] = ns_entity::GetVelocitySpeed;
		entity["ZoomLevel"] = ns_entity::GetZoomLevel;
		entity["HasDefuseKits"] = ns_entity::HasDefuseKits;
		entity["HasC4"] = ns_entity::has_C4;
		entity["IsScoped"] = ns_entity::IsScoped;
		entity["GetNextAttack"] = ns_entity::NextAttack;
		entity["SimulationTime"] = ns_entity::simulation_time;
		entity["OldSimulationTime"] = ns_entity::old_simulation_time;
		entity["PlayerName"] = ns_entity::PlayerName;
		entity["GetShootPos"] = ns_entity::ShootPos;
		entity["ShotFired"] = ns_entity::ShotFire;
		entity["GetTickBase"] = ns_entity::Tickbase;
		entity["GoalFeetYaw"] = ns_entity::anim_GoalFeetYaw;
		entity["EyeAngles"] = ns_entity::EyeAngles;
		entity["m_flCycle"] = ns_entity::anim_m_flCycle;
		entity["m_flFeetYawRate"] = ns_entity::anim_m_flFeetYawRate;
		entity["m_flPlaybackRate"] = ns_entity::anim_m_flPlaybackRate;
		entity["m_flWeight"] = ns_entity::anim_m_flWeight;
		entity["GetSequenceActivity"] = ns_entity::GetSequenceActivity;
		entity["GetPlayer"] = ns_entity::GetPlayer;
		entity["HeadPosX"] = ns_entity::HeadPosX;
		entity["HeadPosY"] = ns_entity::HeadPosY;
		lua_state["entity"] = entity;

		auto cvar = lua_state.create_table();
		cvar["FindVar"] = ns_cvar::find_var;
		lua_state["cvar"] = cvar;

		auto convar = lua_state.create_table();
		convar["GetInt"] = ns_convar::get_int;
		convar["SetFloat"] = ns_convar::set_int;
		convar["GetFloat"] = ns_convar::get_float;
		convar["SetFloat"] = ns_convar::set_float;
		convar["SetChar"] = ns_convar::set_char;
		lua_state["convar"] = convar;

		auto ui = lua_state.create_table();
		ui["new_checkbox"] = ns_ui::new_checkbox;

		lua_state["ui"] = ui;

		refresh_scripts();
		//load_script(get_script_id("autorun.lua"));

	}

	void init_state() {
#ifdef _DEBUG
		lua_writestring(LUA_COPYRIGHT, strlen(LUA_COPYRIGHT));
		lua_writeline();
#endif
		lua::unload();
		g_lua_state = luaL_newstate();
		luaL_openlibs(g_lua_state);
	}

	void unload() {
		if (g_lua_state != NULL) {
			lua_close(g_lua_state);
			g_lua_state = NULL;
		}
	}

	const char* string_tochar(std::string first, std::string second) {
		std::stringstream ss;
		ss << first << second;
		const std::string tmp = std::string{ ss.str() };
		const char* str = tmp.c_str();
		return str;
	}	

	void load_script(int id) {
		if (id == -1)
			return;

		if (loaded.at(id))
			return;

		auto path = get_script_path(id);
		if (path == (""))
			return;

		sol::state_view state(g_lua_state);
		state.script_file(path, [](lua_State* me, sol::protected_function_result result) {
			if (!result.valid()) {
				sol::error err = result;
				std::stringstream ss;
				g_EngineClient->ExecuteClientCmd(string_tochar("echo [Insight LUA] - SCRIPT LOADED! ERRORS : + , WARNING : 0, Error text : ", err.what()));
			}
			else
			{
				g_EngineClient->ExecuteClientCmd("echo [Insight LUA] - SCRIPT LOADED! ERRORS : -, WARNINGS : 0");
			}
			return result;
			});
		loaded.at(id) = true;
	}

	void unload_script(int id) {
		if (id == -1)
			return;

		if (!loaded.at(id))
			return;

		//std::map<std::string, std::map<std::string, std::vector<MenuItem_t>>> updated_items;

		hooks->unregisterHooks(id);
		loaded.at(id) = false;
	}

	void reload_all_scripts() {
		for (auto s : scripts) {
			if (loaded.at(get_script_id(s))) {
				unload_script(get_script_id(s));
				load_script(get_script_id(s));
			}
		}
	}

	void unload_all_scripts() {
		for (auto s : scripts)
			if (loaded.at(get_script_id(s)))
				unload_script(get_script_id(s));
	}

	void refresh_scripts() {
		std::filesystem::path path;
		std::string path_cfg("C:\\snakeware\\lua");
		path = path_cfg;

		if (!std::filesystem::is_directory(path)) {
			std::filesystem::remove(path);
			std::filesystem::create_directory(path);
		}

		auto oldLoaded = loaded;
		auto oldScripts = scripts;

		loaded.clear();
		pathes.clear();
		scripts.clear();

		for (auto& entry : std::filesystem::directory_iterator((path))) {
			if (entry.path().extension() == (".lua")) {
				auto path = entry.path();
				auto filename = path.filename().string();

				bool didPut = false;
				int oldScriptsSize = 0;
				oldScriptsSize = oldScripts.size();
				if (oldScriptsSize < 0)
					continue;

				for (int i = 0; i < oldScriptsSize; i++) {
					if (filename == oldScripts.at(i)) {
						loaded.push_back(oldLoaded.at(i));
						didPut = true;
					}
				}

				if (!didPut)
					loaded.push_back(false);

				pathes.push_back(path);
				scripts.push_back(filename);
			}
		}
	}

	int get_script_id(std::string name) {
		int scriptsSize = 0;
		scriptsSize = scripts.size();
		if (scriptsSize <= 0)
			return -1;

		for (int i = 0; i < scriptsSize; i++) {
			if (scripts.at(i) == name)
				return i;
		}

		return -1;
	}

	int get_script_id_by_path(std::string path) {
		int pathesSize = 0;
		pathesSize = pathes.size();
		if (pathesSize <= 0)
			return -1;

		for (int i = 0; i < pathesSize; i++) {
			if (pathes.at(i).string() == path)
				return i;
		}

		return -1;
	}

	std::string get_script_path(std::string name) {
		return get_script_path(get_script_id(name));
	}

	std::string get_script_path(int id) {
		if (id == -1)
			return  "";

		return pathes.at(id).string();
	}

	void c_lua_hookManager::registerHook(std::string eventName, int scriptId, sol::protected_function func) {
		c_lua_hook hk = { scriptId, func };

		this->hooks[eventName].push_back(hk);
	}

	void c_lua_hookManager::unregisterHooks(int scriptId) {
		for (auto& ev : this->hooks) {
			int pos = 0;

			for (auto& hk : ev.second) {
				if (hk.scriptId == scriptId)
					ev.second.erase(ev.second.begin() + pos);

				pos++;
			}
		}
	}

	std::vector<c_lua_hook> c_lua_hookManager::getHooks(std::string eventName) {
		return this->hooks[eventName];
	}
};

