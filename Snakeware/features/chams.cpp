#include "chams.hpp"
#include <fstream>

#include "../valve_sdk/csgostructs.hpp"
#include "../options.hpp"
#include "../hooks.hpp"
#include "../helpers/input.hpp"

Chams::Chams() {


	materialRegular = g_MatSystem->FindMaterial("debug/debugambientcube");
	materialFlat = g_MatSystem->FindMaterial("debug/debugdrawflat");
	materialSpaceGlow = g_MatSystem->FindMaterial("dev/glow_armsrace", TEXTURE_GROUP_OTHER);
	materialGlow = g_MatSystem->FindMaterial("glowOverlay", TEXTURE_GROUP_OTHER);
	materialDog = g_MatSystem->FindMaterial("vitality_metallic", TEXTURE_GROUP_OTHER);

}

Chams::~Chams() {
}
void modulate(const float* color, IMaterial* material)
{
	if (material)
	{
		material->AlphaModulate(color[3]);
		material->ColorModulate(color[0], color[1], color[2]);
		bool bFound = false;
		// https://developer.valvesoftware.com/wiki/Category:List_of_Shader_Parameterz
		auto pVar = material->FindVar("$envmaptint", &bFound);
		if (bFound)
			(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, color[0], color[1], color[2]);

	}

}
void Chams::OnDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix) {
	static auto fnDME = Hooks::mdlrender_hook.get_original<decltype(&Hooks::hkDrawModelExecute)>(index::DrawModelExecute);
	const auto mdl = info.pModel;

	if (g_MdlRender->IsForcedMaterialOverride())
		return fnDME(g_MdlRender, 0, ctx, state, info, matrix);
	auto entity = C_BasePlayer::GetPlayerByIndex(info.entity_index);

	bool is_player = strstr(mdl->szName, "models/player") != nullptr;
	bool is_hand = strstr(mdl->szName, "arms") != nullptr;
	bool is_weapon = strstr(mdl->szName, "weapons/v_") != nullptr;
	static IMaterial* hand = nullptr;
	static IMaterial* normal = nullptr;
	static IMaterial* weapon = nullptr;
	static IMaterial* hand_layer = nullptr;
	static IMaterial* weapon_layer = nullptr;
	static IMaterial* local = nullptr;
	static IMaterial* desyncmat = nullptr;
	static IMaterial* desync_layer = nullptr;
	static IMaterial* enemy_layer = nullptr;
	static IMaterial* local_layer = nullptr;

	switch (g_Options.chams_player_type)
	{
	case 0:
		normal = materialRegular;
		break;
	case 1:
		normal = materialFlat;
		break;
	case 2:
		normal = materialGlow;
		break;
	case 3:
		normal = materialSpaceGlow;
		break;
	case 4:
		normal = materialDog;
		break;
	}

	switch (g_Options.chams_player_type_layer)
	{
	case 0:
		enemy_layer = materialRegular;
		break;
	case 1:
		enemy_layer = materialFlat;
		break;
	case 2:
		enemy_layer = materialGlow;
		break;
	case 3:
		enemy_layer = materialSpaceGlow;
		break;
	case 4:
		enemy_layer = materialDog;
		break;
	}

	switch (g_Options.chams_arms_type)
	{
	case 0:
		hand = materialRegular;
		break;
	case 1:
		hand = materialFlat;
		break;
	case 2:
		hand = materialGlow;
		break;
	case 3:
		hand = materialSpaceGlow;
		break;
	case 4:
		hand = materialDog;
		break;
	}

	switch (g_Options.chams_arms_type_layer)
	{
	case 0:
		hand_layer = materialRegular;
		break;
	case 1:
		hand_layer = materialFlat;
		break;
	case 2:
		hand_layer = materialGlow;
		break;
	case 3:
		hand_layer = materialSpaceGlow;
		break;
	case 4:
		hand_layer = materialDog;
		break;
	}

	switch (g_Options.chams_weapons_type)
	{
	case 0:
		weapon = materialRegular;
		break;
	case 1:
		weapon = materialFlat;
		break;
	case 2:
		weapon = materialGlow;
		break;
	case 3:
		weapon = materialSpaceGlow;
		break;
	case 4:
		weapon = materialDog;
		break;
	}




	switch (g_Options.chams_weapons_type_layer)
	{
	case 0:
		weapon_layer = materialRegular;
		break;
	case 1:
		weapon_layer = materialFlat;
		break;
	case 2:
		weapon_layer = materialGlow;
		break;
	case 3:
		weapon_layer = materialSpaceGlow;
		break;
	case 4:
		weapon_layer = materialDog;
		break;
	}

	//bool is_weapon = strstr(mdl->szName, "weapons/v_")  != nullptr;
	if (is_player)
	{

		if (g_LocalPlayer && entity && entity->IsAlive() && !entity->IsDormant())
		{
			if (entity->IsEnemy() && g_Options.chams_player_enabled)
			{
				if (g_Options.chams_player_ignorez)
				{
					modulate(g_Options.color_chams_player_enemy_occluded, normal);
					normal->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
					g_MdlRender->ForcedMaterialOverride(normal);
					fnDME(g_MdlRender, 0, ctx, state, info, matrix);

				}

				modulate(g_Options.color_chams_player_enemy_visible, normal);
				normal->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false); // make normal material visible
				g_MdlRender->ForcedMaterialOverride(normal);
				fnDME(g_MdlRender, 0, ctx, state, info, matrix);

				if (g_Options.chams_player_enabled_l) //get ovveride method
				{

					modulate(g_Options.color_chams_player_enemy_visible_l, enemy_layer);
					enemy_layer->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
					g_MdlRender->ForcedMaterialOverride(enemy_layer);
					fnDME(g_MdlRender, 0, ctx, state, info, matrix);

					enemy_layer->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false); // make normal material visible
					modulate(g_Options.color_chams_player_enemy_visible_l, enemy_layer);
					g_MdlRender->ForcedMaterialOverride(enemy_layer);
					fnDME(g_MdlRender, 0, ctx, state, info, matrix);

				}


			}

			if (Snakeware::bShotFound) {
				// Chams
			}



		}
	}
	else if (is_hand)
	{
		if (g_LocalPlayer->IsAlive() && g_Options.chams_arms_enabled)
		{
			modulate(g_Options.color_chams_arms_visible, hand);
			g_MdlRender->ForcedMaterialOverride(hand);
			fnDME(g_MdlRender, 0, ctx, state, info, matrix);

			if (g_Options.chams_hands_enabled_l) //get ovveride method
			{
				enemy_layer->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false); // make normal material visible
				modulate(g_Options.color_chams_arms_visible_layer, hand_layer);
				g_MdlRender->ForcedMaterialOverride(hand_layer);
				fnDME(g_MdlRender, 0, ctx, state, info, matrix);
			}
		}
	}
	else if (is_weapon)
	{
		if (g_LocalPlayer->IsAlive() && g_Options.chams_weapons)
		{
			modulate(g_Options.color_chams_weapons, weapon);
			g_MdlRender->ForcedMaterialOverride(weapon);
			fnDME(g_MdlRender, 0, ctx, state, info, matrix);

			if (g_Options.chams_weapon_enabled_l) //get ovveride method
			{
				enemy_layer->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false); // make normal material visible
				modulate(g_Options.color_chams_weapons_layer, weapon_layer);
				g_MdlRender->ForcedMaterialOverride(weapon_layer);
				fnDME(g_MdlRender, 0, ctx, state, info, matrix);
			}
		}

	}

}

void Chams::OverrideMaterial(bool ignorez, int type, const Color& rgba)
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected()) return;

	IMaterial* material = nullptr;
	IMaterial* material2 = nullptr;
	switch (type)
	{
	case 0: material = materialRegular; break;
	case 1: material = materialFlat; break;
	case 2: material = materialSpaceGlow; break;
	case 3: material = materialGlow; break;
	case 4: material = materialDog; break;
	}

	bool bFound = false;
	auto pVar = material->FindVar("$envmaptint", &bFound);
	if (bFound)
		(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, rgba.r() / 255.f, rgba.g() / 255.f, rgba.b() / 255.f);

	material->IncrementReferenceCount();
	material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignorez); // crash
	material->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, ignorez); // crash
	material->ColorModulate(
		rgba.r() / 255.0f,
		rgba.g() / 255.0f,
		rgba.b() / 255.0f);
	material->AlphaModulate(rgba.a() / 255.0f);

	g_MdlRender->ForcedMaterialOverride(material);
	Hooks::mdlrender_hook.get_original<decltype(&Hooks::hkDrawModelExecute)>(index::DrawModelExecute);
	if (type == 3)
	{
		material2 = materialSpaceGlow;
		auto pVar = material2->FindVar("$envmaptint", &bFound);
		if (bFound)
			(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, 255, 255, 255);

		material2->IncrementReferenceCount();
		material2->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignorez); // crash
		material2->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, ignorez); // crash
		material2->ColorModulate(
			255,
			255,
			255);
		material2->AlphaModulate(rgba.a() / 255.0f);

		g_MdlRender->ForcedMaterialOverride(material2);
		Hooks::mdlrender_hook.get_original<decltype(&Hooks::hkDrawModelExecute)>(index::DrawModelExecute);

	}
}

