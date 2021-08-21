#include "detour_hook.h"
#include "../utils.hpp"
#include "../../options.hpp"
// Detour nano-tech
// by @Snake & @Kamaz

namespace DetourHooks {
	void Hook ( ) {
		static const auto SetupBonesAd = Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C")  + 0x4E;
		static const auto pPlayerVtb   = Utils::PatternScan (GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C") + 0x47;

		DWORD* pPointer              = (DWORD*)*(DWORD*)  (pPlayerVtb);
		DWORD* pBonePointer          = (DWORD*)*(DWORD*)(SetupBonesAd);

		//OriginalUpdateClientSideAnimation = (UpdateClientSideAnimationT)DetourFunction((PBYTE)pPointer[223], (PBYTE)hkUpdateClientSideAnimation);
		//OriginalDoExtraBoneProcessing     = (DoExtraBoneProcessingT    )DetourFunction((PBYTE)pPointer[197], (PBYTE)hkDoExtraBoneProcessing);
		//OriginalStandardBlendingRules     = (StandardBlendingRulesT    )DetourFunction((PBYTE)pPointer[205], (PBYTE)hkStandardBlendingRules);

		//OriginalSetupBones = (SetupBonesT)DetourFunction((PBYTE)pBonePointer[13], (PBYTE)hkSetupBones);
	}

	void Shutdown ( ) 	{
	//	DetourRemove((PBYTE)OriginalSetupBones, (PBYTE)hkSetupBones);
		//DetourRemove((PBYTE)OriginalUpdateClientSideAnimation, (PBYTE)hkUpdateClientSideAnimation);
		//DetourRemove((PBYTE)OriginalDoExtraBoneProcessing,     (PBYTE)hkDoExtraBoneProcessing);
		//DetourRemove((PBYTE)OriginalStandardBlendingRules,     (PBYTE)hkStandardBlendingRules);
	}

};