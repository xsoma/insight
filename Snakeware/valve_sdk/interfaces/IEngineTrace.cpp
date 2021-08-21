
#include "../sdk.hpp"
bool CGameTrace::DidHitWorld() const
{
	//return hit_entity == g_EntityList->GetClientEntity(0);
	return true;
}

bool CGameTrace::DidHitNonWorldEntity() const
{
	return hit_entity != nullptr && !DidHitWorld();
}