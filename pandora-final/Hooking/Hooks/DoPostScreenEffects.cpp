#include "../Hooked.hpp"
#include "../../Features/Visuals/Chams.hpp"
#include "../../Features/Visuals/Glow.hpp"
#include "../../SDK/variables.hpp"

namespace Hooked
{
	int __stdcall DoPostScreenEffects( int a1 ) {
		g_Vars.globals.szLastHookCalled = XorStr( "5" );

		g_Glow.OnPostScreenEffects( );

		g_Vars.globals.m_bInPostScreenEffects = true;
		auto result = oDoPostScreenEffects( m_pClientMode.Xor( ), a1 );
		g_Vars.globals.m_bInPostScreenEffects = false;

		return result;
	}
}
