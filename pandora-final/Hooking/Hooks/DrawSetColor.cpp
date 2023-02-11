#include "../../source.hpp"
#include "../Hooked.hpp"
#include "../../SDK/Displacement.hpp"


void __fastcall Hooked::DrawSetColor( ISurface* thisptr, void* edx, int r, int g, int b, int a ) {
	g_Vars.globals.szLastHookCalled = XorStr( "45" );

	if( !g_Vars.esp.remove_scope ) {
		return oDrawSetColor( thisptr, r, g, b, a );
	}

	static auto return_to_scope_arc = Memory::Scan( XorStr( "client.dll" ), XorStr( "6A 00 FF 50 3C 8B 0D ? ? ? ? FF B7" ) ) + 5;
	static auto return_to_scope_lens = Memory::Scan( XorStr( "client.dll" ), XorStr( "FF 50 3C 8B 4C 24 20" ) ) + 3;

	static auto return_to_scope_lines_clear = Memory::Scan( XorStr( "client.dll" ), XorStr( "0F 82 ? ? ? ? FF 50 3C" ) ) + 0x9;
	static auto return_to_scope_lines_blurry = Memory::Scan( XorStr( "client.dll" ), XorStr( "FF B7 ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B 4C 24 24" ) ) - 0x6;

	if( !return_to_scope_lines_clear || !return_to_scope_lines_blurry || !return_to_scope_arc || !return_to_scope_lens ) {
		return oDrawSetColor( thisptr, r, g, b, a );
	}

	if( uintptr_t( _ReturnAddress( ) ) == return_to_scope_arc ||
		uintptr_t( _ReturnAddress( ) ) == return_to_scope_lens ||
		uintptr_t( _ReturnAddress( ) ) == return_to_scope_lines_clear ||
		uintptr_t( _ReturnAddress( ) ) == return_to_scope_lines_blurry ) {
		// We don't want this to draw, so we set the alpha to 0
		return oDrawSetColor( thisptr, r, g, b, 0 );
	}

	oDrawSetColor( thisptr, r, g, b, a );
}