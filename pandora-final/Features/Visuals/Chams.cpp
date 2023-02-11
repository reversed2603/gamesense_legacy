#include "Chams.hpp"
#include "../../source.hpp"
#include <fstream>
#include "../../SDK/Classes/Player.hpp"
#include "../../SDK/Classes/IMaterialSystem.hpp"
#include "../../SDK/variables.hpp"
#include "../Rage/LagCompensation.hpp"
#include "../../Hooking/hooked.hpp"
#include "../../SDK/displacement.hpp"
#include "../Rage/Resolver.hpp"
#include "../Rage/EnginePrediction.hpp"
#include "../Rage/ServerAnimations.hpp"
#include "../Miscellaneous/Miscellaneous.hpp"
#include "../Visuals/Visuals.hpp"

#include <cstddef>
#include <string_view>
#include <iostream>
#include <string_view>
#include <string>

Chams g_Chams;

bool Chams::CreateMaterials( ) {
	if( m_bInit )
		return true;

	std::ofstream( XorStr( "csgo\\materials\\pdr_shine.vmt" ) ) << XorStr( R"#("VertexLitGeneric"
			{
					"$basetexture"				"vgui/white_additive"
					"$ignorez"					"0"
					"$phong"					"1"
					"$BasemapAlphaPhongMask"    "1"
					"$phongexponent"			"15"
					"$normalmapalphaenvmask"	"1"
					"$envmap"					"env_cubemap"
					"$envmaptint"				"[0.0 0.0 0.0]"
					"$phongboost"				"[0.6 0.6 0.6]"
					"phongfresnelranges"		"[0.5 0.5 1.0]"
					"$nofog"					"1"
					"$model"					"1"
					"$nocull"					"0"
					"$selfillum"				"1"
					"$halflambert"				"1"
					"$znearer"					"0"
					"$flat"						"0"	
					"$rimlight"					"1"
					"$rimlightexponent"			"2"
					"$rimlightboost"			"0"
			}
			)#" );

	m_matFlat = m_pMatSystem->FindMaterial( ( "debug/debugdrawflat" ), nullptr );
	m_matRegular = m_pMatSystem->FindMaterial( XorStr( "pdr_shine" ), TEXTURE_GROUP_MODEL );

	if( !m_matRegular || m_matRegular == nullptr || m_matRegular->IsErrorMaterial( ) )
		return false;

	if( !m_matFlat || m_matFlat == nullptr || m_matFlat->IsErrorMaterial( ) )
		return false;

	m_bInit = true;
	return true;
}

void Chams::OverrideMaterial( bool xqz, const Color_f& color, IMaterial* pOverrideMaterial ) {
	IMaterial* material = nullptr;

	switch( g_Vars.esp.chams_material ) {
	case MATERIAL_REGULAR:
		material = m_matRegular;
		break;
	case MATERIAL_FLAT:
		material = m_matFlat;
		break;
	default:
		material = m_matRegular;
		break;
	}

	if( pOverrideMaterial ) {
		material = pOverrideMaterial;
	}

	if( !material ) {
		m_pStudioRender->m_pForcedMaterial = nullptr;
		m_pStudioRender->m_nForcedMaterialType = 0;
		return;
	}

	// apparently we have to do this, otherwise SetMaterialVarFlag can cause crashes (I crashed once here when loading a different cfg)
	material->IncrementReferenceCount( );

	material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, xqz );

	if( material == m_matRegular ) {
		material->AlphaModulate( color.a );
		material->ColorModulate( color.r, color.g, color.b );

		auto $envmaptint = material->FindVar( XorStr( "$envmaptint" ), nullptr );
		if( $envmaptint )
			$envmaptint->SetVecValue( g_Vars.esp.chams_reflectivity_color.r * ( g_Vars.esp.chams_reflectivity / 100.f ),
				g_Vars.esp.chams_reflectivity_color.g * ( g_Vars.esp.chams_reflectivity / 100.f ),
				g_Vars.esp.chams_reflectivity_color.b * ( g_Vars.esp.chams_reflectivity / 100.f ) );

		auto $phongboost = material->FindVar( XorStr( "$phongboost" ), nullptr );
		if( $phongboost )
			$phongboost->SetVecValue( g_Vars.esp.chams_shine / 100.f, g_Vars.esp.chams_shine / 100.f, g_Vars.esp.chams_shine / 100.f );

		auto $rimlightboost = material->FindVar( XorStr( "$rimlightboost" ), nullptr );
		if( $rimlightboost )
			$rimlightboost->SetFloatValue( g_Vars.esp.chams_rim / 100.f );
	}
	else {
		material->AlphaModulate(
			color.a );

		material->ColorModulate(
			color.r,
			color.g,
			color.b );
	}

	m_pStudioRender->m_pForcedMaterial = material;
	m_pStudioRender->m_nForcedMaterialType = 0;
}

bool Chams::PassesSanityChecks( C_CSPlayer* pLocal, C_CSPlayer* pEntity, ModelRenderInfo_t& RenderInfo ) {
	if( !m_pStudioRender.IsValid( ) )
		return false;

	if( !CreateMaterials( ) )
		return false;

	if( !m_pEngine->IsInGame( ) )
		return false;

	if( RenderInfo.flags & 0x40000000 || !RenderInfo.pRenderable || !RenderInfo.pRenderable->GetIClientUnknown( ) )
		return false;

	auto pClientClass = pEntity->GetClientClass( );
	if( !pClientClass )
		return false;

	if( pEntity->IsDormant( ) )
		return false;

	// entity is dead and it isn't a ragdoll
	if( pEntity->IsDead( ) && pClientClass->m_ClassID != CCSRagdoll )
		return false;

	return true;
}

bool Chams::UsingGlowEffect( int nIndex ) {
	if( !g_Vars.globals.m_bInPostScreenEffects )
		return false;

	if( !m_pStudioRender->m_pForcedMaterial )
		return m_pStudioRender->m_nForcedMaterialType == 2 || m_pStudioRender->m_nForcedMaterialType == 4;

	return strstr( m_pStudioRender->m_pForcedMaterial->GetName( ), XorStr( "dev/glow" ) ) != nullptr;
}

void Chams::OnDrawModelExecute( void* ECX, IMatRenderContext* MatRenderContext, DrawModelState_t& DrawModelState, ModelRenderInfo_t& RenderInfo, matrix3x4_t* pBoneToWorld ) {
	auto pEntity = ( C_CSPlayer* )RenderInfo.pRenderable->GetIClientUnknown( )->GetBaseEntity( );
	if( !pEntity ) {
		return Hooked::oDrawModelExecute( ECX, MatRenderContext, DrawModelState, RenderInfo, pBoneToWorld );
	}

	auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal ) {
		return Hooked::oDrawModelExecute( ECX, MatRenderContext, DrawModelState, RenderInfo, pBoneToWorld );
	}

	auto pClientClass = pEntity->GetClientClass( );
	if( !pClientClass ) {
		return Hooked::oDrawModelExecute( ECX, MatRenderContext, DrawModelState, RenderInfo, pBoneToWorld );
	}

	if( !PassesSanityChecks( pLocal, pEntity, RenderInfo ) ) {
		return Hooked::oDrawModelExecute( ECX, MatRenderContext, DrawModelState, RenderInfo, pBoneToWorld );
	}

	if( pClientClass->m_ClassID == CBaseAnimating ) {
		if( g_Vars.esp.remove_sleeves && strstr( DrawModelState.m_pStudioHdr->szName, XorStr( "sleeve" ) ) != nullptr )
			return;
	}

	// until we add custom glow
	if( UsingGlowEffect( pEntity->EntIndex( ) ) ) {
		return Hooked::oDrawModelExecute( ECX, MatRenderContext, DrawModelState, RenderInfo, pBoneToWorld );
	}

	auto oFn = [ & ] ( ) {
		if( m_pOverrideMatrix ) {
			return Hooked::oDrawModelExecute( ECX, MatRenderContext, DrawModelState, RenderInfo, m_pOverrideMatrix );
		}

		Hooked::oDrawModelExecute( ECX, MatRenderContext, DrawModelState, RenderInfo, pBoneToWorld );
	};

	// player chams
	if( pEntity->IsPlayer( ) ) {
		if( HandlePlayerChams( pEntity, oFn ) ) {
			return;
		}
	}

	Hooked::oDrawModelExecute( ECX, MatRenderContext, DrawModelState, RenderInfo, pBoneToWorld );

	m_pStudioRender->m_pForcedMaterial = nullptr;
	m_pStudioRender->m_nForcedMaterialType = 0;
}

bool Chams::HandlePlayerChams( C_CSPlayer* player, std::function<void( )> oFn ) {
	auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal ) {
		return false;
	}

	if( g_Misc.m_flThirdpersonTransparency != 1.f && player->EntIndex( ) == pLocal->EntIndex( ) ) {
		m_pRenderView->SetBlend( g_Misc.m_flThirdpersonTransparency );
		oFn( );
		return true;
	}

	if( !g_Vars.esp.chams_teammates ) {
		if( player->IsTeammate( pLocal, false ) ) {
			return false;
		}
	}

	auto InvalidateMaterial = [ & ] ( ) -> void {
		m_pStudioRender->m_pForcedMaterial = nullptr;
		m_pStudioRender->m_nForcedMaterialType = 0;
	};

	bool bReturn = false;

	// bReturn shouldn't be true here because this and the player wall chams are 
	// only supposed to work behind walls (so we still should render the
	// regular player model when visible, if we have regular chams disabled)

	matrix3x4_t pShadowMatrix[ 128 ];
	if( g_Vars.esp.chams_shadow && GetCustomMatrix( player, pShadowMatrix, true ) ) {
		m_pOverrideMatrix = pShadowMatrix;

		OverrideMaterial( true, g_Vars.esp.chams_shadow_color, m_matFlat );
		oFn( );

		m_pOverrideMatrix = nullptr;
	}

	if( player != pLocal ) {
		if( g_Vars.esp.chams_player_wall ) {
			OverrideMaterial( true, ( g_Vars.esp.chams_teammates && player->IsTeammate( pLocal ) ) ? g_Vars.esp.chams_teammates_color : g_Vars.esp.chams_player_wall_color );
			oFn( );
		}
	}

	if( g_Vars.esp.chams_player ) {
		OverrideMaterial( false, ( g_Vars.esp.chams_teammates && player->IsTeammate( pLocal ) ) ? g_Vars.esp.chams_teammates_color : g_Vars.esp.chams_player_color );
		oFn( );

		bReturn = true;
	}

	InvalidateMaterial( );
	return bReturn;
}

bool Chams::GetCustomMatrix( C_CSPlayer* entity, matrix3x4_t* out, bool bBacktrack ) {
	if( !entity )
		return false;

	auto data = g_LagCompensation.GetLagData( entity->m_entIndex );
	if( data ) {

		if( bBacktrack ) {
			// start from begin
			for( auto it = data->m_Records.begin( ); it != data->m_Records.end( ); ++it ) {
				if( it->player != entity )
					break;

				std::pair< C_LagRecord*, C_LagRecord* > last;
				if( !g_LagCompensation.IsRecordOutOfBounds( *it, 0.2f, -1, false ) && it + 1 != data->m_Records.end( ) && g_LagCompensation.IsRecordOutOfBounds( *( it + 1 ), 0.2f, -1, false ) )
					last = std::make_pair( &*( it + 1 ), &*it );

				if( !last.first || !last.second )
					continue;

				const auto& pFirstInvalid = last.first;
				const auto& pLastInvalid = last.second;

				if( !pLastInvalid || !pFirstInvalid )
					continue;

				if( pLastInvalid->m_flSimulationTime - pFirstInvalid->m_flSimulationTime > 0.5f )
					continue;

				if( ( pFirstInvalid->m_vecOrigin - entity->m_vecOrigin( ) ).Length( ) < 2.5f )
					continue;

				const auto NextOrigin = pLastInvalid->m_vecOrigin;
				const auto curtime = m_pGlobalVars->curtime;

				auto flDelta = 1.f - ( curtime - pLastInvalid->m_flInterpolateTime ) / ( pLastInvalid->m_flSimulationTime - pFirstInvalid->m_flSimulationTime );
				if( flDelta < 0.f || flDelta > 1.f )
					pLastInvalid->m_flInterpolateTime = curtime;

				flDelta = 1.f - ( curtime - pLastInvalid->m_flInterpolateTime ) / ( pLastInvalid->m_flSimulationTime - pFirstInvalid->m_flSimulationTime );

				const auto lerp = Math::Interpolate( NextOrigin, pFirstInvalid->m_vecOrigin, std::clamp( flDelta, 0.f, 1.f ) );

				matrix3x4_t ret[ 128 ];
				auto pMatrix = pFirstInvalid->m_pMatrix;
				memcpy( ret, pMatrix, sizeof( matrix3x4_t[ 128 ] ) );

				for( size_t i{ }; i < 128; ++i ) {
					const auto matrix_delta = Math::MatrixGetOrigin( pMatrix[ i ] ) - pFirstInvalid->m_vecOrigin;
					Math::MatrixSetOrigin( matrix_delta + lerp, ret[ i ] );
				}

				memcpy( out, ret, sizeof( matrix3x4_t[ 128 ] ) );
				return true;
			}
		}
	}

	return false;
}