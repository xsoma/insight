
#include <algorithm>

#include "../visuals.hpp"
#include "render_helper.h"
#include "../../options.hpp"
#include "../../features/ragebot/autowall/ragebot-autowall.h"
#include "../../helpers/math.hpp"
#include "../../helpers/utils.hpp"
#include "../../helpers/input.hpp"
#include "../../menu.hpp"
#include "../../singleton.hpp"
#include "../../render.hpp"
#include "../../helpers/math.hpp"
#include "../../valve_sdk/csgostructs.hpp"



void RENDER_3D_GUI::LocalCircle()
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive()) return;

		auto weapon = g_LocalPlayer->m_hActiveWeapon();

		if (!weapon) return;

		Vector pos = g_LocalPlayer->m_angAbsOrigin();
		auto idx = weapon->m_Item().m_iItemDefinitionIndex();

		auto weapon_data = weapon->GetCSWeaponData();

		int radius = 40;
		int radius2 = 100;

		if (weapon_data->iWeaponType == WEAPONTYPE_KNIFE)
		{
			
			Render::Get().RenderCircle3D(pos, radius * 3, radius, Color(255, 255, 255));
		}
		else if (idx == WEAPON_ZEUS)
		{
			
			Render::Get().RenderCircle3D(pos, radius2 * 3, radius2, Color(255, 255, 255));
		}
	



}

void RENDER_3D_GUI::DrawMolotov () {

	if (!g_Options.esp_molotov_timer)                                  return;
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected()) return;

	for (auto i = 0; i < g_EntityList->GetHighestEntityIndex(); i++)
	{
		auto ent = C_BaseEntity::GetEntityByIndex(i);
		if (!ent)
			continue;

		if (ent->GetClientClass()->m_ClassID != ClassId_CInferno)
			continue;

		auto inferno = reinterpret_cast<C_Inferno*>(ent);

		auto origin = inferno->m_vecOrigin();
		auto screen_origin = Vector();

		if (!Math::WorldToScreen(origin, screen_origin))
			return;

		const auto spawn_time = inferno->GetSpawnTime();
		const auto timer = (spawn_time + C_Inferno::GetExpireTime()) - g_GlobalVars->curtime;
		const auto factor = timer / C_Inferno::GetExpireTime();
		const auto l_spawn_time = *(float*)(uintptr_t(inferno) + 0x20);
		const auto l_factor = ((l_spawn_time + 7.03125f) - g_GlobalVars->curtime) / 7.03125f;

		Render::Get().RenderCircle3D(origin, 50, 150.f, Color(255, 0, 0));

		auto sz = g_pESP->CalcTextSizeA(14.f, 400, 0.0f, "Molotov");

		Render::Get().RenderTextPixel("Molotov", screen_origin.x - 5, screen_origin.y - 15.f, 13.f, Color(255, 255, 255));


		Render::Get().RenderCircleFilled(screen_origin.x, screen_origin.y - 50.f, 22.f, 20 * 3, Color(255, 0, 0, 240));
		Render::Get().RenderCircleFilled(screen_origin.x, screen_origin.y - 50.f, 20.f, 20 * 3, Color(0, 0, 0, 240));
		Render::Get().RenderTextGiantMOLOTOV("l", screen_origin.x - 8, screen_origin.y - 45.f - 20, 26.f, Color(255, 255, 255));

	}

}

void RENDER_3D_GUI::AutowallCrosshair()
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive()) return;

		//QAngle direction, EyeAng, angles;
		//Vector forward, dst;
		//g_EngineClient->GetViewAngles(&EyeAng);

		//Math::AngleVectors(EyeAng, forward);//EyeAng, forward
		//dst = g_LocalPlayer->GetEyePos() + (forward * 8912.f);

		//int screenX, screenY = 0;
		//g_EngineClient->GetScreenSize(screenX, screenY);


		//static auto percent_col = [](int per) -> Color {
		//	int red = per < 50 ? 255 : floorf(255 - (per * 2 - 100) * 255.f / 100.f);
		//	int green = per > 50 ? 255 : floorf((per * 2) * 255.f / 100.f);

		//	return Color(red, green, 0);
		//};

		//int w, h = 0;
		//g_EngineClient->GetScreenSize(w, h);

		//auto dmg = CAutoWall::Get().get_estimated_point_damage(dst);
		//auto max_dmg = 50.f;


		//max_dmg = float(100);
		//if (max_dmg == 0.f)
		//	max_dmg = 50.f;

		//auto col = percent_col(std::clamp(dmg, 0.f, max_dmg) / max_dmg * 100.f);

		//std::stringstream ss;
		//ss << "DMG : " << dmg;



		//Render::Get().RenderCircleFilled(screenX / 2, screenY / 2, 5, 5 * 3, Color(0, 0, 0, 20));
		//Render::Get().RenderCircleFilled(screenX / 2, screenY / 2, 5, 4 * 3, col);
		//Render::Get().RenderTextPixel(ss.str(), screenX / 2 - 15, screenY / 2 + 50, 14.f, col);
	
}

void RENDER_3D_GUI::SpreadCircle()
{

}	


void DrawModel()
{

}
