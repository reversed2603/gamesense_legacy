#include "../../source.hpp"
#include "../Hooked.hpp"
#include "../../SDK/Displacement.hpp"
#include "../../SDK/Classes/Player.hpp"

void __fastcall Hooked::ModifyEyePosition( C_CSPlayer* ecx, void* edx, Vector* eye_position ) {
	g_Vars.globals.szLastHookCalled = XorStr( "40" );
	auto pLocal = C_CSPlayer::GetLocalPlayer( );
	if( !pLocal ) {
		oModifyEyePosition( ecx, eye_position );
		return;
	}

	if( g_Vars.globals.m_bInCreateMove )
		oModifyEyePosition( ecx, eye_position );

	return;
}