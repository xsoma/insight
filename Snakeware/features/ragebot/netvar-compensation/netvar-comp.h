#pragma once
#include "../../../valve_sdk/csgostructs.hpp"

class NetData {

private:
	class StoredData_t {
	public:
		int    m_tickbase;
		QAngle  m_punch;
		QAngle  m_punch_vel;
		Vector m_view_offset;
		float  m_velocity_modifier;

	public:
		__forceinline StoredData_t() : m_tickbase{ }, m_punch{ }, m_punch_vel{ }, m_view_offset{ }, m_velocity_modifier{ } {};
	};

	std::array< StoredData_t, MULTIPLAYER_BACKUP > m_data;

public:
	void Store();
	void Apply();
	void Reset();

};

extern NetData g_NetData;