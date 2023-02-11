#include "TickbaseShift.hpp"
#include "../../source.hpp"
#include "../../SDK/Classes/player.hpp"
#include "../../SDK/Classes/weapon.hpp"
#include "../Rage/EnginePrediction.hpp"
#include "../../Libraries/minhook-master/include/MinHook.h"
#include "../../Hooking/Hooked.hpp"
#include "../../SDK/Displacement.hpp"
#include "../../Features/Visuals/EventLogger.hpp"
#include "../Rage/Ragebot.hpp"

TickbaseSystem g_TickbaseController;

void TickbaseSystem::Charge( ) {
	m_bPrepareCharge = m_bCharge = true;
}

void TickbaseSystem::UnCharge( bool forceProcess ) {
	if( !IsCharged( ) )
		return;

	m_bPrepareCharge = m_bCharge = m_bPreparedRecharge = false;
	if( forceProcess ) {
		while( m_iProcessTicks ) {
			Hooked::oCL_Move( m_iProcessTicks == 1, 0.f );

			m_iProcessTicks--;
		}
	}
	else {
		m_iProcessTicks = 0;
	}
}

bool TickbaseSystem::CanCharge( ) {
	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return false;

	if( pLocal->IsDead( ) )
		return false;

	if( ( g_Vars.rage.double_tap_bind.enabled || g_Vars.rage.hide_shots_bind.enabled ) && !g_Vars.globals.m_bFakeWalking ) {
		// we want to automatically recharge
		if( fabsf( m_pGlobalVars->realtime - m_flLastExploitTime ) > 0.5f ) {
			Charge( );
		}
	}
	else {
		// not enabled?
		// we prolly want to uncharge
		UnCharge( true );
	}

	if( !m_bCharge || !m_bPreparedRecharge )
		return false;

	return true;
}

bool TickbaseSystem::IncrementProcessTicks( ) {
	if( !CanCharge( ) )
		return false;

	if( IsCharged( ) ) {
		m_bPrepareCharge = m_bCharge = m_bPreparedRecharge = false;

		return false;
	}

	// tell the controller to fix tickbase
	// after we done charging
	m_bFixedCharge = false;

	++m_iProcessTicks;

	return m_iProcessTicks <= m_iMaxProcessTicks;
}

bool TickbaseSystem::IsCharged( ) {
	return m_iProcessTicks >= m_iMaxProcessTicks;
}

void TickbaseSystem::CL_Move( bool bFinalTick, float accumulated_extra_samples ) {
	if( !g_Vars.rage.double_tap_bind.enabled ) {
		UnCharge( );
		return Hooked::oCL_Move( bFinalTick, accumulated_extra_samples );
	}

	if( IncrementProcessTicks( ) ) {
		return;
	}

	if( IsCharged( ) && !m_bFixedCharge ) {
		m_bFixCharge = true;
	}

	Hooked::oCL_Move( bFinalTick, accumulated_extra_samples );
}

void TickbaseSystem::RunExploits( bool* bSendPacket, CUserCmd* pCmd ) {
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer( );

	if( !pLocal )
		return;

	const auto pWeapon = reinterpret_cast< C_WeaponCSBaseGun* >( pLocal->m_hActiveWeapon( ).Get( ) );
	if( !pWeapon )
		return;

	auto pWeaponData = pWeapon->GetCSWeaponData( );
	if( !pWeaponData.IsValid( ) )
		return;

	if( !IsCharged( ) ) {
		m_bTapShot = false;
		return;
	}

	if( pWeaponData->m_iWeaponType == WEAPONTYPE_C4 ||
		/*pWeaponData->m_iWeaponType == WEAPONTYPE_KNIFE ||*/
		pWeaponData->m_iWeaponType == WEAPONTYPE_GRENADE ) {
		return;
	}

	if ((pCmd->buttons & IN_ATTACK) != 0 && pLocal->CanShoot() || g_Ragebot.last_shot_command == pCmd->command_number)
	{
		DoTheTapping(pCmd);
		m_flLastExploitTime = m_pGlobalVars->realtime;
		*bSendPacket = true;
		m_iProcessTicks = 0;
	}
	else
	{
		if (*bSendPacket) {
			auto break_lc_shift = m_iMaxProcessTicks - 1 - m_pClientState->m_nChokedCommands();

			auto v26 = pCmd->command_number;
			if (break_lc_shift > 0)
			{
				auto v32 = break_lc_shift;
				do
				{
					auto v27 = &m_pInput->m_pCommands[(++v26 % 150)];
					if (v27 != pCmd)
						memcpy(v27, pCmd, sizeof(CUserCmd));
					v27->command_number = v26;
					v27->tick_count = 0x7F7FFFFF;
					auto v28 = pCmd->buttons & (IN_BULLRUSH | IN_SPEED | IN_DUCK);
					v27->hasbeenpredicted = 1;
					v27->buttons |= v28;
					auto v29 = &m_pInput->m_pVerifiedCommands[v26 % 150];
					memcpy(&v29->m_cmd, v27, 0x64);
					v29->m_crc = v27->GetChecksum();
					++m_pClientState->m_nChokedCommands();

					//feature::local_player->update(v27, send_packet);
					--v32;
				} while (v32);
			}

			*bSendPacket = true;

		}
	}

	//if( pCmd->buttons & IN_ATTACK ) {
	//	if( !m_bTapShot && !m_iShiftAmount && !m_bCMFix && pLocal->CanShoot( ) ) {
	//		if( DoTheTapping( pCmd ) ) {
	//			m_bCMFix = m_bTapShot = true;
	//		}

	//		//DoRegularShifting( 12 );
	//	}
	//}
	//else {
	//	m_bTapShot = false;
	//}
}

void TickbaseSystem::DoRegularShifting( int iAmount ) {
	if( iAmount >= 10 ) {
		m_bTapShot = true;
	}

	m_iFixAmount = m_iShiftAmount = iAmount;
	m_bPreFix = m_bPostFix = true;
	m_bCMFix = true;
}

bool TickbaseSystem::DoTheTapping( CUserCmd* pFrom ) {
	if( g_Vars.globals.m_bFakeWalking )
		return false;

	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer( );

	if( !pLocal )
		return false;

	C_WeaponCSBaseGun* pWeapon = ( C_WeaponCSBaseGun* )pLocal->m_hActiveWeapon( ).Get( );

	if( !pWeapon )
		return false;

	const int nCommandNumber = pFrom->command_number + 1;
	CUserCmd* pCmd = &m_pInput->m_pCommands[ nCommandNumber % 150 ];
	if( !pCmd )
		return false;

	INetChannel* pNetChannel = m_pEngine->GetNetChannelInfo( );

	if( !pCmd || !pNetChannel )
		return false;

	memcpy( pCmd, pFrom, 0x64 );

	pCmd->command_number = nCommandNumber;
	pCmd->buttons &= ~0x801u;

	//// tell runcommand to do a meme fix
	//m_bPreFix = true;

	//// tell runcommand to not do a meme fix
	//m_bPostFix = false;

	//// tell other stuff that we are going to shift/in shift
	//m_bInShift = true;

	const auto tickbase = pLocal->m_nTickBase();

	// how to shift tickbase..
	for( int i = 0; i < (m_iProcessTicks - 1 - m_pClientState->m_nChokedCommands()); i++ ) {
		const int nNewCommandNumber = i + pCmd->command_number;
		CUserCmd* pNewCommand = &m_pInput->m_pCommands[ nNewCommandNumber % MULTIPLAYER_BACKUP ];
		if( !pNewCommand )
			continue;

		CVerifiedUserCmd* pVerify = &m_pInput->m_pVerifiedCommands[ nNewCommandNumber % MULTIPLAYER_BACKUP ];
		if (!pVerify )
			continue;

		memcpy( pNewCommand, pCmd, 0x64 );

		pNewCommand->hasbeenpredicted = true;

		pLocal->m_nTickBase() = tickbase + 1 + i;

		m_pPrediction->Update(m_pClientState->m_nDeltaTick(), true,
			m_pClientState->m_nLastCommandAck(),
			nNewCommandNumber);

		pNewCommand->command_number = nNewCommandNumber;
		pNewCommand->hasbeenpredicted = pNewCommand->tick_count == 0x7F7FFFFF;

		pNewCommand->forwardmove = 0;
		pNewCommand->sidemove = 0;

		/*Vector vecVelocity = pLocal->m_vecVelocity( );
		float flMaxSpeed = 250.0f;
		auto pWeaponData = pWeapon->GetCSWeaponData( );
		if( pWeaponData.IsValid( ) ) {
			flMaxSpeed = pWeapon->m_zoomLevel( ) == 0 ? pWeaponData->m_flMaxSpeed : pWeaponData->m_flMaxSpeed2;
		}

		if( i >= ( 4 * ( ( flMaxSpeed * 0.33333334f ) > vecVelocity.Length( ) ) + 7 ) ) {
			float flMinForwardMove = fminf( 450.0, pNewCommand->forwardmove );
			if( flMinForwardMove <= -450.0 )
				pNewCommand->forwardmove = -450.0;
			else
				pNewCommand->forwardmove = flMinForwardMove;

			float flMinSideMove = fminf( 450.0, pNewCommand->sidemove );
			if( flMinSideMove <= -450.0 )
				pNewCommand->sidemove = -450.0;
			else
				pNewCommand->sidemove = flMinSideMove;
		}*/

		pNewCommand->viewangles.Clamp( );

		memcpy( &pVerify->m_cmd, pNewCommand, 0x64 );

		pVerify->m_crc = pNewCommand->GetChecksum( );

		++m_pClientState->m_nChokedCommands( );
		++pNetChannel->m_nChokedPackets;
		++pNetChannel->m_nOutSequenceNr;
	}

	pLocal->m_nTickBase() = tickbase; 

	// tell our controller how much to fix
	//m_iFixAmount = m_iMaxProcessTicks;

	// tell createmove to setup stuff for fixing

	*( DWORD* )( m_pPrediction.Xor( ) + 0xC ) = -1;
	*( DWORD* )( m_pPrediction.Xor( ) + 0x1C ) = 0;

	// we are not shifting anymore
	m_bInShift = false;

	return true;
}
