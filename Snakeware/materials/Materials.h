#pragma once
#include "../singleton.hpp"

#include <iostream>
#include <fstream>

class Materials : public Singleton< Materials > {
public:
	void Setup();
	void Remove();
};