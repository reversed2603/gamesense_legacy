#include "../../source.hpp"
#include "../Hooked.hpp"
#include "../../SDK/Displacement.hpp"
#include "../../SDK/Classes/Player.hpp"

void __fastcall Hooked::StandardBlendingRules( C_CSPlayer* pPlayer, uint32_t, CStudioHdr* hdr, Vector* pos, Quaternion* q, float currentTime, int boneMask ) {
	g_Vars.globals.szLastHookCalled = XorStr( "1337" );

	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pPlayer || !pLocal || !pPlayer->IsPlayer( ) || pPlayer->EntIndex( ) == pLocal->EntIndex( ) ) {
		return oStandardBlendingRules( pPlayer, hdr, pos, q, currentTime, boneMask );;
	}

	if( !( pPlayer->m_fEffects( ) & EF_NOINTERP ) )
		pPlayer->m_fEffects( ) |= EF_NOINTERP;

	oStandardBlendingRules( pPlayer, hdr, pos, q, currentTime, boneMask );

	if( pPlayer->m_fEffects( ) & EF_NOINTERP )
		pPlayer->m_fEffects( ) &= ~EF_NOINTERP;
}