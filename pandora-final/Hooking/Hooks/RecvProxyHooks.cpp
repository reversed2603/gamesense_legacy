#include "../Hooked.hpp"
#include "../../SDK/Displacement.hpp"
#include "../../SDK/sdk.hpp"
#include "../../SDK/Classes/Player.hpp"
#include "../../SDK/Valve/CBaseHandle.hpp"
#include "../../Features/Rage/LagCompensation.hpp"

namespace Hooked
{
	void m_nSmokeEffectTickBegin( const CRecvProxyData* pData, void* pStruct, void* pOut ) {
		g_Vars.globals.szLastHookCalled = XorStr( "28" );

		if( !pData || !pStruct || !pOut )
			return;

		m_pDidSmokeEffectSwap->GetOriginalFunction( )( pData, pStruct, pOut );
		if( g_Vars.esp.remove_smoke ) {
			// ref: "explosion_smokegrenade"
			*( uint8_t* )( ( uintptr_t )pStruct + Engine::Displacement.DT_SmokeGrenadeProjectile.m_SmokeParticlesSpawned ) = 1;
		}
	}

	void m_bNightVisionOn( const CRecvProxyData* pData, void* pStruct, void* pOut ) {
		if( !pData || !pStruct || !pOut )
			return;

		auto player = ( C_CSPlayer* )pStruct;
		if( player ) {
			if( player->EntIndex( ) == m_pEngine->GetLocalPlayer( ) ) {
				auto pDataFuckConst = ( CRecvProxyData* )pData; // ok fuck you const
				pDataFuckConst->m_Value.m_Int = 1;

				m_bNightVisionOnSwap->GetOriginalFunction( )( pDataFuckConst, pStruct, pOut );
			}
		}
	}

	void m_bClientSideAnimation( const CRecvProxyData* pData, void* pStruct, void* pOut ) {
		auto local = C_CSPlayer::GetLocalPlayer( );
		if( !local || local->IsDead( ) )
			return m_bClientSideAnimationSwap->GetOriginalFunction( )( pData, pStruct, pOut );

		auto player = ( C_CSPlayer* )pStruct;

		if( player && player->IsPlayer( ) && !player->IsTeammate( local ) )
			*( int* )pOut = ( g_Vars.globals.m_bUpdatingAnimations ? 1 : 0 );
	}

	void m_flVelocityModifier( const CRecvProxyData* pData, void* pStruct, void* pOut ) {
		m_flVelocityModifierSwap->GetOriginalFunction( )( pData, pStruct, pOut );

		if( pStruct ) {
			auto ent = ( C_BasePlayer* )( ( C_BasePlayer* )pStruct )->GetBaseEntity( );

			if( ent->EntIndex( ) == m_pEngine->GetLocalPlayer( ) ) {
				/*if( pData->m_Value.m_Float < g_Vars.globals.LastVelocityModifier ) {
					g_Vars.globals.LastVelocityModifier = pData->m_Value.m_Float;
					*( uint8_t* )( uintptr_t( m_pPrediction.Xor( ) ) + 0x24 ) = 1; // m_bPreviousAckHadErrors 
				}*/
			}
		}
	}
}
