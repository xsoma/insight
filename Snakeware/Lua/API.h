#pragma once
#include "sol.hpp"
#include "../../Lua-5.3.5/src/lua.hpp"
#include "../../Lua-5.3.5/src/lua.h"
#include <map>
#include <filesystem>
#include "../../Lua-5.3.5/src/llex.h"
namespace lua {
	struct c_lua_hook {
		int scriptId;
		sol::protected_function func;
	};

	class c_lua_hookManager {
	public:
		void registerHook(std::string eventName, int scriptId, sol::protected_function func);
		void unregisterHooks(int scriptId);

		std::vector<c_lua_hook> getHooks(std::string eventName);

	private:
		std::map<std::string, std::vector<c_lua_hook>> hooks;
	};

	enum MENUITEMTYPE {
		MENUITEM_CHECKBOX = 5,
		MENUITEM_SLIDERINT,
		MENUITEM_SLIDERFLOAT,
		MENUITEM_KEYBIND,
		MENUITEM_TEXT,
		MENUITEM_SINGLESELECT,
		MENUITEM_MULTISELECT,
		MENUITEM_COLORPICKER,
		MENUITEM_BUTTON
	};
	struct element {
		MENUITEMTYPE type;
		std::string label = "";
		bool* var_bool;
		int var_int;
		float var_float;
		// keybinds
		bool allow_style_change = true;
		// singleselect & multiselect
		std::vector<const char*> items = {};
		// slider_int
		int i_min = 0;
		int i_max = 100;
		// slider_float
		float f_min = 0.f;
		float f_max = 1.f;
		// sliders
		std::string format = "%d";
	};
	void init_state();
	void init_command();
	void init_console();
	void unload();
	void load_script(int id);
	void unload_script(int id);
	void reload_all_scripts();
	void unload_all_scripts();
	void refresh_scripts();
	int get_script_id(std::string name);
	int get_script_id_by_path(std::string path);
	std::string get_script_path(std::string name);
	std::string get_script_path(int id);

	//extern std::map<std::string, std::map<std::string, std::vector<MenuItem_t>>> menu_items;
	extern std::vector<std::filesystem::path> pathes;
	extern std::vector<bool> loaded;
	extern std::vector<std::string> scripts;
	extern c_lua_hookManager* hooks;
	extern lua_State* g_lua_state;
	extern bool g_unload_flag;
	extern element lua_m[15];;
}
