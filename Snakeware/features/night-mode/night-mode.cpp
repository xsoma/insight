#include "night-mode.h"
#include "../../options.hpp"
#include "../../helpers/utils.hpp"
Nightmode g_Nightmode;


static ConVar* old_sky_name;
bool executed = false;
bool NighmoveActive = true;
int BRUH = 1;

void Nightmode::Run()  {

	if (g_EngineClient->IsInGame() && g_Options.esp_nightmode) {
		Apply();
	}

}

void Nightmode::Apply()  {

	static auto st_spops   = g_CVar->FindVar("r_DrawSpecificStaticProp");
	static auto r_3dsky	   = g_CVar->FindVar("r_3dsky");
	old_sky_name           = g_CVar->FindVar("sv_skyname");
	float brightness       = g_Options.esp_nightmode_bright;

	for (auto i = 1; i <= g_EntityList->GetHighestEntityIndex(); ++i) {
		{
			auto entity = C_BasePlayer::GetEntityByIndex(i);

			if (!entity)
				continue;

			if (entity->GetClientClass()->m_ClassID == ClassId::ClassId_CEnvTonemapController)
			{
				auto hdr = dynamic_cast<C_EnvTonemapController*> (entity);
				hdr->use_custom_auto_exposure_min() = true;
				hdr->use_custom_auto_exposure_max() = true;
				hdr->custom_auto_exposure_min() = 1.f * (1.f - g_Options.esp_nightmode_bright / 100) + 0.01;
				hdr->custom_auto_exposure_max() = 1.f * (1.f - g_Options.esp_nightmode_bright / 100) + 0.01;
			}
		}


	}
		Utils::LoadNamedSky("sky_csgo_night02");
	
}
