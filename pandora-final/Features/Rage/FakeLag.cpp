#pragma once
#include "FakeLag.hpp"
#include "../Miscellaneous/Movement.hpp"

#include "../../SDK/Classes/Player.hpp"
#include "../../SDK/Classes/weapon.hpp"
#include "../Rage/TickbaseShift.hpp"
#include "EnginePrediction.hpp"
FakeLag g_FakeLag;

bool FakeLag::ShouldFakeLag( CUserCmd* pCmd ) {
	auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return false;

	if( pLocal->IsDead( ) )
		return false;

	bool bReturnValue = false;

	if( g_Vars.rage.fake_lag_standing ) {
		if( !g_Movement.PressingMovementKeys( pCmd ) && pLocal->m_vecVelocity( ).Length2D( ) <= 0.1f && pLocal->m_fFlags( ) & FL_ONGROUND )
			bReturnValue = true;
	}

	if( g_Vars.rage.fake_lag_moving ) {
		if( ( g_Movement.PressingMovementKeys( pCmd ) || pLocal->m_vecVelocity( ).Length2D( ) > 0.1f ) && pLocal->m_fFlags( ) & FL_ONGROUND )
			bReturnValue = true;
	}

	if( g_Vars.rage.fake_lag_air ) {
		if( !( pLocal->m_fFlags( ) & FL_ONGROUND ) )
			bReturnValue = true;
	}

	if( g_Vars.rage.fake_lag_unduck ) {
		if( ( !( pCmd->buttons & IN_DUCK ) && pLocal->m_flDuckAmount( ) > 0.0f ) )
			bReturnValue = true;
	}

	return bReturnValue;
}

int FakeLag::DetermineFakeLagAmount( CUserCmd* pCmd ) {
	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return 0;

	const auto ApplyType = [ & ] ( int iLagAmount ) {
		const float extrapolated_speed = pLocal->m_vecVelocity( ).Length( ) * m_pGlobalVars->interval_per_tick;
		switch( g_Vars.rage.fake_lag_type ) {
		case 0: // max
			break;
		case 1: // dyn
			iLagAmount = std::min< int >( static_cast< int >( std::ceilf( 64 / extrapolated_speed ) ), static_cast< int >( iLagAmount ) );
			iLagAmount = std::clamp( iLagAmount, 2, m_iLagLimit );
			break;
		case 2: // fluc
			if( pCmd->tick_count % 40 < 20 ) {
				iLagAmount = iLagAmount;
			}
			else {
				iLagAmount = 2;
			}
			break;
		}

		return iLagAmount;
	};

	// skeet.cc
	int variance = 0;
	if( g_Vars.rage.fake_lag_variance > 0.f && !g_Vars.globals.m_bFakeWalking ) {
		variance = int( float( g_Vars.rage.fake_lag_variance * 0.01f ) * float( m_iLagLimit ) );
	}

	auto ApplyVariance = [ this ] ( int variance, int nFakelagAmount ) {
		if( variance > 0 && nFakelagAmount > 0 ) {
			auto max = Math::Clamp( variance, 0, ( m_iLagLimit )-nFakelagAmount );
			auto min = Math::Clamp( -variance, -nFakelagAmount, 0 );

			nFakelagAmount += RandomInt( min, max );

			if( nFakelagAmount == m_iLastChokedCommands ) {
				if( nFakelagAmount >= ( m_iLagLimit ) || nFakelagAmount > 2 && !RandomInt( 0, 1 ) ) {
					--nFakelagAmount;
				}
				else {
					++nFakelagAmount;
				}
			}
		}

		return nFakelagAmount;
	};

	int iLagAmount = ApplyVariance( variance, g_Vars.rage.fake_lag_amount );

	return m_iOverrideLagAmount != -1 ? m_iOverrideLagAmount : ApplyType( iLagAmount );
}

void FakeLag::HandleFakeLag( bool* bSendPacket, CUserCmd* pCmd ) {
	if( g_TickbaseController.m_bPrepareCharge ) {
		*bSendPacket = true;

		if( !m_pClientState->m_nChokedCommands( ) ) {
			g_TickbaseController.m_bPreparedRecharge = true;

			g_TickbaseController.m_bPrepareCharge = false;
		}

		return;
	}

	if( !g_Vars.rage.fake_lag ) {
		return;
	}

	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	auto pWeapon = reinterpret_cast< C_WeaponCSBaseGun* >( pLocal->m_hActiveWeapon( ).Get( ) );
	if( !pWeapon )
		return;

	auto pWeaponData = pWeapon->GetCSWeaponData( );
	if( !pWeaponData.IsValid( ) )
		return;

	m_iOverrideLagAmount = -1;

	// hehe.
	if( g_Vars.globals.m_bFakeWalking ) {
		return;
	}

	if (g_Vars.rage.double_tap_bind.enabled)
	{
		*bSendPacket = m_pClientState->m_nChokedCommands() >= 1;
		return;
	}

	if( ShouldFakeLag( pCmd ) ) {
		m_iAwaitingChoke = DetermineFakeLagAmount( pCmd );
		*bSendPacket = false;
	}

	const int nInPredGround = pLocal->m_fFlags( ) & FL_ONGROUND;
	const int nPrePredGround = g_Prediction.GetFlags() & FL_ONGROUND;

	// engine prediction is one tick ahead, so we'll be on ground 1 tick before
	// we are on ground normally, and once normal flags are on ground the pred flags
	// will already be in air (also should be 1 tick difference), making us send 1 tick
	// before landing and 1 tick after landing 

	if( nInPredGround != nPrePredGround ) {
		m_iAwaitingChoke = 2;
	}

	if( m_pClientState->m_nChokedCommands( ) >= m_iAwaitingChoke ) {
		*bSendPacket = true;
	}

	if( pWeaponData->m_iWeaponType == WEAPONTYPE_GRENADE ) {
		if( !pWeapon->m_bPinPulled( ) || ( pCmd->buttons & ( IN_ATTACK | IN_ATTACK2 ) ) ) {
			float m_fThrowTime = pWeapon->m_fThrowTime( );
			if( m_fThrowTime > 0.f ) {
				*bSendPacket = true;
				return;
			}
		}
	}
	else {
		if( ( ( pCmd->buttons & IN_ATTACK ) || ( pWeaponData->m_iWeaponType == WEAPONTYPE_KNIFE && pCmd->buttons & IN_ATTACK2 ) ) && pLocal->CanShoot( ) ) {
			// send attack packet when not fakeducking
			if( !g_Vars.globals.m_bFakeWalking ) {
				*bSendPacket = true;
			}
		}
	}

	auto v29 = (pLocal->m_nTickBase() % 100) == 0;

	if (v29)
		*bSendPacket = true;

	// note - michal;
	// this should never really happens, but incase we ever go above
	// the fakelag limit, then force send a packet

	// change from 14 to 16 once we bypass the 14 tick choke limit
	m_iLagLimit = 14;

	if( m_pClientState->m_nChokedCommands( ) >= m_iLagLimit ) {
		*bSendPacket = true;
	}
}