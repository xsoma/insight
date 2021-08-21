#include "../detour_hook.h"

DetourHooks::StandardBlendingRulesT DetourHooks::OriginalStandardBlendingRules;

void _fastcall DetourHooks::hkStandardBlendingRules(C_BasePlayer * player, uint32_t, studiohdr_t * hdr, Vector * pos, Quaternion * q, const float time, int mask) {
	if (!player)
		return OriginalStandardBlendingRules(player, hdr, pos, q, time, mask);

	*(int*)((DWORD)player + 0x2698) = 0;
	mask |= 0x200;

	player->GetEffect() |= C_BaseEntity::E_F_NOINTERP;
	OriginalStandardBlendingRules(player, hdr, pos, q, time, mask);
	player->GetEffect() &= ~C_BaseEntity::E_F_NOINTERP;
}