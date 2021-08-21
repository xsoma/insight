#pragma once
#include "../../../valve_sdk/csgostructs.hpp"
#include "../../../valve_sdk/math/VMatrix.hpp"
#include <optional>
#include <array>
#include <deque>


///////////////////////////////////////////
// onetap.su & gucci & aw lagcompensation.
//////////////////////////////////////////


#define LAG_COMPENSATION_TICKS 32
#define DISABLE_INTERPOLATION  0
#define ENABLE_INTERPOLATION   1

class cTickRecord {
public:
	explicit cTickRecord() : iSequence(0), iEntFlags(0), flSimTime(0), flLby(0), flCycle(0) {
	}

	~cTickRecord() {

	}

	void Reset() {
		if (m_bDataFilled)
			return;

		vecOrigin.Clear();
		vecVelocity.Clear();
		vecObsMins.Clear();
		vecObsMaxs.Clear();
		vecHitboxPos.Clear();

		m_eyeAngles.Clear();
		m_absEyeAngles.Clear();

		iSequence = 0;
		iEntFlags = 0;

		flSimTime     = 0.f;
		flLby         = 0.f;
		flCycle       = 0.f;
		flDuckAmmount = 0.f;

		fill(begin(flPoseParamaters), end(flPoseParamaters), 0.f);
		fill(begin(flRagdollPositions), end(flRagdollPositions), 0.f);

		m_bDataFilled = false;
	}

	Vector  vecOrigin, vecabsOrigin, vecVelocity, vecObsMins, vecObsMaxs, vecHitboxPos;
	QAngle  m_eyeAngles, m_absEyeAngles;

	int iSequence, iEntFlags;

	float flSimTime, flLby, flCycle, flDuckAmmount;

	std::array<float, 24> flPoseParamaters;
	std::array<float, 24> flRagdollPositions;
	AnimationLayer        AnimLayers[15];
	matrix3x4_t           mMatrix[128];

	bool m_bDataFilled = false;
};

class cPlayerRecord {
public:
	cPlayerRecord() : pPlayer(nullptr), TickCount(0), bLagCompensated(false), flLby(0), flLbyLastUpdate(0) {
	}

	~cPlayerRecord() {
	}


	void Reset() {
		pPlayer = nullptr;
		TickCount = -1;
		HitboxPositions.Clear();
		m_eyeAngles.Clear();
		flLby = 0.f;
		flLbyLastUpdate = 0.f;

		RestoreRecord.Reset();

		if (!pRecords->empty())
			pRecords->clear();
	}

	C_BasePlayer* pPlayer;
	cTickRecord   RestoreRecord;
	int           TickCount;
	Vector        HitboxPositions;
	QAngle        m_eyeAngles;
	bool          bLagCompensated;
	float         flLby, flLbyLastUpdate;

	std::deque < cTickRecord > pRecords   [LAG_COMPENSATION_TICKS];
	
};

class cSimulationRecord  {
public:
	cSimulationRecord  () : pPlayer(nullptr), bOnGround(false) {
	}

	~cSimulationRecord () {
	}

	C_BasePlayer* pPlayer;

	Vector vecOrigin, vecVelocity;

	bool bOnGround;

	bool m_bDataFilled = false;
};

class LagCompensation : public Singleton < LagCompensation >
{
public:
	LagCompensation() {
	}

	~LagCompensation() {
	}

	void               Instance                    () const;
	static void        StoreRecordData             (C_BasePlayer * pPlayer, cTickRecord * pRecordData);
	static void        ApplyRecordData             (C_BasePlayer * pPlayer, cTickRecord * pRecordData);
	static void        UpdateAnimationsData        (C_BasePlayer * pPlayer);
    bool               IsTimeDelta2Large           (cTickRecord cRecord) const;
	static void        SimulationMovement          (cSimulationRecord * pData);
	static void        Extrapolate                 (C_BasePlayer * pPlayer, Vector & vecOrigin, Vector & vecVelocity, int & iFlags, bool bOnGround);
	void               RebulidCrouchAnimation      (C_BasePlayer * pPlayer);
	void               PredictPlayer               (C_BasePlayer * pPlayer, cTickRecord * curRecord, cTickRecord * nextRecord) const;
	int                StartLagCompensation        (C_BasePlayer * pPlayer, int iTick, cTickRecord * outRecord) const;
	void               StartPositionAdjustment     () const;
	static void        FixDormantAnimation         (C_BasePlayer * Player);
	void               StartPositionAdjustment     (C_BasePlayer * pPlayer) const;
	void               FinishPositionAdjustment    () const;
	static void        FinishPositionAdjustment    (C_BasePlayer * pPlayer);
	static void        SetInterpolationFlags       (C_BasePlayer * pPlayer, int iFlag);
	static void        Reset                       ();
	static void        FixNetvarCompression        (C_BasePlayer * pPlayer);
	static void        UpdateAnimationsInfo        (C_BasePlayer * pPlayer, cTickRecord * fFrom);



	void                 UpdatePlayerRecordData       (C_BasePlayer * pPlayer) const;
	static float         LerpTime                     ();
	static bool          bShouldLagCompensate         (C_BasePlayer * pPlayer);
	


};
