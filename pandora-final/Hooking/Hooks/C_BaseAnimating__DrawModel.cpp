#include "../../source.hpp"
#include "../Hooked.hpp"
#include "../../SDK/Displacement.hpp"
#include "../../SDK/Classes/Player.hpp"

int __fastcall Hooked::C_BaseAnimating__DrawModel( void* ecx, void* edx, int a1, int a2 ) {
	auto pEntity = ( C_CSPlayer* )ecx;
	if( pEntity ) {
		auto pLocal = C_CSPlayer::GetLocalPlayer( );
		if( pLocal && pEntity->IsTeammate( pLocal ) && pEntity->EntIndex( ) != pLocal->EntIndex( ) ) {
			// https://i.imgur.com/wjElvZB.png
			*( bool* )( ( uintptr_t )ecx + 0x270 ) = !g_Vars.esp.disable_teammate_rendering;

			auto wpn = pEntity->m_hActiveWeapon( ).Get( );
			if( wpn )
				*( bool* )( ( uintptr_t )wpn + 0x270 ) = !g_Vars.esp.disable_teammate_rendering;
		}
	}

	return oC_BaseAnimating__DrawModel( ecx, a1, a2 );
}