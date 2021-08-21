#pragma once
#include <map>
#include <deque>
#include <vector>
#include "../../valve_sdk/csgostructs.hpp"
#include "../../options.hpp"
#include "../../render.hpp"


class EventLogs : public IGameEventListener2, public Singleton<EventLogs> {
public:
	void Draw();
	void FireGameEvent(IGameEvent * event);
	void Add(std::string text, Color color);
	int  GetEventDebugID(void);
	void RegisterSelf();
	void UnregisterSelf();

private:
	struct loginfo_t {
		loginfo_t(const float log_time, std::string message, const Color color) {
			this->log_time = log_time;
			this->message = message;
			this->color = color;

			this->x = 6.f;
			this->y = 0.f;
		}

		float log_time;
		std::string message;
		Color color;
		float x, y;
	};

	std::deque< loginfo_t > logs;
};