#pragma once
#include "singleton.hpp"

class Achievments : public Singleton <Achievments>
{
	public:
		void Create_message(const char text, int time, int duration);
};