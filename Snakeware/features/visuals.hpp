#pragma once

#include "../singleton.hpp"

#include "../render.hpp"
#include "../helpers/math.hpp"
#include "../valve_sdk/csgostructs.hpp"

#include <map>

class Visuals : public Singleton<Visuals>
{
	friend class Singleton<Visuals>;

	CRITICAL_SECTION cs;

	Visuals();
	~Visuals();



public:

	struct SoundEspInfo {
		int   Uid;
		float flSoundTime,flAlpha;
		Vector vecSoundPos;
	};

	class Player
	{
	public:
		struct
		{
			C_BasePlayer* pl;
			bool          is_enemy;
			bool          is_visible;
			Color         clr;
			Vector        head_pos;
			Vector        feet_pos;
			RECT          bbox;
		} ctx;

		bool Begin(C_BasePlayer * pl);
		void RenderBox();
		bool IsChoke();
		void RenderFlags();
		void RenderName();
		void RenderWeaponName();
		void RenderAmmo();
		void RenderHealth();
		void RenderArmour();
		void RenderSnapline();
		bool HeadPos(C_BasePlayer* player);
		void RenderSkeleton();

		void RenderHeadDot();
	
	};
	void RenderCrosshair();
	void RenderWeapon(C_BaseCombatWeapon* ent);
	void RenderDefuseKit(C_BaseEntity* ent);
	void RenderPlantedC4(C_BaseEntity* ent);
	void RenderItemEsp(C_BaseEntity* ent);
	void RenderSoundEsp();
	void UpdateSoundEsp();
	void RenderAnimatedSoundEsp();
	void ThirdPerson();
public:
	void DrawFOV();
	void RenderRecoilCrosshair();
	void DrawMolotov();
	void Indicators(); // Skeet
	void DrawScopeLines();
	void AddToDrawList();
	void Render();
	void RenderOffscreen();
	void PlayerChanger(ClientFrameStage_t stage);

 private:
	std::map< int, std::vector< SoundEspInfo > > m_Sounds;
	CUtlVector<SndInfo_t> vecSounds;

};
inline Visuals* viss;
