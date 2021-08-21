#include <algorithm>

#include "visuals.hpp"

#include "../options.hpp"
#include "../helpers/math.hpp"
#include "../helpers/utils.hpp"

#include "../helpers/input.hpp"
#include "../menu.hpp"


RECT GetBBox(C_BaseEntity* ent)
{
	RECT rect{};
	auto collideable = ent->GetCollideable();

	if (!collideable)
		return rect;

	auto min = collideable->OBBMins();
	auto max = collideable->OBBMaxs();

	const matrix3x4_t& trans = ent->m_rgflCoordinateFrame();

	Vector points[] = {
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector pointsTransformed[8];
	for (int i = 0; i < 8; i++) {
		Math::VectorTransform(points[i], trans, pointsTransformed[i]);
	}

	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++) {
		if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i]))
			return rect;
	}

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++) {
		if (left > screen_points[i].x)
			left = screen_points[i].x;
		if (top < screen_points[i].y)
			top = screen_points[i].y;
		if (right < screen_points[i].x)
			right = screen_points[i].x;
		if (bottom > screen_points[i].y)
			bottom = screen_points[i].y;
	}
	return RECT{ (long)left, (long)top, (long)right, (long)bottom };
}

Visuals::Visuals()
{
	InitializeCriticalSection(&cs);
}

Visuals::~Visuals() {
	DeleteCriticalSection(&cs);
}

//--------------------------------------------------------------------------------
void Visuals::Render() {
}
//--------------------------------------------------------------------------------
bool Visuals::Player::Begin(C_BasePlayer* pl)
{
	if (pl->IsDormant() || !pl->IsAlive())
		return false;

	ctx.pl = pl;
	ctx.is_enemy = g_LocalPlayer->m_iTeamNum() != pl->m_iTeamNum();
	ctx.is_visible = g_LocalPlayer->CanSeePlayer(pl, HITBOX_CHEST);
	Snakeware::GetPlayer = pl;

	if (!ctx.is_enemy && g_Options.esp_enemies_only)
		return false;

	ctx.clr = ctx.is_enemy ?(ctx.is_visible ? Color(g_Options.color_esp_enemy_visible) : Color(g_Options.color_esp_enemy_occluded)) :(ctx.is_visible ? Color(g_Options.color_esp_ally_visible) : Color(g_Options.color_esp_ally_occluded));
	auto head = pl->GetHitboxPos(HITBOX_HEAD);
	auto origin = pl->m_vecOrigin();

	head.z += 15;

	if (!Math::WorldToScreen(head, ctx.head_pos) ||
		!Math::WorldToScreen(origin, ctx.feet_pos))
		return false;

	Snakeware::HeadPos = ctx.head_pos;

	auto h = fabs(ctx.head_pos.y - ctx.feet_pos.y);
	auto w = h / 1.65f;

	ctx.bbox.left = static_cast<long>(ctx.feet_pos.x - w * 0.5f);
	ctx.bbox.right = static_cast<long>(ctx.bbox.left + w);
	ctx.bbox.bottom = static_cast<long>(ctx.feet_pos.y);
	ctx.bbox.top = static_cast<long>(ctx.head_pos.y);

	return true;

}

//--------------------------------------------------------------------------------
//static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
//static inline ImVec2 operator*(const ImVec2& lhs, const float& rhs) { return ImVec2(lhs.x*rhs, lhs.y*rhs); }
//static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }

void Visuals::RenderOffscreen ()
{
	for (auto i = 0; i < g_EngineClient->GetMaxClients(); i++) {

		C_BasePlayer* entity = static_cast<C_BasePlayer*> (g_EntityList->GetClientEntity(i));

		if (!entity || !entity->isPlayer())  continue ;


		auto isOnScreen = [](Vector origin, Vector& screen) -> bool {
			if (!Math::WorldToScreen(origin, screen))
				return false;

			int iScreenWidth, iScreenHeight;
			g_EngineClient->GetScreenSize(iScreenWidth, iScreenHeight);

			bool xOk = iScreenWidth > screen.x > 0, yOk = iScreenHeight > screen.y > 0;
			return xOk && yOk;
		};

		Vector screenPos;
		if (isOnScreen(entity->GetHitboxPos(2), screenPos)) return;
		if (!entity)                                        return;
		if (!entity->IsAlive())                             return;
		

		auto radius = (float)g_Options.esp_offscreen_range + 50.f;

		QAngle viewAngles;
		g_EngineClient->GetViewAngles(&viewAngles);

		int width, height;
		g_EngineClient->GetScreenSize(width, height);

		auto center = ImVec2(width / 2.f, height / 2.f);
		auto rot = Utils::Rad(viewAngles.yaw - Math::CalcAngle(g_LocalPlayer->m_vecOrigin(), entity->m_vecOrigin()).yaw - 90.f);
		auto pos = ImVec2(center.x + radius * cosf(rot) * (2 * (0.5f + 10 * 0.5f * 0.01f)), center.y + radius * sinf(rot));
		auto line = pos - center;

		auto arrowBase = pos - (line * (g_Options.esp_offscreen_size / (2 * (tanf(Utils::Rad(45)) / 2) * line.Length())));
		auto normal = ImVec2(-line.y, line.x);
		auto left = arrowBase + normal * (g_Options.esp_offscreen_size / (2 * line.Length()));
		auto right = arrowBase + normal * (-g_Options.esp_offscreen_size / (2 * line.Length()));

		auto col = g_Options.color_esp_offscreen;

		auto alpha = Utils::Map(sinf(g_GlobalVars->realtime * 2.f), -1.f, 1.f, 0.2f, std::clamp(col[3], 0.2f, 1.f));
		Render::Get().RenderTriangleFilled(left, right, pos, ImColor(col[0], col[1], col[2], entity->IsDormant() ? 0.3f : alpha));
	}
}

void Visuals::PlayerChanger(ClientFrameStage_t stage)
{

	static int originalIdx = 0;

	auto pLocal = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
	if (!pLocal) {
		originalIdx = 0;
		return;
	}

	constexpr auto getModel = [](int team) constexpr -> const char* {
		constexpr std::array models{
		"models/player/custom_player/legacy/ctm_fbi_variantb.mdl",
		"models/player/custom_player/legacy/ctm_fbi_variantf.mdl",
		"models/player/custom_player/legacy/ctm_fbi_variantg.mdl",
		"models/player/custom_player/legacy/ctm_fbi_varianth.mdl",
		"models/player/custom_player/legacy/ctm_sas_variantf.mdl",
		"models/player/custom_player/legacy/ctm_st6_variante.mdl",
		"models/player/custom_player/legacy/ctm_st6_variantg.mdl",
		"models/player/custom_player/legacy/ctm_st6_varianti.mdl",
		"models/player/custom_player/legacy/ctm_st6_variantk.mdl",
		"models/player/custom_player/legacy/ctm_st6_variantm.mdl",
		"models/player/custom_player/legacy/tm_balkan_variantf.mdl",
		"models/player/custom_player/legacy/tm_balkan_variantg.mdl",
		"models/player/custom_player/legacy/tm_balkan_varianth.mdl",
		"models/player/custom_player/legacy/tm_balkan_varianti.mdl",
		"models/player/custom_player/legacy/tm_balkan_variantj.mdl",
		"models/player/custom_player/legacy/tm_leet_variantf.mdl",
		"models/player/custom_player/legacy/tm_leet_variantg.mdl",
		"models/player/custom_player/legacy/tm_leet_varianth.mdl",
		"models/player/custom_player/legacy/tm_leet_varianti.mdl",
		"models/player/custom_player/legacy/tm_phoenix_variantf.mdl",
		"models/player/custom_player/legacy/tm_phoenix_variantg.mdl",
		"models/player/custom_player/legacy/tm_phoenix_varianth.mdl"
		};

		switch (team) {
		case 2: return static_cast<std::size_t>(g_Options.playerModelT) < models.size() ? models[g_Options.playerModelT] : nullptr;
		case 3: return static_cast<std::size_t>(g_Options.playerModelCT) < models.size() ? models[g_Options.playerModelCT] : nullptr;
		default: return nullptr;
		}
	};

	if (const auto model = getModel(pLocal->m_iTeamNum())) {
		if (stage == ClientFrameStage_t::FRAME_RENDER_START)
			originalIdx = pLocal->m_nModelIndex();

		const auto idx = stage == ClientFrameStage_t::FRAME_RENDER_END && originalIdx ? originalIdx : g_MdlInfo->GetModelIndex(model);
		//Variables.Visuals.viewmodeloffsetx
		pLocal->SetModelIndex(idx);
	}
}

//-------------------------------------------------------------------------------
void Visuals::Player::RenderBox() {
	switch (g_Options.esp_player_boxes_type)
	{
	case 0: // default
		Render::Get().RenderBoxByType(ctx.bbox.left, ctx.bbox.top, ctx.bbox.right, ctx.bbox.bottom, ctx.clr, 1);
		break;
	case 1: // outlined
		Render::Get().RenderBoxByType(ctx.bbox.left - 1, ctx.bbox.top - 1, ctx.bbox.right + 1, ctx.bbox.bottom + 1, Color(5, 5, 5, 200), 1);
		Render::Get().RenderBoxByType(ctx.bbox.left + 1, ctx.bbox.top + 1, ctx.bbox.right - 1, ctx.bbox.bottom - 1, Color(5, 5, 5, 200), 1);
		Render::Get().RenderBoxByType(ctx.bbox.left, ctx.bbox.top, ctx.bbox.right, ctx.bbox.bottom, ctx.clr, 1);
		break;
	case 2: // corner
		Render::Get().RenderCoalBox(ctx.bbox.left, ctx.bbox.top, ctx.bbox.right, ctx.bbox.bottom, ctx.clr, 1);
		break;
	case 3: // corner outlined
		Render::Get().RenderCoalBox(ctx.bbox.left - 1, ctx.bbox.top - 1, ctx.bbox.right + 1, ctx.bbox.bottom + 1, Color(5, 5, 5, 200), 1);
		Render::Get().RenderCoalBox(ctx.bbox.left + 1, ctx.bbox.top + 1, ctx.bbox.right - 1, ctx.bbox.bottom - 1, Color(5, 5, 5, 200), 1);
		Render::Get().RenderCoalBox(ctx.bbox.left, ctx.bbox.top, ctx.bbox.right, ctx.bbox.bottom, ctx.clr, 1);
		break;
	
	}

	
}
//------------------------------------------------------------------------------------
bool Visuals::Player::IsChoke()
{


	if (ctx.pl->m_flOldSimulationTime() > ctx.pl->m_flSimulationTime())
	{
		return true;
	}

	return false;
}

void Visuals::Player::RenderFlags()
{
	auto Add = 0;
	const auto AddFlag = [&](std::string str, Color clr) -> void
	{
		
		Render::Get().RenderTextPixel(str, ctx.bbox.right + 1 , ctx.bbox.top + Add - 1, 11.f, clr);
		Add += 14;
	};
	// by snake <3 aka shitcoder


	
	

	if (g_Options.esp_player_flags_player && ctx.pl->IsAlive())
	{
		AddFlag("player", Color(255, 255, 0, 255));
	}

	if (g_Options.esp_player_flags_scoped && ctx.pl->m_bIsScoped())
	AddFlag("scoped", Color(255, 0, 255, 255));

	if (g_Options.esp_flags_kit && ctx.pl->m_bHasDefuser())
		AddFlag("defuse", Color(0, 0, 255, 255));

	if (g_Options.esp_flags_move && ctx.pl->m_vecVelocity().Length2D() > 65)
		AddFlag("move", Color(153, 50, 204, 255));

	if (g_Options.esp_flags_armour && ctx.pl->m_ArmorValue() > 0)
		AddFlag("armour", Color(255, 255, 255, 255));

	if (g_Options.esp_flags_choking && IsChoke())
		AddFlag("choking", Color(228, 0, 0, 255));

	if (g_Options.esp_flags_flash && ctx.pl->IsFlashed())
		AddFlag("flashed", Color(255, 255, 255, 155));

	//pPlayer->GetAnimOverlays()[3].

}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderName()
{
	player_info_t info = ctx.pl->GetPlayerInfo();

	auto sz = g_pESP->CalcTextSizeA(14.f, 400, 0.0f, info.szName);

	Render::Get().RenderTextPixel(info.szName, ctx.feet_pos.x - sz.x / 2, ctx.head_pos.y - sz.y, 13.f, ctx.clr);
}
//--------------------------------------------------------------------------------

void Visuals::Player::RenderHealth()
{
	static int animation_val[65];
	auto  hp = ctx.pl->m_iHealth();
	float box_h = (float)fabs(ctx.bbox.bottom - ctx.bbox.top);
	//float off = (box_h / 6.f) + 5;
	float off = 8;

	int height = (box_h * hp) / 100;

	if (animation_val[ctx.pl->EntIndex()] != height) {

		if (animation_val[ctx.pl->EntIndex()] > height)
			animation_val[ctx.pl->EntIndex()] -= (animation_val[ctx.pl->EntIndex()] - height ) / 5;

		if (animation_val[ctx.pl->EntIndex()] < height)
			animation_val[ctx.pl->EntIndex()] += (height - animation_val[ctx.pl->EntIndex()]) / 5;
	}

	int green = int(hp * 2.55f);
	int red = 255 - green;

	int x = ctx.bbox.left - off;
	int y = ctx.bbox.top;
	int w = 4;
	int h = box_h;
	

	Render::Get().RenderBoxFilled(x, y, x + w, y + h, Color(0,0,0,120), 1.f, 0);
	Render::Get().RenderBox(x + 1, y + 1, x + w - 1, y + animation_val[ctx.pl->EntIndex()] - 2, Color(red, green, 0, 255),1.f, true);

	
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderArmour()
{
	auto  armour = ctx.pl->m_ArmorValue();
	float box_h = (float)fabs(ctx.bbox.bottom - ctx.bbox.top);
	//float off = (box_h / 6.f) + 5;
	float off = 4;

	int height = (((box_h * armour) / 100));

	int x = ctx.bbox.right + off;
	int y = ctx.bbox.top;
	int w = 4;
	int h = box_h;

	Render::Get().RenderBoxFilled(x, y, x + w, y + h, Color(0, 0, 0, 120), 1.f, 0);
	Render::Get().RenderBox(x + 1, y + 1, x + w - 1, y + height - 2, Color(0, 50, 255, 255), 1.f, true);
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderWeaponName()
{
	auto weapon = ctx.pl->m_hActiveWeapon().Get();

	if (!weapon) return;
	if (!weapon->GetCSWeaponData()) return;

	

	auto GetWeaponIcon = [](short cw)
	{
		switch (cw)
		{
		case WEAPON_KNIFE:
		case WEAPON_KNIFE_T:
		case 500:
		case 505:
		case 506:
		case 507:
		case 508:
		case 509:
		case 512:
		case 514:
		case 515:
		case 516:
		case 519:
		case 520:
		case 522:
		case 523:
			return "]";
		case WEAPON_DEAGLE:
			return "A";
		case WEAPON_ELITE:
			return "B";
		case WEAPON_FIVESEVEN:
			return "C";
		case WEAPON_GLOCK:
			return "D";
		case WEAPON_P2000:
			return "E";
		case WEAPON_P250:
			return "F";
		case WEAPON_USPS:
			return "G";
		case WEAPON_TEC9:
			return "H";
		case WEAPON_CZ75:
			return "I";
		case WEAPON_REVOLVER:
			return "J";
		case WEAPON_MAC10:
			return "K";
		case WEAPON_UMP45:
			return "L";
		case WEAPON_BIZON:
			return "M";
		case WEAPON_MP7:
			return "N";
		case WEAPON_MP9:
			return "O";
		case WEAPON_P90:
			return "P";
		case WEAPON_GALIL:
			return "Q";
		case WEAPON_FAMAS:
			return "R";
		case WEAPON_M4A4:
			return "S";
		case WEAPON_M4A1S:
			return "T";
		case WEAPON_AUG:
			return "U";
		case WEAPON_SG553:
			return "V";
		case WEAPON_AK47:
			return "W";
		case WEAPON_G3SG1:
			return "X";
		case WEAPON_SCAR20:
			return "Y";
		case WEAPON_AWP:
			return "Z";
		case WEAPON_SSG08:
			return "a";
		case WEAPON_XM1014:
			return "b";
		case WEAPON_SAWEDOFF:
			return "c";
		case WEAPON_MAG7:
			return "d";
		case WEAPON_NOVA:
			return "e";
		case WEAPON_NEGEV:
			return "f";
		case WEAPON_M249:
			return "g";
		case WEAPON_ZEUS:
			return "h";
		case WEAPON_FLASHBANG:
			return "i";
		case WEAPON_HEGRENADE:
			return "j";
		case WEAPON_SMOKEGRENADE:
			return "k";
		case WEAPON_MOLOTOV:
			return "l";
		case WEAPON_DECOY:
			return "m";
		case WEAPON_INC:
			return "n";
		case WEAPON_C4:
			return "o";
		default:
			return "";
		}
	};
	auto text =  weapon->GetCSWeaponData()->szWeaponName + 7;
	auto sz = g_pESP->CalcTextSizeA(14.f, FLT_MAX, 0.0f, text);

	switch (g_Options.esp_player_weapons_type) {
	case 0 :
		
		Render::Get().RenderTextPixel(text, ctx.feet_pos.x, ctx.feet_pos.y, 14.f, ctx.clr, true, g_pESP);

		break;
	case 1:
		auto textIcon = std::string(GetWeaponIcon(weapon->m_Item().m_iItemDefinitionIndex()));
		auto vSz      = g_pIconFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, textIcon.c_str());
		Render::Get().RenderText(textIcon, ctx.feet_pos.x - 10, ctx.feet_pos.y + 3, 14.f, ctx.clr, true, g_pIconFont);

		break;

	}
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderAmmo()
{
	auto weapon = ctx.pl->m_hActiveWeapon().Get();

	if (!weapon) return;
	if (!weapon->GetCSWeaponData()) return;
	const char * clip1 = std::string(std::to_string(weapon->m_iClip1())).c_str();
	auto sz            = g_pESP->CalcTextSizeA(14.f, FLT_MAX, 0.0f, clip1);

	Render::Get().RenderTextPixel(clip1, ctx.feet_pos.x + sz.x, ctx.feet_pos.y, 14.f, ctx.clr, true, g_pESP);


}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderSnapline()
{

	int screen_w, screen_h;
	g_EngineClient->GetScreenSize(screen_w, screen_h);

	Render::Get().RenderLine(screen_w / 2.f, (float)screen_h,
		ctx.feet_pos.x, ctx.feet_pos.y, ctx.clr);
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderSkeleton() {
	auto model = ctx.pl->GetModel();
	if (ctx.pl && !model)
		return;

	studiohdr_t * pStudioModel = g_MdlInfo->GetStudioModel(model);

	if (pStudioModel) {
		static matrix3x4_t pBoneToWorldOut[128];

		if (ctx.pl->SetupBones(pBoneToWorldOut, 128, 256, g_GlobalVars->curtime)) {
			for (int i = 0; i < pStudioModel->numbones; i++) {
				mstudiobone_t* pBone = pStudioModel->GetBone(i);
				if (!pBone || !(pBone->flags & 256) || pBone->parent == -1)
					continue;

				Vector vBonePos1;
				if (!Math::WorldToScreen(Vector(pBoneToWorldOut[i][0][3], pBoneToWorldOut[i][1][3], pBoneToWorldOut[i][2][3]), vBonePos1))
					continue;

				Vector vBonePos2;
				if (!Math::WorldToScreen(Vector(pBoneToWorldOut[pBone->parent][0][3], pBoneToWorldOut[pBone->parent][1][3], pBoneToWorldOut[pBone->parent][2][3]), vBonePos2))
					continue;

				Render::Get().RenderLine((int)vBonePos1.x, (int)vBonePos1.y, (int)vBonePos2.x, (int)vBonePos2.y, Color::White);
			}
		}
	}
}
//--------------------------------------------------------------------------------
void Visuals::RenderCrosshair()
{
	int w, h;

	g_EngineClient->GetScreenSize(w, h);

	int cx = w / 2;
	int cy = h / 2;
	const auto shit = Color(g_Options.color_esp_crosshair);
	Render::Get().RenderLine(cx - 25, cy, cx + 25, cy, shit,2.f);
	Render::Get().RenderLine(cx, cy - 25, cx, cy + 25, shit,2.f);
}
//--------------------------------------------------------------------------------
void Visuals::RenderWeapon(C_BaseCombatWeapon* ent)
{
	auto clean_item_name = [](const char* name) -> const char* {
		if (name[0] == 'C')
			name++;

		auto start = strstr(name, "Weapon");
		if (start != nullptr)
			name = start + 6;

		return name;
	};

	// We don't want to Render weapons that are being held
	if (ent->m_hOwnerEntity().IsValid())
		return;

	auto bbox = GetBBox(ent);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;

	Render::Get().RenderBox(bbox, g_Options.color_esp_weapons);


	auto name = clean_item_name(ent->GetClientClass()->m_pNetworkName);

	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, name);
	int w = bbox.right - bbox.left;


	Render::Get().RenderText(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, g_Options.color_esp_weapons, g_pESP);
}
//--------------------------------------------------------------------------------
void Visuals::RenderDefuseKit(C_BaseEntity* ent)
{
	if (ent->m_hOwnerEntity().IsValid())
		return;

	auto bbox = GetBBox(ent);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;

	Render::Get().RenderBox(bbox, g_Options.color_esp_defuse);

	auto name = "Defuse Kit";
	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, name);
	int w = bbox.right - bbox.left;
	Render::Get().RenderText(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, g_Options.color_esp_defuse);
}
//--------------------------------------------------------------------------------



//---------------------------------------------------------------------------------
void Visuals::RenderPlantedC4(C_BaseEntity* ent)
{
	auto bbox = GetBBox(ent);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;


	Render::Get().RenderBox(bbox, g_Options.color_esp_c4);


	int bombTimer = std::ceil(ent->m_flC4Blow() - g_GlobalVars->curtime);
	std::string timer = std::to_string(bombTimer);

	auto name = (bombTimer < 0.f) ? "Bomb" : timer;
	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, name.c_str());
	int w = bbox.right - bbox.left;
	
	Render::Get().RenderText(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, g_Options.color_esp_c4);
}
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
void Visuals::RenderItemEsp(C_BaseEntity* ent)
{
	std::string itemstr = "Undefined";
	const model_t * itemModel = ent->GetModel();
	if (!itemModel)
		return;
	studiohdr_t * hdr = g_MdlInfo->GetStudioModel(itemModel);
	if (!hdr)
		return;
	itemstr = hdr->szName;
	if (ent->GetClientClass()->m_ClassID == ClassId_CBumpMine)
		itemstr = "";
	else if (itemstr.find("case_pistol") != std::string::npos)
		itemstr = "Pistol Case";
	else if (itemstr.find("case_light_weapon") != std::string::npos)
		itemstr = "Light Case";
	else if (itemstr.find("case_heavy_weapon") != std::string::npos)
		itemstr = "Heavy Case";
	else if (itemstr.find("case_explosive") != std::string::npos)
		itemstr = "Explosive Case";
	else if (itemstr.find("case_tools") != std::string::npos)
		itemstr = "Tools Case";
	else if (itemstr.find("random") != std::string::npos)
		itemstr = "Airdrop";
	else if (itemstr.find("dz_armor_helmet") != std::string::npos)
		itemstr = "Full Armor";
	else if (itemstr.find("dz_helmet") != std::string::npos)
		itemstr = "Helmet";
	else if (itemstr.find("dz_armor") != std::string::npos)
		itemstr = "Armor";
	else if (itemstr.find("upgrade_tablet") != std::string::npos)
		itemstr = "Tablet Upgrade";
	else if (itemstr.find("briefcase") != std::string::npos)
		itemstr = "Briefcase";
	else if (itemstr.find("parachutepack") != std::string::npos)
		itemstr = "Parachute";
	else if (itemstr.find("dufflebag") != std::string::npos)
		itemstr = "Cash Dufflebag";
	else if (itemstr.find("ammobox") != std::string::npos)
		itemstr = "Ammobox";
	else if (itemstr.find("dronegun") != std::string::npos)
		itemstr = "Turrel";
	else if (itemstr.find("exojump") != std::string::npos)
		itemstr = "Exojump";
	else if (itemstr.find("healthshot") != std::string::npos)
		itemstr = "Healthshot";
	else {
		
		return;
	}
	
	auto bbox = GetBBox(ent);
	if (bbox.right == 0 || bbox.bottom == 0)
		return;
	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, itemstr.c_str());
	int w = bbox.right - bbox.left;


	//Render::Get().RenderBox(bbox, g_Options.color_esp_item);
	Render::Get().RenderText(itemstr, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, g_Options.color_esp_item);
}
//--------------------------------------------------------------------------------
void DrawWave(Vector loc, float radius, Color color)
{
	static float Step = IM_PI * 3.0f / 40;
	Vector prev;
	for (float lat = 0; lat <= IM_PI * 3.0f; lat += Step)
	{
		float sin1 = sin(lat);
		float cos1 = cos(lat);
		float sin3 = sin(0.0);
		float cos3 = cos(0.0);

		Vector point1;
		point1 = Vector(sin1 * cos3, cos1, sin1 * sin3) * radius;
		Vector point3 = loc;
		Vector Out;
		point3 += point1;

		if (Math::WorldToScreen(point3, Out))
		{
			if (lat > 0.000)
				Render::Get().RenderLine(prev.x, prev.y, Out.x, Out.y, color);
		}
		prev = Out;
	}
}
//-------------------------------------------------------------------------------

void Visuals::RenderSoundEsp() {
	// Recoded
	UpdateSoundEsp();

	auto Add3DCircle = [](const Vector& position, Color color, float radius) {
		float precision = 160.0f;

		const float step = DirectX::XM_2PI / precision;

		for (float a = 0.f; a < DirectX::XM_2PI; a += step) {
			Vector start(radius * cosf(a) + position.x, radius * sinf(a) + position.y, position.z);
			Vector end(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y, position.z);

			Vector start2d, end2d;
			if (!Math::WorldToScreen(start, start2d) || !Math::WorldToScreen(end, end2d))
				return;

			Render::Get().RenderLine(start2d.x, start2d.y, end2d.x, end2d.y, color);
		}
	};


	for (auto&[entIndex, sound] : m_Sounds) {
		if (sound.empty())
			continue;

		for (auto& info : sound) {
			if (info.flSoundTime + g_Options.sound_esp_time < g_GlobalVars->realtime)
				info.flAlpha -= g_GlobalVars->frametime;

			if (info.flAlpha <= 0.0f)
				continue;

			float deltaTime = g_GlobalVars->realtime - info.flSoundTime;

			auto factor = deltaTime / g_Options.sound_esp_time;
			if (factor > 1.0f)
				factor = 1.0f;

			float radius = g_Options.sound_esp_radius * factor;

			switch (g_Options.sound_esp_type) {
			case 0:
				Add3DCircle(info.vecSoundPos, Color (g_Options.color_sound_esp), radius);
				break;
			case 1:
			
				static float lasttime[64] = { 0.f };

				if (g_GlobalVars->curtime - lasttime[entIndex] > .5f) { // pFix
					BeamInfo_t beamInfo;
					beamInfo.m_nType         = TE_BEAMRINGPOINT;
					beamInfo.m_pszModelName  = "sprites/purplelaser1.vmt";
					beamInfo.m_nModelIndex   = g_MdlInfo->GetModelIndex("sprites/purplelaser1.vmt");
					beamInfo.m_nHaloIndex    = -1;
					beamInfo.m_flHaloScale   = 5;
					beamInfo.m_flLife        = 1.f; //
					beamInfo.m_flWidth       = 14.f;
					beamInfo.m_flFadeLength  = 1.0f;
					beamInfo.m_flAmplitude   = 0.f;
					beamInfo.m_flRed         = g_Options.color_sound_esp[0];
					beamInfo.m_flGreen       = g_Options.color_sound_esp[1];
					beamInfo.m_flBlue        = g_Options.color_sound_esp[2];
					beamInfo.m_flBrightness  = g_Options.color_sound_esp[3];
					beamInfo.m_flSpeed       = 0.f;
					beamInfo.m_nStartFrame   = 0.f;
					beamInfo.m_flFrameRate   = 60.f;
					beamInfo.m_nSegments     = -1;
					beamInfo.m_nFlags        = FBEAM_FADEOUT;
					beamInfo.m_vecCenter     = info.vecSoundPos + Vector(0, 0, 5);
					beamInfo.m_flStartRadius = 20.f;
					beamInfo.m_flEndRadius   = 640.f;
					auto myBeam              = g_RenderBeam->CreateBeamRingPoint(beamInfo);

					if (myBeam)
						g_RenderBeam->DrawBeam(myBeam);

					lasttime[entIndex] = g_GlobalVars->curtime;
				}
				else
					info.flAlpha = 0.f;
			
			break;
			}
		}

		while (!sound.empty()) {
			auto& back = sound.back();
			if (back.flAlpha <= 0.0f)
				sound.pop_back();
			else
				break;
		}
	}
}

void Visuals::UpdateSoundEsp () {

	vecSounds.RemoveAll();
	g_EngineSound->GetActiveSounds(vecSounds);
	if (vecSounds.Count() < 1)
		return;

	Vector eye_pos = g_LocalPlayer->GetEyePos();

	for (int i = 0; i < vecSounds.Count(); ++i) {
		SndInfo_t& sound = vecSounds.Element(i);
		if (sound.m_nSoundSource < 1)
			continue;

		C_BasePlayer* player = C_BasePlayer::GetPlayerByIndex(sound.m_nSoundSource);
		if (!player)
			continue;

		if (player->m_hOwnerEntity().IsValid() && player->IsWeapon()) {
			player = (C_BasePlayer*)player->m_hOwnerEntity().Get();
		}

		if (!player->IsPlayer() || !player->IsAlive())
			continue;

		if ((C_BasePlayer*)g_LocalPlayer == player)
			continue;

		if (player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum()) return;

		if (player->m_vecOrigin().DistTo(g_LocalPlayer->m_vecOrigin()) > 900)
			continue;

		auto& player_sound = m_Sounds[player->EntIndex()];
		if (player_sound.size() > 0) {
			bool bShouldBreak = false;
			for (const auto& snd : player_sound) {
				if (snd.Uid == sound.m_nGuid) {
					bShouldBreak = true;
					break;
				}
			}

			if (bShouldBreak)
				continue;
		}

		SoundEspInfo& snd    = player_sound.emplace_back();
		snd.Uid              = sound.m_nGuid;
		snd.vecSoundPos      = *sound.m_pOrigin;
		snd.flSoundTime      = g_GlobalVars->realtime;
		snd.flAlpha          = 1.0f;
	}
}

//----------------------------------------------------------------------------


void Visuals::Player::RenderHeadDot()
{
	Color clr = Color(g_Options.color_head_dot);
	Render::Get().RenderBox(ctx.head_pos.x - 2, ctx.head_pos.y - 2, ctx.head_pos.x + 2, ctx.head_pos.y + 2, clr);
}



//----------------------------------------------------------------------------
void Visuals::ThirdPerson() {
	if (!g_LocalPlayer)
		return;

	// recoded
	if (InputSys::Get().WasKeyPressed(g_Options.misc_thirdperson_key) && !Menu::Get().IsVisible())
		g_Options.misc_thirdperson = !g_Options.misc_thirdperson;

	if (g_Options.misc_thirdperson && g_LocalPlayer->IsAlive())
	{
		if (!g_Input->m_fCameraInThirdPerson)
			g_Input->m_fCameraInThirdPerson = true;
	}
	else
	{
		g_Input->m_fCameraInThirdPerson = false;
	}
}
#include "legitbot/legitbot.h"
#include "ragebot/autowall/ragebot-autowall.h"
#include "ragebot/ragebot.h"
#include "3D Rendering/render_helper.h"
void Visuals::DrawFOV() {
	auto pWeapon = g_LocalPlayer->m_hActiveWeapon();
	if (!pWeapon)
		return;

	auto settings = g_Options.legitbot_items[pWeapon->m_Item().m_iItemDefinitionIndex()];

	if (settings.enabled) {

		float fov = static_cast<float>(g_LocalPlayer->GetFOV());

		int w, h;
		g_EngineClient->GetScreenSize(w, h);

		Vector2D screenSize = Vector2D(w, h);
		Vector2D center = screenSize * 0.5f;

		float ratio = screenSize.x / screenSize.y;
		float screenFov = atanf((ratio) * (0.75f) * tan(DEG2RAD(fov * 0.5f)));

		float radiusFOV = tanf(DEG2RAD(g_LegitBot.GetFov())) / tanf(screenFov) * center.x;

	
		Render::Get().RenderCircle(center.x, center.y, radiusFOV, 32, Color(0, 0, 0, 100));

		if (settings.silent) {
			float silentRadiusFOV = tanf(DEG2RAD(settings.silent_fov)) / tanf(screenFov) * center.x;

			
			Render::Get().RenderCircle(center.x, center.y, silentRadiusFOV, 32, Color(255, 25, 10, 100));
		}
	}
}


void Visuals::RenderRecoilCrosshair()
{

	int w, h;

	g_EngineClient->GetScreenSize(w, h);



	int x = w / 2;
	int y = h / 2;
	int dy = h / 97;
	int dx = w / 97;

	QAngle punchAngle = g_LocalPlayer->m_aimPunchAngle();
	x -= (dx*(punchAngle.yaw));
	y += (dy*(punchAngle.pitch));




	Render::Get().RenderLine(x - 8, y, x + 8, y, Color(g_Options.color_esp_crosshair)); //If you want to make the PunchCross bigger or smaller just change the numbers
	Render::Get().RenderLine(x, y - 8, x, y + 8, Color(g_Options.color_esp_crosshair));


}


void Visuals::DrawScopeLines()
{

	if (!g_EngineClient->IsConnected() || !g_EngineClient->IsInGame() || !g_LocalPlayer) return;
	if (!g_LocalPlayer->m_bIsScoped()) return;

	int screenX, screenY = 0;
	g_EngineClient->GetScreenSize(screenX, screenY);

	auto wep = g_LocalPlayer->m_hActiveWeapon();

	if (!wep) return;

	auto idx = wep->m_Item().m_iItemDefinitionIndex();
	if (idx != WEAPON_AWP && idx != WEAPON_SSG08 && idx != WEAPON_SCAR20 && idx != WEAPON_G3SG1) return;

	QAngle punchAngle = g_LocalPlayer->m_aimPunchAngle();
	int value = punchAngle.yaw * 1000;



	if (g_Options.remove_scope) {



		Render::Get().RenderLine(screenX / 2 - value, 0, screenX / 2 - value, screenY / 2 - value / 2, Color(0, 0, 0, 100));
		Render::Get().RenderLine(screenX / 2 - value, screenY, screenX / 2 - value, screenY / 2 - value / 2, Color(0, 0, 0, 100));
		Render::Get().RenderLine(0, screenY / 2 - value, screenX / 2 - value / 2, screenY / 2 - value, Color(0, 0, 0, 100));
		Render::Get().RenderLine(screenX, screenY / 2 - value, screenX / 2 - value / 2, screenY / 2 - value, Color(0, 0, 0, 100));


		Render::Get().RenderLine(screenX / 2 + value, 0, screenX / 2 + value, screenY / 2 - value / 2, Color(0, 0, 0, 100));
		Render::Get().RenderLine(screenX / 2 + value, screenY, screenX / 2 + value, screenY / 2 - value / 2, Color(0, 0, 0, 100));
		Render::Get().RenderLine(0, screenY / 2 + value, screenX / 2 - value / 2, screenY / 2 + value, Color(0, 0, 0, 100));
		Render::Get().RenderLine(screenX, screenY / 2 + value, screenX / 2 - value / 2, screenY / 2 + value, Color(0, 0, 0, 100));

		Render::Get().RenderLine(screenX / 2, 0, screenX / 2, screenY, Color(g_Options.color_esp_crosshair));
		Render::Get().RenderLine(0, screenY / 2, screenX, screenY / 2, Color(g_Options.color_esp_crosshair));

		RENDER_3D_GUI::Get().AutowallCrosshair();

	
	}

}

void Visuals::AddToDrawList() {

	
	for (auto i = 1; i <= g_EntityList->GetHighestEntityIndex(); ++i) {
		auto entity = C_BaseEntity::GetEntityByIndex(i);

		if (!entity)
			continue;
		
		if (entity == g_LocalPlayer && !g_Input->m_fCameraInThirdPerson)
			continue;


		if (g_Options.misc_engine_radar) 
			entity->m_bSpotted() = true; // best netvar func

		if (i <= g_GlobalVars->maxClients) {
			auto player = Player();
			if (player.Begin((C_BasePlayer*)entity)) {

				if (g_Options.esp_player_snaplines) player.RenderSnapline();
				if (g_Options.esp_player_boxes)     player.RenderBox();
				if (g_Options.esp_player_ammo)     player.RenderAmmo();
				if (g_Options.esp_player_weapons)   player.RenderWeaponName();
				if (g_Options.esp_player_names)     player.RenderName();
				if (g_Options.esp_player_health)    player.RenderHealth();
				if (g_Options.esp_player_armour)    player.RenderArmour();
				if (g_Options.esp_player_flags)     player.RenderFlags();
				if (g_Options.esp_player_skeleton)     player.RenderSkeleton();
				if (g_Options.esp_head_dot)     player.RenderHeadDot();
			}
		}
		else if (g_Options.esp_dropped_weapons && entity->IsWeapon())
			RenderWeapon(static_cast<C_BaseCombatWeapon*>(entity));
		else if (g_Options.esp_dropped_weapons && entity->IsDefuseKit())
			RenderDefuseKit(entity);
		else if (entity->IsPlantedC4() && g_Options.esp_planted_c4)
			RenderPlantedC4(entity);
		else if (entity->IsLoot() && g_Options.esp_items)
			RenderItemEsp(entity);

	
			
	}

	auto drawAngleLine = [&](const Vector& origin, const Vector& w2sOrigin, const float& angle, const char* text, Color clr) {
		Vector forward;
		Math::AngleVectors(QAngle(0.0f, angle, 0.0f), forward);
		float AngleLinesLength = 30.0f;

		Vector w2sReal;
		if (Math::WorldToScreen(origin + forward * AngleLinesLength, w2sReal)) {
			Render::Get().RenderLine(w2sOrigin.x, w2sOrigin.y, w2sReal.x, w2sReal.y, Color::White, 1.0f);
			Render::Get().RenderBoxFilled(w2sReal.x - 5.0f, w2sReal.y - 5.0f, w2sReal.x + 5.0f, w2sReal.y + 5.0f, Color::White);
			Render::Get().RenderText(text, w2sReal.x, w2sReal.y - 5.0f, 14.0f, clr, true);
		}
	};

	extern float real_angle;
	extern float view_angle;

	if (g_Options.esp_angle_lines) {
		Vector w2sOrigin;
		if (Math::WorldToScreen(g_LocalPlayer->m_vecOrigin(), w2sOrigin)) {
			drawAngleLine(g_LocalPlayer->m_vecOrigin(), w2sOrigin, view_angle, "viewangles", Color(0.937f, 0.713f, 0.094f, 1.0f));
			drawAngleLine(g_LocalPlayer->m_vecOrigin(), w2sOrigin, g_LocalPlayer->m_flLowerBodyYawTarget(), "lby", Color(0.0f, 0.0f, 1.0f, 1.0f));
			drawAngleLine(g_LocalPlayer->m_vecOrigin(), w2sOrigin, real_angle, "real", Color(0.0f, 1.0f, 0.0f, 1.0f));
		}
	}
	
	if (g_Options.esp_crosshair)
		RenderCrosshair();

	if (g_Options.esp_recoil_crosshair)
		RenderRecoilCrosshair();

	if (g_Options.esp_offscreen)
		RenderOffscreen();

	if (g_Options.sound_esp)
		RenderSoundEsp();


	//  SELFCODE 3D RENDER BY Ba1m0v  //

	RENDER_3D_GUI::Get().LocalCircle();
	RENDER_3D_GUI::Get().DrawMolotov();
	RENDER_3D_GUI::Get().SpreadCircle();

	
}
