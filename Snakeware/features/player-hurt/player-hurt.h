#pragma once

#include "../../singleton.hpp"
#include <vector>
#include "../../valve_sdk/interfaces/IGameEventmanager.hpp"
#include <map>

// gladiatorz v2.1 code
// yeap, paste


struct HitMarkerInfo
{
	float m_flExpTime;
	int m_iDmg;
};

class PlayerHurtEvent : public IGameEventListener2, public Singleton<PlayerHurtEvent>
{
public:

	void FireGameEvent(IGameEvent *event);
	int  GetEventDebugID(void);
	void KillText(IGameEvent* event);
	void RegisterSelf();
	void UnregisterSelf();

	void Paint(void);

private:

	std::vector<HitMarkerInfo> hitMarkerInfo;
};