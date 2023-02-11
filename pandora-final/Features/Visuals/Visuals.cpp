#include "Visuals.hpp"
#include "../../source.hpp"
#include "../../Renderer/Render.hpp"
#include "../../SDK/Classes/weapon.hpp"
#include "../../SDK/Classes/WeaponInfo.hpp"
#include "../Rage/Resolver.hpp"

#include "../Rage/Autowall.hpp"
#include "../../Utils/Threading/threading.h"
#include "../Rage/AntiAim.hpp"
#include "../Rage/ServerAnimations.hpp"
#include <ctime>
#include <iomanip>
#include "../Miscellaneous/Movement.hpp"

Visuals g_Visuals;
ExtendedVisuals g_ExtendedVisuals;

bool Visuals::SetupBoundingBox( C_CSPlayer* player, Box_t& box ) {
	Vector pos = player->GetAbsOrigin( );
	Vector top = pos + Vector( 0, 0, player->GetCollideable( )->OBBMaxs( ).z );

	Vector2D pos_screen, top_screen;

	if( !Render::Engine::WorldToScreen( pos, pos_screen ) ||
		!Render::Engine::WorldToScreen( top, top_screen ) )
		return false;

	box.x = int( top_screen.x - ( ( pos_screen.y - top_screen.y ) / 2 ) / 2 );
	box.y = int( top_screen.y );

	box.w = int( ( ( pos_screen.y - top_screen.y ) ) / 2 );
	box.h = int( ( pos_screen.y - top_screen.y ) );

	const bool out_of_fov = pos_screen.x + box.w + 20 < 0 || pos_screen.x - box.w - 20 > Render::Engine::m_width || pos_screen.y + 20 < 0 || pos_screen.y - box.h - 20 > Render::Engine::m_height;

	return !out_of_fov;
}

bool Visuals::IsValidPlayer( C_CSPlayer* entity ) {
	auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return false;

	if( !entity )
		return false;

	if( !entity->IsPlayer( ) )
		return false;

	if( entity->IsDead( ) )
		return false;

	if( entity->EntIndex( ) == m_pEngine->GetLocalPlayer( ) )
		return false;

	if( entity->IsTeammate( pLocal ) )
		return g_Vars.visuals_enemy.teammates;

	return true;
}

bool Visuals::IsValidEntity( C_BaseEntity* entity ) {
	auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return false;

	if( !entity )
		return false;

	if( entity->IsDormant( ) )
		return false;

	return true;
}

void Visuals::RenderBox( const Box_t& box, C_CSPlayer* entity ) {
	Render::Engine::Rect( box.x, box.y, box.w, box.h, DetermineVisualsColor( visuals_config->box_color.ToRegularColor( ).OverrideAlpha( 210, true ), entity ) );
	Render::Engine::Rect( box.x - 1, box.y - 1, box.w + 2, box.h + 2, Color( 0, 0, 0, 180 * player_fading_alpha.at( entity->EntIndex( ) ) ) );
	Render::Engine::Rect( box.x + 1, box.y + 1, box.w - 2, box.h - 2, Color( 0, 0, 0, 180 * player_fading_alpha.at( entity->EntIndex( ) ) ) );
}

void Visuals::RenderName( const Box_t& box, C_CSPlayer* entity ) {
	player_info_t info;
	if( !m_pEngine->GetPlayerInfo( entity->EntIndex( ), &info ) )
		return;

	// yeah, performance isn't the best here
	std::string name( info.szName );
	if( name.length( ) > 15 ) {
		name.resize( 15 );
		name.append( XorStr( "..." ) );
	}

	Render::Engine::esp_bold.string( box.x + ( box.w / 2 ), box.y - Render::Engine::esp_bold.m_size.m_height,
		DetermineVisualsColor( visuals_config->name_color.ToRegularColor( ).OverrideAlpha( 180, true ), entity ), name.data( ), Render::Engine::ALIGN_CENTER );
}

void Visuals::RenderHealth( const Box_t& box, C_CSPlayer* entity ) {
	const int bar_size = std::clamp( int( ( entity->m_iHealth( ) * box.h ) / entity->m_iMaxHealth( ) ), 0, box.h );

	const int red = std::min( ( 510 * ( entity->m_iMaxHealth( ) - entity->m_iHealth( ) ) ) / entity->m_iMaxHealth( ), 255 );
	const int green = std::min( ( 510 * entity->m_iHealth( ) ) / entity->m_iMaxHealth( ), 255 );

	auto color = Color( red, green, 0 );

	Render::Engine::RectFilled( box.x - 6, box.y - 1, 4, box.h + 2, Color( 0, 0, 0, 180 * player_fading_alpha.at( entity->EntIndex( ) ) ) );
	Render::Engine::RectFilled( box.x - 5, box.y + ( box.h - bar_size ), 2, bar_size, DetermineVisualsColor( color.OverrideAlpha( 210, true ), entity ) );

	// draw health amount when it's lethal damage
	if( entity->m_iHealth( ) <= 92 || entity->m_iHealth( ) > entity->m_iMaxHealth( ) ) {
		// note - michal;
		// either sprintf here, or add some nice formatting library
		// std::to_string is slow, could kill some frames when multiple people are being rendered
		// this could also be apparent on a much larger scale on lower-end pcs

		Render::Engine::esp_pixel.string( box.x - 5, box.y + ( box.h - bar_size ) - 7, Color( 255, 255, 255, 180 * player_fading_alpha.at( entity->EntIndex( ) ) ), std::to_string( entity->m_iHealth( ) ), Render::Engine::ALIGN_CENTER );
	}
}

std::string Visuals::GetWeaponIcon( const int id ) {
	auto search = m_WeaponIconsMap.find( id );
	if( search != m_WeaponIconsMap.end( ) )
		return std::string( &search->second, 1 );

	return XorStr( "" );
}

void Visuals::RenderBottomInfo( const Box_t& box, C_CSPlayer* entity ) {
	auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	const auto RoundToMultiple = [ & ] ( int in, int multiple ) {
		const auto ratio = static_cast< double >( in ) / multiple;
		const auto iratio = std::lround( ratio );
		return static_cast< int >( iratio * multiple );
	};

	const float flDistance = !pLocal->IsDead( ) ? pLocal->GetAbsOrigin( ).Distance( entity->m_vecOrigin( ) ) : ( pLocal->m_hObserverTarget( ).IsValid( ) && pLocal->m_hObserverTarget( ).Get( ) ) ? reinterpret_cast< C_CSPlayer* >( pLocal->m_hObserverTarget( ).Get( ) )->GetAbsOrigin( ).Distance( entity->m_vecOrigin( ) ) : 0.f;

	const float flMeters = flDistance * 0.0254f;
	const float flFeet = flMeters * 3.281f;

	std::string str = std::to_string( RoundToMultiple( flFeet, 5 ) ) + XorStr( " FT" );

	int offset = 3;
	if( visuals_config->lby_timer ) {
		if( g_Resolver.m_arrResolverData[ entity->EntIndex( ) ].m_bInPredictionStage ) {
			float flAnimationTime = entity->m_flOldSimulationTime( ) + m_pGlobalVars->interval_per_tick;
			float flTimeDifference = entity->m_flSimulationTime( ) - entity->m_flOldSimulationTime( );
			float flUpdateTime = g_Resolver.m_arrResolverData[ entity->EntIndex( ) ].m_flLowerBodyRealignTimer - flAnimationTime;
			float flBoxMultiplier = ( 1.1f - flUpdateTime ) / 1.1f;

			animated_lby.at( entity->EntIndex( ) ) = GUI::Approach( animated_lby.at( entity->EntIndex( ) ), flBoxMultiplier, m_pGlobalVars->frametime * 10.f );
			if( flBoxMultiplier < animated_lby.at( entity->EntIndex( ) ) )
				animated_lby.at( entity->EntIndex( ) ) = flBoxMultiplier;

			const int box_width = std::clamp<int>( ( box.w + 1 ) * animated_lby.at( entity->EntIndex( ) ), 0, box.w );

			Render::Engine::RectFilled( box.x - 1, box.y + box.h + offset - 1, box.w + 2, 4, Color( 0, 0, 0, 180 * player_fading_alpha.at( entity->EntIndex( ) ) ) );
			Render::Engine::RectFilled( box.x, box.y + box.h + offset, box_width, 2, DetermineVisualsColor( visuals_config->lby_timer_color.ToRegularColor( ).OverrideAlpha( 210, true ), entity ) );

			// 4px height
			offset += 5;
		}
	}

	auto pWeapon = reinterpret_cast< C_WeaponCSBaseGun* >( entity->m_hActiveWeapon( ).Get( ) );

	const bool bDormant = entity->IsDormant( );

	CCSWeaponInfo* pWeaponData = nullptr;
	if( !bDormant || ( bDormant && g_ExtendedVisuals.m_arrWeaponInfos[ entity->EntIndex( ) ].second == nullptr ) ) {
		if( pWeapon ) {
			g_ExtendedVisuals.m_arrWeaponInfos[ entity->EntIndex( ) ].first = pWeapon->m_iItemDefinitionIndex( );
			g_ExtendedVisuals.m_arrWeaponInfos[ entity->EntIndex( ) ].second = pWeaponData = pWeapon->GetCSWeaponData( ).Xor( );
		}
	}

	bool bDontDo = false;
	if( !pWeaponData ) {
		if( bDormant ) {
			pWeaponData = g_ExtendedVisuals.m_arrWeaponInfos[ entity->EntIndex( ) ].second;
		}

		if( !pWeaponData ) {
			bDontDo = true;
		}
	}

	if( !bDontDo ) {
		if( visuals_config->ammo && ( ( !bDormant && pWeapon && pWeaponData ) || ( bDormant && pWeaponData ) ) ) {
			const bool bMaxOutAmmo = bDormant; //&& last_non_dormant_weapon.at( entity->EntIndex( ) ) != pWeapon->m_iItemDefinitionIndex( );

			// don't render on knifes, zeus, etc
			bool bIsInvalidWeapon = pWeaponData->m_iWeaponType == WEAPONTYPE_GRENADE || pWeaponData->m_iWeaponType == WEAPONTYPE_KNIFE || pWeaponData->m_iWeaponType == WEAPONTYPE_C4 || pWeaponData->m_iMaxClip <= 0;
			if( !bIsInvalidWeapon ) {
				const int nCurrentClip = bMaxOutAmmo ? pWeaponData->m_iMaxClip : pWeapon->m_iClip1( );

				// also prevent division by zero, lol
				float flBoxMultiplier = ( float )nCurrentClip / pWeaponData->m_iMaxClip;

				bool bReloading = false;
				auto pReloadLayer = entity->m_AnimOverlay( ).Element( 1 );
				if( pReloadLayer.m_pOwner ) {
					const int nActivity = entity->GetSequenceActivity( pReloadLayer.m_nSequence );

					if( nActivity == 967 && pReloadLayer.m_flWeight != 0.f ) {
						// smooth out the ammo bar for reloading players
						flBoxMultiplier = pReloadLayer.m_flCycle;
						bReloading = true;
					}
				}

				const int box_width = std::clamp<int>( ( box.w + 1 ) * flBoxMultiplier, 0, box.w );

				Render::Engine::RectFilled( box.x - 1, box.y + box.h + offset - 1, box.w + 2, 4, Color( 0, 0, 0, 180 * player_fading_alpha.at( entity->EntIndex( ) ) ) );
				Render::Engine::RectFilled( box.x, box.y + box.h + offset, box_width, 2, DetermineVisualsColor( visuals_config->ammo_color.ToRegularColor( ).OverrideAlpha( 210, true ), entity ) );

				// ammo is less than 90% of the max ammo
				if( nCurrentClip > 0 && nCurrentClip <= int( std::floor( float( pWeaponData->m_iMaxClip ) * 0.9f ) ) && !bReloading ) {
					Render::Engine::esp_pixel.string( box.x + box_width, ( box.y + box.h + offset ) - 3, DetermineVisualsColor( Color::White( ).OverrideAlpha( 210, true ), entity ),
						std::to_string( nCurrentClip ), Render::Engine::ALIGN_CENTER );
				}

				// 4px height
				offset += 4;
			}
		}
	}

	if( visuals_config->distance ) {
		Render::Engine::esp_pixel.string( box.x + ( box.w / 2 ), box.y + box.h + offset,
			DetermineVisualsColor( Color::White( ).OverrideAlpha( 210, true ), entity ), str.data( ), Render::Engine::ALIGN_CENTER );

		offset += Render::Engine::esp_pixel.m_size.m_height;
	}

	if( !bDontDo ) {
		if( visuals_config->weapon && pWeaponData ) {
			// note - michal;
			// not the best code optimization-wise, again...
			// i'll end up leaving notes everywhere that i'll improve performance later on
			std::wstring localized = m_pLocalize->Find( pWeaponData->m_szHudName );
			std::string name( localized.begin( ), localized.end( ) );
			std::transform( name.begin( ), name.end( ), name.begin( ), ::toupper );

			Render::Engine::esp_pixel.string( box.x + ( box.w / 2 ), box.y + box.h + offset,
				DetermineVisualsColor( Color::White( ).OverrideAlpha( 210, true ), entity ), name.data( ), Render::Engine::ALIGN_CENTER );

			offset += Render::Engine::esp_pixel.m_size.m_height;
		}
	}

	if( visuals_config->weapon_icon && ( ( !bDormant && pWeapon ) || bDormant ) ) {
		Render::Engine::cs.string( box.x + ( box.w / 2 ), box.y + box.h + offset,
			DetermineVisualsColor( visuals_config->weapon_color.ToRegularColor( ).OverrideAlpha( 210, true ), entity ), GetWeaponIcon( bDormant ? g_ExtendedVisuals.m_arrWeaponInfos[ entity->EntIndex( ) ].first : pWeapon->m_iItemDefinitionIndex( ) ), Render::Engine::ALIGN_CENTER );

		offset += Render::Engine::cs.m_size.m_height;
	}
}

void Visuals::RenderSideInfo( const Box_t& box, C_CSPlayer* entity ) {
	std::vector<std::pair<std::string, Color>> vec_flags{ };

#if 1
	if( g_Vars.visuals_enemy.money )
		vec_flags.emplace_back( std::string( XorStr( "$" ) ).append( std::to_string( entity->m_iAccount( ) ) ), Color( 155, 210, 100 ) );

	if( entity->IsReloading( ) )
		vec_flags.emplace_back( XorStr( "R" ), Color( 0, 175, 255 ) );

	if( entity->m_ArmorValue( ) > 0 ) {
		if( entity->m_bHasHelmet( ) )
			vec_flags.emplace_back( XorStr( "HK" ), Color( 255, 255, 255 ) );
		else
			vec_flags.emplace_back( XorStr( "K" ), Color( 255, 255, 255 ) );
	}

	float m_flFlashBangTime = *( float* )( ( uintptr_t )entity + 0xA2E8 );
	if( m_flFlashBangTime > 0.f )
		vec_flags.emplace_back( XorStr( "BLIND" ), Color( 0, 175, 255 ) );

	if( entity->m_bIsScoped( ) )
		vec_flags.emplace_back( XorStr( "ZOOM" ), Color( 0, 175, 255 ) );

	if( auto pLagData = g_LagCompensation.GetLagData( entity->m_entIndex ); pLagData ) {
		if( pLagData->m_Records.size( ) ) {
			if( auto pRecord = &pLagData->m_Records.front( ); pRecord ) {
				if( !( pRecord->m_RecordFlags & ELagRecordFlags::RF_IsResolved ) && g_Vars.rage.antiaim_correction ) {
					vec_flags.emplace_back( XorStr( "FAKE" ), Color( 255, 255, 255 ) );
				}
			}
		}
	}

	for( size_t i = 0; i < 48; ++i ) {
		auto pWeapons = entity->m_hMyWeapons( )[ i ];
		if( !pWeapons.IsValid( ) )
			break;

		auto pItem = ( C_BaseCombatWeapon* )pWeapons.Get( );
		if( !pItem )
			continue;

		auto nItemDefinitionIndex = pItem->m_Item( ).m_iItemDefinitionIndex( );
		if( nItemDefinitionIndex == WEAPON_C4 )
			vec_flags.emplace_back( XorStr( "B" ), Color( 255, 0, 0 ) );
	}
#else
	C_AnimationLayer* layer_6 = &entity->m_AnimOverlay( )[ 6 ];
	C_AnimationLayer* layer_3 = &entity->m_AnimOverlay( )[ 3 ];
	C_AnimationLayer* layer_12 = &entity->m_AnimOverlay( )[ 12 ];
	if( layer_6 && layer_3 && layer_12 && entity->m_PlayerAnimState( ) ) {
		int layer_3_activity = entity->GetSequenceActivity( layer_3->m_nSequence );

		vec_flags.emplace_back( std::string( XorStr( "layer 3 act: " ) ).append( std::to_string( layer_3_activity ) ), Color( 255, 255, 255 ) );
		vec_flags.emplace_back( std::string( XorStr( "layer 3 weight: " ) ).append( std::to_string( layer_3->m_flWeight ) ), Color( 255, 255, 255 ) );
		vec_flags.emplace_back( std::string( XorStr( "layer 3 cycle: " ) ).append( std::to_string( layer_3->m_flCycle ) ), Color( 255, 255, 255 ) );
		vec_flags.emplace_back( std::string( XorStr( "layer 6 weight: " ) ).append( std::to_string( layer_6->m_flWeight ) ), Color( 255, 255, 255 ) );
		vec_flags.emplace_back( std::string( XorStr( "layer 6 cycle: " ) ).append( std::to_string( layer_6->m_flCycle ) ), Color( 255, 255, 255 ) );
		vec_flags.emplace_back( std::string( XorStr( "layer 6 playback: " ) ).append( std::to_string( layer_6->m_flPlaybackRate ) ), Color( 255, 255, 255 ) );
		vec_flags.emplace_back( std::string( XorStr( "layer 12 weight: " ) ).append( std::to_string( layer_12->m_flWeight ) ), Color( 255, 255, 255 ) );
		vec_flags.emplace_back( std::string( XorStr( "choke: " ) ).append( std::to_string( TIME_TO_TICKS( entity->m_flSimulationTime( ) - entity->m_flOldSimulationTime( ) ) ) ), Color( 255, 255, 255 ) );
		vec_flags.emplace_back( std::string( XorStr( "lby: " ) ).append( std::to_string( entity->m_flLowerBodyYawTarget( ) ) ), Color( 255, 255, 255 ) );
		vec_flags.emplace_back( std::string( XorStr( "velocity: " ) ).append( std::to_string( entity->m_PlayerAnimState( )->m_flVelocityLengthXY ) ), Color( 255, 255, 255 ) );
		vec_flags.emplace_back( std::string( XorStr( "abs yaw delta: " ) ).append( std::to_string( Math::AngleDiff( entity->m_PlayerAnimState( )->m_flEyeYaw, entity->m_PlayerAnimState( )->m_flFootYaw ) ) ), Color( 255, 255, 255 ) );
	}
#endif

	int offset{ 0 };
	for( auto flag : vec_flags ) {
		std::transform( flag.first.begin( ), flag.first.end( ), flag.first.begin( ), ::toupper );

		// draw the string
		Render::Engine::esp_pixel.string( box.x + box.w + 2, box.y - 1 + offset, DetermineVisualsColor( flag.second.OverrideAlpha( 180 ), entity ), flag.first );

		// extend offset
		offset += Render::Engine::esp_pixel.m_size.m_height;
	}
}

void Visuals::RenderDroppedWeapons( C_BaseEntity* entity ) {
	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	auto pWeapon = reinterpret_cast< C_WeaponCSBaseGun* >( entity );
	if( !pWeapon )
		return;

	if( !pWeapon->m_iItemDefinitionIndex( ) || pWeapon->m_hOwnerEntity( ) != -1 )
		return;

	auto pWeaponData = pWeapon->GetCSWeaponData( );
	if( !pWeaponData.IsValid( ) )
		return;

	Vector2D screen_position{ };
	if( !Render::Engine::WorldToScreen( pWeapon->GetAbsOrigin( ), screen_position ) )
		return;

	auto clientClass = entity->GetClientClass( );
	if( !clientClass )
		return;

	auto bIsC4 = clientClass->m_ClassID == CC4;

	// note - michal;
	// not the best code optimization-wise, again...
	// i'll end up leaving notes everywhere that i'll improve performance later on
	std::wstring localized = m_pLocalize->Find( pWeaponData->m_szHudName );
	std::string name( localized.begin( ), localized.end( ) );
	std::transform( name.begin( ), name.end( ), name.begin( ), ::toupper );

	if( name.empty( ) )
		return;

	// LOL
	float distance = !pLocal->IsDead( ) ? pLocal->GetAbsOrigin( ).Distance( entity->m_vecOrigin( ) ) : ( pLocal->m_hObserverTarget( ).IsValid( ) && pLocal->m_hObserverTarget( ).Get( ) ) ? reinterpret_cast< C_CSPlayer* >( pLocal->m_hObserverTarget( ).Get( ) )->GetAbsOrigin( ).Distance( entity->m_vecOrigin( ) ) : 0.f;

	const auto clamped_distance = std::clamp<float>( distance - 300.f, 0.f, 360.f );
	float alpha = bIsC4 ? 180.f : 180.f - ( clamped_distance * 0.5f );

	if( alpha < 0.f )
		return;

	if( g_Vars.esp.dropped_weapons || ( bIsC4 && g_Vars.esp.draw_bomb ) ) {
		Render::Engine::esp_pixel.string( screen_position.x, screen_position.y,
			( bIsC4 && g_Vars.esp.draw_bomb ) ? g_Vars.esp.draw_bomb_color.ToRegularColor( ).OverrideAlpha( 180, true ) : g_Vars.esp.dropped_weapons_color.ToRegularColor( ).OverrideAlpha( alpha ),
			( bIsC4 && g_Vars.esp.draw_bomb ) ? XorStr( "BOMB" ) : name.data( ) );
	}

	if( g_Vars.esp.dropped_weapons_ammo && !bIsC4 ) {
		const auto clip = pWeapon->m_iClip1( );
		if( clip > 0 ) {
			const auto text_size = Render::Engine::esp_pixel.size( name );
			const auto max_clip = pWeaponData->m_iMaxClip;

			auto width = text_size.m_width;
			width *= clip;

			// even tho max_clip should never be 0, better safe..
			if( max_clip )
				width /= max_clip;

			// outline
			Render::Engine::RectFilled( screen_position + Vector2D( 0, 9 ), Vector2D( text_size.m_width + 1, 4 ), Color( 0.f, 0.f, 0.f, alpha ) );

			// actual bar
			Render::Engine::RectFilled( screen_position + Vector2D( 1, 10 ), Vector2D( width - 1, 2 ), visuals_config->ammo_color.ToRegularColor( ).OverrideAlpha( alpha ) );

			// draw bullets in clip
			if( clip <= static_cast< int >( max_clip * 0.34 ) ) {
				Render::Engine::esp_pixel.string( screen_position.x + width, screen_position.y + 8, Color::White( ).OverrideAlpha( alpha ), std::to_string( clip ) );
			}
		}
	}
}

void Visuals::RenderNades( C_BaseEntity* entity ) {
	if( !entity )
		return;

	auto client_class = entity->GetClientClass( );
	if( !client_class )
		return;

	std::string grenade{ };

	// CInferno does not have a model...
	if( client_class->m_ClassID != CInferno ) {
		auto model = entity->GetModel( );
		if( !model )
			return;

		std::string model_name = m_pModelInfo->GetModelName( model );
		if( model_name.empty( ) )
			return;

		const auto smoke = reinterpret_cast< C_SmokeGrenadeProjectile* >( entity );
		switch( client_class->m_ClassID ) {
		case CBaseCSGrenadeProjectile:
			// seriously, just why?
			// this game is so shit
			if( model_name.find( XorStr( "fraggrenade" ) ) != std::string::npos ) {
				grenade = XorStr( "FRAG" );
			}
			else {
				grenade = XorStr( "FLASHBANG" );
			}
			break;
		case CMolotovProjectile:
			grenade = XorStr( "FIRE" );
			break;
		case CSmokeGrenadeProjectile:
			grenade = XorStr( "SMOKE" );

			// apparently m_bDidSmokeEffect doesn't seem to work?
			if( smoke ) {
				const auto spawn_time = TICKS_TO_TIME( smoke->m_nSmokeEffectTickBegin( ) );
				const auto time = ( spawn_time + C_SmokeGrenadeProjectile::GetExpiryTime( ) ) - m_pGlobalVars->curtime;
				const auto factor = time / C_SmokeGrenadeProjectile::GetExpiryTime( );

				if( factor > 0.0f ) {
					grenade.clear( );
				}
			}

			break;
		case CDecoyProjectile:
			grenade = XorStr( "DECOY" );
			break;
		}
	}

	Vector2D screen_position{ };
	if( !Render::Engine::WorldToScreen( entity->GetAbsOrigin( ), screen_position ) )
		return;

	// draw nade string..
	if( !grenade.empty( ) ) {
		Render::Engine::esp_pixel.string( screen_position.x, screen_position.y, g_Vars.esp.grenades_color.ToRegularColor( ).OverrideAlpha( 180, true ), grenade, Render::Engine::ALIGN_CENTER );
	}
}

std::string Visuals::GetBombSite( C_PlantedC4* entity ) {
	if( !m_pPlayerResource.IsValid( ) || !( *m_pPlayerResource.Xor( ) ) )
		return XorStr( "Error while finding bombsite..." );

	const auto& origin = entity->GetAbsOrigin( );

	// gosh I hate dereferencing it here!
	const auto& bomb_a = ( *m_pPlayerResource.Xor( ) )->m_bombsiteCenterA( );
	const auto& bomb_b = ( *m_pPlayerResource.Xor( ) )->m_bombsiteCenterB( );

	const auto dist_a = origin.Distance( bomb_a );
	const auto dist_b = origin.Distance( bomb_b );

	return dist_a < dist_b ? XorStr( "A" ) : XorStr( "B" );
}

void Visuals::RenderObjectives( C_BaseEntity* entity ) {
	if( !entity )
		return;

	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	auto client_class = entity->GetClientClass( );
	if( !client_class )
		return;

	if( client_class->m_ClassID == CPlantedC4 ) {
		const auto plantedc4 = reinterpret_cast< C_PlantedC4* >( entity );

		if( !plantedc4 )
			return;

		const float c4timer = g_Vars.mp_c4timer->GetFloat( );

		// note - nico;
		// I don't know if we should clamp this to mp_c4timer->GetFloat( )
		// if the mp_c4timer changes while the c4 is planted to something lower than the remaining time
		// it will clamp it.. (this should never really happen, but yeah)
		const float time = std::clamp( plantedc4->m_flC4Blow( ) - m_pGlobalVars->curtime, 0.f, c4timer );

		if( time && !plantedc4->m_bBombDefused( ) ) {
			// SUPREMACY SUPREMACY SUPREMACY SUPREMACY SUPREMACY SUPREMACY SUPREMACY SUPREMACY SUPREMACY SUPREMACY 

			CGameTrace tr;
			CTraceFilter filter;
			auto explosion_origin = plantedc4->GetAbsOrigin( );
			auto explosion_origin_adjusted = explosion_origin;
			explosion_origin_adjusted.z += 8.f;

			// setup filter and do first trace.
			filter.pSkip = plantedc4;

			m_pEngineTrace->TraceRay(
				Ray_t( explosion_origin_adjusted, explosion_origin_adjusted + Vector( 0.f, 0.f, -40.f ) ),
				MASK_SOLID,
				&filter,
				&tr
			);

			// pull out of the wall a bit.
			if( tr.fraction != 1.f )
				explosion_origin = tr.endpos + ( tr.plane.normal * 0.6f );

			// this happens inside CCSGameRules::RadiusDamage.
			explosion_origin.z += 1.f;

			// set all other vars.
			auto m_planted_c4_explosion_origin = explosion_origin;

			auto dst = pLocal->WorldSpaceCenter( );
			auto to_target = m_planted_c4_explosion_origin - dst;
			auto dist = to_target.Length( );

			// calculate the bomb damage based on our distance to the C4's explosion.
			float range_damage = 500.f * std::exp( ( dist * dist ) / ( ( ( ( 500.f * 3.5f ) / 3.f ) * -2.f ) * ( ( 500.f * 3.5f ) / 3.f ) ) );

			static auto scale_damage = [ ] ( float damage, int armor_value ) {
				float new_damage, armor;

				if( armor_value > 0 ) {
					new_damage = damage * 0.5f;
					armor = ( damage - new_damage ) * 0.5f;

					if( armor > ( float )armor_value ) {
						armor = ( float )armor_value * 2.f;
						new_damage = damage - armor;
					}

					damage = new_damage;
				}

				return std::max( 0, ( int )std::floor( damage ) );
			};

			// now finally, scale the damage based on our armor (if we have any).
			float final_damage = scale_damage( range_damage, pLocal->m_ArmorValue( ) );

			// we can clamp this in range 0-10, it can't be higher than 10, lol!
			const float remaining_defuse_time = std::clamp( plantedc4->m_flDefuseCountDown( ) - m_pGlobalVars->curtime, 0.f, 10.f );

			const float factor_c4 = time / ( ( c4timer != 0.f ) ? c4timer : 40.f );
			const float factor_defuse = remaining_defuse_time / 10.f;

			char time_buf[ 128 ] = { };
			sprintf( time_buf, XorStr( " - %.1fs" ), time );

			char dmg_buf[ 128 ] = { };
			sprintf( dmg_buf, XorStr( "-%d HP" ), int( final_damage ) );

			char defuse_buf[ 128 ] = { };
			sprintf( defuse_buf, XorStr( "%.1fs" ), remaining_defuse_time );

			// compute bombsite string
			const auto bomb_site = GetBombSite( plantedc4 ).append( time_buf );

			if( g_Vars.esp.draw_bomb ) {
				const auto screen = Render::GetScreenSize( );
				static const auto size = Vector2D( 160.f, 3.f );

				const float width_c4 = size.x * factor_c4;

				Color site_color = Color( 150, 200, 60, 220 );
				if( time <= 10.f ) {
					site_color = Color( 255, 255, 185, 220 );

					if( time <= 5.f ) {
						site_color = Color( 255, 0, 0, 220 );
					}
				}

				// is this thing being defused?
				if( plantedc4->m_bBeingDefused( ) ) {
					const float width_defuse = size.y * factor_defuse;

					// background
					Render::Engine::RectFilled( Vector2D( 0, 0 ), Vector2D( 20, screen.y ), Color( 0, 0, 0, 100 ) );

					// defuse timer bar
					int height = ( screen.y - 2 ) * factor_defuse;
					Render::Engine::RectFilled( Vector2D( 1, 1 + ( int )abs( screen.y - height ) ), Vector2D( 18, height ), Color( 30, 170, 30 ) );
				}

				// draw bomb site string
				if( time > 0.f )
					Render::Engine::esp_indicator.string( 8, 8, site_color, bomb_site, Render::Engine::ALIGN_LEFT );

				if( final_damage > 0 )
					Render::Engine::esp_indicator.string( 8, 36,
						final_damage >= pLocal->m_iHealth( ) ? Color( 255, 0, 0 ) : Color( 255, 255, 185 ), final_damage >= pLocal->m_iHealth( ) ? XorStr( "FATAL" ) : dmg_buf, Render::Engine::ALIGN_LEFT );
			}

			if( g_Vars.esp.draw_bomb ) {
				static const auto size = Vector2D( 80.f, 3.f );

				Vector2D screen{ };
				if( Render::Engine::WorldToScreen( entity->GetAbsOrigin( ), screen ) ) {
					const float width_c4 = size.x * factor_c4;

					// draw bomb site string
					Render::Engine::esp_pixel.string( screen.x, screen.y - Render::Engine::esp_pixel.m_size.m_height - 1, g_Vars.esp.draw_bomb_color.ToRegularColor( ).OverrideAlpha( 180, true ), XorStr( "BOMB" ), Render::Engine::ALIGN_CENTER );
				}
			}
		}
	}
}

void Visuals::RenderArrows( C_BaseEntity* entity ) {
	auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	auto RotateArrow = [ ] ( std::array< Vector2D, 3 >& points, float rotation ) {
		const auto vecPointsCenter = ( points.at( 0 ) + points.at( 1 ) + points.at( 2 ) ) / 3;
		for( auto& point : points ) {
			point -= vecPointsCenter;

			const auto temp_x = point.x;
			const auto temp_y = point.y;

			const auto theta = DEG2RAD( rotation );
			const auto c = cos( theta );
			const auto s = sin( theta );

			point.x = temp_x * c - temp_y * s;
			point.y = temp_x * s + temp_y * c;

			point += vecPointsCenter;
		}
	};

	const float flWidth = Render::GetScreenSize( ).x;
	const float flHeight = Render::GetScreenSize( ).y;
	if( !entity || !entity->IsPlayer( ) || entity == pLocal || entity->m_iTeamNum( ) == pLocal->m_iTeamNum( ) )
		return;

	static float alpha[ 65 ];
	static bool plus_or_minus[ 65 ];
	if( alpha[ entity->EntIndex( ) ] <= 5 || alpha[ entity->EntIndex( ) ] >= 255 )
		plus_or_minus[ entity->EntIndex( ) ] = !plus_or_minus[ entity->EntIndex( ) ];

	alpha[ entity->EntIndex( ) ] += plus_or_minus[ entity->EntIndex( ) ] ? ( 255.f / 1.f * m_pGlobalVars->frametime ) : -( 255.f / 1.f * m_pGlobalVars->frametime );
	alpha[ entity->EntIndex( ) ] = std::clamp<float>( alpha[ entity->EntIndex( ) ], 5.f, 255.f );

	Vector2D vecScreenPos;
	const bool bWorldToScreened = Render::Engine::WorldToScreen( entity->GetAbsOrigin( ), vecScreenPos );

	// give some extra room for screen position to be off screen.
	const float flExtraRoom = flWidth / 18.f;

	if( !bWorldToScreened
		|| vecScreenPos.x < -flExtraRoom
		|| vecScreenPos.x >( flWidth + flExtraRoom )
		|| vecScreenPos.y < -flExtraRoom
		|| vecScreenPos.y >( flHeight + flExtraRoom ) )
	{
		QAngle angViewAngles;
		m_pEngine.Xor( )->GetViewAngles( angViewAngles );

		const Vector2D vecScreenCenter = Vector2D( flWidth * .5f, flHeight * .5f );
		const float flAngleYaw = DEG2RAD( angViewAngles.y - Math::CalcAngle( pLocal->GetEyePosition( ), entity->GetAbsOrigin( ), true ).y - 90 );

		// note - michal;
		// when the day comes, i'll eventuall make this dynamic so that we can 
		// choose the distance and the size in pixels of the arrow, but this looks nice for now
		const float flNewPointX = ( vecScreenCenter.x + ( ( ( flHeight - 60.f ) / 2 ) * ( visuals_config->view_arrows_distance / 100.0f ) ) * cos( flAngleYaw ) ) + 8.f;
		const float flNewPointY = ( vecScreenCenter.y + ( ( ( flHeight - 60.f ) / 2 ) * ( visuals_config->view_arrows_distance / 100.0f ) ) * sin( flAngleYaw ) ) + 8.f;

		std::array< Vector2D, 3 >vecPoints{
			Vector2D( flNewPointX - visuals_config->view_arrows_size, flNewPointY - visuals_config->view_arrows_size ),
			Vector2D( flNewPointX + visuals_config->view_arrows_size, flNewPointY ),
			Vector2D( flNewPointX - visuals_config->view_arrows_size, flNewPointY + visuals_config->view_arrows_size ) };

		RotateArrow( vecPoints, angViewAngles.y - Math::CalcAngle( pLocal->GetEyePosition( ), entity->GetAbsOrigin( ), true ).y - 90 );

		std::array< Vertex_t, 3 > uVertices{
			Vertex_t( vecPoints.at( 0 ) ),
			Vertex_t( vecPoints.at( 1 ) ),
			Vertex_t( vecPoints.at( 2 ) ) };

		static int nTextureID;
		if( !m_pSurface->IsTextureIDValid( nTextureID ) )
			nTextureID = m_pSurface.Xor( )->CreateNewTextureID( true );

		Color clr = visuals_config->view_arrows_color.ToRegularColor( ).OverrideAlpha( 255 * player_fading_alpha.at( entity->EntIndex( ) ), true );

		// fill
		m_pSurface.Xor( )->DrawSetColor( clr.r( ), clr.g( ), clr.b( ), clr.a( ) * ( alpha[ entity->EntIndex( ) ] / 255.f ) );
		m_pSurface.Xor( )->DrawSetTexture( nTextureID );
		m_pSurface.Xor( )->DrawTexturedPolygon( 3, uVertices.data( ) );
	}
}

void Visuals::Spectators( ) {
	std::vector< std::string > spectators{ };
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer( );

	if( m_pEngine->IsInGame( ) && pLocal ) {
		const auto local_observer = pLocal->m_hObserverTarget( );
		for( int i{ 1 }; i <= m_pGlobalVars->maxClients; ++i ) {
			C_CSPlayer* player = ( C_CSPlayer* )m_pEntList->GetClientEntity( i );
			if( !player )
				continue;

			if( !player->IsDead( ) )
				continue;

			if( player->IsDormant( ) )
				continue;

			if( player->EntIndex( ) == pLocal->EntIndex( ) )
				continue;

			player_info_t info;
			if( !m_pEngine->GetPlayerInfo( i, &info ) )
				continue;

			if( pLocal->IsDead( ) ) {
				auto observer = player->m_hObserverTarget( );
				if( local_observer.IsValid( ) && observer.IsValid( ) ) {
					const auto spec = ( C_CSPlayer* )m_pEntList->GetClientEntityFromHandle( local_observer );
					auto target = reinterpret_cast< C_CSPlayer* >( m_pEntList->GetClientEntityFromHandle( observer ) );

					if( target == spec && spec ) {
						spectators.push_back( std::string( info.szName ).substr( 0, 24 ) );
					}
				}
			}
			else {
				if( player->m_hObserverTarget( ) != pLocal )
					continue;

				spectators.push_back( std::string( info.szName ).substr( 0, 24 ) );
			}
		}
	}

	if( spectators.empty( ) )
		return;

	if( pLocal->m_iObserverMode( ) == /*OBS_MODE_ROAMING*/6 )
		return;

	for( size_t i{ }; i < spectators.size( ); ++i ) {
		auto msg = spectators[ i ];
		auto width = Render::Engine::esp_bold.size( msg ).m_width;
		auto height = Render::Engine::esp_bold.size( msg ).m_height + 2;

		Render::Engine::esp_bold.string( Render::GetScreenSize( ).x - 7 - width, 4 + ( height * i ), Color( 255, 255, 255, 220 ), msg );
	}
}

void Visuals::RenderSkeleton( C_CSPlayer* entity ) {
	if( !entity ) {
		return;
	}

	auto model = entity->GetModel( );
	if( !model ) {
		return;
	}

	auto* hdr = m_pModelInfo->GetStudiomodel( model );
	if( !hdr ) {
		return;
	}

	auto set = hdr->pHitboxSet( entity->m_nHitboxSet( ) );

	if( !set )
		return;

	std::pair< Vector2D, Vector2D > positions[ 32 ];
	int point_count = 0;

	auto chest = set->pHitbox( HITBOX_UPPER_CHEST );
	auto neck = set->pHitbox( HITBOX_NECK );

	if( !chest || !neck )
		return;

	if( entity->IsDead( ) ) {
		return;
	}

	if( g_Visuals.player_fading_alpha[ entity->EntIndex( ) ] <= 0.05f || g_ExtendedVisuals.m_arrSoundPlayers.at( entity->EntIndex( ) ).m_bValidSound ) {
		return;
	}

	auto middle = entity->m_CachedBoneData( ).m_Memory.m_pMemory[ chest->bone ].at( 3 ) + entity->m_CachedBoneData( ).m_Memory.m_pMemory[ neck->bone ].at( 3 );
	middle *= 0.5f;

	auto upperarm_left = set->pHitbox( HITBOX_LEFT_UPPER_ARM );
	auto upperarm_right = set->pHitbox( HITBOX_RIGHT_UPPER_ARM );

	for( int i = 0; i < hdr->numbones; ++i ) {
		auto bone = hdr->pBone( i );
		if( !bone )
			continue;

		if( ( bone->flags & BONE_USED_BY_HITBOX ) == 0 || bone->parent < 0 )
			continue;

		if( bone->parent == chest->bone && i != neck->bone )
			continue;

		Vector parentBone;
		if( i == upperarm_left->bone || i == upperarm_right->bone ) {
			parentBone = middle;
		}
		else {
			parentBone = entity->m_CachedBoneData( ).m_Memory.m_pMemory[ bone->parent ].at( 3 );
		}

		Vector2D parentPos;
		if( !Render::Engine::WorldToScreen( parentBone, parentPos ) )
			continue;

		Vector2D childPos;
		if( !Render::Engine::WorldToScreen( entity->m_CachedBoneData( ).m_Memory.m_pMemory[ i ].at( 3 ), childPos ) )
			continue;

		positions[ point_count ].first = parentPos;
		positions[ point_count ].second = childPos;
		point_count++;

		if( point_count > 31 )
			break;
	}

	if( point_count <= 0 )
		return;

	for( const auto& pos : positions ) {
		Render::Engine::Line( pos.first, pos.second, DetermineVisualsColor( g_Vars.visuals_enemy.skeleton_color.ToRegularColor( ).OverrideAlpha( 210, true ), entity ) );
	}
}

void Visuals::HandlePlayerVisuals( C_CSPlayer* entity ) {
	// do view arrows before any other visuals
	// this is so the bounding box check doesnt interfere with em
	if( visuals_config->view_arrows ) {
		RenderArrows( entity );
	}

	if( g_Vars.misc.spectators ) {
		Spectators( );
	}

	Box_t box;
	if( !SetupBoundingBox( entity, box ) )
		return;

	if( visuals_config->name ) {
		RenderName( box, entity );
	}

	if( visuals_config->box ) {
		RenderBox( box, entity );
	}

	if( visuals_config->health ) {
		RenderHealth( box, entity );
	}

	if( visuals_config->skeleton ) {
		RenderSkeleton( entity );
	}

	RenderBottomInfo( box, entity );

	if( visuals_config->flags )
		RenderSideInfo( box, entity );
}

void Visuals::HandleWorldVisuals( C_BaseEntity* entity ) {
	auto client_class = entity->GetClientClass( );
	if( !client_class )
		return;

	if( g_Vars.esp.dropped_weapons || g_Vars.esp.dropped_weapons_ammo ) {
		if( entity->IsWeapon( ) ) {
			RenderDroppedWeapons( entity );
		}
	}

	switch( client_class->m_ClassID ) {
	case CBaseCSGrenadeProjectile:
	case CMolotovProjectile:
	case CSmokeGrenadeProjectile:
	case CDecoyProjectile:
	case CInferno:
		if( g_Vars.esp.grenades ) {
			RenderNades( entity );
		}
		break;
	case CC4:
	case CPlantedC4:
		RenderObjectives( entity );
		break;
	}

	// already found an entity we are in range of?
	if( !g_Vars.globals.m_bInsideFireRange ) {
		// ok this maths is a huge meme and can prolly be cleaned up LMFAO
		if( client_class->m_ClassID == CInferno ) {
			const auto pLocal = C_CSPlayer::GetLocalPlayer( );
			if( pLocal ) {
				Vector mins, maxs, nmins, nmaxs;
				if( entity->GetClientRenderable( ) )
					entity->GetClientRenderable( )->GetRenderBounds( mins, maxs );

				auto vecLocalAbs = pLocal->GetAbsOrigin( ).ToVector2D( );
				const auto& vecAbsOrigin = entity->GetAbsOrigin( );

				C_CSPlayer* pOwner = ( C_CSPlayer* )entity->m_hOwnerEntity( ).Get( );
				bool bIsLethal = true;
				if( pOwner ) {
					if( pOwner->m_iTeamNum( ) == pLocal->m_iTeamNum( ) && pOwner->EntIndex( ) != m_pEngine->GetLocalPlayer( ) ) {
						if( g_Vars.mp_friendlyfire->GetInt( ) != 1 )
							bIsLethal = false;
					}
				}

				nmins = mins;
				nmaxs = maxs;
				nmins.x *= -1.f;
				nmaxs.x *= -1.f;

				maxs += vecAbsOrigin;
				mins += vecAbsOrigin;
				nmins += vecAbsOrigin;
				nmaxs += vecAbsOrigin;

				bool inBounds = bIsLethal && ( vecLocalAbs >= mins.ToVector2D( ) && vecLocalAbs <= maxs.ToVector2D( ) ) || ( vecLocalAbs >= nmins.ToVector2D( ) && vecLocalAbs <= nmaxs.ToVector2D( ) );

				g_Vars.globals.m_bInsideFireRange = inBounds;
			}
		}
	}
}

Color Visuals::DetermineVisualsColor( Color regular, C_CSPlayer* entity ) {
	if( entity->IsDormant( ) && g_Vars.visuals_enemy.dormant ) {
		return Color( 210, 210, 210, regular.a( ) * player_fading_alpha.at( entity->EntIndex( ) ) );
	}

	Color cRetColor{ regular };
	cRetColor.RGBA[ 3 ] *= player_fading_alpha.at( entity->EntIndex( ) );

	return cRetColor;
}

// lol
bool IsAimingAtPlayerThroughPenetrableWall( C_CSPlayer* local, C_WeaponCSBaseGun* pWeapon ) {
	auto weaponInfo = pWeapon->GetCSWeaponData( );
	if( !weaponInfo.IsValid( ) )
		return -1.0f;

	QAngle view_angles;
	m_pEngine->GetViewAngles( view_angles );

	Autowall::C_FireBulletData data;

	data.m_Player = local;
	data.m_TargetPlayer = nullptr;
	data.m_bPenetration = true;
	data.m_vecStart = local->GetEyePosition( );
	data.m_vecDirection = view_angles.ToVectors( );
	data.m_flMaxLength = data.m_vecDirection.Normalize( );
	data.m_WeaponData = weaponInfo.Xor( );
	data.m_flCurrentDamage = static_cast< float >( weaponInfo->m_iWeaponDamage );

	return Autowall::FireBullets( &data ) >= 1.f;
}

float GetPenetrationDamage( C_CSPlayer* local, C_WeaponCSBaseGun* pWeapon ) {
	auto weaponInfo = pWeapon->GetCSWeaponData( );
	if( !weaponInfo.IsValid( ) )
		return -1.0f;

	Autowall::C_FireBulletData data;

	data.m_iPenetrationCount = 4;
	data.m_Player = local;
	data.m_TargetPlayer = nullptr;

	QAngle view_angles;
	m_pEngine->GetViewAngles( view_angles );
	data.m_vecStart = local->GetEyePosition( );
	data.m_vecDirection = view_angles.ToVectors( );
	data.m_flMaxLength = data.m_vecDirection.Normalize( );
	data.m_WeaponData = weaponInfo.Xor( );
	data.m_flTraceLength = 0.0f;

	data.m_flCurrentDamage = static_cast< float >( weaponInfo->m_iWeaponDamage );

	CTraceFilter filter;
	filter.pSkip = local;

	Vector end = data.m_vecStart + data.m_vecDirection * weaponInfo->m_flWeaponRange;

	Autowall::TraceLine( data.m_vecStart, end, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &data.m_EnterTrace );
	Autowall::ClipTraceToPlayers( data.m_vecStart, end + data.m_vecDirection * 40.0f, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &data.m_EnterTrace );
	if( data.m_EnterTrace.fraction == 1.f )
		return -1.0f;

	data.m_flTraceLength += data.m_flMaxLength * data.m_EnterTrace.fraction;
	if( data.m_flMaxLength != 0.0f && data.m_flTraceLength >= data.m_flMaxLength )
		return data.m_flCurrentDamage;

	data.m_flCurrentDamage *= powf( weaponInfo->m_flRangeModifier, data.m_flTraceLength * 0.002f );
	data.m_EnterSurfaceData = m_pPhysSurface->GetSurfaceData( data.m_EnterTrace.surface.surfaceProps );

	C_BasePlayer* hit_player = static_cast< C_BasePlayer* >( data.m_EnterTrace.hit_entity );
	bool can_do_damage = ( data.m_EnterTrace.hitgroup >= Hitgroup_Head && data.m_EnterTrace.hitgroup <= Hitgroup_Gear );
	bool hit_target = !data.m_TargetPlayer || hit_player == data.m_TargetPlayer;
	if( can_do_damage && hit_player && hit_player->EntIndex( ) <= m_pGlobalVars->maxClients && hit_player->EntIndex( ) > 0 && hit_target ) {
		if( pWeapon && pWeapon->m_iItemDefinitionIndex( ) == WEAPON_ZEUS )
			return ( data.m_flCurrentDamage * 0.9f );

		if( pWeapon->m_iItemDefinitionIndex( ) == WEAPON_ZEUS ) {
			data.m_EnterTrace.hitgroup = Hitgroup_Generic;
		}

		data.m_flCurrentDamage = Autowall::ScaleDamage( ( C_CSPlayer* )hit_player, data.m_flCurrentDamage, weaponInfo->m_flArmorRatio, data.m_EnterTrace.hitgroup );
		return data.m_flCurrentDamage;
	};

	if( data.m_flTraceLength > 3000.0f && weaponInfo->m_flPenetration > 0.f || 0.1f > data.m_EnterSurfaceData->game.flPenetrationModifier )
		return -1.0f;

	if( Autowall::HandleBulletPenetration( &data ) )
		return -1.0f;

	return data.m_flCurrentDamage;
};

void Visuals::PenetrationCrosshair( ) {
	auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	if( !g_Vars.esp.autowall_crosshair ) {
		return;
	}

	if( pLocal->IsDead( ) ) {
		return;
	}

	C_WeaponCSBaseGun* pWeapon = ( C_WeaponCSBaseGun* )pLocal->m_hActiveWeapon( ).Get( );
	if( !pWeapon )
		return;

	if( !pWeapon->GetCSWeaponData( ).IsValid( ) )
		return;

	auto type = pWeapon->GetCSWeaponData( ).Xor( )->m_iWeaponType;

	if( type == WEAPONTYPE_KNIFE || type == WEAPONTYPE_C4 || type == WEAPONTYPE_GRENADE )
		return;

	auto screen = Render::GetScreenSize( ) / 2;
	auto dmg = ( int )GetPenetrationDamage( pLocal, pWeapon );
	bool aim = IsAimingAtPlayerThroughPenetrableWall( pLocal, pWeapon ) && dmg >= 1;
	Color color = aim ? ( Color( 0, 100, 255, 210 ) ) : ( dmg >= 1 ? Color( 0, 255, 0, 210 ) : Color( 255, 0, 0, 210 ) );

	Render::Engine::RectFilled( screen - 1, { 3, 3 }, Color( 0, 0, 0, 125 ) );
	Render::Engine::RectFilled( Vector2D( screen.x, screen.y - 1 ), Vector2D( 1, 3 ), color );
	Render::Engine::RectFilled( Vector2D( screen.x - 1, screen.y ), Vector2D( 3, 1 ), color );
}

void Visuals::Draw( ) {
	auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	if( !m_pEngine->IsInGame( ) || !m_pEngine->IsConnected( ) )
		return;

	visuals_config = &g_Vars.visuals_enemy;

	if( !visuals_config )
		return;

	if( g_Vars.esp.remove_scope ) {
		const auto pWeapon = reinterpret_cast< C_WeaponCSBaseGun* >( pLocal->m_hActiveWeapon( ).Get( ) );
		if( pWeapon ) {
			auto pWeaponData = pWeapon->GetCSWeaponData( );
			if( pWeaponData.IsValid( ) ) {
				if( pLocal->m_bIsScoped( ) && pWeaponData->m_iWeaponType == WEAPONTYPE_SNIPER_RIFLE ) {

					const auto screen = Render::GetScreenSize( );

					int w = screen.x,
						h = screen.y,
						x = w / 2,
						y = h / 2,
						size = g_Vars.cl_crosshair_sniper_width->GetInt( );

					if( size > 1 ) {
						x -= ( size / 2 );
						y -= ( size / 2 );
					}

					Render::Engine::RectFilled( 0, y, w, size, Color::Black( ) );
					Render::Engine::RectFilled( x, 0, size, h, Color::Black( ) );
				}
			}
		}
	}

	DrawWatermark( );
	PenetrationCrosshair( );

	g_ExtendedVisuals.Adjust( );

	g_Vars.globals.m_bInsideFireRange = false;

	// main entity loop
	for( int i = 1; i <= m_pEntList->GetHighestEntityIndex( ); ++i ) {
		auto entity = reinterpret_cast< C_BaseEntity* >( m_pEntList->GetClientEntity( i ) );
		if( !entity ) {

			// reset if entity got invalid
			if( i <= 64 ) {
				g_ExtendedVisuals.m_arrWeaponInfos[ i ].first = 0;
				g_ExtendedVisuals.m_arrWeaponInfos[ i ].second = nullptr;
			}

			continue;
		}

		// let's not even bother if we are out of range
		if( i <= 64 ) {
			// convert entity to csplayer
			const auto player = ToCSPlayer( entity );

			if( g_Vars.misc.ingame_radar )
				player->m_bSpotted( ) = true;

			// handle player visuals
			if( IsValidPlayer( player ) ) {
				if( visuals_config->dormant ) {
					// not dormant?
					if( !player->IsDormant( ) || g_ExtendedVisuals.m_arrSoundPlayers.at( i ).m_bValidSound ) {
						if( g_ExtendedVisuals.m_arrSoundPlayers.at( i ).m_bValidSound ) {
							// set full alpha if this was a sound based update
							player_fading_alpha.at( i ) = 1.f;
						}
						else {
							// increase alpha.
							player_fading_alpha.at( i ) += ( 5.f ) * m_pGlobalVars->frametime;
						}
					}
					else {
						if( player_fading_alpha.at( i ) < 0.6f ) {
							// decrease alpha FAST.
							player_fading_alpha.at( i ) -= ( 1.f ) * m_pGlobalVars->frametime;
						}
						else {
							// decrease alpha.
							player_fading_alpha.at( i ) -= ( 0.05f ) * m_pGlobalVars->frametime;
						}
					}
				}
				else {
					if( !player->IsDormant( ) ) {
						// increase alpha.
						player_fading_alpha.at( i ) += ( 5.f ) * m_pGlobalVars->frametime;
					}
					else {
						// decrease alpha FAST.
						player_fading_alpha.at( i ) -= ( 1.f ) * m_pGlobalVars->frametime;
					}
				}

				// now clamp it
				player_fading_alpha.at( i ) = std::clamp( player_fading_alpha.at( i ), 0.f, 1.0f );

				HandlePlayerVisuals( player );
			}
		}

		// handle world visuals
		if( IsValidEntity( entity ) ) {
			HandleWorldVisuals( entity );
		}
	}

	g_ExtendedVisuals.Restore( );
}

void Visuals::DrawWatermark( ) {
	auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	if( pLocal->IsDead( ) ) {
		return;
	}

	C_WeaponCSBaseGun* pWeapon = ( C_WeaponCSBaseGun* )pLocal->m_hActiveWeapon( ).Get( );
	if( !pWeapon )
		return;

	if( !pWeapon->GetCSWeaponData( ).IsValid( ) )
		return;

	struct Indicator_t { Color color; std::string text; };
	std::vector< Indicator_t > indicators{ };

	if( g_Vars.rage.fake_lag ) {
		float change = std::abs( Math::AngleNormalize( g_ServerAnimations.m_uServerAnimations.m_flLowerBodyYawTarget - g_AntiAim.angViewangle.y ) );

		Indicator_t ind{ };
		ind.color = g_ServerAnimations.m_uServerAnimations.m_bBreakingTeleportDst ? Color( 150, 200, 60 ) : Color( 255, 0, 0 );
		ind.text = XorStr( "LC" );

		if( g_ServerAnimations.m_uServerAnimations.m_bBreakingTeleportDst || pLocal->m_vecVelocity( ).Length2D( ) > 260.f )
			indicators.push_back( ind );
	}

	if( g_Vars.rage.anti_aim_fake_type > 0 ) {
		float change = std::abs( Math::AngleNormalize( pLocal->m_flLowerBodyYawTarget( ) - g_AntiAim.angViewangle.y ) );

		Indicator_t ind{ };
		ind.color = change > 35.f ? Color( 150, 200, 60 ) : Color( 255, 0, 0 );
		ind.text = XorStr( "LBY" );
		indicators.push_back( ind );
	}

	if( indicators.empty( ) )
		return;

	// iterate and draw indicators.
	for( size_t i{ }; i < indicators.size( ); ++i ) {
		auto& indicator = indicators[ i ];

		Render::Engine::esp_indicator.string( 12, Render::Engine::m_height - 80 - ( 30 * i ), indicator.color, indicator.text );
	}
}

void Visuals::AddDebugMessage( std::string msg ) {
	bool dont = false;

	if( debug_messages.size( ) ) {
		for( int i = 0; i < debug_messages.size( ); ++i ) {
			auto msgs = debug_messages[ i ];

			if( msgs.find( msg ) != std::string::npos )
				dont = true;
		}
	}

	if( dont ) {
		return;
	}

	debug_messages.push_back( msg );
}

void ExtendedVisuals::NormalizeSound( C_CSPlayer* player, SndInfo_t& sound ) {
	if( !player || !&sound || !sound.m_pOrigin )
		return;

	Vector src3D, dst3D;
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = player;
	src3D = ( *sound.m_pOrigin ) + Vector( 0, 0, 1 );
	dst3D = src3D - Vector( 0, 0, 100 );
	ray.Init( src3D, dst3D );

	m_pEngineTrace->TraceRay( ray, MASK_PLAYERSOLID, &filter, &tr );

	// step = (tr.fraction < 0.20)
	// shot = (tr.fraction > 0.20)
	// stand = (tr.fraction > 0.50)
	// crouch = (tr.fraction < 0.50)

	// Player stuck, idk how this happened
	if( tr.allsolid ) {
		m_arrSoundPlayers.at( sound.m_nSoundSource ).m_iReceiveTime = -1;

		m_arrSoundPlayers.at( sound.m_nSoundSource ).m_vecOrigin = *sound.m_pOrigin;
		m_arrSoundPlayers.at( sound.m_nSoundSource ).m_nFlags = player->m_fFlags( );
	}
	else {
		m_arrSoundPlayers.at( sound.m_nSoundSource ).m_vecOrigin = ( tr.fraction < 0.97 ? tr.endpos : *sound.m_pOrigin );
		m_arrSoundPlayers.at( sound.m_nSoundSource ).m_nFlags = player->m_fFlags( );
		m_arrSoundPlayers.at( sound.m_nSoundSource ).m_nFlags |= ( tr.fraction < 0.50f ? FL_DUCKING : 0 ) | ( tr.fraction != 1 ? FL_ONGROUND : 0 );
		m_arrSoundPlayers.at( sound.m_nSoundSource ).m_nFlags &= ( tr.fraction > 0.50f ? ~FL_DUCKING : 0 ) | ( tr.fraction == 1 ? ~FL_ONGROUND : 0 );
	}
}

bool ExtendedVisuals::ValidateSound( SndInfo_t& sound ) {
	if( !sound.m_bFromServer )
		return false;

	for( int i = 0; i < m_vecSoundBuffer.Count( ); i++ ) {
		const SndInfo_t& cachedSound = m_vecSoundBuffer[ i ];
		// was this sound already cached/processed?
		if( cachedSound.m_nGuid == sound.m_nGuid ) {
			return false;
		}
	}

	return true;
}

void ExtendedVisuals::Adjust( ) {
	if( !g_Vars.visuals_enemy.dormant )
		return;

	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	for( int i = 1; i <= m_pGlobalVars->maxClients; ++i ) {
		C_CSPlayer* player = C_CSPlayer::GetPlayerByIndex( i );
		if( !player ) {
			continue;
		}

		// only grab info from dead players who are spectating others
		if( !player->IsDead( ) ) {
			continue;
		}

		// we can grab info from this spectator
		if( player->IsDormant( ) ) {
			continue;
		}

		// this player is spectating someone
		if( player->m_hObserverTarget( ).IsValid( ) ) {
			// grab the player who is being spectated (pSpectatee) by the spectator (player)
			const auto pSpectatee = ( C_CSPlayer* )player->m_hObserverTarget( ).Get( );
			if( pSpectatee && pSpectatee->EntIndex( ) <= 64 ) {
				// if the guy that's being spectated is dormant, set his origin
				// to the spectators origin (and ofc set dormancy state for esp to draw him)
				if( pSpectatee->IsDormant( ) ) {
					// make sure they're actually spectating them instead of flying around or smth
					if( player->m_iObserverMode( ) == 4 || player->m_iObserverMode( ) == 5 ) {
						m_arrSpectatorPlayers.at( pSpectatee->EntIndex( ) ).m_bOverride = true;

						// xd chams for dormant niggas? 
						// std::memcpy( pSpectatee->m_CachedBoneData( ).Base( ), player->m_CachedBoneData( ).Base( ), player->m_CachedBoneData( ).Count( ) * sizeof( matrix3x4_t ) );

						m_arrSoundPlayers.at( pSpectatee->EntIndex( ) ).m_vecOrigin = player->m_vecOrigin( );
						m_arrSoundPlayers.at( pSpectatee->EntIndex( ) ).m_iReceiveTime = GetTickCount( );
					}
					// the observer mode is wronk we cant do anything
					else {
						m_arrSpectatorPlayers.at( pSpectatee->EntIndex( ) ).Reset( );
					}
				}
				// not dormant, no point to do anything coz we already see him
				else {
					m_arrSpectatorPlayers.at( pSpectatee->EntIndex( ) ).Reset( );
				}
			}
		}
	}

	CUtlVector<SndInfo_t> m_vecCurSoundList;
	m_pEngineSound->GetActiveSounds( m_vecCurSoundList );

	// No active sounds.
	if( !m_vecCurSoundList.Count( ) )
		return;

	for( int i = 0; i < m_vecCurSoundList.Count( ); i++ ) {
		SndInfo_t& sound = m_vecCurSoundList[ i ];
		if( sound.m_nSoundSource == 0 || // World
			sound.m_nSoundSource > 64 )   // Most likely invalid
			continue;

		C_CSPlayer* player = C_CSPlayer::GetPlayerByIndex( sound.m_nSoundSource );

		if( !player || !sound.m_pOrigin || !player->IsPlayer( ) || player == pLocal || player->IsTeammate( pLocal ) || sound.m_pOrigin->IsZero( ) )
			continue;

		// we have a valid spectator dormant player entry
		if( m_arrSpectatorPlayers.at( player->EntIndex( ) ).m_bOverride ) {
			// no need to do anything else for this player
			continue;
		}

		if( !ValidateSound( sound ) )
			continue;

		NormalizeSound( player, sound );

		m_arrSoundPlayers.at( sound.m_nSoundSource ).Override( sound );
	}

	for( int i = 1; i <= m_pGlobalVars->maxClients; ++i ) {
		const auto player = C_CSPlayer::GetPlayerByIndex( i );
		if( !player || player->IsDead( ) || !player->IsDormant( ) ) {
			// notify visuals that this target is officially not dormant..
			m_arrSoundPlayers.at( i ).m_bValidSound = false;
			m_arrSpectatorPlayers.at( i ).m_bOverride = false;
			continue;
		}

		constexpr int EXPIRE_DURATION = 450;
		auto& soundPlayer = m_arrSoundPlayers.at( player->EntIndex( ) );

		bool bSoundExpired = GetTickCount( ) - soundPlayer.m_iReceiveTime > EXPIRE_DURATION;
		if( bSoundExpired ) {
			continue;
		}

		// first backup the player
		SoundPlayer backupPlayer;
		backupPlayer.m_iIndex = player->m_entIndex;
		backupPlayer.m_nFlags = player->m_fFlags( );
		backupPlayer.m_vecOrigin = player->m_vecOrigin( );
		backupPlayer.m_vecAbsOrigin = player->GetAbsOrigin( );

		m_vecRestorePlayers.emplace_back( backupPlayer );

		// notify visuals that this target is officially dormant but we found a sound..
		soundPlayer.m_bValidSound = true;

		// set stuff accordingly.
		player->m_fFlags( ) = soundPlayer.m_nFlags;
		player->m_vecOrigin( ) = soundPlayer.m_vecOrigin;
		player->SetAbsOrigin( soundPlayer.m_vecOrigin );
	}

	// copy sounds (cache)
	m_vecSoundBuffer = m_vecCurSoundList;
}

void ExtendedVisuals::Restore( ) {
	if( m_vecRestorePlayers.empty( ) )
		return;

	for( auto& restorePlayer : m_vecRestorePlayers ) {
		auto player = C_CSPlayer::GetPlayerByIndex( restorePlayer.m_iIndex );
		if( !player || player->IsDormant( ) )
			continue;

		player->m_fFlags( ) = restorePlayer.m_nFlags;
		player->m_vecOrigin( ) = restorePlayer.m_vecOrigin;
		player->SetAbsOrigin( restorePlayer.m_vecAbsOrigin );

		m_arrSoundPlayers.at( restorePlayer.m_iIndex ).m_bValidSound = false;
	}

	m_vecRestorePlayers.clear( );
}