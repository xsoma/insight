#pragma once
#include "../../valve_sdk/csgostructs.hpp"
#include "../../options.hpp"
#include <vector>

struct BulletImpactInfo
{
	float m_flExpTime;
	Vector m_vecHitPos;
};

class BulletImpactEvent : public IGameEventListener2, public Singleton<BulletImpactEvent>
{
public:

	void FireGameEvent(IGameEvent *event);
	int  GetEventDebugID(void);

	void RegisterSelf();
	void UnregisterSelf();

	void Paint(void);

	void AddSound(C_BasePlayer * p);

private:

	std::vector<BulletImpactInfo> bulletImpactInfo;
};