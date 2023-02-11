#pragma once
#include "../Hooked.hpp"
#include "../../source.hpp"
#include "../../Features/Rage/EnginePrediction.hpp"

#include "../../SDK/Classes/Player.hpp"

void __fastcall Hooked::PacketStart( void* ECX, void* EDX, int incoming_sequence, int outgoing_acknowledged ) {
	g_Vars.globals.szLastHookCalled = XorStr( "19" );
	g_Prediction.PacketStart( incoming_sequence, outgoing_acknowledged );
}

void __fastcall Hooked::PacketEnd( void* ecx, void* ) {
	g_Vars.globals.szLastHookCalled = XorStr( "20" );
	g_Prediction.PacketCorrection( reinterpret_cast< uintptr_t >( ecx ) );
	oPacketEnd( ecx );
}