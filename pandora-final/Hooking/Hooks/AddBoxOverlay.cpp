#include "../../source.hpp"
#include "../Hooked.hpp"
#include "../../SDK/Displacement.hpp"

void __fastcall Hooked::AddBoxOverlay( void* ecx, void* edx, const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, int r, int g, int b, int a, float duration ) {
	g_Vars.globals.szLastHookCalled = XorStr( "41" );

	static uintptr_t FireBullets = 0;
	if( !FireBullets ) {
		FireBullets = Memory::Scan( XorStr( "client.dll" ), XorStr( "3B 3D ?? ?? ?? ?? 75 4C" ) );
	}

	if( !g_Vars.esp.server_impacts || uintptr_t( _ReturnAddress( ) ) != FireBullets )
		return oAddBoxOverlay( ecx, origin, mins, max, orientation, r, g, b, a, duration );

	return oAddBoxOverlay( ecx, origin, mins, max, orientation,
		255, 0, 0, 125,
		duration );
}