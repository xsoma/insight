#pragma once

#include "valve_sdk/csgostructs.hpp"
#include "helpers/vfunc_hook.hpp"
#include <d3d9.h>

namespace index {
	
	constexpr auto EmitSound1               = 5;
	constexpr auto EmitSound2               = 6;
    constexpr auto EndScene                 = 42;
    constexpr auto Reset                    = 16;
    constexpr auto PaintTraverse            = 41;
    constexpr auto CreateMove               = 24; // fixed
	constexpr auto WriteUserCmd             = 24; // fixed
    constexpr auto PlaySound                = 82;
    constexpr auto FrameStageNotify         = 37;
	constexpr auto LvLPreEntity             = 5;
	constexpr auto LvLShutdown              = 7;
    constexpr auto DrawModelExecute         = 21;
    constexpr auto DoPostScreenSpaceEffects = 44;
	constexpr auto SvCheatsGetBool          = 13;
	constexpr auto OverrideView             = 18;
	constexpr auto LockCursor               = 67;
	constexpr auto AspectRatio              = 101;
	constexpr auto FireEvent                = 9;
	constexpr auto RunCommand               = 19;
	constexpr auto SetupMove                = 20;
	constexpr auto InPrediction             = 14;
	constexpr auto ProcessPacket            = 39;
	constexpr auto SendNetMes               = 40;
	constexpr auto IsHLTV                   = 93;
	constexpr auto FixedFireEvent           = 59;
	

}





namespace Hooks
{
    void Initialize();
    void Shutdown();

	inline vfunc_hook net_hook; // skeet.idb
	inline vfunc_hook player_hook; // skeet.idb
	inline vfunc_hook prediction_hook; // predict hook
    inline vfunc_hook hlclient_hook;
	inline vfunc_hook engine_hook; // new
	inline vfunc_hook gameevents_hook;
	inline recv_prop_hook* sequence_hook;
	inline vfunc_hook direct3d_hook;
	inline vfunc_hook vguipanel_hook;
	inline vfunc_hook vguisurf_hook;
	inline vfunc_hook mdlrender_hook;
	inline vfunc_hook viewrender_hook;
	inline vfunc_hook sound_hook;
	inline vfunc_hook clientmode_hook;
	inline vfunc_hook sv_cheats;

	typedef bool(__fastcall* ShouldSkipAnimFrameFn)();
	typedef void(__thiscall* BuildTransformationsFn)(void* ecx, int a2, int a3, int a4, int a5, int a6, int a7);
	typedef void(__thiscall* DoExtraBoneProcessingFn)(C_BasePlayer*, CStudioHdr*, Vector*, Quaternion*, const matrix3x4_t&, uint8_t*, void*);
	typedef void(__thiscall* StandardBlendingRulesFn)(C_BasePlayer*, studiohdr_t*, Vector*, Quaternion*, float, int);
	typedef bool(__thiscall* InPredictionFn)(IPrediction*);
	typedef void(__thiscall* SetupMoveFn)(C_BasePlayer*, CUserCmd*, IMoveHelper*, CMoveData*);
	typedef void(__thiscall* RunCommandFn)(void*, C_BasePlayer*, CUserCmd*, IMoveHelper*);
	typedef bool(__thiscall* WriteUsercmdDeltaToBufferFn)(void* ecx, int nSlot, void* buf, int from, int to, bool isNewCmd);

	



	int32_t __fastcall hkIsBoxVisible(IVEngineClient* engine_client, uint32_t, Vector& min, Vector& max);
	bool __fastcall hkIsHLTV(IVEngineClient* IEngineClient, uint32_t);
	void __fastcall hkFFireEvent(IVEngineClient* ecx);
	void __fastcall hkDoExtraBoneProcessing(C_BasePlayer* Player, uint32_t, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& matrix, uint8_t* bone_computed, void* context);
	void __fastcall hkUpdateClientSideAnimation(C_BasePlayer* player, uint32_t);
	bool  __fastcall InPrediction(IPrediction* prediction, uint32_t);
	void __stdcall  HkSetupMove(C_BasePlayer* m_pPlayer, CUserCmd* m_pCmd, IMoveHelper* m_pMoveHelper, CMoveData* m_pMoveData);
	void __fastcall  HkRunCommand(void* ecx, void* edx, C_BasePlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper);
	void __fastcall HkBuildTransformations(C_BasePlayer* player, uint32_t, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& transform, int32_t mask, byte* computed);
	bool  __fastcall hkIsConnected();
	bool __fastcall hkWriteUsercmdDeltaToBuffer(void* ECX, void* EDX, int nSlot, bf_write* buf, int from, int to, bool isNewCmd);
    long __stdcall hkEndScene(IDirect3DDevice9* device);
    long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);
	void __fastcall hkProcessPacket(void* ecx, void* edx, void* packet, bool header);
	bool __fastcall HkSendNetMsg(INetChannel* pNetChan, void* edx, INetMessage& msg, bool bForceReliable, bool bVoice);
	bool __stdcall hkCreateMove(float smt, CUserCmd * cmd);
	void __fastcall hkPaintTraverse(void* _this, int edx, vgui::VPANEL panel, bool forceRepaint, bool allowForce);
	static float __stdcall GetScreenAspectRatio(int width, int height) noexcept; // new
	void __fastcall hkEmitSound1(void* _this, int, IRecipientFilter & filter, int iEntIndex, int iChannel, const char * pSoundEntry, unsigned int nSoundEntryHash, const char * pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector * pOrigin, const Vector * pDirection, void * pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int unk);
    void __fastcall hkDrawModelExecute(void* _this, int, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld);
    void __fastcall hkFrameStageNotify(void* _this, int, ClientFrameStage_t stage);

	void __fastcall hkLvLPreEntity(void* _this, const char* map);
	void __fastcall hkLvLShutdown(void* _this);

	void __fastcall hkOverrideView(void* _this, int, CViewSetup * vsView);
	void __fastcall hkLockCursor(void* _this);
    int  __fastcall hkDoPostScreenEffects(void* _this, int, int a1);
	bool __fastcall hkSvCheatsGetBool(void* pConVar, void* edx);
	bool __stdcall hkFireEvent(IGameEvent* pEvent);
	void hkRecvProxy(const CRecvProxyData* pData, void* entity, void* output);
}
