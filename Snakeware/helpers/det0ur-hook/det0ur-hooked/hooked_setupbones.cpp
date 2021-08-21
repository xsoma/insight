
#include "../detour_hook.h"
#include "../../../options.hpp"
DetourHooks::SetupBonesT DetourHooks::OriginalSetupBones;

bool __fastcall DetourHooks::hkSetupBones (IClientRenderable* ECX, void* edx, matrix3x4_t* Bone2WorldOut, int iMaxBones, int BoneMask, float flTime) {
	
	// onetap cheat v4 
	static auto vJiggleBones              = g_CVar->FindVar("r_jiggle_bones");
	auto        backupJiggleBones         = vJiggleBones->GetInt();

	vJiggleBones->SetValue(0);

	auto pPlayer = reinterpret_cast <C_BasePlayer*> ((uintptr_t)ECX - 0x4);
	// Based on legendware.
	if (!ECX)                                                     return OriginalSetupBones(ECX, Bone2WorldOut, iMaxBones, BoneMask, flTime);

	if (!pPlayer || !pPlayer->IsAlive() || pPlayer->IsTeam() || pPlayer->IsLocalPlayer() || pPlayer->IsPlayer()) 
		return OriginalSetupBones(ECX, Bone2WorldOut, iMaxBones, BoneMask, flTime);

	if (Snakeware::bBoneSetup)                                    return OriginalSetupBones(ECX, Bone2WorldOut, iMaxBones, BoneMask, flTime);
	
	if (pPlayer == g_LocalPlayer)                                 return OriginalSetupBones(ECX, Bone2WorldOut, iMaxBones, BoneMask, flTime);

	if (!g_Options.ragebot_enabled || !g_LocalPlayer->IsAlive())  return OriginalSetupBones(ECX, Bone2WorldOut, iMaxBones, BoneMask, flTime);





	if (Bone2WorldOut) {
		if (iMaxBones > MAXSTUDIOBONES)
			iMaxBones = MAXSTUDIOBONES;

		memcpy(Bone2WorldOut, pPlayer->m_CachedBoneData().Base(), iMaxBones * sizeof(matrix3x4_t));
	}

	vJiggleBones->SetValue(backupJiggleBones);

	return true;
}

