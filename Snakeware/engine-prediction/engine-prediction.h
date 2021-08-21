#pragma once
#include "../valve_sdk/csgostructs.hpp"
#include "../helpers/math.hpp"
// Engine-prediction
// by Slazy && PlatinaTrisotni
// Баимов как ты и просил

class EnginePrediction : public Singleton<EnginePrediction> {
private:

	struct {
		__forceinline void Store() {
			flCurTime = g_GlobalVars->curtime;
			flFrameTime = g_GlobalVars->frametime;

			bInPrediction = g_Prediction->InPrediction();

		}

		__forceinline void Restore() {
			g_GlobalVars->curtime = flCurTime;
			g_GlobalVars->frametime = flFrameTime;

			*(bool*)(uintptr_t(g_Prediction) + 8) = bInPrediction;

		}

		float flFrameTime, flCurTime;
		bool  bInPrediction;
	} Backup;

public:
	EnginePrediction() {
		iRandomSeed = *(int**)(Utils::PatternScan(GetModuleHandleA("client.dll"), "8B 0D ?? ?? ?? ?? BA ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4") + 0x2);

		MoveData = reinterpret_cast<CMoveData*>(g_MemAlloc->Alloc(sizeof(CMoveData)));
	}

	void Predict(C_BasePlayer* pPlayer, CUserCmd* pCmd);

	void ProcessPredict(C_BasePlayer * pPlayer, CUserCmd * pCmd);

	void Restore(C_BasePlayer * pPlayer, CUserCmd * pCmd);

	int* iRandomSeed;
	C_BasePlayer* m_pPlayer;

	CMoveData* MoveData;
	float flSpread, flInaccuracy;



	void PreStart();

};