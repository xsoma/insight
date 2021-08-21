#pragma once
#include "../../../valve_sdk/csgostructs.hpp"
#include "../../../valve_sdk/math/VMatrix.hpp"
#include <optional>
#include <array>
#include <deque>

#include <xmemory>

#define LAG_COMPENSATION_TICKS 32
#define DISABLE_INTERPOLATION  0
#define ENABLE_INTERPOLATION   1

class LagCompensation : public Singleton < LagCompensation > {
private:

	std::array < PlayerLog, 64u > mLogs;
public:
	void Reset() {
		for (auto& log : mLogs) {
			log.Reset();
		}
	}

	void Instance (ClientFrameStage_t stage);

	PlayerLog* GetLog( int index) { return &mLogs.at(index - 1); }

};