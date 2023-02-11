#include "../Hooked.hpp"
#include "../../Features/Rage/EnginePrediction.hpp"
#include "../../SDK/Classes/weapon.hpp"
#include "../../SDK/Valve/CBaseHandle.hpp"
#include "../../SDK/Classes/Player.hpp"
#include "../../SDK/Displacement.hpp"
#include <deque>
#include "../../Features/Rage/TickbaseShift.hpp"

void __fastcall Hooked::RunCommand( void* ecx, void* edx, C_CSPlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper ) {
	g_Vars.globals.szLastHookCalled = XorStr( "32" );
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal || !player || player != pLocal || !ucmd ) {
		oRunCommand( ecx, player, ucmd, moveHelper );
		return;
	}

	// out of range?
	if( !( m_pGlobalVars->tickcount + 64 + g_Vars.sv_max_usercmd_future_ticks->GetInt( ) > ucmd->tick_count ) ) {
		ucmd->hasbeenpredicted = true;
		return;
	}

	// in some cases
	// tickbase seems to be
	// 1 tick ahead basically
	if( g_TickbaseController.m_bFixCharge && !g_TickbaseController.m_bFixedCharge ) {
		player->m_nTickBase( ) += g_TickbaseController.m_iMaxProcessTicks;

		m_pGlobalVars->curtime = float( player->m_nTickBase( ) ) * m_pGlobalVars->interval_per_tick;

		g_TickbaseController.m_bFixCharge = false;
		g_TickbaseController.m_bFixedCharge = true;
	}

	if( g_TickbaseController.m_bPreFix && g_TickbaseController.m_iFixCommand == ucmd->command_number ) {
		//printf( "called pre fix | command to fix %i | command number: %i | tickbase %i", g_TickbaseController.m_iFixCommand, ucmd->command_number, player->m_nTickBase( ) );

		player->m_nTickBase( ) -= g_TickbaseController.m_iFixAmount;
		//printf( " | fixed tickbase %i\n", player->m_nTickBase( ) );

		m_pGlobalVars->curtime = float( player->m_nTickBase( ) ) * m_pGlobalVars->interval_per_tick;

		g_TickbaseController.m_bPreFix = false;
	}

	if( g_TickbaseController.m_bPostFix && ( g_TickbaseController.m_iFixCommand + 2 ) == ucmd->command_number ) {
		//printf( "called post fix | command to fix %i | command number: %i | tickbase %i", g_TickbaseController.m_iFixCommand + 1, ucmd->command_number, player->m_nTickBase( ) );
		player->m_nTickBase( ) += g_TickbaseController.m_iFixAmount;
		//printf( " | fixed tickbase %i\n", player->m_nTickBase( ) );

		m_pGlobalVars->curtime = float( player->m_nTickBase( ) ) * m_pGlobalVars->interval_per_tick;

		g_TickbaseController.m_bPostFix = false;
	}

	static int nTickbaseRecords[ 150 ] = { };
	static bool bInAttackRecords[ 150 ] = { };
	static bool bCanShootRecords[ 150 ] = { };

	nTickbaseRecords[ ucmd->command_number % 150 ] = player->m_nTickBase( );
	bInAttackRecords[ ucmd->command_number % 150 ] = ( ucmd->buttons & ( IN_ATTACK2 | IN_ATTACK ) ) != 0;
	bCanShootRecords[ ucmd->command_number % 150 ] = player->CanShoot( true );

	// sure let it stay, whatever
	auto FixPostponeTime = [ player ] ( int command_number ) {
		auto weapon = ( C_WeaponCSBaseGun* )player->m_hActiveWeapon( ).Get( );
		if( weapon ) {
			auto postpone = FLT_MAX;
			if( weapon->m_iItemDefinitionIndex( ) == WEAPON_REVOLVER ) {
				auto tick_rate = int( 1.0f / m_pGlobalVars->interval_per_tick );
				if( tick_rate >> 1 > 1 ) {
					auto cmd_nr = command_number - 1;
					auto shoot_nr = 0;
					for( int i = 1; i < tick_rate >> 1; ++i ) {
						shoot_nr = cmd_nr;
						if( !bInAttackRecords[ cmd_nr % 150 ] || !bCanShootRecords[ cmd_nr % 150 ] )
							break;

						--cmd_nr;
					}

					if( shoot_nr ) {
						auto tick = 1 - ( signed int )( float )( -0.03348f / m_pGlobalVars->interval_per_tick );
						if( command_number - shoot_nr >= tick )
							postpone = TICKS_TO_TIME( nTickbaseRecords[ ( tick + shoot_nr ) % 150 ] ) + 0.2f;
					}
				}
			}

			weapon->m_flPostponeFireReadyTime( ) = postpone;
		}
	};

	auto impactBackup = g_Vars.sv_show_impacts->GetInt( );
	if( g_Vars.esp.server_impacts ) {
		g_Vars.sv_show_impacts->SetValue( 2 );
	}

	g_Prediction.RestoreNetvarCompression(ucmd->command_number-1);

	FixPostponeTime( ucmd->command_number );
	oRunCommand( ecx, player, ucmd, moveHelper );
	FixPostponeTime( ucmd->command_number );

	player->m_vphysicsCollisionState() = 0;
	g_Prediction.StoreNetvarCompression(ucmd->command_number);

	if( g_Vars.esp.server_impacts ) {
		g_Vars.sv_show_impacts->SetValue( impactBackup );
	}
}
