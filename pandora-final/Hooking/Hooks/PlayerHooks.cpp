#include "../Hooked.hpp"
#include "../../SDK/sdk.hpp"
#include "../../SDK/Displacement.hpp"
#include "../../SDK/Classes/Player.hpp"
#include "../../SDK/Classes/weapon.hpp"
#include "../../Features/Miscellaneous/SkinChanger.hpp"
#include "../../Features/Rage/LagCompensation.hpp"
#include <intrin.h>

#include "../../Utils/Threading/threading.h"

ClientClass* CCSPlayerClass;
CreateClientClassFn oCreateCCSPlayer;
std::map< int, Hooked::PlayerHook > Hooked::player_hooks;

namespace Hooked
{
	void __fastcall PostDataUpdate( uintptr_t ecx, void* edx, int updateType );

	void __fastcall DoExtraBonesProccesing( C_CSPlayer* ecx, void* edx, CStudioHdr* hdr, Vector* pos, Quaternion* rotations, matrix3x4_t* transforma, void* bone_list, void* ik_context ) {
		g_Vars.globals.szLastHookCalled = XorStr( "22" );
		//printf( "debp called\n" );

		auto& hook = player_hooks[ ecx->m_entIndex ];

		using Fn = void( __thiscall* )( C_CSPlayer*, CStudioHdr*, Vector*, Quaternion*, matrix3x4_t*, void*, void* );
		auto _do_extra_bone_processing = hook.clientHook.VCall< Fn >( 197 );

		if( ecx->m_fEffects( ) & 8 )
			return;

		auto animState = ecx->m_PlayerAnimState( );

		if( !animState )
			_do_extra_bone_processing( ecx, hdr, pos, rotations, transforma, bone_list, ik_context );

		const auto backup_tickcount = *reinterpret_cast< int32_t* >( animState + 8 );
		*reinterpret_cast< int32_t* >( animState + 8 ) = 0;
		_do_extra_bone_processing( ecx, hdr, pos, rotations, transforma, bone_list, ik_context );
		*reinterpret_cast< int32_t* >( animState + 8 ) = backup_tickcount;
	}

	Hooked::PlayerHook::~PlayerHook( ) {
		//clientHook.Destroy( );
		renderableHook.Destroy( );
		networkableHook.Destroy( );
	}

	void Hooked::PlayerHook::SetHooks( ) {
		//networkableHook.Hook( EntityRelease, 1 );
		networkableHook.Hook( PostDataUpdate, 7 );
		renderableHook.Hook( SetupBones, 13 );
		//clientHook.Hook( DoExtraBonesProccesing, 197 );
	}


	bool __fastcall SetupBones( uintptr_t ecx, void* edx, matrix3x4_t* matrix, int bone_count, int bone_mask, float time ) {
		if( !ecx )
			return false;

		auto player = reinterpret_cast< C_CSPlayer* >( ecx - 0x4 );
		if( !player )
			return false;

		auto hook = &player_hooks[ player->m_entIndex ];
		if( !hook )
			return true;

		C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer( );
		if( !pLocal )
			return false;

		using SetupBonesFn = bool( __thiscall* )( uintptr_t, matrix3x4_t*, int, int, float );
		auto oSetupBones = hook->renderableHook.VCall< SetupBonesFn >( 13 );

		// since we fix bonesetup manually
		if( g_AnimationSystem.m_bInBoneSetup[ player->EntIndex( ) ] ) {
			return oSetupBones( ecx, matrix, bone_count, bone_mask, time );
		}

		if( matrix ) {
			if( bone_count > 0 )
				std::memcpy( matrix, player->m_CachedBoneData( ).Base( ), sizeof( matrix3x4_t ) * bone_count );
		}

		return true;
	}

	void __fastcall EntityRelease( uintptr_t ecx, void* edx ) {
		g_Vars.globals.szLastHookCalled = XorStr( "24" );
		auto entity = reinterpret_cast< C_BaseEntity* >( ecx - 0x8 );
		if( !entity || !ecx )
			return;

		auto hook = &player_hooks[ entity->m_entIndex ];

		if( !hook )
			return;

		using Fn = void( __thiscall* )( uintptr_t );
		auto orig = hook->networkableHook.VCall< Fn >( 1 );

		player_hooks.erase( entity->m_entIndex );

		orig( ecx );
	}

	void __fastcall PostDataUpdate( uintptr_t ecx, void* edx, int updateType ) {
		g_Vars.globals.szLastHookCalled = XorStr( "25" );
		auto entity = reinterpret_cast< C_CSPlayer* >( ecx - 0x8 );

		if( !entity || !ecx )
			return;

		auto hook = &player_hooks[ entity->m_entIndex ];

		if( !hook )
			return;

		using Fn = void( __thiscall* )( uintptr_t, int );
		auto orig = hook->networkableHook.VCall< Fn >( 7 );

		auto local = C_CSPlayer::GetLocalPlayer( );

		if( local ) {
			if( local->m_entIndex == entity->m_entIndex )
				g_SkinChanger.OnNetworkUpdate( true );

			orig( ecx, updateType );

			if( local->m_entIndex == entity->m_entIndex )
				g_SkinChanger.OnNetworkUpdate( false );
		}
		else {
			orig( ecx, updateType );
		}
	}

	IClientNetworkable* CreateCCSPlayer( int entnum, int serialNum ) {
		g_Vars.globals.szLastHookCalled = XorStr( "26" );
		auto entity = ( IClientNetworkable* )oCreateCCSPlayer( entnum, serialNum );

		auto& new_hook = player_hooks[ entnum ];
		//new_hook.clientHook.Create( ( void* )( ( uintptr_t )entity - 0x8 ) );
		new_hook.renderableHook.Create( ( void* )( ( uintptr_t )entity - 0x4 ) );
		new_hook.networkableHook.Create( entity );
		new_hook.SetHooks( );
		return entity;
	}
}
