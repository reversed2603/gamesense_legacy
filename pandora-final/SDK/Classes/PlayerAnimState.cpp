#include "PlayerAnimState.hpp"
#include "../../Utils/Math.hpp"
#include "../Displacement.hpp"
#include "player.hpp"

void CCSGOPlayerAnimState::Reset( ) {
	using ResetAnimState_t = void( __thiscall* )( CCSGOPlayerAnimState* );
	( ( ResetAnimState_t )Engine::Displacement.Function.m_uResetAnimState )( this );
}

void CCSGOPlayerAnimState::Update( QAngle angles ) {
	__asm
	{
		push 0
		mov ecx, this

		movss xmm1, dword ptr[ angles + 4 ]
		movss xmm2, dword ptr[ angles ]

		call Engine::Displacement.Function.m_uUpdateAnimState
	}
}

const char* CCSGOPlayerAnimState::GetWeaponPrefix( ) {
	typedef const char* ( __thiscall* fnGetWeaponPrefix )( void* );
	static const auto get_weapon_adr = Memory::Scan( XorStr( "client.dll" ), XorStr( "53 56 57 8B F9 33 F6 8B 4F 60 8B 01 FF 90 2C 04 00 00 89 47 64" ) );
	return ( ( fnGetWeaponPrefix )get_weapon_adr ) ( this );
}

float CCSGOPlayerAnimState::GetMaxFraction( ) {
	float speedFactor = Math::Clamp( m_flSpeedAsPortionOfWalkTopSpeed, 0.0f, 1.0f );
	float groundFraction = ( ( m_flWalkToRunTransition * -0.3f ) - 0.2f ) * speedFactor;
	float maxFraction = groundFraction + 1.0f;

	if( m_flAnimDuckAmount > 0.0f ) {
		float maxVelocity = Math::Clamp( m_flSpeedAsPortionOfCrouchTopSpeed, 0.0f, 1.0f );
		float duckSpeed = m_flAnimDuckAmount * maxVelocity;
		maxFraction += ( duckSpeed * ( 0.5f - maxFraction ) );
	}
	return maxFraction;
}

float CCSGOPlayerAnimState::GetDesyncDelta( bool useMinYaw ) {
	float frac = GetMaxFraction( );

	return useMinYaw ? m_flAimYawMin * frac : m_flAimYawMax * frac;
}

#include "../../Features/Rage/ServerAnimations.hpp"
int CCSGOPlayerAnimState::GetWeightedSequenceFromActivity( int activity ) {
	// kys
	if( activity != 985 ) return-1;

	int seq = 0;
	if( m_flSpeedAsPortionOfWalkTopSpeed > .25f ) {
		seq = m_flAnimDuckAmount > .55f ? 18 : 16;
	}
	else {
		seq = m_flAnimDuckAmount > .55f ? 17 : 15;
	}

	return seq;

	//m_pMDLCache->BeginLock( );
	//int new_seq = g_ServerAnimations.SelectWeightedSequenceFromModifiers( (C_CSPlayer*)this->m_pPlayer, activity, m_ActivityModifiers );
	//m_pMDLCache->EndLock( );
	//return new_seq;
}

struct mstudioposeparamdesc_t1 {
	int sznameindex;
	inline char* const pszName( void ) const { return ( ( char* )this ) + sznameindex; }
	int flags;   // ???? ( volvo, really? )
	float start; // starting value
	float end;   // ending value
	float loop;  // looping range, 0 for no looping, 360 for rotations, etc.
};
mstudioposeparamdesc_t1* pPoseParameter( CStudioHdr* hdr, int index ) {
	using poseParametorFN = mstudioposeparamdesc_t1 * ( __thiscall* )( CStudioHdr*, int );
	poseParametorFN p_pose_parameter = ( poseParametorFN )Engine::Displacement.Function.m_pPoseParameter;
	return p_pose_parameter( hdr, index );
}

void animstate_pose_param_cache_t::SetValue( C_CSPlayer* player, float flValue ) {
	auto hdr = player->m_pStudioHdr( );
	if( hdr ) {
		auto pose_param = pPoseParameter( hdr, index );
		if( !pose_param )
			return;

		auto PoseParam = *pose_param;

		if( PoseParam.loop ) {
			float wrap = ( PoseParam.start + PoseParam.end ) / 2.0f + PoseParam.loop / 2.0f;
			float shift = PoseParam.loop - wrap;

			flValue = flValue - PoseParam.loop * std::floorf( ( flValue + shift ) / PoseParam.loop );
		}

		auto ctlValue = ( flValue - PoseParam.start ) / ( PoseParam.end - PoseParam.start );
		player->m_flPoseParameter( )[ index ] = ctlValue;
	}
}
