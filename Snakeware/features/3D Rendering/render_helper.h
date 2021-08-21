#pragma once
#include "../../singleton.hpp"

class RENDER_3D_GUI : public Singleton <RENDER_3D_GUI>
{
public:
	void LocalCircle();
	void DrawMolotov();
	void AutowallCrosshair();
	void SpreadCircle();

};