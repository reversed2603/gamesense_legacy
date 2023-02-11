#include "GameEvent.hpp"
#include "../Visuals/EventLogger.hpp"
#include "../../source.hpp"
#include "../../Utils/extern/FnvHash.hpp"
#include "../../SDK/Classes/Player.hpp"
#include "../Rage/LagCompensation.hpp"
#include <sstream>
#include "../../SDK/core.hpp"
#include "../Visuals/Chams.hpp"
#include "../Rage/Resolver.hpp"
#include "../Rage/ShotHandling.hpp"
#pragma comment(lib,"Winmm.lib")
#include "../Rage/TickbaseShift.hpp"
#include "../Visuals/Hitmarker.hpp"
#include "../Visuals/Visuals.hpp"
#include <fstream>
#include "../Visuals/BulletTracers.hpp"
#include "../Scripting/Scripting.hpp"
#include "../../Loader/Security/Security.hpp"
#include "SkinChanger.hpp"

#define ADD_GAMEEVENT(n)  m_pGameEvent->AddListener(this, XorStr(#n), false)

GameEvent g_GameEvent;

void GameEvent::Register( ) {
	ADD_GAMEEVENT( team_info );
	ADD_GAMEEVENT( team_score );
	ADD_GAMEEVENT( teamplay_broadcast_audio );
	ADD_GAMEEVENT( player_team );
	ADD_GAMEEVENT( player_class );
	ADD_GAMEEVENT( player_death );
	ADD_GAMEEVENT( player_say );
	ADD_GAMEEVENT( player_chat );
	ADD_GAMEEVENT( player_score );
	ADD_GAMEEVENT( player_spawn );
	ADD_GAMEEVENT( player_shoot );
	ADD_GAMEEVENT( player_use );
	ADD_GAMEEVENT( player_changename );
	ADD_GAMEEVENT( player_hintmessage );
	//ADD_GAMEEVENT( base_player_teleported );
	ADD_GAMEEVENT( game_init );
	ADD_GAMEEVENT( game_newmap );
	ADD_GAMEEVENT( game_start );
	ADD_GAMEEVENT( game_end );
	ADD_GAMEEVENT( round_start );
	ADD_GAMEEVENT( round_end );
	ADD_GAMEEVENT( game_message );
	ADD_GAMEEVENT( break_breakable );
	ADD_GAMEEVENT( break_prop );
	ADD_GAMEEVENT( entity_killed );
	ADD_GAMEEVENT( bonus_updated );
	ADD_GAMEEVENT( achievement_event );
	ADD_GAMEEVENT( achievement_increment );
	ADD_GAMEEVENT( physgun_pickup );
	ADD_GAMEEVENT( flare_ignite_npc );
	ADD_GAMEEVENT( helicopter_grenade_punt_miss );
	ADD_GAMEEVENT( user_data_downloaded );
	ADD_GAMEEVENT( ragdoll_dissolved );
	ADD_GAMEEVENT( hltv_changed_mode );
	ADD_GAMEEVENT( hltv_changed_target );
	ADD_GAMEEVENT( vote_ended );
	ADD_GAMEEVENT( vote_started );
	ADD_GAMEEVENT( vote_changed );
	ADD_GAMEEVENT( vote_passed );
	ADD_GAMEEVENT( vote_failed );
	ADD_GAMEEVENT( vote_cast );
	ADD_GAMEEVENT( vote_options );
	//ADD_GAMEEVENT( replay_saved );
	//ADD_GAMEEVENT( entered_performance_mode );
	//ADD_GAMEEVENT( browse_replays );
	//ADD_GAMEEVENT( replay_youtube_stats );
	ADD_GAMEEVENT( inventory_updated );
	ADD_GAMEEVENT( cart_updated );
	ADD_GAMEEVENT( store_pricesheet_updated );
	ADD_GAMEEVENT( gc_connected );
	ADD_GAMEEVENT( item_schema_initialized );
	ADD_GAMEEVENT( weapon_fire );
	ADD_GAMEEVENT( player_hurt );
	ADD_GAMEEVENT( player_connect_full );
	ADD_GAMEEVENT( item_purchase );
	ADD_GAMEEVENT( bullet_impact );
	ADD_GAMEEVENT( bomb_planted );
	ADD_GAMEEVENT( player_disconnect );
	ADD_GAMEEVENT( other_death );
	ADD_GAMEEVENT( bomb_beginplant );
	ADD_GAMEEVENT( bomb_abortplant );
	ADD_GAMEEVENT( bomb_defused );
	ADD_GAMEEVENT( bomb_exploded );
	ADD_GAMEEVENT( bomb_dropped );
	ADD_GAMEEVENT( bomb_pickup );
	ADD_GAMEEVENT( defuser_dropped );
	ADD_GAMEEVENT( defuser_pickup );
	ADD_GAMEEVENT( announce_phase_end );
	ADD_GAMEEVENT( cs_intermission );
	ADD_GAMEEVENT( bomb_begindefuse );
	ADD_GAMEEVENT( bomb_abortdefuse );
	ADD_GAMEEVENT( hostage_follows );
	ADD_GAMEEVENT( hostage_hurt );
	ADD_GAMEEVENT( hostage_killed );
	ADD_GAMEEVENT( hostage_rescued );
	ADD_GAMEEVENT( hostage_stops_following );
	ADD_GAMEEVENT( hostage_rescued_all );
	ADD_GAMEEVENT( hostage_call_for_help );
	ADD_GAMEEVENT( vip_escaped );
	ADD_GAMEEVENT( vip_killed );
	ADD_GAMEEVENT( player_radio );
	ADD_GAMEEVENT( bomb_beep );
	ADD_GAMEEVENT( weapon_fire_on_empty );
	ADD_GAMEEVENT( grenade_thrown );
	ADD_GAMEEVENT( weapon_outofammo );
	ADD_GAMEEVENT( weapon_reload );
	ADD_GAMEEVENT( weapon_zoom );
	ADD_GAMEEVENT( silencer_detach );
	ADD_GAMEEVENT( inspect_weapon );
	ADD_GAMEEVENT( weapon_zoom_rifle );
	ADD_GAMEEVENT( player_spawned );
	ADD_GAMEEVENT( item_pickup );
	ADD_GAMEEVENT( item_pickup_failed );
	ADD_GAMEEVENT( item_remove );
	ADD_GAMEEVENT( ammo_pickup );
	ADD_GAMEEVENT( item_equip );
	ADD_GAMEEVENT( enter_buyzone );
	ADD_GAMEEVENT( exit_buyzone );
	ADD_GAMEEVENT( buytime_ended );
	ADD_GAMEEVENT( enter_bombzone );
	ADD_GAMEEVENT( exit_bombzone );
	ADD_GAMEEVENT( enter_rescue_zone );
	ADD_GAMEEVENT( exit_rescue_zone );
	ADD_GAMEEVENT( silencer_off );
	ADD_GAMEEVENT( silencer_on );
	ADD_GAMEEVENT( buymenu_open );
	ADD_GAMEEVENT( buymenu_close );
	ADD_GAMEEVENT( round_prestart );
	ADD_GAMEEVENT( round_poststart );
	ADD_GAMEEVENT( grenade_bounce );
	ADD_GAMEEVENT( hegrenade_detonate );
	ADD_GAMEEVENT( flashbang_detonate );
	ADD_GAMEEVENT( smokegrenade_detonate );
	ADD_GAMEEVENT( smokegrenade_expired );
	ADD_GAMEEVENT( molotov_detonate );
	ADD_GAMEEVENT( decoy_detonate );
	ADD_GAMEEVENT( decoy_started );
	ADD_GAMEEVENT( tagrenade_detonate );
	ADD_GAMEEVENT( inferno_startburn );
	ADD_GAMEEVENT( inferno_expire );
	ADD_GAMEEVENT( inferno_extinguish );
	ADD_GAMEEVENT( decoy_firing );
	ADD_GAMEEVENT( player_footstep );
	ADD_GAMEEVENT( player_jump );
	ADD_GAMEEVENT( player_blind );
	ADD_GAMEEVENT( player_falldamage );
	ADD_GAMEEVENT( door_moving );
	ADD_GAMEEVENT( round_freeze_end );
	ADD_GAMEEVENT( mb_input_lock_success );
	ADD_GAMEEVENT( mb_input_lock_cancel );
	ADD_GAMEEVENT( nav_blocked );
	ADD_GAMEEVENT( nav_generate );
	ADD_GAMEEVENT( player_stats_updated );
	ADD_GAMEEVENT( achievement_info_loaded );
	ADD_GAMEEVENT( spec_target_updated );
	ADD_GAMEEVENT( spec_mode_updated );
	ADD_GAMEEVENT( cs_game_disconnected );
	ADD_GAMEEVENT( cs_win_panel_round );
	ADD_GAMEEVENT( cs_win_panel_match );
	ADD_GAMEEVENT( cs_match_end_restart );
	ADD_GAMEEVENT( cs_pre_restart );
	ADD_GAMEEVENT( show_freezepanel );
	ADD_GAMEEVENT( hide_freezepanel );
	ADD_GAMEEVENT( freezecam_started );
	ADD_GAMEEVENT( player_avenged_teammate );
	ADD_GAMEEVENT( achievement_earned );
	ADD_GAMEEVENT( achievement_earned_local );
	ADD_GAMEEVENT( item_found );
	ADD_GAMEEVENT( items_gifted );
	ADD_GAMEEVENT( repost_xbox_achievements );
	ADD_GAMEEVENT( match_end_conditions );
	ADD_GAMEEVENT( round_mvp );
	ADD_GAMEEVENT( player_decal );
	ADD_GAMEEVENT( teamplay_round_start );
	ADD_GAMEEVENT( client_disconnect );
	ADD_GAMEEVENT( gg_player_levelup );
	ADD_GAMEEVENT( ggtr_player_levelup );
	ADD_GAMEEVENT( assassination_target_killed );
	ADD_GAMEEVENT( ggprogressive_player_levelup );
	ADD_GAMEEVENT( gg_killed_enemy );
	ADD_GAMEEVENT( gg_final_weapon_achieved );
	ADD_GAMEEVENT( gg_bonus_grenade_achieved );
	ADD_GAMEEVENT( switch_team );
	ADD_GAMEEVENT( gg_leader );
	ADD_GAMEEVENT( gg_team_leader );
	ADD_GAMEEVENT( gg_player_impending_upgrade );
	ADD_GAMEEVENT( write_profile_data );
	ADD_GAMEEVENT( trial_time_expired );
	ADD_GAMEEVENT( update_matchmaking_stats );
	ADD_GAMEEVENT( player_reset_vote );
	ADD_GAMEEVENT( enable_restart_voting );
	ADD_GAMEEVENT( sfuievent );
	ADD_GAMEEVENT( start_vote );
	ADD_GAMEEVENT( player_given_c4 );
	ADD_GAMEEVENT( player_become_ghost );
	ADD_GAMEEVENT( gg_reset_round_start_sounds );
	ADD_GAMEEVENT( tr_player_flashbanged );
	//ADD_GAMEEVENT( tr_highlight_ammo );
	ADD_GAMEEVENT( tr_mark_complete );
	ADD_GAMEEVENT( tr_mark_best_time );
	ADD_GAMEEVENT( tr_exit_hint_trigger );
	ADD_GAMEEVENT( bot_takeover );
	ADD_GAMEEVENT( tr_show_finish_msgbox );
	ADD_GAMEEVENT( tr_show_exit_msgbox );
	ADD_GAMEEVENT( reset_player_controls );
	ADD_GAMEEVENT( jointeam_failed );
	ADD_GAMEEVENT( teamchange_pending );
	ADD_GAMEEVENT( material_default_complete );
	ADD_GAMEEVENT( cs_prev_next_spectator );
	ADD_GAMEEVENT( cs_handle_ime_event );
	ADD_GAMEEVENT( nextlevel_changed );
	ADD_GAMEEVENT( seasoncoin_levelup );
	ADD_GAMEEVENT( tournament_reward );
	ADD_GAMEEVENT( start_halftime );
	ADD_GAMEEVENT( ammo_refill );
	ADD_GAMEEVENT( parachute_pickup );
	ADD_GAMEEVENT( parachute_deploy );
	ADD_GAMEEVENT( dronegun_attack );
	ADD_GAMEEVENT( drone_dispatched );
	ADD_GAMEEVENT( loot_crate_visible );
	ADD_GAMEEVENT( loot_crate_opened );
	ADD_GAMEEVENT( open_crate_instr );
	ADD_GAMEEVENT( smoke_beacon_paradrop );
	ADD_GAMEEVENT( drone_cargo_detached );
	ADD_GAMEEVENT( choppers_incoming_warning );
	ADD_GAMEEVENT( firstbombs_incoming_warning );
	ADD_GAMEEVENT( dz_item_interaction );
	ADD_GAMEEVENT( snowball_hit_player_face );
}

void GameEvent::Shutdown( ) {
	m_pGameEvent->RemoveListener( this );
}

void GameEvent::FireGameEvent( IGameEvent* pEvent ) {
	if( !pEvent )
		return;

	auto event_hash = hash_32_fnv1a( pEvent->GetName( ) );

	g_Vars.globals.szLastHookCalled = XorStr( "g_e" );

#if defined(LUA_SCRIPTING)
	if( !m_pEngine->IsDrawingLoadingImage( ) && event_hash ) {
		Scripting::Script::DoCallback( event_hash, pEvent );
	}
#endif

	g_ShotHandling.GameEvent( pEvent );

	auto HitgroupToString = [ ] ( int hitgroup ) -> std::string {
		switch( hitgroup ) {
		case Hitgroup_Generic:
			return XorStr( "generic" );
		case Hitgroup_Head:
			return XorStr( "head" );
		case Hitgroup_Chest:
			return XorStr( "chest" );
		case Hitgroup_Stomach:
			return XorStr( "stomach" );
		case Hitgroup_LeftArm:
			return XorStr( "left arm" );
		case Hitgroup_RightArm:
			return XorStr( "right arm" );
		case Hitgroup_LeftLeg:
			return XorStr( "left leg" );
		case Hitgroup_RightLeg:
			return XorStr( "right leg" );
		case Hitgroup_Neck:
			return XorStr( "neck" );
		}
		return XorStr( "generic" );
	};

	g_Hitmarker.GameEvent( pEvent );

	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal || !m_pEngine->IsInGame( ) )
		return;

	static auto sv_showimpacts_time = m_pCvar->FindVar( XorStr( "sv_showimpacts_time" ) );

	switch( event_hash ) {
		// todo: render visualize sounds beams on player_blind
	case hash_32_fnv1a_const( "bullet_impact" ): {
		const auto userid = pEvent->GetInt( XorStr( "userid" ) );
		const auto user_index = m_pEngine->GetPlayerForUserID( userid );

		const auto vecImpact = Vector( pEvent->GetFloat( XorStr( "x" ) ), pEvent->GetFloat( XorStr( "y" ) ), pEvent->GetFloat( XorStr( "z" ) ) );

		if( user_index == m_pEngine->GetLocalPlayer( ) ) {
			if( g_Vars.esp.server_impacts ) {
				m_pDebugOverlay->AddBoxOverlay( vecImpact, { -2, -2, -2 }, { 2, 2, 2 }, { }, 0, 0, 255, 125, sv_showimpacts_time->GetFloat( ) );
			}
		}

		auto pEntity = ( C_CSPlayer* )m_pEntList->GetClientEntity( user_index );
		if( pEntity && !pEntity->IsTeammate( pLocal, false ) ) {

			if( g_Visuals.player_fading_alpha.at( user_index ) > 0.4f ) {
				if( g_Vars.esp.bullet_beams )
					g_BeamEffects.Add( { m_pGlobalVars->realtime, pEntity->GetEyePosition( ), vecImpact, g_Vars.esp.bullet_beams_color.ToRegularColor( ), pEntity->EntIndex( ), pEntity->m_nTickBase( ), false } );
			}
		}

		break;
	}
	case hash_32_fnv1a_const( "round_start" ): {
		if( g_BeamEffects.m_vecBeamEffects.size( ) )
			g_BeamEffects.m_vecBeamEffects.clear( );

		for( int i = 1; i <= m_pGlobalVars->maxClients; ++i ) {
			g_Resolver.m_arrResolverData.at( i ).Reset( );

			g_Visuals.player_fading_alpha.at( i ) = 0.f;
			g_ExtendedVisuals.m_arrSoundPlayers.at( i ).m_bValidSound = false;
			g_ExtendedVisuals.m_arrSpectatorPlayers.at( i ).m_bOverride = false;

			const auto player = C_CSPlayer::GetPlayerByIndex( i );
			if( !player )
				continue;

			if( player->m_iTeamNum( ) != TEAM_CT && player->m_iTeamNum( ) != TEAM_TT )
				continue;

			if( player->IsTeammate( pLocal ) )
				continue;

			if( player->IsDead( ) )
				continue;

			// reset health...
			player->m_iHealth( ) = 100;
		}

		break;
	}
	case hash_32_fnv1a_const( "round_poststart" ): {

		for( int i = 1; i <= m_pGlobalVars->maxClients; ++i ) {
			const auto player = C_CSPlayer::GetPlayerByIndex( i );
			if( !player )
				continue;

			if( player->m_iTeamNum( ) != TEAM_CT && player->m_iTeamNum( ) != TEAM_TT )
				continue;

			if( player->IsTeammate( pLocal ) )
				continue;

			if( player->IsDead( ) )
				continue;

			// reset health...
			player->m_iHealth( ) = 100;
		}

		break;
	}
	case hash_32_fnv1a_const( "player_death" ): {

		auto enemy = pEvent->GetInt( XorStr( "userid" ) );
		auto enemy_index = m_pEngine->GetPlayerForUserID( enemy );

		for( int i = 1; i <= m_pGlobalVars->maxClients; ++i ) {
			if( i == enemy_index )
				g_Resolver.m_arrResolverData.at( i ).Reset( );
		}

		//if( m_pEngine->GetPlayerForUserID( pEvent->GetInt( XorStr( "attacker" ) ) ) == m_pEngine->GetLocalPlayer( ) ) {
		//	if( const auto icon_override = g_SkinChanger.GetIconOverride( pEvent->GetString( XorStr( "weapon" ) ) ) )
		//		pEvent->SetString( XorStr( "weapon" ), icon_override );
		//}
		//
		break;
	}
	case hash_32_fnv1a_const( "item_equip" ): {
		if( !g_Vars.visuals_enemy.dormant )
			return;

		auto enemy = pEvent->GetInt( XorStr( "userid" ) );
		auto enemy_index = m_pEngine->GetPlayerForUserID( enemy );

		if( enemy_index == m_pEngine->GetLocalPlayer( ) )
			return;

		auto wep_type = pEvent->GetInt( XorStr( "weptype" ) );
		auto weap = pEvent->GetInt( XorStr( "defindex" ) );

		const auto pNewWeaponData = m_pWeaponSystem->GetWeaponInfo( weap );

		g_ExtendedVisuals.m_arrWeaponInfos[ enemy_index ].first = weap;
		g_ExtendedVisuals.m_arrWeaponInfos[ enemy_index ].second = pNewWeaponData ? pNewWeaponData : nullptr;

		// this is fine, but sometimes pWeapon will be nullptr, therefore it won't render the weapon
		//auto pWeapon = reinterpret_cast< C_WeaponCSBaseGun* >( player->m_hActiveWeapon( ).Get( ) );
		//if( pWeapon ) {
		//	//pWeapon->m_iItemDefinitionIndex( ) = weap;

		//	// idk what other shit to save
		//	/*if( pNewWeaponData && pWeapon->GetCSWeaponData( ).IsValid( ) ) {
		//		pWeapon->GetCSWeaponData( )->m_iMaxClip = pNewWeaponData->m_iMaxClip;
		//		pWeapon->GetCSWeaponData( )->m_iWeaponType = wep_type; //pNewWeaponData->m_iWeaponType;
		//		pWeapon->GetCSWeaponData( )->m_szHudName = pNewWeaponData->m_szHudName; // lol idk
		//		pWeapon->GetCSWeaponData( )->m_szWeaponName = pNewWeaponData->m_szWeaponName; // lol idk
		//	}*/
		//}
		break;
	}
	case hash_32_fnv1a_const( "item_purchase" ): {
		if( !g_Vars.esp.event_buy )
			return;

		const std::string item = pEvent->GetString( XorStr( "weapon" ) );
		if( item == XorStr( "weapon_unknown" ) )
			return;

		auto enemy = pEvent->GetInt( XorStr( "userid" ) );
		auto enemy_index = m_pEngine->GetPlayerForUserID( enemy );
		auto player = C_CSPlayer::GetPlayerByIndex( enemy_index );

		if( !player || player->IsTeammate( pLocal ) || enemy_index == m_pEngine->GetLocalPlayer( ) )
			return;

		player_info_t info;
		if( !m_pEngine->GetPlayerInfo( enemy_index, &info ) )
			return;

		std::stringstream msg{ };
		std::string name( info.szName );
		if( name.length( ) > 15 ) {
			name.resize( 15 );
			name.append( XorStr( "..." ) );
		}
		msg << name << XorStr( " purchased " ) << item;

		g_EventLog.PushEvent( msg.str( ), Color_f( 255, 255, 255 ), true, XorStr( "buy" ) );
		break;
	}
	case hash_32_fnv1a_const( "player_hurt" ): {
		auto enemy = pEvent->GetInt( XorStr( "userid" ) );
		auto attacker = pEvent->GetInt( XorStr( "attacker" ) );
		auto remaining_health = pEvent->GetString( XorStr( "health" ) );
		auto dmg_to_health = pEvent->GetInt( XorStr( "dmg_health" ) );
		auto hitgroup = pEvent->GetInt( XorStr( "hitgroup" ) );

		auto enemy_index = m_pEngine->GetPlayerForUserID( enemy );
		auto attacker_index = m_pEngine->GetPlayerForUserID( attacker );

		player_info_t attacker_info;
		player_info_t enemy_info;

		auto pEntity = ( C_CSPlayer* )m_pEntList->GetClientEntity( enemy_index );

		if( pEntity && !pEntity->IsTeammate( pLocal, false ) && g_Vars.esp.visualize_sounds && !pEntity->IsDormant( ) ) {
			g_BeamEffects.Add( { m_pGlobalVars->realtime, pEntity->GetAbsOrigin( ), { }, g_Vars.esp.visualize_sounds_color.ToRegularColor( ), pEntity->EntIndex( ), pEntity->m_nTickBase( ), false, true } );
		}

		if( m_pEngine->GetPlayerInfo( attacker_index, &attacker_info ) && m_pEngine->GetPlayerInfo( enemy_index, &enemy_info ) ) {
			if( !pLocal )
				return;

			if( attacker_index != m_pEngine->GetLocalPlayer( ) ) {
				if( enemy_index == pLocal->m_entIndex ) {

				}
			}
			else {
				if( g_Vars.esp.event_dmg ) {
					std::stringstream msg;

					std::string name = enemy_info.szName;
					if( name.length( ) > 15 ) {
						name.resize( 15 );
						name.append( XorStr( "..." ) );
					}

					msg << XorStr( "Hit " ) << name;
					msg << XorStr( " in the " ) << HitgroupToString( hitgroup ).data( );
					msg << XorStr( " for " ) << dmg_to_health;
					msg << XorStr( " damage (" ) << remaining_health << XorStr( " health remaining)" );

					g_EventLog.PushEvent( msg.str( ), Color_f( 255, 255, 255 ), true );
				}

				if( g_Vars.misc.hitsound ) {
					if( g_Vars.misc.hitsound_type && !g_Vars.globals.m_vecHitsounds.empty( ) ) {
						// DIRECTORY XD!!!
						int idx = g_Vars.misc.hitsound_custom;
						if( idx >= g_Vars.globals.m_vecHitsounds.size( ) )
							idx = g_Vars.globals.m_vecHitsounds.size( ) - 1;
						else if( idx < 0 )
							idx = 0;

						std::string curfile = g_Vars.globals.m_vecHitsounds[ idx ];
						if( !curfile.empty( ) ) {
							std::string dir = GetDocumentsDirectory( ).append( XorStr( "\\pdr\\" ) ).append( curfile );

							auto ReadWavFileIntoMemory = [ & ] ( std::string fname, BYTE** pb, DWORD* fsize ) {
								std::ifstream f( fname, std::ios::binary );

								f.seekg( 0, std::ios::end );
								int lim = f.tellg( );
								*fsize = lim;

								*pb = new BYTE[ lim ];
								f.seekg( 0, std::ios::beg );

								f.read( ( char* )*pb, lim );

								f.close( );
							};

							DWORD dwFileSize;
							BYTE* pFileBytes;
							ReadWavFileIntoMemory( dir.data( ), &pFileBytes, &dwFileSize );

							// danke anarh1st47, ich liebe dich
							// dieses code snippet hat mir so sehr geholfen https://i.imgur.com/ybWTY2o.png
							// thanks anarh1st47, you are the greatest
							// loveeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
							// kochamy anarh1st47
							auto modify_volume = [ & ] ( BYTE* bytes ) {
								int offset = 0;
								for( int i = 0; i < dwFileSize / 2; i++ ) {
									if( bytes[ i ] == 'd' && bytes[ i + 1 ] == 'a'
										&& bytes[ i + 2 ] == 't' && bytes[ i + 3 ] == 'a' ) {
										offset = i;
										break;
									}
								}

								if( !offset )
									return;

								BYTE* pDataOffset = ( bytes + offset );
								DWORD dwNumSampleBytes = *( DWORD* )( pDataOffset + 4 );
								DWORD dwNumSamples = dwNumSampleBytes / 2;

								SHORT* pSample = ( SHORT* )( pDataOffset + 8 );
								for( DWORD dwIndex = 0; dwIndex < dwNumSamples; dwIndex++ ) {
									SHORT shSample = *pSample;
									shSample = ( SHORT )( shSample * ( g_Vars.misc.hitsound_volume / 100.f ) );
									*pSample = shSample;
									pSample++;
									if( ( ( BYTE* )pSample ) >= ( bytes + dwFileSize - 1 ) )
										break;
								}
							};

							if( pFileBytes ) {
								modify_volume( pFileBytes );
								PlaySoundA( ( LPCSTR )pFileBytes, NULL, SND_MEMORY | SND_ASYNC );
							}
						}
					}
					else {
						m_pSurface->PlaySound_( XorStr( "buttons\\arena_switch_press_02.wav" ) );
					}
				}
			}
		}
		break;
	}
	case hash_32_fnv1a_const( "player_footstep" ): {
		auto enemy = pEvent->GetInt( XorStr( "userid" ) );
		auto enemy_index = m_pEngine->GetPlayerForUserID( enemy );

		auto pEntity = ( C_CSPlayer* )m_pEntList->GetClientEntity( enemy_index );

		if( pEntity && !pEntity->IsTeammate( pLocal, false ) && g_Vars.esp.visualize_sounds && !pEntity->IsDormant( ) ) {
			g_BeamEffects.Add( { m_pGlobalVars->realtime, pEntity->GetAbsOrigin( ), { }, g_Vars.esp.visualize_sounds_color.ToRegularColor( ), pEntity->EntIndex( ), pEntity->m_nTickBase( ), false, true } );
		}

		break;
	}
	case hash_32_fnv1a_const( "weapon_fire" ): {
		auto enemy = pEvent->GetInt( XorStr( "userid" ) );
		auto enemy_index = m_pEngine->GetPlayerForUserID( enemy );

		auto pEntity = ( C_CSPlayer* )m_pEntList->GetClientEntity( enemy_index );

		if( pEntity && !pEntity->IsTeammate( pLocal, false ) && g_Vars.esp.visualize_sounds && !pEntity->IsDormant( ) ) {
			g_BeamEffects.Add( { m_pGlobalVars->realtime, pEntity->GetAbsOrigin( ), { }, g_Vars.esp.visualize_sounds_color.ToRegularColor( ), pEntity->EntIndex( ), pEntity->m_nTickBase( ), false, true } );
		}

		break;
	}
	}

}

int GameEvent::GetEventDebugID( void ) {
	return 42;
}
