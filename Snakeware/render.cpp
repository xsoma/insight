#include "render.hpp"

#include <mutex>

#include "features/visuals.hpp"
#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "menu.hpp"
#include "options.hpp"
#include "fonts/fonts.hpp"
#include "helpers/math.hpp"
#include "grenade-prediction/grenade-prediction.h"
#include "features/event-logger/event-logger.h"
#include "features/player-hurt/player-hurt.h"
#include "Lua/API.h"


ImFont* g_pDefaultFont;
ImFont* g_pESP;
ImFont* g_pBig;
ImFont* g_pPixel;
ImFont* g_pSecondFont;
ImFont* g_pCSGO_icons;
ImFont* g_Molotov_icon;
ImFont* g_pIconFont;
ImDrawListSharedData _data;

std::mutex render_mutex;

void Render::Initialize()
{
	ImGui::CreateContext();


	ImGui_ImplWin32_Init(InputSys::Get().GetMainWindow());
	ImGui_ImplDX9_Init(g_D3DDevice9);

	draw_list = new ImDrawList(ImGui::GetDrawListSharedData());
	draw_list_act = new ImDrawList(ImGui::GetDrawListSharedData());
	draw_list_rendering = new ImDrawList(ImGui::GetDrawListSharedData());

	GetFonts();
}

void Render::GetFonts() {

	


	// menu font
	ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
		Fonts::GoogleSansRegular,
		119984,
		16.f);


	g_pBig = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
		Fonts::GoogleSansMedium,
		118508,
		30.f);
	
	// esp font

	// rebulided
	g_pDefaultFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
		Fonts::GoogleSansRegular,
		119984,
		16);
	
	g_pESP = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
		Fonts::verdana,
		243304,
		13);

	g_pPixel = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
		Fonts::verdana,
		243304,
		12);

	g_pPixel = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
		Fonts::verdana,
		243304,
		12);


	// font for watermark; just example
	g_pCSGO_icons = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
		Fonts::csgoicons,
		107544,
		14.f); 

	g_Molotov_icon = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
		Fonts::molotov_icon,
		52988,
		30.f);

	g_pIconFont 
		= ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF
		(Fonts::undefeated_compressed_data,
		Fonts::undefeated_compressed_size, 16.f);
}

void Render::ClearDrawList() {
	render_mutex.lock();
	draw_list_act->Clear();
	render_mutex.unlock();
}

void Render::BeginScene() {
	draw_list->Clear();
	draw_list->PushClipRectFullScreen();


	if (g_EngineClient->IsInGame() && g_LocalPlayer)
	{
		if (!g_Options.misc_anti_screenshot && !g_EngineClient->IsTakingScreenshot() && g_Options.esp_enabled)
		{
			Visuals::Get().AddToDrawList();
			GrenadePredict.draw();
			if (g_Options.esp_draw_fov)
				Visuals::Get().DrawFOV();
		}
		if (g_Options.misc_hitmarker)
			PlayerHurtEvent::Get().Paint();
		if (g_Options.misc_event_log)
		EventLogs::Get().Draw();
		
		
	}
	Visuals::Get().DrawScopeLines();


	render_mutex.lock();
	*draw_list_act = *draw_list;
	render_mutex.unlock();
}

ImDrawList* Render::RenderScene() {

	if (render_mutex.try_lock()) {
		*draw_list_rendering = *draw_list_act;
		render_mutex.unlock();
	}

	return draw_list_rendering;
}


float Render::RenderText(const std::string& text, ImVec2 pos, float size, Color color, bool center, bool outline, ImFont* pFont)
{
	ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, text.c_str());
	if (!pFont->ContainerAtlas) return 0.f;
	draw_list->PushTextureID(pFont->ContainerAtlas->TexID);

	if (center)
		pos.x -= textSize.x / 2.0f;

	if (outline) {
		draw_list->AddText(pFont, size, ImVec2(pos.x + 1, pos.y + 1), GetU32(Color(0, 0, 0, 100)), text.c_str());
		draw_list->AddText(pFont, size, ImVec2(pos.x - 1, pos.y - 1), GetU32(Color(0, 0, 0, 100)), text.c_str());
		draw_list->AddText(pFont, size, ImVec2(pos.x + 1, pos.y - 1), GetU32(Color(0, 0, 0, 100)), text.c_str());
		draw_list->AddText(pFont, size, ImVec2(pos.x - 1, pos.y + 1), GetU32(Color(0, 0, 0, 100)), text.c_str());
	}

	draw_list->AddText(pFont, size, pos, GetU32(Color(color.r(), color.g(), color.b(), color.a())), text.c_str());

	draw_list->PopTextureID();

	return pos.y + textSize.y;
}

void Render::RenderCircle3D(Vector position, float points, float radius, Color color)
{
	float step = (float)M_PI * 2.0f / points;

	for (float a = 0; a < (M_PI * 2.0f); a += step)
	{
		Vector start(radius * cosf(a) + position.x, radius * sinf(a) + position.y, position.z);
		Vector end(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y, position.z);

		Vector start2d, end2d;
		if (g_DebugOverlay->ScreenPosition(start, start2d) || g_DebugOverlay->ScreenPosition(end, end2d))
			return;

		RenderLine(start2d.x, start2d.y, end2d.x, end2d.y, color);
	}
}
