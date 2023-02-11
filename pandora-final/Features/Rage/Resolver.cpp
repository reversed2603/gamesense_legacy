#include "Resolver.hpp"
#include "../Visuals/EventLogger.hpp"
#include "../Scripting/Scripting.hpp"
#include <sstream>
#include "Autowall.hpp"
#include "AntiAim.hpp"
#include <iomanip>

Resolver g_Resolver;

void Resolver::UpdateLBYPrediction( AnimationRecord* pRecord ) {
	auto& resolverData = m_arrResolverData.at( pRecord->m_pEntity->EntIndex( ) );

	if( !pRecord )
		return;

	pRecord->m_AnimationFlags &= ~ELagRecordFlags::RF_LBYFlicked;

	if( pRecord->m_vecVelocity.Length( ) > 0.1f || ( pRecord->m_fFlags & FL_ONGROUND ) == 0/*&& !( pRecord->m_AnimationFlags & ELagRecordFlags::RF_FakeWalking )*/ ) {
		resolverData.m_flLowerBodyRealignTimer = FLT_MAX;
		resolverData.m_iDetectedBodyUpdate = 0;
		resolverData.m_bInPredictionStage = false;
		return;
	}

	if( resolverData.m_flPreviousLowerBodyYaw != FLT_MAX && fabs( Math::AngleDiff( pRecord->m_flLowerBodyYawTarget, resolverData.m_flPreviousLowerBodyYaw ) ) > 30.f )
	{
		resolverData.m_iDetectedBodyUpdate = 1;
		resolverData.m_flLowerBodyRealignTimer = pRecord->m_flAnimationTime + 1.1f;

		resolverData.m_bInPredictionStage = true;
		pRecord->m_AnimationFlags |= ELagRecordFlags::RF_LBYFlicked;
	}

	resolverData.m_flPreviousLowerBodyYaw = pRecord->m_flLowerBodyYawTarget;

	/*auto layer3 = Encrypted_t(&pRecord->m_pServerAnimOverlays[3]);

	int layer_3_activity = pRecord->m_pEntity->GetSequenceActivity(layer3->m_nSequence);

	if (resolverData.m_iDetectedBodyUpdate != 1
		&& resolverData.m_flLastLbyActivity != 979
		&& layer_3_activity == 979
		&& resolverData.m_flLastLbyActivity != layer_3_activity
		&& layer3->m_flCycle > 0.0f
		&& layer3->m_flWeight > 0.0f)
	{
		resolverData.m_iDetectedBodyUpdate = 2;
		resolverData.m_flLowerBodyRealignTimer = pRecord->m_flAnimationTime - (layer3->m_flCycle / layer3->m_flPlaybackRate);
	}*/

	//resolverData.m_flLastLbyActivity = layer_3_activity;

	//if(resolverData.m_iDetectedBodyUpdate == 0) 
	//{
	//	// we don't have any last move data, perform basic bruteforce
	//	if( resolverData.m_vecLastMoveOrigin.IsZero( ) ) {
	//		pRecord->m_AnimationFlags &= ~ELagRecordFlags::RF_LBYFlicked;
	//		resolverData.ResetPredData( );
	//		return;
	//	}

	//	// last move origin is too far away, the enemy has moved when we haven't seen them(?)
	//	if( ( resolverData.m_vecLastMoveOrigin - pRecord->m_vecOrigin ).Length( ) > 128.f ) {
	//		pRecord->m_AnimationFlags &= ~ELagRecordFlags::RF_LBYFlicked;
	//		resolverData.ResetPredData( );
	//		return;
	//	}
	//}

	if( pRecord->m_flAnimationTime >= resolverData.m_flLowerBodyRealignTimer && resolverData.m_flLowerBodyRealignTimer < FLT_MAX )
	{
		resolverData.m_flLowerBodyRealignTimer = pRecord->m_flAnimationTime + 1.1f;

		resolverData.m_bInPredictionStage = true;
		pRecord->m_AnimationFlags |= ELagRecordFlags::RF_LBYFlicked;
	}

	//// this will be called on first init too, obviously
	//if( resolverData.m_flPreviousLowerBodyYaw != pRecord->m_flLowerBodyYawTarget ) {

	//	// make sure not to call these on first init
	//	if( resolverData.m_flPreviousLowerBodyYaw != FLT_MAX ) {
	//		resolverData.m_flLowerBodyRealignTimer = pRecord->m_flAnimationTime + 1.1f;

	//		pRecord->m_angEyeAngles.y = pRecord->m_flLowerBodyYawTarget;

	//		resolverData.m_bDetectedBodyUpdate = true;
	//	}

	//	resolverData.m_flPreviousLowerBodyYaw = pRecord->m_flLowerBodyYawTarget;
	//}

	//// if we detected a lower body update, allow prediction
	//if( !resolverData.m_bDetectedBodyUpdate ) {
	//	// we don't have any last move data, perform basic bruteforce
	//	if( resolverData.m_vecLastMoveOrigin.IsZero( ) ) {
	//		pRecord->m_AnimationFlags &= ~ELagRecordFlags::RF_LBYFlicked;
	//		resolverData.ResetPredData( );
	//		return;
	//	}

	//	// last move origin is too far away, the enemy has moved when we haven't seen them(?)
	//	if( ( resolverData.m_vecLastMoveOrigin - pRecord->m_vecOrigin ).Length( ) > 128.f ) {
	//		pRecord->m_AnimationFlags &= ~ELagRecordFlags::RF_LBYFlicked;
	//		resolverData.ResetPredData( );
	//		return;
	//	}
	//}

	//// we have valid move data, attempt to predict

	//if( pRecord->m_vecVelocity.Length( ) > 0.1f && !( pRecord->m_AnimationFlags & ELagRecordFlags::RF_FakeWalking ) ) {
	//	resolverData.m_flLowerBodyRealignTimer = pRecord->m_flAnimationTime + 0.22f;
	//	resolverData.m_bInPredictionStage = false;
	//}
	//else if( pRecord->m_flAnimationTime >= resolverData.m_flLowerBodyRealignTimer ) {
	//	resolverData.m_flLowerBodyRealignTimer = pRecord->m_flAnimationTime + 1.1f;

	//	resolverData.m_bInPredictionStage = true;
	//	pRecord->m_AnimationFlags |= ELagRecordFlags::RF_LBYFlicked;
	//}
}

void Resolver::UpdateResolverStage( AnimationRecord* pRecord ) {
	auto& resolverData = m_arrResolverData.at( pRecord->m_pEntity->EntIndex( ) );

	if( ( pRecord->m_fFlags & FL_ONGROUND ) && ( pRecord->m_vecVelocity.Length( ) <= 0.1f || ( pRecord->m_AnimationFlags & ELagRecordFlags::RF_FakeWalking ) ) )
		resolverData.m_eCurrentStage = EResolverStages::RESOLVE_MODE_STAND;

	bool bInPredictionStage = ( pRecord->m_AnimationFlags & ELagRecordFlags::RF_LBYFlicked ) && resolverData.m_iMissedShotsLBY <= 2;
	if( ( pRecord->m_fFlags & FL_ONGROUND ) && ( pRecord->m_vecVelocity.Length( ) > 0.1f && !( pRecord->m_AnimationFlags & ELagRecordFlags::RF_FakeWalking ) ) ) {
		resolverData.m_eCurrentStage = EResolverStages::RESOLVE_MODE_MOVE;
		pRecord->m_AnimationFlags |= ELagRecordFlags::RF_IsResolved;
	}
	else if( !( pRecord->m_fFlags & FL_ONGROUND ) ) {
		resolverData.m_eCurrentStage = EResolverStages::RESOLVE_MODE_AIR;
	}
	else if( bInPredictionStage ) {
		resolverData.m_eCurrentStage = EResolverStages::RESOLVE_MODE_PRED;
		pRecord->m_AnimationFlags |= ELagRecordFlags::RF_IsResolved;
	}

	// allow lby flicks on override
	// also important allow override to override moving players because of the retard
	// low dopium kid dogs shit faggots who use their shitty exploit =D
	if( g_Vars.rage.antiaim_correction_override.enabled && !bInPredictionStage ) {
		resolverData.m_eCurrentStage = EResolverStages::RESOLVE_MODE_OVERRIDE;
		pRecord->m_AnimationFlags &= ~ELagRecordFlags::RF_IsResolved;
	}
}

void Resolver::ResolveBrute( AnimationRecord* pRecord ) {
	auto& resolverData = m_arrResolverData.at( pRecord->m_pEntity->EntIndex( ) );
	auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	// reset move data here, we won't be working with it anyway
	resolverData.ResetMoveData( );

	QAngle angAway;
	pLocal->IsDead( ) ? Vector( 0, 180, 0 ) : Math::VectorAngles( pLocal->m_vecOrigin( ) - pRecord->m_vecOrigin, angAway );
	
	const float flFreestandYaw = ResolveAntiFreestand( pRecord );

	switch( resolverData.m_iMissedShots % 7 ) {
	case 0:
		pRecord->m_angEyeAngles.y = flFreestandYaw != FLT_MAX ? flFreestandYaw : angAway.y + 180.f;
		break;
	case 1:
		pRecord->m_angEyeAngles.y = angAway.y + 180.f;
		break;
	case 2:
		pRecord->m_angEyeAngles.y = pRecord->m_flLowerBodyYawTarget;
		break;
	case 3:
		pRecord->m_angEyeAngles.y = pRecord->m_flLowerBodyYawTarget + 180.f;
		break;
	case 4:
		pRecord->m_angEyeAngles.y = pRecord->m_flLowerBodyYawTarget + 110.f;
		break;
	case 5:
		pRecord->m_angEyeAngles.y = pRecord->m_flLowerBodyYawTarget - 110.f;
		break;
	case 6:
		pRecord->m_angEyeAngles.y = angAway.y;
		break;
	}
}

// todo - michal;
// add some sort of per entity logging, where if the first stand resolver stage (last move lby)
// fails on a target twice or more, then for that target do anti-freestanding for the first shot in the future
void Resolver::ResolveStand( AnimationRecord* pRecord ) {
	auto& resolverData = m_arrResolverData.at( pRecord->m_pEntity->EntIndex( ) );

	// we don't have any last move data, perform basic bruteforce
	if( resolverData.m_vecLastMoveOrigin.IsZero( ) ) {
		return ResolveBrute( pRecord );
	}

	// last move origin is too far away, the enemy has moved when we haven't seen them(?)
	if( ( resolverData.m_vecLastMoveOrigin - pRecord->m_vecOrigin ).Length( ) > 128.f ) {
		return ResolveBrute( pRecord );
	}

	float flAnimTimeDelta = pRecord->m_flAnimationTime - resolverData.m_flLastMoveAnimTime;

	// the enemy still hasn't performed the first 0.22 lby flick
	// until that flick happens, we can force lby
	if( flAnimTimeDelta < 0.22f ) {
		pRecord->m_angEyeAngles.y = resolverData.m_flLastMoveBody;
		return;
	}

	const float flMoveDelta = fabs( resolverData.m_flLastMoveBody - pRecord->m_flLowerBodyYawTarget );
	const float flFreestandYaw = ResolveAntiFreestand( pRecord );

	// we have valid move data, use it
	// note - michal;
	// todo, if miss one shot, and then we hit properly (head or feet) on the anti-freestand
	// stage then take note of this, and the next time anti-freestand for first shot instead of last move lby

	// of course if we then miss the anti-freestand shot, for next shot try shooting and last move lby
	switch( resolverData.m_iMissedShots % 6 ) {
	case 0:
		pRecord->m_angEyeAngles.y = resolverData.m_flLastMoveBody;
		break;
	case 1:
		pRecord->m_angEyeAngles.y = flFreestandYaw != FLT_MAX ? flFreestandYaw : resolverData.m_flLastMoveBody + 180.f;
		break;
	case 2:
		pRecord->m_angEyeAngles.y = resolverData.m_flLastMoveBody - 90.f;
		break;
	case 3:
		pRecord->m_angEyeAngles.y = resolverData.m_flLastMoveBody + 90.f;
		break;
	case 4:
		if( flMoveDelta > 0.f ) {
			pRecord->m_angEyeAngles.y = resolverData.m_flLastMoveBody - flMoveDelta;
		}
		else {
			pRecord->m_angEyeAngles.y = resolverData.m_flLastMoveBody - 115.f;
		}
		break;
	case 5:
		if( flMoveDelta > 0.f ) {
			pRecord->m_angEyeAngles.y = resolverData.m_flLastMoveBody + flMoveDelta;
		}
		else {
			pRecord->m_angEyeAngles.y = resolverData.m_flLastMoveBody + 115.f;
		}
		break;
	}
}

void Resolver::ResolveMove( AnimationRecord* pRecord ) {
	auto& resolverData = m_arrResolverData.at( pRecord->m_pEntity->EntIndex( ) );

	resolverData.m_flLastMoveBody = pRecord->m_flLowerBodyYawTarget;
	resolverData.m_vecLastMoveOrigin = pRecord->m_vecOrigin;
	resolverData.m_flLastMoveAnimTime = pRecord->m_flAnimationTime;

	// we saw him move again, let's repredict and allow shooting at lby flicks!
	if( pRecord->m_vecVelocity.Length2D( ) >= 25.f )
		resolverData.m_iMissedShotsLBY = 0;

	pRecord->m_angEyeAngles.y = pRecord->m_flLowerBodyYawTarget;
}

void Resolver::ResolveAir( AnimationRecord* pRecord ) {
	auto& resolverData = m_arrResolverData.at( pRecord->m_pEntity->EntIndex( ) );

	const float flVelocityDirYaw = RAD2DEG( std::atan2( pRecord->m_vecVelocity.x, pRecord->m_vecVelocity.y ) );

	auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	QAngle angAway;
	pLocal->IsDead( ) ? Vector( 0, 180, 0 ) : Math::VectorAngles( pLocal->m_vecOrigin( ) - pRecord->m_vecOrigin, angAway );

	switch( resolverData.m_iMissedShots % 4 ) {
	case 0:
		pRecord->m_angEyeAngles.y = pRecord->m_flLowerBodyYawTarget;
		break;
	case 1:
		if( resolverData.m_flLastMoveBody < FLT_MAX && abs( Math::AngleDiff( pRecord->m_flLowerBodyYawTarget, resolverData.m_flLastMoveBody ) ) > 60.f )
			pRecord->m_angEyeAngles.y = resolverData.m_flLastMoveBody;
		else
			pRecord->m_angEyeAngles.y = angAway.y + 180.f;
		break;
	case 2:
		pRecord->m_angEyeAngles.y = angAway.y;
		break;
	case 3:
		pRecord->m_angEyeAngles.y = flVelocityDirYaw - 180.f;
		break;
	}
}

void Resolver::ResolvePred( AnimationRecord* pRecord ) {
	auto& resolverData = m_arrResolverData.at( pRecord->m_pEntity->EntIndex( ) );

	// we've mispredicted 3 times, stop predicting
	if( resolverData.m_iMissedShotsLBY > 2 ) {
		return;
	}

	pRecord->m_angEyeAngles.y = pRecord->m_flLowerBodyYawTarget;
}

// FATALITY XDDDDDD
void Resolver::ResolveOverride( AnimationRecord* pRecord ) {
	auto& resolverData = m_arrResolverData.at( pRecord->m_pEntity->EntIndex( ) );
	resolverData.m_bIsOverriding = false;

	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	if( !g_Vars.rage.antiaim_resolver_override )
		return;

	static std::vector<C_CSPlayer*> targets;

	static auto weapon_recoil_scale = m_pCvar->FindVar( XorStr( "weapon_recoil_scale" ) );
	static auto last_checked = 0.f;

	QAngle viewangles;
	m_pEngine->GetViewAngles( viewangles );

	if( last_checked != m_pGlobalVars->curtime ) {
		last_checked = m_pGlobalVars->curtime;
		targets.clear( );

		const auto needed_fov = 20.f;
		for( auto i = 1; i <= m_pGlobalVars->maxClients; i++ ) {
			auto ent = ( C_CSPlayer* )m_pEntList->GetClientEntity( i );
			if( !ent || ent->IsDead( ) || ent->IsDormant( ) )
				continue;

			const auto fov = Math::GetFov( viewangles + pLocal->m_aimPunchAngle( ) * weapon_recoil_scale->GetFloat( ), pLocal->GetEyePosition( ), ent->GetEyePosition( ) );
			if( fov < needed_fov ) {
				targets.push_back( ent );
			}
		}
	}

	bool had_target = false;
	if( targets.empty( ) ) {
		had_target = false;
		return;
	}

	auto found = false;
	for( auto& target : targets ) {
		if( pRecord->m_pEntity == target ) {
			found = true;
			break;
		}
	}

	if( !found )
		return;

	static auto last_delta = 0.f;
	static auto last_angle = 0.f;

	//Vector angAway;
	//pLocal->IsDead( ) ? Vector( 0, 180, 0 ) : Math::VectorAngles( pLocal->m_vecOrigin( ) - pRecord->m_vecOrigin, angAway );

	const float at_target_yaw = Math::CalcAngle( pLocal->m_vecOrigin( ), pRecord->m_vecOrigin ).y;

	auto delta = Math::AngleNormalize( viewangles.y - at_target_yaw );

	if( had_target && fabsf( viewangles.y - last_angle ) < 0.1f ) {
		viewangles.y = last_angle;
		delta = last_delta;
	}

	had_target = true;

	//	g_EventLog.PushEvent( std::to_string( delta ), Color_f( 1.f, 1.f, 1.f, 1.f ), true );

	if( delta >= 4.0f )
		pRecord->m_angEyeAngles.y = Math::AngleNormalize( at_target_yaw + 90.f );
	else if( delta <= -4.0f )
		pRecord->m_angEyeAngles.y = Math::AngleNormalize( at_target_yaw - 90.f );
	else
		pRecord->m_angEyeAngles.y = Math::AngleNormalize( at_target_yaw );

	resolverData.m_bIsOverriding = true;

	last_angle = viewangles.y;
	last_delta = delta;
}

float Resolver::ResolveAntiFreestand( AnimationRecord* pRecord ) {
	auto& resolverData = m_arrResolverData.at( pRecord->m_pEntity->EntIndex( ) );

	auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return FLT_MAX;

	constexpr float STEP{ 4.f };
	constexpr float RANGE{ 32.f };

	QAngle angAway;
	pLocal->IsDead( ) ? Vector( 0, 180, 0 ) : Math::VectorAngles( pLocal->m_vecOrigin( ) - pRecord->m_vecOrigin, angAway );

	auto enemy_eyepos = pRecord->m_pEntity->GetEyePosition( );

	// construct vector of angles to test.
	std::vector< AdaptiveAngle > angles{ };
	angles.emplace_back( angAway.y - 180.f );
	angles.emplace_back( angAway.y + 90.f );
	angles.emplace_back( angAway.y - 90.f );

	// start the trace at the enemy shoot pos.
	auto start = pLocal->GetEyePosition( );

	// see if we got any valid result.
	// if this is false the path was not obstructed with anything.
	bool valid{ false };

	// iterate vector of angles.
	for( auto it = angles.begin( ); it != angles.end( ); ++it ) {

		// compute the 'rough' estimation of where our head will be.
		Vector end{ enemy_eyepos.x + std::cos( DEG2RAD( it->m_yaw ) ) * RANGE,
			enemy_eyepos.y + std::sin( DEG2RAD( it->m_yaw ) ) * RANGE,
			enemy_eyepos.z };

		// draw a line for debugging purposes.
		//g_csgo.m_debug_overlay->AddLineOverlay( start, end, 255, 0, 0, true, 0.1f );

		// compute the direction.
		Vector dir = end - start;
		float len = dir.Normalize( );

		// should never happen.
		if( len <= 0.f )
			continue;

		// step thru the total distance, 4 units per step.
		for( float i{ 0.f }; i < len; i += STEP ) {
			// get the current step position.
			Vector point = start + ( dir * i );

			// get the contents at this point.
			int contents = m_pEngineTrace->GetPointContents( point, MASK_SHOT_HULL );

			// contains nothing that can stop a bullet.
			if( !( contents & MASK_SHOT_HULL ) )
				continue;

			float mult = 1.f;

			// over 50% of the total length, prioritize this shit.
			if( i > ( len * 0.5f ) )
				mult = 1.25f;

			// over 90% of the total length, prioritize this shit.
			if( i > ( len * 0.75f ) )
				mult = 1.25f;

			// over 90% of the total length, prioritize this shit.
			if( i > ( len * 0.9f ) )
				mult = 2.f;

			// append 'penetrated distance'.
			it->m_dist += ( STEP * mult );

			// mark that we found anything.
			valid = true;
		}
	}

	if( !valid ) {
		return FLT_MAX;
	}

	// put the most distance at the front of the container.
	std::sort( angles.begin( ), angles.end( ),
		[ ] ( const AdaptiveAngle& a, const AdaptiveAngle& b ) {
		return a.m_dist > b.m_dist;
	} );

	// the best angle should be at the front now.
	AdaptiveAngle* best = &angles.front( );

	return Math::AngleNormalize( best->m_yaw );
}

void Resolver::ResolvePlayers( AnimationRecord* pRecord ) {
	if( !pRecord )
		return;

	auto& resolverData = m_arrResolverData.at( pRecord->m_pEntity->EntIndex( ) );

	player_info_t info;
	m_pEngine->GetPlayerInfo( pRecord->m_pEntity->EntIndex( ), &info );
	if( !g_Vars.rage.antiaim_correction )
		return;

	if( !g_Vars.rage.antiaim_resolver )
		return;

	if( info.fakeplayer ) {
		pRecord->m_AnimationFlags |= ELagRecordFlags::RF_IsResolved;
		return;
	}

	// handle lby prediction
	UpdateLBYPrediction( pRecord );

	// handle resolver stages
	UpdateResolverStage( pRecord );

	switch( resolverData.m_eCurrentStage ) {
	case EResolverStages::RESOLVE_MODE_STAND:
		ResolveStand( pRecord );
		break;
	case EResolverStages::RESOLVE_MODE_MOVE:
		ResolveMove( pRecord );
		break;
	case EResolverStages::RESOLVE_MODE_AIR:
		ResolveAir( pRecord );
		break;
	case EResolverStages::RESOLVE_MODE_PRED:
		ResolvePred( pRecord );
		break;
	case EResolverStages::RESOLVE_MODE_OVERRIDE:
		ResolveOverride( pRecord );
		break;
	}

	// resolve player
	pRecord->m_pEntity->m_angEyeAngles( ).y = pRecord->m_angEyeAngles.y;
}