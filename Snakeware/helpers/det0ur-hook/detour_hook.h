#pragma once
#include "../../valve_sdk/csgostructs.hpp"
#include "det0urs-hook.h"


namespace DetourHooks {
	void Hook     ();
	void Shutdown ();

	// Баимов не дай бог ты тронешь детуры..
	// Я тебя сука зарежу
	typedef void (__thiscall* StandardBlendingRulesT) (C_BasePlayer*, studiohdr_t*, Vector*, Quaternion*, float, int);
	extern         StandardBlendingRulesT   OriginalStandardBlendingRules;
	void _fastcall hkStandardBlendingRules (C_BasePlayer* pPlayer, uint32_t, studiohdr_t* hdr, Vector* pos, Quaternion* q, const float time, int mask);

	typedef void (__thiscall* UpdateClientSideAnimationT)  (C_BasePlayer*);
	extern          UpdateClientSideAnimationT              OriginalUpdateClientSideAnimation;
	void __fastcall hkUpdateClientSideAnimation            (C_BasePlayer* pPlayer, uint32_t);

	typedef void(__thiscall* DoExtraBoneProcessingT) (void*, studiohdr_t*, Vector*, Quaternion*, const matrix3x4_t&, uint8_t*, void*);
	extern                   DoExtraBoneProcessingT   OriginalDoExtraBoneProcessing;
	void _fastcall           hkDoExtraBoneProcessing (void* ecx, uint32_t, studiohdr_t* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& matrix, uint8_t* bone_computed, void* context);

    // From onetapv3 & legendware 
	typedef bool(__thiscall* SetupBonesT) (IClientRenderable*, matrix3x4_t*, int, int, float);
	extern                   SetupBonesT   OriginalSetupBones;
	bool __fastcall          hkSetupBones (IClientRenderable* ecx, void* edx, matrix3x4_t* bone_to_world_out, int max_bones, int bone_mask, float curtime);
};