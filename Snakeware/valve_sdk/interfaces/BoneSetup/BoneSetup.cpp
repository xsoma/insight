#include "BoneSetup.h"
#include "../../../helpers/math.hpp"
// func
C_BasePlayer* GetPlayer(const int index) { return reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(index)); }
