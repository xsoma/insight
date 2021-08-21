#include "netvar-comp.h"


NetData g_NetData{};;

void NetData::Store() {
	int          tickbase;
	StoredData_t *data;

	if (!g_LocalPlayer && !g_LocalPlayer->IsAlive()) {
		Reset();
		return;
	}

	tickbase = g_LocalPlayer->m_nTickBase();

	// get current record and store data.
	data = &m_data[tickbase % MULTIPLAYER_BACKUP];

	data->m_tickbase = tickbase;
	data->m_punch = g_LocalPlayer->m_aimPunchAngle();
	data->m_punch_vel = g_LocalPlayer->m_aimPunchAngleVel();
	data->m_view_offset = g_LocalPlayer->m_vecViewOffset();
	data->m_velocity_modifier = g_LocalPlayer->m_flVelocityModifier();
}

void NetData::Apply() {
	int          tickbase;
	StoredData_t *data;
	QAngle        punch_delta, punch_vel_delta;
	Vector       view_delta;
	float        modifier_delta;

	if (!g_LocalPlayer && !g_LocalPlayer->IsAlive()) {
		Reset();
		return;
	}

	tickbase = g_LocalPlayer->m_nTickBase();

	// get current record and validate.
	data = &m_data[tickbase % MULTIPLAYER_BACKUP];

	if (g_LocalPlayer->m_nTickBase() != data->m_tickbase)
		return;

	// get deltas.
	// note - dex;  before, when you stop shooting, punch values would sit around 0.03125 and then goto 0 next update.
	//              with this fix applied, values slowly decay under 0.03125.
	punch_delta = g_LocalPlayer->m_aimPunchAngle() - data->m_punch;
	punch_vel_delta = g_LocalPlayer->m_aimPunchAngleVel() - data->m_punch_vel;
	view_delta = g_LocalPlayer->m_vecViewOffset() - data->m_view_offset;
	modifier_delta = g_LocalPlayer->m_flVelocityModifier() - data->m_velocity_modifier;

	// set data.
	if (std::abs(punch_delta.pitch) < 0.03125f &&
		std::abs(punch_delta.yaw) < 0.03125f &&
		std::abs(punch_delta.roll) < 0.03125f)
		g_LocalPlayer->m_aimPunchAngle() = data->m_punch;

	if (std::abs(punch_vel_delta.pitch) < 0.03125f &&
		std::abs(punch_vel_delta.yaw) < 0.03125f &&
		std::abs(punch_vel_delta.roll) < 0.03125f)
		g_LocalPlayer->m_aimPunchAngleVel() = data->m_punch_vel;

	if (std::abs(view_delta.x) < 0.03125f &&
		std::abs(view_delta.y) < 0.03125f &&
		std::abs(view_delta.z) < 0.03125f)
		g_LocalPlayer->m_vecViewOffset() = data->m_view_offset;

	if (std::abs(modifier_delta) < 0.03125f)
		g_LocalPlayer->m_flVelocityModifier() = data->m_velocity_modifier;
}

void NetData::Reset() {
	m_data.fill(StoredData_t());
}