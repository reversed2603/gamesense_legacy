#include "Hitmarker.hpp"

#include "../../SDK/Classes/Player.hpp"

Hitmarker g_Hitmarker;

void Hitmarker::Draw( ) {
	if( !g_Vars.esp.visualize_hitmarker_world && !g_Vars.esp.vizualize_hitmarker ) {
		m_vecHitmarkers.clear( );
		return;
	}

	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	auto RenderHitmarker = [ & ] ( Vector2D vecCenter, const int nPaddingFromCenter, const int nSize, const int nWidth, Color color ) {
		for( int i = 0; i < nSize; ++i ) // top left
			Render::Engine::RectFilled( ( vecCenter - nPaddingFromCenter ) - i, Vector2D( nWidth, nWidth ), color );

		for( int i = 0; i < nSize; ++i ) // bottom left
			Render::Engine::RectFilled( vecCenter - Vector2D( nPaddingFromCenter, -nPaddingFromCenter ) - Vector2D( i, -i ), Vector2D( nWidth, nWidth ), color );

		for( int i = 0; i < nSize; ++i ) // top right
			Render::Engine::RectFilled( vecCenter + Vector2D( nPaddingFromCenter, -nPaddingFromCenter ) + Vector2D( i, -i ), Vector2D( nWidth, nWidth ), color );

		for( int i = 0; i < nSize; ++i ) // bottom right
			Render::Engine::RectFilled( ( vecCenter + nPaddingFromCenter ) + i, Vector2D( nWidth, nWidth ), color );
	};

	const Vector2D vCenter = Render::GetScreenSize( ) * 0.5f;

	// i'm sorry i had to, it was nice asf (+ i showcased it to p3x and others and they all said 
	// that the old screen hitmarker was much nicer and unique :P)
	if( g_Vars.esp.vizualize_hitmarker ) {
		static Vector2D vDrawCenter = vCenter;
		if( m_flMarkerAlpha == 255.f ) {
			m_flRandomRotation = 0.f;
			m_flRandomEnlargement = 0.f;
			vDrawCenter = vCenter;
			m_bFirstMarker = false;
		}

		constexpr float flFadeFactor = 1.0f / 0.2f;
		float flFadeIncrement = ( flFadeFactor * m_pGlobalVars->frametime );
		m_flMarkerAlpha -= flFadeFactor;
		m_flRandomEnlargement -= m_flMarkerAlpha / 1020;

		if( m_flMarkerAlpha <= 0 ) {
			m_bFirstMarker = true;
			m_flMarkerAlpha = 0;
		}
		else {
			RenderHitmarker( vDrawCenter, 6, 6, 1, Color::White( ).OverrideAlpha( m_flMarkerAlpha ) );
		}
	}

}

void Hitmarker::GameEvent( IGameEvent* pEvent ) {
	if( !g_Vars.esp.visualize_hitmarker_world && !g_Vars.esp.vizualize_hitmarker )
		return;

	if( !pEvent )
		return;

	auto eventHash = hash_32_fnv1a( pEvent->GetName( ) );
	if( eventHash == hash_32_fnv1a_const( "player_hurt" ) ) {
		PlayerHurt( pEvent );
	}
	else if( eventHash == hash_32_fnv1a_const( "bullet_impact" ) ) {
		BulletImpact( pEvent );
	}
}

void Hitmarker::PlayerHurt( IGameEvent* pEvent ) {
	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	int attacker = m_pEngine->GetPlayerForUserID( pEvent->GetInt( XorStr( "attacker" ) ) );
	int player = m_pEngine->GetPlayerForUserID( pEvent->GetInt( XorStr( "userid" ) ) );

	if( attacker != m_pEngine->GetLocalPlayer( ) || player == m_pEngine->GetLocalPlayer( ) )
		return;

	const auto pPlayer = C_CSPlayer::GetPlayerByIndex( player );
	const auto pAttacker = C_CSPlayer::GetPlayerByIndex( attacker );

	if( !pPlayer || !pAttacker )
		return;

	float bestDistance = FLT_MAX;
	impact_t bestImpact{ };
	for( auto it = m_vecImpacts.begin( ); it != m_vecImpacts.end( ); ) {
		if( m_pGlobalVars->curtime > it->time + 1.f ) {
			it = m_vecImpacts.erase( it );
			continue;
		}

		Vector pos = Vector( it->x, it->y, it->z );
		float distance = pos.Distance( pPlayer->GetAbsOrigin( ) );

		if( distance < bestDistance ) {
			bestDistance = distance;
			bestImpact = *it;

			it = m_vecImpacts.erase( it );
		}
		else {
			it++;
		}
	}

	hitmarker_t hitmarker{ };
	hitmarker.distance = bestDistance;
	if( hitmarker.distance != FLT_MAX )
		hitmarker.impact = bestImpact;

	hitmarker.time = m_pGlobalVars->realtime;
	hitmarker.alpha = 1.f;
	hitmarker.damage = pEvent->GetInt( XorStr( "dmg_health" ) );

	m_uMarkerColor = pEvent->GetInt( XorStr( "hitgroup" ) ) == Hitgroup_Head ? Color( 0, 150, 255 ) : Color( 255, 255, 255 );
	m_flMarkerAlpha = 255.f;

	m_vecHitmarkers.push_back( hitmarker );
}

void Hitmarker::BulletImpact( IGameEvent* pEvent ) {
	int index = m_pEngine->GetPlayerForUserID( pEvent->GetInt( XorStr( "userid" ) ) );
	if( !index )
		return;

	if( index != m_pEngine->GetLocalPlayer( ) )
		return;

	impact_t impact{ };
	impact.x = pEvent->GetFloat( XorStr( "x" ) );
	impact.y = pEvent->GetFloat( XorStr( "y" ) );
	impact.z = pEvent->GetFloat( XorStr( "z" ) );
	impact.time = m_pGlobalVars->curtime;

	m_vecImpacts.push_back( impact );
}
