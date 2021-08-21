#pragma once

#include <map>
#include <deque>
#include "../../../valve_sdk/csgostructs.hpp"

struct backtrack_data {
	float simTime;
	Vector hitboxPos;
	matrix3x4_t boneMatrix[128];
};


class LegitBacktrack {
public:
	void OnMove(CUserCmd *pCmd);
	std::map<int, std::deque<backtrack_data>> data;
private:
	float correct_time = 0.0f;
	float latency = 0.0f;
	float lerp_time = 0.0f;
};
extern LegitBacktrack g_LegitBacktrack;
