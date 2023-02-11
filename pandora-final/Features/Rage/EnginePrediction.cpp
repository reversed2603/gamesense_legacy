#include "EnginePrediction.hpp"
#include "../../SDK/Classes/Player.hpp"
#include "../../SDK/Classes/weapon.hpp"
#include "../../source.hpp"
#include "../../SDK/Valve/CBaseHandle.hpp"
#include "../../SDK/displacement.hpp"
#include "../Rage/TickbaseShift.hpp"
#include "../../Hooking/Hooked.hpp"
#include "../Rage/ServerAnimations.hpp"
#include "../Miscellaneous/Movement.hpp"
#include <deque>

Prediction g_Prediction;

void Prediction::StartCommand( C_CSPlayer* player, CUserCmd* ucmd ) {
	// StartCommand rebuild
	player->SetCurrentCommand( predictionData->m_pCmd.Xor( ) );
	C_BaseEntity::SetPredictionRandomSeed( predictionData->m_pCmd.Xor( ) );
	C_BaseEntity::SetPredictionPlayer( predictionData->m_pPlayer );
}

void Prediction::PrePrediction(Encrypted_t<CUserCmd> _cmd) 
{
	predictionData->m_pCmd = _cmd;

	if (!predictionData->m_pCmd.IsValid())
		return;

	predictionData->m_pPlayer = C_CSPlayer::GetLocalPlayer();

	if (!predictionData->m_pPlayer || predictionData->m_pPlayer->IsDead())
		return;

	predictionData->m_pWeapon = (C_WeaponCSBaseGun*)predictionData->m_pPlayer->m_hActiveWeapon().Get();
	if (!predictionData->m_pWeapon)
		return;

	predictionData->m_pWeaponInfo = predictionData->m_pWeapon->GetCSWeaponData();
	if (!predictionData->m_pWeaponInfo.IsValid())
		return;

	m_in_prediction = m_pPrediction->m_in_prediction;
	m_first_command_pred = m_pPrediction->m_is_first_time_predicted;

	predictionData->m_nTickBase = predictionData->m_pPlayer->m_nTickBase();

	// correct tickbase.
	if (!m_pLastCmd || m_pLastCmd->hasbeenpredicted)
		m_iSeqDiff = _cmd->command_number - predictionData->m_pPlayer->m_nTickBase();

	m_pLastCmd = predictionData->m_pCmd.Xor();

	if (!predictionData->m_pPlayer->m_bIsScoped())
		m_flMaxWeaponSpeed = predictionData->m_pWeaponInfo->m_flMaxSpeed;
	else
		m_flMaxWeaponSpeed = predictionData->m_pWeaponInfo->m_flMaxSpeed2;

	predictionData->m_fFlags = predictionData->m_pPlayer->m_fFlags();
	predictionData->m_vecVelocity = predictionData->m_pPlayer->m_vecVelocity();
	predictionData->m_vecAbsVelocity = predictionData->m_pPlayer->GetAbsVelocity();
	predictionData->m_flVelocityModifier = predictionData->m_pPlayer->m_flVelocityModifier();

	predictionData->m_flCurrentTime = m_pGlobalVars->curtime;
	predictionData->m_flFrameTime = m_pGlobalVars->frametime;

	if (m_nDeltaTick > 0 || m_nDeltaTick != m_pClientState->m_nDeltaTick()) {
		m_nDeltaTick = m_pClientState->m_nDeltaTick();

		predictionData->m_RestoreData.Setup(C_CSPlayer::GetLocalPlayer());
		return;
	}

	RestoreNetvarCompression(_cmd->command_number-1);

	predictionData->m_RestoreData.Setup(C_CSPlayer::GetLocalPlayer());
}

// C_BasePlayer::PhysicsSimulate & CPrediction::RunCommand rebuild
void Prediction::Begin( Encrypted_t<CUserCmd> _cmd )
{
	m_bInPrediction = true;

	const QAngle oldang = _cmd->viewangles;

	_cmd->viewangles = g_Movement.m_vecCmdAngles;

	if( Engine::Displacement.Function.m_MD5PseudoRandom ) {
		predictionData->m_pCmd->random_seed = ( ( uint32_t( __thiscall* )( uint32_t ) )Engine::Displacement.Function.m_MD5PseudoRandom )( predictionData->m_pCmd->command_number ) & 0x7fffffff;
	}

	predictionData->m_pPlayer->m_flVelocityModifier() = predictionData->m_flVelocityModifier;

	m_pMoveHelper->SetHost( predictionData->m_pPlayer );

	StartCommand( predictionData->m_pPlayer, predictionData->m_pCmd.Xor( ) );

	m_pGlobalVars->curtime = static_cast< float >( predictionData->m_nTickBase ) * m_pGlobalVars->interval_per_tick;
	m_pGlobalVars->frametime = m_pGlobalVars->interval_per_tick;

	// m_bEnginePaused or player frozen
	if( m_pPrediction->m_engine_paused || predictionData->m_pPlayer->m_fFlags( ) & 0x40 ) {
		m_pGlobalVars->frametime = 0.0f;
	}

	m_pPrediction->m_in_prediction = true;
	m_pPrediction->m_is_first_time_predicted = false;

	m_pGameMovement->StartTrackPredictionErrors( predictionData->m_pPlayer );

	// Setup input.
	m_pPrediction->SetupMove( predictionData->m_pPlayer, predictionData->m_pCmd.Xor( ), m_pMoveHelper.Xor( ), &predictionData->m_MoveData );

	//predictionData->m_RestoreData.Setup( predictionData->m_pPlayer );

	m_pGameMovement->ProcessMovement( predictionData->m_pPlayer, &predictionData->m_MoveData );

	m_pPrediction->FinishMove( predictionData->m_pPlayer, predictionData->m_pCmd.Xor( ), &predictionData->m_MoveData );

	if (predictionData->m_pWeapon) 
	{
		predictionData->m_pWeapon->UpdateAccuracyPenalty();

		m_flSpread = predictionData->m_pWeapon->GetSpread();
		m_flInaccuracy = predictionData->m_pWeapon->GetInaccuracy();

		auto is_special_weapon = (predictionData->m_pWeapon->m_iItemDefinitionIndex() == 9
			|| predictionData->m_pWeapon->m_iItemDefinitionIndex() == 11
			|| predictionData->m_pWeapon->m_iItemDefinitionIndex() == 38
			|| predictionData->m_pWeapon->m_iItemDefinitionIndex() == 40);

		auto pweapon_info = Encrypted_t(predictionData->m_pWeaponInfo);

		if (predictionData->m_pPlayer->m_fFlags() & FL_DUCKING)
		{
			if (is_special_weapon)
				m_flPerfectAccuracy = pweapon_info->m_flInaccuracyCrouchAlt;
			else
				m_flPerfectAccuracy = pweapon_info->m_flInaccuracyCrouch;
		}
		else if (is_special_weapon)
			m_flPerfectAccuracy = pweapon_info->m_flInaccuracyStandAlt;
		else
			m_flPerfectAccuracy = pweapon_info->m_flInaccuracyStand;
	}

	m_flWeaponRange = predictionData->m_pWeaponInfo->m_flWeaponRange;
	_cmd->viewangles = oldang;
}

void Prediction::PostEntityThink( C_CSPlayer* player ) {
	static auto PostThinkVPhysics = reinterpret_cast< bool( __thiscall* )( C_BaseEntity* ) >( Memory::Scan( "client.dll", "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 8B D9 56 57 83 BB ? ? ? ? ? 75 50 8B 0D" ) );
	static auto SimulatePlayerSimulatedEntities = reinterpret_cast< void( __thiscall* )( C_BaseEntity* ) >( Memory::Scan( "client.dll", "56 8B F1 57 8B BE ? ? ? ? 83 EF 01 78 72 90 8B 86" ) );

	if( player && !player->IsDead( ) ) {
		using UpdateCollisionBoundsFn = void( __thiscall* )( void* );
		Memory::VCall<UpdateCollisionBoundsFn>( player, 329 )( player );

		if( player->m_fFlags( ) & FL_ONGROUND )
			*reinterpret_cast< uintptr_t* >( uintptr_t( player ) + 0x3004 ) = 0;

		if( *reinterpret_cast< int* >( uintptr_t( player ) + 0x28AC ) == -1 ) {
			using SetSequenceFn = void( __thiscall* )( void*, int );
			Memory::VCall<SetSequenceFn>( player, 213 )( player, 0 );
		}

		using StudioFrameAdvanceFn = void( __thiscall* )( void* );
		Memory::VCall<StudioFrameAdvanceFn>( player, 214 )( player );

		PostThinkVPhysics( player );
	}
	SimulatePlayerSimulatedEntities( player );
}

void Prediction::End( bool* send_packet ) {
	if( !predictionData->m_pCmd.IsValid( ) || !predictionData->m_pPlayer || !predictionData->m_pWeapon )
		return;

	m_pGlobalVars->curtime = predictionData->m_flCurrentTime;
	m_pGlobalVars->frametime = predictionData->m_flFrameTime;

	m_pPrediction->m_in_prediction = m_in_prediction;
	m_pPrediction->m_is_first_time_predicted = m_first_command_pred;

	predictionData->m_pPlayer->m_flVelocityModifier() = predictionData->m_flVelocityModifier;

	m_pGameMovement->FinishTrackPredictionErrors( predictionData->m_pPlayer );

	auto& correct = predictionData->m_CorrectionData.emplace_front( );
	correct.command_nr = predictionData->m_pCmd->command_number;
	correct.tickbase = predictionData->m_pPlayer->m_nTickBase( ) + 1;
	correct.tickbase_shift = 0;
	correct.chokedcommands = m_pClientState->m_nChokedCommands( ) + 1;
	correct.tickcount = m_pGlobalVars->tickcount;

	auto& out = predictionData->m_OutgoingCommands.emplace_back( );
	out.is_outgoing = *send_packet != false;
	out.command_nr = predictionData->m_pCmd->command_number;
	out.is_used = false;
	out.prev_command_nr = 0;

	if( *send_packet ) {
		predictionData->m_ChokedNr.clear( );
	}
	else {
		predictionData->m_ChokedNr.push_back( predictionData->m_pCmd->command_number );
	}

	while( predictionData->m_CorrectionData.size( ) > int( 2.0f / m_pGlobalVars->interval_per_tick ) ) {
		predictionData->m_CorrectionData.pop_back( );
	}

	m_pMoveHelper->SetHost( nullptr );

	// Finish command
	predictionData->m_pPlayer->SetCurrentCommand( nullptr );
	C_BaseEntity::SetPredictionRandomSeed( nullptr );
	C_BaseEntity::SetPredictionPlayer( nullptr );

	//m_pGameMovement->Reset( );

	m_bInPrediction = false;
}

void Prediction::Invalidate( ) {
	predictionData->m_pCmd = nullptr;
	predictionData->m_pPlayer = nullptr;
	predictionData->m_pWeapon = nullptr;
	predictionData->m_pSendPacket = nullptr;
	predictionData->m_OutgoingCommands.clear( );

	predictionData->m_RestoreData.Reset( );
}

void Prediction::RunGamePrediction( ) {
	// force game to repredict data
	/*if( g_Vars.globals.LastVelocityModifier < 1.0f ) {
		// https://github.com/pmrowla/hl2sdk-csgo/blob/49e950f3eb820d88825f75e40f56b3e64790920a/game/client/prediction.cpp#L1533
		*( uint8_t* )( uintptr_t( m_pPrediction.Xor( ) ) + 0x24 ) = 1; // m_bPreviousAckHadErrors
		*( uint32_t* )( uintptr_t( m_pPrediction.Xor( ) ) + 0x1C ) = 0; // m_nCommandsPredicted
	}*/

	if( m_pClientState->m_nDeltaTick( ) > 0 ) {
		m_pPrediction->Update( m_pClientState->m_nDeltaTick( ), true,
			m_pClientState->m_nLastCommandAck( ),
			m_pClientState->m_nLastOutgoingCommand( ) + m_pClientState->m_nChokedCommands( ) );
	}

}

int Prediction::GetFlags( ) {
	return predictionData->m_fFlags;
}

Vector Prediction::GetVelocity( ) {
	return predictionData->m_vecVelocity;
}

Encrypted_t<CUserCmd> Prediction::GetCmd( ) {
	return predictionData->m_pCmd;
}

Encrypted_t<PredictionData> Prediction::GetData( ) {
	return predictionData;
}

float Prediction::GetFrametime( ) {
	return predictionData->m_flFrameTime;
}

float Prediction::GetCurtime( ) {
	return predictionData->m_flCurrentTime;
}

float Prediction::GetSpread( ) {
	return m_flSpread;
}

float Prediction::GetInaccuracy( ) {
	return m_flInaccuracy;
}

float Prediction::GetWeaponRange( ) {
	return m_flWeaponRange;
}

void Prediction::PacketStart( int incoming_sequence, int outgoing_acknowledged ) {
	if( !predictionData->m_bSendingPacket ) {
		Hooked::oPacketStart( ( void* )( uintptr_t( m_pClientState.Xor( ) ) + 8 ),
			incoming_sequence, outgoing_acknowledged );
		return;
	}

	if( m_bFixSendDataGram )
		outgoing_acknowledged = m_pClientState.Xor( )->m_nLastCommandAck( );

	if( !predictionData->m_OutgoingCommands.empty( ) ) {
		for( auto it = predictionData->m_OutgoingCommands.rbegin( ); it != predictionData->m_OutgoingCommands.rend( ); ++it ) {
			if( !it->is_outgoing ) {
				continue;
			}

			if( it->command_nr == outgoing_acknowledged
				|| outgoing_acknowledged > it->command_nr && ( !it->is_used || it->prev_command_nr == outgoing_acknowledged ) ) {
				it->prev_command_nr = outgoing_acknowledged;
				it->is_used = true;
				Hooked::oPacketStart( ( void* )( uintptr_t( m_pClientState.Xor( ) ) + 8 ),
					incoming_sequence, outgoing_acknowledged );
				break;
			}
		}

		auto result = false;
		for( auto it = predictionData->m_OutgoingCommands.begin( ); it != predictionData->m_OutgoingCommands.end( ); ) {
			if( outgoing_acknowledged == it->command_nr || outgoing_acknowledged == it->prev_command_nr )
				result = true;

			if( outgoing_acknowledged > it->command_nr && outgoing_acknowledged > it->prev_command_nr ) {
				it = predictionData->m_OutgoingCommands.erase( it );
			}
			else {
				it++;
			}
		}

		if( !result )
			Hooked::oPacketStart( ( void* )( uintptr_t( m_pClientState.Xor( ) ) + 8 ),
				incoming_sequence, outgoing_acknowledged );
	}
	else {
		Hooked::oPacketStart( ( void* )( uintptr_t( m_pClientState.Xor( ) ) + 8 ),
			incoming_sequence, outgoing_acknowledged );
	}
}

void Prediction::PacketCorrection( uintptr_t cl_state ) {

	auto local = C_CSPlayer::GetLocalPlayer( );
	if( !local || local->IsDead( ) ) {
		predictionData->m_CorrectionData.clear( );
		return;
	}

	// Did we get any messages this tick (i.e., did we call PreEntityPacketReceived)?
	if( *( int* )( uintptr_t( cl_state ) + 0x164 ) == *( int* )( uintptr_t( cl_state ) + 0x16C ) ) {
		//g_Vars.globals.m_flLastVelocityModifier = local->m_flVelocityModifier( );

		if( g_Vars.globals.m_flLastVelocityModifier != local->m_flVelocityModifier( ) )
		{
			if( g_Vars.globals.m_flLastVelocityModifier <= 1.f )
				m_pPrediction->ForceRepredict( true );
		}

		g_Vars.globals.m_flLastVelocityModifier = local->m_flVelocityModifier( );

		//if (g_Vars.globals.m_flLastVelocityModifier > 1.f)
		//	g_Vars.globals.m_flLastVelocityModifier = local->m_flVelocityModifier();
	}
}

void Prediction::KeepCommunication( bool* bSendPacket ) {
	auto local = C_CSPlayer::GetLocalPlayer( );
	if( !local )
		return;

	static bool is_alive = false;

	if( *bSendPacket ) {
		if( m_pGameRules.Xor( ) )
			is_alive = local && !local->IsDead( ) && !( local->m_fFlags( ) & 0x40 ) && !m_pGameRules->m_bFreezePeriod( );

		if( !is_alive ) {
			predictionData->m_bSendingPacket = false;
		}

		return;
	}

	if( !is_alive )
		return;

	predictionData->m_bSendingPacket = true;

	auto _netchannel = m_pEngine->GetNetChannelInfo( );

	if( !_netchannel )
		return;

	if( predictionData->m_OutgoingCommands.size( ) > 0 && _netchannel->m_nChokedPackets >0 ) {
		m_bFixSendDataGram = true;
		const auto choked = _netchannel->m_nChokedPackets;
		_netchannel->m_nChokedPackets = 0;
		_netchannel->SendDatagram( );
		--_netchannel->m_nOutSequenceNr;
		_netchannel->m_nChokedPackets = choked;
	}

	m_bFixSendDataGram = false;
}

void Prediction::StoreNetvarCompression( int command_number )
{
	auto local = C_CSPlayer::GetLocalPlayer( );

	if( !local )
		return;

	// collect data for netvar compression fix
	auto data = &predictionData->m_Data[ command_number % 150 ];

	data->m_nCommandNumber = command_number;
	data->m_aimPunchAngle = local->m_aimPunchAngle( );
	data->m_aimPunchAngleVel = local->m_aimPunchAngleVel( );
	data->m_vecViewOffset = fminf( fmaxf( local->m_vecViewOffset( ).z, 46.f ), 64.f );
	data->m_flVelocityModifier = local->m_flVelocityModifier( );
}

void Prediction::RestoreNetvarCompression( int command_number )
{
	auto local = C_CSPlayer::GetLocalPlayer( );

	if( !local )
		return;

	// restore data for netvar compression fix
	auto data = &predictionData->m_Data[ command_number % 150 ];

	const auto punch_delta = local->m_aimPunchAngle( ) - data->m_aimPunchAngle;
	const auto punch_vel_delta = local->m_aimPunchAngleVel( ) - data->m_aimPunchAngleVel;
	const auto view_offset_delta = local->m_vecViewOffset( ).z - data->m_vecViewOffset;
	const auto velocity_modifier_delta = local->m_flVelocityModifier( ) - data->m_flVelocityModifier;

	if( abs( punch_delta.x ) <= 0.03125f && abs( punch_delta.y ) <= 0.03125f && abs( punch_delta.z ) <= 0.03125f )
		local->m_aimPunchAngle( ) = data->m_aimPunchAngle;

	if( abs( punch_vel_delta.x ) <= 0.03125f && abs( punch_vel_delta.y ) <= 0.03125f && abs( punch_vel_delta.z ) <= 0.03125f )
		local->m_aimPunchAngleVel( ) = data->m_aimPunchAngleVel;

	if( abs( view_offset_delta ) <= 0.03125f )
		local->m_vecViewOffset( ).z = data->m_vecViewOffset;

	if( abs( velocity_modifier_delta ) <= 0.00625f )
		local->m_flVelocityModifier( ) = data->m_flVelocityModifier;

	local->m_vphysicsCollisionState() = 0;
}

void Prediction::CorrectViewModel( ClientFrameStage_t stage ) {
	if( !m_pEngine->IsInGame( ) )
		return;

	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	if( pLocal->IsDead( ) )
		return;

	// this works fine
	// but it could be improved/smoothed out
	// in some situations
	if( pLocal->m_hViewModel( ).IsValid( ) ) {
		const auto pViewmodel = pLocal->m_hViewModel( ).Get( );
		if( pViewmodel ) {
			static DWORD32 m_flCycle = SDK::Memory::FindInDataMap( pViewmodel->GetPredDescMap( ), XorStr( "m_flCycle" ) );
			static DWORD32 m_flAnimTime = SDK::Memory::FindInDataMap( pViewmodel->GetPredDescMap( ), XorStr( "m_flAnimTime" ) );

			// apply stored data
			if( stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START ) {
				*( float* )( uintptr_t( pViewmodel ) + m_flCycle ) = m_flViewmodelCycle;
				*( float* )( uintptr_t( pViewmodel ) + m_flAnimTime ) = m_flViewmodelAnimTime;
			}

			// store data
			m_flViewmodelCycle = *( float* )( uintptr_t( pViewmodel ) + m_flCycle );
			m_flViewmodelAnimTime = *( float* )( uintptr_t( pViewmodel ) + m_flAnimTime );
		}
	}
}

void RestoreData::Setup( C_CSPlayer* player ) {
	this->m_aimPunchAngle = player->m_aimPunchAngle( );
	this->m_aimPunchAngleVel = player->m_aimPunchAngleVel( );
	this->m_viewPunchAngle = player->m_viewPunchAngle( );

	this->m_vecViewOffset = player->m_vecViewOffset( );
	this->m_vecBaseVelocity = player->m_vecBaseVelocity( );
	this->m_vecVelocity = player->m_vecVelocity( );
	this->m_vecAbsVelocity = player->GetAbsVelocity( );
	this->m_vecOrigin = player->m_vecOrigin( );

	this->m_flFallVelocity = player->m_flFallVelocity( );
	this->m_flVelocityModifier = player->m_flVelocityModifier( );
	this->m_flDuckAmount = player->m_flDuckAmount( );
	this->m_flDuckSpeed = player->m_flDuckSpeed( );

	static int playerSurfaceFrictionOffset = Memory::FindInDataMap( player->GetPredDescMap( ), XorStr( "m_surfaceFriction" ) );
	float playerSurfaceFriction = *( float* )( uintptr_t( player ) + playerSurfaceFrictionOffset );

	this->m_surfaceFriction = playerSurfaceFriction;

	static int hGroundEntity = Memory::FindInDataMap( player->GetPredDescMap( ), XorStr( "m_hGroundEntity" ) );
	int playerhGroundEntity = *( int* )( uintptr_t( player ) + hGroundEntity );

	this->m_hGroundEntity = playerhGroundEntity;
	this->m_nMoveType = player->m_MoveType( );
	this->m_nFlags = player->m_fFlags( );
	this->m_nTickBase = player->m_nTickBase( );

	auto weapon = ( C_WeaponCSBaseGun* )player->m_hActiveWeapon( ).Get( );
	if( weapon ) {
		static int fAccuracyPenalty = Memory::FindInDataMap( player->GetPredDescMap( ), XorStr( "m_fAccuracyPenalty" ) );
		float playerfAccuracyPenalty = *( float* )( uintptr_t( weapon ) + fAccuracyPenalty );

		this->m_fAccuracyPenalty = playerfAccuracyPenalty;
		this->m_flRecoilIndex = weapon->m_flRecoilIndex( );
	}

	memcpy( &this->m_MoveData, &g_Prediction.GetData( )->m_MoveData, sizeof( CMoveData ) );

	this->is_filled = true;
}

void RestoreData::Apply( C_CSPlayer* player ) {
	if( !this->is_filled )
		return;

	player->m_aimPunchAngle( ) = this->m_aimPunchAngle;
	player->m_aimPunchAngleVel( ) = this->m_aimPunchAngleVel;
	player->m_viewPunchAngle( ) = this->m_viewPunchAngle;

	player->m_vecViewOffset( ) = this->m_vecViewOffset;
	player->m_vecBaseVelocity( ) = this->m_vecBaseVelocity;
	player->m_vecVelocity( ) = this->m_vecVelocity;
	player->SetAbsVelocity( this->m_vecAbsVelocity );
	player->m_vecOrigin( ) = this->m_vecOrigin;

	player->m_flFallVelocity( ) = this->m_flFallVelocity;
	player->m_flVelocityModifier( ) = this->m_flVelocityModifier;
	player->m_flDuckAmount( ) = this->m_flDuckAmount;
	player->m_flDuckSpeed( ) = this->m_flDuckSpeed;

	static int playerSurfaceFrictionOffset = Memory::FindInDataMap( player->GetPredDescMap( ), XorStr( "m_surfaceFriction" ) );
	float playerSurfaceFriction = *( float* )( uintptr_t( player ) + playerSurfaceFrictionOffset );
	*( float* )( uintptr_t( player ) + playerSurfaceFrictionOffset ) = this->m_surfaceFriction;

	static int hGroundEntity = Memory::FindInDataMap( player->GetPredDescMap( ), XorStr( "m_hGroundEntity" ) );
	*( int* )( uintptr_t( player ) + hGroundEntity ) = this->m_hGroundEntity;

	player->m_MoveType( ) = this->m_nMoveType;
	player->m_fFlags( ) = this->m_nFlags;
	player->m_nTickBase( ) = this->m_nTickBase;

	auto weapon = ( C_WeaponCSBaseGun* )player->m_hActiveWeapon( ).Get( );
	if( weapon ) {
		static int fAccuracyPenalty = Memory::FindInDataMap( player->GetPredDescMap( ), XorStr( "m_fAccuracyPenalty" ) );

		*( float* )( uintptr_t( weapon ) + fAccuracyPenalty ) = this->m_fAccuracyPenalty;
		weapon->m_flRecoilIndex( ) = this->m_flRecoilIndex;
	}

	memcpy( &g_Prediction.GetData( )->m_MoveData, &this->m_MoveData, sizeof( CMoveData ) );

	this->is_filled = false;
}