#pragma once

#include "sdk.hpp"
#include <array>
#include "../helpers/utils.hpp"
#include "../helpers/math.hpp"
#include <chrono>
#include <mutex>
#include <algorithm>
#include <deque>
#include <xmemory>
#include <limits>
#include "../helpers/x0r-protection/xor.h"

#define Assert( _exp ) ((void)0)
#define OFFSET(func, type, offset) type& func { return *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset); }
#define FOFFSET( type, ptr, offset ) ( *( type* ) ( ( std::uintptr_t ) (ptr) + ( offset ) ) )
#define NETVAR(type, name, table, netvar)                           \
    type& name##() const {                                          \
        static int _##name = NetvarSys::Get().GetOffset(table, netvar);     \
        return *(type*)((uintptr_t)this + _##name);                 \
    }
/////////////////////////////////////////////////////////////////////////////////////////
#define NETVARADDOFFS(type, name, table, netvar, offs)                           \
    type& name##() const {                                          \
        static int _##name = NetvarSys::Get().GetOffset(table, netvar) + offs;     \
        return *(type*)((uintptr_t)this + _##name);                 \
	}
/////////////////////////////////////////////////////////////////////////////////////////
#define PNETVAR(type, name, table, netvar)                           \
    type* name##() const {                                          \
        static int _##name = NetvarSys::Get().GetOffset(table, netvar);     \
        return (type*)((uintptr_t)this + _##name);                 \
    }
//////////////////////////////////////////////////////////////////////////////////////
#define NETPROP(name, table, netvar) static RecvProp* name() \
{ \
	static auto prop_ptr = NetvarSys::Get().GetNetvarProp(table,netvar); \
	return prop_ptr; \
}
/////////////////////////////////////////////////////////////////////////////////////
#define POFFSET(funcname, type, offset) type* funcname() \
{ \
	static uint16_t _offset = offset; \
	Assert(_offset); \
	return reinterpret_cast<type*>(uintptr_t(this) + _offset); \
}


#define PPOFFSET(func, type, offset) __forceinline type& func { return **reinterpret_cast<type**>(reinterpret_cast<uintptr_t>(this) + offset); }

/////////////////////////////////////////////////////////////////////////////////////
#define APOFFSET(funcname, type, num, offset) std::array<type, num>& funcname() \
{ \
	static uint16_t _offset = offset; \
	Assert(_offset); \
	return **reinterpret_cast<std::array<type, num>**>(uintptr_t(this) + _offset); \
}
/////////////////////////////////////////////////////////////////////////////////////


struct datamap_t;
class AnimationLayer;
class CBasePlayerAnimState;
class CCSGOPlayerAnimState;
class C_BaseEntity;

using PoseParams = std::array<float, 24u>;
using AnimLayers = std::array<AnimationLayer, 13u>;
using BoneArrayS = std::array<matrix3x4_t, 128u>; // wtf

enum CSWeaponType
{
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_PLACEHOLDER,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_UNKNOWN
};


template <typename T>
struct BitFlag {
	BitFlag() = default;
	BitFlag(const T& value) { m_value = value; }

	__forceinline bool Has(const T& value) const { return m_value & value; }

	__forceinline void Add(const T& value) { m_value |= value; }

	__forceinline void Remove(const T& value) { m_value &= ~value; }

	__forceinline void Clear() { m_value = {}; }

	__forceinline bool Empty() const { return m_value == std::numeric_limits<T>::quiet_NaN(); }

	__forceinline operator T() const { return m_value; }

	__forceinline BitFlag<T>& operator=(const BitFlag<T>& value) {
		m_value = value.m_value;

		return *this;
	}

	__forceinline T& operator=(const T& value) {
		m_value = value;

		return m_value;
	}

	T m_value = {};
};


// Created with ReClass.NET by KN4CK3R
class CHudTexture
{
public:
	char szShortName[64];    //0x0000
	char szTextureFile[64];  //0x0040
	bool bRenderUsingFont;   //0x0080
	bool bPrecached;         //0x0081
	int8_t cCharacterInFont; //0x0082
	uint8_t pad_0083[1];     //0x0083
	uint32_t hFont;          //0x0084
	int32_t iTextureId;      //0x0088
	float afTexCoords[4];    //0x008C
	uint8_t pad_009C[16];    //0x009C
};

class C_EconItemView
{
private:
	using str_32 = char[32];
public:
	NETVAR(int32_t, m_bInitialized, "DT_BaseAttributableItem", "m_bInitialized");
	NETVAR(int16_t, m_iItemDefinitionIndex, "DT_BaseAttributableItem", "m_iItemDefinitionIndex"); // old 12int by snake
	NETVAR(int32_t, m_iEntityLevel, "DT_BaseAttributableItem", "m_iEntityLevel");
	NETVAR(int32_t, m_iAccountID, "DT_BaseAttributableItem", "m_iAccountID");
	NETVAR(int32_t, m_iItemIDLow, "DT_BaseAttributableItem", "m_iItemIDLow");
	NETVAR(int32_t, m_iItemIDHigh, "DT_BaseAttributableItem", "m_iItemIDHigh");
	NETVAR(int32_t, m_iEntityQuality, "DT_BaseAttributableItem", "m_iEntityQuality");
	NETVAR(str_32, m_iCustomName, "DT_BaseAttributableItem", "m_szCustomName");
};

class C_BaseEntity : public IClientEntity
{
public:

	// Rifk
	static constexpr auto E_FL_DIRTYABSVELOCITY = 0x1000;
	static constexpr auto E_F_BONEMERGE = 1;
	static constexpr auto E_F_NOINTERP = 8;
	static constexpr auto E_FITEM = 0x100;
	enum invalidate_physics_bits
	{
		position_changed = 0x1,
		angles_changed = 0x2,
		velocity_changed = 0x4,
		animation_changed = 0x8,
		bounds_changed = 0x10,
		sequence_changed = 0x20
	};

	datamap_t * GetDataDescMap() {
		typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
		return CallVFunction<o_GetPredDescMap>(this, 15)(this);
	}

	datamap_t *GetPredDescMap() {
		typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
		return CallVFunction<o_GetPredDescMap>(this, 17)(this);
	}
	static __forceinline C_BaseEntity* GetEntityByIndex(int index) {
		return static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(index));
	}
	static __forceinline C_BaseEntity* get_entity_from_handle(CBaseHandle h) {
		return static_cast<C_BaseEntity*>(g_EntityList->GetClientEntityFromHandle(h));
	}

	NETVAR(int32_t, m_nModelIndex, "DT_BaseEntity", "m_nModelIndex");
	NETVAR(int32_t, m_iTeamNum, "DT_BaseEntity", "m_iTeamNum");
	NETVAR(Vector, m_vecOrigin, "DT_BaseEntity", "m_vecOrigin");
	NETVAR(Vector, m_vecAngles, "DT_BaseEntity", "m_vecAngles");
	NETVAR(bool, m_bShouldGlow, "DT_DynamicProp", "m_bShouldGlow");
	NETVAR(CHandle<C_BasePlayer>, m_hOwnerEntity, "DT_BaseEntity", "m_hOwnerEntity");
	NETVAR(bool, m_bSpotted, "DT_BaseEntity", "m_bSpotted");
	NETVAR(float_t, m_flC4Blow, "DT_PlantedC4", "m_flC4Blow");

	void SetModelIndex(const int index)
	{
		return CallVFunction<void(__thiscall*)(C_BaseEntity*, int)>(this, 75)(this, index);
	}

	const matrix3x4_t& m_rgflCoordinateFrame()
	{
		static auto _m_rgflCoordinateFrame = NetvarSys::Get().GetOffset("DT_BaseEntity", "m_CollisionGroup") - 0x30;
		return *(matrix3x4_t*)((uintptr_t)this + _m_rgflCoordinateFrame);
	}

	bool IsPlayer();
	bool IsLoot();
	bool IsWeapon();
	bool IsPlantedC4();
	bool IsDefuseKit();
	//bool isSpotted();
};

struct C_Inferno : public C_BaseEntity {
	float GetSpawnTime() {
		return *(float*)((uintptr_t)this + 0x20);
	}
	static float GetExpireTime() {
		return 7.f;
	}
};

class C_EnvTonemapController : public C_BaseEntity
{
public:
	NETVAR(bool, use_custom_auto_exposure_min, "DT_EnvTonemapController", "m_bUseCustomAutoExposureMin");
	NETVAR(bool, use_custom_auto_exposure_max, "DT_EnvTonemapController", "m_bUseCustomAutoExposureMax");
	NETVAR(bool, use_custom_bloom_scale, "DT_EnvTonemapController", "m_bUseCustomBloomScale");
	NETVAR(float_t, custom_auto_exposure_min, "DT_EnvTonemapController", "m_flCustomAutoExposureMin");
	NETVAR(float_t, custom_auto_exposure_max, "DT_EnvTonemapController", "m_flCustomAutoExposureMax");
	NETVAR(float_t, custom_bloom_scale, "DT_EnvTonemapController", "m_flCustomBloomScale");
	NETVAR(float_t, custom_bloom_scale_minimum, "DT_EnvTonemapController", "m_flCustomBloomScaleMinimum");
	NETVAR(float_t, bloom_exponent, "DT_EnvTonemapController", "m_flBloomExponent");
	NETVAR(float_t, bloom_saturation, "DT_EnvTonemapController", "m_flBloomSaturation");
	NETVAR(float_t, tonemap_percent_target, "DT_EnvTonemapController", "m_flTonemapPercentTarget");
	NETVAR(float_t, tonemap_percent_bright_pixels, "DT_EnvTonemapController", "m_flTonemapPercentBrightPixels");
	NETVAR(float_t, tonemap_min_avg_lum, "DT_EnvTonemapController", "m_flTonemapMinAvgLum");
	NETVAR(float_t, tonemap_rate, "DT_EnvTonemapController","m_flTonemapRate");
};

class C_PlantedC4
{
public:
	NETVAR(bool, m_bBombTicking, "DT_PlantedC4", "m_bBombTicking");
	NETVAR(bool, m_bBombDefused, "DT_PlantedC4", "m_bBombDefused");
	NETVAR(float, m_flC4Blow, "DT_PlantedC4", "m_flC4Blow");
	NETVAR(float, m_flTimerLength, "DT_PlantedC4", "m_flTimerLength");
	NETVAR(float, m_flDefuseLength, "DT_PlantedC4", "m_flDefuseLength");
	NETVAR(float, m_flDefuseCountDown, "DT_PlantedC4", "m_flDefuseCountDown");
	NETVAR(CHandle<C_BasePlayer>, m_hBombDefuser, "DT_PlantedC4", "m_hBombDefuser");
};

class C_BaseAttributableItem : public C_BaseEntity
{
public:
	NETVAR(uint64_t, m_OriginalOwnerXuid, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
	NETVAR(int32_t, m_OriginalOwnerXuidLow, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
	NETVAR(int32_t, m_OriginalOwnerXuidHigh, "DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh");
	NETVAR(int32_t, m_nFallbackStatTrak, "DT_BaseAttributableItem", "m_nFallbackStatTrak");
	NETVAR(int32_t, m_nFallbackPaintKit, "DT_BaseAttributableItem", "m_nFallbackPaintKit");
	NETVAR(int32_t, m_nFallbackSeed, "DT_BaseAttributableItem", "m_nFallbackSeed");
	NETVAR(float_t, m_flFallbackWear, "DT_BaseAttributableItem", "m_flFallbackWear");

	NETVAR(C_EconItemView, m_Item2, "DT_BaseAttributableItem", "m_Item");

	C_EconItemView& m_Item()
	{
		return *(C_EconItemView*)this;
	}
	void SetGloveModelIndex(int modelIndex);
	void SetModelIndex(const int index)
	{
		return CallVFunction<void(__thiscall*)(C_BaseEntity*, int)>(this, 75)(this, index);
	}

};

class C_BaseWeaponWorldModel : public C_BaseEntity
{
public:
	NETVAR(int32_t, m_nModelIndex, "DT_BaseWeaponWorldModel", "m_nModelIndex");
};

class C_BaseCombatWeapon : public C_BaseAttributableItem
{
public:
	NETVAR(float_t, m_flNextPrimaryAttack, "DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
	NETVAR(float_t, m_flNextSecondaryAttack, "DT_BaseCombatWeapon", "m_flNextSecondaryAttack");
	NETVAR(int32_t, m_iClip1, "DT_BaseCombatWeapon", "m_iClip1");
	NETVAR(int32_t, m_iClip2, "DT_BaseCombatWeapon", "m_iClip2");
	NETVAR(float_t, m_flRecoilIndex, "DT_WeaponCSBase", "m_flRecoilIndex");
	NETVAR(int32_t, m_iViewModelIndex, "DT_BaseCombatWeapon", "m_iViewModelIndex");
	NETVAR(int32_t, m_iWorldModelIndex, "DT_BaseCombatWeapon", "m_iWorldModelIndex");
	NETVAR(int32_t, m_iWorldDroppedModelIndex, "DT_BaseCombatWeapon", "m_iWorldDroppedModelIndex");
	NETVAR(bool, m_bPinPulled, "DT_BaseCSGrenade", "m_bPinPulled");
	NETVAR(float_t, m_fThrowTime, "DT_BaseCSGrenade", "m_fThrowTime");
	NETVAR(float_t, m_flPostponeFireReadyTime, "DT_BaseCombatWeapon", "m_flPostponeFireReadyTime");
	NETVAR(float_t, m_fLastShotTime, "DT_BaseCombatWeapon", "m_fLastShotTime");
	NETVAR(CHandle<C_BaseWeaponWorldModel>, m_hWeaponWorldModel, "DT_BaseCombatWeapon", "m_hWeaponWorldModel");
	NETVAR(int32_t, m_zoomLevel, "CWeaponCSBaseGun", "m_zoomLevel");
	CCSWeaponInfo* GetCSWeaponData();

	__forceinline float GetMaxSpeed() {
		const auto weapon_data = GetCSWeaponData();
		if (!weapon_data)
			return 260.f;

		return m_zoomLevel() ? weapon_data->flMaxPlayerSpeed : weapon_data->flMaxPlayerSpeedAlt;
	}



	bool HasBullets();
	bool IsWeaponNonAim();
	bool CanFire();
	bool IsGrenade();
	bool CanShiftTickbase ();
	bool IsKnife();
	bool IsReloading();
	bool IsRifle  ();
	bool IsZeus   ();
	bool IsPistol ();
	bool IsAuto();
	bool IsShotgun();
	bool IsSMG();
	bool IsMashineGun();
	bool IsSniper ();
	bool IsGun    ();
	float GetInaccuracy();
	float GetSpread();

	float Hitchance();

	void UpdateAccuracyPenalty();
	CUtlVector<IRefCounted*>& m_CustomMaterials();
	bool* m_bCustomMaterialInitialized();



};
class CBoneAccessor;
class CBoneCache;

class C_BaseRagdoll: public C_BaseEntity {
public:
	// Pasted
	NETVAR(Vector, m_vecForce, "DT_CSRagdoll", "m_vecForce");
	NETVAR(Vector, m_vecRagdollVelocity, "DT_CSRagdoll", "m_vecRagdollVelocity");
};
//Player lag compensation...




class C_BasePlayer : public C_BaseEntity
{
public:
	static __forceinline C_BasePlayer* GetPlayerByUserId(int id)
	{
		return static_cast<C_BasePlayer*>(GetEntityByIndex(g_EngineClient->GetPlayerForUserID(id)));
	}
	static __forceinline C_BasePlayer* GetPlayerByIndex(int i)
	{
		return static_cast<C_BasePlayer*>(GetEntityByIndex(i));
	}

	template< typename t >
	__forceinline t Ass() {
		return (t)this;
	}
	PPOFFSET(GetGayLayers(), AnimLayers, 0x2980)
	NETVAR(bool, m_bHasDefuser, "DT_CSPlayer", "m_bHasDefuser");
	NETVAR(bool, m_bGunGameImmunity, "DT_CSPlayer", "m_bGunGameImmunity");
	NETVAR(int32_t, m_iShotsFired, "DT_CSPlayer", "m_iShotsFired");
	NETVAR(QAngle, m_angEyeAngles, "DT_CSPlayer", "m_angEyeAngles[0]");
	NETVAR(int, m_ArmorValue, "DT_CSPlayer", "m_ArmorValue");
	NETVAR(bool, m_bHasHeavyArmor, "DT_CSPlayer", "m_bHasHeavyArmor");
	NETVAR(bool, m_bHasHelmet, "DT_CSPlayer", "m_bHasHelmet");
	NETVAR(bool, m_bIsScoped, "DT_CSPlayer", "m_bIsScoped");;
	NETVAR(float, m_flLowerBodyYawTarget, "DT_CSPlayer", "m_flLowerBodyYawTarget");
	NETVAR(float, m_flVelocityModifier, "DT_CSPlayer", "m_flVelocityModifier");
	NETVAR(int32_t, m_iHealth, "DT_BasePlayer", "m_iHealth");
	NETVAR(int32_t, m_lifeState, "DT_BasePlayer", "m_lifeState");
	NETVAR(int32_t, m_fFlags, "DT_BasePlayer", "m_fFlags");
	NETVAR(BitFlag<uint32_t>, m_fGayFlags, "DT_BasePlayer", "m_fFlags");
	NETVAR(int32_t, m_vphysicsCollisionState, "DT_BasePlayer", "m_vphysicsCollisionState");
	NETVAR(int32_t, m_nTickBase, "DT_BasePlayer", "m_nTickBase");
	NETVAR(Vector, m_vecViewOffset, "DT_BasePlayer", "m_vecViewOffset[0]");
	NETVAR(QAngle, m_viewPunchAngle, "DT_BasePlayer", "m_viewPunchAngle");
	NETVAR(QAngle, m_aimPunchAngle, "DT_BasePlayer", "m_aimPunchAngle");
	NETVAR(QAngle, m_aimPunchAngleVel, "DT_BasePlayer", "m_aimPunchAngleVel");
	NETVAR(CHandle<C_BaseViewModel>, m_hViewModel, "DT_BasePlayer", "m_hViewModel[0]");
	NETVAR(Vector, m_vecVelocity, "DT_BasePlayer", "m_vecVelocity[0]");
	NETVAR(Vector, m_vecMins, "DT_BaseEntity", "m_vecMins"); // min
	NETVAR(Vector, m_vecMaxs, "DT_BaseEntity", "m_vecMaxs"); // max
	NETVAR(float, m_flMaxspeed, "DT_BasePlayer", "m_flMaxspeed");
	NETVAR(CHandle<C_BasePlayer>, m_hObserverTarget, "DT_BasePlayer", "m_hObserverTarget");
	NETVAR(float, m_flFlashMaxAlpha, "DT_CSPlayer", "m_flFlashMaxAlpha");
	NETVAR(int32_t, m_nHitboxSet, "DT_BaseAnimating", "m_nHitboxSet");
	NETVAR(CHandle<C_BaseCombatWeapon>, m_hActiveWeapon, "DT_BaseCombatCharacter", "m_hActiveWeapon");
	NETVAR(int32_t, m_iAccount, "DT_CSPlayer", "m_iAccount");
	NETVAR(float, m_flFlashDuration, "DT_CSPlayer", "m_flFlashDuration");
	NETVAR(float, m_flSimulationTime, "DT_BaseEntity", "m_flSimulationTime");
	NETVAR(QAngle, m_angRotation, "DT_BaseEntity", "m_angRotation"); // wrong
	NETVARADDOFFS(float, m_flOldSimulationTime, "DT_BaseEntity", "m_flSimulationTime", 0x4); // ya daun
	POFFSET(GetBoneCache, CBoneCache, 0x2900)
		POFFSET(GetBoneAccessor, CBoneAccessor, 0x2698)
		NETVAR(bool, m_bClientSideAnimation, "DT_BaseAnimating", "m_bClientSideAnimation"); // no baseplayer...

	NETVAR(int32_t, m_nForceBone, "DT_BaseAnimating", "m_nForceBone"); // no baseplayer...

	NETVAR(float, m_flCycle, "DT_BaseAnimating", "m_flCycle");
	NETVAR(int, m_nSequence, "DT_BaseViewModel", "m_nSequence");
	NETVAR(float, m_flNextAttack, "DT_BaseCombatCharacter", "m_flNextAttack");
	NETVAR(int32_t, m_nSurvivalTeam, "DT_CSPlayer", "m_nSurvivalTeam");
	NETVAR(float, m_flHealthShotBoostExpirationTime, "DT_CSPlayer", "m_flHealthShotBoostExpirationTime");
	NETVAR(QAngle, m_angAbsAngles, "DT_BaseEntity", "m_angAbsAngles");
	NETVAR(float, m_flDuckSpeed, "DT_BasePlayer", "m_flDuckSpeed"); // gitt
	NETVAR(float, m_flDuckAmount, "DT_BasePlayer", "m_flDuckAmount");// gitt

	NETVAR(int32_t, m_iFOV, "DT_BasePlayer", "m_iFOV");
	NETVAR(int32_t, m_iDefaultFOV, "DT_BasePlayer", "m_iDefaultFOV");
	std::array<float, 24>& m_flPoseParameter() const {
		static int _m_flPoseParameter = NetvarSys::Get().GetOffset("DT_BaseAnimating", "m_flPoseParameter");
		return *(std::array<float, 24>*)((uintptr_t)this + _m_flPoseParameter);
	}
	

	PNETVAR(CHandle<C_BaseCombatWeapon>, m_hMyWeapons, "DT_BaseCombatCharacter", "m_hMyWeapons");
	PNETVAR(CHandle<C_BaseAttributableItem>, m_hMyWearables, "DT_BaseCombatCharacter", "m_hMyWearables");
	PNETVAR(char, m_szLastPlaceName, "DT_BasePlayer", "m_szLastPlaceName");


	NETPROP(m_flLowerBodyYawTargetProp, "DT_CSPlayer", "m_flLowerBodyYawTarget");
	CUserCmd*& m_pCurrentCommand();


	int C_BasePlayer::GetFOV() {
		if (m_iFOV() != 0)
			return m_iFOV();
		return m_iDefaultFOV();
	}
	matrix3x4_t* BoneCache() {
		return FOFFSET(matrix3x4_t*, GetClientRenderable(), 0x290C);
	}

	matrix3x4_t**& get_bone_cache() {
		return *reinterpret_cast<matrix3x4_t***>(reinterpret_cast<uintptr_t>(this) + 0x2910);
	}

	matrix3x4_t** CachedBones() {
		return (matrix3x4_t**)((unsigned long long)this + 0x2910);
	}

	int& CachedBonesCount() {
		return *(int*)((unsigned long long)this + 0x291C);
	}

	CUtlVector< matrix3x4_t >& m_CachedBoneData() {
		return *(CUtlVector< matrix3x4_t >*)((uintptr_t)this + 0x2910);
	}

	int GetIndex()
	{
		return *reinterpret_cast<int*>(uintptr_t(this) + 0x64);
	}

	int GetHitboxSet() {

		return *reinterpret_cast <int*>(uintptr_t (m_nHitboxSet()));
	}

	Vector GetEyeOrigin()
	{
		return m_vecOrigin() + m_vecViewOffset();
	}
	Vector& m_angAbsOrigin() {

		typedef Vector& (__thiscall* OriginalFn)(void*);

		if (this) 
			return ((OriginalFn)CallVFunction<OriginalFn>(this, 10))(this);
		else
		return Vector(0, 0, 0);
	}
	static uint32_t* GetVT()
	{
		static const auto vt = reinterpret_cast<uint32_t>(Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C")) + 0x47;
		return *reinterpret_cast<uint32_t**>(vt);
	}
	/*Gladiator v2.1*/
	void InvalidateBoneCache();
	bool SetupBonesRebulid  ();
	bool SetupBonesFixed    (matrix3x4_t mMatrix[MAXSTUDIOBONES], int iMask);
	void SetSnakewareAngles(QAngle angles);
	void SetAbsAngles(const QAngle& angles);
	const Vector WorldSpaceCenter();
	void SetAbsOrigin(const Vector& origin);

	void InvalidatePhysics(int32_t flags);
	QAngle& VisualAngles();
	int GetNumAnimOverlays();
	bool IsTeam();
	
	AnimationLayer* GetAnimOverlays();
	AnimationLayer* GetAnimOverlay(int i);
	int GetSequenceActivity(int sequence);
	CCSGOPlayerAnimState* GetPlayerAnimState();

	bool IsLocalPlayer() const; // rifkk

	static void UpdateAnimationState(CCSGOPlayerAnimState* state, QAngle angle);
	static void ResetAnimationState(CCSGOPlayerAnimState* state);
	void CreateAnimationState(CCSGOPlayerAnimState* state);

	ICollideable* GetCollisionState()
	{
		if (this)
			return reinterpret_cast<ICollideable*>((DWORD)this + 0x320);//0x31C
	}
	matrix3x4_t*& GetBoneArrayForWrite() {
		return *reinterpret_cast<matrix3x4_t**>(reinterpret_cast<uintptr_t>(this) + 0x26A8);
	}
	uint32_t& GetReadableBones() {
		return *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(this) + 0x26AC);
	}
	uint32_t& GetWritableBones() {
		return *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(this) + 0x26B0);
	}
	uint32_t& GetOcclusionFlags() {
		return *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(this) + 0xA28);
	}
	float& GetLastBoneSetupTime()
	{
		return *reinterpret_cast<float*>(uintptr_t(this) + 0x2924);
	}
	uint32_t& GetMostRecentModelBoneCounter()
	{
		return *reinterpret_cast<uint32_t*>(uintptr_t(this) + 0x2690);
	}
	uint32_t& GetOcclusionFramecount() {
		return *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(this) + 0xA30);
	}
	int& GetEffect()
	{
		static unsigned int _GetEffect = Utils::FindInDataMap(GetPredDescMap(), "m_fEffects");
		return *(int*)((uintptr_t)this + _GetEffect);
	}
	Vector& m_vecAbsVelocity()
	{
		static unsigned int _m_vecAbsVelocity = Utils::FindInDataMap(GetPredDescMap(), "m_vecAbsVelocity");
		return *(Vector*)((uintptr_t)this + _m_vecAbsVelocity);
	}
	uint32_t& m_iEFlags()
	{
		static unsigned int _m_iEFlags = Utils::FindInDataMap(GetPredDescMap(), "m_iEFlags");
		return *(uint32_t*)((uintptr_t)this + _m_iEFlags);
	}
	float_t& m_surfaceFriction()
	{
		static unsigned int _m_surfaceFriction = Utils::FindInDataMap(GetPredDescMap(), "m_surfaceFriction");
		return *(float_t*)((uintptr_t)this + _m_surfaceFriction);
	}
	Vector& m_vecBaseVelocity()
	{
		static unsigned int _m_vecBaseVelocity = Utils::FindInDataMap(GetPredDescMap(), "m_vecBaseVelocity");
		return *(Vector*)((uintptr_t)this + _m_vecBaseVelocity);
	}

	float_t& m_flMaxspeed()
	{
		static unsigned int _m_flMaxspeed = Utils::FindInDataMap(GetPredDescMap(), "m_flMaxspeed");
		return *(float_t*)((uintptr_t)this + _m_flMaxspeed);
	}
	float_t& m_flAnimTime()
	{
		static unsigned int m_flAnimTime = Utils::FindInDataMap(GetPredDescMap(), "m_flAnimTime");
		return *(float_t*)((uintptr_t)this + m_flAnimTime);
	}
	float CalcServerTime (CUserCmd * m_pcmd) {
		static int tick = 0;
		static CUserCmd* last_cmd = nullptr;

		if (!m_pcmd)
			return tick * g_GlobalVars->interval_per_tick;

		if (!last_cmd || last_cmd->hasbeenpredicted)
			tick = g_LocalPlayer->m_nTickBase();
		else
			++tick;

		last_cmd = m_pcmd;
		return tick * g_GlobalVars->interval_per_tick;
	}

	
	bool BadMatrix() {
		return false;
	}
	std::string   GetName();
	Vector        GetEyePos();
	void          SetAnimLayers(std::array<AnimationLayer, 15>& layers);
	player_info_t GetPlayerInfo();
	bool  IsVisible(bool smokecheck);
	bool          IsAlive();
	bool InDangerzone();
	bool IsEnemy();
	bool		  IsFlashed();
	bool          HasC4();
	Vector        GetHitboxPos(int hitbox_id);
	void          ForceBoneRebuilid();
	mstudiohitboxset_t * GetHitboxHandle(matrix3x4_t matrix[128]);
	mstudiobbox_t* GetHitbox(int hitbox_id);
	Vector GetHitboxPosition(int hitbox_id, matrix3x4_t boneMatrix[MAXSTUDIOBONES]);
	bool          GetHitboxPos(int hitbox, Vector& output);
	bool          PointVisible(Vector endpos);
	Vector        HitboxPosition(int hitbox_id, matrix3x4_t matrix[128]);
	Vector        GetBonePos(int bone);
	bool          CanSeePlayer(C_BasePlayer* player, int hitbox);
	bool          CanSeePlayer(C_BasePlayer* player, const Vector& pos);
	bool CanSeePoint(Vector endpos);
	void WeaponShootPos(Vector& pos);
	Vector GetShootPos();
	void UpdateClientSideAnimation();

	bool IsNotTarget();

	int m_nMoveType();
	QAngle* GetVAngles();
	float_t m_flSpawnTime();

};

class C_BaseViewModel : public C_BaseEntity
{
public:
	NETVAR(int32_t, m_nModelIndex, "DT_BaseViewModel", "m_nModelIndex");
	NETVAR(int32_t, m_nViewModelIndex, "DT_BaseViewModel", "m_nViewModelIndex");
	NETVAR(CHandle<C_BaseCombatWeapon>, m_hWeapon, "DT_BaseViewModel", "m_hWeapon");
	NETVAR(CHandle<C_BasePlayer>, m_hOwner, "DT_BaseViewModel", "m_hOwner");
	NETPROP(m_nSequence, "DT_BaseViewModel", "m_nSequence");
	void SendViewModelMatchingSequence(int sequence);
};

class AnimationLayer
{
public:
	char  pad_0000[20];
	// These should also be present in the padding, don't see the use for it though
	//float	m_flLayerAnimtime;
	//float	m_flLayerFadeOuttime;
	uint32_t m_nOrder; //0x0014
	uint32_t m_nSequence; //0x0018
	float_t m_flPrevCycle; //0x001C
	float_t m_flWeight; //0x0020
	float_t m_flWeightDeltaRate; //0x0024
	float_t m_flPlaybackRate; //0x0028
	float_t m_flCycle; //0x002C
	void* m_pOwner; //0x0030 // player's thisptr
	char  pad_0038[4]; //0x0034
}; //Size: 0x0038
class CBasePlayerAnimState
{
public:
	char pad[4];
	char bUnknown; //0x4 || triggers new animation like when switching weapons
	float framecount_thing; // ref DoProceduralFootPlant
	float someTime;
	char pad2[83];
	void* pBaseEntity; //0x60
	void* pActiveWeapon; //0x64
	void* pLastActiveWeapon; //0x68
	float m_flLastClientSideAnimationUpdateTime; //0x6C
	int m_iLastClientSideAnimationUpdateFramecount; //0x70
	float m_flFrameTime; //0x74 (Curtime - m_flLastClientSideAnimationUpdateTime)
	float m_flYaw; //0x78
	float m_flPitch; //0x7C
	float m_flGoalFeetYaw; //0x80
	float m_flCurrentFeetYaw; //0x84
	float m_flCurrentTorsoYaw; //0x88
	float m_flUnknownVelocityLean; //0x8C //changes when moving/jumping/hitting ground
	float m_flLeanAmount; //0x90
	char pad4[4]; //NaN
	float m_flFeetCycle; //0x98 0 to 1
	float m_flFeetYawRate; //0x9C 0 to 1
	float m_fUnknown2;
	float m_fDuckAmount; //0xA4
	float m_fLandingDuckAdditiveSomething; //0xA8
	float m_fUnknown3; //0xAC
	Vector m_vOrigin; //0xB0, 0xB4, 0xB8
	Vector m_vLastOrigin; //0xBC, 0xC0, 0xC4
	//Vector velocity; // 0xC8, 0xCC, 0xD0
	float m_vVelocityX; //0xC8
	float m_vVelocityY; //0xCC
	float m_vVelocityZ; //0xD0
	float m_flUnknownFloat1; //0xD4 Affected by movement and direction
	char pad6[8];
	float m_flUnknownFloat2; //0xE0 //from -1 to 1 when moving and affected by direction
	float m_flUnknownFloat3; //0xE4 //from -1 to 1 when moving and affected by direction
	float not_speed_fraction; //0xE8 //possible speed fraction
	float speed_2d; //0xEC
	float flUpVelocity; //0xF0
	float m_flSpeedNormalized; //0xF4 //from 0 to 1
	float m_flFeetSpeedForwardsOrSideWays; //0xF8 //from 0 to 2. something  is 1 when walking, 2.something when running, 0.653 when crouch walking
	float m_flFeetSpeedUnknownForwardOrSideways; //0xFC //from 0 to 3. something
	float m_flTimeSinceStartedMoving; //0x100
	float m_flTimeSinceStoppedMoving; //0x104
	bool m_bOnGround; //0x108
	bool m_bInHitGroundAnimation; //0x109
	char pad7[10];
	float m_flLastOriginZ; //0x114
	float m_flHeadHeightOrOffsetFromHittingGroundAnimation; //0x118 from 0 to 1, is 1 when standing
	float m_flStopToFullRunningFraction; //0x11C from 0 to 1, doesnt change when walking or crouching, only running
	char pad8[4]; //NaN
	float m_flUnknownFraction; //0x124 affected while jumping and running, or when just jumping, 0 to 1
	char pad9[4]; //NaN
	float m_flUnknown3;
	char pad10[528];


};
class CCSGOPlayerAnimState
{
public:
	void* pThis;
	char pad2[91];
	void* pBaseEntity; //0x60
	void* pActiveWeapon; //0x64
	void* pLastActiveWeapon; //0x68
	float m_flLastClientSideAnimationUpdateTime; //0x6C
	int m_iLastClientSideAnimationUpdateFramecount; //0x70
	float m_flEyePitch; //0x74
	float m_flEyeYaw; //0x78
	float m_flPitch; //0x7C
	float m_flGoalFeetYaw; //0x80
	float m_flCurrentFeetYaw; //0x84
	float m_flCurrentTorsoYaw; //0x88
	float m_flUnknownVelocityLean; //0x8C //changes when moving/jumping/hitting ground
	float m_flLeanAmount; //0x90
	char pad4[4]; //NaN
	float m_flFeetCycle; //0x98 0 to 1
	float m_flFeetYawRate; //0x9C 0 to 1
	float m_fUnknown2;
	float m_fDuckAmount; //0xA4
	float m_fLandingDuckAdditiveSomething; //0xA8
	float m_fUnknown3; //0xAC
	Vector m_vOrigin; //0xB0, 0xB4, 0xB8
	Vector m_vLastOrigin; //0xBC, 0xC0, 0xC4
	float m_vVelocityX; //0xC8
	float m_vVelocityY; //0xCC
	char pad5[4];
	float m_flUnknownFloat1; //0xD4 Affected by movement and direction
	char pad6[8];
	float m_flUnknownFloat2; //0xE0 //from -1 to 1 when moving and affected by direction
	float m_flUnknownFloat3; //0xE4 //from -1 to 1 when moving and affected by direction
	float m_unknown; //0xE8
	float speed_2d; //0xEC
	float flUpVelocity; //0xF0
	float m_flSpeedNormalized; //0xF4 //from 0 to 1
	float m_flFeetSpeedForwardsOrSideWays; //0xF8 //from 0 to 2. something  is 1 when walking, 2.something when running, 0.653 when crouch walking
	float m_flFeetSpeedUnknownForwardOrSideways; //0xFC //from 0 to 3. something
	float m_flTimeSinceStartedMoving; //0x100
	float m_flTimeSinceStoppedMoving; //0x104
	unsigned char m_bOnGround; //0x108
	unsigned char m_bInHitGroundAnimation; //0x109
	char pad7[10];
	float m_flLastOriginZ; //0x114
	float m_flHeadHeightOrOffsetFromHittingGroundAnimation; //0x118 from 0 to 1, is 1 when standing
	float m_flStopToFullRunningFraction; //0x11C from 0 to 1, doesnt change when walking or crouching, only running
	char pad8[4]; //NaN
	float m_flUnknownFraction; //0x124 affected while jumping and running, or when just jumping, 0 to 1
	char pad9[4]; //NaN
	float m_flUnknown3;
	char pad10[528];

	float& m_flTimeSinceInAir() {
		return *(float*)((uintptr_t)this + 0x110);
	}
	float& m_flMinPitch() {
		return *(float*)((uintptr_t)this + 0x033C);
	}
}; //Size=0x344

class DT_CSPlayerResource
{
public:
	PNETVAR(int32_t, m_nActiveCoinRank, "DT_CSPlayerResource", "m_nActiveCoinRank");
	PNETVAR(int32_t, m_nMusicID, "DT_CSPlayerResource", "m_nMusicID");
	PNETVAR(int32_t, m_nPersonaDataPublicLevel, "DT_CSPlayerResource", "m_nPersonaDataPublicLevel");
	PNETVAR(int32_t, m_nPersonaDataPublicCommendsLeader, "DT_CSPlayerResource", "m_nPersonaDataPublicCommendsLeader");
	PNETVAR(int32_t, m_nPersonaDataPublicCommendsTeacher, "DT_CSPlayerResource", "m_nPersonaDataPublicCommendsTeacher");
	PNETVAR(int32_t, m_nPersonaDataPublicCommendsFriendly, "DT_CSPlayerResource", "m_nPersonaDataPublicCommendsFriendly");
	PNETVAR(int32_t, m_iCompetitiveRanking, "DT_CSPlayerResource", "m_iCompetitiveRanking");
	PNETVAR(int32_t, m_iCompetitiveWins, "DT_CSPlayerResource", "m_iCompetitiveWins");
	PNETVAR(int32_t, m_iPlayerVIP, "DT_CSPlayerResource", "m_iPlayerVIP");
	PNETVAR(int32_t, m_iMVPs, "DT_CSPlayerResource", "m_iMVPs");
	PNETVAR(int32_t, m_iScore, "DT_CSPlayerResource", "m_iScore");
};


enum ResolverState {
	RESOLVER_STATE_NONE,
	RESOLVER_STATE_LEFT,
	RESOLVER_STATE_RIGHT
};

struct PlayerRecord {

	PlayerRecord () = default;
	PlayerRecord (C_BasePlayer* pPlayer, PlayerRecord* Previous, bool bDormant = false);
	

	void BulidBones (C_BasePlayer* pPlayer, ResolverState rState);

	bool bDormant;
	int  iSimTicks, iTotalCommands;
	float flSimTime, flAnimTime, flLastShotTime;

	QAngle eyeAngles;
	Vector vecOrigin, vecVelocity;
	BitFlag<uint32_t> mFlags;

	AnimLayers ServerLayers;

	struct {
		std::array<float, RESOLVER_STATE_RIGHT> flAbsYaw;
		std::array<PoseParams, RESOLVER_STATE_RIGHT> flPoses;
		std::array<AnimLayers, RESOLVER_STATE_RIGHT + 1u> AnimLayers;
	} mResolver;

	std::array<BoneArrayS, RESOLVER_STATE_RIGHT + 1u> mBones;
};


// by LNK1181 aka Platina 300
struct PlayerLog {

	PlayerLog() = default;

	__forceinline void Reset() {
		flSpawnTime = -1.f;

		mRecords.clear();
	}

	__forceinline PlayerRecord* GetLatestRecord() {
		if (mRecords.empty())
			return nullptr;

		return &mRecords.front();
	}

	C_BasePlayer* m_pPlayer = nullptr;

	float flSpawnTime = -1.f;

	struct Interpolated {
		Interpolated () = default;
		Interpolated (float sim_time, const Vector& origin, const Vector& velocity, bool on_ground) {
			flSimTime = sim_time;

			vecOrigin = origin;
			vecVelocity = velocity;

			bOnGround = on_ground;
		}

		__forceinline void Restore(C_BasePlayer* player) const {
			player->m_vecOrigin()  = player->m_angAbsOrigin() = vecOrigin;
			player->m_vecVelocity() = player->m_vecAbsVelocity() = vecVelocity;

			if (bOnGround)
				return player->m_fGayFlags().Add ( FL_ONGROUND );

			player->m_fGayFlags().Remove(FL_ONGROUND);
		}
		 
		float  flSimTime;
		bool   bOnGround;
		Vector vecOrigin, vecVelocity;
	};

	__forceinline void interpolate(PlayerRecord* record, PlayerRecord* previous) {
		mInterpolated.clear();

		if (record->iTotalCommands <= 1) {
			mInterpolated.emplace_back(
				record->flAnimTime,
				record->vecOrigin,
				record->vecVelocity,
				record->mFlags.Has(FL_ONGROUND)
			);
			return;
		}

		const auto land_sim_time = record->flSimTime - record->ServerLayers.at(4).m_flPlaybackRate * record->ServerLayers.at(4).m_flCycle;
		auto will_land_in_this_cycle = land_sim_time >= previous->flSimTime && record->ServerLayers.at(4).m_flCycle < 0.5f && (!record->mFlags.Has(FL_ONGROUND) || !previous->mFlags.Has(FL_ONGROUND));

		for (auto i = 1; i <= record->iTotalCommands; i++) {
			const auto lerp = i / static_cast<float>(record->iTotalCommands);

			auto interpolated = Interpolated(
				record->flAnimTime + TICKS_TO_TIME(i - 1),
				Math::Interpolate(previous->vecOrigin, record->vecOrigin, lerp),
				Math::Interpolate(previous->vecVelocity, record->vecVelocity, lerp),
				record->mFlags.Has(FL_ONGROUND)
			);

			if (will_land_in_this_cycle) {
				if (land_sim_time <= interpolated.flSimTime) {
					will_land_in_this_cycle = false;

					interpolated.bOnGround = true;
				}
				else {
					interpolated.bOnGround = previous->mFlags.Has(FL_ONGROUND);
				}
			}

			mInterpolated.push_back(std::move(interpolated));
		}
	}

	std::deque < PlayerRecord >  mRecords;
	std::vector<Interpolated> mInterpolated;
};