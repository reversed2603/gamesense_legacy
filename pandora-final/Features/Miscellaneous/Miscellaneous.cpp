#include "Miscellaneous.hpp"
#include "../../SDK/variables.hpp"
#include "../../SDK/Classes/Player.hpp"
#include "../../SDK/Classes/weapon.hpp"

#include "../../SDK/Displacement.hpp"

Miscellaneous g_Misc;

void Miscellaneous::ThirdPerson( ) {
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	// for whatever reason overrideview also gets called from the main menu.
	if( !m_pEngine->IsInGame( ) )
		return;

	// no need to do thirdperson
	if( !g_Vars.misc.third_person ) {
		return;
	}

	// reset this before doing anything
	g_Misc.m_flThirdpersonTransparency = 1.f;

	// check if we have a local player and he is alive.
	const bool bAlive = !pLocal->IsDead( );
	if( bAlive ) {
		C_WeaponCSBaseGun* pWeapon = ( C_WeaponCSBaseGun* )pLocal->m_hActiveWeapon( ).Get( );

		if( !pWeapon )
			return;

		auto pWeaponData = pWeapon->GetCSWeaponData( );
		if( !pWeaponData.IsValid( ) )
			return;

		if( pWeaponData->m_iWeaponType == WEAPONTYPE_GRENADE || pLocal->m_bIsScoped( ) ) {
			g_Misc.m_flThirdpersonTransparency = 0.45f;
		}
	}


	// camera should be in thirdperson.
	if( g_Vars.misc.third_person_bind.enabled || ( g_Vars.misc.first_person_dead && !bAlive ) ) {
		// if alive and not in thirdperson already switch to thirdperson.
		if( bAlive && !m_pInput->CAM_IsThirdPerson( ) )
			m_pInput->CAM_ToThirdPerson( );

		// if dead and spectating in firstperson switch to thirdperson.
		else if( pLocal->m_iObserverMode( ) == 4 ) {
			if( g_Vars.misc.first_person_dead ) {
				// if in thirdperson, switch to firstperson.
				// we need to disable thirdperson to spectate properly.
				if( m_pInput->CAM_IsThirdPerson( ) ) {
					m_pInput->CAM_ToFirstPerson( );
				}

				pLocal->m_iObserverMode( ) = 5;
			}
		}
	}

	// camera should be in firstperson.
	else if( m_pInput->CAM_IsThirdPerson( ) ) {
		m_pInput->CAM_ToFirstPerson( );
	}

	// if after all of this we are still in thirdperson.
	if( m_pInput->CAM_IsThirdPerson( ) ) {
		// get camera angles.
		QAngle offset;
		m_pEngine->GetViewAngles( offset );

		// get our viewangle's forward directional vector.
		Vector forward;
		Math::AngleVectors( offset, forward );

		// setup thirdperson distance
		offset.z = m_pCvar->FindVar( XorStr( "cam_idealdist" ) )->GetFloat( );

		// start pos.
		const Vector origin = pLocal->GetEyePosition( true );

		// setup trace filter and trace.
		CTraceFilterWorldAndPropsOnly filter;
		CGameTrace tr;

		m_pEngineTrace->TraceRay(
			Ray_t( origin, origin - ( forward * offset.z ), { -16.f, -16.f, -16.f }, { 16.f, 16.f, 16.f } ),
			MASK_NPCWORLDSTATIC,
			( ITraceFilter* )&filter,
			&tr
		);

		m_pInput->m_fCameraInThirdPerson = true;

		m_pInput->m_vecCameraOffset = { offset.x, offset.y, offset.z * tr.fraction };

		pLocal->UpdateVisibilityAllEntities( );
	}
}

void Miscellaneous::Modulation( ) {
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer( );
	// bool bWorldModulation = g_Vars.esp.brightness_adjustment == 1;
	// bool bPropModulation = bWorldModulation;

	// float wall_alpha = std::clamp<float>( ( g_Vars.esp.transparent_walls + 5.f ) / 100.f, 0.f, 1.f );
	// float prop_alpha = std::clamp<float>( ( g_Vars.esp.transparent_props + 5.f ) / 100.f, 0.f, 1.f );

	SkyBoxChanger( );

	static float old_prop_alpha = 1.f;
	static float old_walls_alpha = 1.f;
	std::vector< IMaterial* > world = { }, props = { };
	static Color_f wall_color = Color_f( 1.f, 1.f, 1.f, 1.f );
	static bool reset = false;
	if( !pLocal || !m_pEngine->IsInGame( ) || m_pEngine->IsDrawingLoadingImage( ) ) {
		wall_color = Color_f( 1.f, 1.f, 1.f, 1.f );
		reset = true;

		old_prop_alpha = 1.f;
		old_walls_alpha = 1.f;

		return;
	}

	if( reset ) {
		if( pLocal->IsDead( ) ) {
			return;
		}
	}

	reset = false;

	const float prop_alpha = g_Vars.esp.transparent_props >= 99.f ? 1.f : g_Vars.esp.transparent_props / 100.f;
	const float wall_alpha = g_Vars.esp.transparent_walls >= 99.f ? 1.f : g_Vars.esp.transparent_walls / 100.f;

	// iterate material handles.
	for( uint16_t h{ m_pMatSystem->FirstMaterial( ) }; h != m_pMatSystem->InvalidMaterial( ); h = m_pMatSystem->NextMaterial( h ) ) {
		// get material from handle.
		IMaterial* mat = m_pMatSystem->GetMaterial( h );

		if( !mat || mat->IsErrorMaterial( ) )
			continue;

		// store world materials.
		if( hash_32_fnv1a( mat->GetTextureGroupName( ) ) == hash_32_fnv1a( "World textures" ) )
			world.push_back( mat );

		// store props.
		else if( hash_32_fnv1a( mat->GetTextureGroupName( ) ) == hash_32_fnv1a( "StaticProp textures" ) )
			props.push_back( mat );
	}

	// night.
	if( g_Vars.esp.brightness_adjustment == 1 ) {
		if( wall_color != Color_f( 0.17f, 0.16f, 0.18f ) ) {
			for( const auto& w : world )
				w->ColorModulate( 0.17f, 0.16f, 0.18f );

			for( const auto& p : props ) {
				p->AlphaModulate( 0.7f );
				p->ColorModulate( 0.5f, 0.5f, 0.5f );
			}

			wall_color = Color_f( 0.17f, 0.16f, 0.18f );
		}
	}

	// disable night.
	else {
		if( wall_color != Color_f( 1.f, 1.f, 1.f, 1.f ) ) {
			wall_color = Color_f( 1.f, 1.f, 1.f, 1.f );

			for( const auto& w : world )
				w->ColorModulate( 1.f, 1.f, 1.f );

			for( const auto& p : props ) {
				p->AlphaModulate( 1.f );
				p->ColorModulate( 1.f, 1.f, 1.f );
			}
		}
	}

	// transgender walls
	if( old_walls_alpha != wall_alpha ) {
		old_walls_alpha = wall_alpha;

		for( const auto& w : world ) {
			w->AlphaModulate( wall_alpha );
		}
	}

	if( old_prop_alpha != prop_alpha ) {
		old_prop_alpha = prop_alpha;

		for( const auto& p : props ) {
			p->AlphaModulate( prop_alpha );
		}
	}
}

void Miscellaneous::SkyBoxChanger( ) {
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer( );

	static int iOldSky = 0;

	if( !pLocal || !m_pEngine->IsInGame( ) || !m_pEngine->IsConnected( ) ) {
		iOldSky = 0;
		return;
	}

	static auto fnLoadNamedSkys = ( void( __fastcall* )( const char* ) )Engine::Displacement.Function.m_uLoadNamedSkys;
	static ConVar* sv_skyname = m_pCvar->FindVar( XorStr( "sv_skyname" ) );
	if( sv_skyname ) {

		if( iOldSky != g_Vars.esp.brightness_adjustment ) {
			const char* sky_name = g_Vars.esp.brightness_adjustment == 1 ? "sky_csgo_night02" : sv_skyname->GetString( );
			fnLoadNamedSkys( sky_name );

			iOldSky = g_Vars.esp.brightness_adjustment;
		}
	}
}

void Miscellaneous::PreserveKillfeed( ) {
	auto local = C_CSPlayer::GetLocalPlayer( );

	if( !local || !m_pEngine->IsInGame( ) || !m_pEngine->IsConnected( ) ) {
		return;
	}

	static auto bStatus = false;
	static float m_flSpawnTime = local->m_flSpawnTime( );

	auto bSet = false;
	if( m_flSpawnTime != local->m_flSpawnTime( ) || bStatus != g_Vars.esp.preserve_killfeed ) {
		bSet = true;
		bStatus = g_Vars.esp.preserve_killfeed;
		m_flSpawnTime = local->m_flSpawnTime( );
	}

	for( int i = 0; i < m_pDeathNotices->m_vecDeathNotices.Count( ); i++ ) {
		auto cur = &m_pDeathNotices->m_vecDeathNotices[ i ];
		if( !cur ) {
			continue;
		}

		if( local->IsDead( ) || bSet ) {
			if( cur->set != 1.f && !bSet ) {
				continue;
			}

			cur->m_flStartTime = m_pGlobalVars->curtime;
			cur->m_flStartTime -= local->m_iHealth( ) <= 0 ? 2.f : 7.5f;
			cur->set = 2.f;

			continue;
		}

		if( cur->set == 2.f ) {
			continue;
		}

		if( !bStatus ) {
			cur->set = 1.f;
			return;
		}

		if( cur->set == 1.f ) {
			continue;
		}

		if( cur->m_flLifeTimeModifier == 1.5f ) {
			cur->m_flStartTime = FLT_MAX;
		}

		cur->set = 1.f;
	}
}

void Miscellaneous::RemoveSmoke( ) {
	if( !m_pEngine->IsInGame( ) )
		return;

	static uint32_t* pSmokeCount = nullptr;
	if( !pSmokeCount ) {
		pSmokeCount = *reinterpret_cast< uint32_t** > ( Memory::Scan( XorStr( "client.dll" ), XorStr( "A3 ? ? ? ? 57 8B CB" ) ) + 0x1 );
	}

	if( g_Vars.esp.remove_smoke ) {
		*pSmokeCount = 0;
	}

	static IMaterial* pSmokeGrenade = nullptr;
	if( !pSmokeGrenade ) {
		pSmokeGrenade = m_pMatSystem->FindMaterial( XorStr( "particle/vistasmokev1/vistasmokev1_smokegrenade" ), nullptr );
	}

	static IMaterial* pSmokeFire = nullptr;
	if( !pSmokeFire ) {
		pSmokeFire = m_pMatSystem->FindMaterial( XorStr( "particle/vistasmokev1/vistasmokev1_fire" ), nullptr );
	}

	static IMaterial* pSmokeDust = nullptr;
	if( !pSmokeDust ) {
		pSmokeDust = m_pMatSystem->FindMaterial( XorStr( "particle/vistasmokev1/vistasmokev1_emods_impactdust" ), nullptr );
	}

	static IMaterial* pSmokeMods = nullptr;
	if( !pSmokeMods ) {
		pSmokeMods = m_pMatSystem->FindMaterial( XorStr( "particle/vistasmokev1/vistasmokev1_emods" ), nullptr );
	}

	if( !pSmokeGrenade || !pSmokeFire || !pSmokeDust || !pSmokeMods ) {
		return;
	}

	pSmokeGrenade->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, g_Vars.esp.remove_smoke );
	pSmokeGrenade->IncrementReferenceCount( );

	pSmokeFire->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, g_Vars.esp.remove_smoke );
	pSmokeFire->IncrementReferenceCount( );

	pSmokeDust->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, g_Vars.esp.remove_smoke );
	pSmokeDust->IncrementReferenceCount( );

	pSmokeMods->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, g_Vars.esp.remove_smoke );
	pSmokeMods->IncrementReferenceCount( );

	// nitro code (alt to forcing cvar etc)
	static auto DisablePostProcess = Memory::Scan( XorStr( "client.dll" ), XorStr( "80 3D ? ? ? ? ? 53 56 57 0F 85" ) ) + 0x2;

	// get post process address
	static bool* disable_post_process = *reinterpret_cast< bool** >( DisablePostProcess );

	// set it.
	//if( *disable_post_process != g_Vars.esp.remove_post_proccesing )
	//	*disable_post_process = g_Vars.esp.remove_post_proccesing;

	enum PostProcessParameterNames_t {
		PPPN_FADE_TIME = 0,
		PPPN_LOCAL_CONTRAST_STRENGTH,
		PPPN_LOCAL_CONTRAST_EDGE_STRENGTH,
		PPPN_VIGNETTE_START,
		PPPN_VIGNETTE_END,
		PPPN_VIGNETTE_BLUR_STRENGTH,
		PPPN_FADE_TO_BLACK_STRENGTH,
		PPPN_DEPTH_BLUR_FOCAL_DISTANCE,
		PPPN_DEPTH_BLUR_STRENGTH,
		PPPN_SCREEN_BLUR_STRENGTH,
		PPPN_FILM_GRAIN_STRENGTH,

		POST_PROCESS_PARAMETER_COUNT
	};

	struct PostProcessParameters_t {
		PostProcessParameters_t( ) {
			memset( m_flParameters, 0, sizeof( m_flParameters ) );
			m_flParameters[ PPPN_VIGNETTE_START ] = 0.8f;
			m_flParameters[ PPPN_VIGNETTE_END ] = 1.1f;
		}

		float m_flParameters[ POST_PROCESS_PARAMETER_COUNT ];
	};

	static auto PostProcessParameters = *reinterpret_cast< PostProcessParameters_t** >( Memory::Scan( XorStr( "client.dll" ), XorStr( "0F 11 05 ? ? ? ? 0F 10 87" ) ) + 3 );
	PostProcessParameters->m_flParameters[ PPPN_VIGNETTE_BLUR_STRENGTH ] = 0.f;
	PostProcessParameters->m_flParameters[ PPPN_SCREEN_BLUR_STRENGTH ] = 0.f;
}

void Miscellaneous::ForceCrosshair( ) {
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal ) {
		return;
	}

	static auto weapon_debug_spread_show = m_pCvar->FindVar( XorStr( "weapon_debug_spread_show" ) );
	if( !weapon_debug_spread_show ) {
		return;
	}

	static bool bReset = false;
	if( !g_Vars.esp.force_sniper_crosshair ) {
		if( bReset ) {
			weapon_debug_spread_show->SetValue( 0 );
			bReset = false;
		}

		return;
	}

	const int nCrosshairValue = pLocal->m_bIsScoped( ) ? 0 : 3;
	if( weapon_debug_spread_show->GetInt( ) != nCrosshairValue )
		weapon_debug_spread_show->SetValue( nCrosshairValue );

	bReset = true;
}