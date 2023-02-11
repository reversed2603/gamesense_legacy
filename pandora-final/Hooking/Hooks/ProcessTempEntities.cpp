#include "../../source.hpp"
#include "../Hooked.hpp"
#include "../../SDK/Displacement.hpp"

bool __fastcall Hooked::ProcessTempEntities( void* ecx, void* edx, void* msg )
{
	auto backup = m_pClientState->m_nMaxClients( );

	m_pClientState->m_nMaxClients( ) = 1;
	bool ret = oProcessTempEntities( ecx, msg );
	m_pClientState->m_nMaxClients( ) = backup;

	Hooked::CL_FireEvents( );

	return ret;
}
