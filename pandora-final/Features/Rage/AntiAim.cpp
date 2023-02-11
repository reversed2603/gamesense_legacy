#include "AntiAim.hpp"

#include "../Miscellaneous/Movement.hpp"
#include "../Rage/FakeLag.hpp"
#include "../Visuals/Visuals.hpp"
#include "ServerAnimations.hpp"
#include "EnginePrediction.hpp"

#include "../../SDK/Classes/Player.hpp"
#include "../../SDK/Classes/weapon.hpp"

AntiAim g_AntiAim;

bool AntiAim::ShouldAntiAim( CUserCmd* pCmd ) {
	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return false;

	auto pWeapon = reinterpret_cast< C_WeaponCSBaseGun* >( pLocal->m_hActiveWeapon( ).Get( ) );
	if( !pWeapon )
		return false;

	auto pWeaponData = pWeapon->GetCSWeaponData( );
	if( !pWeaponData.IsValid( ) )
		return false;

	// don't do anti-aim while in freezeperiod
	if( m_pGameRules.Xor( ) )
		if( m_pGameRules->m_bFreezePeriod( ) )
			return false;

	// handle attacks...
	if( pWeaponData->m_iWeaponType == WEAPONTYPE_GRENADE ) {
		if( !pWeapon->m_bPinPulled( ) || ( pCmd->buttons & ( IN_ATTACK | IN_ATTACK2 ) ) ) {
			float m_fThrowTime = pWeapon->m_fThrowTime( );
			if( m_fThrowTime > 0.f ) {
				return false;
			}
		}
	}
	else {
		if( ( ( pCmd->buttons & IN_ATTACK ) || ( pWeaponData->m_iWeaponType == WEAPONTYPE_KNIFE && pCmd->buttons & IN_ATTACK2 ) ) && pLocal->CanShoot( ) ) {
			return false;
		}
	}

	// don't do anti-aim when we're holding our use key
	if( pCmd->buttons & IN_USE )
		return false;

	// don't do anti-aim on ladders or while noclipping
	if( pLocal->m_MoveType( ) == MOVETYPE_LADDER || pLocal->m_MoveType( ) == MOVETYPE_NOCLIP )
		return false;

	// frog champ everything went well
	return true;
}

void AntiAim::Prepare( bool* bSendPacket, CUserCmd* pCmd ) {
	// so basically, because of the call order
	// we gotta reset this here..
	// to make fakeduck etc. properly work
	// I know it's not the cleanest
	// but I don't care to be honest.
	if( !g_Vars.globals.m_bFakeWalking ) {
		g_FakeLag.m_iAwaitingChoke = 0;
	}

	if( !ShouldAntiAim( pCmd ) )
		return;

	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	// we need to choke atleast 1 tick if we're using eye yaw
	// and 2 ticks if we're using opposite. only do this if 
	// the awaiting choke doesn't reach either of these, to not 
	// override our fakelag if we're fakelagging more
	if( g_FakeLag.m_iAwaitingChoke < 1 ) {
		// we are going to choke nForceChoke ticks (telling fakelag to not force send the packet..)
		g_FakeLag.m_iAwaitingChoke = 1;

		// force fakelag
		*bSendPacket = m_pClientState->m_nChokedCommands( ) >= 1;
	}

}

void AntiAim::HandlePitch( CUserCmd* pCmd ) {
	if( !g_Vars.rage.anti_aim_base_pitch )
		return;

	// note - michal;
	// reminder that when/if we also add nospread/untrusted compatibility
	// we should change these to untrusted pitches. no need for now
	switch( g_Vars.rage.anti_aim_base_pitch ) {
	case 1:
		pCmd->viewangles.x = 89.f;
		break;
	case 2:
		pCmd->viewangles.x = -89.f;
		break;
	case 3:
		pCmd->viewangles.x = 0.f;
		break;
	}
}

void AntiAim::HandleBaseYaws( CUserCmd* pCmd ) {
	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal || !pLocal->m_PlayerAnimState( ) )
		return;

	const float flAdditive = m_bJitterUpdate ? -g_Vars.rage.anti_aim_yaw_jitter : g_Vars.rage.anti_aim_yaw_jitter;

	if( g_Vars.rage.anti_aim_yaw_while_running == 1 && pLocal->m_PlayerAnimState( )->m_flVelocityLengthXY > 0.1f && ( pLocal->m_fFlags( ) & FL_ONGROUND ) && ( g_Prediction.GetFlags( ) & FL_ONGROUND ) && !( pCmd->buttons & IN_JUMP ) ) {
		pCmd->viewangles.y += Math::AngleNormalize( 180.f + RandomFloat( -75.f, 75.f ) );
		return;
	}

	switch( g_Vars.rage.anti_aim_yaw ) {
	case 0:
		break;
	case 1: // 180
		pCmd->viewangles.y += 179.f;
		break;
	case 2: // 180 jitter
		pCmd->viewangles.y += Math::AngleNormalize( 179.f + flAdditive );
		break;
	case 3: // jitter
		pCmd->viewangles.y += Math::AngleNormalize( m_bJitterUpdate ? g_Vars.rage.anti_aim_yaw_jitter + 180.f : g_Vars.rage.anti_aim_yaw_jitter );
		break;
	case 4: // Spin
		// finish this (if not ill do later np)
		pCmd->viewangles.y += Math::AngleNormalize( m_bJitterUpdate ? g_Vars.rage.anti_aim_yaw_jitter + 180.f : g_Vars.rage.anti_aim_yaw_jitter );
		break;
	case 5: // Sideways
		pCmd->viewangles.y += g_Vars.rage.anti_aim_base_yaw_additive;
		break;
	case 6: // Random
		pCmd->viewangles.y += RandomFloat( -360.f, 360.f );
		break;
	case 7: // Static
		pCmd->viewangles.y = g_Vars.rage.anti_aim_base_yaw_additive;
		break;
	case 8: // 180z
	{
		static bool bShould180z = false;

		// choose your starting angle
		float startPoint = 110.0f;

		// fix it to one float
		static float currentAng = startPoint;

		bool ZDSFGHKJTXDGFongroundLHKBJFLKHSDUF = ( pLocal->m_fFlags( ) & FL_ONGROUND ) || ( g_Prediction.GetFlags( ) & FL_ONGROUND );

		if( !ZDSFGHKJTXDGFongroundLHKBJFLKHSDUF ) {
			// increment it if we're in air
			currentAng += 5.0f;

			// clamp it incase it goes out of our maximum spin rage
			if( currentAng >= 250.f )
				currentAng = 250.f;

			// yurr
			bShould180z = true;

			// do 180 z ))
			pCmd->viewangles.y += currentAng;
		}
		else {
			// next tick, start at starting point
			currentAng = startPoint;

			// if we were in air tick before, go back to start point 
			// in order to properly rotate next (so fakelag doesnt fuck it up)
			pCmd->viewangles.y += bShould180z ? startPoint : 179.0f;

			// set for future idkf nigger
			bShould180z = false;
		}
		break;
	}
	}
}

void AntiAim::HandleYaw( CUserCmd* pCmd ) {
	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal || !pLocal->m_PlayerAnimState( ) )
		return;

	static float flRandomYaw = FLT_MAX;
	static float flLastMoveYaw = pLocal->m_flLowerBodyYawTarget( );
	const bool bOnGround = ( pLocal->m_fFlags( ) & FL_ONGROUND ) && ( g_Prediction.GetFlags( ) & FL_ONGROUND );
	const auto bEdgeData = HandleEdge( );
	const bool bShouldEdge = bEdgeData.first && bEdgeData.second != FLT_MAX && g_Vars.rage.anti_aim_edge > 0 && bOnGround && pLocal->m_PlayerAnimState( )->m_flVelocityLengthXY == 0.0f;

	// base yaw first.
	HandleManualAA( pCmd );

	HandleAntiAimOptions( pCmd );

	// manual override everything
	if( m_CurrentDirection != Directions::YAW_NONE ) {
		switch( m_CurrentDirection ) {
		case Directions::YAW_RIGHT:
			pCmd->viewangles.y += -90.f;

			return;
			break;
		case Directions::YAW_BACK:
			pCmd->viewangles.y += 180.f;

			return;
			break;
		case Directions::YAW_LEFT:
			pCmd->viewangles.y += 90.f;

			return;
			break;
		default:
			break;
		}

		return;
	}

	// first do yaws
	HandleBaseYaws( pCmd );

	// don't do freestand & edge in air
	if( !bOnGround ) {
		return;
	}

	// then freestand, under conditions
	bool bAllowFreestand = false;
	if( g_Vars.rage.anti_aim_freestand_default && pLocal->m_PlayerAnimState( )->m_flVelocityLengthXY == 0.0f )
		bAllowFreestand = true;

	if( g_Vars.rage.anti_aim_freestand_running && ( pLocal->m_PlayerAnimState( )->m_flVelocityLengthXY != 0.0f && !g_Vars.globals.m_bFakeWalking ) )
		bAllowFreestand = true;

	// then edge when necessary
	if( bShouldEdge ) {
		pCmd->viewangles.y = bEdgeData.second;

		// "Subtle edge -> Normal edge, slider not 0 activates some kind of jitter, the value confuses me idk what its doing"
		if( g_Vars.rage.anti_aim_edge == 2 ) {
			pCmd->viewangles.y += RandomFloat( -35.f, 35.f );
		}

		m_bEdging = true;

		// freestand on edge is off, don't do it
		if( !g_Vars.rage.anti_aim_freestand_edge ) {
			return;
		}
	}

	// then freestand, should override all yaws and edge
	if( bAllowFreestand )
		HandleFreestand( pCmd );
}

void AntiAim::HandleManualAA( CUserCmd* pCmd ) {
	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	if( g_Vars.rage.anti_aim_left_key.enabled ) {
		m_CurrentDirection = Directions::YAW_LEFT;
	}
	else if( g_Vars.rage.anti_aim_back_key.enabled ) {
		m_CurrentDirection = Directions::YAW_BACK;
	}
	else if( g_Vars.rage.anti_aim_right_key.enabled ) {
		m_CurrentDirection = Directions::YAW_RIGHT;
	}
	else {
		m_CurrentDirection = Directions::YAW_NONE;
	}

}

void AntiAim::HandleAntiAimOptions( CUserCmd* pCmd )
{
	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal || m_CurrentDirection != Directions::YAW_NONE )
		return;

	if( g_Vars.rage.anti_aim_base_yaw == 1 ) {
		const auto player = GetBestPlayer( pCmd );
		if( player ) {
			const float yaw = Math::CalcAngle( player->m_vecOrigin( ), pLocal->m_vecOrigin( ) ).y;

			pCmd->viewangles.y = yaw;
		}
	}
	else {
		if( g_Vars.rage.anti_aim_base_yaw == 2 ) {
			const auto player = GetBestPlayer( pCmd, true );
			if( player ) {
				const float yaw = Math::CalcAngle( player->m_vecOrigin( ), pLocal->m_vecOrigin( ) ).y;

				pCmd->viewangles.y = yaw;
			}
		}
	}
}

C_CSPlayer* AntiAim::GetBestPlayer(CUserCmd* pCmd, bool distance) {
	const auto pLocal = C_CSPlayer::GetLocalPlayer();
	if (!pLocal)
		return false;

	float bestDist = FLT_MAX;
	float bestFov = FLT_MAX;
	C_CSPlayer* bestPlayer = nullptr;
	for (int i = 1; i <= m_pGlobalVars->maxClients; i++) {
		auto player = C_CSPlayer::GetPlayerByIndex(i);
		if (!player || player->IsDead())
			continue;

		if (g_Visuals.player_fading_alpha.at(i) <= 0.4f)
			continue;

		if (player->IsTeammate(pLocal))
			continue;

		Vector vecOrigin = player->m_vecOrigin();

		if (distance) {
			float dist = (vecOrigin - pLocal->m_vecOrigin()).LengthSquared();
			if (dist < bestDist) {
				bestDist = dist;
				bestPlayer = player;
			}
		}
		else {
			auto AngleDistance = [&](QAngle& angles, const Vector& start, const Vector& end) -> float {
				auto direction = end - start;
				auto aimAngles = direction.ToEulerAngles();
				auto delta = aimAngles - angles;
				delta.Normalize();

				return sqrtf(delta.x * delta.x + delta.y * delta.y);
			};

			float Fov = AngleDistance(pCmd->viewangles, pLocal->GetEyePosition(), vecOrigin);

			if (Fov < bestFov) {
				bestFov = Fov;
				bestPlayer = player;
			}
		}
	}

	return bestPlayer;
}

//void AntiAim::HandleFreestand(CUserCmd* pCmd) {
//	const auto pLocal = C_CSPlayer::GetLocalPlayer();
//	if (!pLocal)
//		return;
//
//	auto best_player = GetBestPlayer(pCmd, false);
//
//	// we will require these eye positions
//	Vector eye_position = pLocal->GetAbsOrigin() + pLocal->m_vecViewOffset();
//	Vector target_eye_position = pLocal->GetAbsOrigin() + pLocal->m_vecViewOffset();
//
//	int right{0};
//	int left{0};
//
//	// this will result in a 45.0f deg step, modify if you want it to be more 'precise'
//	constexpr float angle_step = 0.31415927f;
//
//	// our result
//	float yaw = 0.0f;
//
//	// iterate through 45.0f deg angles  
//	for (float n = 0.0f; n < (M_PI * 2.0f); n += angle_step)
//	{
//		Vector head_position(38.f * cos(n) + eye_position.x, 38.f * sin(n) + eye_position.y, eye_position.z);
//
//		Ray_t ray;
//		CGameTrace tr;
//		ray.Init(eye_position, head_position);
//		CTraceFilter traceFilter;
//		traceFilter.pSkip = pLocal;
//		m_pEngineTrace->TraceRay(ray, 0x4600400B, &traceFilter, &tr);
//
//		/*if (tr.fraction != 1.f)
//		{
//			m_pDebugOverlay->AddLineOverlay(eye_position, eye_position + ((head_position - eye_position) * tr.fraction), 255, 0, 0, false, 2.f * m_pGlobalVars->interval_per_tick);
//		}*/
//		if (tr.fraction != 1.f)
//		{
//			if (auto diff = Math::AngleDiff(RAD2DEG(n), g_Movement.m_vecCmdAngles.y); abs(diff) <= 120.f)
//			{
//				if (diff > 0) {
//					right++;
//					//m_pDebugOverlay->AddLineOverlay(eye_position, eye_position + ((head_position - eye_position) * tr.fraction), 255, 0, 0, false, 2.f * m_pGlobalVars->interval_per_tick);
//
//				}
//				else {
//					left++;
//					//m_pDebugOverlay->AddLineOverlay(eye_position, eye_position + ((head_position - eye_position) * tr.fraction), 0, 255, 0, false, 2.f * m_pGlobalVars->interval_per_tick);
//				}
//			}
//		}
//	}
//
//	if (left == right)
//	{
//		m_CurrentDirection = Directions::YAW_BACK;
//		return;
//	}
//	
//	m_bFreestanding = true;
//	pCmd->viewangles.y = Math::AngleNormalize(pCmd->viewangles.y - (left < right ? -90.f : 90.f));
//
//}
void AntiAim::HandleFreestand( CUserCmd* pCmd ) {
	// constants.
	constexpr float STEP{ 4.f };
	constexpr float RANGE{ 120.f };

	const auto pLocal = C_CSPlayer::GetLocalPlayer( );

	if( !pLocal ) {
		m_CurrentDirection = Directions::YAW_BACK;
		return;
	}

	const auto player = GetBestPlayer( pCmd, false );

	if( !player ) {
		m_CurrentDirection = Directions::YAW_BACK;
		return;
	}

	if( !g_Vars.rage.anti_aim_freestand_default && !g_Vars.rage.anti_aim_freestand_running && !g_Vars.rage.anti_aim_freestand_edge ) {
		m_CurrentDirection = Directions::YAW_BACK;
		return;
	}

	// we will require these eye positions
	Vector eye_position = pLocal->GetAbsOrigin() + pLocal->m_vecViewOffset();
	
	int right{0};
	int left{0};
	
	// this will result in a 45.0f deg step, modify if you want it to be more 'precise'
	constexpr float angle_step = 0.31415927f;
	
	// our result
	float yaw = 0.0f;
	
	// iterate through 45.0f deg angles  
	for (float n = 0.0f; n < (M_PI * 2.0f); n += angle_step)
	{
		Vector head_position(38.f * cos(n) + eye_position.x, 38.f * sin(n) + eye_position.y, eye_position.z);
	
		Ray_t ray;
		CGameTrace tr;
		ray.Init(eye_position, head_position);
		CTraceFilter traceFilter;
		traceFilter.pSkip = pLocal;
		m_pEngineTrace->TraceRay(ray, 0x4600400B, &traceFilter, &tr);
	
		/*if (tr.fraction != 1.f)
		{
			m_pDebugOverlay->AddLineOverlay(eye_position, eye_position + ((head_position - eye_position) * tr.fraction), 255, 0, 0, false, 2.f * m_pGlobalVars->interval_per_tick);
		}*/
		if (tr.fraction != 1.f)
		{
			if (auto diff = Math::AngleDiff(RAD2DEG(n), g_Movement.m_vecCmdAngles.y); abs(diff) <= 120.f)
			{
				if (diff > 0) {
					right++;
					//m_pDebugOverlay->AddLineOverlay(eye_position, eye_position + ((head_position - eye_position) * tr.fraction), 255, 0, 0, false, 2.f * m_pGlobalVars->interval_per_tick);
	
				}
				else {
					left++;
					//m_pDebugOverlay->AddLineOverlay(eye_position, eye_position + ((head_position - eye_position) * tr.fraction), 0, 255, 0, false, 2.f * m_pGlobalVars->interval_per_tick);
				}
			}
		}
	}
	
	if (abs(left - right) > 1)
	{
		m_bFreestanding = true;
		pCmd->viewangles.y = Math::AngleNormalize(pCmd->viewangles.y - (left < right ? -90.f : 90.f));
		return;
	}

	auto local_eyepos = eye_position;

	// construct vector of angles to test.
	std::vector< AdaptiveAngle > angles{ };
	angles.emplace_back( g_Movement.m_vecOriginalCmdAngles.y - 180.f );
	angles.emplace_back( g_Movement.m_vecOriginalCmdAngles.y + 90.f );
	angles.emplace_back( g_Movement.m_vecOriginalCmdAngles.y - 90.f );

	// start the trace at the enemy shoot pos.
	auto start = player->GetAbsOrigin() + player->m_vecViewOffset();

	// see if we got any valid result.
	// if this is false the path was not obstructed with anything.
	bool valid{ false };

	// iterate vector of angles.
	for( auto it = angles.begin( ); it != angles.end( ); ++it ) {

		// compute the 'rough' estimation of where our head will be.
		Vector end{ local_eyepos.x + std::cos( DEG2RAD( it->m_yaw ) ) * RANGE,
			local_eyepos.y + std::sin( DEG2RAD( it->m_yaw ) ) * RANGE,
			local_eyepos.z };

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
		// set angle to backwards.
		m_CurrentDirection = Directions::YAW_BACK;
		return;
	}

	// put the most distance at the front of the container.
	std::sort( angles.begin( ), angles.end( ),
		[ ] ( const AdaptiveAngle& a, const AdaptiveAngle& b ) {
		return a.m_dist > b.m_dist;
	} );

	// the best angle should be at the front now.
	AdaptiveAngle* best = &angles.front( );

	m_bFreestanding = true;
	pCmd->viewangles.y = Math::AngleNormalize( best->m_yaw );
}


std::pair<bool, float> AntiAim::HandleEdge( ) {
	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return { };

	float flClosestDistance = 100.0f;
	float step = M_PI * 2.0 / 8;
	Vector vecEyePos = pLocal->GetEyePosition( );

	Ray_t ray;
	CGameTrace tr;
	CTraceFilter filter;
	filter.pSkip = pLocal;

	float flEdgeYaw = FLT_MAX;
	for( float i = 0; i < ( M_PI * 2.0 ); i += ( M_PI * 2.0 / 8 ) ) {
		const Vector location( 45.f * cos( i ) + vecEyePos.x, 45.1f * sin( i ) + vecEyePos.y, vecEyePos.z );

		m_pEngineTrace->TraceRay( { vecEyePos, location }, 0x4600400B, &filter, &tr );

		float flClosestWall = vecEyePos.Distance( tr.endpos );

		if( flClosestWall < flClosestDistance ) {
			flClosestDistance = flClosestWall;
			flEdgeYaw = RAD2DEG( i );
		}
	}

	return std::make_pair( flClosestDistance < 45.f, flEdgeYaw );
}

void AntiAim::HandleAntiAim( bool* bSendPacket, bool* bFinalTick, CUserCmd* pCmd )
{
	const auto pLocal = C_CSPlayer::GetLocalPlayer( );

	if( !pLocal || !pLocal->m_PlayerAnimState( ) )
		return;

	if( !ShouldAntiAim( pCmd ) )
		return;

	// handle pitch, yaw, desync
	HandlePitch( pCmd );

	if( !*bSendPacket ) {
		HandleYaw( pCmd );
		m_flLastRealAngle = pCmd->viewangles.y;

		// handle twist (basically last move lby breaker i guess??)
		if( g_Vars.rage.anti_aim_crooked ) {
			static float flLastMoveYaw = pCmd->viewangles.y;
			if( pLocal->m_PlayerAnimState( )->m_flVelocityLengthXY != 0.0f ) {
				flLastMoveYaw = pCmd->viewangles.y;
			}

			if( pLocal->m_PlayerAnimState( )->m_flVelocityLengthXY == 0.0f ) {
				if( abs( Math::AngleDiff( flLastMoveYaw, pCmd->viewangles.y ) ) < 35.f ) {
					pCmd->viewangles.y += 45.f;
				}
			}
		}
	}
	else {
		m_bJitterUpdate = !m_bJitterUpdate;

		switch( g_Vars.rage.anti_aim_fake_type ) {
		case 1: pCmd->viewangles.y = Math::AngleNormalize( m_flLastRealAngle + RandomFloat( -85.f, 85.f ) ); break;
		case 2: /*local view*/ break;
		case 3: pCmd->viewangles.y += Math::AngleNormalize( m_flLastRealAngle + 180.f ); break;
		}
	}

	if( !m_pClientState->m_nChokedCommands( )
		&& g_Vars.rage.anti_aim_fake_type > 0
		&& ( pLocal->m_fFlags( ) & FL_ONGROUND ) != 0
		&& abs( m_pGlobalVars->curtime - g_ServerAnimations.m_uServerAnimations.m_flLowerBodyRealignTimer ) < 1.35f
		&& pLocal->m_PlayerAnimState( )->m_flVelocityLengthXY < 0.1f )
	{
		RandomSeed(m_pGlobalVars->tickcount);
		float flLbyDelta = RandomFloat(105.f, 118.f);
		float flBreakDelta = (m_bLBYDirectionSwitch ? -flLbyDelta : flLbyDelta);

		if( m_bEdging ) {
			flBreakDelta = g_Vars.rage.anti_aim_edge_delta;
		}

		if( m_bFreestanding ) {
			flBreakDelta = g_Vars.rage.anti_aim_freestand_edge ? g_Vars.rage.anti_aim_edge_delta : g_Vars.rage.anti_aim_freestand_delta;
		}

		if( m_pGlobalVars->curtime > g_ServerAnimations.m_uServerAnimations.m_flLowerBodyRealignTimer ) {
			/*
			pCmd->viewangles.y = m_flLastRealAngle + ( m_bLBYDirectionSwitch ? -105.f : 105.f );
			m_bLBYDirectionSwitch = !m_bLBYDirectionSwitch;
			*/

			pCmd->viewangles.y = m_flLastRealAngle + flBreakDelta;
			if (!g_Vars.globals.m_bFakeWalking)
			*bSendPacket = false;

			m_bLBYDirectionSwitch = !m_bLBYDirectionSwitch;
		}
		else {
			if( ( m_pGlobalVars->curtime + m_pGlobalVars->interval_per_tick ) > g_ServerAnimations.m_uServerAnimations.m_flLowerBodyRealignTimer ) {
				if( g_Vars.rage.anti_aim_twist )
					pCmd->viewangles.y = m_flLastRealAngle - flBreakDelta;
			}
		}
	}

	pCmd->viewangles.y = Math::AngleNormalize( pCmd->viewangles.y );
}
