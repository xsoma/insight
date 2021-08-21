#pragma once

#include <functional>
#include "../Misc/IHandleEntity.hpp"
#include "IVEngineClient.hpp"
class IClientNetworkable;
class IClientEntity;
class C_BasePlayer;
using ulong_t = unsigned long;
class IClientEntityList
{
public:
    virtual IClientNetworkable*   GetClientNetworkable(int entnum) = 0;
    virtual void*                 vtablepad0x1(void) = 0;
    virtual void*                 vtablepad0x2(void) = 0;
    virtual IClientEntity*        GetClientEntity(int entNum) = 0;
    virtual IClientEntity*        GetClientEntityFromHandle(CBaseHandle hEnt) = 0;
    virtual int                   NumberOfEntities(bool bIncludeNonNetworkable) = 0;
    virtual int                   GetHighestEntityIndex(void) = 0;
    virtual void                  SetMaxEntities(int maxEnts) = 0;
    virtual int                   GetMaxEntities() = 0;

	void ForEachPlayer(const std::function<void(C_BasePlayer*)> fn)	{
		for (auto i = 0; i <= 65; ++i) { // m3m3
			const auto entity = GetClientEntity(i);
			if (entity && entity->isPlayer())
				fn(reinterpret_cast<C_BasePlayer*>(entity));
		}
	}

	template< typename t = C_BasePlayer * >
	__forceinline t GetClientEntityFromHandle2(ulong_t handle) {
		return CallVFunction< t(__thiscall*)(decltype(this), ulong_t) >(this, 4)(this, handle);
	}
};