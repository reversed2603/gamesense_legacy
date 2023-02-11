#include "../hooked.hpp"
#include "../../Features/Visuals/BulletTracers.hpp"

void __fastcall Hooked::BeginFrame( void* ECX, void* EDX, float ft ) {
   g_Vars.globals.szLastHookCalled = XorStr( "1" );

   g_BeamEffects.Draw( );

   oBeginFrame( ECX, ft );
}
