#include "options.hpp"

int realAimSpot[] = { 8, 7, 6, 0 };
int realHitboxSpot[] = { 0, 1, 2, 3 };
namespace Snakeware
{
	bool UpdateAnims;
	int MissedShots[65];
	int CmdTick = 0;
	int BaseTick = 0;
	QAngle LocalAngle = QAngle(0, 0, 0);
	QAngle FakeAngle = QAngle(0, 0, 0);
	QAngle RealAngle = QAngle(0, 0, 0);
	matrix3x4_t mOnShotMatrix[MAXSTUDIOBONES];
	matrix3x4_t FakeMatrix[128];
	matrix3x4_t realmatrix[128];
	matrix3x4_t FakeLagMatrix[128];
	Vector Angle;
	QAngle Aimangle = QAngle(0, 0, 0);
	uint32_t ShotCmd;
	int UnpredTick;
	CUserCmd* cmd;
	C_BasePlayer* GetPlayer;
	Vector HeadPos;
	float flNextSecondaryAttack;
	float g_flVelocityModifer;
	bool bSendPacket ;
	bool OnShot;
	bool DoubleTapCharged;
	bool g_bOverrideVelMod;
	bool LBY_Update = false;
	bool bAimbotting = false;
	int m_nTickbaseShift;
	int m_nBaseTick;
	int SkipTicks;
	bool bShotFound;
	bool bBoneSetup;


	std::string Delta;
	std::string Delta2;
	std::string Delta3;
	std::string Delta4;
	std::string EyeDelta;

	bool bVisualAimbotting = false;
	QAngle vecVisualAimbotAngs = QAngle(0.f, 0.f, 0.f);
	
	
}


CUserCmd * Cmd::GetCommand()
{

		return cmdshka;

}

Cmd g_Cmd;