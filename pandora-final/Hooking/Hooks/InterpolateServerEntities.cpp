#include "../Hooked.hpp"
#include "../../SDK/Displacement.hpp"
#include "../../SDK/Classes/Player.hpp"
#include "../../Features/Rage/LagCompensation.hpp"
#include "../../Features/Rage/ServerAnimations.hpp"

void __cdecl Hooked::InterpolateServerEntities( ) {
	g_Vars.globals.szLastHookCalled = XorStr( "11" );
	//if( !g_Vars.globals.RenderIsReady )
	//	return oInterpolateServerEntities( );

	//return oInterpolateServerEntities( );

	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal || !m_pEngine->IsInGame( ) )
		return oInterpolateServerEntities( );

	oInterpolateServerEntities( );

	auto pStudioHdr = pLocal->m_pStudioHdr( );

	// fix server model origin
	{
		pLocal->SetAbsAngles( QAngle( 0.0f, g_ServerAnimations.m_uServerAnimations.m_flFootYaw, 0.0f ) );

		matrix3x4_t matWorldMatrix{ };
		matWorldMatrix.AngleMatrix( QAngle( 0.f, g_ServerAnimations.m_uServerAnimations.m_flFootYaw, 0.f ), pLocal->GetAbsOrigin( ) );

		if( pStudioHdr ) {
			uint8_t uBoneComputed[ 0x20 ] = { 0 };
			pLocal->BuildTransformations( pStudioHdr, g_ServerAnimations.m_uServerAnimations.m_vecBonePos, g_ServerAnimations.m_uServerAnimations.m_quatBoneRot,
				matWorldMatrix, BONE_USED_BY_ANYTHING, uBoneComputed );
		}

		pLocal->InvalidateBoneCache( );

		auto pBackupBones = pLocal->m_BoneAccessor( ).m_pBones;
		pLocal->m_BoneAccessor( ).m_pBones = pLocal->m_CachedBoneData( ).Base( );

		if( pStudioHdr ) {
			using AttachmentHelper_t = void( __thiscall* )( C_CSPlayer*, CStudioHdr* );
			static AttachmentHelper_t AttachmentHelperFn = ( AttachmentHelper_t )Engine::Displacement.Function.m_AttachmentHelper;
			AttachmentHelperFn( pLocal, pStudioHdr );
		}

		pLocal->m_BoneAccessor( ).m_pBones = pBackupBones;
	}

	for( int i = 1; i <= m_pGlobalVars->maxClients; ++i ) {
		const auto pEntity = ( C_CSPlayer* )m_pEntList->GetClientEntity( i );
		if( !pEntity )
			continue;

		if( pEntity->EntIndex( ) == m_pEngine->GetLocalPlayer( ) )
			continue;

		if( pEntity->IsDead( ) )
			continue;

		if( pEntity->IsDormant( ) )
			continue;

		// generate visual matrix
		pEntity->SetupBones( nullptr, 128, 0x7FF00, pEntity->m_flSimulationTime( ) );
	}
}