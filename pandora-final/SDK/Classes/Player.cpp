#include "player.hpp"
#include "../displacement.hpp"
#include "../source.hpp"
#include "weapon.hpp"
#include "../../Features/Rage/TickbaseShift.hpp"
#include "../Classes/PropManager.hpp"
#include "../../Features/Rage/animationsystem.hpp"

QAngle& C_BasePlayer::m_aimPunchAngle( ) {
	return *( QAngle* )( ( uintptr_t )this + Engine::Displacement.DT_BasePlayer.m_aimPunchAngle );
}

QAngle& C_BasePlayer::m_aimPunchAngleVel( ) {
	return *( QAngle* )( ( uintptr_t )this + Engine::Displacement.DT_BasePlayer.m_aimPunchAngleVel );
}

QAngle& C_BasePlayer::m_viewPunchAngle( ) {
	return *( QAngle* )( ( uintptr_t )this + Engine::Displacement.DT_BasePlayer.m_viewPunchAngle );
}

Vector& C_BasePlayer::m_vecViewOffset( ) {
	return *( Vector* )( ( uintptr_t )this + Engine::Displacement.DT_BasePlayer.m_vecViewOffset );
}
Vector& C_BasePlayer::m_vecVelocity( ) {
	return *( Vector* )( ( uintptr_t )this + Engine::Displacement.DT_BasePlayer.m_vecVelocity );
}

Vector& C_BasePlayer::m_vecNetworkOrigin( ) {
	static auto offset = Memory::FindInDataMap( GetPredDescMap( ), XorStr( "m_vecNetworkOrigin" ) );
	return *( Vector* )( ( uintptr_t )this + offset );
}

Vector& C_BasePlayer::m_vecBaseVelocity( ) {
	return *( Vector* )( ( uintptr_t )this + Engine::Displacement.DT_BasePlayer.m_vecBaseVelocity );
}

Vector& C_BasePlayer::m_vecLadderNormal( ) {
	return *( Vector* )( ( uintptr_t )this + Engine::Displacement.DT_BasePlayer.m_vecLadderNormal );
}

float& C_BasePlayer::m_flFallVelocity( ) {
	return *( float* )( ( uintptr_t )this + Engine::Displacement.DT_BasePlayer.m_flFallVelocity );
}

float& C_BasePlayer::m_flDuckAmount( ) {
	return *( float* )( ( uintptr_t )this + Engine::Displacement.DT_BasePlayer.m_flDuckAmount );
}

float& C_BasePlayer::m_flDuckSpeed( ) {
	return *( float* )( ( uintptr_t )this + Engine::Displacement.DT_BasePlayer.m_flDuckSpeed );
}

char& C_BasePlayer::m_lifeState( ) {
	return *( char* )( ( uintptr_t )this + Engine::Displacement.DT_BasePlayer.m_lifeState );
}

int& C_BasePlayer::m_nTickBase( ) {
	return *( int* )( ( uintptr_t )this + Engine::Displacement.DT_BasePlayer.m_nTickBase );
}

int& C_BasePlayer::m_iHealth( ) {
	return *( int* )( ( uintptr_t )this + Engine::Displacement.DT_BasePlayer.m_iHealth );
}

int C_BasePlayer::m_iMaxHealth( ) {
	return 100;
}

bool& C_BasePlayer::m_bSpotted( ) {
	return *( bool* )( ( uintptr_t )this + Engine::Displacement.DT_BaseEntity.m_bSpotted );
}

int& C_BasePlayer::m_fFlags( ) {
	return *( int* )( ( uintptr_t )this + Engine::Displacement.DT_BasePlayer.m_fFlags );
}

int& C_BasePlayer::m_iDefaultFOV( ) {
	return *( int* )( ( uintptr_t )this + Engine::Displacement.DT_BasePlayer.m_iDefaultFOV );
}

int& C_BasePlayer::m_iObserverMode( ) {
	return *( int* )( ( uintptr_t )this + Engine::Displacement.DT_BasePlayer.m_iObserverMode );
}

CPlayerState& C_BasePlayer::pl( ) {
	return *( CPlayerState* )( ( uintptr_t )this + Engine::Displacement.DT_BasePlayer.pl );
}

CBaseHandle& C_BasePlayer::m_hObserverTarget( ) {
	return *( CBaseHandle* )( ( uintptr_t )this + Engine::Displacement.DT_BasePlayer.m_hObserverTarget );
}

CHandle<C_BaseViewModel> C_BasePlayer::m_hViewModel( ) {
	return *( CHandle<C_BaseViewModel>* )( ( uintptr_t )this + Engine::Displacement.DT_BasePlayer.m_hViewModel );
}

float C_BasePlayer::GetMaxSpeed( ) {
	if( !this )
		return 250.0f;

	auto hWeapon = this->m_hActiveWeapon( );
	if( hWeapon == -1 )
		return 250.0f;

	auto pWeapon = reinterpret_cast< C_WeaponCSBaseGun* >( this->m_hActiveWeapon( ).Get( ) );
	if( !pWeapon )
		return 250.0f;

	auto pWeaponData = pWeapon->GetCSWeaponData( );
	if( !pWeaponData.IsValid( ) )
		return 250.0f;

	if( pWeapon->m_zoomLevel( ) == 0 )
		return pWeaponData->m_flMaxSpeed;

	return pWeaponData->m_flMaxSpeed2;
}


float C_BasePlayer::SequenceDuration( CStudioHdr* pStudioHdr, int iSequence ) {
	//55 8B EC 53 57 8B 7D 08 8B D9 85 FF 75 double __userpurge SequenceDuration@<st0>(int a1@<ecx>, float a2@<xmm0>, int *a3, int a4)
	using SequenceDurationFn = float( __thiscall* )( void*, CStudioHdr*, int );
	return Memory::VCall< SequenceDurationFn >( this, 221 )( this, pStudioHdr, iSequence );
}

const Vector& C_BasePlayer::WorldSpaceCenter( ) {
	using WorldSpaceCenterFn = const Vector& ( __thiscall* )( void* );
	return Memory::VCall< WorldSpaceCenterFn >( this, 78 )( this );
}

#pragma runtime_checks( "", off )
float C_BasePlayer::GetSequenceMoveDist( CStudioHdr* pStudioHdr, int iSequence ) {
	Vector vecReturn;

	// int __usercall GetSequenceLinearMotion@<eax>(int a1@<edx>, _DWORD *a2@<ecx>, int a3, _DWORD *a4)
	// it fastcall, but edx and ecx swaped
	// xref: Bad pstudiohdr in GetSequenceLinearMotion()!\n | Bad sequence (%i out of %i max) in GetSequenceLinearMotion() for model '%s'!\n
	static uintptr_t ptr = Memory::CallableFromRelative( Memory::Scan( XorStr( "client.dll" ), XorStr( "E8 ? ? ? ? F3 0F 10 4D ? 83 C4 08 F3 0F 10 45 ? F3 0F 59 C0" ) ) );

	using GetSequenceLinearMotionFn = int( __fastcall* )( CStudioHdr*, int, float*, Vector* );
	( ( GetSequenceLinearMotionFn )ptr )( pStudioHdr, iSequence, m_flPoseParameter( ), &vecReturn );
	__asm {
		add esp, 8
	}

	return vecReturn.Length( );
}
#pragma runtime_checks( "", restore )

bool C_BasePlayer::IsDead( ) {
	if( !this )
		return false;

	return ( this->m_lifeState( ) );
}

void C_BasePlayer::SetCurrentCommand( CUserCmd* cmd ) {
	*( CUserCmd** )( ( uintptr_t )this + Engine::Displacement.C_BasePlayer.m_pCurrentCommand ) = cmd;
}

Vector C_BasePlayer::GetEyePosition( ) {
	Vector vecOrigin = this->m_vecOrigin( );

	Vector offset = this->m_vecViewOffset( );
	if( offset.z >= 46.1f ) {
		if( offset.z > 64.0f ) {
			offset.z = 64.0f;
		}
	}
	else {
		offset.z = 46.0f;
	}
	vecOrigin += offset;

	return vecOrigin;
}

Vector C_BasePlayer::GetViewHeight( ) {
	Vector offset;
	if( this->m_flDuckAmount( ) == 0.0f ) {
		offset = m_pGameMovement->GetPlayerViewOffset( false );
	}
	else {
		offset = m_vecViewOffset( );
	}
	return offset;
}

//float C_BasePlayer::GetLayerSequenceCycleRate( C_AnimationLayer* pLayer, int iSequence ) {
//	using GetLayerSequenceCycleRateFn = float( __thiscall* )( void*, C_AnimationLayer*, int );
//	return Memory::VCall< GetLayerSequenceCycleRateFn >( this, 222 )( this, pLayer, iSequence );
//}

C_AnimationLayer& C_BasePlayer::GetAnimLayer( int index ) {
	// ref: CCBaseEntityAnimState::ComputePoseParam_MoveYaw
	// move_x move_y move_yaw
	typedef C_AnimationLayer& ( __thiscall* Fn )( void*, int, bool );
	static Fn fn = NULL;

	if( !fn )
		fn = ( Fn )Memory::Scan( XorStr( "client.dll" ), XorStr( "55 8B EC 57 8B F9 8B 97 ? ? ? ? 85 D2" ) );

	index = Math::Clamp( index, 0, 13 );

	return fn( this, index, true );
}

// L3D451R7, raxer23 (c)
void C_BasePlayer::TryInitiateAnimation( C_AnimationLayer* pLayer, int nSequence ) {
	if( !pLayer || nSequence < 2 )
		return;

	pLayer->m_flPlaybackRate = GetLayerSequenceCycleRate( pLayer, nSequence );
	pLayer->m_nSequence = nSequence;
	pLayer->m_flCycle = pLayer->m_flWeight = 0.f;
};

C_CSPlayer* C_CSPlayer::GetLocalPlayer( ) {
	if( g_Vars.globals.m_bForceFullUpdate )
		return nullptr;

	auto index = m_pEngine->GetLocalPlayer( );

	if( !index )
		return nullptr;

	auto client = m_pEntList->GetClientEntity( index );

	if( !client )
		return nullptr;

	return ToCSPlayer( client->GetBaseEntity( ) );
}

C_CSPlayer* C_CSPlayer::GetPlayerByIndex( int index ) {
	if( g_Vars.globals.m_bForceFullUpdate )
		return nullptr;

	if( !index )
		return nullptr;

	auto client = m_pEntList->GetClientEntity( index );

	if( !client )
		return nullptr;

	return ToCSPlayer( client->GetBaseEntity( ) );
}

std::array<int, 5>& C_CSPlayer::m_vecPlayerPatchEconIndices( )
{
	return *( std::array<int, 5>* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_vecPlayerPatchEconIndices );
}

CCSGOPlayerAnimState*& C_CSPlayer::m_PlayerAnimState( ) {
	return *( CCSGOPlayerAnimState** )( ( uintptr_t )this + Engine::Displacement.C_CSPlayer.m_PlayerAnimState );
}

QAngle& C_CSPlayer::m_angEyeAngles( ) {
	return *( QAngle* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_angEyeAngles );
}

int& C_CSPlayer::m_nSurvivalTeam( ) {
	return *( int* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_nSurvivalTeam );
}

int& C_CSPlayer::m_ArmorValue( ) {
	return *( int* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_ArmorValue );
}

int& C_CSPlayer::m_iAccount( ) {
	return *( int* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_iAccount );
}

int& C_CSPlayer::m_iFOV( ) {
	return *( int* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_iFOV );
}

int& C_CSPlayer::m_iShotsFired( ) {
	return *( int* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_iShotsFired );
}

float& C_CSPlayer::m_flFlashDuration( ) {
	return *( float* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_flFlashDuration );
}

float& C_CSPlayer::m_flSecondFlashAlpha( ) {
	return *( float* )( uintptr_t( this ) + Engine::Displacement.DT_CSPlayer.m_flFlashDuration - 0xC );
}

float& C_CSPlayer::m_flVelocityModifier( ) {
	return *( float* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_flVelocityModifier );
}

float& C_CSPlayer::m_flLowerBodyYawTarget( ) {
	return *( float* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_flLowerBodyYawTarget );
}

float& C_CSPlayer::m_flSpawnTime( ) {
	return *( float* )( ( uintptr_t )this + Engine::Displacement.C_CSPlayer.m_flSpawnTime );
}

float& C_CSPlayer::m_flHealthShotBoostExpirationTime( ) {
	return *( float* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_flHealthShotBoostExpirationTime );
}

bool& C_CSPlayer::m_bHasHelmet( ) {
	return *( bool* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_bHasHelmet );
}

bool& C_CSPlayer::m_bHasHeavyArmor( ) {
	return *( bool* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_bHasHeavyArmor );
}

bool& C_CSPlayer::m_bIsScoped( ) {
	return *( bool* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_bScoped );
}

bool& C_CSPlayer::m_bWaitForNoAttack( ) {
	return *( bool* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_bWaitForNoAttack );
}

bool& C_CSPlayer::m_bIsPlayerGhost( )
{
	return *( bool* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_bIsPlayerGhost );
}

int& C_CSPlayer::m_iMatchStats_Kills( ) {
	return *( int* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_iMatchStats_Kills );
}

int& C_CSPlayer::m_iMatchStats_Deaths( ) {
	return *( int* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_iMatchStats_Deaths );
}

int& C_CSPlayer::m_iMatchStats_HeadShotKills( ) {
	return *( int* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_iMatchStats_HeadShotKills );
}

bool& C_CSPlayer::m_bGunGameImmunity( ) {
	return *( bool* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_bGunGameImmunity );
}

bool& C_CSPlayer::m_bIsDefusing( ) {
	return *( bool* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_bIsDefusing );
}

bool& C_CSPlayer::m_bHasDefuser( ) {
	return *( bool* )( ( uintptr_t )this + Engine::Displacement.DT_CSPlayer.m_bHasDefuser );
}

bool C_CSPlayer::PhysicsRunThink( int unk01 ) {
	static auto impl_PhysicsRunThink = ( bool( __thiscall* )( void*, int ) )Engine::Displacement.Function.m_uImplPhysicsRunThink;
	return impl_PhysicsRunThink( this, unk01 );
}

int C_CSPlayer::SetNextThink( int tick ) {
	static const auto next_think = Memory::Scan( XorStr( "client.dll" ), XorStr( "55 8B EC 56 57 8B F9 8B B7 E8" ) );

	typedef int( __thiscall* fnSetNextThink ) ( C_CSPlayer*, int tick );
	auto ret = ( ( fnSetNextThink )next_think ) ( this, tick );
	return ret;
}

void C_CSPlayer::Think( ) {
	static auto index = *( int* )( Memory::Scan( XorStr( "client.dll" ), XorStr( "FF 90 ? ? ? ? FF 35 ? ? ? ? 8B 4C 24 3C" ) ) + 2 ) / 4; // ref: CPrediction::ProcessMovement  (*(void (__thiscall **)(_DWORD *))(*player + 552))(player);
	//xref CPrediction::ProcessMovement (*(void (__thiscall **)(_DWORD *))(*a2 + 552))(a2);
	using Fn = void( __thiscall* )( void* );
	Memory::VCall<Fn>( this, /*138*/ 137 )( this ); // 139
}

void C_CSPlayer::PreThink( ) {
	static auto index = *( int* )( Memory::Scan( XorStr( "client.dll" ), XorStr( "FF 90 ? ? ? ? 8B 86 ? ? ? ? 83 F8 FF" ) ) + 2 ) / 4;
	//xref CPrediction::ProcessMovement 
	// if ( (unsigned __int8)sub_102FED00(0) )
	// (*(void (__thiscall **)(_DWORD *))(*a2 + 1268))(a2);
	using Fn = void( __thiscall* )( void* );
	Memory::VCall<Fn>( this, /*315*/ 307 )( this ); // 169
}

void C_CSPlayer::PostThink( ) {
	using Fn = void( __thiscall* )( void* );
	Memory::VCall<Fn>( this, /*316*/ 308 )( this );
}

bool C_CSPlayer::is( std::string networkname ) {
	if( !this )
		return false;

	auto clientClass = this->GetClientClass( );
	if( !clientClass )
		return false;

	return Engine::g_PropManager.GetClientID( networkname ) == clientClass->m_ClassID;
}

bool C_CSPlayer::IsTeammate( C_CSPlayer* player, bool nigger ) {
	if( !player || !this )
		return false;

	C_CSPlayer* ent = player;

	if( nigger )
		if( player->EntIndex( ) == m_pEngine->GetLocalPlayer( ) ) {
			if( player->IsDead( ) ) {
				if( player->m_hObserverTarget( ).IsValid( ) ) {
					const auto spec = ( C_CSPlayer* )player->m_hObserverTarget( ).Get( );
					if( spec ) {
						if( player->m_iObserverMode( ) == 4 || player->m_iObserverMode( ) == 5 )
							ent = spec;
					}
				}
			}
		}

	return this->m_iTeamNum( ) == ent->m_iTeamNum( );
}

bool C_CSPlayer::CanShoot( bool bSkipRevolver ) {
	bool local = EntIndex( ) == m_pEngine->GetLocalPlayer( );

	auto weapon = ( C_WeaponCSBaseGun* )( this->m_hActiveWeapon( ).Get( ) );
	if( !weapon )
		return false;

	auto weapon_data = weapon->GetCSWeaponData( );
	if( !weapon_data.IsValid( ) )
		return false;

	if( this->m_fFlags( ) & 0x40 )
		return false;

	if( m_pGameRules.Xor( ) ) {
		if( m_pGameRules->m_bFreezePeriod( ) )
			return false;
	}

	if( this->m_bWaitForNoAttack( ) )
		return false;

	if( *( int* )( uintptr_t( this ) + Engine::Displacement.DT_CSPlayer.m_iPlayerState ) )
		return false;

	if( this->m_bIsDefusing( ) )
		return false;

	if( this->IsReloading( ) )
		return false;

	if( weapon_data->m_iWeaponType >= WEAPONTYPE_PISTOL && weapon_data->m_iWeaponType <= WEAPONTYPE_MACHINEGUN && weapon->m_iClip1( ) < 1 )
		return false;

	float curtime = TICKS_TO_TIME( this->m_nTickBase( ) );
	if( curtime < m_flNextAttack( ) )
		return false;

	if( ( weapon->m_iItemDefinitionIndex( ) == WEAPON_GLOCK || weapon->m_iItemDefinitionIndex( ) == WEAPON_FAMAS ) && weapon->m_iBurstShotsRemaining( ) > 0 ) {
		if( curtime >= weapon->m_fNextBurstShot( ) )
			return true;
	}

	if( curtime < weapon->m_flNextPrimaryAttack( ) )
		return false;

	if( weapon->m_iItemDefinitionIndex( ) != WEAPON_REVOLVER )
		return true;

	if( bSkipRevolver )
		return true;

	if( *( int* )( uintptr_t( weapon ) + Engine::Displacement.DT_BaseAnimating.m_nSequence ) != 5 )
		return false;

	return curtime >= weapon->m_flPostponeFireReadyTime( );
}

bool C_CSPlayer::IsReloading( ) {
	if( !this )
		return false;

	auto animLayer = this->m_AnimOverlay( ).Element( 1 );
	if( !animLayer.m_pOwner )
		return false;

	return GetSequenceActivity( animLayer.m_nSequence ) == 967 && animLayer.m_flWeight != 0.f;
}

int C_CSPlayer::LookupBone( const char* szName )
{
	static auto lookupBone = Memory::CallableFromRelative( ( DWORD )Memory::Scan( XorStr( "client.dll" ), XorStr( "E8 ? ? ? ? 89 44 24 5C" ) ) );
	return reinterpret_cast< int( __thiscall* )( void*, const char* ) >( lookupBone )( this, szName );
}

Vector C_CSPlayer::GetBonePosition( int iBone )
{
	if( iBone < 0 || iBone > 128 )
		return Vector::Zero;

	matrix3x4_t bonetoworld = m_CachedBoneData( ).Base( )[ iBone ];

	return Vector( bonetoworld[ 0 ][ 3 ], bonetoworld[ 1 ][ 3 ], bonetoworld[ 2 ][ 3 ] );
}

void C_CSPlayer::ModifyEyePosition( Vector* eyepos )
{
	auto x = Encrypted_t( this );

	if( !x.IsValid( ) )
		return;

	int bone = x->LookupBone( XorStr( "head_0" ) );

	if( bone != -1 )
	{
		Vector vecHeadPos = x->GetBonePosition( bone );

		vecHeadPos.z += 1.7f;

		auto eyepos_z = eyepos->z;
		if( eyepos->z > vecHeadPos.z )
		{
			auto v7 = 0.0f;
			auto v8 = ( fabs( eyepos_z - vecHeadPos.z ) - 4.0f ) * 0.16666667f;
			if( v8 >= 0.0f )
				v7 = fminf( v8, 1.0f );
			eyepos->z = ( ( vecHeadPos.z - eyepos->z ) * ( ( ( v7 * v7 ) * 3.0f ) - ( ( ( v7 * v7 ) * 2.0f ) * v7 ) ) ) + eyepos->z;
		}
	}
}

Vector C_CSPlayer::GetEyePosition( bool ignore_modify_eye_pos ) {
	//Vector vecOrigin = this->GetAbsOrigin( );
	Vector eyePos = m_vecOrigin( ) + m_vecViewOffset( );

	if( !ignore_modify_eye_pos ) {
		if( auto animstate = m_PlayerAnimState( ); animstate != nullptr && ( animstate->m_bLanding || animstate->m_flAnimDuckAmount != 0.f || !( m_fFlags( ) & FL_ONGROUND ) ) )
			ModifyEyePosition( &eyePos );
	}

	return eyePos;
}

QAngle C_CSPlayer::DecayAimPunchAngle( QAngle& vPunchAngleVelocity ) {
	QAngle vPunchAngleExpDecay;
	float flExpDecay, flLinDecay, flLinDecayUsable;

	float flPunchAngleExpDecayLength;

	static auto weapon_recoil_decay2_exp = m_pCvar->FindVar( XorStr( "weapon_recoil_decay2_exp" ) );
	static auto weapon_recoil_decay2_lin = m_pCvar->FindVar( XorStr( "weapon_recoil_decay2_lin" ) );
	static auto weapon_recoil_vel_decay = m_pCvar->FindVar( XorStr( "weapon_recoil_vel_decay" ) );

	QAngle& angPunchAngle = m_aimPunchAngle( );

	flExpDecay = weapon_recoil_decay2_exp->GetFloat( );
	flLinDecay = weapon_recoil_decay2_lin->GetFloat( );

	vPunchAngleExpDecay = angPunchAngle;
	flPunchAngleExpDecayLength = Vector( vPunchAngleExpDecay.x, vPunchAngleExpDecay.y, vPunchAngleExpDecay.z ).Length( );

	flLinDecayUsable = flLinDecay * m_pGlobalVars->interval_per_tick;

	// inlined HybridDecay
	if( ( flPunchAngleExpDecayLength > 0.0 ) && ( flPunchAngleExpDecayLength > flLinDecayUsable ) ) {
		float flMultiplier;

		flMultiplier = 1.0f - ( flLinDecayUsable / flPunchAngleExpDecayLength );
		vPunchAngleExpDecay *= flMultiplier;
	}
	else {
		vPunchAngleExpDecay = { 0, 0, 0 };
	}

	angPunchAngle = vPunchAngleExpDecay + ( vPunchAngleExpDecay * m_pGlobalVars->interval_per_tick * 0.5f );
	vPunchAngleVelocity *= expf( m_pGlobalVars->interval_per_tick * -weapon_recoil_vel_decay->GetFloat( ) );
	angPunchAngle += ( vPunchAngleExpDecay * m_pGlobalVars->interval_per_tick * 0.5f );

	return angPunchAngle;
}

uint8_t* C_CSPlayer::GetServerEdict( ) {

	if( !GetModuleHandleA( "server.dll" ) )
		return nullptr;

	static uintptr_t pServerGlobals = **( uintptr_t** )( Memory::Scan( XorStr( "server.dll" ), XorStr( "8B 15 ? ? ? ? 33 C9 83 7A 18 01" ) ) + 0x2 );
	int iMaxClients = *( int* )( ( uintptr_t )pServerGlobals + 0x18 );
	if( EntIndex( ) > 0 && iMaxClients >= 1 ) {
		if( EntIndex( ) <= iMaxClients ) {
			int v10 = EntIndex( ) * 16;
			uintptr_t v11 = *( uintptr_t* )( pServerGlobals + 96 );
			if( v11 ) {
				if( !( ( *( uintptr_t* )( v11 + v10 ) >> 1 ) & 1 ) ) {
					uintptr_t v12 = *( uintptr_t* )( v10 + v11 + 12 );
					if( v12 ) {
						uint8_t* pReturn = nullptr;

						// abusing asm is not good
						__asm
						{
							pushad
							mov ecx, v12
							mov eax, dword ptr[ ecx ]
							call dword ptr[ eax + 0x14 ]
							mov pReturn, eax
							popad
						}

						return pReturn;
					}
				}
			}
		}
	}
	return nullptr;
}

int& C_CSPlayer::m_nLastNonSkippedFrame( ) {
	return *( int* )( uintptr_t( this ) + 0xA68 );
}

int& C_CSPlayer::m_iMostRecentModelBoneCounter( ) {
	return *( int* )( uintptr_t( this ) + ( Engine::Displacement.DT_BaseEntity.m_nForceBone + 0x4 ) );
}

int& C_CSPlayer::m_nCustomBlendingRuleMask( ) {
	return *( int* )( uintptr_t( this ) + 0xA24 );
}

int& C_CSPlayer::m_nAnimLODflags( ) {
	return *( int* )( uintptr_t( this ) + 0xA28 );
}

int& C_CSPlayer::m_ClientEntEffects( ) {
	return *( int* )( uintptr_t( this ) + 0x68 );
}

int& C_CSPlayer::m_pIK( ) {
	return *( int* )( uintptr_t( this ) + ( Engine::Displacement.DT_BaseEntity.m_nForceBone - 0x1C ) );
}

int& C_CSPlayer::m_vphysicsCollisionState( ) {
	static auto vphysics_state = Engine::g_PropManager.GetOffset( XorStr( "DT_BasePlayer" ), XorStr( "m_vphysicsCollisionState" ) );
	return *( int* )( uintptr_t( this ) + vphysics_state );
}

bool& C_CSPlayer::m_bIsWalking( ) {
	static auto m_bIsWalking = Engine::g_PropManager.GetOffset( XorStr( "DT_CSPlayer" ), XorStr( "m_bIsWalking" ) );
	return *( bool* )( uintptr_t( this ) + m_bIsWalking );
}

BYTE& C_CSPlayer::m_ubEFNoInterpParity( )
{
	static auto m_ubEFNoInterpParity = Engine::g_PropManager.GetOffset( XorStr( "DT_BasePlayer" ), XorStr( "m_ubEFNoInterpParity" ) );
	return *( BYTE* )( uintptr_t( this ) + m_ubEFNoInterpParity );
}

BYTE& C_CSPlayer::m_ubEFNoInterpParityOld( )
{
	static auto m_ubEFNoInterpParity = Engine::g_PropManager.GetOffset( XorStr( "DT_BasePlayer" ), XorStr( "m_ubEFNoInterpParity" ) );
	return *( BYTE* )( uintptr_t( this ) + m_ubEFNoInterpParity + 1 );
}

bool C_CSPlayer::IsSane( bool bDormant ) {
	if( !this )
		return false;

	if( this->IsDormant( ) && bDormant )
		return false;

	if( this->IsDead( ) )
		return false;

	if( !this->IsPlayer( ) )
		return false;

	return true;
}

bool C_CSPlayer::SetupBones( matrix3x4_t* pBoneToWorld, int nMaxBones, int boneMask, float currentTime ) {
	if( !this )
		return nullptr;

	auto renderable = GetClientRenderable( );
	if( !renderable )
		return nullptr;

	const auto m_pIk = m_pIK( );
	const auto client_ent_flags = m_ClientEntEffects( );
	const auto effects = m_fEffects( );
	const auto animlod = m_nAnimLODflags( );
	const auto jigglebones = m_bIsJiggleBonesEnabled( );

	const auto framecount = m_pGlobalVars->framecount;
	const auto curtime_backup = m_pGlobalVars->curtime;
	const auto realtime_backup = m_pGlobalVars->realtime;
	const auto frametime = m_pGlobalVars->frametime;
	const auto absoluteframetime = m_pGlobalVars->absoluteframetime;
	const auto tickcount = m_pGlobalVars->tickcount;
	const auto interpolation_amount = m_pGlobalVars->interpolation_amount;

	float time = m_flSimulationTime( );
	int ticks = TIME_TO_TICKS( time );

	m_pGlobalVars->curtime = time;
	m_pGlobalVars->realtime = time;
	m_pGlobalVars->frametime = m_pGlobalVars->interval_per_tick;
	m_pGlobalVars->absoluteframetime = m_pGlobalVars->interval_per_tick;
	m_pGlobalVars->tickcount = ticks;
	m_pGlobalVars->interpolation_amount = 0.f;

	m_pIK( ) = 0;
	m_BoneAccessor( ).m_ReadableBones = 0;
	m_iMostRecentModelBoneCounter( ) = 0;
	m_flLastBoneSetupTime( ) = ( float )0xFF7FFFFF; // FLT_MAX * -1.f
	m_nLastNonSkippedFrame( ) = 0;
	m_nAnimLODflags( ) &= ~2u; //flag: ANIMLODFLAG_OUTSIDEVIEWFRUSTUM
	m_nCustomBlendingRuleMask( ) = -1;
	m_ClientEntEffects( ) |= 2u; //flag: NO_IK
	m_fEffects( ) |= 8u;
	m_bIsJiggleBonesEnabled( ) = false;

	m_pGlobalVars->framecount = -999;

	auto boneSnapshot1 = ( float* )( uintptr_t( this ) + 0x39F0 + 4 );
	auto boneSnapshot2 = ( float* )( uintptr_t( this ) + 0x6E40 + 4 );

	auto bk_snapshot1 = *boneSnapshot1;
	auto bk_snapshot2 = *boneSnapshot2;

	*boneSnapshot1 = 0.0f;
	*boneSnapshot2 = 0.0f;

	g_AnimationSystem.m_bInBoneSetup[ EntIndex( ) ] = true;
	auto bone_result = renderable->SetupBones( pBoneToWorld, nMaxBones, boneMask, currentTime );
	g_AnimationSystem.m_bInBoneSetup[ EntIndex( ) ] = false;

	*boneSnapshot1 = bk_snapshot1;
	*boneSnapshot2 = bk_snapshot2;

	m_pGlobalVars->framecount = framecount;

	m_pIK( ) = m_pIk;
	m_fEffects( ) = effects;
	m_ClientEntEffects( ) = client_ent_flags;
	m_nAnimLODflags( ) = animlod;

	m_pGlobalVars->curtime = curtime_backup;
	m_pGlobalVars->realtime = realtime_backup;
	m_pGlobalVars->frametime = frametime;
	m_pGlobalVars->absoluteframetime = absoluteframetime;
	m_pGlobalVars->tickcount = tickcount;
	m_pGlobalVars->interpolation_amount = interpolation_amount;
}