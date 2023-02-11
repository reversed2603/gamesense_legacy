#include "AnimationSystem.hpp"
#include "LagCompensation.hpp"
#include "Resolver.hpp"
#include "../../Utils/Threading/Threading.h"

AnimationSystem g_AnimationSystem;

AnimationData* AnimationSystem::GetAnimationData( int index ) {
	if( m_AnimatedEntities.count( index ) < 1 )
		return nullptr;

	return &m_AnimatedEntities[ index ];
}

void AnimationSystem::FrameStage( ) {
	if( !m_pEngine->IsInGame( ) || !m_pEngine->GetNetChannelInfo( ) ) {
		m_AnimatedEntities.clear( );
		return;
	}

	if (!m_pEngine->IsPlayingDemo())
	{
		constexpr auto te_fire_bullets = hash_32_fnv1a_const("CTEFireBullets");

		for (auto event = *(CEventInfo**)(uintptr_t(m_pClientState.Xor()) + 0x4DEC); event; event = event->pNextEvent)
		{
			auto v25 = event->pClientClass;
			if (v25)
			{
				if (hash_32_fnv1a_const(v25->m_pNetworkName) == te_fire_bullets)
					event->fire_delay = 0.0f;
			}
		}

		m_pEngine->FireEvents();
	}

	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	for( int i = 1; i <= m_pGlobalVars->maxClients; ++i ) {
		auto player = C_CSPlayer::GetPlayerByIndex( i );
		if( !player || player == pLocal )
			continue;

		player_info_t player_info;
		if( !m_pEngine->GetPlayerInfo( player->m_entIndex, &player_info ) ) {
			continue;
		}

		m_AnimatedEntities[ i ].Collect( player );
	}

	if( m_AnimatedEntities.empty( ) )
		return;

	for( auto& [key, value] : m_AnimatedEntities ) {
		auto entity = C_CSPlayer::GetPlayerByIndex( key );
		if( !entity )
			continue;

		if( value.m_bUpdated )
			value.Update( );

		value.m_bUpdated = false;
	}
}

void AnimationData::CorrectVelocity(Encrypted_t<C_CSPlayer> m_player, Encrypted_t< AnimationRecord> m_record, Encrypted_t< AnimationRecord> m_previous, Encrypted_t< AnimationRecord> m_pre_previous)
{
	m_record->m_vecVelocity.Set();
	m_record->m_vecAbsVelocity.Set();
	Vector vecAnimVelocity = m_player->m_vecVelocity();

	if (!m_previous.IsValid())
	{
		if (m_record->m_pServerAnimOverlays[6].m_flPlaybackRate > 0.0f && vecAnimVelocity.Length() > 0.f)
		{
			if (m_record->m_pServerAnimOverlays[6].m_flWeight > 0.0f) {
				auto v30 = m_player->GetMaxSpeed();

				if (m_record->m_fFlags & 6)
					v30 = v30 * 0.34f;
				else if (m_player->m_bIsWalking())
					v30 = v30 * 0.52f;

				auto v35 = m_record->m_pServerAnimOverlays[6].m_flWeight * v30;
				vecAnimVelocity *= v35 / vecAnimVelocity.Length();
			}

			if (m_record->m_fFlags & 1)
				vecAnimVelocity.z = 0.f;
		}
		else
			vecAnimVelocity.Set();

		m_record->m_vecVelocity = vecAnimVelocity;
		m_record->m_vecAbsVelocity = vecAnimVelocity;
		return;
	}

	// check if player has been on ground for two consecutive ticks
	bool bIsOnground = m_record->m_fFlags & FL_ONGROUND && m_previous->m_fFlags & FL_ONGROUND;

	const Vector vecOriginDelta = m_record->m_vecOrigin - m_previous->m_vecOrigin;

	if (bIsOnground && m_record->m_pServerAnimOverlays[6].m_flPlaybackRate <= 0.0f)
		vecAnimVelocity.Set();

	//
	// entity teleported, reset his velocity (thats what server does)
	// - L3D451R7
	//
	if ((m_player->m_fEffects() & 8) != 0
		|| m_player->m_ubEFNoInterpParity() != m_player->m_ubEFNoInterpParityOld())
	{
		m_record->m_vecVelocity.Set();
		m_record->m_vecAbsVelocity.Set();
		return;
	}

	if (m_record->m_iChokeTicks <= 1 || (m_record->m_AnimationFlags & ELagRecordFlags::RF_IsInvalid)) {
		m_record->m_vecVelocity = vecAnimVelocity;
		m_record->m_vecAbsVelocity = vecAnimVelocity;
		return;
	}

	// fix up velocity 
	auto origin_delta_lenght = vecOriginDelta.Length();

	if (m_record->m_flChokeTime > 0.0f && m_record->m_flChokeTime < 1.0f && origin_delta_lenght >= 1.f && origin_delta_lenght <= 1000000.0f) {
		vecAnimVelocity = vecOriginDelta / m_record->m_flChokeTime;

		vecAnimVelocity.ValidateVector();

		m_record->m_vecVelocity = vecAnimVelocity;
		// store estimated velocity as main velocity.

		if (!bIsOnground)
		{
			//
			// s/o estk for this correction :^)
			// -L3D451R7
			auto currently_ducking = m_record->m_fFlags & 2;
			if ((m_previous->m_fFlags & 2) != currently_ducking)
			{
				float duck_modifier;
				if (currently_ducking)
					duck_modifier = 9.f;
				else
					duck_modifier = -9.f;
				
				vecAnimVelocity.z += duck_modifier;
			}
		}
	}

	float anim_speed = 0.f;

	// determine if we can calculate animation speed modifier using server anim overlays
	if( bIsOnground
		&& m_record->m_pServerAnimOverlays[ ANIMATION_LAYER_ALIVELOOP ].m_flWeight > 0.0f 
		&& m_record->m_pServerAnimOverlays[ ANIMATION_LAYER_ALIVELOOP ].m_flWeight < 1.0f 
		&& m_record->m_pServerAnimOverlays[ ANIMATION_LAYER_ALIVELOOP ].m_flPlaybackRate == m_record->m_pServerAnimOverlays[ ANIMATION_LAYER_ALIVELOOP ].m_flPlaybackRate )
	{
		// calculate animation speed yielded by anim overlays
		auto flAnimModifier = 0.35f * ( 1.0f - m_record->m_pServerAnimOverlays[ ANIMATION_LAYER_ALIVELOOP ].m_flWeight );
		if( flAnimModifier > 0.0f && flAnimModifier < 1.0f )
			anim_speed = m_player->GetMaxSpeed() * ( flAnimModifier + 0.55f );
	}

	// this velocity is valid ONLY IN ANIMFIX UPDATE TICK!!!
	// so don't store it to record as m_vecVelocity. i added vecAbsVelocity for that, it acts as a animationVelocity.
	// -L3D451R7
	if(anim_speed > 0.0f ) {
		anim_speed /= vecAnimVelocity.Length2D();
		vecAnimVelocity.x *= anim_speed;
		vecAnimVelocity.y *= anim_speed;
	}

	// calculate average player direction when bunny hopping
	if (m_pre_previous.IsValid() && vecAnimVelocity.Length() >= 400.f)
	{
		auto vecPreviousVelocity = (m_previous->m_vecOrigin - m_pre_previous->m_vecOrigin) / m_previous->m_flChokeTime;

		// make sure to only calculate average player direction whenever they're bhopping
		if (!vecPreviousVelocity.IsZero() && !bIsOnground) {
			auto vecCurrentDirection = Math::AngleNormalize(RAD2DEG(atan2(vecAnimVelocity.y, vecAnimVelocity.x)));
			auto vecPreviousDirection = Math::AngleNormalize(RAD2DEG(atan2(vecPreviousVelocity.y, vecPreviousVelocity.x)));

			auto vecAvgDirection = vecCurrentDirection - vecPreviousDirection;
			vecAvgDirection = DEG2RAD(Math::AngleNormalize(vecCurrentDirection + vecAvgDirection * 0.5f));

			auto vecDirectionCos = cos(vecAvgDirection);
			auto vecDirectionSin = sin(vecAvgDirection);

			// modify velocity accordingly
			vecAnimVelocity.x = vecDirectionCos * vecAnimVelocity.Length2D();
			vecAnimVelocity.y = vecDirectionSin * vecAnimVelocity.Length2D();
		}
	}

	if (!bIsOnground)
	{
		// correct z velocity.
		// https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/shared/gamemovement.cpp#L1950
		if (!(m_record->m_fFlags & FL_ONGROUND))
			vecAnimVelocity.z -= g_Vars.sv_gravity->GetFloat() * m_record->m_flChokeTime * 0.5f;
		else
			vecAnimVelocity.z = 0.0f;
	}
	else
		vecAnimVelocity.z = 0.0f;

	// detect fakewalking players
	if (!vecAnimVelocity.IsZero() && vecAnimVelocity.Length() >= 0.1f)
	{
		if (m_record->m_pServerAnimOverlays[4].m_flPlaybackRate == 0.0f
			&& m_record->m_pServerAnimOverlays[5].m_flPlaybackRate == 0.0f
			&& m_record->m_pServerAnimOverlays[6].m_flPlaybackRate == 0.0f
			&& m_record->m_fFlags & FL_ONGROUND) 
		{
			vecAnimVelocity.Init();
			m_record->m_AnimationFlags |= ELagRecordFlags::RF_FakeWalking;
			m_record->m_vecVelocity.Init();
		}
	}

	vecAnimVelocity.ValidateVector();

	// assign fixed velocity to record velocity
	m_record->m_vecAbsVelocity = vecAnimVelocity;
}

void AnimationData::Collect( C_CSPlayer* pPlayer ) {
	if( pPlayer->IsDead( ) )
		player = nullptr;

	// reset data
	if( player != pPlayer ) {
		m_flSimulationTime = 0.0f;
		m_flOldSimulationTime = 0.0f;
		m_AnimationRecord.clear( );
		m_bWasDormant = false;
		player = pPlayer;
		m_bIsAlive = false;
	}

	if( !pPlayer )
		return;

	m_bIsAlive = true;
	m_flOldSimulationTime = player->m_flOldSimulationTime( );
	m_flSimulationTime = player->m_flSimulationTime( );

	if( m_flSimulationTime == 0.0f || player->IsDormant( ) ) {
		m_bWasDormant = true;
		last_alive_loop_data.m_flCycle = 0;
		last_alive_loop_data.m_flPlaybackRate = 0;
		return;
	}

	if (last_alive_loop_data.m_flCycle == player->m_AnimOverlay()[animstate_layer_t::ANIMATION_LAYER_ALIVELOOP].m_flCycle
		&& last_alive_loop_data.m_flPlaybackRate == player->m_AnimOverlay()[animstate_layer_t::ANIMATION_LAYER_ALIVELOOP].m_flPlaybackRate) 
	{
		// fake update.
		//
		player->m_flSimulationTime() = m_flOldSimulationTime; // s/o estk :^))))
		return;
	}

	last_alive_loop_data.m_flCycle = player->m_AnimOverlay( )[ animstate_layer_t::ANIMATION_LAYER_ALIVELOOP ].m_flCycle;
	last_alive_loop_data.m_flPlaybackRate = player->m_AnimOverlay( )[ animstate_layer_t::ANIMATION_LAYER_ALIVELOOP ].m_flPlaybackRate;

	//
	// no need for this check anymore boi
	// - L3D451R7
	/*if( m_flOldSimulationTime == m_flSimulationTime ) {
		return;
	}*/

	if( m_bWasDormant ) {
		m_AnimationRecord.clear( );
	}

	m_bUpdated = true;
	m_bWasDormant = false;

	int nTickRate = int( 1.0f / m_pGlobalVars->interval_per_tick );

	while( m_AnimationRecord.size( ) > nTickRate ) {
		m_AnimationRecord.pop_back( );
	}

	auto pWeapon = ( C_WeaponCSBaseGun* )( player->m_hActiveWeapon( ).Get( ) );

	AnimationRecord* pPreviousRecord = nullptr;
	AnimationRecord* pPenultimateRecord = nullptr;

	if( m_AnimationRecord.size( ) > 0 ) {
		pPreviousRecord = &m_AnimationRecord.front( );

		if( m_AnimationRecord.size( ) > 1 ) {
			pPenultimateRecord = &m_AnimationRecord.at( 1 );
		}
	}

	// emplace new record
	auto pNewAnimRecord = &m_AnimationRecord.emplace_front( );

	// fill up this record with all basic information
	std::memcpy( pNewAnimRecord->m_pServerAnimOverlays.data( ), player->m_AnimOverlay( ).Base( ), 13 * sizeof( C_AnimationLayer ) );
	pNewAnimRecord->m_flFeetCycle = pNewAnimRecord->m_pServerAnimOverlays[ 6 ].m_flCycle;
	pNewAnimRecord->m_flMoveWeight = pNewAnimRecord->m_pServerAnimOverlays[ 6 ].m_flWeight;
	pNewAnimRecord->m_flLowerBodyYawTarget = player->m_flLowerBodyYawTarget( );
	pNewAnimRecord->m_angEyeAngles = player->m_angEyeAngles( );
	pNewAnimRecord->m_flDuckAmount = player->m_flDuckAmount( );
	pNewAnimRecord->m_flSimulationTime = m_flSimulationTime;
	pNewAnimRecord->m_flAnimationTime = m_flOldSimulationTime + m_pGlobalVars->interval_per_tick;
	pNewAnimRecord->m_vecOrigin = player->m_vecOrigin( );
	pNewAnimRecord->m_fFlags = player->m_fFlags( );
	pNewAnimRecord->m_AnimationFlags = ELagRecordFlags::RF_None;
	pNewAnimRecord->m_flShotTime = 0.0f;
	pNewAnimRecord->m_pEntity = player;

	const auto simulation_ticks = TIME_TO_TICKS(m_flSimulationTime);
	const auto old_simulation_ticks = TIME_TO_TICKS(m_flOldSimulationTime);

	// calculate choke time and choke ticks, compensating
	// for players coming out of dormancy / newly generated records
	if( pPreviousRecord && abs(simulation_ticks - old_simulation_ticks) <= 31) {
		pNewAnimRecord->m_flChokeTime = m_flSimulationTime - m_flOldSimulationTime;
		pNewAnimRecord->m_iChokeTicks = simulation_ticks - old_simulation_ticks;
	}
	else {
		pNewAnimRecord->m_flChokeTime = m_pGlobalVars->interval_per_tick;
		pNewAnimRecord->m_iChokeTicks = 1;
		pPreviousRecord = nullptr;
	}

	if (pWeapon && pWeapon->IsGun())
	{
		const auto& shot_tick = TIME_TO_TICKS(pWeapon->m_fLastShotTime());

		if (shot_tick > old_simulation_ticks && simulation_ticks >= shot_tick)
		{
			pNewAnimRecord->m_AnimationFlags |= ELagRecordFlags::RF_Shot;

			if (shot_tick == simulation_ticks)
				pNewAnimRecord->m_AnimationFlags |= ELagRecordFlags::RF_ShotNoPSilent;

			//m_pCvar->ConsoleColorPrintf(Color::Red(), "shot fired! [%s]:[%i]\n", (pNewAnimRecord->m_AnimationFlags & ELagRecordFlags::RF_ShotNoPSilent ? "+" : "-"), TIME_TO_TICKS(m_flSimulationTime) - shot_tick);
		}
		else
		{
			if (abs(simulation_ticks - shot_tick) > pNewAnimRecord->m_iChokeTicks + 2)
			{
				m_flLastNonShotPitch = pNewAnimRecord->m_angEyeAngles.x;
			}
		}
	}

	if (pNewAnimRecord->m_AnimationFlags & ELagRecordFlags::RF_Shot)
	{
		//fakelagging_and_other_checks = (ImportantFlags_2 & 16) == 0;
		//if (fakelagging_and_other_checks)
		//{
		if (pNewAnimRecord->m_iChokeTicks > 1) 
		{
			if (pNewAnimRecord->m_angEyeAngles.x < 70.0f)            // if(eyeangles.x < 70)
			{
				if (m_flLastNonShotPitch > 80.0f)
					pNewAnimRecord->m_angEyeAngles.x = m_flLastNonShotPitch;
			}
		}
		//}
	}

	pNewAnimRecord->m_AnimationFlags &= ~ELagRecordFlags::RF_IsInvalid;

	CorrectVelocity(pPlayer, pNewAnimRecord, pPreviousRecord, pPenultimateRecord);

	// we'll need information from the previous record in order to further
	// fix animations, skip everything and invalidate crucial data
	if( !pPreviousRecord ) {
		pNewAnimRecord->m_AnimationFlags |= ELagRecordFlags::RF_None;
		pNewAnimRecord->m_vecVelocity.Init( );

		pNewAnimRecord->m_AnimationFlags &= ~ELagRecordFlags::RF_BrokeTeleportDst;

		// we're done here
		return;
	}

	// detect players breaking teleport distance
	// https://github.com/perilouswithadollarsign/cstrike15_src/blob/master/game/server/player_lagcompensation.cpp#L384-L388
	if( pNewAnimRecord->m_vecOrigin.DistanceSquared( pPreviousRecord->m_vecOrigin ) > 4096.0f ) {
		pNewAnimRecord->m_AnimationFlags |= ELagRecordFlags::RF_BrokeTeleportDst;
	}

	// NITRO CODE NITRO CODE NITRO CODE

	// set previous flags.
	if( pPreviousRecord && pNewAnimRecord->m_iChokeTicks > 2 ) {
		player->m_fFlags( ) = pPreviousRecord->m_fFlags;

		// strip the on ground flag.
		player->m_fFlags( ) &= ~FL_ONGROUND;

		// been onground for 2 consecutive ticks? fuck yeah.
		if( pNewAnimRecord->m_fFlags & FL_ONGROUND && pPreviousRecord->m_fFlags & FL_ONGROUND )
			player->m_fFlags( ) |= FL_ONGROUND;

		// fix jump_fall.
		if( pNewAnimRecord->m_pServerAnimOverlays[ 4 ].m_flWeight != 1.f && pPreviousRecord->m_pServerAnimOverlays[ 4 ].m_flWeight == 1.f && pNewAnimRecord->m_pServerAnimOverlays[ 5 ].m_flWeight != 0.f )
			player->m_fFlags( ) |= FL_ONGROUND;

		if( pNewAnimRecord->m_fFlags & FL_ONGROUND && !( pPreviousRecord->m_fFlags & FL_ONGROUND ) )
			player->m_fFlags( ) &= ~FL_ONGROUND;
	}
}

void AnimationData::Update( ) {
	if( !player || m_AnimationRecord.size( ) < 1 )
		return;

	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	auto pAnimationRecord = &m_AnimationRecord.front( );
	if( !pAnimationRecord )
		return;

	AnimationRecord* pPreviousAnimationRecord = nullptr;
	if( m_AnimationRecord.size( ) > 1 ) {
		pPreviousAnimationRecord = &m_AnimationRecord.at( 1 );
	}

	// simulate player animations
	SimulateAnimations( pAnimationRecord, pPreviousAnimationRecord );

	// restore server animlayers
	std::memcpy( player->m_AnimOverlay( ).Base( ), pAnimationRecord->m_pServerAnimOverlays.data( ), 13 * sizeof( C_AnimationLayer ) );

	player->m_AnimOverlay( )[ animstate_layer_t::ANIMATION_LAYER_LEAN ].m_flWeight = 0.f;

	// generate aimbot matrix
	player->SetupBones( pAnimationRecord->m_pMatrix, 128, 0xFFF00, player->m_flSimulationTime( ) );

	player->m_AnimOverlay( )[ animstate_layer_t::ANIMATION_LAYER_LEAN ].m_flWeight = pAnimationRecord->m_pServerAnimOverlays[ animstate_layer_t::ANIMATION_LAYER_LEAN ].m_flWeight;
}

void AnimationData::UpdatePlayer( C_CSPlayer* pEntity, AnimationRecord* pRecord ) {
	const float flCurtime = m_pGlobalVars->curtime;
	const float flFrametime = m_pGlobalVars->frametime;

	CCSGOPlayerAnimState* pState = pEntity->m_PlayerAnimState( );
	if( !pState )
		return;

	// backup data before changes
	float flCurtimeBackup = m_pGlobalVars->curtime;
	float flFrametimeBackup = m_pGlobalVars->frametime;

	// calculate animations based on ticks aka server frames instead of render frames
	if( pRecord->m_iChokeTicks > 19 )
		m_pGlobalVars->curtime = pEntity->m_flSimulationTime( );
	else
		m_pGlobalVars->curtime = pRecord->m_flAnimationTime;

	m_pGlobalVars->frametime = m_pGlobalVars->interval_per_tick;

	pEntity->m_iEFlags( ) &= ~EFlags_t::EFL_DIRTY_ABSVELOCITY;
	pEntity->GetAbsVelocity() = pRecord->m_vecAbsVelocity;

	if( pState->m_nLastUpdateFrame >= m_pGlobalVars->framecount )
		pState->m_nLastUpdateFrame = m_pGlobalVars->framecount - 1;

	// resolve player
	if( !pEntity->IsTeammate( C_CSPlayer::GetLocalPlayer( ) ) ) {
		g_Resolver.ResolvePlayers( pRecord );
	}

	pEntity->m_bClientSideAnimation( ) = true;
	pEntity->UpdateClientSideAnimation( );
	pEntity->m_bClientSideAnimation( ) = false;

	pEntity->InvalidatePhysicsRecursive( ANGLES_CHANGED | ANIMATION_CHANGED | SEQUENCE_CHANGED );

	// restore globals
	m_pGlobalVars->curtime = flCurtimeBackup;
	m_pGlobalVars->frametime = flFrametimeBackup;
}

void AnimationData::SimulateMovement( C_CSPlayer* pEntity, Vector& vecOrigin, Vector& vecVelocity, int& fFlags, bool bOnGround ) {
	if( !( fFlags & FL_ONGROUND ) )
		vecVelocity.z -= TICKS_TO_TIME( g_Vars.sv_gravity->GetFloat( ) );
	else if( ( pEntity->m_fFlags( ) & FL_ONGROUND ) && !bOnGround )
		vecVelocity.z = g_Vars.sv_jump_impulse->GetFloat( );

	const auto src = vecOrigin;
	auto vecEnd = src + vecVelocity * m_pGlobalVars->interval_per_tick;

	Ray_t r;
	r.Init( src, vecEnd, pEntity->OBBMins( ), pEntity->OBBMaxs( ) );

	CGameTrace t{ };
	CTraceFilter filter;
	filter.pSkip = pEntity;

	m_pEngineTrace->TraceRay( r, CONTENTS_SOLID, &filter, &t );

	if( t.fraction != 1.f ) {
		for( auto i = 0; i < 2; i++ ) {
			vecVelocity -= t.plane.normal * vecVelocity.Dot( t.plane.normal );

			const auto flDot = vecVelocity.Dot( t.plane.normal );
			if( flDot < 0.f )
				vecVelocity -= Vector( flDot * t.plane.normal.x,
					flDot * t.plane.normal.y, flDot * t.plane.normal.z );

			vecEnd = t.endpos + vecVelocity * TICKS_TO_TIME( 1.f - t.fraction );

			r.Init( t.endpos, vecEnd, pEntity->OBBMins( ), pEntity->OBBMaxs( ) );
			m_pEngineTrace->TraceRay( r, CONTENTS_SOLID, &filter, &t );

			if( t.fraction == 1.f )
				break;
		}
	}

	vecOrigin = vecEnd = t.endpos;
	vecEnd.z -= 2.f;

	r.Init( vecOrigin, vecEnd, pEntity->OBBMins( ), pEntity->OBBMaxs( ) );
	m_pEngineTrace->TraceRay( r, CONTENTS_SOLID, &filter, &t );

	fFlags &= ~FL_ONGROUND;

	if( t.DidHit( ) && t.plane.normal.z > .7f )
		fFlags |= FL_ONGROUND;
}

void AnimationData::SimulateAnimations( AnimationRecord* pRecord, AnimationRecord* pPrevious ) {
	auto pState = player->m_PlayerAnimState( );
	if( !pState ) {
		return;
	}

	int m_fFlags;
	float m_flDuckAmount;
	float m_flPrimaryCycle;
	float m_flMoveWeight;
	Vector m_vecOrigin;

	m_fFlags = player->m_fFlags( );
	m_flDuckAmount = player->m_flDuckAmount( );
	m_vecOrigin = player->m_vecOrigin( );

	m_flPrimaryCycle = pState->m_flPrimaryCycle;
	m_flMoveWeight = pState->m_flMoveWeight;

	// nothing to work with..
	if( !pPrevious ) {
		player->m_fFlags( ) = m_fFlags;
		player->m_flDuckAmount( ) = m_flDuckAmount;
		player->m_vecOrigin( ) = m_vecOrigin;

		pState->m_flPrimaryCycle = m_flPrimaryCycle;
		pState->m_flMoveWeight = m_flMoveWeight;

		UpdatePlayer( player, pRecord );
		return;
	}

	int fPreviousFlags = pPrevious->m_fFlags;

	// backuP SHIT
	const float flBackupSimulationTime = player->m_flSimulationTime( );

	player->m_vecVelocity( ) = pRecord->m_vecVelocity;
	player->SetAbsVelocity( pRecord->m_vecVelocity );
	player->SetAbsOrigin( pRecord->m_vecOrigin );
	player->m_fFlags( ) = pRecord->m_fFlags;
	player->m_angEyeAngles( ) = pRecord->m_angEyeAngles;
	player->m_flDuckAmount( ) = pRecord->m_flDuckAmount;

	float frac = ( pRecord->m_flAnimationTime - pRecord->m_flSimulationTime ) / ( pPrevious->m_flSimulationTime - pRecord->m_flSimulationTime );

	// fix duck amount while choking
	if( pRecord->m_iChokeTicks >= 2 )
		player->m_flDuckAmount( ) = Math::Lerp( frac, pPrevious->m_flDuckAmount, pRecord->m_flDuckAmount );

	// simulate player movement
	if( g_Vars.rage.fakelag_correction == 1 )
		SimulateMovement( player, pPrevious->m_vecOrigin, player->m_vecVelocity( ), player->m_fFlags( ), fPreviousFlags & FL_ONGROUND );

	// store previous player flags, in order to simulate player movement the next tick
	fPreviousFlags = player->m_fFlags( );

	// simulate player
	UpdatePlayer( player, pRecord );

	player->m_fFlags( ) = m_fFlags;
	player->m_flDuckAmount( ) = m_flDuckAmount;
	player->m_vecOrigin( ) = m_vecOrigin;

	pState->m_flPrimaryCycle = m_flPrimaryCycle;
	pState->m_flMoveWeight = m_flMoveWeight;

	player->m_flSimulationTime( ) = flBackupSimulationTime;
}