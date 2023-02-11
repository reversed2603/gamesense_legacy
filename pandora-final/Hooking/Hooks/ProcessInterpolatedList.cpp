#include "../../source.hpp"
#include "../Hooked.hpp"
#include "../../SDK/Displacement.hpp"


void __cdecl Hooked::ProcessInterpolatedList( ) {
	g_Vars.globals.szLastHookCalled = XorStr( "38" );

	**( bool** )( Engine::Displacement.Data.s_bAllowExtrapolation ) = false;
	oProcessInterpolatedList( );
}
