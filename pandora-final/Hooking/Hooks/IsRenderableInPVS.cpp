#include "../../source.hpp"
#include "../Hooked.hpp"
#include "../../SDK/Displacement.hpp"
#include "../../SDK/Classes/Player.hpp"


bool __fastcall Hooked::IsRenderableInPVS( void* ecx, void* edx, IClientRenderable* pRenderable ) {
	g_Vars.globals.szLastHookCalled = XorStr( "6969" );

	if( !ecx || !pRenderable )
		return oIsRenderableInPvs( ecx, pRenderable );

	auto player = reinterpret_cast< C_CSPlayer* >( reinterpret_cast< uintptr_t >( pRenderable ) - 0x4 );

	if( player ) {
		C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer( );
		if( pLocal ) {
			if( !player->IsTeammate( pLocal ) )
				return true;
		}
	}

	return oIsRenderableInPvs( ecx, pRenderable );
}