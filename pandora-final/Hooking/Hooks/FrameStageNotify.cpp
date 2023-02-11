#include "../Hooked.hpp"
#include "../../Features/Rage/EnginePrediction.hpp"
#include "../../SDK/Displacement.hpp"
#include "../../SDK/sdk.hpp"
#include "../../SDK/Classes/Player.hpp"
#include "../../Features/Visuals/EventLogger.hpp"
#include "../../Features/Visuals/GrenadePrediction.hpp"
#include "../../Renderer/Render.hpp"
#include "../../Utils/InputSys.hpp"
#include "../../SDK/Classes/Exploits.hpp"
#include "../../Features/Rage/LagCompensation.hpp"
#include "../../Utils/Threading/threading.h"
#include "../../Features/Rage/AnimationSystem.hpp"
#include "../../Features/Rage/Resolver.hpp"
#include "../../Features/Rage/ShotHandling.hpp"
#include "../../Features/Rage/TickbaseShift.hpp"
#include "../../Features/Visuals/Chams.hpp"
#include "../../Features/Scripting/Scripting.hpp"
#include "../../Features/Rage/ServerAnimations.hpp"
#include "../../Features/Miscellaneous/Miscellaneous.hpp"

void __fastcall Hooked::FrameStageNotify( void* ecx, void* edx, ClientFrameStage_t stage ) {
	g_Vars.globals.szLastHookCalled = XorStr( "9" );

	static auto extend_fakelag_packets = [ ] ( ) -> void
	{
		static bool noob = false;

		if( noob )
			return;

		if( !noob ) {
			static DWORD lol = Memory::Scan( XorStr( "engine.dll" ), XorStr( "55 8B EC A1 ? ? ? ? 81 EC ? ? ? ? B9 ? ? ? ? 53 8B 98" ) ) + 0xBC + 1;

			DWORD old;
			VirtualProtect( ( LPVOID )lol, 1, PAGE_READWRITE, &old );
			*( int* )lol = 62;
			VirtualProtect( ( LPVOID )lol, 1, old, &old );
			noob = true;
		}
	};

	extend_fakelag_packets( );

	if( g_Vars.globals.m_bForceFullUpdate ) {
		m_pClientState->m_nDeltaTick( ) = -1;

		g_Vars.globals.m_bForceFullUpdate = false;

		return oFrameStageNotify( ecx, stage );
	}

	auto pLocal = C_CSPlayer::GetLocalPlayer( );

	static const auto ppGameRulesProxy = *reinterpret_cast< CCSGameRules*** >( Memory::Scan( XorStr( "client.dll" ), XorStr( "8B 0D ?? ?? ?? ?? 85 C0 74 0A 8B 01 FF 50 78 83 C0 54" ) ) + 2 );
	if( *ppGameRulesProxy ) {
		m_pGameRules = *ppGameRulesProxy;
	}

#if defined(LUA_SCRIPTING)
	if( !m_pEngine->IsDrawingLoadingImage( ) && pLocal ) {
		Scripting::Script::DoCallback( hash_32_fnv1a_const( "framestage" ), stage );
	}
#endif

	// cache random values cuz valve random system cause performance issues
	if( !g_Vars.globals.m_bInitRandomSeed ) {
		for( auto i = 0; i <= 255; i++ ) {
			RandomSeed( i + 1 );

			g_Vars.globals.SpreadRandom[ i ].flRand1 = RandomFloat( 0.0f, 1.0f );
			g_Vars.globals.SpreadRandom[ i ].flRandPi1 = RandomFloat( 0.0f, 6.2831855f );
			g_Vars.globals.SpreadRandom[ i ].flRand2 = RandomFloat( 0.0f, 1.0f );
			g_Vars.globals.SpreadRandom[ i ].flRandPi2 = RandomFloat( 0.0f, 6.2831855f );
		}

		g_Vars.globals.m_bInitRandomSeed = true;
	}

	if( stage == FRAME_RENDER_START ) {
		g_Misc.RemoveSmoke( );
		g_Misc.Modulation( );

		static bool bSetClantag = false;

		static auto fnClantagChanged = ( int( __fastcall* )( const char*, const char* ) ) Engine::Displacement.Function.m_uClanTagChange;
		if( g_Vars.misc.clantag_changer ) {
			static int prev_time = ( int )m_pGlobalVars->curtime;

			if( prev_time != ( int )m_pGlobalVars->curtime ) {
				fnClantagChanged( XorStr( "falschkopf" ), XorStr( "falschkopf" ) );
				prev_time = ( int )m_pGlobalVars->curtime;
			}

			bSetClantag = true;
		}
		else {
			if( bSetClantag ) {
				fnClantagChanged( XorStr( "" ), XorStr( "" ) );
				bSetClantag = false;
			}
		}

		if( m_pEngine->IsConnected( ) ) {
			if( pLocal ) {
				g_ShotHandling.ProcessShots( );

				if( g_Vars.esp.remove_smoke )
					*( int* )Engine::Displacement.Data.m_uSmokeCount = 0;

				auto aim_punch = pLocal->m_aimPunchAngle( ) * g_Vars.weapon_recoil_scale->GetFloat( ) * g_Vars.view_recoil_tracking->GetFloat( );
				if( g_Vars.esp.remove_recoil_shake )
					pLocal->pl( ).v_angle -= pLocal->m_viewPunchAngle( );

				if( g_Vars.esp.remove_recoil_punch )
					pLocal->pl( ).v_angle -= aim_punch;

				pLocal->pl( ).v_angle.Normalize( );

				if( g_Vars.esp.remove_flash ) {
					pLocal->m_flFlashDuration( ) = 0.f;
				}
			}
		}
	}

	if( stage == FRAME_RENDER_END ) {
		if( pLocal ) {
			g_Vars.globals.RenderIsReady = true;
		}
	}

	g_Prediction.CorrectViewModel( stage );

	oFrameStageNotify( ecx, stage );

	if( stage == FRAME_NET_UPDATE_END ) {
		if( m_pEngine->IsConnected( ) ) {
			g_AnimationSystem.FrameStage( );

			// finish up the rest of the queued bonesetups
			// we need this data immediately, always
			// Threading::FinishQueue( true );

			g_LagCompensation.Update( );
		}

		Hooked::CL_FireEvents( );
	}
}