#pragma once
#include <string>

class solution : public Singleton<solution>
{
public:
	LPCSTR Module = "client.dll";
	const char* username = "username";
	const char* sub_days = "30";
	std::string viewmodel_fov = "viewmodel_fov";
	std::string mat_ambient_light_r = "mat_ambient_light_r";
	std::string mat_ambient_light_g = "mat_ambient_light_g";
	std::string mat_ambient_light_b = "mat_ambient_light_b";
	std::string crosshair = "crosshair";
	std::string Retutn2SetupVelocity = "84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80";
	std::string Return2AccumLayers = "84 C0 75 0D F6 87";
	std::string IsLoadout = "84 C0 75 04 B0 01 5F";
	std::string enginedll = "engine.dll";
	std::string WriteUsercmdDeltaToBufferReturn = "84 C0 74 04 B0 01 EB 02 32 C0 8B FE 46 3B F3 7E C9 84 C0 0F 84";
	std::string sendmovecall = "84 C0 74 04 B0 01 EB 02 32 C0 8B FE 46 3B F3 7E C9 84 C0 0F 84 ? ? ? ?";
	std::string CL_SendMoveF = "55 8B EC A1 ? ? ? ? 81 EC ? ? ? ? B9 ? ? ? ? 53 8B 98";
	std::string WriteUsercmdF = "55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D";
};