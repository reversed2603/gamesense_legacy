#include "CCSGameRules.hpp"
#include "../Displacement.hpp"

bool CCSGameRules::m_bIsValveDS( ){
	return *( bool* )( ( uintptr_t )this + 0x75 );
}

bool CCSGameRules::m_bFreezePeriod( ){
	return *( bool* )( ( uintptr_t )this + 0x20 );
}