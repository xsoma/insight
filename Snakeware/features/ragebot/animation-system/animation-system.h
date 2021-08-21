#pragma once
#include "../../../valve_sdk/csgostructs.hpp"
#include <unordered_map>
#include <deque>
#include <optional>
#include <functional>


class Animations : public Singleton<Animations> {
public:
	

private :
	// Old : C_BasePlayerAnimState
	struct InterPolatedBackup {
		InterPolatedBackup (C_BasePlayer* pPlayer, CCSGOPlayerAnimState* state) {
			mFlags = pPlayer->m_fGayFlags();

			flFeetCycle  = state->m_flFeetCycle;
			flFeetWeight = state->m_flFeetYawRate;
		}
		// Old : C_BasePlayerAnimState
		__forceinline void Restore (C_BasePlayer* pPlayer, CCSGOPlayerAnimState* state) const {
			pPlayer->m_fGayFlags() = mFlags;

			state->m_flFeetCycle   = flFeetCycle;
			state->m_flFeetYawRate = flFeetWeight;
		}

		BitFlag<uint32_t> mFlags;
		float flFeetCycle , flFeetWeight;
	};

	struct AnimationBackup {
		AnimationBackup (C_BasePlayer* pPlayer) {
			m_Poses = pPlayer->m_flPoseParameter();
			m_State = *pPlayer->GetPlayerAnimState();
		}

		__forceinline void Restore (C_BasePlayer* pPlayer) const {
			pPlayer->m_flPoseParameter()   = m_Poses;
			*pPlayer->GetPlayerAnimState() = m_State;
		}

		PoseParams m_Poses;

		CCSGOPlayerAnimState m_State;
	};

	struct MovementBackup {
		MovementBackup ( C_BasePlayer* pPlayer ){
			vecOrigin    = pPlayer->m_vecOrigin();
			vecAbsOrigin = pPlayer->m_angAbsOrigin();

			vecVelocity    = pPlayer->m_vecVelocity();
			vecAbsVelocity = pPlayer->m_vecAbsVelocity();
		}

		__forceinline void Restore (C_BasePlayer* pPlayer) const {
			pPlayer->m_vecOrigin() = vecOrigin;
			pPlayer->m_angAbsOrigin() = vecAbsOrigin;

			pPlayer->m_vecVelocity() = vecVelocity;
			pPlayer->m_vecAbsVelocity() = vecAbsVelocity;
		}

		Vector vecOrigin, vecAbsOrigin, vecVelocity, vecAbsVelocity;
	};

public :


	void OnPlayerLog                (PlayerLog * log, PlayerRecord * record, PlayerRecord * previous);

	void Update                     (PlayerLog * log, PlayerRecord * record, PlayerRecord * previous, ResolverState r_state);

	void UpdateClientSideAnimations (C_BasePlayer* pPlayer, float Time);







	// LocalPlayer part
	float m_ServerAbsRotation = 0.f;

	void FakeAnimation            ();

	void FixLocalPlayer           ();

	void SetLocalPlayerAnimations ();




	// fake  matrix & state
	CCSGOPlayerAnimState* RealAnimstate = nullptr;
	CCSGOPlayerAnimState* FakeAnimstate = nullptr;




	QAngle m_current_real_angle = QAngle(0.f, 0.f, 0.f);
	QAngle m_current_fake_angle = QAngle(0.f, 0.f, 0.f);
	

private:

};