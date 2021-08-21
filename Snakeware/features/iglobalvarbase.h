#pragma once
// by platina aka lnk1181
#include <functional>
class i_global_vars {
public:
	__forceinline void set(float time, int ticks, const std::function<void()>& fn) {
		const auto backup_real_time = m_real_time;
		const auto backup_cur_time = m_cur_time;
		const auto backup_frame_time = m_frame_time;
		const auto backup_absolute_frame_time = m_absolute_frame_time;
		const auto backup_frame_count = m_frame_count;
		const auto backup_tick_count = m_tick_count;
		const auto backup_interpolation_amount = m_interpolation_amount;

		m_real_time = time;
		m_cur_time = time;
		m_frame_time = m_interval_per_tick;
		m_absolute_frame_time = m_interval_per_tick;
		m_frame_count = ticks;
		m_tick_count = ticks;
		m_interpolation_amount = 0.f;

		fn();

		m_real_time = backup_real_time;
		m_cur_time = backup_cur_time;
		m_frame_time = backup_frame_time;
		m_absolute_frame_time = backup_absolute_frame_time;
		m_frame_count = backup_frame_count;
		m_tick_count = backup_tick_count;
		m_interpolation_amount = backup_interpolation_amount;
	}

	float m_real_time;
	int   m_frame_count;
	float m_absolute_frame_time;
	float m_absolute_frame_start_time_std_dev;
	float m_cur_time;
	float m_frame_time;
	int   m_max_clients;
	int   m_tick_count;
	float m_interval_per_tick;
	float m_interpolation_amount;
	int   m_sim_ticks_this_frame;
	int   m_network_protocol;
	void* m_save_data;
	bool  m_client;
	bool  m_remote_client;
};