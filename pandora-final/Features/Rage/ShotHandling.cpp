#include "ShotHandling.hpp"
#include "Resolver.hpp"
#include "../Visuals/EventLogger.hpp"

ShotHandling g_ShotHandling;

void ShotHandling::ProcessShots( ) {
	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	const auto pWeapon = reinterpret_cast< C_WeaponCSBaseGun* >( pLocal->m_hActiveWeapon( ).Get( ) );
	if( !pWeapon )
		return;

	auto pWeaponData = pWeapon->GetCSWeaponData( );
	if( !pWeaponData.IsValid( ) )
		return;

	if( !m_bGotEvents ) {
		return;
	}

	m_bGotEvents = false;

	if( m_vecFilteredShots.empty( ) ) {
		return;
	}

	auto it = m_vecFilteredShots.begin( );

	for( auto it = m_vecFilteredShots.begin( ); it != m_vecFilteredShots.end( ); ) {
		if( it->m_vecImpacts.empty( ) ) {
			it = m_vecFilteredShots.erase( it );
			continue;
		}

		const bool weHit = it->m_vecDamage.size( );

		// for some reason the m_Shotdata filtering is wrong but everything else seems to be fine
		// gotta look into it lmao
		if( weHit ) {

		}
		// note - michal;
		// we'll have to handle other misses than spread, occlusion and resolver too for 
		// max accuracy log wise (so user knows why he 'missed'), those include misses due to
		// the enemy dying (for example you fired two DT shots, both aimed at chest but due to 
		// spread you hit his head on the first shot, killed him, and second DT shot already
		// fired before we got the info about the player dying, causing in a "miss"), or misses
		// due to death (local, aka clientsides), other misses due to death of enemy (teammate
		// killed him like a tick before), etc.. you get the idea
		else {
			// intersection with resolved matrix
			// when true we missed due to resolver
			const bool bHit = TraceShot( &*it );

			std::stringstream msg{ };

			// handle correction here
			if( bHit ) {
				if( it->m_ShotData.m_bWasLBYFlick )
					g_Resolver.m_arrResolverData.at( it->m_ShotData.m_iTargetIndex ).m_iMissedShotsLBY++;
				else {
					// don't count misses when overriding
					if( !g_Resolver.m_arrResolverData.at( it->m_ShotData.m_iTargetIndex ).m_bIsOverriding )
						g_Resolver.m_arrResolverData.at( it->m_ShotData.m_iTargetIndex ).m_iMissedShots++;
				}
			}

			// handle miss logs here
			const float flPointDistance = it->m_ShotData.m_vecStart.Distance( it->m_ShotData.m_vecEnd );
			const float flImpactDistance = it->m_ShotData.m_vecStart.Distance( it->m_vecImpacts.back( ) );

			// TODO: Improve this/make it more consistent!!!!!!
			if( it->m_ShotData.m_bHadPredError ) {
				//msg << XorStr( "prediction" );
			}
			else {
				if( bHit ) {
					//msg << XorStr( "resolver " );
				}
				// spread/occlusion miss
				else {
					// [0] spread (occlusion)
					if( flPointDistance > flImpactDistance ) {
						//	msg << XorStr( "spread" );
					}
					// [1] spread (inaccuracy)
					else {
						//	msg << XorStr( "spread" );
					}

					g_Resolver.m_arrResolverData.at( it->m_ShotData.m_iTargetIndex ).m_iMissedShotsSpread++;

					if( g_Vars.esp.event_resolver )
						g_EventLog.PushEvent( XorStr( "Missed shot due to spread" ), Color_f( 1.0f, 1.0f, 1.0f ), true );
				}
			}
		}

		it = m_vecFilteredShots.erase( it );
	}
}

void ShotHandling::RegisterShot( ShotInformation_t& shot ) {
	auto& newShot = m_vecShots.emplace_back( );

	newShot.m_ShotData = shot;
}

void ShotHandling::GameEvent( IGameEvent* pEvent ) {
	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	const auto pWeapon = reinterpret_cast< C_WeaponCSBaseGun* >( pLocal->m_hActiveWeapon( ).Get( ) );
	if( !pWeapon )
		return;

	auto pWeaponData = pWeapon->GetCSWeaponData( );
	if( !pWeaponData.IsValid( ) )
		return;

	if( m_vecShots.empty( ) && m_vecFilteredShots.empty( ) )
		return;

	if( !m_vecShots.empty( ) ) {
		for( auto it = m_vecShots.begin( ); it != m_vecShots.end( ); ) {
			if( std::fabsf( it->m_ShotData.m_flTime - m_pGlobalVars->realtime ) >= 2.5f || it->m_ShotData.m_bMatched ) {
				it = m_vecShots.erase( it );
			}
			else {
				it = next( it );
			}
		}
	}

	switch( hash_32_fnv1a( pEvent->GetName( ) ) ) {
	case hash_32_fnv1a_const( "weapon_fire" ): {
		if( m_pEngine->GetPlayerForUserID( pEvent->GetInt( XorStr( "userid" ) ) ) != pLocal->EntIndex( ) || m_vecShots.empty( ) )
			return;

		const auto it = std::find_if( m_vecShots.rbegin( ), m_vecShots.rend( ), [ & ] ( ShotEvents_t it ) -> bool {
			return !it.m_ShotData.m_bMatched;
		} );

		if( !&it || it->m_ShotData.m_bMatched )
			return;

		it->m_ShotData.m_bMatched = true;

		auto& filteredShot = m_vecFilteredShots.emplace_back( *it );

		break;
	}
	case hash_32_fnv1a_const( "player_hurt" ): {
		if( m_vecFilteredShots.empty( ) )
			return;

		auto target = C_CSPlayer::GetPlayerByIndex( m_pEngine->GetPlayerForUserID( pEvent->GetInt( XorStr( "userid" ) ) ) );
		if( !target || target == pLocal || pLocal->IsTeammate( target ) || target->IsDormant( ) )
			return;

		ShotEvents_t::player_hurt_t hurt{ };
		hurt.m_iDamage = pEvent->GetInt( XorStr( "dmg_health" ) );
		hurt.m_iTargetIndex = target->EntIndex( );
		hurt.m_iHitgroup = pEvent->GetInt( XorStr( "hitgroup" ) );

		m_vecFilteredShots.back( ).m_vecDamage.push_back( hurt );

		break;
	}
	case hash_32_fnv1a_const( "bullet_impact" ): {
		if( m_vecFilteredShots.empty( ) || m_pEngine->GetPlayerForUserID( pEvent->GetInt( XorStr( "userid" ) ) ) != pLocal->EntIndex( ) )
			return;

		m_vecFilteredShots.back( ).m_vecImpacts.emplace_back( pEvent->GetFloat( XorStr( "x" ) ), pEvent->GetFloat( XorStr( "y" ) ), pEvent->GetFloat( XorStr( "z" ) ) );

		break;
	}
	}

	m_bGotEvents = true;
}

bool ShotHandling::TraceShot( ShotEvents_t* shot ) {
	if( !shot )
		return false;

	if( !shot->m_ShotData.m_pHitbox || shot->m_ShotData.m_pHitbox == nullptr )
		return false;

	if( !shot->m_ShotData.m_pRecord.player || shot->m_ShotData.m_pRecord.player == nullptr )
		return false;

	const auto pMatrix = shot->m_ShotData.m_pRecord.m_pMatrix;
	if( !pMatrix )
		return false;

	const Vector vecServerImpact = shot->m_vecImpacts.back( );

	const float flRadius = shot->m_ShotData.m_pHitbox->m_flRadius;
	const bool bCapsule = flRadius != -1.f;

	const Vector vecMin = shot->m_ShotData.m_pHitbox->bbmin.Transform( pMatrix[ shot->m_ShotData.m_pHitbox->bone ] );
	const Vector vecMax = shot->m_ShotData.m_pHitbox->bbmax.Transform( pMatrix[ shot->m_ShotData.m_pHitbox->bone ] );

	auto dir = vecServerImpact - shot->m_ShotData.m_vecStart;
	dir.Normalize();

	//m_pDebugOverlay->AddLineOverlay(shot->m_ShotData.m_vecStart, vecServerImpact, 255, 0, 0, true, 4.f);

	const bool bIntersected = bCapsule ? Math::IntersectSegmentToSegment( shot->m_ShotData.m_vecStart, vecServerImpact, vecMin, vecMax, flRadius ) : Math::IntersectionBoundingBox(shot->m_ShotData.m_vecStart, shot->m_ShotData.m_pHitbox, dir, pMatrix[shot->m_ShotData.m_pHitbox->bone]);

	return bIntersected;
}