#pragma once

#include "../../valve_sdk/csgostructs.hpp"


class Fakelag : public Singleton < Fakelag > {

   public:

	   void  OnCreateMove (CUserCmd * pCmd);

	   void  ForceChoke   (int iForceChoked = 0);

	   void  FakelagTriggererd ();

	   int   iChoke, iChoked, iLlamaTick , iRandomTick = 0 ;

	   float flSpeed, flSpeed2D = 0.f;


	   // Fakelag-trigger's

	   bool  bStandTrigger, bMoveTrigger, bHighMoveTrigger, bAirTrigger, bSlowWalkTrigger;


};

