#include "BulletTracers.hpp"

#include "../../SDK/Classes/Player.hpp"
#include "../../SDK/Classes/weapon.hpp"

BeamEffects g_BeamEffects;

void BeamEffects::Draw( ) {
	const auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal )
		return;

	bool bFinalImpact = false;
	for( size_t i{ 0 }; i < m_vecBeamEffects.size( ); i++ ) {
		auto& currentImpact = m_vecBeamEffects.at( i );

		if( std::abs( m_pGlobalVars->realtime - currentImpact.m_flExpTime ) > 3.f ) {
			m_vecBeamEffects.erase( m_vecBeamEffects.begin( ) + i );
			continue;
		}

		if( currentImpact.ignore )
			continue;

		if( currentImpact.m_bRing ) {
			BeamInfo_t beamInfo;
			beamInfo.m_nType = TE_BEAMRINGPOINT;
			beamInfo.m_pszModelName = XorStr( "sprites/purplelaser1.vmt" );
			beamInfo.m_nModelIndex = m_pModelInfo->GetModelIndex( XorStr( "sprites/purplelaser1.vmt" ) );
			beamInfo.m_pszHaloName = XorStr( "sprites/purplelaser1.vmt" );
			beamInfo.m_nHaloIndex = m_pModelInfo->GetModelIndex( XorStr( "sprites/purplelaser1.vmt" ) );
			beamInfo.m_flHaloScale = 5.f;
			beamInfo.m_flLife = 1.0f;
			beamInfo.m_flWidth = 6.0f;
			beamInfo.m_flEndWidth = 6.0f;
			beamInfo.m_flFadeLength = 0.0f;
			beamInfo.m_flAmplitude = 0.0f;//2.f
			beamInfo.m_flBrightness = currentImpact.m_cColor.a( );
			beamInfo.m_flSpeed = 5.0f;
			beamInfo.m_nStartFrame = 0;
			beamInfo.m_flFrameRate = 0.f;
			beamInfo.m_flRed = currentImpact.m_cColor.r( );
			beamInfo.m_flGreen = currentImpact.m_cColor.g( );
			beamInfo.m_flBlue = currentImpact.m_cColor.b( );
			beamInfo.m_nSegments = 1;
			beamInfo.m_bRenderable = true;
			beamInfo.m_nFlags = 0;
			beamInfo.m_vecCenter = currentImpact.m_vecStartPos + Vector( 0, 0, 5 );
			beamInfo.m_flStartRadius = 1;
			beamInfo.m_flEndRadius = 550;

			auto beam = m_pRenderBeams->CreateBeamRingPoint( beamInfo );
			if( beam ) {
				m_pRenderBeams->DrawBeam( beam );
			}

			currentImpact.ignore = true;
			continue;
		}

		if( ( i + 1 ) < m_vecBeamEffects.size( ) ) {
			if( currentImpact.m_nTickBase != m_vecBeamEffects.operator[ ]( i + 1 ).m_nTickBase ) {
				bFinalImpact = true;
			}
		}
		else if( ( i == ( m_vecBeamEffects.size( ) - 1 ) ) ) {
			bFinalImpact = true;
		}
		else {
			bFinalImpact = false;
		}

		if( bFinalImpact || currentImpact.m_nIndex != pLocal->EntIndex( ) ) {
			Vector start = currentImpact.m_vecStartPos;
			Vector end = currentImpact.m_vecHitPos;

			BeamInfo_t beamInfo;
			beamInfo.m_vecStart = start;
			beamInfo.m_vecEnd = end;
			beamInfo.m_nType = 0;
			beamInfo.m_pszModelName = XorStr( "sprites/purplelaser1.vmt" );
			beamInfo.m_nModelIndex = m_pModelInfo->GetModelIndex( XorStr( "sprites/purplelaser1.vmt" ) );
			beamInfo.m_flHaloScale = 0.0f;
			beamInfo.m_flLife = 3.f;
			beamInfo.m_flWidth = 4.0f;
			beamInfo.m_flEndWidth = 4.0f;
			beamInfo.m_flFadeLength = 0.0f;
			beamInfo.m_flAmplitude = 2.0f;
			beamInfo.m_flBrightness = currentImpact.m_cColor.a( );
			beamInfo.m_flSpeed = 0.2f;
			beamInfo.m_nStartFrame = 0;
			beamInfo.m_flFrameRate = 0.f;
			beamInfo.m_flRed = currentImpact.m_cColor.r( );
			beamInfo.m_flGreen = currentImpact.m_cColor.g( );
			beamInfo.m_flBlue = currentImpact.m_cColor.b( );
			beamInfo.m_nSegments = 2;
			beamInfo.m_bRenderable = true;
			beamInfo.m_nFlags = 0x100 | 0x200 | 0x8000;

			const auto beam = m_pRenderBeams->CreateBeamPoints( beamInfo );

			if( beam ) {
				m_pRenderBeams->DrawBeam( beam );
			}

			currentImpact.ignore = true;
		}
	}
}

void BeamEffects::Add( BeamEffectsInfo_t beamEffect ) {
	m_vecBeamEffects.push_back( beamEffect );
}