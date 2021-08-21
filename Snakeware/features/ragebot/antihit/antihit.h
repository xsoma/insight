#pragma once
#include "../../../valve_sdk/csgostructs.hpp"


// Selfoded anti-aim.
// Credit's : @Snake.

class AntiHit : public Singleton<AntiHit> {

 public:
	// AntiAim nano-tech.

	bool switchSide = true;

	bool sideLeft, sideRight, sideBack;

	void createMove (CUserCmd * pcmd);


	void  Pitch (), Fake (), Lby(), oppositeLby();

	void  swayLby(), lowdeltaLby();

	int   getTickBase (CUserCmd * ucmd = nullptr);

	void  updateLbyBreaker (CUserCmd * usercmd);

	

	float jitterRange (int min, int max), jitterRadius;

	float EmotionPitch (), Yaw (), manualYaw();

	int pitchType, yawType, jitterType, lbyType;

 private :
	// CUserCmd class on func.
	CUserCmd *userpcmd = nullptr;
	CBaseHandle *m_ulEntHandle = nullptr;
	CCSGOPlayerAnimState *m_serverAnimState = nullptr;

	bool m_bBreakLowerBody = false;
	float_t m_flSpawnTime = 0.f, m_flNextBodyUpdate = 0.f;

};



