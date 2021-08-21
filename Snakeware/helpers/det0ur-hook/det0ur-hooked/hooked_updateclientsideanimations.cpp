#include "../detour_hook.h"
#include "../../../options.hpp"

DetourHooks::UpdateClientSideAnimationT  DetourHooks::OriginalUpdateClientSideAnimation;

void __fastcall DetourHooks::hkUpdateClientSideAnimation(C_BasePlayer * player, uint32_t) {


	if (!player || !player->IsAlive() || !player->IsPlayer())
		return OriginalUpdateClientSideAnimation(player);

	if (player->IsLocalPlayer() || player->IsTeam() || player->IsDormant())
		return OriginalUpdateClientSideAnimation(player);

	// �� ������ � ����� ��� ���������� � �������� �� ���� UpdateAnims[player->EntIndex()] 
	if (Snakeware::UpdateAnims) {

		OriginalUpdateClientSideAnimation(player);

	}
}

