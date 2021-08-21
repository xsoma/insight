#include "hooks.hpp"
#include <intrin.h>  
#include <algorithm>
#include "valve_sdk/misc/UtlVector.hpp"

#include "render.hpp"
#include "menu.hpp"
#include "options.hpp"
#include "helpers/input.hpp"
#include "helpers/utils.hpp"
#include "features/bhop.hpp"
#include "features/chams.hpp"
#include "features/fakelag/fakelag.h"
//LUA EXTENDS

#include "features/visuals.hpp"
#include "features/glow.hpp"
#include "features/miscellaneous/miscellaneous.h"
#include "engine-prediction/engine-prediction.h"
#include "features/ragebot/animation-system/animation-system.h"
#include "grenade-prediction/grenade-prediction.h"
#include "features/legitbot/legitbot.h"
#include "features/legitbot/legit-backtrack/legit-backtrack.h"
#include "features/night-mode/night-mode.h"
#include "features/ragebot/antihit/antihit.h"
#include "features/player-hurt/player-hurt.h"
#include "features/bullet-manipulation/bullet-event.h"
#include "skin-changer/skin-changer.h"
#include "features/ragebot/ragebot.h"
#include "features/ragebot/netvar-compensation/netvar-comp.h"
#include "features/event-logger/event-logger.h"
#include "features/tickbase-shift/tickbase-exploits.h"
// material system
#include "materials/Materials.h"
#include "Protected/enginer.h"
#include "Achievment_sys.h"
#include "Lua/API.h"
#define Snake
#pragma intrinsic(_ReturnAddress)  
float real_angle = 0.0f;
float view_angle = 0.0f;
int32_t originalShotsMissed = 0;
int32_t tickHitPlayer = 0;
int32_t tickHitWall = 0;


namespace Hooks {

	void Initialize() {

		INetChannel* g_NetChannel = (INetChannel*)g_ClientState->pNetChannel;
		net_hook.setup(g_NetChannel);
		prediction_hook.setup(g_Prediction);
		engine_hook.setup(g_EngineClient);
		hlclient_hook.setup(g_CHLClient);
		direct3d_hook.setup(g_D3DDevice9);
		vguipanel_hook.setup(g_VGuiPanel);
		vguisurf_hook.setup(g_VGuiSurface);
		sound_hook.setup(g_EngineSound);
		mdlrender_hook.setup(g_MdlRender);
		clientmode_hook.setup(g_ClientMode);
		ConVar* sv_cheats_con = g_CVar->FindVar("sv_cheats");
		sv_cheats.setup(sv_cheats_con);
		gameevents_hook.setup(g_GameEvents);
		gameevents_hook.hook_index(index::FireEvent, hkFireEvent);
		sequence_hook = new recv_prop_hook(C_BaseViewModel::m_nSequence(), hkRecvProxy);
		if (g_ClientState != nullptr && g_NetChannel != nullptr && g_LocalPlayer != nullptr && g_LocalPlayer->IsAlive()){
			net_hook.hook_index(index::ProcessPacket, hkProcessPacket);
			net_hook.hook_index(index::SendNetMes, HkSendNetMsg);
		}
		//prediction_hook.hook_index(index::SetupMove, HkSetupMove);
		prediction_hook.hook_index(index::InPrediction, InPrediction);
		prediction_hook.hook_index(index::RunCommand, HkRunCommand);
		direct3d_hook.hook_index(index::EndScene, hkEndScene);
		direct3d_hook.hook_index(index::Reset, hkReset);
		hlclient_hook.hook_index(index::FrameStageNotify, hkFrameStageNotify);
		clientmode_hook.hook_index(index::CreateMove, hkCreateMove);
		hlclient_hook.hook_index(index::WriteUserCmd, hkWriteUsercmdDeltaToBuffer); // like createmove
		vguipanel_hook.hook_index(index::PaintTraverse, hkPaintTraverse);
		sound_hook.hook_index(index::EmitSound1, hkEmitSound1);
		vguisurf_hook.hook_index(index::LockCursor, hkLockCursor);
		mdlrender_hook.hook_index(index::DrawModelExecute, hkDrawModelExecute);
		clientmode_hook.hook_index(index::DoPostScreenSpaceEffects, hkDoPostScreenEffects);
		clientmode_hook.hook_index(index::OverrideView, hkOverrideView);
		sv_cheats.hook_index(index::SvCheatsGetBool, hkSvCheatsGetBool);
		engine_hook.hook_index(index::AspectRatio, GetScreenAspectRatio);
		engine_hook.hook_index(index::IsHLTV, hkIsHLTV);
		engine_hook.hook_index(27, hkIsConnected);
		engine_hook.hook_index(32, hkIsBoxVisible);
		//hlclient_hook.hook_index(index::LvLPreEntity, hkLvLPreEntity);
		//hlclient_hook.hook_index(index::LvLShutdown, hkLvLShutdown);
		const char* message = "Cheat injected";
		PlayerHurtEvent::Get().RegisterSelf();
		EventLogs::Get().RegisterSelf();
		BulletImpactEvent::Get().RegisterSelf();
		
	}
	//--------------------------------------------------------------------------------
	void Shutdown()
	{
		
		prediction_hook.unhook_all();
		engine_hook.unhook_all();
		hlclient_hook.unhook_all();
		direct3d_hook.unhook_all();
		vguipanel_hook.unhook_all();
		vguisurf_hook.unhook_all();
		mdlrender_hook.unhook_all();
		clientmode_hook.unhook_all();
		sound_hook.unhook_all();
		sv_cheats.unhook_all();
		gameevents_hook.unhook_all();
		sequence_hook->~recv_prop_hook();


		Materials::Get().Remove();
		Glow::Get().Shutdown();
	//	EventLogs::Get().UnregisterSelf();
		BulletImpactEvent::Get().UnregisterSelf();
		PlayerHurtEvent::Get().UnregisterSelf();
		


	}
	//--------------------------------------------------------------------------------
	long __stdcall hkEndScene(IDirect3DDevice9* pDevice)
	{
		static auto oEndScene = direct3d_hook.get_original<decltype(&hkEndScene)>(index::EndScene);
		for (auto hk : lua::hooks->getHooks("on_scene_end"))
		{
			try
			{
				auto result = hk.func();
				if (!result.valid()) {
					sol::error err = result;
				}
			}
			catch (const std::exception&)
			{

			}
		}
		static auto viewmodel_fov = g_CVar->FindVar("viewmodel_fov");
		static auto mat_ambient_light_r = g_CVar->FindVar("mat_ambient_light_r");
		static auto mat_ambient_light_g = g_CVar->FindVar("mat_ambient_light_g");
		static auto mat_ambient_light_b = g_CVar->FindVar("mat_ambient_light_b");
		static auto crosshair_cvar = g_CVar->FindVar("crosshair");

		viewmodel_fov->m_fnChangeCallbacks.m_Size = 0;
		viewmodel_fov->SetValue(g_Options.viewmodel_fov);
		mat_ambient_light_r->SetValue(g_Options.mat_ambient_light_r);
		mat_ambient_light_g->SetValue(g_Options.mat_ambient_light_g);
		mat_ambient_light_b->SetValue(g_Options.mat_ambient_light_b);
		
		crosshair_cvar->SetValue(!(g_Options.esp_enabled && g_Options.esp_crosshair));

		DWORD colorwrite, srgbwrite;
		IDirect3DVertexDeclaration9* vert_dec = nullptr;
		IDirect3DVertexShader9* vert_shader = nullptr;
		DWORD dwOld_D3DRS_COLORWRITEENABLE = NULL;
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		//removes the source engine color correction
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
		pDevice->GetVertexDeclaration(&vert_dec);
		pDevice->GetVertexShader(&vert_shader);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);

		
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();


		auto esp_drawlist = Render::Get().RenderScene();

		static bool Textured3 = false;



		Menu::Get().Render();
		BulletImpactEvent::Get().Paint();
		

		Miscellaneous::Get().SpectatorList();
		Miscellaneous::Get().RenderIndicators();
		Miscellaneous::Get().RenderRadar();
		Miscellaneous::Get().Binds();
		ImGui::Render(esp_drawlist);

		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
		pDevice->SetVertexDeclaration(vert_dec);
		pDevice->SetVertexShader(vert_shader);

		return oEndScene(pDevice);
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		static auto oReset = direct3d_hook.get_original<decltype(&hkReset)>(index::Reset);

		Menu::Get().OnDeviceLost();

		auto hr = oReset(device, pPresentationParameters);

		if (hr >= 0)
			Menu::Get().OnDeviceReset();

		return hr;
	}
	//--------------------------------------------------------------------------------

	
	//--------------------------------------------------------------------------------
	bool  __fastcall InPrediction(IPrediction* prediction, uint32_t) {

		static auto InPrediction = prediction_hook.get_original< InPredictionFn >(index::InPrediction);

		static const auto returnTrans = *reinterpret_cast<uint32_t **>(Utils::PatternScan(GetModuleHandleA("client.dll"), "84 C0 74 17 8B 87")); // shit pattern


		if (_ReturnAddress() == (uint32_t*)returnTrans)
			return false;

		return InPrediction(prediction);
	}
	//--------------------------------------------------------------------------------
	void FixAttackPacket(CUserCmd* m_pCmd, bool m_bPredict)
	{
		static bool m_bLastAttack = false;
		static bool m_bInvalidCycle = false;
		static float m_flLastCycle = 0.f;

		if (m_bPredict)
		{
			m_bLastAttack = m_pCmd->weaponselect || (m_pCmd->buttons & IN_ATTACK2);
			m_flLastCycle = g_LocalPlayer->m_flCycle();
		}
		else if (m_bLastAttack && !m_bInvalidCycle)
			m_bInvalidCycle = g_LocalPlayer->m_flCycle() == 0.f && m_flLastCycle > 0.f;

		if      (m_bInvalidCycle)
			g_LocalPlayer->m_flCycle() = m_flLastCycle;
	}

	
	void FixResolverPacket () {
		
		auto pWeapon = g_LocalPlayer->m_hActiveWeapon();
	


		if (!pWeapon || !g_LocalPlayer) return;


		if (pWeapon->m_Item().m_iItemDefinitionIndex() == 64 && /*g_Options.ragebot_autorevolver*/ pWeapon->m_flNextSecondaryAttack() == FLT_MAX)
			pWeapon->m_flNextSecondaryAttack() = Snakeware::flNextSecondaryAttack;

	}


	//---------------------------------------------------------------------------------------------------------

	void __fastcall HkRunCommand(void* ecx, void* edx, C_BasePlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper) {
		// Onetap hack
		static auto runCommand = prediction_hook.get_original< RunCommandFn >(index::RunCommand);

		if (!player || player->EntIndex() != g_EngineClient->GetLocalPlayer()) // supremacy.vip
			return runCommand(ecx, player, ucmd, moveHelper);

		if (g_EngineClient->IsConnected () && g_EngineClient->IsInGame ()) {
			// OneTap code
			if (ucmd->tick_count >= (Snakeware::UnpredTick + int(1 / g_GlobalVars->interval_per_tick) + 8)) {
				ucmd->hasbeenpredicted = true;
				return;
			}



			float m_flVelModBackup = g_LocalPlayer->m_flVelocityModifier();
			int m_nTickbase = g_LocalPlayer->m_nTickBase();
			int m_flCurtime = g_GlobalVars->curtime;

			if (ucmd->command_number == Snakeware::ShotCmd) {
				g_LocalPlayer->m_nTickBase() = Snakeware::BaseTick - Snakeware::m_nTickbaseShift + 1;
				++g_LocalPlayer->m_nTickBase();
				g_GlobalVars->curtime = TICKS_TO_TIME(g_LocalPlayer->m_nTickBase());

			}

			runCommand(ecx, player, ucmd, moveHelper);
			FixResolverPacket();
			FixAttackPacket(ucmd, true);


			if (Snakeware::g_bOverrideVelMod && ucmd->command_number == g_ClientState->nLastCommandAck + 1)
				g_LocalPlayer->m_flVelocityModifier() = Snakeware::g_flVelocityModifer;

			FixResolverPacket();
				

			if (ucmd->command_number == Snakeware::ShotCmd) {
				g_LocalPlayer->m_nTickBase() = m_nTickbase;
				g_GlobalVars->curtime = m_flCurtime;
			}

			if (!Snakeware::g_bOverrideVelMod)
				g_LocalPlayer->m_flVelocityModifier() = m_flVelModBackup;


		
			player->m_vphysicsCollisionState() = 0; // Fixing collision state

			FixAttackPacket(ucmd, false);

			g_NetData.Store();

			
		}
		else {
			runCommand(ecx, player, ucmd, moveHelper);
		}


	}
	//------------------------------------------------------------------------------


	//-------------------------------------------------------------------------------
	int32_t __fastcall hkIsBoxVisible(IVEngineClient* engine_client, uint32_t, Vector& min, Vector& max)
	{
		typedef int32_t(__thiscall* BoxVisibleFn)(IVEngineClient*, Vector&, Vector&);
		static auto BoxVisible = engine_hook.get_original< BoxVisibleFn >(32);

		static const auto ret = ("\x85\xC0\x74\x2D\x83\x7D\x10\x00\x75\x1C");

		if (!memcmp(_ReturnAddress(), ret, 10))
			return 1;

		return BoxVisible(engine_client, min, max);
	}
	//-------------------------------------------------------------------------------
	bool __fastcall hkIsHLTV(IVEngineClient* IEngineClient, uint32_t) {
		typedef bool(__thiscall* IsHLTVFn)(IVEngineClient*);
		static auto IsHLTV = engine_hook.get_original< IsHLTVFn >(index::IsHLTV);

		static const auto Retutn2SetupVelocity = Utils::PatternScan(GetModuleHandleA(solution::Get().Module), "84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80");
		static const auto Return2AccumLayers   = Utils::PatternScan(GetModuleHandleA(solution::Get().Module), "84 C0 75 0D F6 87");
		if (_ReturnAddress() == (uint32_t*)(Return2AccumLayers) && IEngineClient->IsInGame())   return true;

		if (_ReturnAddress() == (uint32_t*)(Retutn2SetupVelocity) && IEngineClient->IsInGame()) return true;

		return IsHLTV(IEngineClient);
	}

	bool  __fastcall hkIsConnected () {
		if (!g_EngineClient) return false;
		using IsConnectedT = bool(__thiscall*)(void*);
	
		static const auto IsLoadout = Utils::PatternScan(GetModuleHandleA("client.dll"), "84 C0 75 04 B0 01 5F");

		if (g_Options.misc_unlock_inventory && _ReturnAddress() == IsLoadout)
			return false;

		return engine_hook.get_original< IsConnectedT >(27);
	}

	//----------------------------------------------------------------------------------------------------------------
	void WriteUserCmd(bf_write* buf, CUserCmd* in, CUserCmd* out) {
		static DWORD WriteUsercmdF = (DWORD)Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 51 53 56 8B D9");

		__asm
		{
			mov ecx, buf
			mov edx, in
			push out
			call WriteUsercmdF
			add esp, 4
		}
	}
	//-------------------------------------------------------------------------------
	bool __fastcall hkWriteUsercmdDeltaToBuffer(void* ECX, void* EDX, int nSlot, bf_write* Buffer, int fFrom, int iTo, bool isNewCmd)
	{
	
		static auto oReturn = Utils::PatternScan(GetModuleHandleA(solution::Get().enginedll.c_str()), "84 C0 74 04 B0 01 EB 02 32 C0 8B FE 46 3B F3 7E C9 84 C0 0F 84 ? ? ? ?");
		static auto oFunc = hlclient_hook.get_original<WriteUsercmdDeltaToBufferFn>(24);

		if (!Snakeware::m_nTickbaseShift) return oFunc(ECX, nSlot, Buffer, fFrom, iTo, isNewCmd);

		if (fFrom != -1)                  return true;

		auto iFinalFrom = -1;

		uintptr_t frame_ptr; // from LVv3
		__asm mov frame_ptr, ebp;

		auto BackupCommands = reinterpret_cast <int*> (frame_ptr + 0xFD8);
		auto NewCommands    = reinterpret_cast <int*> (frame_ptr + 0xFDC);

		auto NewCmds = *NewCommands;
		auto iShift  = Snakeware::m_nTickbaseShift;

		Snakeware::m_nTickbaseShift = 0;
		*BackupCommands             = 0;

		auto m_nTotalNewCmds = NewCmds + iShift;

		if (m_nTotalNewCmds > 62)
			m_nTotalNewCmds = 62;

		*NewCommands = m_nTotalNewCmds;

		auto mNextCmd = g_ClientState->iChokedCommands + g_ClientState->nLastOutgoingCommand + 1;
		auto iFinalTo = mNextCmd - NewCmds + 1;

		if (iFinalTo <= mNextCmd) {

			while (oFunc(ECX, nSlot, Buffer, fFrom, iTo, true)) {

				iFinalFrom = iFinalTo++;

				if (iFinalTo > mNextCmd)
					goto LABEL_22;
			}

			return false;
		}

	LABEL_22:

		auto pUserCmd = g_Input->GetUserCmd(iFinalFrom);

		if (!pUserCmd) return true;

		CUserCmd toCmd;
		CUserCmd fromCmd;

		fromCmd = *pUserCmd;
		toCmd   = fromCmd;

		toCmd.command_number++;
		toCmd.tick_count += 200;

		if (NewCmds > m_nTotalNewCmds)
			return true;

		for (auto i = m_nTotalNewCmds - NewCmds + 1; i > 0; --i) {

			WriteUserCmd(Buffer, &toCmd, &fromCmd);

			fromCmd = toCmd;

			toCmd.command_number++;
			toCmd.tick_count++;
		}

		return true;
	}
	//--------------------------------------------------------------------------------

	void __fastcall hkProcessPacket(void* ecx, void* edx, void* packet, bool header) {
		using ProcessPacket_t = void(__thiscall*)(void*, void*, bool);
		auto ProcessPacket = net_hook.get_original<ProcessPacket_t>(index::ProcessPacket);
		if (!g_ClientState->pNetChannel)
			return ProcessPacket(ecx, packet, header);

		ProcessPacket(ecx, packet, header);

		// get this from CL_FireEvents string "Failed to execute event for classId" in engine.dll
		for (CEventInfo* it{ g_ClientState->pEvents }; it != nullptr; it = it->m_next) {
			if (!it->m_class_id)
				continue;

			// set all delays to instant.
			it->m_fire_delay = 0.f;
		}

		g_EngineClient->FireEvents();
	}
	bool __fastcall HkSendNetMsg(INetChannel* pNetChan, void* edx, INetMessage& msg, bool bForceReliable, bool bVoice)
	{
		using SendNetMsg_t = bool(__thiscall*)(void*, INetMessage&, bool, bool);
		auto SendNetMsg = net_hook.get_original<SendNetMsg_t>(index::SendNetMes);

		if (msg.GetType() == 14) // Return and don't send messsage if its FileCRCCheck
			return false;

		if (msg.GetGroup() == 9) // Fix lag when transmitting voice and fakelagging
			bVoice = true;

		return SendNetMsg(pNetChan, msg, bForceReliable, bVoice);
	}

	//--------------------------------------------------------------------------------





	


	void __fastcall  hkSetupMove(void* ecx, void* edx, C_BasePlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper, void* pMoveData)
	{
		static auto ofunc = prediction_hook.get_original<decltype(&hkSetupMove)>(index::SetupMove);

		if (!player || player != g_LocalPlayer)
			return;

		if (pMoveData && &EnginePrediction::Get().MoveData != pMoveData)
			std::memcpy(&EnginePrediction::Get().MoveData, pMoveData, sizeof(CMoveData));

		return ofunc(ecx, edx, player, ucmd, moveHelper, pMoveData);
	}

	//---------------------------------------------------------------------------------
	using CreateMove_t = bool(__thiscall*)(IClientMode *, float, CUserCmd *);
	bool __stdcall hkCreateMove(float smt, CUserCmd * cmd)
	{
		for (auto hk : lua::hooks->getHooks("on_create_move"))
		{
			try
			{
				auto result = hk.func(cmd);
				if (!result.valid()) {
					sol::error err = result;
				}
			}
			catch (const std::exception&)
			{

			}
		}
		static auto original_fn = clientmode_hook.get_original< CreateMove_t >(index::CreateMove);
		RageBot::Get().ResetTarget();


		if (!cmd || !cmd->command_number)
			return  original_fn(g_ClientMode, smt, cmd);
		
		if (!g_LocalPlayer->IsAlive()) 
			return  original_fn(g_ClientMode, smt, cmd);
		Miscellaneous::Get().FakeFps();
	
		if (g_LocalPlayer->IsAlive()) {
			
			Snakeware::UnpredTick = g_GlobalVars->tickcount;
			Tickbase::Get().PreverseCharge(cmd);
			RageBot::Get().ResetTarget();
		}
		
		uintptr_t *frame_ptr;
		__asm mov frame_ptr, ebp;
		
		g_Cmd.SetCommand(cmd);
		Utils::ServerTime();

		AntiHit::Get().getTickBase(cmd);
		
		AntiHit::Get().updateLbyBreaker(cmd);

		Snakeware::bSendPacket = true;
		if (Menu::Get().IsVisible())
			cmd->buttons &= ~IN_ATTACK;

		static auto Prediction = new EnginePrediction();


		Miscellaneous::Get().KnifeBot(cmd);

		Miscellaneous::Get().LeftKnife();
		if (g_Options.misc_clantag)
			Miscellaneous::Get().ClanTag();

		Miscellaneous::Get().ChatSpamer();
		if (g_Options.misc_bhop)

			BunnyHop::OnCreateMove(cmd);
		if (g_Options.misc_autostrafe)
			BunnyHop::AutoStrafe(cmd);

		if (g_Options.misc_infinity_duck)
			Miscellaneous::Get().InfinityDuck(cmd);

		Miscellaneous::Get().JumpThrow(cmd);
		if (g_Options.remove_flash)
			g_LocalPlayer->m_flFlashDuration() = g_Options.esp_flash_ammount;
		
	
		GrenadePredict.trace(cmd); // trace lines
		Fakelag::Get().OnCreateMove(cmd); // After prediction O_o

		Snakeware::g_bOverrideVelMod = true;

		Prediction->PreStart();

		Prediction->ProcessPredict (g_LocalPlayer, cmd);
		{
			g_LegitBot.OnMove(cmd);
			g_LegitBot.TriggerBot(cmd);
			g_LegitBacktrack.OnMove(cmd);
			Miscellaneous::Get().FakeDuck(cmd);

			AntiHit::Get().createMove (cmd);

			RageBot::Get().Instance (cmd);

			Miscellaneous::Get().AutoPeek(cmd);
			Miscellaneous::Get().SlowWalk(cmd);
			Miscellaneous::Get().SilentWalk(cmd);
			Miscellaneous::Get().LegitAntiAim(cmd);
			

			Snakeware::g_bOverrideVelMod = false;

			Tickbase::Get().DoubleTap(cmd);

			if (g_Options.exploit_doubletap && GetAsyncKeyState(g_Options.exploit_doubletap_key)) {
				
					if (!Tickbase::Get().CanDoubleTap())
						Snakeware::DoubleTapCharged = false;
					else if (!Snakeware::DoubleTapCharged && Snakeware::SkipTicks == 0) {
						Snakeware::SkipTicks = 16;
					}
				
			}

		}
		Prediction->Restore(g_LocalPlayer,cmd);

		if (g_ClientState->iChokedCommands > 14)
			Snakeware::bSendPacket = true; // own

	
		// SendPacket standart
		*(bool*)(*frame_ptr - 0x1C) = Snakeware::bSendPacket;
		

		if (g_Options.misc_anti_untrusted)
			Math::Normalize3(cmd->viewangles);

		
		

		if (g_Options.misc_showranks && cmd->buttons & IN_SCORE) // rank revealer will work even after unhooking, idk how to "hide" ranks  again
			g_CHLClient->DispatchUserMessage(CS_UM_ServerRankRevealAll, 0, 0, nullptr);

		// trash code 0_0
		auto animstate = g_LocalPlayer->GetPlayerAnimState();
		if (Snakeware::bSendPacket) {
			real_angle = animstate->m_flGoalFeetYaw;
			view_angle = animstate->m_flEyeYaw;
		}

		Math::NormalizeAngles(cmd->viewangles);
		Math::ClampAngles(cmd->viewangles);
		//cmd->viewangles = Math::NormalizeAng(cmd->viewangles);
		QAngle oldAngle;
		float oldForward;
		float oldSideMove;
		g_EngineClient->GetViewAngles(&oldAngle);
		oldForward = cmd->forwardmove;
		oldSideMove = cmd->sidemove;
		if (g_LocalPlayer->m_nMoveType() != MOVETYPE_LADDER)
			Utils::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
	
		
		if (Snakeware::bSendPacket)
		{
			Snakeware::LocalAngle = cmd->viewangles;
		
			Snakeware::FakeAngle = cmd->viewangles;
		
		}
		else
		{
			Snakeware::RealAngle = cmd->viewangles;
		
		}
		
		

		return false;
	}

	//--------------------------------------------------------------------------------
	static float __stdcall GetScreenAspectRatio(int width, int height) noexcept
	{
		static auto oFunc = engine_hook.get_original<decltype(&GetScreenAspectRatio)>(index::AspectRatio);

		if (g_Options.esp_aspect_ratio)
			return g_Options.esp_aspect_ratio;

		

		return oFunc(width, height);
	}
	//--------------------------------------------------------------------------------



	//-------------------------------------------------------------------------------
	void __fastcall hkPaintTraverse(void* _this, int edx, vgui::VPANEL panel, bool forceRepaint, bool allowForce)
	{
		static auto panelId        = vgui::VPANEL{ 0 };
		static auto oPaintTraverse = vguipanel_hook.get_original<decltype(&hkPaintTraverse)>(index::PaintTraverse);
		const auto panelName       = g_VGuiPanel->GetName(panel);
		static uint32_t HudZoomPanel;

		if (!HudZoomPanel)
			if (!strcmp("HudZoom", panelName))
				HudZoomPanel = panel;

		if (HudZoomPanel == panel && g_Options.remove_scope && g_LocalPlayer && g_LocalPlayer->m_hActiveWeapon().Get())
		{
			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsSniper() && g_LocalPlayer->m_bIsScoped())
				return;
		}


		oPaintTraverse(g_VGuiPanel, edx, panel, forceRepaint, allowForce);
		if (!panelId) {
			const auto panelName = g_VGuiPanel->GetName(panel);
			if (!strcmp(panelName, "FocusOverlayPanel")) {
				panelId = panel;
			}
		}
		else if (panelId == panel)
		{
			//Ignore 50% cuz it called very often
			static bool bSkip = false;
			bSkip = !bSkip;

			if (bSkip)
				return;

			Render::Get().BeginScene();

			for (auto hooks_ : lua::hooks->getHooks("on_paint"))
			{
				try
				{
					auto result = hooks_.func();
					if (!result.valid()) {
						sol::error err = result;
					}
				}
				catch (const std::exception&)
				{

				}
			}
		}

	}
	//--------------------------------------------------------------------------------
	void __fastcall hkEmitSound1(void* _this, int edx, IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector* pOrigin, const Vector* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int unk) {
		static auto ofunc = sound_hook.get_original<decltype(&hkEmitSound1)>(index::EmitSound1);


		if (!strcmp(pSoundEntry, "UIPanorama.popup_accept_match_beep")) {
			static auto fnAccept = reinterpret_cast<bool(__stdcall*)(const char*)>(Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12"));

			if (fnAccept) {

				fnAccept("");

				//This will flash the CSGO window on the taskbar
				//so we know a game was found (you cant hear the beep sometimes cause it auto-accepts too fast)
				FLASHWINFO fi;
				fi.cbSize = sizeof(FLASHWINFO);
				fi.hwnd = InputSys::Get().GetMainWindow();
				fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
				fi.uCount = 0;
				fi.dwTimeout = 0;
				FlashWindowEx(&fi);
			}
		}

		ofunc(g_EngineSound, edx, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, nSeed, flAttenuation, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, unk);

	}
	//--------------------------------------------------------------------------------
	int __fastcall hkDoPostScreenEffects(void* _this, int edx, int a1)
	{
		static auto oDoPostScreenEffects = clientmode_hook.get_original<decltype(&hkDoPostScreenEffects)>(index::DoPostScreenSpaceEffects);

		if (g_LocalPlayer && g_Options.glow_enabled)
			Glow::Get().Run();

		return oDoPostScreenEffects(g_ClientMode, edx, a1);
	}
	//--------------------------------------------------------------------------------



	void __fastcall hkLvLPreEntity (void* _this,const char* map) {
	
		// hooked LevelPreEntity by @Snake
		// <decltype(

		using LevelInitPreEntityT = void(__thiscall*)(void*, const char*);
		static auto preEnt = hlclient_hook.get_original<LevelInitPreEntityT>(index::LvLPreEntity);


		 
		preEnt(_this,map);
		// Detour sig someone...
	}


	void  __fastcall hkLvLShutdown(void* _this) {
		// hooked LevelShutodwn by @Snake
		// <decltype(
		using LevelShutdownT = void(__thiscall*)(void*);
		static auto downEnt = hlclient_hook.get_original<LevelShutdownT>(index::LvLShutdown);



		downEnt(_this);
	}




	//-------------------------------------------------------------------------------
	void __fastcall hkFrameStageNotify(void* _this, int edx, ClientFrameStage_t stage)
	{

		static auto ofunc = hlclient_hook.get_original<decltype(&hkFrameStageNotify)>(index::FrameStageNotify);
		for (auto hk : lua::hooks->getHooks("on_frame_stage_notify"))
		{
			try
			{
				auto result = hk.func((int)stage);
				if (!result.valid()) {
					sol::error err = result;
				}
			}
			catch (const std::exception&)
			{

			}
		}
		Skins::OnFrameStageNotify(stage);
		LagCompensation::Get().Instance(stage);
		//RageBot::Get().PVSFix(stage);


		QAngle aim_punch_old;
		QAngle view_punch_old;

		QAngle *aim_punch = nullptr;
		QAngle *view_punch = nullptr;

		ConVar* mat_fullbright = g_CVar->FindVar("mat_fullbright");
		if (g_Options.esp_fullbright)
			mat_fullbright->SetValue(1);
		else
			mat_fullbright->SetValue(0);


		Miscellaneous::Get().BloomEffect();
		Miscellaneous::Get().ModelAmbient();


		if (g_LocalPlayer && g_LocalPlayer->IsAlive())
		{
			static int m_iLastCmdAck = 0;
			static float m_flNextCmdTime = 0.f;

			if (g_ClientState && (m_iLastCmdAck != g_ClientState->nLastCommandAck || m_flNextCmdTime != g_ClientState->flNextCmdTime))
			{
				if (Snakeware::g_flVelocityModifer != g_LocalPlayer->m_flVelocityModifier())
				{
					*(bool*)((uintptr_t)g_Prediction + 0x24) = true;
					Snakeware::g_flVelocityModifer = g_LocalPlayer->m_flVelocityModifier();
				}

				m_iLastCmdAck = g_ClientState->nLastCommandAck;
				m_flNextCmdTime = g_ClientState->flNextCmdTime;
			}


		}


	
	

		if (g_Options.remove_visual_recoil && stage == ClientFrameStage_t::FRAME_RENDER_START)
		{
			if (g_LocalPlayer && g_LocalPlayer->IsAlive())
			{
				aim_punch = &g_LocalPlayer->m_aimPunchAngle();
				view_punch = &g_LocalPlayer->m_viewPunchAngle();

				aim_punch_old = *aim_punch;
				view_punch_old = *view_punch;

				*aim_punch = QAngle(0, 0, 0);
				*view_punch = QAngle(0, 0, 0);
			}

			
			
		}
		if (stage == ClientFrameStage_t::FRAME_RENDER_START)
		{
			
			Miscellaneous::Get().RemoveSmoke();
			Miscellaneous::Get().Ragdoll();
			Animations::Get().SetLocalPlayerAnimations();

				g_Nightmode.Run();
				Visuals::Get().PlayerChanger(FRAME_RENDER_START);


			
		}

		if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_END)
		{
			g_NetData.Apply();
			
			

			//Animations::Get().FakeAnimation();
			Miscellaneous::Get().CallLegitResolver();
			Animations::Get().FixLocalPlayer();
			//Animations::Get().UpdatePlayerAnimations();
		}
		
		// may be u will use it lol
		ofunc(g_CHLClient, edx, stage);
		if (aim_punch && view_punch)
		{
			*aim_punch = aim_punch_old;
			*view_punch = view_punch_old;
		}



		
		
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkOverrideView(void* _this, int edx, CViewSetup* vsView)
	{
		static auto ofunc = clientmode_hook.get_original<decltype(&hkOverrideView)>(index::OverrideView);

		if (g_EngineClient->IsInGame() && vsView)
			Visuals::Get().ThirdPerson();

		if (g_Options.misc_fakeduck && GetAsyncKeyState(g_Options.misc_fakeduck_key) && g_LocalPlayer && g_LocalPlayer->IsAlive() && g_Input->m_fCameraInThirdPerson)
			vsView->origin.z = g_LocalPlayer->m_angAbsOrigin().z + 64.f;

		if (g_LocalPlayer && !g_LocalPlayer->m_bIsScoped() && g_Options.world_fov > 0 && g_Options.esp_enabled) {

			vsView->fov = 90 + g_Options.world_fov * g_LocalPlayer->m_bIsScoped() * 90;
		}

		ofunc(g_ClientMode, edx, vsView);
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkLockCursor(void* _this)
	{
		static auto ofunc = vguisurf_hook.get_original<decltype(&hkLockCursor)>(index::LockCursor);

		if (Menu::Get().IsVisible()) {
			g_VGuiSurface->UnlockCursor();
			g_InputSystem->ResetInputState();
			return;
		}
		ofunc(g_VGuiSurface);

	}
	//--------------------------------------------------------------------------------




	//-------------------------------------------------------------------------------
	void __fastcall hkDrawModelExecute(void* _this, int edx, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		static auto ofunc = mdlrender_hook.get_original<decltype(&hkDrawModelExecute)>(index::DrawModelExecute);

		if (g_MdlRender->IsForcedMaterialOverride() &&
			!strstr(pInfo.pModel->szName, "arms") &&
			!strstr(pInfo.pModel->szName, "weapons/v_")) {
			return ofunc(_this, edx, ctx, state, pInfo, pCustomBoneToWorld);
		}

		if (Miscellaneous::Get().RemoveSleeves(pInfo.pModel->szName) ) // shitcode
			return;

		if (g_MdlRender->IsForcedMaterialOverride())
			return ofunc(_this, edx, ctx, state, pInfo, pCustomBoneToWorld);


		Chams::Get().OnDrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);

		ofunc(_this, edx, ctx, state, pInfo, pCustomBoneToWorld);

		g_MdlRender->ForcedMaterialOverride(nullptr);
	}

	
	
	bool __fastcall hkSvCheatsGetBool(PVOID pConVar, void* edx)
	{
		static auto dwCAM_Think = Utils::PatternScan(GetModuleHandleW(L"client.dll"), "85 C0 75 30 38 86");
		static auto ofunc = sv_cheats.get_original<bool(__thiscall *)(PVOID)>(13);
		if (!ofunc)
			return false;

		if (reinterpret_cast<DWORD>(_ReturnAddress()) == reinterpret_cast<DWORD>(dwCAM_Think))
			return true;
		return ofunc(pConVar);
	}
	using FireEvent = bool(__thiscall*)(IGameEventManager2*, IGameEvent* pEvent);
	bool __stdcall hkFireEvent(IGameEvent* pEvent)
	{
		static auto oFireEvent = gameevents_hook.get_original<FireEvent>(index::FireEvent);

		if (!strcmp(pEvent->GetName(), "player_death") && g_EngineClient->GetPlayerForUserID(pEvent->GetInt("attacker")) == g_EngineClient->GetLocalPlayer()) {
			auto& weapon = g_LocalPlayer->m_hActiveWeapon();
			if (weapon && weapon->IsWeapon()) {
				auto& skin_data = g_Options.m_skins.m_items[weapon->m_Item().m_iItemDefinitionIndex()];
				if (skin_data.enabled && skin_data.stat_trak) {
					skin_data.stat_trak++;
					weapon->m_nFallbackStatTrak() = skin_data.stat_trak;
					weapon->GetClientNetworkable()->PostDataUpdate(0);
					weapon->GetClientNetworkable()->OnDataChanged(0);
				}
			}
			const auto icon_override = g_Options.m_skins.get_icon_override(pEvent->GetString("weapon"));
			if (icon_override) {
				pEvent->SetString("weapon", icon_override);
			}
		}
		return oFireEvent(g_GameEvents, pEvent);
	}
	//--------------------------------------------------------------------------------
	static auto random_sequence(const int low, const int high) -> int
	{
		return rand() % (high - low + 1) + low;
	}
	static auto fix_animation(const char* model, const int sequence) -> int
	{
		enum ESequence
		{
			SEQUENCE_DEFAULT_DRAW = 0,
			SEQUENCE_DEFAULT_IDLE1 = 1,
			SEQUENCE_DEFAULT_IDLE2 = 2,
			SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
			SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
			SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
			SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
			SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
			SEQUENCE_DEFAULT_LOOKAT01 = 12,
			SEQUENCE_BUTTERFLY_DRAW = 0,
			SEQUENCE_BUTTERFLY_DRAW2 = 1,
			SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
			SEQUENCE_BUTTERFLY_LOOKAT03 = 15,
			SEQUENCE_FALCHION_IDLE1 = 1,
			SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
			SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
			SEQUENCE_FALCHION_LOOKAT01 = 12,
			SEQUENCE_FALCHION_LOOKAT02 = 13,
			SEQUENCE_DAGGERS_IDLE1 = 1,
			SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
			SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
			SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
			SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,
			SEQUENCE_BOWIE_IDLE1 = 1,
		};
		if (strstr(model, "models/weapons/v_knife_canis.mdl") || strstr(model, "models/weapons/v_knife_outdoor.mdl") || strstr(model, "models/weapons/v_knife_cord.mdl") || strstr(model, "models/weapons/v_knife_skeleton.mdl")) {
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_DRAW:
				return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
			default:
				return sequence + 1;
			}
		}

		if (strstr(model, "models/weapons/v_knife_butterfly.mdl")) {
			switch (sequence) {
			case SEQUENCE_DEFAULT_DRAW:
				return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
			default:
				return sequence + 1;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_falchion_advanced.mdl")) {
			switch (sequence) {
			case SEQUENCE_DEFAULT_IDLE2:
				return SEQUENCE_FALCHION_IDLE1;
			case SEQUENCE_DEFAULT_HEAVY_MISS1:
				return random_sequence(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				return sequence;
			default:
				return sequence - 1;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_push.mdl")) {
			switch (sequence) {
			case SEQUENCE_DEFAULT_IDLE2:
				return SEQUENCE_DAGGERS_IDLE1;
			case SEQUENCE_DEFAULT_LIGHT_MISS1:
			case SEQUENCE_DEFAULT_LIGHT_MISS2:
				return random_sequence(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
			case SEQUENCE_DEFAULT_HEAVY_MISS1:
				return random_sequence(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
			case SEQUENCE_DEFAULT_HEAVY_HIT1:
			case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
			case SEQUENCE_DEFAULT_LOOKAT01:
				return sequence + 3;
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				return sequence;
			default:
				return sequence + 2;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_survival_bowie.mdl")) {
			switch (sequence) {
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				return sequence;
			case SEQUENCE_DEFAULT_IDLE2:
				return SEQUENCE_BOWIE_IDLE1;
			default:
				return sequence - 1;
			}
		}
		else {
			return sequence;
		}
	}
	void hkRecvProxy(const CRecvProxyData* pData, void* entity, void* output)
	{
		static auto original_fn = sequence_hook->get_original_function();
		const auto local = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
		if (local && local->IsAlive())
		{
			const auto proxy_data = const_cast<CRecvProxyData*>(pData);
			const auto view_model = static_cast<C_BaseViewModel*>(entity);
			if (view_model && view_model->m_hOwner() && view_model->m_hOwner().IsValid())
			{
				const auto owner = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntityFromHandle(view_model->m_hOwner()));
				if (owner == g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()))
				{
					const auto view_model_weapon_handle = view_model->m_hWeapon();
					if (view_model_weapon_handle.IsValid())
					{
						const auto view_model_weapon = static_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(view_model_weapon_handle));
						if (view_model_weapon)
						{
							if (k_weapon_info.count(view_model_weapon->m_Item().m_iItemDefinitionIndex()))
							{
								auto original_sequence = proxy_data->m_Value.m_Int;
								const auto override_model = k_weapon_info.at(view_model_weapon->m_Item().m_iItemDefinitionIndex()).model;
								proxy_data->m_Value.m_Int = fix_animation(override_model, proxy_data->m_Value.m_Int);
							}
						}
					}
				}
			}
		}
		original_fn(pData, entity, output);
	}


	
}
