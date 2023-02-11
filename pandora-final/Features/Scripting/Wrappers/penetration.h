#pragma once
#include "../../../SDK/variables.hpp"

namespace Wrappers::Penetration { 
	bool AimingAtPlayer( ) {
		return false;//return g_Vars.globals.m_bAimAtEnemyThruWallOrVisibleLoool;
	}	
	
	int GetPenDamage( ) {
		return 0;// return g_Vars.globals.m_nPenetrationDmg;
	}
}