#include "Glow.hpp"
#include "Chams.hpp"
#include <fstream>
#include "Visuals.hpp"

Glow g_Glow;

void Glow::OnPostScreenEffects( ) {
	if( !m_pEngine->IsInGame( ) )
		return;

	auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	for( int i = 0; i < m_pGlowObjManager->m_GlowObjectDefinitions.m_Size; ++i ) {
		auto& object = m_pGlowObjManager->m_GlowObjectDefinitions[ i ];
		if( object.IsUnused( ) )
			continue;

		auto* pEntity = reinterpret_cast< C_CSPlayer* >( object.m_pEntity );
		if( !pEntity )
			continue;

		switch( pEntity->GetClientClass( )->m_ClassID ) {
		case CBaseCSGrenadeProjectile:
		case CMolotovProjectile:
		case CSmokeGrenadeProjectile:
		case CDecoyProjectile:
		case CInferno:
			if( g_Vars.esp.grenades ) {
				object.m_vGlowColor =
					Vector4D(
						g_Vars.esp.grenades_color.r,
						g_Vars.esp.grenades_color.g,
						g_Vars.esp.grenades_color.b,
						g_Vars.esp.grenades_color.a * 0.7 );

				object.m_bRenderWhenOccluded = true;
				object.m_bRenderWhenUnoccluded = false;
			}
			break;
		case CCSPlayer:
			if( g_Vars.visuals_enemy.glow ) {
				const bool bIsLocalPlayer = pEntity->EntIndex( ) == m_pEngine->GetLocalPlayer( );
				const bool bIsTeammate = pEntity->IsTeammate( pLocal, false ) && !g_Vars.visuals_enemy.teammates;
				const bool bIsEnemy = !bIsTeammate && !bIsLocalPlayer;

				if( bIsEnemy ) {
					object.m_vGlowColor =
						Vector4D( 
							g_Vars.visuals_enemy.glow_color.r,
							g_Vars.visuals_enemy.glow_color.g,
							g_Vars.visuals_enemy.glow_color.b,
							g_Vars.visuals_enemy.glow_color.a );

					object.m_bRenderWhenOccluded = true;
					object.m_bRenderWhenUnoccluded = false;
				}
			}
			break;
		}
	}
}
