#include "../../source.hpp"
#include "../Hooked.hpp"
#include "../../SDK/Displacement.hpp"
#include "../../SDK/Classes/Player.hpp"
#include "../../Features/Rage/EnginePrediction.hpp"

void __fastcall Hooked::PhysicsSimulate( void* ecx, void* edx ) {
	auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !ecx || !pLocal || pLocal->IsDead( ) || pLocal != ecx )
		return oPhysicsSimulate( ecx );

	int nSimulationTick = *( int* )( uintptr_t( ecx ) + 0x2AC );
	auto pCommandContext = ( C_CommandContext* )( uintptr_t( ecx ) + 0x34FC );

	if( !pCommandContext || m_pGlobalVars->tickcount == nSimulationTick || !pCommandContext->needsprocessing ) {
		return oPhysicsSimulate( ecx );
	}

	//g_Prediction.RestoreNetvarCompression( &pCommandContext->cmd );
	oPhysicsSimulate( ecx );
	//g_Prediction.StoreNetvarCompression( &pCommandContext->cmd );
}
