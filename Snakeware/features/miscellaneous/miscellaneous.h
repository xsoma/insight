#pragma once
#include "../../valve_sdk/csgostructs.hpp"

class Miscellaneous : public Singleton<Miscellaneous>
{
public:

	bool RemoveSleeves(const char* modelName) noexcept;

	void RemoveSmoke() noexcept; // noo

	bool KnifeBestTarget();

	// knifebot
	void KnifeBot(CUserCmd * cmd);
	bool lastAttacked = false;
	int m_nBestIndex = -1;
	float m_nBestDist = -1;
	C_BasePlayer* pBestEntity;

	
	void Ragdoll();
	void LegitResolver(C_BasePlayer * enemy);
	void CallLegitResolver();
	void SetThirdpersonAngles(ClientFrameStage_t stage);
	void UpdateBreaker(CUserCmd * cmd);
	void LegitAntiAim(CUserCmd * cmd);
	void SpectatorList();
	void RenderIndicators();
	void BloomEffect();
	void ModelAmbient();
	void RenderRadar(); // aye
	void RadarRange(float * x, float * y, float range);
	void CalcRadarPoint(Vector vOrigin, int & screenx, int & screeny);
	void InfinityDuck(CUserCmd * cmd);
	void OnRenderPlayer();
	void JumpThrow(CUserCmd * cmd);
	void FakeFps();
	void Binds();
	void AutoPeekStart(CUserCmd * cmd);
	void AutoPeek(CUserCmd * cmd);
	void SilentWalk(CUserCmd * cmd);
	void ClanTag();

	void LeftKnife();

	void FakeDuck(CUserCmd * cmd);

	void SlowWalk(CUserCmd * cmd);

	void ChatSpamer();
	
private:
	CCSGOPlayerAnimState AnimState;
	float next_lby = 0.0f;
	std::string snakeware = "snakeware ";
	std::string spam = "snakeware | snakeware.xyz ";

};